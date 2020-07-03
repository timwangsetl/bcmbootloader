/*
 * $Copyright Open Broadcom Corporation$
 */

#include <config.h>

#include <common.h>
#include <pci.h>
#include <asm/io.h>
#include <post.h>
#include "asm/types.h"
#include "pcie_iproc.h"
#include "asm/arch/socregs.h"

extern void iproc_pcie_iomux(int op);
static unsigned long  PORT_IN_USE = 0;
#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
static uint32_t swap_u32(uint32_t i) {
    uint8_t c1, c2, c3, c4;

    c1 = i & 255;
    c2 = (i >> 8) & 255;
    c3 = (i >> 16) & 255;
    c4 = (i >> 24) & 255;

    return ((uint32_t)c1 << 24) + ((uint32_t)c2 << 16) + ((uint32_t)c3 << 8) + c4;
}

static void pcie_iol_w(uint32_t val, volatile uint32_t *addr)
{
    *addr = swap_u32(val);
}

static uint32_t pcie_iol_r(volatile uint32_t *addr)
{
    return(swap_u32(*addr));
}

#else

static void pcie_iol_w(uint32_t val, volatile uint32_t *addr)
{
    *addr = val;
}

static uint32_t pcie_iol_r(volatile uint32_t *addr)
{
    return(*addr);
}
#endif



#if defined(CONFIG_CYGNUS)
struct soc_pcie_port soc_pcie_ports[2] = {
    {
    0x18012000,
    0x20000000,
    0
    },
    {
    0x18013000, 
    0x40000000,
    0
    }
};

#else
static 
struct soc_pcie_port soc_pcie_ports[3] = {
    {
    0x18012000,
    0x08000000,
    0
    },
    {
    0x18013000, 
    0x40000000,
    0
    },
    {
    0x18014000,
    0x48000000,
    0
    }
};
#endif

struct pci_controller pci_hoses[3];

static int conf_trace = 0;
#ifndef CONFIG_CYGNUS
static void pci_dump_standard_conf(struct soc_pcie_port * port);
#endif
static void pci_dump_extended_conf(struct soc_pcie_port * port);

void pci_bus0_read_config_dword (struct soc_pcie_port *pcie_port, pci_dev_t dev, int where, unsigned long *val)
{
    if(conf_trace) printf("pci_bus0_read_config_word: dev: %08x <B%d, D%d, F%d>, where: %08x\n", 
                dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where);

    __raw_writel( where & 0xffc, (u32)(pcie_port->reg_base) + SOC_PCIE_EXT_CFG_ADDR ); // BDF = 0

    * val = __raw_readl((u32)(pcie_port->reg_base) + SOC_PCIE_EXT_CFG_DATA);

    if(conf_trace) printf("Return : dev: %08x, where: %08x, val: %04lx\n\n", dev, where, *val);
}

void pci_bus0_read_config_word (struct soc_pcie_port *pcie_port, pci_dev_t dev, int where, unsigned short *val)
{
    unsigned int tmp;
    if(conf_trace) printf("pci_bus0_read_config_word: dev: %08x <B%d, D%d, F%d>, where: %08x\n", 
                dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where);

    __raw_writel( where & 0xffc, (u32)(pcie_port->reg_base) + SOC_PCIE_EXT_CFG_ADDR ); // BDF = 0

    tmp = __raw_readl((u32)(pcie_port->reg_base) + SOC_PCIE_EXT_CFG_DATA);
    *val = (tmp >> (8 * (where & 3))) & 0xffff;
    if(conf_trace) printf("Return : dev: %08x, where: %08x, val: %04x\n\n", dev, where, *val);
}

void pci_bus0_read_config_byte (struct soc_pcie_port *pcie_port, pci_dev_t dev, int where, unsigned char *val)
{
    unsigned int tmp;
    if(conf_trace) printf("pci_bus0_read_config_byte: dev: %08x <B%d, D%d, F%d>, where: %08x\n", 
                dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where);

    __raw_writel( where & 0xffc, (u32)(pcie_port->reg_base) + SOC_PCIE_EXT_CFG_ADDR ); // BDF = 0

    tmp = __raw_readl((u32)(pcie_port->reg_base) + SOC_PCIE_EXT_CFG_DATA);
    *val = (tmp >> (8 * (where & 3))) & 0xff;
    if(conf_trace) printf("Return : dev: %08x, where: %08x, val: %02x\n\n", dev, where, *val);
}

void pci_bus0_write_config_dword (struct soc_pcie_port *pcie_port, pci_dev_t dev, int where, unsigned long val)
{
    if(conf_trace) printf("pci_bus0_write_config_dword: dev: %08x <B%d, D%d, F%d>, where: %08x, val: %04lx\n", 
                dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where, val);

    __raw_writel( where & 0xffc, (u32)(pcie_port->reg_base) + SOC_PCIE_EXT_CFG_ADDR ); // BDF = 0
    __raw_writel( val, (u32)(pcie_port->reg_base) + SOC_PCIE_EXT_CFG_DATA);

    if(conf_trace) printf("pci_bus0_write_config_dword write done\n");
}

void pci_bus0_write_config_word (struct soc_pcie_port *pcie_port, pci_dev_t dev, int where, unsigned short val)
{
    unsigned int tmp;

    if(conf_trace) printf("pci_bus0_write_config_word: dev: %08x <B%d, D%d, F%d>, where: %08x, val: %04x\n", 
                dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where, val);

    __raw_writel( where & 0xffc, (u32)(pcie_port->reg_base) + SOC_PCIE_EXT_CFG_ADDR ); // BDF = 0

    tmp = __raw_readl((u32)(pcie_port->reg_base) + SOC_PCIE_EXT_CFG_DATA);
 
    if(conf_trace) printf("pci_bus0_write_config_word read first: dev: %08x <B%d, D%d, F%d>, where: %08x, tmp_val: %04x\n", 
                dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where, tmp);
    tmp &= ~(0xffff << (8 * (where & 3)) );
    tmp |= (val << (8 * (where & 3)) );

    if(conf_trace) printf("pci_bus0_write_config_word write back: dev: %08x <B%d, D%d, F%d>, where: %08x, tmp_val: %04x\n", 
                dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where, tmp);
    __raw_writel( where & 0xffc, (u32)(pcie_port->reg_base) + SOC_PCIE_EXT_CFG_ADDR );
    __raw_writel(tmp, (u32)(pcie_port->reg_base) + SOC_PCIE_EXT_CFG_DATA);

    if(conf_trace) printf("pci_bus0_write_config_word write done\n");
}

void pci_bus0_write_config_byte (struct soc_pcie_port *pcie_port, pci_dev_t dev, int where, unsigned char val)
{
    unsigned int tmp;

    if(conf_trace) printf("pci_bus0_write_config_byte: dev: %08x <B%d, D%d, F%d>, where: %08x, val: %02x\n", 
                dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), where, val);

    __raw_writel( where & 0xffc, (u32)(pcie_port->reg_base) + SOC_PCIE_EXT_CFG_ADDR ); // BDF = 0

    tmp = __raw_readl((u32)(pcie_port->reg_base) + SOC_PCIE_EXT_CFG_DATA);

    tmp &= ~(0xff << (8 * (where & 3)) );
    tmp |= (val << (8 * (where & 3)) );

    __raw_writel( where & 0xffc, (u32)(pcie_port->reg_base) + SOC_PCIE_EXT_CFG_ADDR );
    __raw_writel(tmp, (u32)(pcie_port->reg_base) + SOC_PCIE_EXT_CFG_DATA);
}

static int __pci_bus0_find_next_cap_ttl(struct soc_pcie_port *pcie_port, unsigned int devfn,
        u8 pos, int cap, int *ttl)
{
    u8 id;

    while ((*ttl)--) {
        pci_bus0_read_config_byte(pcie_port, devfn, pos, &pos);
        if (pos < 0x40)
            break;
        pos &= ~3;
        pci_bus0_read_config_byte(pcie_port,devfn, pos + PCI_CAP_LIST_ID,
                &id);
        if (id == 0xff)
            break;
        if (id == cap)
            return pos;
        pos += PCI_CAP_LIST_NEXT;
    }
    return 0;
}
#define PCI_FIND_CAP_TTL        48

 static int __pci_bus0_find_next_cap(struct soc_pcie_port *pcie_port, unsigned int devfn,
                                u8 pos, int cap)
 {
         int ttl = PCI_FIND_CAP_TTL;

         return __pci_bus0_find_next_cap_ttl(pcie_port, devfn, pos, cap, &ttl);
 }


