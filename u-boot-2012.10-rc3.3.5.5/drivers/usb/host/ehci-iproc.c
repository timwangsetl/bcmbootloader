/*
 * $Copyright Open Broadcom Corporation$
 */

#include <common.h>
#include <pci.h>
#include <usb.h>
#include <asm/io.h>

#include "ehci.h"
#include "ehci-core.h"

#ifdef CONFIG_CYGNUS
#include "asm/arch/socregs.h"
#include "asm/arch/reg_utils.h"
#endif

#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
static uint32_t swap_u32(uint32_t i) {
    uint8_t c1, c2, c3, c4;

    c1 = i & 255;
    c2 = (i >> 8) & 255;
    c3 = (i >> 16) & 255;
    c4 = (i >> 24) & 255;

    return ((uint32_t)c1 << 24) + ((uint32_t)c2 << 16) + ((uint32_t)c3 << 8) + c4;
}

static void ehci_iol_w(uint32_t val, volatile uint32_t *addr)
{
    *addr = swap_u32(val);
}

static uint32_t ehci_iol_r(volatile uint32_t *addr)
{
    return(swap_u32(*addr));
}
#else
static void ehci_iol_w(uint32_t val, volatile uint32_t *addr)
{
    *addr = val;
}

static uint32_t ehci_iol_r(volatile uint32_t *addr)
{
    return(*addr);
}
#endif

#if ((defined(CONFIG_NS_PLUS)) || (defined(CONFIG_HELIX4)) || (defined(CONFIG_KATANA2)))

#define IPROC_USB2_CLK_CONTROL_ENABLE	  (0x1803f180)
#define IPROC_USB2_CLK_CONTROL_PLL		  (0x1803f164)
#define IPROC_STRAP_SKU_VECTOR			  (0x1811A500)
#define IPROC_IDM_USB2_RESET_CONTROL	  (0x18115800)
#define IPROC_EHCI                   	  (0x1802a000)
#define USB2_IDM_IDM_IO_CONTROL_DIRECT    (0x18115408)

#define IPROC_SKU_STRAP_MASK 0xC

#elif defined(CONFIG_CYGNUS)


#define IPROC_IDM_USB2_RESET_CONTROL          (USB2_IDM_IDM_RESET_CONTROL)
#define IPROC_EHCI                   	       USBH_HCCAPBASE
 
#else
#define IPROC_USB2_CLK_CONTROL_ENABLE	  (0x1800C180)
#define IPROC_USB2_CLK_CONTROL_PLL		  (0x1800C164)
#define IPROC_STRAP_SKU_VECTOR			  (0x1810D500)
#define IPROC_IDM_USB2_RESET_CONTROL	  (0x18115800)
#define IPROC_EHCI                   	  (0x18021000)

#define IPROC_SKU_STRAP_MASK 0xC
#endif

#if 0
struct usbh_ctrl_regs {
	u32 mode;
#define MODE_ULPI_TTL (1<<0)
#define MODE_ULPI_PHY (1<<1)
#define MODE_UTMI_TTL (1<<2)
#define MODE_UTMI_PHY (1<<3)
#define MODE_PORT_CFG(port, mode) ((mode) << (4 * port))

	u32 strap_q;
#define STRAP_PWR_STATE_VALID                (1 << 7)    /* ss_power_state_valid */
#define STRAP_SIM_MODE                       (1 << 6)    /* ss_simulation_mode */
#define STRAP_OHCI_CNTSEL_SIM                (1 << 5)    /* ohci_0_cntsel_i_n */
#define STRAP_PWR_STATE_NXT_VALID            (1 << 4)    /* ss_nxt_power_state_valid_i */
#define STRAP_PWR_STATE_NXT_SHIFT            2           /* ss_next_power_state_i */
#define STRAP_PWR_STATE_NXT_MASK             (3 << STRAP_PWR_STATE_NXT_SHIFT)
#define STRAP_PWR_STATE_SHIFT                0           /* ss_power_state_i */
#define STRAP_PWR_STATE_MASK                 (3 << STRAP_PWR_STATE_SHIFT)

	u32 framelen_adj_q;
	u32 framelen_adj_qx[USBH_NUM_PORTS];       
	u32 misc;
#define MISC_RESUME_R23_ENABLE               (1 << 4) /* ss_utmi_backward_enb_i */
#define MISC_RESUME_R23_UTMI_PLUS_DISABLE    (1 << 3) /* ss_resume_utmi_pls_dis_i */
#define MISC_ULPI_BYPASS_ENABLE              (1 << 2) /* ulpi_bypass_en_i */
#define MISC_PORT_PWRDWN_OVERCURRENT         (1 << 1) /* ss_autoppd_on_overcur_en_i */
#define MISC_OHCI_CLK_RESTART                (1 << 0) /* app_start_clk_i */

};

static struct usbh_ctrl_regs  host_ctrl_regs;
#endif


#if defined(CONFIG_HELIX4)
#define IPROC_WRAP_USBPHY_CTRL	0x1803fc34
#endif

#if defined(CONFIG_KATANA2)
#define IPROC_WRAP_USBPHY_CTRL	0x1803fc20
#endif

#if ((defined(CONFIG_HELIX4)) || (defined(CONFIG_KATANA2)))
#define IPROC_IDM_USB2_IO_CONTROL_DIRECT	0x18115408
#define IPROC_XGPLL							0x1803fc2c
#define IPROC_WRAP_MISC_STATUS				0x1803fc28
#define IPROC_USB_PHY_CTRL					IPROC_WRAP_USBPHY_CTRL
#define IPROC_CLK_NDIV_40					0x80
#define IPROC_CLK_NDIV_20					0x8C
#define USB_CLK_NDIV_MASK					0xFE7FFE00
#define USB_CLK_PLL_RESET_MASK				0xFF7FFE00
#define USB_CLK_PHY_RESET_MASK				0xFFFFFE00
#define USB_CLK_NDIV_40						0x30
#define USB_CLK_NDIV_20						0x60
#define SUPPLY_USBD_POWER					0xfffffffd
#define ChipcommonA_GPIOOut					0x18000064
#define ChipcommonA_GPIOOutEn				0x18000068

