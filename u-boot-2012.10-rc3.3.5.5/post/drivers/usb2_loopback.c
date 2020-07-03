
#include <common.h>
#include <post.h>

#if defined(CONFIG_CYGNUS)
#if CONFIG_POST & CONFIG_SYS_POST_USB20
#include "asm/arch/socregs.h"
#include "asm/arch/reg_utils.h"
#include "usb2d_apis.h"

#define error_log printf
#define print_log printf
#define rd(x) reg32_read((volatile uint32_t *)x)
#define wr(x, v) reg32_write((volatile uint32_t *)x, v)

typedef  volatile struct {
    uint32_t next_qTD_T:1;
    uint32_t next_qTD_RESVD0:4;
    uint32_t next_qTD_ptr:27;
    uint32_t altn_qTD_T:1;
    uint32_t altn_qTD_RESVD0:4;
    uint32_t altn_qTD_ptr:27;
    uint32_t status:8;
    uint32_t pid_code:2;
    uint32_t cerr:2;
    uint32_t c_page:3;
    uint32_t ioc:1;
    uint32_t total_bytes:15;
    uint32_t dt:1;
    uint32_t buffer_pointer_0:32;
    uint32_t buffer_pointer_1:32;
    uint32_t buffer_pointer_2:32;
    uint32_t buffer_pointer_3:32;
    uint32_t buffer_pointer_4:32;
}__packed qTD;

typedef  volatile struct {
    uint32_t T:1;
    uint32_t Typ:2;
    uint32_t RES0:2;
    uint32_t QH_HLP:27;
    uint32_t device_address:7;
    uint32_t I:1;
    uint32_t EndPt:4;
    uint32_t EPS:2;
    uint32_t dtc:1;
    uint32_t H:1;
    uint32_t max_pkt_len:11;
    uint32_t C:1;
    uint32_t RL:4;
    uint32_t uFrame_S_mask:8;
    uint32_t uFrame_C_mask:8;
    uint32_t Hub_Addr:7;
    uint32_t Port_Number:7;
    uint32_t Mult:2;
    uint32_t RES1:5;
    uint32_t Current_qTD_ptr:27;
    qTD      current_qTD_cache;
} __packed qH;

typedef  volatile struct {
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
}__packed setup_fmt;

typedef  volatile struct {
    uint32_t tx_bytes:16;
    uint32_t reserved:11;
    uint32_t L:1;
    uint32_t TxSts:2;
    uint32_t BS:2;
} __packed usbd_in_buffer_status_t;

typedef  volatile struct {
    usbd_in_buffer_status_t buffer_status;
    uint32_t reserved;
    uint32_t buffer_pointer;
    uint32_t next_desc_ptr;
} __packed usbd_in_data_desc_t;

typedef  volatile struct {
    uint32_t rx_bytes:16;
    uint32_t reserved:11;
    uint32_t L:1;
    uint32_t RxSts:2;
    uint32_t BS:2;
} __packed usbd_out_buffer_status_t;

typedef  volatile struct {
    usbd_out_buffer_status_t buffer_status;
    uint32_t reserved;
    uint32_t buffer_pointer;
    uint32_t next_desc_ptr;
} __packed usbd_out_data_desc_t;

typedef  volatile struct {
    uint32_t reserved:16;
    uint32_t ConfigSts:12;
    uint32_t RxSts:2;
    uint32_t BS:2;
} __packed usbd_setup_buffer_status_t;

typedef  volatile struct {
    usbd_setup_buffer_status_t buffer_status;
    uint32_t reserved;
    uint32_t setup_data0;
    uint32_t setup_data1;
} __packed usbd_setup_data_desc_t;



static int usb0h_prepare(void)
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
        return 0;   
}
static int usb0h_check(void)
{
        int i = 0, count = 1000;
        volatile u32 reg_data = 10;
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
static int usb1h_prepare(void)
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
        return 0;   
}