static int __pci_bus0_find_cap_start(struct soc_pcie_port *pcie_port,unsigned int devfn, u8 hdr_type)
 {
         u16 status;

         pci_bus0_read_config_word(pcie_port, devfn, PCI_STATUS, &status);
         if (!(status & PCI_STATUS_CAP_LIST))
                 return 0;

         switch (hdr_type) {
         case PCI_HEADER_TYPE_NORMAL:
         case PCI_HEADER_TYPE_BRIDGE:
                 return PCI_CAPABILITY_LIST;
         case PCI_HEADER_TYPE_CARDBUS:
                 return PCI_CB_CAPABILITY_LIST;
         default:
                 return 0;
         }

         return 0;
}

static int pci_bus0_find_capability(struct soc_pcie_port *pcie_port,unsigned int devfn, int cap)
{
         int pos;
         u8 hdr_type;

         pci_bus0_read_config_byte(pcie_port,devfn, PCI_HEADER_TYPE, &hdr_type);

         pos = __pci_bus0_find_cap_start(pcie_port, devfn, hdr_type & 0x7f);
         if (pos)
             pos = __pci_bus0_find_next_cap(pcie_port, devfn, pos, cap);

         return pos;
 }
//Link status register definitions



//3:0
//RO
//Link Speed. The negotiated Link speed.
//   0001b = 2.5 Gb/s
//All other encodings are reserved.

//9:4
//RO
//Negotiated Link Width. The negotiated Link width.
//   000001b = x1
//   000010b = x2
//   000100b = x4
//   001000b = x8
//   001100b = x12
//   010000b = x16
//   100000b = x32
//   All other encodings are reserved.

//10
//RO
//Training Error. 1 = indicates that a Link training error occurred. Reserved on Endpoint devices and Switch upstream ports.
//Cleared by hardware upon successful training of the Link to the L0 Link state.

//11
//RO
//Link Training. When set to one, indicates that Link training is in progress (Physical Layer LTSSM is
//in the Configuration or Recovery state) or that the Retrain Link bit was set to one but Link training has not yet begun.
//Hardware clears this bit once Link training is complete.
// This bit is not applicable and reserved on Endpoint devices and the Upstream Ports of Switches.

//12
//HWInit
//Slot Clock Configuration. This bit indicates that the component uses the same physical reference clock
//that the platform provides on the connector. If the device uses an independent clock irrespective of the
//presence of a reference on the connector, this bit must be clear.

int soc_pcie_check_link(int port)
{
    u32 devfn = 0;
    u16 pos, tmp16;
    u8 nlw, tmp8;
    struct soc_pcie_port * pcie_port = &soc_pcie_ports[port];
    //struct pci_controller *hose = &pci_hoses[port];
    pcie_port->linkError = 0;

    /* See if the port is in EP mode, indicated by header type 00 */
    pci_bus0_read_config_byte(pcie_port, devfn, PCI_HEADER_TYPE, &tmp8);
    if( tmp8 != PCI_HEADER_TYPE_BRIDGE ) {
//        printf("PCIe port %d in End-Point mode - ignored\n", port);
        pcie_port->linkError = 1;
        return -1;
    }
    else
        printf("PCIe port %d in RC mode\n" , port);

    /* NS PAX only changes NLW field when card is present */
    pos = pci_bus0_find_capability(pcie_port,devfn, PCI_CAP_ID_EXP);

    printf("\n pos is %d\n", pos);

    pci_bus0_read_config_word(pcie_port,devfn, pos + PCI_EXP_LNKSTA, &tmp16);

    printf("==>PCIE: LINKSTA reg %#x val %#x\n",
        pos+PCI_EXP_LNKSTA, tmp16 );



    nlw = (tmp16 & PCI_EXP_LNKSTA_NLW) >> PCI_EXP_LNKSTA_NLW_SHIFT ;
    if ( nlw == 0 )
    {
        pcie_port->linkError = 1;
    }
    //port->link = tmp16 & PCI_EXP_LNKSTA_DLLLA ;

    //if( nlw != 0 ) port->link = 1;
#if 0
    for( ; pos < 0x100; pos += 2 )
        {
            pci_bus0_read_config_word(pcie_port, devfn, pos , &tmp16);
            if( tmp16 ) printf("reg[%#x]=%#x, ", pos , tmp16 );
        }
    //printf("PCIE link=%d\n", port->link );
#endif

    return( (nlw)? 0: -1);
}

#if (defined(CONFIG_CYGNUS))
#define ChipcommonB_MII_Management_Control       ChipcommonG_MII_Management_Control    
#define ChipcommonB_MII_Management_Command_Data  ChipcommonG_MII_Management_Command_Data

#define ChipcommonB_MII_Management_Control__BSY        ChipcommonG_MII_Management_Control__BSY
#define ChipcommonB_MII_Management_Control__EXT        ChipcommonG_MII_Management_Control__EXT
#define ChipcommonB_MII_Management_Command_Data__SB_R  ChipcommonG_MII_Management_Command_Data__SB_R
#define ChipcommonB_MII_Management_Command_Data__OP_R  ChipcommonG_MII_Management_Command_Data__OP_R
#define ChipcommonB_MII_Management_Command_Data__PA_R  ChipcommonG_MII_Management_Command_Data__PA_R
#define ChipcommonB_MII_Management_Command_Data__TA_R  ChipcommonG_MII_Management_Command_Data__TA_R
#define ChipcommonB_MII_Management_Command_Data__RA_R  ChipcommonG_MII_Management_Command_Data__RA_R
#define ChipcommonB_MII_Management_Control__PRE        ChipcommonG_MII_Management_Control__PRE

#include <asm/io.h>
#include <asm/arch/bcmutils.h>

#define reg32_read readl
#define reg32_write(a, v)  writel(v, a)