/*
 * Function to initialize USB host related low level hardware including PHY,
 * clocks, etc.
 *
 * TODO: expand support for more than one host in the future if needed
 */
int linux_usbh_init(void)
{
	int clk_enable, k;
	unsigned int iClk, USBClk, usbdgpiopwr, usb2_reset_state, pllStatus;

	/* Do USB PHY reset */
	USBClk = ehci_iol_r(IPROC_USB_PHY_CTRL);
	/* bring phy pll out of reset if not done already */
	printf("phy ctrl %08x\n", USBClk);
	if ((USBClk & 0x01000000) == 0 )
	{
		USBClk |= 0x01000000;
		ehci_iol_w(USBClk, IPROC_USB_PHY_CTRL);
		pllStatus = ehci_iol_r(IPROC_WRAP_MISC_STATUS);
		for (k = 0; k < 100000; k++)
		{
			if ((pllStatus & 2) == 2)
			{
				printf("USB phy pll locked\n");
				break;
			}
			pllStatus = ehci_iol_r(IPROC_WRAP_MISC_STATUS);
		}
	}
	udelay(1000);
	USBClk = ehci_iol_r(IPROC_USB_PHY_CTRL);
	ehci_iol_w(USBClk & (~(1<<23)), IPROC_USB_PHY_CTRL);
	udelay(1000);

	/* enable clock */
	clk_enable = ehci_iol_r(IPROC_IDM_USB2_IO_CONTROL_DIRECT);
	printf("Initial usb2h clock is: %08x\n", clk_enable);
	clk_enable |= 1;
	printf("Initial usb2h clock is: %08x\n", clk_enable);
	ehci_iol_w(clk_enable, IPROC_IDM_USB2_IO_CONTROL_DIRECT);

#if defined(CONFIG_HELIX4)
	/* set USB clock to configured */
	iClk = ehci_iol_r(IPROC_XGPLL);
	USBClk = ehci_iol_r(IPROC_USB_PHY_CTRL);
	printf("iClk = %08x, USBClk = %08x\n", iClk, USBClk);
	if ((iClk & 0xff) == IPROC_CLK_NDIV_40)
	{
		ehci_iol_w((USBClk & USB_CLK_NDIV_MASK) | USB_CLK_NDIV_40, IPROC_USB_PHY_CTRL);
		udelay(10);
		ehci_iol_w((USBClk & USB_CLK_PLL_RESET_MASK) | USB_CLK_NDIV_40, IPROC_USB_PHY_CTRL);
		udelay(10);
		ehci_iol_w((USBClk & USB_CLK_PHY_RESET_MASK) | USB_CLK_NDIV_40, IPROC_USB_PHY_CTRL);
		udelay(10);
		USBClk = ehci_iol_r(IPROC_USB_PHY_CTRL);
		printf("iClk = %08x, USBClk = %08x\n", iClk, USBClk);
	}
	else if ((iClk & 0xff) == IPROC_CLK_NDIV_20)
	{
		ehci_iol_w((USBClk & USB_CLK_NDIV_MASK) | USB_CLK_NDIV_20, IPROC_USB_PHY_CTRL);
		udelay(10);
		ehci_iol_w((USBClk & USB_CLK_PLL_RESET_MASK) | USB_CLK_NDIV_20, IPROC_USB_PHY_CTRL);
		udelay(10);
		ehci_iol_w((USBClk & USB_CLK_PHY_RESET_MASK) | USB_CLK_NDIV_20, IPROC_USB_PHY_CTRL);
		udelay(10);
		USBClk = ehci_iol_r(IPROC_USB_PHY_CTRL);
		printf("iClk = %08x, USBClk = %08x\n", iClk, USBClk);
	}
#endif

	/* bring USB PHY out of reset */
	ehci_iol_w(USBClk | (1<<23), IPROC_USB_PHY_CTRL);
	udelay(100);

    printf("\nBring usb2h_out of reset.......\n");
    ehci_iol_w(0x0, IPROC_IDM_USB2_RESET_CONTROL);
    udelay(100000);
    usb2_reset_state = ehci_iol_r(IPROC_IDM_USB2_RESET_CONTROL);
    printf("usb2_reset_state is set and now it is: %08x\n", usb2_reset_state);

#if ((defined(CONFIG_HELIX4)) || (defined(CONFIG_KATANA2)))
	/* supply power for USB device connected to the host */
	usbdgpiopwr = ehci_iol_r(ChipcommonA_GPIOOut);
	usbdgpiopwr &= SUPPLY_USBD_POWER;
	ehci_iol_w(usbdgpiopwr, ChipcommonA_GPIOOut);
	ehci_iol_w(0x2, ChipcommonA_GPIOOutEn);
#endif
	return 0;
}

#endif

/*
 * Function to initialize USB host related low level hardware including PHY,
 * clocks, etc.
 *
 * TODO: expand support for more than one host in the future if needed
 */
