/*
 * $Copyright Open Broadcom Corporation$
 */


#include <common.h>
#include <post.h>


#if CONFIG_POST & CONFIG_SYS_POST_USB20

/* Size in byte */
#define USB_DATA_BUF_SIZE	1024*1024

#if (defined(CONFIG_NS_PLUS))
#define USB_DATA_BUF_ADDR   0x70000000
#define USB_DATA_BUF_ADDR_STR  "70000000"
#define USB_DST_DATA_BUF_ADDR 0x71000000
#define USB_DST_DATA_BUF_ADDR_STR "71000000"
#elif (defined(CONFIG_CYGNUS))
#define USB_DATA_BUF_ADDR   0x70000000
#define USB_DATA_BUF_ADDR_STR  "70000000"
#define USB_DST_DATA_BUF_ADDR 0x71000000
#define USB_DST_DATA_BUF_ADDR_STR "71000000"
#else
#define USB_DATA_BUF_ADDR   0x89000000
#define USB_DATA_BUF_ADDR_STR  "89000000"
#define USB_DST_DATA_BUF_ADDR 0x8A000000
#define USB_DST_DATA_BUF_ADDR_STR "8A000000"
#endif

extern int do_usb(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);

#if (defined(CONFIG_CYGNUS))
/*
CRMU_CHIP_IO_PAD_CONTROL  0x0301d0bc = 0;  //Global IOMUX

GP_OUT_EN  0x03024808 [2:3] = [1:1];    
GP_DATA_OUT 0x03024804[2:3] = [0:0];

CRMU_IOMUX_CTRL0         0x0301d0c8 [30:28] = 0;   //gpio7 function selection
ChipcommonG_GP_OUT_EN    0x1800a008 [7]=1          //gpio7 out enable
ChipcommonG_GP_DATA_OUT  0x1800a004 [7]=1          //pull high gpio7 to power on USB p2

*/
static void usb2_port2_power_supply(int on)
{
	volatile unsigned int *regaddr = 0;
	
	regaddr = 0x03024808;
	* regaddr = (* regaddr) | 0xC;
	regaddr = 0x03024804;
	* regaddr = (* regaddr) & (~0xC);
	
	regaddr = 0x0301d0c8;
	* regaddr = (* regaddr) & (~(0x7<<28));
	regaddr = 0x1800a008;
	* regaddr = (* regaddr) | (1 << 7);
		
	if (on){
		regaddr = 0x1800a004;
		* regaddr = (* regaddr) | (1 << 7);
	} else {		
		regaddr = 0x1800a004;
		* regaddr = (* regaddr) & (~(1 << 7));
	}	
}

/*
  0x180a5408  
*/

static void usb2_port2_power_supply_voip(int on)
{
	volatile unsigned int *regaddr = 0;
	
	/* set gpio116 to output*/
	regaddr = 0x180a5408;
	* regaddr = (* regaddr) | (1 << (116-24-32*2));	
		
	if (!on){
	    /* pulldown gpio116*/
	    regaddr = 0x180a5404;
	    * regaddr = (* regaddr) & (~(1 << (116-24-32*2)));
	} else {		
	    /* pullup gpio116*/
	    regaddr = 0x180a5404;
	    * regaddr = (* regaddr) | (1 << (116-24-32*2));
	}	
}

#endif