static void
phy_write(uint phyaddr, uint reg, uint16_t v)
{
	uint32_t tmp;
	volatile uint32_t *phy_ctrl = (uint32_t *)ChipcommonB_MII_Management_Control;
	volatile uint32_t *phy_data = (uint32_t *)ChipcommonB_MII_Management_Command_Data;


	/* wait until Mii mgt interface not busy */
	SPINWAIT((*phy_ctrl & (1<<ChipcommonB_MII_Management_Control__BSY)), 1000);
	tmp = reg32_read(phy_ctrl);
	if (tmp & (1<<ChipcommonB_MII_Management_Control__BSY)) {
		printf("%d: %s: busy\n", 0, __FUNCTION__);
	}

	/* set preamble and MDCDIV */
	tmp = 	(1<<ChipcommonB_MII_Management_Control__PRE) |			/* PRE */
			0x1a;		/* MDCDIV */

        tmp &= ~(1 << ChipcommonB_MII_Management_Control__EXT);
	
	reg32_write(phy_ctrl, tmp);
	//printf("%s wrt phy_ctrl: 0x%x\n", __FUNCTION__, tmp);

	/* wait for it to complete */
	SPINWAIT((*phy_ctrl & (1<<ChipcommonB_MII_Management_Control__BSY)), 1000);
	tmp = reg32_read(phy_ctrl);
	if (tmp & (1<<ChipcommonB_MII_Management_Control__BSY)) {
		printf("et%d: %s ChipcommonB_MII_Management_Control did not complete\n", 0, __FUNCTION__);
	}

	/* issue the write */
	/* set start bit, write op, phy addr, phy reg & data */
	tmp = 	(1 << ChipcommonB_MII_Management_Command_Data__SB_R) |			/* SB */
			(1 << ChipcommonB_MII_Management_Command_Data__OP_R) |			/* OP - wrt */
			(phyaddr << ChipcommonB_MII_Management_Command_Data__PA_R) |	/* PA */
			(reg << ChipcommonB_MII_Management_Command_Data__RA_R) |		/* RA */
			(2 << ChipcommonB_MII_Management_Command_Data__TA_R) |			/* TA */
			v;																/* Data */
	//printf("%s wrt phy_data: 0x%x phyaddr(0x%x) reg(0x%x) val(0x%x)\n",
	//		__FUNCTION__, tmp, phyaddr, reg, v);
	reg32_write(phy_data, tmp);

	/* wait for it to complete */
	SPINWAIT((*phy_ctrl & (1<<ChipcommonB_MII_Management_Control__BSY)), 1000);
	tmp = reg32_read(phy_ctrl);
	if (tmp & (1<<ChipcommonB_MII_Management_Control__BSY)) {
		printf("et%d: %s ChipcommonB_MII_Management_Command_Data did not complete\n", 0, __FUNCTION__);
	}
}
static uint16_t
phy_read(uint phyaddr, uint reg)
{
	uint32_t tmp;
	volatile uint32_t *phy_ctrl = (uint32_t *)ChipcommonB_MII_Management_Control;
	volatile uint32_t *phy_data = (uint32_t *)ChipcommonB_MII_Management_Command_Data;

	

	/* wait until Mii mgt interface not busy */
	SPINWAIT((*phy_ctrl & (1<<ChipcommonB_MII_Management_Control__BSY)), 1000);
	tmp = reg32_read(phy_ctrl);
	if (tmp & (1<<ChipcommonB_MII_Management_Control__BSY)) {
		printf("et%d: %s: busy\n", 0, __FUNCTION__);
	}

	/* set preamble and MDCDIV */
	tmp = 	(1<<ChipcommonB_MII_Management_Control__PRE) |			/* PRE */
			0x1a;													/* MDCDIV */

	tmp &= ~(1 << ChipcommonB_MII_Management_Control__EXT);
	
	reg32_write(phy_ctrl, tmp);
	//printf("%s wrt phy_ctrl: 0x%x\n", __FUNCTION__, tmp);

	/* wait for it to complete */
	SPINWAIT((*phy_ctrl & (1<<ChipcommonB_MII_Management_Control__BSY)), 1000);
	tmp = reg32_read(phy_ctrl);
	if (tmp & (1<<ChipcommonB_MII_Management_Control__BSY)) {
		printf("et%d: %s ChipcommonB_MII_Management_Control did not complete\n", 0, __FUNCTION__);
	}

	/* issue the read */
	/* set start bit, write op, phy addr, phy reg & data */
	tmp = 	(1 << ChipcommonB_MII_Management_Command_Data__SB_R) |			/* SB */
			(2 << ChipcommonB_MII_Management_Command_Data__OP_R) |			/* OP - rd*/
			(phyaddr << ChipcommonB_MII_Management_Command_Data__PA_R) |	/* PA */
			(reg << ChipcommonB_MII_Management_Command_Data__RA_R) |		/* RA */
			(2 << ChipcommonB_MII_Management_Command_Data__TA_R);			/* TA */
	//printf("%s wrt phy_data:0x%x phyaddr(0x%x) reg(0x%x)\n",
	//		__FUNCTION__, tmp, phyaddr, reg);
	reg32_write(phy_data, tmp);

	/* wait for it to complete */
	SPINWAIT((*phy_ctrl & (1<<ChipcommonB_MII_Management_Control__BSY)), 1000);
	tmp = reg32_read(phy_ctrl);
	if (tmp & (1<<ChipcommonB_MII_Management_Control__BSY)) {
		printf("et%d: %s ChipcommonB_MII_Management_Command_Data did not complete\n", 0, __FUNCTION__);
	}

	/* read data */
	tmp = reg32_read(phy_data);
	//printf("%s rd phyaddr(0x%x) reg(0x%x) data: 0x%x\n", __FUNCTION__, phyaddr, reg, tmp);

	return (tmp & 0xffff);
}

int  pcie0_wr_ind_reg(uint32_t addr, uint32_t data)
{
    reg32_write(PAXB_0_CONFIG_IND_ADDR, addr);
    data = reg32_read(PAXB_0_CONFIG_IND_DATA);
    printf("PCIE indirect reg write: Addr = 0x%08X Data = 0x%08X\n", addr, data);
    return 0;
}


int  pcie0_rd_ind_reg(uint32_t addr, uint32_t *data)
{
    reg32_write(PAXB_0_CONFIG_IND_ADDR, addr);
    *data = reg32_read(PAXB_0_CONFIG_IND_DATA);
    printf("PCIE indirect reg read:  Addr = 0x%08X Data = 0x%08X\n", addr, *data);
    return 0;
}

int  pcie1_wr_ind_reg(uint32_t addr, uint32_t data)
{
    reg32_write(PAXB_1_CONFIG_IND_ADDR, addr);
    data = reg32_read(PAXB_1_CONFIG_IND_DATA);
    printf("PCIE indirect reg write: Addr = 0x%08X Data = 0x%08X\n", addr, data);
    return 0;
}


int  pcie1_rd_ind_reg(uint32_t addr, uint32_t *data)
{
    reg32_write(PAXB_1_CONFIG_IND_ADDR, addr);
    *data = reg32_read(PAXB_1_CONFIG_IND_DATA);
    printf("PCIE indirect reg read:  Addr = 0x%08X Data = 0x%08X\n", addr, *data);
    return 0;
}



static void soc_pcie_hw_pre(int port)
{
    u32 data, phy_addr, reg_addr, i;
    u32 *addr;
    /*PCIe Serdes default register setting was wrong,
	Have to do MDIO write to serdes register to enable refclk output */
    if (port == 0)
    {
    data = reg32_read(PAXB_0_CLK_CONTROL);
    printf("PAXB_0_CLK_CONTROL = 0x%08X\n", data);
    //data &= ~(1 << PAXB_0_CLK_CONTROL__EP_PERST_SOURCE_SELECT);
    data |= (1 << PAXB_0_CLK_CONTROL__RC_PCIE_RST_OUTPUT);
    reg32_write(PAXB_0_CLK_CONTROL, data);
    data = reg32_read(PAXB_0_CLK_CONTROL);
    printf("PAXB_0_CLK_CONTROL = 0x%08X\n", data);
    
    addr = (uint32_t *) (PAXB_0_CLK_CONTROL + 0x400);
    for(i=0; i<2; i++) {
        printf("Addr = 0x%08X Data = 0x%08X\n", (u32)addr, *addr);
        addr++;
    }

    for(i=0; i<2; i++) {
        pcie0_rd_ind_reg(i*4, &data);
        printf("Local PCIE Header Reg = 0x%08X Data = 0x%08X\n", i*4, data);
    }
   
    //----------------------------------------------------

    phy_addr = 5;
    reg_addr = 0x2103;
    data = phy_read(phy_addr, reg_addr);
    data = 0x2B1C;
    phy_write(phy_addr, reg_addr, data);
    data = phy_read(phy_addr, reg_addr);
    reg_addr = 0x1300;
    data = phy_read(phy_addr, reg_addr);
    
    //----------------------------------------------------
    } else {
       data = reg32_read(PAXB_1_CLK_CONTROL);
    printf("PAXB_1_CLK_CONTROL = 0x%08X\n", data);
    //data &= ~(1 << PAXB_0_CLK_CONTROL__EP_PERST_SOURCE_SELECT);
    data |= (1 << PAXB_1_CLK_CONTROL__RC_PCIE_RST_OUTPUT);
    reg32_write(PAXB_1_CLK_CONTROL, data);
    data = reg32_read(PAXB_1_CLK_CONTROL);
    printf("PAXB_1_CLK_CONTROL = 0x%08X\n", data);
    
    addr = (uint32_t *) (PAXB_1_CLK_CONTROL + 0x400);
    for(i=0; i<2; i++) {
        printf("Addr = 0x%08X Data = 0x%08X\n", (u32)addr, *addr);
        addr++;
    }

    for(i=0; i<2; i++) {
        pcie1_rd_ind_reg(i*4, &data);
        printf("Local PCIE Header Reg = 0x%08X Data = 0x%08X\n", i*4, data);
    }
   
    //----------------------------------------------------

    phy_addr = 6;
    reg_addr = 0x2103;
    data = phy_read(phy_addr, reg_addr);
    data = 0x2B1C;
    phy_write(phy_addr, reg_addr, data);
    data = phy_read(phy_addr, reg_addr);
    reg_addr = 0x1300;
    data = phy_read(phy_addr, reg_addr);
    
    //----------------------------------------------------

    }

}

#endif
/*
 * Initializte the PCIe controller
 */