int usb0h_init(void);
int usb1h_init(void);
int usb2h_init(void);
int ehci_hcd_init(void)
{
#if ((defined(CONFIG_NS_PLUS)) || (defined(CONFIG_HELIX4)) || (defined(CONFIG_KATANA2)))

	int usb2_clk_cntrl, usb2_clk_enable, sku_vect, usb2_reset_state;

    /* Start clock */
	sku_vect = ehci_iol_r(IPROC_STRAP_SKU_VECTOR);
#if 0
	//if ((sku_vect & IPROC_SKU_STRAP_MASK) != 0x0)
	//{
		/* enable clocks */
		ehci_iol_w(0xEA68, IPROC_USB2_CLK_CONTROL_ENABLE);
		usb2_clk_cntrl = ehci_iol_r(IPROC_USB2_CLK_CONTROL_ENABLE);
		printf("USB clk control enable register is: %08x\n", usb2_clk_cntrl);

		ehci_iol_w(0xDD10F3, IPROC_USB2_CLK_CONTROL_PLL);
		usb2_clk_enable = ehci_iol_r(IPROC_USB2_CLK_CONTROL_PLL);
		printf("USB clk enable register is: %08x\n", usb2_clk_enable);

		ehci_iol_w(0x0, IPROC_USB2_CLK_CONTROL_ENABLE);
		usb2_clk_cntrl = ehci_iol_r(IPROC_USB2_CLK_CONTROL_ENABLE);
		printf("USB clk control enable register is: %08x\n", usb2_clk_cntrl);
	//}
#endif

    //usb2_clk_enable = ehci_iol_r(IPROC_USB2_CLK_CONTROL_PLL);

#if defined(CONFIG_NS_PLUS)
	usb2_clk_enable = ehci_iol_r(USB2_IDM_IDM_IO_CONTROL_DIRECT);
    printf("Before reset, USB clk enable register is: %08x\n", usb2_clk_enable);
    usb2_clk_enable |= 0x1;
    ehci_iol_w( usb2_clk_enable, USB2_IDM_IDM_IO_CONTROL_DIRECT);
    usb2_clk_enable = ehci_iol_r(USB2_IDM_IDM_IO_CONTROL_DIRECT);
    printf("Before reset, USB clk enable register is: %08x\n", usb2_clk_enable);
    udelay(1000);
#else
	linux_usbh_init();
#endif

    /* Reset USBH 2.0 core */
    //ehci_iol_w(0x1, IPROC_IDM_USB2_RESET_CONTROL);
    usb2_reset_state = ehci_iol_r(IPROC_IDM_USB2_RESET_CONTROL);
	printf("usb2_reset_state is: %08x\n", usb2_reset_state);
	if ((usb2_reset_state & 1) == 1)
	{
		printf("\nBring usb2h_out of reset.......\n");
		ehci_iol_w(0x0, IPROC_IDM_USB2_RESET_CONTROL);
		udelay(1000);
		usb2_reset_state = ehci_iol_r(IPROC_IDM_USB2_RESET_CONTROL);
		printf("usb2_reset_state is set and now it is: %08x\n", usb2_reset_state);
	}

    //usb2_clk_enable = ehci_iol_r(IPROC_USB2_CLK_CONTROL_PLL);
    //printf("After reset, USB clk enable register is: %08x\n", usb2_clk_enable);

    /* Take out PHY? */
    //printf("\nusb2, take PHY out of reset\n");
    //ehci_iol_w(0x3ff, 0x18021200);
    //ehci_iol_w(0x3ff, 0x18021204);

    /* map registers */
    hccr = (struct ehci_hccr *)(IPROC_EHCI);
    hcor = (struct ehci_hcor *)((uint32_t) hccr +
            HC_LENGTH(ehci_iol_r(&hccr->cr_capbase)));

#elif defined(CONFIG_CYGNUS)




	debug("%d:%s() enter\n", __LINE__, __func__);

    if (usb0h_init() != 0) {
        if (usb1h_init() != 0) {
                usb2h_init();
        } 
    }
   
	/* map registers */
	hccr = (struct ehci_hccr *)(IPROC_EHCI);
	hcor = (struct ehci_hcor *)(USBH_USBCMD);

	debug("Cygnus-INFO: hccr= 0x%x, hcor= 0x%x\n", (int)hccr, (int)hcor);

#else
	int usb2_clk_cntrl, usb2_clk_enable, sku_vect, usb2_reset_state;

    /* Start clock */
	sku_vect = ehci_iol_r(IPROC_STRAP_SKU_VECTOR);
	if ((sku_vect & IPROC_SKU_STRAP_MASK) != 0x0)
	{
		/* enable clocks */
		ehci_iol_w(0xEA68, IPROC_USB2_CLK_CONTROL_ENABLE);
		usb2_clk_cntrl = ehci_iol_r(IPROC_USB2_CLK_CONTROL_ENABLE);
		printf("USB clk control enable register is: %08x\n", usb2_clk_cntrl);

		ehci_iol_w(0xDD10F3, IPROC_USB2_CLK_CONTROL_PLL);
		usb2_clk_enable = ehci_iol_r(IPROC_USB2_CLK_CONTROL_PLL);
		printf("USB clk enable register is: %08x\n", usb2_clk_enable);

		ehci_iol_w(0x0, IPROC_USB2_CLK_CONTROL_ENABLE);
		usb2_clk_cntrl = ehci_iol_r(IPROC_USB2_CLK_CONTROL_ENABLE);
		printf("USB clk control enable register is: %08x\n", usb2_clk_cntrl);
	}

    usb2_clk_enable = ehci_iol_r(IPROC_USB2_CLK_CONTROL_PLL);
    //printf("Before reset, USB clk enable register is: %08x\n", usb2_clk_enable);

    /* Reset USBH 2.0 core */
    ehci_iol_w(0x1, IPROC_IDM_USB2_RESET_CONTROL);
    printf("\nusb2h_reset.......\n");
    ehci_iol_w(0x0, IPROC_IDM_USB2_RESET_CONTROL);
    usb2_reset_state = ehci_iol_r(IPROC_IDM_USB2_RESET_CONTROL);
    printf("usb2_reset_state is set and now it is: %08x\n", usb2_reset_state);

    usb2_clk_enable = ehci_iol_r(IPROC_USB2_CLK_CONTROL_PLL);
    //printf("After reset, USB clk enable register is: %08x\n", usb2_clk_enable);

    /* Take out PHY? */
    //printf("\nusb2, take PHY out of reset\n");
    //ehci_iol_w(0x3ff, 0x18021200);
    //ehci_iol_w(0x3ff, 0x18021204);

    /* map registers */
    hccr = (struct ehci_hccr *)(IPROC_EHCI);
    hcor = (struct ehci_hcor *)((uint32_t) hccr +
            HC_LENGTH(ehci_iol_r(&hccr->cr_capbase)));
#endif
    return 0;
}