static int usb1h_check(void)
{
        int i = 0, count = 1000;
        volatile u32 reg_data = 10;
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

//static 
int usb2d_prepare(void)
{
    uint32_t  reg_data;
    int i = 0, count = 1000;
    
    ///////////////////
    //wr(CDRU_USBPHY_CLK_RST_SEL, 2);
    //wr(CDRU_SPARE_REG_0, 0x7);
    
    ///////////////////
    
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
        //print_log("CDRU_USBPHY_P2_STATUS = 0x%08X\n", reg_data);
        if (i >= count) {  return -1; }
    } while ((reg_data & (1 << CDRU_USBPHY_P2_STATUS__USBPHY_ILDO_ON_FLAG)) == 0);
    
    i = 0;
    do
    {
    	udelay(1);
        i++;
        reg_data = rd(CDRU_USBPHY_P2_STATUS);
        //print_log("CDRU_USBPHY_P2_STATUS = 0x%08X\n", reg_data);
        if (i >= count) {  return -1; }
    } while ((reg_data & (1 << CDRU_USBPHY_P2_STATUS__USBPHY_PLL_LOCK)) == 0);
  
//    print_log("Initializing USB Device\n");
//    print_log("USB clock enable\n");
    //USB Host clock enable
    
       /////////////////////////////////
    reg_data = rd(USB2_IDM_IDM_IO_CONTROL_DIRECT);
    reg_data &= ~(1 << USB2_IDM_IDM_IO_CONTROL_DIRECT__clk_enable);
    wr(USB2_IDM_IDM_IO_CONTROL_DIRECT, reg_data);
    ///////////////////////////////// 
    
    reg_data = rd(USB2D_IDM_IDM_IO_CONTROL_DIRECT);
    reg_data |= (1 << USB2D_IDM_IDM_IO_CONTROL_DIRECT__clk_enable); 
    wr(USB2D_IDM_IDM_IO_CONTROL_DIRECT, reg_data);


    //////////////////////////////
    reg_data = rd(USB2D_IDM_IDM_RESET_CONTROL);
    reg_data |= (1 << USB2D_IDM_IDM_RESET_CONTROL__RESET); 
    wr(USB2D_IDM_IDM_RESET_CONTROL, reg_data);
    udelay(100);
    /////////////////////////////

    //Deassert the IDM reset if it is alreay asserted otherwise assert reset and then deassert
    // Bring the EHCI/OHCI Core out of reset --- by this time PHY is out of reset and clocks are stable.
//    print_log("Bring USB Device out of reset\n");
    reg_data = rd(USB2D_IDM_IDM_RESET_CONTROL);
//    print_log("IDM RESET STATE OF USB2D_IDM_IDM_RESET_CONTROL= 0x%08X\n", reg_data);
    reg_data &= ~(1 << USB2D_IDM_IDM_RESET_CONTROL__RESET); 
    wr(USB2D_IDM_IDM_RESET_CONTROL, reg_data);

    reg_data = rd(USB2D_IDM_IDM_RESET_CONTROL);
//    print_log("IDM RESET FOR USB2D_IDM_IDM_RESET_CONTROL= 0x%08X\n", reg_data);
    
    ////////////////////////////////////////////
    udelay(1000);

    //Program USB Port 2 in device Mode
    reg_data = rd(CDRU_USBPHY2_HOST_DEV_SEL);
    reg_data &= ~(1 << CDRU_USBPHY2_HOST_DEV_SEL__PHY2_DEV_HOST_CTRL_SEL);
    wr(CDRU_USBPHY2_HOST_DEV_SEL,reg_data);
    
    
    //////////////////////////////////////////////
       
    return 0;
}
extern uint32_t USB_DEV_INIT(void);
int usb2_loopback_test(uint32_t usbh_port)
{
    uint32_t i, count=1000; 
    uint32_t reg_data, data;

    qTD *qTD_SetupStage;
    qTD *qTD_DataStage;
    qTD *qTD_StatusStage;
    setup_fmt *setup_fmt_get_desc;
    qH *qH_controlTransfer_get_descriptor;
    usbd_in_data_desc_t *usbd_in_data_desc;
    usbd_setup_data_desc_t *usbd_setup_data_desc;
    uint32_t usbd_dev_desc_0;
    uint32_t usbd_dev_desc_1;
    uint32_t usbd_dev_desc_2;
    uint32_t usbd_dev_desc_3;
    uint32_t usbh_recv_usbd_dev_desc_0;
    uint32_t usbh_recv_usbd_dev_desc_1;
    uint32_t usbh_recv_usbd_dev_desc_2;
    uint32_t usbh_recv_usbd_dev_desc_3;
    
    usb2d_prepare(); 
    
    if (usbh_port == 0) {
        if (0 != usb0h_prepare()) {
            return -1;  
        }
    } else {
        if (0 != usb1h_prepare()) {
            return -1;
        }
    }
            
     
    devTestInfo.trSize = 0;
    devTestInfo.bufferFill = 0;
    devTestInfo.bulkNumItr = 0;
    devTestInfo.bulkCurItr = 0;
    devTestInfo.done = 0;
    devTestInfo.usbdDescMem= 0x02030000;
    devTestInfo.usbdDescDataMem= 0x02031000;
    devTestInfo.usbdXferMem = 0x02032000;


    // Host Configuration
    if (usbh_port == 0) {
        if (0 != usb0h_check()) {
            return -1;
        }
    } else {
        if (0 != usb1h_check()) {
            return -1;
        }
    }

    
    // Device Configuration  
    if(USB_DEV_INIT()) {
        print_log("USB DEV INIT successful\n");
    } else {
        return -1;
    }
    
    //------------------------------------------------------------------
    // Device Configuration
    //------------------------------------------------------------------
    
    //do{
    //    reg_data = rd(USB2D_DEVINTR);
    //    print_log("waiting for bus reset to be completed USB2D_DEVINTR = 0x%08X\n", reg_data);
    //} while ((reg_data & (1 << USB2D_DEVINTR__UR)) == 0);
  
    // clear bus reset interrupt status
    wr(USB2D_DEVINTR, (1 << USB2D_DEVINTR__UR));
    
    i = 0;
    do{
    	udelay(1);
    	i++;
        reg_data = rd(USB2D_DEVINTR);
        print_log("waiting for sof on the bus USB2D_DEVINTR = 0x%08X\n", reg_data);
        if (i >= count) { printf("failed\n"); return -1; }
    } while ((reg_data & (1 << USB2D_DEVINTR__SOF)) == 0);
    wr(USB2D_DEVINTR, (1 << USB2D_DEVINTR__SOF));
  
    usbd_setup_data_desc = (usbd_setup_data_desc_t *) 0x02033000;
    usbd_setup_data_desc->buffer_status.BS = 0x0;
    usbd_setup_data_desc->buffer_status.RxSts = 0x3;

    print_log("usbd_setup_data_desc->buffer_status.RxSts = 0x%08X\n", usbd_setup_data_desc->buffer_status.RxSts);
    print_log("usbd_setup_data_desc->setup_data0 = 0x%08X\n", usbd_setup_data_desc->setup_data0);
    print_log("usbd_setup_data_desc->setup_data1 = 0x%08X\n", usbd_setup_data_desc->setup_data1);
   
    //update setup buffer pointer
    wr(USB2D_ENDPNT_OUT_SETUPBUF_0, (uint32_t)usbd_setup_data_desc);
    
    usbd_in_data_desc = (usbd_in_data_desc_t *) 0x02034000;
    usbd_in_data_desc->buffer_status.tx_bytes = 16;
    usbd_in_data_desc->buffer_status.L = 1;
    usbd_in_data_desc->buffer_status.TxSts = 3;
    usbd_in_data_desc->buffer_status.BS = 0;
    usbd_in_data_desc->buffer_pointer= 0x02035000;

    usbd_dev_desc_0 = 0x11223344;//rand();
    usbd_dev_desc_1 = 0x22334411;//rand();
    usbd_dev_desc_2 = 0x33441122;//rand();
    usbd_dev_desc_3 = 0x44112233;//rand();
    
    wr(0x02035000, usbd_dev_desc_0);
    wr(0x02035004, usbd_dev_desc_1);
    wr(0x02035008, usbd_dev_desc_2);
    wr(0x0203500C, usbd_dev_desc_3);

    print_log("Before xfer - usbd_in_data_desc->buffer_status.TxSts = %d\n", usbd_in_data_desc->buffer_status.TxSts);
    
    wr(USB2D_ENDPNT_IN_BUFFER_0, 0xFFFF);
    wr(USB2D_ENDPNT_IN_MAXPACKSIZE_0, 0xFFFF);
    
    data = rd(USB2D_DEVINTR);
    print_log("USB2D_DEVINTR = 0x%08X\n", data);

    //------------------------------------------------------------------
    // Host Configuration
    //------------------------------------------------------------------

    print_log("sizeof qTD0 = 0x%08X\n", sizeof(qTD));

    //FIXME : remove hard coding

    qTD_SetupStage  = (qTD *) 0x02036000;
    qTD_DataStage   = (qTD *) 0x02036020;
    qTD_StatusStage = (qTD *) 0x02036040;
    setup_fmt_get_desc = (setup_fmt *) 0x02037000;
    qH_controlTransfer_get_descriptor = (qH *) 0x02038000;

    setup_fmt_get_desc->bmRequestType = 0x80;
    setup_fmt_get_desc->bRequest = 0x06;
    setup_fmt_get_desc->wValue = 0x0100;
    setup_fmt_get_desc->wIndex = 0;
    setup_fmt_get_desc->wLength = 18;
   
    memset((void *) qTD_SetupStage, 0, sizeof(qTD));
    qTD_SetupStage->next_qTD_T = 0;
    qTD_SetupStage->next_qTD_ptr = (0x02036020 >> 5); //fixme
    qTD_SetupStage->altn_qTD_ptr = (0x02036000 >> 5); //fixme
    qTD_SetupStage->altn_qTD_T = 0;
    qTD_SetupStage->status = 0x80;
    qTD_SetupStage->pid_code = 2; //fixme
    qTD_SetupStage->cerr = 0;
    qTD_SetupStage->c_page = 0;
    qTD_SetupStage->ioc = 1;
    qTD_SetupStage->total_bytes = 8;
    qTD_SetupStage->dt = 0;
    //qTD_SetupStage->current_offset = 0;
    qTD_SetupStage->buffer_pointer_0 = (uint32_t) setup_fmt_get_desc;
    
    memset((void *) qTD_DataStage, 0, sizeof(qTD));
    qTD_DataStage->next_qTD_T = 0;
    qTD_DataStage->next_qTD_ptr = (0x02036040 >> 5); //fixme
    qTD_DataStage->altn_qTD_ptr = (0x02036040 >> 5); //fixme
    qTD_DataStage->altn_qTD_T = 0;
    qTD_DataStage->status = 0x80;
    qTD_DataStage->pid_code = 1; //fixme
    qTD_DataStage->cerr = 0;
    qTD_DataStage->c_page = 0;
    qTD_DataStage->ioc = 1;
    qTD_DataStage->total_bytes = setup_fmt_get_desc->wLength;
    qTD_DataStage->dt = 1;
    //qTD_DataStage->current_offset = 0;
    qTD_DataStage->buffer_pointer_0 = 0x02039000; //FIXME
    
    memset((void *) qTD_StatusStage, 0, sizeof(qTD));
    qTD_StatusStage->next_qTD_T = 1;
    qTD_StatusStage->next_qTD_ptr = 0;
    qTD_StatusStage->altn_qTD_ptr = (0x02036040 >> 5);
    qTD_StatusStage->altn_qTD_T = 0;
    qTD_StatusStage->status = 0x80;
    qTD_StatusStage->pid_code = 0; //FIXME
    qTD_StatusStage->cerr = 0;
    qTD_StatusStage->c_page = 0;
    qTD_StatusStage->ioc = 1;
    qTD_StatusStage->total_bytes = 0;
    qTD_StatusStage->dt = 1;
    //qTD_StatusStage->current_offset = 0;
    qTD_StatusStage->buffer_pointer_0 = 0x02039000; //FIXME
    
    memset((void *) qH_controlTransfer_get_descriptor, 0, sizeof(qH));
    qH_controlTransfer_get_descriptor->T = 0; //should always be 0 for asynchronous list
    qH_controlTransfer_get_descriptor->Typ = 1;
    qH_controlTransfer_get_descriptor->RES0 = 0;
    qH_controlTransfer_get_descriptor->QH_HLP = (((uint32_t) qH_controlTransfer_get_descriptor) >> 5);
    qH_controlTransfer_get_descriptor->device_address = 0;
    qH_controlTransfer_get_descriptor->I = 0;
    qH_controlTransfer_get_descriptor->EndPt = 0;
    qH_controlTransfer_get_descriptor->EPS = 2;
    qH_controlTransfer_get_descriptor->dtc = 1;
    qH_controlTransfer_get_descriptor->H = 1;
    qH_controlTransfer_get_descriptor->max_pkt_len = 64;
    qH_controlTransfer_get_descriptor->C = 0;
    qH_controlTransfer_get_descriptor->RL = 0;
    qH_controlTransfer_get_descriptor->uFrame_S_mask = 0;
    qH_controlTransfer_get_descriptor->uFrame_C_mask = 0;
    qH_controlTransfer_get_descriptor->Hub_Addr = 0;
    qH_controlTransfer_get_descriptor->Port_Number = 0;
    qH_controlTransfer_get_descriptor->Mult = 3;
    qH_controlTransfer_get_descriptor->RES1 = 0;
    qH_controlTransfer_get_descriptor->Current_qTD_ptr = 0;
    qH_controlTransfer_get_descriptor->current_qTD_cache.next_qTD_ptr = (((uint32_t) qTD_SetupStage) >> 5);
    
    wr(USBH_ASYNCLISTADDR, (uint32_t)qH_controlTransfer_get_descriptor);
    wr(USBH_USBCMD, 0x10021);
    
    i = 0;
    do {
    	udelay(1);
    	i++;
        reg_data = rd(USBH_USBSTS);
        print_log("USBH_USBSTS = 0x%08X\n", reg_data);
        if (i >= count) { printf("failed\n"); return -1; }
    } while ((reg_data & (1 << USBH_USBSTS__Asynchronous_Schedule_Status)) == 0);

    //------------------------------------------------------------------
    // Device Configuration
    //------------------------------------------------------------------
    
    // update IN data descriptor pointer
    wr(USB2D_ENDPNT_IN_DATADESCR_0, (uint32_t)usbd_in_data_desc);
    
    print_log("USB2D_ENDPNT_IN_CTRL_0 = 0x%08X\n", rd(USB2D_ENDPNT_IN_CTRL_0));
    data = rd(USB2D_ENDPNT_IN_CTRL_0);
    data |= (1 << USB2D_ENDPNT_IN_CTRL_0__CNAK);
    data |= (1 << USB2D_ENDPNT_IN_CTRL_0__P);
    wr(USB2D_ENDPNT_IN_CTRL_0, data);
    print_log("USB2D_ENDPNT_IN_CTRL_0 = 0x%08X\n", rd(USB2D_ENDPNT_IN_CTRL_0));

    // Host Configuration + Data intergrity check
    
    i = 0;
    do {
    	udelay(1);
    	i++;
        reg_data = rd(USBH_USBSTS);
        print_log("Waiting for USB transfer to complete --> USBH_USBSTS = 0x%08X\n", reg_data);
        if (i >= count) { printf("failed\n"); return -1; }
    } while ((reg_data & (1 << USBH_USBSTS__USB_Interrupt_USBINT)) == 0);
        
    //wr(USBH_USBCMD, 0x10001);
        
    usbh_recv_usbd_dev_desc_0 = rd(0x02039000);
    usbh_recv_usbd_dev_desc_1 = rd(0x02039004);
    usbh_recv_usbd_dev_desc_2 = rd(0x02039008);
    usbh_recv_usbd_dev_desc_3 = rd(0x0203900C);

    print_log("Device descriptor recevied by Host     = 0x%08X 0x%08X 0x%08X 0x%08X\n", usbh_recv_usbd_dev_desc_3, usbh_recv_usbd_dev_desc_2, usbh_recv_usbd_dev_desc_1, usbh_recv_usbd_dev_desc_0);
    print_log("Device descriptor configured in Device = 0x%08X 0x%08X 0x%08X 0x%08X\n", usbd_dev_desc_3, usbd_dev_desc_2, usbd_dev_desc_1, usbd_dev_desc_0);

    if(usbh_recv_usbd_dev_desc_0 != usbd_dev_desc_0) {
        error_log("Device descriptor word0 failed - Expected = 0x%08X, Actual = 0x%08X\n", usbd_dev_desc_0, usbh_recv_usbd_dev_desc_0);
    }
    if(usbh_recv_usbd_dev_desc_1 != usbd_dev_desc_1) {
        error_log("Device descriptor word0 failed - Expected = 0x%08X, Actual = 0x%08X\n", usbd_dev_desc_1, usbh_recv_usbd_dev_desc_1);
    }
    if(usbh_recv_usbd_dev_desc_2 != usbd_dev_desc_2) {
        error_log("Device descriptor word0 failed - Expected = 0x%08X, Actual = 0x%08X\n", usbd_dev_desc_2, usbh_recv_usbd_dev_desc_2);
    }
    if(usbh_recv_usbd_dev_desc_3 != usbd_dev_desc_3) {
        error_log("Device descriptor word0 failed - Expected = 0x%08X, Actual = 0x%08X\n", usbd_dev_desc_3, usbh_recv_usbd_dev_desc_3);
    }
    
    print_log("After xfer - usbd_in_data_desc->buffer_status.TxSts = %d\n", usbd_in_data_desc->buffer_status.TxSts);
    print_log("usbd_setup_data_desc->buffer_status.RxSts = 0x%08X\n", usbd_setup_data_desc->buffer_status.RxSts);
    print_log("usbd_setup_data_desc->setup_data0 = 0x%08X\n", usbd_setup_data_desc->setup_data0);
    print_log("usbd_setup_data_desc->setup_data1 = 0x%08X\n", usbd_setup_data_desc->setup_data1);
    print_log("USB2D_ENDPNT_IN_CTRL_0 = 0x%08X\n", rd(USB2D_ENDPNT_IN_CTRL_0));
    print_log("USB2D_DEVINTR = 0x%08X\n", rd(USB2D_DEVINTR));
 
    return 0;
}

#endif
#endif