static void  soc_pcie_hw_init(struct soc_pcie_port * port)
{
    /* Turn-on Root-Complex (RC) mode, from reset defailt of EP */

    /* The mode is set by straps, can be overwritten via DMU
       register <cru_straps_control> bit 5, "1" means RC
     */

    /* Send a downstream reset */
	pcie_iol_w( 0x3, port->reg_base + SOC_PCIE_CONTROL);
    udelay(2500);
	pcie_iol_w( 0x1, port->reg_base + SOC_PCIE_CONTROL);
    udelay(100000);

//	printf("\n soc_pcie_hw_init : port->reg_base = 0x%x , its value = 0x%x \n", port->reg_base, pcie_iol_r(port->reg_base + SOC_PCIE_CONTROL));
    /* TBD: take care of PM, check we're on */
}


int iproc_pcie_rd_conf_dword(struct pci_controller *hose, pci_dev_t dev, int offset, u32 * value)
{
    struct soc_pcie_port *pcie_port = (struct soc_pcie_port *)(hose->priv_data);
    
    if(conf_trace) printf("Pcie_rd_conf_dword: dev: %08x <B%d, D%d, F%d>, where: %08x\n", 
                dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), offset); 
	pcie_iol_w(PCIE_CONF_BUS( PCI_BUS(dev) ) |
                    PCIE_CONF_DEV( PCI_DEV(dev) ) |
                    PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
                    PCIE_CONF_REG(offset),
                    (u32)(pcie_port->reg_base) + PCIE_CONF_ADDR_OFF);

	*value = pcie_iol_r((u32)(pcie_port->reg_base) + PCIE_CONF_DATA_OFF);

    if(conf_trace) printf("Return : dev: %08x, where: %08x, val: %08x\n\n", dev, offset, *value);

    return PCIBIOS_SUCCESSFUL;
}

int iproc_pcie_rd_conf_word(struct pci_controller *hose, pci_dev_t dev, int offset, u16 * value)
{
    volatile unsigned int tmp;
    struct soc_pcie_port *pcie_port = (struct soc_pcie_port *)(hose->priv_data);
    
    if(conf_trace) printf("Pcie_rd_conf_word: dev: %08x <B%d, D%d, F%d>, where: %08x\n", 
                dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), offset); 
	pcie_iol_w(PCIE_CONF_BUS( PCI_BUS(dev) ) |
                    PCIE_CONF_DEV( PCI_DEV(dev) ) |
                    PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
                    PCIE_CONF_REG(offset),
                    (u32)(pcie_port->reg_base) + PCIE_CONF_ADDR_OFF);

	tmp = pcie_iol_r((u32)(pcie_port->reg_base) + PCIE_CONF_DATA_OFF);
    *value = (tmp >> (8 * (offset & 3))) & 0xffff;

    if(conf_trace) printf("Return : dev: %08x, where: %08x, val: %04x\n\n", dev, offset, *value);

    return PCIBIOS_SUCCESSFUL;
}

int iproc_pcie_rd_conf_byte(struct pci_controller *hose, pci_dev_t dev, int offset, u8 * value)
{
    volatile unsigned int tmp;
    struct soc_pcie_port *pcie_port = (struct soc_pcie_port *)(hose->priv_data);
    
    if(conf_trace) printf("Pcie_rd_conf_byte: dev: %08x <B%d, D%d, F%d>, where: %08x\n", 
                dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), offset); 
	pcie_iol_w(PCIE_CONF_BUS( PCI_BUS(dev) ) |
                    PCIE_CONF_DEV( PCI_DEV(dev) ) |
                    PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
                    PCIE_CONF_REG(offset),
                    (u32)(pcie_port->reg_base) + PCIE_CONF_ADDR_OFF);

	tmp = pcie_iol_r((u32)(pcie_port->reg_base) + PCIE_CONF_DATA_OFF);
    *value = (tmp >> (8 * (offset & 3))) & 0xff;

    if(conf_trace) printf("Return : dev: %08x, where: %08x, val: %02x\n\n", dev, offset, *value);

    return PCIBIOS_SUCCESSFUL;
}

int iproc_pcie_wr_conf_dword(struct pci_controller *hose, pci_dev_t dev, int offset, u32 value)
{
    int ret = PCIBIOS_SUCCESSFUL;
    struct soc_pcie_port *pcie_port = (struct soc_pcie_port *)(hose->priv_data);
    
    if(conf_trace) printf("Pcie_wr_conf_dword: dev: %08x <B%d, D%d, F%d>, where: %08x, val: %08x\n", 
                dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), offset, value); 
	pcie_iol_w(PCIE_CONF_BUS( PCI_BUS(dev) ) |
                    PCIE_CONF_DEV( PCI_DEV(dev) ) |
                    PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
                    PCIE_CONF_REG(offset),
                    (u32)(pcie_port->reg_base)  + PCIE_CONF_ADDR_OFF);

	pcie_iol_w(value, (u32)(pcie_port->reg_base) + PCIE_CONF_DATA_OFF);
    return ret;
}

int iproc_pcie_wr_conf_word(struct pci_controller *hose, pci_dev_t dev, int offset, u16 value)
{
    int ret = PCIBIOS_SUCCESSFUL;
    struct soc_pcie_port *pcie_port = (struct soc_pcie_port *)(hose->priv_data);
    
    if(conf_trace) printf("Pcie_wr_conf_word: dev: %08x <B%d, D%d, F%d>, where: %08x, val: %04x\n", 
                dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), offset, value); 
	pcie_iol_w(PCIE_CONF_BUS( PCI_BUS(dev) ) |
                    PCIE_CONF_DEV( PCI_DEV(dev) ) |
                    PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
                    PCIE_CONF_REG(offset),
                    (u32)(pcie_port->reg_base) + PCIE_CONF_ADDR_OFF);

    __raw_writew(value, (u32)(pcie_port->reg_base) + PCIE_CONF_DATA_OFF + (offset & 3) );
    return ret;
}

int iproc_pcie_wr_conf_byte(struct pci_controller *hose, pci_dev_t dev, int offset, u8 value)
{
    int ret = PCIBIOS_SUCCESSFUL;
    struct soc_pcie_port *pcie_port = (struct soc_pcie_port *)(hose->priv_data);
    
    if(conf_trace) printf("Pcie_wr_conf_byte: dev: %08x <B%d, D%d, F%d>, where: %08x, val: %02x\n", 
                dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), offset, value); 
	pcie_iol_w(PCIE_CONF_BUS( PCI_BUS(dev) ) |
                    PCIE_CONF_DEV( PCI_DEV(dev) ) |
                    PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
                    PCIE_CONF_REG(offset),
                    (u32)(pcie_port->reg_base) + PCIE_CONF_ADDR_OFF);

    __raw_writeb(value, (u32)(pcie_port->reg_base) + PCIE_CONF_DATA_OFF + (offset & 3) );
    return ret;
}


int pci_read_config_dword (pci_dev_t dev, int where, unsigned int *val)
{
    return iproc_pcie_rd_conf_dword(&pci_hoses[PORT_IN_USE], dev, where, val);
}

int pci_read_config_word (pci_dev_t dev, int where, unsigned short *val)
{
    return iproc_pcie_rd_conf_word(&pci_hoses[PORT_IN_USE], dev, where, val);
}

int pci_read_config_byte (pci_dev_t dev, int where, unsigned char *val)
{
    return iproc_pcie_rd_conf_byte(&pci_hoses[PORT_IN_USE], dev, where, val);
}

int pci_write_config_dword (pci_dev_t dev, int where, unsigned int val)
{
    return iproc_pcie_wr_conf_dword(&pci_hoses[PORT_IN_USE], dev, where, val);
}

int pci_write_config_word (pci_dev_t dev, int where, unsigned short val)
{
    return iproc_pcie_wr_conf_word(&pci_hoses[PORT_IN_USE], dev, where, val);
}

int pci_write_config_byte (pci_dev_t dev, int where, unsigned char val)
{
    return iproc_pcie_wr_conf_byte(&pci_hoses[PORT_IN_USE], dev, where, val);
}

/*
 * Setup the address translation
 */