#if defined(CONFIG_CYGNUS)
//#undef debug
//#define debug printf
#define rd(x) reg32_read((volatile uint32_t *)x)
#define wr(x,v) reg32_write((volatile uint32_t *)x,v)
#define print_log printf

int usb1h_init(void)
{
	int i = 0, count = 1000;
	volatile u32 reg_data = 10;

	wr(CDRU_USBPHY_CLK_RST_SEL, 1);
	    wr(CDRU_SPARE_REG_0, 0x7);

	    //Bring the AFE block out of reset to start powering up the PHY
	    reg_data = rd(CRMU_USB_PHY_AON_CTRL);
	    reg_data = reg_data | (1 << CRMU_USB_PHY_AON_CTRL__CRMU_USBPHY_P1_AFE_CORERDY_VDDC);
	    wr(CRMU_USB_PHY_AON_CTRL, reg_data);

	    //Wait for the PLL lock status
        i = 0;
	    do
	    {
            udelay(1);
            i++;
	        reg_data = rd(CDRU_USBPHY_P1_STATUS);
	        //print_log("( = 0x%08X\n", reg_data);
            if (i >= count) {printf("failed\n");return -1; }
	    } while ((reg_data & (1 << CDRU_USBPHY_P1_STATUS__USBPHY_ILDO_ON_FLAG)) == 0);

	    print_log("Initializing USB Host\n");

	    print_log("USB clock enable\n");
	    //USB Host clock enable
	    reg_data = rd(USB2_IDM_IDM_IO_CONTROL_DIRECT);
	    reg_data |= (1 << USB2_IDM_IDM_IO_CONTROL_DIRECT__clk_enable);
	    wr(USB2_IDM_IDM_IO_CONTROL_DIRECT, reg_data);

	    print_log("Bring USB Host out of reset\n");
	    reg_data = rd(USB2_IDM_IDM_RESET_CONTROL);
	    reg_data |= (1 << USB2_IDM_IDM_RESET_CONTROL__RESET);
	    wr(USB2_IDM_IDM_RESET_CONTROL, reg_data);
	    reg_data &= ~(1 << USB2_IDM_IDM_RESET_CONTROL__RESET);
	    wr(USB2_IDM_IDM_RESET_CONTROL, reg_data);

        udelay(10);

	    //wr(USBH_Phy_Ctrl_P0, 0x3FF);
	    wr(USBH_Phy_Ctrl_P1, 0x3FF);

	    reg_data = rd(USBH_HCCAPBASE);
	    reg_data = reg_data >> USBH_HCCAPBASE__HCIVERSION_R;
	    print_log("EHCI revision number = 0x%08X\n", reg_data);
        
        i = 0;
	    do {
            udelay(1);
            i++;
	        reg_data = rd(USBH_USBSTS);
	        //print_log("USBH_USBSTS = 0x%08X\n", reg_data);
            if (i >= count) {printf("failed\n");return -1; }
	    } while ((reg_data & (1 << USBH_USBSTS__HCHalted)) == 0);

	    ////fixme
	    //wr(USBH_PERIODICLISTBASE, 0x90000000);

	    reg_data = rd(USBH_USBCMD);
	    reg_data |= 1;
	    wr(USBH_USBCMD, reg_data);

	    reg_data = rd(USBH_CONFIGFLAG);
	    reg_data |= 1;
	    wr(USBH_CONFIGFLAG, reg_data);
        
        i = 0;
	    do {
            udelay(1);
            i++;
	        reg_data = rd(USBH_USBSTS);
	        //print_log("USBH_USBSTS = 0x%08X\n", reg_data);
            if (i >= count) { printf("failed\n");return -1; }
	    } while (reg_data & (1 << USBH_USBSTS__HCHalted));

	    reg_data = rd(USBH_PORTSC_1);
	    reg_data |= (1 << USBH_PORTSC_1__Port_Power_PP);
	    wr(USBH_PORTSC_1, reg_data);

	    u32 last_reg_data = 0x11223344;
        i = 0;
	    do {
            udelay(1);
            i++;
	        reg_data = rd(USBH_USBSTS);
	        if(reg_data != last_reg_data) {
	            //print_log("Waiting for Port_Change_Detect - USBH_USBSTS = 0x%08X\n", reg_data);
	        }
	        last_reg_data = reg_data;
            if (i >= count) { printf("failed\n");return -1; }
	    } while ((reg_data & (1 << USBH_USBSTS__Port_Change_Detect)) == 0);

	    wr(USBH_USBSTS, (1 << USBH_USBSTS__Port_Change_Detect));

	    reg_data = rd(USBH_PORTSC_1);
	    print_log("USBH_PORTSC_1 = 0x%08X\n", reg_data);
	    reg_data |= (1 << USBH_PORTSC_1__Port_Reset);
	    reg_data &= ~(1 << USBH_PORTSC_1__Port_Enabled_Disabled);
	    wr(USBH_PORTSC_1, reg_data);
	    reg_data = rd(USBH_PORTSC_1);
	    print_log("USBH_PORTSC_1 = 0x%08X\n", reg_data);

        udelay(1000);

	    //disable port reset
	    reg_data = rd(USBH_PORTSC_1);
	    reg_data &= ~(1 << USBH_PORTSC_1__Port_Reset);
	    wr(USBH_PORTSC_1, reg_data);
	    reg_data = rd(USBH_PORTSC_1);
	    print_log("USBH_PORTSC_1 = 0x%08X\n", reg_data);
        
        i = 0;
	    do {
            udelay(1);
            i++;
	        reg_data = rd(USBH_PORTSC_1);
	        //print_log("USBH_PORTSC_1 = 0x%08X\n", reg_data);
            if (i >= count) { printf("failed\n");return -1; }
	    } while (reg_data & (1 << USBH_PORTSC_1__Port_Reset));

        i = 0;
	    do {
            udelay(1);
            i++;
	        reg_data = rd(USBH_PORTSC_1);
	        //print_log("USBH_PORTSC_1 = 0x%08X\n", reg_data);
            if (i >= count) { printf("failed\n");return -1; }
	    } while ((reg_data & (1 << USBH_PORTSC_1__Port_Enabled_Disabled)) == 0);

	    print_log("Port is now enabled by HOST !!!\n");
        
        i = 0;
	    do {
            udelay(1);
            i++;
	        reg_data = rd(USBH_PORTSC_1);
	        //print_log("USBH_PORTSC_1 = 0x%08X\n", reg_data);
            if (i >= count) { printf("failed\n");return -1; }
	    } while ((reg_data & (1 << USBH_PORTSC_1__Current_Connect_Status)) == 0);

	    print_log("Device connected on Port-1 !!!\n");
        return 0;
}


