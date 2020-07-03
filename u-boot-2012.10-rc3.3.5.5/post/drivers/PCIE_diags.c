/*
 * $Copyright Open Broadcom Corporation$
 */

#include <common.h>
#include <post.h>

#if CONFIG_POST & CONFIG_SYS_POST_PCIE
#include <ns16550.h>
#include "asm/arch/socregs.h"
#if defined(CONFIG_CYGNUS)
#include "pcie_iproc.h"
#endif

extern void pci_iproc_set_port(int port);
extern u32 pci_iproc_get_reg_base(int port);
extern u32 pci_iproc_get_pci_addr(int port);
extern int iproc_pcie_get_link_status(int port);
extern int pci_iproc_port_mode_ep(void);
extern void iproc_pcie_iomux(int op);

#define UART_MEM_BASE	(0x1000)

#define PCIE_UART_THR	0
#define PCIE_UART_RHR	0
#define PCIE_UART_IER	1
#define PCIE_UART_FCR	2
#define PCIE_UART_ISR	2
#define PCIE_UART_LCR	3
#define PCIE_UART_MCR	4
#define PCIE_UART_LSR	5
#define PCIE_UART_MSR	6
#define PCIE_UART_SPR	7
#define PCIE_UART_DLL	0
#define PCIE_UART_DLM	1

#if defined(CONFIG_CYGNUS)
#define IPROC_CCB_MDIO_CTRL              (0x18002000)
#define IPROC_CCB_MDIO_DATA              (0x18002004)
#else
#define IPROC_CCB_MDIO_CTRL              (0x18032000)
#define IPROC_CCB_MDIO_DATA              (0x18032004)
#endif
/* Configure for MII needed for usb3, has to be done before usb2 */
#define MII_CTRL_INIT_VAL     0x9a
#define MII_MGMT_CMD_DATA_VAL 0x587e8000
#define MII_MGMT_CMD_DATA_VAL1  0x582EC000
#define MII_MGMT_CMD_DATA_VAL4  0x582E8000



int PCIE_post_enable_phy(void)
{
		__raw_writel(MII_CTRL_INIT_VAL, IPROC_CCB_MDIO_CTRL);
  		mdelay(10);
  		__raw_writel(MII_MGMT_CMD_DATA_VAL, IPROC_CCB_MDIO_DATA);
   		mdelay(10);
   		__raw_writel(MII_MGMT_CMD_DATA_VAL1, IPROC_CCB_MDIO_DATA);
  		mdelay(10);
  		__raw_writel(MII_MGMT_CMD_DATA_VAL4, IPROC_CCB_MDIO_DATA);
  		mdelay(10);
		return 0;
}