static void soc_pcie_map_init(struct soc_pcie_port * port)
{

    /* pass thru' address translation */
#if 0
    unsigned size, i ;
    u32 addr;

    /*
     * NOTE:
     * All PCI-to-CPU address mapping are 1:1 for simplicity
     */

    /* Outbound address translation setup */
    size = SZ_128M;
    addr = OUT_PCI_ADDR;

    for(i=0 ; i < 3; i++)
        {
        const unsigned win_size = SZ_64M;
        /* 64-bit LE regs, write low word, high is 0 at reset */
		pcie_iol_w( addr,	port->reg_base + SOC_PCIE_SYS_OMAP(i));
		pcie_iol_w( addr|0x1,	port->reg_base + SOC_PCIE_SYS_OARR(i));
        addr += win_size;
        if( size >= win_size )
            size -= win_size;
        if( size == 0 )
            break;
        }

    /* 
     * Inbound address translation setup
     * Northstar only maps up to 128 MiB inbound, DRAM could be up to 1 GiB.
     *
     * For now allow access to entire DRAM, assuming it is less than 128MiB,
     * otherwise DMA bouncing mechanism may be required.
     * Also consider DMA mask to limit DMA physical address
     */

    size = SZ_64M;
    addr = IN_DDR_ADDR;

    size >>= 20;    /* In MB */
    size &= 0xff;   /* Size is an 8-bit field */

    /* 64-bit LE regs, write low word, high is 0 at reset */
	pcie_iol_w(addr | size | 0x1,
        port->reg_base + SOC_PCIE_SYS_IMAP1(0));
	pcie_iol_w(addr | 0x1,
        port->reg_base + SOC_PCIE_SYS_IARR(1));
#endif
}


/*
 * Setup PCIE Host bridge
 */
static void soc_pcie_bridge_init(struct soc_pcie_port * port)
{
        u32 devfn = 0;
        u8 tmp8;
        u16 tmp16;
        u32 mem_size = SZ_128M;
        
#if defined(CONFIG_CYGNUS)
        mem_size = SZ_512M;
#endif
        /* Fake <bus> object */
        pci_bus0_read_config_byte(port, devfn, PCI_PRIMARY_BUS, &tmp8);
        pci_bus0_read_config_byte(port, devfn, PCI_SECONDARY_BUS, &tmp8);
        pci_bus0_read_config_byte(port, devfn, PCI_SUBORDINATE_BUS, &tmp8);

        pci_bus0_write_config_byte(port, devfn, PCI_PRIMARY_BUS, 0);
        pci_bus0_write_config_byte(port, devfn, PCI_SECONDARY_BUS, 1);
        pci_bus0_write_config_byte(port, devfn, PCI_SUBORDINATE_BUS, 4);

        pci_bus0_read_config_byte(port, devfn, PCI_PRIMARY_BUS, &tmp8);
        pci_bus0_read_config_byte(port, devfn, PCI_SECONDARY_BUS, &tmp8);
        pci_bus0_read_config_byte(port, devfn, PCI_SUBORDINATE_BUS, &tmp8);

        printf("membase %#x memlimit %#x\n",
               ((u32)port->out_pci_addr), ((u32)port->out_pci_addr) + mem_size);

        pci_bus0_read_config_word(port, devfn, PCI_CLASS_DEVICE, &tmp16);
        pci_bus0_read_config_word(port, devfn, PCI_MEMORY_BASE, &tmp16);
        pci_bus0_read_config_word(port, devfn, PCI_MEMORY_LIMIT, &tmp16);

        pci_bus0_write_config_word(port, devfn, PCI_MEMORY_BASE, 
               ((u32)port->out_pci_addr) >> 16 );
        pci_bus0_write_config_word(port, devfn, PCI_MEMORY_LIMIT, 
               (((u32)port->out_pci_addr) + mem_size) >> 16 );

    /* Force class to that of a Bridge */
        pci_bus0_write_config_word(port, devfn, PCI_CLASS_DEVICE,
        PCI_CLASS_BRIDGE_PCI);

        pci_bus0_read_config_word(port, devfn, PCI_CLASS_DEVICE, &tmp16);
        pci_bus0_read_config_word(port, devfn, PCI_MEMORY_BASE, &tmp16);
        pci_bus0_read_config_word(port, devfn, PCI_MEMORY_LIMIT, &tmp16);
    
}

/* ****************************************************
 * this function is only applicable to type 1 command
 ******************************************************/
int pcie_diag_wr_conf_dword(struct pci_controller *hose, pci_dev_t dev, int offset, u32 value)
{
    int ret = PCIBIOS_SUCCESSFUL;
    struct soc_pcie_port *pcie_port = (struct soc_pcie_port *)(hose->priv_data);

    if(conf_trace) printf("Pcie_wr_conf_dword: dev: %08x <B%d, D%d, F%d>, where: %08x, val: %08x\n", 
                dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), offset, value); 
		pcie_iol_w(PCIE_CONF_BUS( PCI_BUS(dev) ) |
                    PCIE_CONF_DEV( PCI_DEV(dev) ) |
                    PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
                    PCIE_CONF_REG(offset) | 0x01,
                    (u32)(pcie_port->reg_base) + PCIE_CONF_ADDR_OFF);

		pcie_iol_w(value, (u32)(pcie_port->reg_base) + PCIE_CONF_DATA_OFF);
    return ret;
}

/* ****************************************************
 * this function is only applicable to type 1 command
 ******************************************************/
int pcie_diag_rd_conf_dword(struct pci_controller *hose, pci_dev_t dev, int offset, u32 * value)
{
    struct soc_pcie_port *pcie_port = (struct soc_pcie_port *)(hose->priv_data);
    
    if(conf_trace) printf("Pcie_rd_conf_dword: dev: %08x <B%d, D%d, F%d>, where: %08x\n", 
                dev, PCI_BUS(dev), PCI_DEV(dev), PCI_FUNC(dev), offset); 
	pcie_iol_w(PCIE_CONF_BUS( PCI_BUS(dev) ) |
                    PCIE_CONF_DEV( PCI_DEV(dev) ) |
                    PCIE_CONF_FUNC( PCI_FUNC(dev) ) |
                    PCIE_CONF_REG(offset)|0x01,
                    (u32)(pcie_port->reg_base) + PCIE_CONF_ADDR_OFF);

	*value = pcie_iol_r((u32)(pcie_port->reg_base) + PCIE_CONF_DATA_OFF);

    if(conf_trace) printf("Return : dev: %08x, where: %08x, val: %08x\n\n", dev, offset, *value);

    return PCIBIOS_SUCCESSFUL;
}

#if (defined(CONFIG_NS_PLUS))