int usb0h_init(void)
{
    int i = 0, count = 1000;
    volatile u32 reg_data = 10;

        wr(CDRU_USBPHY_CLK_RST_SEL, 0);
        wr(CDRU_SPARE_REG_0, 0x7);
        
        //Bring the AFE block out of reset to start powering up the PHY
        reg_data = rd(CRMU_USB_PHY_AON_CTRL);
        reg_data = reg_data | (1 << CRMU_USB_PHY_AON_CTRL__CRMU_USBPHY_P0_AFE_CORERDY_VDDC);
        wr(CRMU_USB_PHY_AON_CTRL, reg_data);

        //Wait for the PLL lock status
        i = 0;
        do
        {
            udelay(1);
            i++;
            reg_data = rd(CDRU_USBPHY_P0_STATUS);
            //print_log("( = 0x%08X\n", reg_data);
            if (i >= count) {printf("failed");return -1; }
        } while ((reg_data & (1 << CDRU_USBPHY_P0_STATUS__USBPHY_ILDO_ON_FLAG)) == 0);

        print_log("Initializing USB Host\n");

        print_log("USB clock enable\n");
        //USB Host clock enable
        reg_data = rd(USB2_IDM_IDM_IO_CONTROL_DIRECT);
        reg_data |= (1 << USB2_IDM_IDM_IO_CONTROL_DIRECT__clk_enable);
        wr(USB2_IDM_IDM_IO_CONTROL_DIRECT, reg_data);

        print_log("Bring USB Host out of reset\n");
        reg_data = rd(USB2_IDM_IDM_RESET_CONTROL);
        reg_data |= (1 << USB2_IDM_IDM_RESET_CONTROL__RESET);
        wr(USB2_IDM_IDM_RESET_CONTROL, reg_data);
        reg_data &= ~(1 << USB2_IDM_IDM_RESET_CONTROL__RESET);
        wr(USB2_IDM_IDM_RESET_CONTROL, reg_data);

        udelay(1000);

        wr(USBH_Phy_Ctrl_P0, 0x3FF);
        //wr(USBH_Phy_Ctrl_P1, 0x3FF);

        reg_data = rd(USBH_HCCAPBASE);
        reg_data = reg_data >> USBH_HCCAPBASE__HCIVERSION_R;
        print_log("EHCI revision number = 0x%08X\n", reg_data);
        
        i = 0;
        do {
            udelay(1);
            i++;
            reg_data = rd(USBH_USBSTS);
            //print_log("USBH_USBSTS = 0x%08X\n", reg_data);
            if (i >= count) {printf("failed\n"); return -1; }
        } while ((reg_data & (1 << USBH_USBSTS__HCHalted)) == 0);

        ////fixme 
        //wr(USBH_PERIODICLISTBASE, 0x90000000);

        reg_data = rd(USBH_USBCMD);
        reg_data |= 1;
        wr(USBH_USBCMD, reg_data);

        reg_data = rd(USBH_CONFIGFLAG);
        reg_data |= 1;
        wr(USBH_CONFIGFLAG, reg_data);
        
        i = 0;
        do {
            udelay(1);
            i++;
            reg_data = rd(USBH_USBSTS);
            //print_log("USBH_USBSTS = 0x%08X\n", reg_data);
            if (i >= count) { printf("failed\n");return -1; }
        } while (reg_data & (1 << USBH_USBSTS__HCHalted));

        reg_data = rd(USBH_PORTSC_0);
        reg_data |= (1 << USBH_PORTSC_0__Port_Power_PP);
        wr(USBH_PORTSC_0, reg_data);

        u32 last_reg_data = 0x11223344;
        i = 0;
        do {
            udelay(1);
            i++;
            reg_data = rd(USBH_USBSTS);
            if(reg_data != last_reg_data) {
                //print_log("Waiting for Port_Change_Detect - USBH_USBSTS = 0x%08X\n", reg_data);
            }
            last_reg_data = reg_data;
            if (i >= count) { printf("failed\n");return -1; }
        } while ((reg_data & (1 << USBH_USBSTS__Port_Change_Detect)) == 0);

        wr(USBH_USBSTS, (1 << USBH_USBSTS__Port_Change_Detect));

        reg_data = rd(USBH_PORTSC_0);
        print_log("USBH_PORTSC_0 = 0x%08X\n", reg_data);
        reg_data |= (1 << USBH_PORTSC_0__Port_Reset);
        reg_data &= ~(1 << USBH_PORTSC_0__Port_Enabled_Disabled);
        wr(USBH_PORTSC_0, reg_data);
        reg_data = rd(USBH_PORTSC_0);
        print_log("USBH_PORTSC_0 = 0x%08X\n", reg_data);

        udelay(1000);

        //disable port reset
        reg_data = rd(USBH_PORTSC_0);
        reg_data &= ~(1 << USBH_PORTSC_0__Port_Reset);
        wr(USBH_PORTSC_0, reg_data);
        reg_data = rd(USBH_PORTSC_0);
        print_log("USBH_PORTSC_0 = 0x%08X\n", reg_data);
        
        i = 0;
        do {
            udelay(1);
            i++;
            reg_data = rd(USBH_PORTSC_0);
            //print_log("USBH_PORTSC_0 = 0x%08X\n", reg_data);
            if (i >= count) { printf("failed\n");return -1; }
        } while (reg_data & (1 << USBH_PORTSC_0__Port_Reset));

        i = 0;
        do {
            udelay(1);
            i++;
            reg_data = rd(USBH_PORTSC_0);
            //print_log("USBH_PORTSC_0 = 0x%08X\n", reg_data);
            if (i >= count) { printf("failed\n");return -1; }
        } while ((reg_data & (1 << USBH_PORTSC_0__Port_Enabled_Disabled)) == 0);

        print_log("Port 0 is now enabled by HOST !!!\n");
        
        i = 0;
        do {
            udelay(1);
            i++;
            reg_data = rd(USBH_PORTSC_0);
            //print_log("USBH_PORTSC_1 = 0x%08X\n", reg_data);
            if (i >= count) { printf("failed\n");return -1; }
        } while ((reg_data & (1 << USBH_PORTSC_0__Current_Connect_Status)) == 0);

        print_log("Device connected on Port-0 !!!\n");
        return 0;
}