int USB20_post_test (int flags)
{
    char argv_buf[7*10];
    char * usb_argv[7];
    int  i, rc = 0;
    char ch;
#if (defined(CONFIG_CYGNUS))
    int respin = 0;
#endif     

#if (defined(CONFIG_NS_PLUS))
    if ( post_get_board_diags_type() & SVK_BOARDS )
	{

		if ( (flags & POST_AUTO) !=  POST_AUTO )
		{
			post_log("\nPlease set strap pin: A2=0, A3=0x3, A4=0xf, A5=0x4f, A6=0xf3", 0);
			post_getConfirmation("\nReady? (Y/N)");
		}
		else
		{
			post_set_strappins("A2 0%A3 3%A4 f%A5 5f%A6 f3%");
		}
	}
#elif (defined(CONFIG_CYGNUS))
    /*set strap pins*/
    
#else
    if ( (flags & POST_AUTO) !=  POST_AUTO )
    {
        post_log("\nPlease set strap pin: A2=0, A3=0x3, A4=0xf, A5=0x4f, A6=0xf3", 0);
        post_getConfirmation("\nReady? (Y/N)");
    }
    else
    {
        post_set_strappins("A2 0%A3 3%A4 f%A5 5f%A6 f3%");
    }
#endif

//loopback test
#if defined(CONFIG_CYGNUS)
extern int usb2_loopback_test(uint32_t usbh_port);
extern int usb2h_init(void);
extern int usb2d_prepare(void);
extern void iproc_gpio116_iomux(int op);    
    
    post_log("\nis there a power supplying respin for usb port2 (Y/N): ");
    do 
    {
        ch = (char)serial_getc();
    } while ((ch !='y') && (ch !='Y') && (ch !='N') && (ch !='n'));
    if ((ch == 'y') || ( ch =='Y')) {
    	respin = 1;
    	if (post_get_board_diags_type() == BCM911360K) {
    		iproc_gpio116_iomux(1);
    		usb2_port2_power_supply_voip(1);
    	} else {
            usb2_port2_power_supply(1);
        }
    }
    
    post_log("\nwill you select outer loopback test (Y/N): ");
    do 
    {
        ch = (char)serial_getc();
    } while ((ch !='y') && (ch !='Y') && (ch !='N') && (ch !='n'));

    if ((ch == 'y') || ( ch =='Y')) {
        if (post_get_board_diags_type() == BCM911360K) {
            usb2d_prepare();
            mdelay(2000);
        }
        rc = usb2_loopback_test(0);
        if (rc  == 0) {
        	if (post_get_board_diags_type() == BCM911360K) {
        	    usb2h_init(); /*restore to host mode*/
        	}
        } else {       	
            rc = usb2_loopback_test(1);
        }
        if (respin && (post_get_board_diags_type() == BCM911360K)) {
    	    usb2_port2_power_supply_voip(0);
    	    iproc_gpio116_iomux(0);
        }
        return rc;
    }   

#endif
    

    memset(&argv_buf, 0, 7*10);
    for(i=0; i<7; i++)
	usb_argv[i] = &argv_buf[i*10];

    /* USB Reset */
    post_log("\n\nUSB reset - Reset/Enumerate USB device\n");
    strcpy(usb_argv[0], "usb");
    strcpy(usb_argv[1], "reset");
    rc = do_usb(0, 0, 2, usb_argv);
	if(rc != 0) goto failed;

    /* USB Tree */
    post_log("\n\nUSB tree - Display USB device\n");
    strcpy(usb_argv[0], "usb");
    strcpy(usb_argv[1], "tree");
    rc = do_usb(0, 0, 2, usb_argv);
	if(rc != 0) goto failed;

    /* USB Info */
    post_log("\n\nUSB info - Display USB device info\n");
    strcpy(usb_argv[0], "usb");
    strcpy(usb_argv[1], "info");
    rc = do_usb(0, 0, 2, usb_argv);
	if(rc != 0) goto failed;

    /* USB Storage */
    post_log("\n\nUSB storage - Display USB disk info\n");
    strcpy(usb_argv[0], "usb");
    strcpy(usb_argv[1], "storage");
    rc = do_usb(0, 0, 2, usb_argv);
	if(rc != 0) goto failed;

    /* USB read */
    post_log("\n\nUSB raw data read, 1Mbyte\n");
    post_log("USB read ddr_addr=0x%x blk_addr=0x8000 cnt=2048\n",USB_DATA_BUF_ADDR);
    strcpy(usb_argv[0], "usb");
    strcpy(usb_argv[1], "read");
    strcpy(usb_argv[2], USB_DATA_BUF_ADDR_STR );
    strcpy(usb_argv[3], "8000");
    strcpy(usb_argv[4], "800");
    rc = do_usb(0, 0, 5, usb_argv);
	if(rc != 0) goto failed;

#if defined(CONFIG_CYGNUS)
    i=USB_DATA_BUF_SIZE/4;
#else
    /* Memory Fill */
    post_log("\n\nFill memory with incremental data, 1Mbyte, addr=0x%x\n",USB_DATA_BUF_ADDR);
    for(i=0; i<USB_DATA_BUF_SIZE/4; i++)
    {
		*(unsigned long *)(USB_DATA_BUF_ADDR + i*4) = i; 
	}

    /* Memory Display */
    post_log("\n\Addr 0x%x - Display filled memory\n",USB_DATA_BUF_ADDR);
    strcpy(usb_argv[0], "md");
    strcpy(usb_argv[1], USB_DATA_BUF_ADDR_STR);
    do_mem_md(0, 0, 2, usb_argv);

    /* USB write */
    post_log("\n\nUSB raw data write, 1Mbyte\n");
    post_log("USB write ddr_addr=0x%x blk_addr=0x8000 cnt=2048\n",USB_DATA_BUF_ADDR);
    strcpy(usb_argv[0], "usb");
    strcpy(usb_argv[1], "write");
    strcpy(usb_argv[2], USB_DATA_BUF_ADDR_STR);
    strcpy(usb_argv[3], "8000");
    strcpy(usb_argv[4], "800");
    rc = do_usb(0, 0, 5, usb_argv);
	if(rc != 0) goto failed;

    /* USB read back and compare */
    post_log("\n\nUSB read back and compare\n");
    post_log("USB read ddr_addr=0x%x blk_addr=0x8000 cnt=2048\n",USB_DST_DATA_BUF_ADDR);
    strcpy(usb_argv[0], "usb");
    strcpy(usb_argv[1], "read");
    strcpy(usb_argv[2], USB_DST_DATA_BUF_ADDR_STR);
    strcpy(usb_argv[3], "8000");
    strcpy(usb_argv[4], "800");
    rc = do_usb(0, 0, 5, usb_argv);
	if(rc != 0) goto failed;

    for(i=0; i<USB_DATA_BUF_SIZE/4; i++)
    {
		if(*(unsigned long *)(USB_DST_DATA_BUF_ADDR + i*4) != i)
		{
			post_log("\n !! Compare failed at address <0x%08x> = 0x%08x\n", 
						USB_DATA_BUF_ADDR + i*4, *(unsigned long *)(USB_DST_DATA_BUF_ADDR + i*4) );
			break;
		}
	}
#endif
    if(i==USB_DATA_BUF_SIZE/4)
	{
    	post_log("\n\nRead back and compare OK\n");
    	post_log("\n\nUSB test done\n");
	    rc = 0;
	}
	else
	{
failed:
    	post_log("\n\nUSB test failed\n");
		rc = -1;
	}
#if defined(CONFIG_CYGNUS)
    if (respin) {
    	usb2_port2_power_supply_voip(0);
    	iproc_gpio116_iomux(0);
    }
#endif 
    return rc;
}
#endif /* CONFIG_POST & CONFIG_SYS_POST_USB20 */