static void pcieSwitchInit( pci_dev_t busdevfn)
{

#define   SZ_48M    (0x100000 * 48)




    u32     dRead = 0;
    u32     bm = 0;
    struct pci_controller  *hose = &pci_hoses[0];
    unsigned int base_temp, limit_temp;
    u32 u32Readout;

    u32 busno = PCI_BUS(busdevfn);
    conf_trace = 0;


    //printf("\n Enters pcieSwitchInit: BDF  0x%x \n\n",busdevfn);


    if(conf_trace) printf("==>pcieSwitchInit: dev: %08x <B%d, D%d, F%d>",
            busdevfn, PCI_BUS(busdevfn), PCI_DEV(busdevfn), PCI_FUNC(busdevfn));




    pcie_diag_rd_conf_dword(hose, busdevfn, 0x100,/* 4,*/ &dRead);


    if(conf_trace) printf("PCIE: Doing PLX switch Init...Test Read = %08x\n",(unsigned int)dRead);

    //Debug control register.
    pcie_diag_rd_conf_dword(hose, busdevfn, 0x1dc, /*4,*/ &dRead);

    dRead &= ~(1<<22);

    pcie_diag_wr_conf_dword/*soc_pci_write_config*/(hose, busdevfn, 0x1dc, /*4,*/ dRead);

/***/
    //Set GPIO enables.
    pcie_diag_rd_conf_dword(hose, busdevfn, 0x62c, /*4,*/ &dRead);

    if(conf_trace) printf("PCIE: Doing PLX switch Init...GPIO Read = %08x\n",(unsigned int)dRead);

    dRead &= ~((1<<0)|(1<<1)|(1<<3));
    dRead |= ((1<<4)|(1<<5)|(1<<7));

    pcie_diag_wr_conf_dword/*soc_pci_write_config*/(hose, busdevfn, 0x62c, /*4,*/ dRead);

    mdelay(50);
    dRead |= ((1<<0)|(1<<1));

    pcie_diag_wr_conf_dword/*soc_pci_write_config*/(hose, busdevfn, 0x62c, /*4,*/ dRead);

    pcie_diag_rd_conf_dword/*soc_pci_read_config*/(hose, busdevfn, 0x4, /*2,*/ &bm);

#if NS_PCI_DEBUG
    printf("bus master: %08x\n", bm);
#endif
    bm |= 0x06;
    pcie_diag_wr_conf_dword/*soc_pci_write_config*/(hose, busdevfn, 0x4,/*2,*/ bm);
        //pcie_diag_wr_conf_dword(hose, busdevfn, 0x4, bm);
    bm = 0;

#if 1
    pcie_diag_rd_conf_dword/*soc_pci_read_config*/(hose, busdevfn, 0x4, /*2,*/ &bm);
    printf("bus master after: %08x\n", bm);
    bm =0;
#endif
    //Bus 1 if the upstream port of the switch. Bus 2 has the two downstream ports, one on each device number.


    if(PCI_BUS(busdevfn) == 1)
    {
        pcie_diag_wr_conf_dword/*soc_pci_write_config*/(hose, busdevfn, 0x18, /*4,*/ pcieSwitchPrimSecBusNum);
        pcie_diag_rd_conf_dword(hose, busdevfn, 0x18, &u32Readout);
        //printf("==>pcieSwitchInit: 0x18 Readout %x\n", u32Readout);
        //TODO: We need to scan all outgoing windows, to look for a base limit pair for this register.
        //npciConfigOutLong(instance, busNo, deviceNo, 0, 0x20,0xcff0c000);
        /* MEM_BASE, MEM_LIM require 1MB alignment */

        //limit_temp = (0x8000000 + SZ_32M) >> 16; 
        limit_temp = (0x8000000 + SZ_4M - 1) >> 16; 
        base_temp = 0x8000000 >> 16;

        base_temp = base_temp | (limit_temp << 16  );

        pcie_diag_wr_conf_dword(hose, busdevfn, PCI_MEMORY_BASE, base_temp);

        pcie_diag_rd_conf_dword(hose, busdevfn, PCI_MEMORY_BASE, &u32Readout);
        //printf("==>pcieSwitchInit: PCI_MEMORY_BASE Readout %x\n", u32Readout);

    }
    else if(PCI_BUS(busdevfn) == 2)
    {

        //TODO: I need to fix these hard coded addresses.
        if(PCI_DEV(busdevfn) == (0x8 >> 3))
        {
            pcie_diag_wr_conf_dword/*soc_pci_write_config*/(hose, busdevfn, 0x18,/* 4,*/ (0x00000000 | ((busno+1)<<16) | ((busno+1)<<8) | busno));

            pcie_diag_rd_conf_dword(hose, busdevfn, 0x18, &u32Readout);

            //limit_temp = (0x8000000 + SZ_48M + SZ_32M) >> 16;
            //base_temp = ( 0x8000000 + SZ_48M) >> 16;
            limit_temp = (0x8000000 + SZ_2M - 1) >> 16;
            base_temp = ( 0x8000000 ) >> 16;

            base_temp = base_temp | (limit_temp << 16  );

            pcie_diag_wr_conf_dword(hose, busdevfn, PCI_MEMORY_BASE, base_temp);
               
                        pcie_diag_rd_conf_dword(hose, busdevfn, PCI_MEMORY_BASE, &u32Readout);
                //printf("==>pcieSwitchInit: PCI_MEMORY_BASE Readout %x\n", u32Readout);

            iproc_pcie_rd_conf_word/*soc_pci_read_config*/(hose, busdevfn, 0x7A, /*2,*/ &bm);

            if(conf_trace) printf("bm = %04x\n busdevfn = = %08x, bus = %08x\n", bm, busdevfn, PCI_BUS(busdevfn));
        }
        else if(PCI_DEV(busdevfn) == (0x10>>3))
        {
            pcie_diag_wr_conf_dword/*soc_pci_write_config*/(hose, busdevfn, 0x18, /*4,*/ (0x00000000 | ((busno+2)<<16) | ((busno+2)<<8) | busno));
            pcie_diag_rd_conf_dword(hose, busdevfn, 0x18, &u32Readout);
            //printf("==>pcieSwitchInit: 0x18 Readout %x\n", u32Readout);

            //limit_temp = (0x8000000 +  (SZ_48M * 2 ) + SZ_32M)  >> 16;
            //base_temp = ( 0x8000000 + SZ_48M *2) >> 16;
            limit_temp = (0x8000000 +  SZ_4M -1)  >> 16;
            base_temp = ( 0x8000000 + SZ_2M) >> 16;

            base_temp = base_temp | (limit_temp << 16  );

            pcie_diag_wr_conf_dword(hose, busdevfn, PCI_MEMORY_BASE, base_temp);

            pcie_diag_rd_conf_dword(hose, busdevfn, PCI_MEMORY_BASE, &u32Readout);
            //printf("==>pcieSwitchInit: PCI_MEMORY_BASE Readout %x\n", u32Readout);

            if(conf_trace) printf("bm = %04x\n busdevfn = = %08x, bus = %08x\n", bm, busdevfn, PCI_BUS(busdevfn));
        }

    }

}

static void pcieWIFIdeviceInit( pci_dev_t busdevfn)
{

    u32     dRead = 0;
    u32     bm = 0;
    struct pci_controller  *hose = &pci_hoses[0];
    unsigned int base_temp, limit_temp;
    u32 u32Readout;

    u32 busno = PCI_BUS(busdevfn);
    conf_trace = 0;

    if(conf_trace) printf("SATA Device Init: dev: %08x <B%d, D%d, F%d>",
            busdevfn, PCI_BUS(busdevfn), PCI_DEV(busdevfn), PCI_FUNC(busdevfn));

    //pcie_diag_wr_conf_dword(hose, busdevfn, 0x18, pcieSwitchPrimSecBusNum);
    
    //limit_temp = (0x8000000 + SZ_16K) >> 16; 
    base_temp = 0x8000000 ;

    //base_temp = base_temp | (limit_temp << 16  );

    pcie_diag_wr_conf_dword(hose, busdevfn, 0x10, base_temp);
}

static void pcieSATAdeviceInit( pci_dev_t busdevfn)
{

    u32     dRead = 0;
    u32     bm = 0;
    struct pci_controller  *hose = &pci_hoses[0];
    unsigned int base_temp, limit_temp;
    u32 u32Readout;

    u32 busno = PCI_BUS(busdevfn);
    conf_trace = 0;

    if(conf_trace) printf("SATA Device Init: dev: %08x <B%d, D%d, F%d>",
            busdevfn, PCI_BUS(busdevfn), PCI_DEV(busdevfn), PCI_FUNC(busdevfn));

    //pcie_diag_wr_conf_dword(hose, busdevfn, 0x18, pcieSwitchPrimSecBusNum);
    
    //limit_temp = (0x8200000 + 0x1FF) >> 16; 


    base_temp = 0xffffffff;

    //base_temp = base_temp | (limit_temp << 16  );

    pcie_diag_wr_conf_dword(hose, busdevfn, 0x20, base_temp);



    base_temp = 0x8200000;

    //base_temp = base_temp | (limit_temp << 16  );

    pcie_diag_wr_conf_dword(hose, busdevfn, 0x20, base_temp);
    base_temp = 0xffffffff;

    //base_temp = base_temp | (limit_temp << 16  );

    pcie_diag_wr_conf_dword(hose, busdevfn, 0x24, base_temp);

    //limit_temp = (0x8600000 + 0xFFFF) >> 16; 
    base_temp = 0x8600000;

    //base_temp = base_temp | (limit_temp << 16  );

    pcie_diag_wr_conf_dword(hose, busdevfn, 0x24, base_temp);
}