int usb2h_init(void)
{
    int i = 0, count = 1000;
    volatile u32 reg_data = 10;

        wr(CDRU_USBPHY_CLK_RST_SEL, 2);
        wr(CDRU_SPARE_REG_0, 0x7);
        
        //Bring the AFE block out of reset to start powering up the PHY
        reg_data = rd(CRMU_USB_PHY_AON_CTRL);
        reg_data = reg_data | (1 << CRMU_USB_PHY_AON_CTRL__CRMU_USBPHY_P2_AFE_CORERDY_VDDC);
        wr(CRMU_USB_PHY_AON_CTRL, reg_data);

        //Wait for the PLL lock status
        i = 0;
        do
        {
            udelay(1);
            i++;
            reg_data = rd(CDRU_USBPHY_P2_STATUS);
            //print_log("( = 0x%08X\n", reg_data);
            if (i >= count) {return -1; }
        } while ((reg_data & (1 << CDRU_USBPHY_P2_STATUS__USBPHY_ILDO_ON_FLAG)) == 0);

        print_log("Initializing USB Host\n");

        print_log("USB clock enable\n");
        //USB Host clock enable
        reg_data = rd(USB2_IDM_IDM_IO_CONTROL_DIRECT);
        reg_data |= (1 << USB2_IDM_IDM_IO_CONTROL_DIRECT__clk_enable);
        wr(USB2_IDM_IDM_IO_CONTROL_DIRECT, reg_data);

        print_log("Bring USB Host out of reset\n");
        reg_data = rd(USB2_IDM_IDM_RESET_CONTROL);
        reg_data |= (1 << USB2_IDM_IDM_RESET_CONTROL__RESET);
        wr(USB2_IDM_IDM_RESET_CONTROL, reg_data);
        reg_data &= ~(1 << USB2_IDM_IDM_RESET_CONTROL__RESET);
        wr(USB2_IDM_IDM_RESET_CONTROL, reg_data);

        udelay(1000);

        wr(USBH_Phy_Ctrl_P2, 0x3FF);
        //wr(USBH_Phy_Ctrl_P1, 0x3FF);

             //Program USB Port 2 in host Mode
        reg_data = rd(CDRU_USBPHY2_HOST_DEV_SEL);
        reg_data |= (1 << CDRU_USBPHY2_HOST_DEV_SEL__PHY2_DEV_HOST_CTRL_SEL);
        wr(CDRU_USBPHY2_HOST_DEV_SEL,reg_data);
        print_log("CDRU USBPHY2 HOST DEV SEL = 0x%08X \n",reg_data);

        reg_data = rd(USBH_HCCAPBASE);
        wr(USBH_HCCAPBASE, reg_data & 0xFFFF);
        reg_data = reg_data >> USBH_HCCAPBASE__HCIVERSION_R;
        print_log("EHCI revision number = 0x%08X\n", reg_data);
        
        i = 0;
        do {
            udelay(1);
            i++;
            reg_data = rd(USBH_USBSTS);
            //print_log("USBH_USBSTS = 0x%08X\n", reg_data);
            if (i >= count) { return -1; }
        } while ((reg_data & (1 << USBH_USBSTS__HCHalted)) == 0);

        ////fixme 
        //wr(USBH_PERIODICLISTBASE, 0x90000000);

        reg_data = rd(USBH_USBCMD);
        reg_data |= 1;
        wr(USBH_USBCMD, reg_data);

        reg_data = rd(USBH_CONFIGFLAG);
        reg_data |= 1;
        wr(USBH_CONFIGFLAG, reg_data);
        
        i = 0;
        do {
            udelay(1);
            i++;
            reg_data = rd(USBH_USBSTS);
            //print_log("USBH_USBSTS = 0x%08X\n", reg_data);
            if (i >= count) { return -1; }
        } while (reg_data & (1 << USBH_USBSTS__HCHalted));

        reg_data = rd(USBH_PORTSC_2);
        reg_data |= (1 << USBH_PORTSC_2__Port_Power_PP);
        wr(USBH_PORTSC_2, reg_data);

        u32 last_reg_data = 0x11223344;
        i = 0;
        do {
            udelay(1);
            i++;
            reg_data = rd(USBH_USBSTS);
            if(reg_data != last_reg_data) {
                //print_log("Waiting for Port_Change_Detect - USBH_USBSTS = 0x%08X\n", reg_data);
            }
            last_reg_data = reg_data;
            if (i >= count) {return -1; }
        } while ((reg_data & (1 << USBH_USBSTS__Port_Change_Detect)) == 0);

        wr(USBH_USBSTS, (1 << USBH_USBSTS__Port_Change_Detect));

        reg_data = rd(USBH_PORTSC_2);
        print_log("USBH_PORTSC_2 = 0x%08X\n", reg_data);
        reg_data |= (1 << USBH_PORTSC_2__Port_Reset);
        reg_data &= ~(1 << USBH_PORTSC_2__Port_Enabled_Disabled);
        wr(USBH_PORTSC_2, reg_data);
        reg_data = rd(USBH_PORTSC_2);
        print_log("USBH_PORTSC_2 = 0x%08X\n", reg_data);

        udelay(1000);

        //disable port reset
        reg_data = rd(USBH_PORTSC_2);
        reg_data &= ~(1 << USBH_PORTSC_2__Port_Reset);
        wr(USBH_PORTSC_2, reg_data);
        reg_data = rd(USBH_PORTSC_2);
        print_log("USBH_PORTSC_2 = 0x%08X\n", reg_data);
        
        i = 0;
        do {
            udelay(1);
            i++;
            reg_data = rd(USBH_PORTSC_2);
            //print_log("USBH_PORTSC_2 = 0x%08X\n", reg_data);
            if (i >= count) {return -1; }
        } while (reg_data & (1 << USBH_PORTSC_2__Port_Reset));

        i = 0;
        do {
            udelay(1);
            i++;
            reg_data = rd(USBH_PORTSC_2);
            //print_log("USBH_PORTSC_0 = 0x%08X\n", reg_data);
            if (i >= count) {return -1; }
        } while ((reg_data & (1 << USBH_PORTSC_2__Port_Enabled_Disabled)) == 0);

        print_log("Port 2 is now enabled by HOST !!!\n");
        
        i = 0;
        do {
            udelay(1);
            i++;
            reg_data = rd(USBH_PORTSC_2);
            //print_log("USBH_PORTSC_2 = 0x%08X\n", reg_data);
            if (i >= count) { return -1; }
        } while ((reg_data & (1 << USBH_PORTSC_2__Current_Connect_Status)) == 0);

        print_log("Device connected on Port-2 !!!\n");
        return 0;
}