#if defined(CONFIG_CYGNUS)
extern struct soc_pcie_port soc_pcie_ports[];
extern struct pci_controller pci_hoses[];
extern int soc_pcie_check_link(int port);
extern void pci_bus0_read_config_dword (struct soc_pcie_port *pcie_port, pci_dev_t dev, int where, unsigned long *val);
extern void pci_bus0_write_config_dword (struct soc_pcie_port *pcie_port, pci_dev_t dev, int where, unsigned long val);
#endif
extern int do_pci (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
extern int do_mem_md ( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int PCIE_post_test (int flags)
{
#ifdef CONFIG_CMD_PCI
        char argv_buf[7*10];
        char * cmd_argv[7];
        int  i;
		unsigned long uart_base;
		unsigned char c = 0x30;
		unsigned char tmp1, tmp2;
	
		u32 slot = 0,index=0;
		u32  out_pci_addr;

#if (defined(CONFIG_NS_PLUS))
        if ( post_get_board_diags_type() & SVK_BOARDS )
		{

			if ( (flags & POST_AUTO) !=  POST_AUTO )
			{
				post_log("\nPlease set strap pin:A1=F4 A2=0, A3=0x3, A4=0xf, A5=0x4f, A6=0xf3", 0);
				post_log("\nIf using PCIE 2, please power cycle after changing A1 to F4 ");
				post_getConfirmation("\nReady? (Y/N)");
			}
			else
			{
				post_set_strappins("A1 F4%A2 0%A3 3%A4 f%A5 5f%A6 f3%");
			}
		}
		else
		{
			return (runPCIETestForHR_ER());
		}
#elif (defined(CONFIG_NORTHSTAR))
        if ( (flags & POST_AUTO) !=  POST_AUTO )
        {
            post_log("\nPlease set strap pin:A1=F4 A2=0, A3=0x3, A4=0xf, A5=0x4f, A6=0xf3", 0);
            post_log("\nIf using PCIE 2, please power cycle after changing A1 to F4 ");
            post_getConfirmation("\nReady? (Y/N)");
        }
        else
        {
            post_set_strappins("A1 F4%A2 0%A3 3%A4 f%A5 5f%A6 f3%");
        }
#elif (defined(CONFIG_CYGNUS))
        iproc_pcie_iomux(1);
#endif

        //Auto test still defaults to port 1
        //Manual test - user selectable
        if ( (flags & POST_AUTO) !=  POST_AUTO )
        {
#if ((defined(CONFIG_NS_PLUS)) || (defined(CONFIG_NORTHSTAR)))
			post_log("\n\nSelect PCI slot # to test:0 - PCIe0; 1 - PCIe1; 2 - PCIe2 \n");
#elif ((defined (CONFIG_HELIX4)) || (defined(CONFIG_KATANA2)) || (defined(CONFIG_CYGNUS)))
			post_log("\n\nSelect PCI slot # to test:0 - PCIe0; 1 - PCIe1\n");
#endif
#if ((defined(CONFIG_NS_PLUS)) || (defined(CONFIG_NORTHSTAR)))
			slot = post_getUserInput("Slot #? (0/1/2) : ");
#elif ((defined (CONFIG_HELIX4)) || (defined(CONFIG_KATANA2)))
			slot = post_getUserInput("Slot #? (0/1) : ");
#elif (defined(CONFIG_HURRICANE2))
			slot = 0;
#elif (defined(CONFIG_CYGNUS))
            if (pci_iproc_port_mode_ep()) {
            	slot = 1;
            	printf("pcie port 0 in EP mode\n");
            } else {
	            slot = post_getUserInput("Slot #? (0/1) : ");
            }
#endif    
			switch(slot)
			{
				case 0:
					index = 0;
					break;
				case 1:
					index = 1;
					break;
				case 2:
                    post_log("\n\nSet PCIe2 PHY through MDIO\n");  
                    PCIE_post_enable_phy();
					index = 2;
					break;
				default:
					index = 0;
					break;
			}
            
			pci_iproc_set_port(index);
			
        }


#if defined(CONFIG_CYGNUS)
		if (pci_iproc_port_mode_ep() && (soc_pcie_check_link(1) == 0)) 
		{
			u32 val,org;  
		
			pci_bus0_read_config_dword(&soc_pcie_ports[0], 0, 0x10,(unsigned long *)&org);
			printf("orginsal PCIe0 config reg val: 0x%x\n", org);
			val = 0x2000000c;
			pci_bus0_write_config_dword(&soc_pcie_ports[0], 0, 0x10, val);
			printf("changed  PCIe0 config reg val: 0x%x\n", val);
			//udelay(10);
			iproc_pcie_rd_conf_dword(&pci_hoses[1], 0, 0x10, &val);
			printf("get PCIe0 config reg val: 0x%x from PCIe1\n", val);
			
			pci_bus0_write_config_dword(&soc_pcie_ports[0], 0, 0x10, org);
			printf("restore PCIe0 config reg val: 0x%x\n", org);
			
			
			if (val == 0x2000000c) {
				post_log("\n\nPCIE test done\n");
				return 0;
			} else {
				post_log("\n\nPCIE test failed\n");
				return -1;
			}
		}

        if ((index == 0) && pci_iproc_port_mode_ep())
        {
        	index = 1;
        	slot = 1;
        }
#endif        
        //reg_base     = pci_iproc_get_reg_base(index);
        out_pci_addr = pci_iproc_get_pci_addr(index);

        uart_base = out_pci_addr + UART_MEM_BASE;
        memset(&argv_buf, 0, 7*10);
        for(i=0; i<7; i++)
		cmd_argv[i] = &argv_buf[i*10];


        /* PCI Enum */
        printf("\n\nPCIE Enumeration\n");
        strcpy(cmd_argv[0], "pci");
        strcpy(cmd_argv[1], "enum");
        do_pci(0, 0, 2, cmd_argv);
		udelay(2000*100);

		if (iproc_pcie_get_link_status(index) != 0)
		{
			goto failed;
		}

        printf("\n\nPCIE header display, BDF<00:00:00>\n");
        strcpy(cmd_argv[0], "pci");
        strcpy(cmd_argv[1], "header");
        strcpy(cmd_argv[2], "0.0.0");
        do_pci(0, 0, 3, cmd_argv);
		udelay(2000*100);


		printf("\n\nPCIE EP memory raw access, enable port0/1\n");
		printf("Device ID: 0x%08x\n", __raw_readl(out_pci_addr) );
		__raw_writel(0x3, out_pci_addr+4);
		printf("Port enabled: 0x%08x\n", __raw_readl(out_pci_addr+4) );
		udelay(2000);


		/* PCI memory dump */
        printf("\n\nPCIE memory dump\n");
        strcpy(cmd_argv[0], "md");
        sprintf(cmd_argv[1], "0x%x ", out_pci_addr);
        do_mem_md(0, 0, 2, cmd_argv);

		/* Serial init */		
		{
			/* Force 450 mode */
			__raw_writeb(0x00, 			uart_base + PCIE_UART_FCR);	
			__raw_writeb(UART_LCR_BKSE | UART_LCR_8N1, 	uart_base + PCIE_UART_LCR);	

			/* Set Baud 115K */
			__raw_writeb(0x22, uart_base + PCIE_UART_DLL);
			__raw_writeb(0,    uart_base + PCIE_UART_DLM);
			
			tmp1 = __raw_readb(uart_base + PCIE_UART_DLM);
			tmp2 = __raw_readb(uart_base + PCIE_UART_DLL);
			printf("\nRead out Baud DLM=0x%02x, DLL=0x%02x\n\n", tmp1, tmp2);
			if((tmp1!=0) || (tmp2!=0x22))
				goto failed;
		
			__raw_writeb(UART_LCR_8N1, 	uart_base + PCIE_UART_LCR);	

		    udelay(1000);
			__raw_writeb(0x00, 			uart_base + PCIE_UART_IER);	
			__raw_writeb((UART_FCR_RXSR | UART_FCR_TXSR),   uart_base + PCIE_UART_FCR);
			__raw_writeb((UART_MCR_DTR | UART_MCR_RTS),     uart_base + PCIE_UART_MCR);	

            udelay(1000);
			printf("\nRead out UART Regs <0x%08lx>=0x%08x, <0x%08lx>=0x%08x\n\n", 
						uart_base, 		__raw_readl(uart_base),
						uart_base + 4, 	__raw_readl(uart_base + 4) );

			if ( flags & POST_AUTO)
			{

#if (defined(CONFIG_NORTHSTAR) || defined(CONFIG_NS_PLUS))

			    __raw_writeb('Y', uart_base + PCIE_UART_THR);
			    mdelay(100);
				char ch;

			   // while( _serial_tstc(2) == 0  ){}

                if (_serial_tstc(2) != 0 )
                {
                    ch = (char)_serial_getc(2);
                    if ( ('y' == ch) || ('Y'==ch ))
                    {
                        post_log("\nReceived char on console 2\n PCIE test passed");
                        return 0;
                    }
                }
                post_log("\n PCIE test failed");
                return -1;
#else
                printf("\nUART setting: 8bit data, NoneParity, 1bit stop, 115K baud rate\n");
                printf("\nStart to output 2K chars....\n\n");
                /* Send char out */
                for(i=0; i<2000; i++)
                {
                    if(c++ > 0x7a) c = 0x30;
                    while ((__raw_readb(uart_base + PCIE_UART_LSR) & UART_LSR_THRE) == 0);
                    __raw_writeb(c, uart_base + PCIE_UART_THR);
                    udelay(1000*10);
                }
#endif
			}
			else
			{
	            printf("\nUART setting: 8bit data, NoneParity, 1bit stop, 115K baud rate\n");
	            printf("\nStart to output 2K chars....\n\n");
                /* Send char out */
                for(i=0; i<2000; i++)
                {
                    if(c++ > 0x7a) c = 0x30;
                    while ((__raw_readb(uart_base + PCIE_UART_LSR) & UART_LSR_THRE) == 0);
                    __raw_writeb(c, uart_base + PCIE_UART_THR);
                    udelay(1000*10);
                }
			}	
		}
#endif /* CONFIG_CMD_PCI */

    	post_log("\n\nPCIE test done\n");
	    return 0;

failed:
    	post_log("\n\nPCIE test failed\n");
		return -1;
}
#if (defined(CONFIG_NS_PLUS))
#define NUM_PORT 2
int runPCIETestForHR_ER ( void )
{
    char argv_buf[7*10];
    char * cmd_argv[7];
	unsigned long uart_base;
	char ch;
	u32 slot,i,index=1;
	u32 ret = 0;
	int errorCode[NUM_PORT]= {0};


	for ( index = 0; index < NUM_PORT; index++ )
	{		
        pci_iproc_set_port(index);

		uart_base = pci_iproc_get_pci_addr(index) + UART_MEM_BASE;
		memset(&argv_buf, 0, 7*10);
		for(i=0; i<7; i++)
		cmd_argv[i] = &argv_buf[i*10];


		/* PCI Enum */
		post_log("\n\nPCIE Enumeration\n");
		strcpy(cmd_argv[0], "pci");
		strcpy(cmd_argv[1], "enum");
		do_pci(0, 0, 2, cmd_argv);
		udelay(2000*100);

		if (iproc_pcie_get_link_status(index) == 0)
		{

            if ( (post_get_board_diags_type() == BCM958622HR) && ( 0 == index ) )
			{
				dumpPCIEDeviceOnSwitch(index);
			}
			else
			{
                    
				post_log("\n\nPCIE header display, BDF<01:00:00>\n");
				strcpy(cmd_argv[0], "pci");
				strcpy(cmd_argv[1], "header");
				strcpy(cmd_argv[2], "1.0.0");
				do_pci(0, 0, 3, cmd_argv);
				udelay(2000*100);


				/* PCI memory dump */
				post_log("\n\nPCIE memory dump\n");
				strcpy(cmd_argv[0], "md");
				sprintf(cmd_argv[1], "0x%x ", pci_iproc_get_pci_addr(index));
				do_mem_md(0, 0, 2, cmd_argv);
			 }
		}
		else
		{
			ret = -1;

			errorCode[index] = -1;
		}
	}

	for ( index = 0; index < NUM_PORT; index++ )
	{
		if(-1 == errorCode[index])
		{
			post_log("\nPort %d failed\n", index);
		}
		else
		{
			post_log("\nPort %d passed\n", index);
		}
	}
    return ret;
}
#endif
#endif /* CONFIG_POST & CONFIG_SYS_POST_PCIE */