int pci_diag_hose_config_device(struct pci_controller *hose,
               pci_dev_t dev,
               unsigned long io,
               pci_addr_t mem,
               unsigned long command)
{
    unsigned int bar_response, old_command;
    pci_addr_t bar_value;
    pci_size_t bar_size;
    unsigned char pin;
    int bar, found_mem64;

    debug ("PCI Config: I/O=0x%lx, Memory=0x%llx, Command=0x%lx\n",
        io, (u64)mem, command);

    pcie_diag_wr_conf_dword (hose, dev, PCI_COMMAND, 0);

    for (bar = PCI_BASE_ADDRESS_0; bar <= PCI_BASE_ADDRESS_5; bar += 4) {
        pcie_diag_wr_conf_dword (hose, dev, bar, 0xffffffff);
        pcie_diag_rd_conf_dword (hose, dev, bar, &bar_response);

        if (!bar_response)
            continue;

        found_mem64 = 0;

        /* Check the BAR type and set our address mask */
        if (bar_response & PCI_BASE_ADDRESS_SPACE) {
            bar_size = ~(bar_response & PCI_BASE_ADDRESS_IO_MASK) + 1;
            /* round up region base address to a multiple of size */
            io = ((io - 1) | (bar_size - 1)) + 1;
            bar_value = io;
            /* compute new region base address */
            io = io + bar_size;
        } else {
            if ((bar_response & PCI_BASE_ADDRESS_MEM_TYPE_MASK) ==
                PCI_BASE_ADDRESS_MEM_TYPE_64) {
                u32 bar_response_upper;
                u64 bar64;
                pcie_diag_wr_conf_dword(hose, dev, bar+4, 0xffffffff);
                pcie_diag_rd_conf_dword(hose, dev, bar+4, &bar_response_upper);

                bar64 = ((u64)bar_response_upper << 32) | bar_response;

                bar_size = ~(bar64 & PCI_BASE_ADDRESS_MEM_MASK) + 1;
                found_mem64 = 1;
            } else {
                bar_size = (u32)(~(bar_response & PCI_BASE_ADDRESS_MEM_MASK) + 1);
            }

            /* round up region base address to multiple of size */
            mem = ((mem - 1) | (bar_size - 1)) + 1;
            bar_value = mem;
            /* compute new region base address */
            mem = mem + bar_size;
        }

        /* Write it out and update our limit */
        pcie_diag_wr_conf_dword (hose, dev, bar, (u32)bar_value);

        if (found_mem64) {
            bar += 4;
#ifdef CONFIG_SYS_PCI_64BIT
            pcie_diag_wr_conf_dword(hose, dev, bar, (u32)(bar_value>>32));
#else
            pcie_diag_wr_conf_dword (hose, dev, bar, 0x00000000);
#endif
        }
    }

    /* Configure Cache Line Size Register */
//  pci_hose_write_config_byte (hose, dev, PCI_CACHE_LINE_SIZE, 0x08);  

    // These two settings seem to hang the system.

    /* Configure Latency Timer */
//  pci_hose_write_config_byte (hose, dev, PCI_LATENCY_TIMER, 0x80);

    /* Disable interrupt line, if device says it wants to use interrupts */
#if 0
    pci_hose_read_config_byte (hose, dev, PCI_INTERRUPT_PIN, &pin);
    if (pin != 0) {
        pci_hose_write_config_byte (hose, dev, PCI_INTERRUPT_LINE, 0xff);
    }
#endif
    pcie_diag_rd_conf_dword (hose, dev, PCI_COMMAND, &old_command);
    pcie_diag_wr_conf_dword (hose, dev, PCI_COMMAND,
                     (old_command & 0xffff0000) | command);

    return 0;
}
#endif

#if defined(CONFIG_CYGNUS)
int  pci_iproc_port_mode_ep(void)
{
	struct soc_pcie_port *pcie_port = &soc_pcie_ports[0];
	int status;
	status =__raw_readl((u32)(pcie_port->reg_base)+PAXB_0_STRAP_STATUS_BASE);
	if ((status & (1 << PAXB_0_STRAP_STATUS__STRAP_IPROC_PCIE_USER_RC_MODE)) == 0)
	{
	    return 1;
	}
	else
	{
		return 0;
	}
}
void pci_iproc_init_board_ep(int port)
{
    struct soc_pcie_port *pcie_port = &soc_pcie_ports[port];
 	//to-do
    soc_pcie_hw_pre(port);    
    soc_pcie_hw_init(pcie_port);
}
 
#endif
extern void pci_iproc_set_port(int port);
void pci_iproc_init_board (int port)
{
 
    struct pci_controller * hose = &pci_hoses[port];
    struct soc_pcie_port *pcie_port = &soc_pcie_ports[port];

	pci_iproc_set_port(port);

#if defined(CONFIG_CYGNUS)
    soc_pcie_hw_pre(port);
#endif
    soc_pcie_hw_init(pcie_port);
    
    hose->priv_data =(void *)pcie_port;
    hose->first_busno = 0;
    hose->last_busno = 0;
    hose->current_busno = 0;
    

    pci_set_ops(hose,
            iproc_pcie_rd_conf_byte,
            iproc_pcie_rd_conf_word,
            iproc_pcie_rd_conf_dword,
            iproc_pcie_wr_conf_byte,
            iproc_pcie_wr_conf_word,
            iproc_pcie_wr_conf_dword );

    pci_register_hose(hose);

    udelay(1000);

    soc_pcie_map_init(pcie_port);

    if( soc_pcie_check_link(port) != 0 )
    {
//        printf("\n**************\n port %d is not active!!\n**************\n",port);
        return;
    }
//    printf("\n**************\n port %d is active!!\n**************\n",port);
	
	
    pci_iproc_set_port(port);
    
    soc_pcie_bridge_init(pcie_port);

#if !defined(CONFIG_CYGNUS)
    pci_hose_config_device(hose, PCI_BDF(1,0,0), NULL, (u32)soc_pcie_ports[port].out_pci_addr, 0x146);
    pci_write_config_word(0, 0x3c, 0x1a9);  /* Set IRQ */
#endif
    pci_bus0_write_config_word(pcie_port, 0, 0x3c, 0x1a9);
    /* Set bridge */
    pci_bus0_write_config_byte(pcie_port, 0, 0x1b, 0x00);  // latency timer
    pci_bus0_write_config_word(pcie_port, 0, 0x28, 0x00);  // prefetch_base
    pci_bus0_write_config_word(pcie_port, 0, 0x2a, 0x00);
    pci_bus0_write_config_word(pcie_port, 0, 0x4,  0x146);  // cmd

#if (defined(CONFIG_NS_PLUS))
    if ( (post_get_board_diags_type() == BCM958622HR) && ( 0 == port ) )
    {
        pcieSwitchInit(get_pci_dev("1.0.0"));
        printf("\n Finished PCI switch init 1 \n");
        pcieSwitchInit(get_pci_dev("2.1.0"));
        pcieSwitchInit(get_pci_dev("2.2.0"));

        pcieWIFIdeviceInit("3.0.0");
        pcieSATAdeviceInit("4.0.0");

        pci_diag_hose_config_device(hose, PCI_BDF(4,0,0), NULL, 0x08200000, 0x146);
    }
    else
    {
#endif

#ifdef CONFIG_PCI_SCAN_SHOW
    printf("\nExtended Config\n");
    pci_dump_extended_conf(pcie_port);

#if !defined(CONFIG_CYGNUS)
    printf("\nStandard Config\n");
    pci_dump_standard_conf(pcie_port);
#endif

    hose->last_busno = pci_hose_scan(hose);
    //hose->last_busno = 1;
    printf("\n pci_iproc_init_board : hose->last_busno = 0x%x \n", hose->last_busno);
#endif
#if (defined(CONFIG_NS_PLUS))
    }
#endif

    printf("Done PCI initialization\n");


}

#if (defined(CONFIG_NS_PLUS))