#if 0
void usb2d_init(void) {

	unsigned int reg_data;



	print_log("USB clock enable\n");
	//USB Host clock enable
	reg_data = rd(USB2_IDM_IDM_IO_CONTROL_DIRECT);
	reg_data |= (1 << USB2D_IDM_IDM_IO_CONTROL_DIRECT__clk_enable);
	wr(USB2_IDM_IDM_IO_CONTROL_DIRECT, reg_data);

	print_log("Bring USB Host out of reset\n");
	reg_data = rd(USB2_IDM_IDM_RESET_CONTROL);
	reg_data |= (1 << USB2_IDM_IDM_RESET_CONTROL__RESET);
	wr(USB2_IDM_IDM_RESET_CONTROL, reg_data);
	reg_data &= ~(1 << USB2_IDM_IDM_RESET_CONTROL__RESET);
	wr(USB2_IDM_IDM_RESET_CONTROL, reg_data);

	////wait for ?? time - FIXME
	//for(i=0; i<10000; i++);

	wr(USBH_Phy_Ctrl_P0, 0x3FF);
	wr(USBH_Phy_Ctrl_P1, 0x3FF);
	 wr(USBH_Phy_Ctrl_P2, 0x3FF);


	 //Program USB Port 2 in Device Mode
	reg_data = rd(CDRU_USBPHY2_HOST_DEV_SEL);
	reg_data |= (0 << CDRU_USBPHY2_HOST_DEV_SEL__PHY2_DEV_HOST_CTRL_SEL);
	wr(CDRU_USBPHY2_HOST_DEV_SEL,reg_data);
	print_log("CDRU USBPHY2 HOST DEV SEL = 0x%08X \n",reg_data);
	print_log("USB Port 2 configured in Device Mode \n");
	 print_log("REgister address is 0x%08X and value is 0x%08X \n",CDRU_USBPHY2_HOST_DEV_SEL,reg_data);


	//Wire Host to Port 1
	reg_data = rd(CDRU_USBPHY_CLK_RST_SEL);
	reg_data = 1;
	 //(1 << CDRU_USBPHY_CLK_RST_SEL__USBPHY_TO_HOST_CLK_RST_SEL_R);
	wr(CDRU_USBPHY_CLK_RST_SEL,reg_data);
	print_log("CDRU_USBPHY_CLK_RST_SEL = 0x%08X \n",reg_data);
	print_log("USB Port 1 wired to the Host \n");

	 reg_data = rd(CRMU_USB_PHY_AON_CTRL);
	reg_data = reg_data | (1 << CRMU_USB_PHY_AON_CTRL__CRMU_USBPHY_P2_RESETB);
	 reg_data = reg_data | (1 << CRMU_USB_PHY_AON_CTRL__CRMU_USBPHY_P1_RESETB);
	  reg_data = reg_data | (1 << CRMU_USB_PHY_AON_CTRL__CRMU_USBPHY_P0_RESETB);
	wr(CRMU_USB_PHY_AON_CTRL,reg_data);

	reg_data = rd(CDRU_SPARE_REG_0);
	reg_data = reg_data | 0x7;
	wr(CDRU_SPARE_REG_0,reg_data);


	do {
		reg_data = rd(USBH_HCCAPBASE);
		reg_data = reg_data >> USBH_HCCAPBASE__HCIVERSION_R;
		print_log("EHCI revision number = 0x%08X\n", reg_data);
	} while (!reg_data);

	u32 reg_data_last = 1;//rand();
	do {
		reg_data = rd(USBH_USBSTS);
		if(reg_data_last != reg_data) {
			print_log("Waiting for USBSTS__HCHalted bit to be set - USBH_USBSTS = 0x%08X\n", reg_data);
		}
		reg_data_last = reg_data;
	} while ((reg_data & (1 << USBH_USBSTS__HCHalted)) == 0);

	reg_data = rd(USBH_USBCMD);
	reg_data |= 1;
	wr(USBH_USBCMD, reg_data);

	reg_data = rd(USBH_CONFIGFLAG);
	reg_data |= 1;
	wr(USBH_CONFIGFLAG, reg_data);

	reg_data_last = 2;//rand();
	do {
		reg_data = rd(USBH_USBSTS);
		if(reg_data_last != reg_data) {
			print_log("Waiting for USBSTS__HCHalted bit to be cleared - USBH_USBSTS = 0x%08X\n", reg_data);
		}
		reg_data_last = reg_data;
	} while (reg_data & (1 << USBH_USBSTS__HCHalted));

	//Set the RUN STOP Bit to 1 //sathishs
    reg_data = rd(USBH_USBCMD);
    reg_data |= 1;
    wr(USBH_USBCMD, reg_data);
     //debug("REgister address is 0x%08X and value is 0x%08X \n",USBH_USBCMD,reg_data);

    //configure the default port routing control logic. Last action in the host controller configuration section //sathishs
    reg_data = rd(USBH_CONFIGFLAG);
    reg_data |= 1;
    wr(USBH_CONFIGFLAG, reg_data);
     //debug("REgister address is 0x%08X and value is 0x%08X \n",USBH_CONFIGFLAG,reg_data);

    //check if the HALTED bit is made '0' as the RUN/STOP bit has been made '1' //sathishs
    do {
        reg_data = rd(USBH_USBSTS);
//        debug("2USBH_USBSTS = 0x%08X\n", reg_data);

        udelay(1000);
    } while (reg_data & (1 << USBH_USBSTS__HCHalted));
     debug("REgister address is 0x%08X and value is 0x%08X \n",USBH_USBSTS,reg_data);

    //Power up the relevant port - Changing the port to '2' - commented out amits port 1 code //sathishs
	reg_data = rd(USBH_PORTSC_2);
    reg_data |= (1 << USBH_PORTSC_2__Port_Power_PP);
    wr(USBH_PORTSC_2, reg_data);
     //debug("REgister address is 0x%08X and value is 0x%08X \n",USBH_PORTSC_2,reg_data);

	debug("Wait till the port change detect has become a '1' \n"); //sathishs
   do {
        reg_data = rd(USBH_USBSTS);
//        debug("3USBH_USBSTS = 0x%08X\n", reg_data);

        udelay(1000);
    } while ((reg_data & (1 << USBH_USBSTS__Port_Change_Detect)) == 0);
    debug("REgister address is 0x%08X and value is 0x%08X \n",USBH_USBSTS,reg_data);

    //Since USBH_USBSTS is a R/WC register. A write of a '1' will se tthe corresponding bit to '0'. Writing '0' to a bit position has no effect. //sathishs
	//Writing a '1' to the port change detect field in order to make it a '0' //sathishs
    wr(USBH_USBSTS, (1 << USBH_USBSTS__Port_Change_Detect));
    reg_data = rd(USBH_USBSTS);
    debug("Check to see if the port change detect has been made 0 again 0x%08X \n", reg_data);

	//Changing the register configs to Port_2 instead of Port_1 //sathishs
	//Port Reset is made one and port enabled disabled is made 0 //sathishs
    reg_data = rd(USBH_PORTSC_2);
    debug("USBH_PORTSC_2 = 0x%08X\n", reg_data);
    reg_data |= (1 << USBH_PORTSC_2__Port_Reset);
    debug("Port reset is made 1 \n ");
    reg_data &= ~(1 << USBH_PORTSC_2__Port_Enabled_Disabled);
    debug("Port Enabled Disabled is made 0 \n");
    wr(USBH_PORTSC_2, reg_data);
    reg_data = rd(USBH_PORTSC_2);
    debug("USBH_PORTSC_2 = 0x%08X\n", reg_data);
     debug("REgister address is 0x%08X and value is 0x%08X \n",USBH_PORTSC_2,reg_data);

    //wait for ?? time - FIXME
//    for(i=0; i<100000; i++);
     mdelay(500);

    //Changing the register configs to Port_2 instead of Port_1 //sathishs
	//Disable port_2 reset
    debug("Print the status of the Port Reset bit\n");
	reg_data = rd(USBH_PORTSC_2);
    reg_data &= ~(1 << USBH_PORTSC_2__Port_Reset);
    debug("Actual Value programmed into USBH_PORTSC_2 register 0x%08x",reg_data);
    wr(USBH_PORTSC_2, reg_data);
    reg_data = rd(USBH_PORTSC_2);
    debug("USBH_PORTSC_2 = 0x%08X\n", reg_data);
     debug("REgister address is 0x%08X and value is 0x%08X \n",USBH_PORTSC_2,reg_data);


     //Changing the register configs to Port_2 instead of Port_1 //sathishs
	 //Make sure that the Port reset for port 2 has become 0 //sathishs
	 do {
        reg_data = rd(USBH_PORTSC_2);
//        debug("4USBH_PORTSC_2 = 0x%08X\n", reg_data);

        udelay(1000);
    } while (reg_data & (1 << USBH_PORTSC_2__Port_Reset));

    //Changing the register configs to Port_2 instead of Port_1 //sathishs
	//Make sure the port enabled disabled register becomes a '1'
	   do {
        reg_data = rd(USBH_PORTSC_2);
//        debug("5USBH_PORTSC_2 = 0x%08X\n", reg_data);

        udelay(1000);
    } while ((reg_data & (1 << USBH_PORTSC_2__Port_Enabled_Disabled)) == 0);

    debug("Port 2 is now enabled by HOST !!!\n");

    //Changing the register configs to Port_2 instead of Port_1 //sathishs
	//Wait till the current connect status for port 2 is a '1' //sathishs
	do {
        reg_data = rd(USBH_PORTSC_2);
//        debug("USBH_PORTSC_2 = 0x%08X\n", reg_data);

        udelay(1000);
    } while ((reg_data & (1 << USBH_PORTSC_2__Current_Connect_Status)) == 0);

    printf("Device connected on Port-2 !!!\n"); //sathishs
}
#endif
#endif
/*
 * Function to terminate USB host related low level hardware including PHY,
 * clocks, etc.
 *
 * TODO: expand support for more than one host in the future if needed
 */
int ehci_hcd_stop(void)
{
	return 0;
}