void dumpSATAinfo(int port)
{
    unsigned int tmp32;
    struct pci_controller * hose = &pci_hoses[port];
        printf("SATA pcie device:\n");
    pcie_diag_rd_conf_dword(hose, get_pci_dev("4.0.0"), 0, &tmp32);
    printf("      Vendor ID: %04x\n", tmp32 & 0xFFFF);
    printf("      Device ID: %04x\n", (tmp32 >>16)& 0xFFFF);
    pcie_diag_rd_conf_dword(hose, get_pci_dev("4.0.0"), 8, &tmp32);
    printf("      Class: %04x\n", (tmp32 >> 16));
    printf("      subclass: %04x\n", (tmp32)& 0xFFFF);
    pcie_diag_rd_conf_dword(hose, get_pci_dev("4.0.0"), 0x20, &tmp32);
    printf("      Bar 5: %08x\n", tmp32 );
        pcie_diag_rd_conf_dword(hose, get_pci_dev("4.0.0"), 0x24, &tmp32);
    printf("      Bar 6: %08x\n", tmp32);

}
void dumpPCIEDeviceOnSwitch( int port )
{
    struct pci_controller * hose = &pci_hoses[port];
    unsigned int tmp32;
    unsigned int memBase;

    printf("Device 0:\n");
    pcie_diag_rd_conf_dword(hose, get_pci_dev("1.0.0"), 0, &tmp32);
    printf("      Vendor ID: %04x\n", tmp32 & 0xFFFF);
    printf("      Device ID: %04x\n", (tmp32 >>16)& 0xFFFF);
    pcie_diag_rd_conf_dword(hose, get_pci_dev("1.0.0"), 8, &tmp32);
    printf("      Class: %04x\n", (tmp32 >> 16));
    printf("      subclass: %04x\n", (tmp32)& 0xFFFF);
    pcie_diag_rd_conf_dword(hose, get_pci_dev("1.0.0"), 0x20, &tmp32);
    printf("      Memory Base: %08x\n", tmp32 & 0xFFFF);
    printf("      Memory Limit: %08x\n", (tmp32 >> 16) & 0xFFFF );

    printf("Device 1:\n");
    pcie_diag_rd_conf_dword(hose, get_pci_dev("2.1.0"), 0, &tmp32);
    printf("      Vendor ID: %04x\n", tmp32 & 0xFFFF);
    printf("      Device ID: %04x\n", (tmp32 >>16)& 0xFFFF);
    pcie_diag_rd_conf_dword(hose, get_pci_dev("2.1.0"), 8, &tmp32);
    printf("      Class: %04x\n", (tmp32 >> 16));
    printf("      subclass: %04x\n", (tmp32)& 0xFFFF);
    pcie_diag_rd_conf_dword(hose, get_pci_dev("2.1.0"), 0x20, &tmp32);
    printf("      Memory Base: %08x\n", tmp32 & 0xFFFF);
    printf("      Memory Limit: %08x\n", (tmp32 >> 16) & 0xFFFF );

    printf("Device 2:\n");
    pcie_diag_rd_conf_dword(hose, get_pci_dev("2.2.0"), 0, &tmp32);
    printf("      Vendor ID: %04x\n", tmp32 & 0xFFFF);
    printf("      Device ID: %04x\n", (tmp32 >>16)& 0xFFFF);
    pcie_diag_rd_conf_dword(hose, get_pci_dev("2.2.0"), 8, &tmp32);
    printf("      Class: %04x\n", (tmp32 >> 16));
    printf("      subclass: %04x\n", (tmp32)& 0xFFFF);
    pcie_diag_rd_conf_dword(hose, get_pci_dev("2.2.0"), 0x20, &tmp32);
    printf("      Memory Base: %08x\n", tmp32 & 0xFFFF);
    printf("      Memory Limit: %08x\n", (tmp32 >> 16) & 0xFFFF );

    printf("Device 3:\n");
    pcie_diag_rd_conf_dword(hose, get_pci_dev("3.0.0"), 0, &tmp32);
    printf("      Vendor ID: %04x\n", tmp32 & 0xFFFF);
    printf("      Device ID: %04x\n", (tmp32 >>16)& 0xFFFF);
    pcie_diag_rd_conf_dword(hose, get_pci_dev("3.0.0"), 8, &tmp32);
    printf("      Class: %04x\n", (tmp32 >> 16));
    printf("      subclass: %04x\n", (tmp32)& 0xFFFF);
    pcie_diag_rd_conf_dword(hose, get_pci_dev("3.0.0"), 0x10, &tmp32);
    printf("      Bar 0: %08x\n", tmp32 );
    //printf("      Memory Limit: %08x\n", ((tmp32 >>16)& 0xFFFF)<<16);

    printf("\n Memory Base : ");

    memBase= (tmp32 & 0xFFFF) <<16;

    printf("Device 4:\n");
    pcie_diag_rd_conf_dword(hose, get_pci_dev("4.0.0"), 0, &tmp32);
    printf("      Vendor ID: %04x\n", tmp32 & 0xFFFF);
    printf("      Device ID: %04x\n", (tmp32 >>16)& 0xFFFF);
    pcie_diag_rd_conf_dword(hose, get_pci_dev("4.0.0"), 8, &tmp32);
    printf("      Class: %04x\n", (tmp32 >> 16));
    printf("      subclass: %04x\n", (tmp32)& 0xFFFF);
    pcie_diag_rd_conf_dword(hose, get_pci_dev("4.0.0"), 0x20, &tmp32);
    printf("      Bar 5: %08x\n", tmp32 );
        pcie_diag_rd_conf_dword(hose, get_pci_dev("4.0.0"), 0x24, &tmp32);
    printf("      Bar 6: %08x\n", tmp32);
#if 0
    printf("SATA pcie device:\n");
    tmp32 = 0x08200000;
    for ( i = 0; i < 64; i+=4)
    {
        printf("%08x = 0x%08x = \n", tmp32+i, *((unsigned int*)(tmp32+i)));
    }       
#endif        
}

#endif
#ifndef CONFIG_CYGNUS
static void pci_dump_standard_conf(struct soc_pcie_port * port)
{
    unsigned int i, val, cnt;



    /* Disable trace */
    conf_trace = 0;

    for(i=0; i<64; i++)
    {      
        iproc_pcie_rd_conf_dword(&pci_hoses[PORT_IN_USE], 0, i*4, &val);

        cnt = i % 4;
        if(cnt==0) printf("i=%d <%x> \t 0x%08x \t", i, i, val);
        else if(cnt==3) printf("0x%08x \n", val);
        else printf("0x%08x \t", val);
    }
    printf("\n");

    for(i=0; i<6; i++)
    {      
        iproc_pcie_rd_conf_dword(&pci_hoses[PORT_IN_USE], 0, 0x10+i*4, &val);
        printf(" BAR-%d: 0x%08x\n\n", i, val);
    }
}
#endif
static void pci_dump_extended_conf(struct soc_pcie_port * port)
{
    unsigned int i,cnt;
    unsigned short val16;

    conf_trace = 0;

    for(i=0; i<128; i++)
    {
        pci_bus0_read_config_word(port, 0, i*2, &val16);

        cnt = i % 8;
        if(cnt==0) printf("i=%d <%x> \t 0x%04x  ", i, i, val16);
        else if(cnt==7) printf("0x%04x \n", val16);
        else printf("0x%04x  ", val16);
    }
    printf("\n");
}


int pci_skip_dev(struct pci_controller *hose, pci_dev_t dev)
{
    return 0;
}

#ifdef CONFIG_PCI_SCAN_SHOW
int pci_print_dev(struct pci_controller *hose, pci_dev_t dev)
{
    return 1;
}
#endif /* CONFIG_PCI_SCAN_SHOW */

extern void pci_unregister_hoses(void);

/* set the current working slot*/
void pci_iproc_set_port(int port)
{
    PORT_IN_USE = port;
}

int iproc_pcie_get_link_status(int port) 
{
    return  soc_pcie_ports[port].linkError;
}

u32 pci_iproc_get_pci_addr(int port)
{
    return soc_pcie_ports[port].out_pci_addr;
}

u32 pci_iproc_get_reg_base(int port)
{
    return soc_pcie_ports[port].reg_base;
}

void pci_init(void)
{
#if defined(CONFIG_PCI_BOOTDELAY)
    char *s;
    int i;

    /* wait "pcidelay" ms (if defined)... */
    s = getenv ("pcidelay");
    if (s) {
        int val = simple_strtoul (s, NULL, 10);
        for (i=0; i<val; i++)
            udelay (1000);
    }
#endif /* CONFIG_PCI_BOOTDELAY */
    
    pci_unregister_hoses();
#if defined(CONFIG_CYGNUS)
#ifndef CONFIG_CYGNUS_EMULATION
    iproc_pcie_iomux(1);
#endif
    if (pci_iproc_port_mode_ep()) {
        pci_iproc_init_board_ep(0);
    }  else  {
        pci_iproc_init_board(0);
    }
    pci_iproc_init_board(1);
#else
    pci_iproc_init_board(0);
#endif
}

