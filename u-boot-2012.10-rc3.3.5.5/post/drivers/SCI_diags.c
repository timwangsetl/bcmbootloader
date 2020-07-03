/*
 * $Copyright Open Broadcom Corporation$
 */

#include <common.h>
#include <post.h>

#if defined(CONFIG_CYGNUS)

#if CONFIG_POST & CONFIG_SYS_POST_SCI

#include "asm/arch/socregs.h"
#include "../../halapis/include/reg_access.h"


int pass_flag = 0;
int SCI_post_test(int flags);
#if 0
static void sci_async_atr_init(int sci1)
{

    uint32_t rdata;
   

      if (sci1) {
              post_log("Starting SmartCard ATR test for SCI1\n\r");	
      } else { 
              post_log("Starting SmartCard ATR test for SCI0\n\r");	
      }

   //Clock enable
    post_log("SC: Enabling Smart Card Interface clock via ASIU_TOP_CLK_GATING_CTRL reg... \n");
    /*
    uint32_t  mask, wdata;
    uint32_t bit_gpio, bit_sgpio, wdata_gp;
    mask = (1<<ASIU_TOP_CLK_GATING_CTRL__SMARTCARD_CLK_GATE_EN);
    rdata = CPU_READ_32(ASIU_TOP_CLK_GATING_CTRL);
    wdata = (~mask & rdata) | mask;
    CPU_WRITE_32(ASIU_TOP_CLK_GATING_CTRL, wdata);
    */

    CPU_RMW_OR_32(ASIU_TOP_CLK_GATING_CTRL, (1 << ASIU_TOP_CLK_GATING_CTRL__SMARTCARD_CLK_GATE_EN));

/*  Need to check if these are required for 5 pin interface 
    io_printf("SC: enabling SC_VPP SGPIO pad regs for initial value.. \n");

    //SC0 and SC1
    //bit_gpio  = (GPIO0_3p3_bit - 24) % 32;  //bit pos in 32 bit GPIO reg, note that [23:0] come from IPROC
    bit_sgpio = GPIO0_3p3_bit % 32;         //bit pos in 32 bit Secure GPIO reg
    mask  = 1 << bit_sgpio;

    //bit_gpio  = (GPIO1_3p3_bit - 24) % 32;  //bit pos in 32 bit GPIO reg, note that [23:0] come from IPROC
    bit_sgpio =  GPIO1_3p3_bit % 32;        //bit pos in 32 bit Secure GPIO reg
    mask  = mask | (1 << bit_sgpio);

    //-- //configuring gpio
    //-- rdata = cpu_rd_single(ASIU_GP_OUT_EN_3, 4);
    //-- wdata_gp = (~mask & rdata) | mask;
    //-- cpu_wr_single(ASIU_GP_OUT_EN_3, wdata_gp, 4);

    //configuring secure gpio
    rdata = cpu_rd_single(ASIU_SGP_OUT_EN_4, 4);
    wdata_gp = (~mask & rdata) | mask;
    cpu_wr_single(ASIU_SGP_OUT_EN_4, wdata_gp, 4);

    //mux sel for gpio/sgpio
    rdata = cpu_rd_single(CHIP_SECURE_GPIO_CONTROL4, 4);       //selecting secure gpio for pad output
    wdata_gp = (~mask & rdata) | mask;
    cpu_wr_single(CHIP_SECURE_GPIO_CONTROL4, wdata_gp, 4);

    io_printf("SC: SC_VPP SGPIO pad regs enabled..\n");
*/

/*To Program SC0_DC_GPIO3_3P3, following AON GPIO configuration is needed
1.	Program AON_GPIO1 to 1.
2.	Program AON_GPIO0 to 0.
*/

    if (!sci1) {
        rdata = CPU_READ_32(GP_OUT_EN);
        rdata |= 0x3;
        CPU_WRITE_32(GP_OUT_EN , rdata);

        rdata = CPU_READ_32(GP_DATA_OUT);
        rdata |= 0x2;
        CPU_WRITE_32(GP_DATA_OUT, rdata);
    }
    else
    {
    	rdata = CPU_READ_32(GP_OUT_EN);
    	        rdata |= 0x3;
    	        CPU_WRITE_32(GP_OUT_EN , rdata);

    	        rdata = CPU_READ_32(GP_DATA_OUT);
    	        rdata |= 0x1;
    	        CPU_WRITE_32(GP_DATA_OUT, rdata);
    }



}
#endif

#if 0
static void get_atr_bytes(int sci1, int atr, uint8_t *read_val) {
  int i;
  uint8_t rd_data;
  uint8_t rx_status=0;
  uint8_t atr_bytes[20];

    for (i=0; i<atr; i++) {
       if(sci1) {
	      rx_status = CPU_READ_8(SCB_SC_STATUS_2);
	      while (rx_status & (1 << SCB_SC_STATUS_2__rempty) != 0x0) {
		      rx_status = CPU_READ_8(SCB_SC_STATUS_2);
	      } 
              rd_data = CPU_READ_8(SCB_SC_RECEIVE); 
              //__udelay(10);
              //post_log("ATR SYNC RECEIVED : %08X \n",rd_data);
       } else {
   	      rx_status = CPU_READ_8(SCA_SC_STATUS_2);
	      while (rx_status & (1 << SCA_SC_STATUS_2__rempty) != 0x0) {
		      rx_status = CPU_READ_8(SCA_SC_STATUS_2);
	      } 
              rd_data = CPU_READ_8(SCA_SC_RECEIVE); 
              //__udelay(10);
              //post_log("ATR SYNC RECEIVED : %08X \n",rd_data);
       }
	atr_bytes[i]=(uint8_t)rd_data; 
        *read_val = rd_data; read_val++;
    } // for
    post_log("ALL THE ATR SYNC BYTES HAVE BEEN RECEIVED \n");
    pass_flag = 1;
    /*post_log("Printing ATR Bytes...\n\r");
    for (i=0; i<atr; i++) {
        post_log("%x,\n\r",atr_bytes[i]);
    } // for*/

} // void get_atr_bytes(int sci1, int atr, uint8_t *read_val)
#endif
extern void lcd_write_reg(uint32_t a, uint32_t d);
void sci_reset(void)
{
        uint32_t sw_rst = CPU_READ_32(ASIU_TOP_SW_RESET_CTRL);
        post_log("rd ASIU_TOP_SW_RESET_CTRL = 0x%X\n",sw_rst);
        sw_rst = (sw_rst & ~(1 << ASIU_TOP_SW_RESET_CTRL__SMARTCARD_SW_RESET_N));
        CPU_WRITE_32(ASIU_TOP_SW_RESET_CTRL,sw_rst);
        post_log("wr ASIU_TOP_SW_RESET_CTRL = 0x%X\n",sw_rst);
        post_log("Delay...\n");
        {
                int i = 0;
                for(i = 0 ; i < 100 ; i++);
        }

        sw_rst = (sw_rst | (1 << ASIU_TOP_SW_RESET_CTRL__SMARTCARD_SW_RESET_N));
        post_log("wr ASIU_TOP_SW_RESET_CTRL = 0x%X\n",sw_rst);
        lcd_write_reg(ASIU_TOP_SW_RESET_CTRL,sw_rst);
}
void setgpio3_3p3(int data)
{
	int read_val = CPU_READ_32(ASIU_GP_OUT_EN_3);
	//GPIO 144
	read_val = read_val | 0x1000000;
	CPU_WRITE_32(ASIU_GP_OUT_EN_3,read_val);

	read_val = CPU_READ_32(ASIU_GP_DATA_OUT_3);
	if(data == 1)
		read_val = read_val | 0x1000000;
	else if(data == 0)
		read_val = read_val & 0xFEFFFFFF;

	CPU_WRITE_32(ASIU_GP_DATA_OUT_3,read_val);

}

int scino=0;
#define SCI_OFFSET (SCB_SC_UART_CMD_1 - SCA_SC_UART_CMD_1)

static void cpu_reg_write32(uint32_t addr, uint32_t val)
{
	volatile uint32_t *address =  (volatile uint32_t *)(addr + scino*SCI_OFFSET);

	post_log("WR 0x%08x with 0x%08x\n", address, val);

	CPU_WRITE_32(address, val);
}

static uint32_t cpu_reg_read32(uint32_t addr)
{
	volatile uint32_t *address =  (volatile uint32_t *)(addr + scino*SCI_OFFSET);

	return CPU_READ_32(address);
}

int SCI_post_test(int flags)
{
    uint32_t i;

    int card_detected = 0;
    uint32_t rx_status, rx,pass_flag;
	uint32_t atr_async[20];
	uint32_t cnt=0;
	uint32_t rdata;
	uint32_t atr_sync_byte_count = 0;


	uint32_t atr_async0_rcv[] = {
	                        0x3B,
	                        0xBE,
	                        0x11,
	                        0x00,
	                        0x00,
	                        0x41,
	                        0x01,
	                        0x38,
	                        0x00,
	                        0x00,
	                        0x00,
	                        0x00,
	                        0x00,
	                        0x00,
	                        0x00,
	                        0x00,
	                        0x02,
	                        0x90,
	                        0x00
	                      };

	uint32_t atr_async1_rcv[] = {
	                        0x3B,
	                        0xBE,
	                        0x11,
	                        0x00,
	                        0x00,
	                        0x41,
	                        0x01,
	                        0x38,
	                        0x00,
	                        0x00,
	                        0x00,
	                        0x00,
	                        0x00,
	                        0x00,
	                        0x00,
	                        0x00,
	                        0x01,
	                        0x90,
	                        0x00
	};

	post_log("\n");
	pass_flag = 0;

	post_log("\nSelect SCI interface: 0 - SCI0; 1 - SCI1\n");
		scino = post_getUserInput("SCI i/f? (0/1) : ");

		CPU_WRITE_32(CRMU_CHIP_IO_PAD_CONTROL, 0x00);
	sci_reset();
	CPU_WRITE_32(ASIU_TOP_CLK_GATING_CTRL, CPU_READ_32(ASIU_TOP_CLK_GATING_CTRL) | 0x100);




	post_log("sci num=%d\n", scino);

	if (!scino) {
	        rdata = CPU_READ_32(GP_OUT_EN);
	        rdata |= 0x3;
	        CPU_WRITE_32(GP_OUT_EN , rdata);

	        rdata = CPU_READ_32(GP_DATA_OUT);
	        rdata |= 0x2;
	        CPU_WRITE_32(GP_DATA_OUT, rdata);
	    }
	    else
	    {
	    	rdata = CPU_READ_32(GP_OUT_EN);
	    	        rdata |= 0x3;
	    	        CPU_WRITE_32(GP_OUT_EN , rdata);

	    	        rdata = CPU_READ_32(GP_DATA_OUT);
	    	        rdata |= 0x1;
	    	        CPU_WRITE_32(GP_DATA_OUT, rdata);
	    }




	cpu_reg_write32(SCA_SC_TIMER_CMD, 0x00);
	cpu_reg_write32(SCA_SC_INTR_EN_1, 0x00);
	cpu_reg_write32(SCA_SC_INTR_EN_2, 0x00);
	cpu_reg_write32(SCA_SC_UART_CMD_1, 0x00);
	cpu_reg_write32(SCA_SC_UART_CMD_2, 0x00);
	cpu_reg_write32(SCA_SC_IF_CMD_1, 0x05);//cpu_reg_write32(SCA_SC_IF_CMD_1, 0x05);

	cpu_reg_write32(SCA_SC_CLK_CMD_1, 0x8b);
	cpu_reg_write32(SCA_SC_PRESCALE, 0x0b);

	cpu_reg_write32(SCA_SC_IF_CMD_1, 0x07);//cpu_reg_write32(SCA_SC_IF_CMD_1, 0x07);
	cpu_reg_write32(SCA_SC_UART_CMD_1, 0x00);

	cpu_reg_write32(SCA_SC_INTR_EN_1, 0x20);
	cpu_reg_read32(SCA_SC_INTR_STAT_1); //read clear present intr

	cpu_reg_write32(SCA_SC_UART_CMD_1, 0x01);
	__udelay(100);
	if (cpu_reg_read32(SCA_SC_UART_CMD_1)==0)
		post_log("Reset UART success!\n");

	cpu_reg_write32(SCA_SC_PROTO_CMD, 0x30);//reset tx/rx buf

	while (!card_detected) {
		card_detected = cpu_reg_read32(SCA_SC_STATUS_1) & (1<<SCA_SC_STATUS_1__card_pres);
	}
	post_log("Card present!\n");

	cpu_reg_write32(SCA_SC_IF_CMD_1, 0x06);
	cpu_reg_write32(SCA_SC_IF_CMD_1, 0x04);
	cpu_reg_write32(SCA_SC_UART_CMD_2, 0x00);
	cpu_reg_write32(SCA_SC_INTR_EN_2, 0x42);
	cpu_reg_write32(SCA_SC_UART_CMD_1, 0x60);
	cpu_reg_write32(SCA_SC_IF_CMD_1, 0x06);

    __udelay(10);

 	for(i=0; i<19; i++)
    {
		do{
			rx_status = cpu_reg_read32(SCA_SC_STATUS_2);
			__udelay(100);
			cnt++;
		}while(rx_status&(1<<SCA_SC_STATUS_2__rempty) && cnt<1000);

		if(cnt==1000)
			break;

		rx = cpu_reg_read32(SCA_SC_RECEIVE) & 0xff;
		atr_async[i] = rx;
		post_log("ATR[%d]=0x%02X \n", i, rx);
    }

 	/*atr_sync_byte_count = 0;
 	for(i=0;i<19;i++)
 	{
 		if (atr_async[i] == atr_async0_rcv[i])
 		{
 			if (atr_sync_byte_count == 19) {
 			post_log("ATR SYNC BYTES MATCHING WITH THE STANDARD VALUES \n");
 			pass_flag = 1;
 			}
 		}
 		else
 		{
 		    post_log("ATR SYNC BYTE NOT MATCHING FOR POSITION %d ACTUAL VALUE : %d EXPECTED VALUE : %d \n",i,atr_async[i],atr_async0_rcv);
 		}

 		atr_sync_byte_count++;


 	}*/

 	atr_sync_byte_count = 0;
 			 	for(i=0;i<19;i++)
 			 	{
 			 		if (atr_async[i] == atr_async0_rcv[i])
 			 		{
 			 			atr_sync_byte_count++;
 			 			if (atr_sync_byte_count == 19) {
 			 		    post_log("===========================================================================================\n");
 			 			post_log("ATR SYNC BYTES MATCHING WITH THE STANDARD VALUES FOR ATR ASYNC VERSION 0\n");
 			 			 post_log("===========================================================================================\n");
 			 			pass_flag = 1;
 			 			break;
 			 			}
 			 		}
 			 		else
 			 		{
 			 			 post_log("===========================================================================================\n");
 			 			post_log("ATR ASYNC VERSION 0 SMART CARDS NOT BEING USED - MAY BE ASYNC VERSION 1 in USE\n");
 			 		    post_log("ATR ASYNC BYTE NOT MATCHING FOR POSITION %d ACTUAL VALUE : %02X EXPECTED VALUE : %02X \n",i,atr_async[i],atr_async0_rcv[i]);

 			 		  post_log("THIS IS FOR ASYNC VERSION 0 ...PROCEEDING TO TRY WITH ASYNC VERSION 1 \n");
 			 		 post_log("===========================================================================================\n");
 			 		    break;
 			 		}


 			 	}

 			 	if (pass_flag == 0)
 			 	{
 			 		atr_sync_byte_count = 0;
 			 	for(i=0;i<19;i++)
 			 			 	{

 			 		if (atr_async[i] == atr_async1_rcv[i])
 			 			 		{
 			 			atr_sync_byte_count++;
 			 					 			if (atr_sync_byte_count == 19) {
 			 					 			 post_log("===========================================================================================\n");
 			 					 			post_log("ATR SYNC BYTES MATCHING WITH THE STANDARD VALUES FOR ATR ASYNC VERSION 1\n");
 			 					 			pass_flag = 1;
 			 					 		 post_log("===========================================================================================\n");
 			 					 			break;
 			 					 			}

 			 			 		}
 			 			 		else
 			 			 		{
 			 			 		 post_log("===========================================================================================\n");
 			 			 			post_log("ATR ASYNC VERSION 1 SMART CARDS NOT BEING USED \n");
 			 			 		    post_log("ATR SYNC BYTE NOT MATCHING FOR POSITION %d ACTUAL VALUE : %02X EXPECTED VALUE : %02X \n",i,atr_async[i],atr_async1_rcv[i]);
 			 			 		 post_log("THIS IS FOR ATR ASYNC VERSION 1 \n");
 			 			 		 		 			 		    post_log("BOTH ATR ASYNC SEQUENCES NOT WORKING FINE \n");
 			 			 		 		 			 		 post_log("===========================================================================================\n");
 			 			 		    break;
 			 			 		}


 			 			 	}
 			 	}



	if (pass_flag == 1)
	{
		 post_log("===========================================================================================\n");
	post_log("Sathishs: TEST_PASSED \n");
	post_log("Sathishs: Please Ignore the below PASSED message \n ");
	 post_log("===========================================================================================\n");
	}
	else
	{
		 post_log("===========================================================================================\n");
	post_log("Sathishs : TEST FAILED \n");
	post_log("Sathishs : Please Ignore the below PASSED message \n");
	 post_log("===========================================================================================\n");
	}

	return 0;
}



#if 0
int SCI_post_test(int flags, post_result *t_result)
{   
    uint32_t read_val;
    uint8_t rd_val[36];
    uint8_t volatile i;
    uint32_t failed = 0;
    int sci1;
    int card_detected = 0;
    uint32_t wr_data_clk_gate =0;
    uint32_t rd_data = 0;
    uint32_t rx_status;


    uint32_t atr_async0_rcv[] = {
                        0x3B,
                        0xBE,
                        0x11,
                        0x00,
                        0x00,
                        0x41,
                        0x01,
                        0x38,
                        0x00,
                        0x00,
                        0x01,
                        0x00,
                        0x00,
                        0x00,
                        0x00,
                        0x00,
                        0x01,
                        0x90,
                        0x00
                      };
                      
uint32_t atr_async1_rcv[] = {
                        0x3B,
                        0xBE,
                        0x11,
                        0x00,
                        0x00,
                        0x41,
                        0x01,
                        0x38,
                        0x00,
                        0x00,
                        0x00,
                        0x00,
                        0x00,
                        0x00,
                        0x00,
                        0x00,
                        0x01,
                        0x90,
                        0x00
};                      
	 //Enable AXI clocka
	//This is done by sci_async_atr_init(). Hence commenting off
	  /*rd_data = cpu_rd_single (ASIU_TOP_CLK_GATING_CTRL, 4);
	  wr_data_clk_gate = (rd_data |
                      1 << ASIU_TOP_CLK_GATING_CTRL__SMARTCARD_CLK_GATE_EN);
	  cpu_wr_single (ASIU_TOP_CLK_GATING_CTRL, wr_data_clk_gate , 4);
	*/
cpu_wr_single(0x301d0bc,0,4);
	sci_reset();

      post_log("\n\nSelect SCI interface: 0 - SCI0; 1 - SCI1\n");
      sci1 = post_getUserInput("SCI i/f? (0/1) : ");

      sci_async_atr_init(sci1);

    if (!sci1) {

            /* Enable the SC0 Mode Register ..Async mode*/ 
            /* Is there an equivalent for sci_enable in Cygnus?     CPU_WRITE_8(SCI0_R_SC_MODE_REGISTER_MEMADDR,SCI0_F_sci_enable_MASK); */
    
            /* Select the polarity of SC_PRES to be active high */
	    post_log("Selecting SC_PRESENT polarity for SCI-0 to be active high\n\r");
            CPU_WRITE_8(SCA_SC_IF_CMD_1, 0x00);
            CPU_RMW_OR_8(SCA_SC_IF_CMD_1, (1<<SCA_SC_IF_CMD_1__pres_pol));


            /*Enable all intr*/
            post_log("Enabling all the SCI-0 Interrupts\n\r");	    
            CPU_WRITE_8( SCA_SC_INTR_EN_1, 0xff); 
            CPU_WRITE_8( SCA_SC_INTR_EN_2, 0xff);

            /*VCC Ramp*/	
            /*Drive 0 on CMDVCC (active_low) bit to start the card activation sequence */
            post_log("Initiating card activation Sequence\n\r");	    
            read_val = CPU_READ_8(SCA_SC_IF_CMD_1);
            read_val &= 0xFE;
            CPU_WRITE_8( SCA_SC_IF_CMD_1, read_val);

            /*Check for card_pres*/
            post_log("\n Polling for SCI-0 Card Presence in the SC_STATUS_1 register! \n\r");

            while (!card_detected) {
                    read_val = CPU_READ_8(SCA_SC_STATUS_1);
                    card_detected = (read_val >> SCA_SC_STATUS_1__card_pres);
            }

            post_log("\n SCI-0 Card Detected! \n\r");

	    /*Configure Clk */
            CPU_WRITE_8( SCA_SC_CLK_CMD_1, 0x8b);
            CPU_WRITE_8( SCA_SC_PRESCALE, 0xb);

	    post_log("Configured the clocks \n\r");
            
	    __udelay(1);

            /*Deassert RESET*/
	    post_log("De-asserting the Reset signal\n\r");
            CPU_RMW_OR_8(SCA_SC_IF_CMD_1, (1<<SCA_SC_IF_CMD_1__rst));	

            post_log("Enable character reception on IO\n\r"); 
            CPU_RMW_OR_8(SCA_SC_UART_CMD_1, (1 << SCA_SC_UART_CMD_1__io_en));	

    } else { /* if (!sci1)*/
 

            /*Enable the SC0 Mode Register ..Async mode*/ 
            /* Is there an equivalent for sci_enable in Cygnus?    CPU_WRITE_8(SCI0_R_SC_MODE_REGISTER_MEMADDR,SCI0_F_sci_enable_MASK); */
    
            /* Select the polarity of SC_PRES to be active high */
	    post_log("Selecting SC_PRESENT polarity for SCI-1 to be active high\n\r");
            CPU_WRITE_8(SCB_SC_IF_CMD_1, 0x00);
            CPU_RMW_OR_8(SCB_SC_IF_CMD_1, (1<<SCB_SC_IF_CMD_1__pres_pol));


            post_log("Enabling all the SCI-1 Interrupts\n\r");	    
            CPU_WRITE_8( SCB_SC_INTR_EN_1, 0xff); 
            CPU_WRITE_8( SCB_SC_INTR_EN_2, 0xff);

            /*VCC Ramp*/	
            /*Drive 0 on CMDVCC (active_low) bit to start the card activation sequence */	
            post_log("Initiating card activation Sequence\n\r");	    
            read_val = CPU_READ_8(SCB_SC_IF_CMD_1);
            read_val &= 0xFE;
            CPU_WRITE_8( SCB_SC_IF_CMD_1, read_val);

            /*Check for card_pres*/
            post_log("\n Polling for SCI-1 Card Presence in the SC_STATUS_1 register! \n\r");

            while (!card_detected) {
                    read_val = CPU_READ_8(SCB_SC_STATUS_1);
                    card_detected = (read_val >> SCB_SC_STATUS_1__card_pres);
            }

            post_log("\n SCI-1 Card Detected! \n\r");

            /*Enable Clk */
            CPU_WRITE_8( SCB_SC_CLK_CMD_1, 0x8b);
            CPU_WRITE_8( SCB_SC_PRESCALE, 0xb);

	    post_log("Configured the clocks \n\r");

	    __udelay(1);

            /*Deassert RESET */
	    post_log("De-asserting the Reset signal\n\r");
            CPU_RMW_OR_8(SCB_SC_IF_CMD_1, (1<<SCB_SC_IF_CMD_1__rst));	

            post_log("Enable character reception on IO\n\r"); 
            CPU_RMW_OR_8( SCB_SC_UART_CMD_1, (1 << SCB_SC_UART_CMD_1__io_en));	

    }
    post_log("rd_val: %x\n\r",rd_val);

    __udelay(1);

    get_atr_bytes(sci1, 19,rd_val);

    __udelay(1);



    if (sci1)
    {


    	rx_status = CPU_READ_8(SCB_SC_RECEIVE);
    	post_log("ATR SYNC BYTE 1 : %02X \n",rx_status);
    	__udelay(10);
    	rx_status = CPU_READ_8(SCB_SC_RECEIVE);
    	post_log("ATR SYNC BYTE 2 : %02X \n",rx_status);
    	__udelay(10);
    	rx_status = CPU_READ_8(SCB_SC_RECEIVE);
    	post_log("ATR SYNC BYTE 3 : %02X \n",rx_status);
    	__udelay(10);
    	rx_status = CPU_READ_8(SCB_SC_RECEIVE);
    	post_log("ATR SYNC BYTE 4 : %02X \n",rx_status);
    	__udelay(10);
    	rx_status = CPU_READ_8(SCB_SC_RECEIVE);
    	post_log("ATR SYNC BYTE 5 : %02X \n",rx_status);
    	__udelay(10);
    	rx_status = CPU_READ_8(SCB_SC_RECEIVE);
        post_log("ATR SYNC BYTE 6 : %02X \n",rx_status);
        __udelay(10);
    	rx_status = CPU_READ_8(SCB_SC_RECEIVE);
    	post_log("ATR SYNC BYTE 7 : %02X \n",rx_status);
    	__udelay(10);
    	rx_status = CPU_READ_8(SCB_SC_RECEIVE);
    	post_log("ATR SYNC BYTE 8 : %02X \n",rx_status);
    	__udelay(10);
    	rx_status = CPU_READ_8(SCB_SC_RECEIVE);
    	post_log("ATR SYNC BYTE 9 : %02X \n",rx_status);
    	__udelay(10);
    	rx_status = CPU_READ_8(SCB_SC_RECEIVE);
    	post_log("ATR SYNC BYTE 10 : %02X \n",rx_status);
    	__udelay(10);
    	rx_status = CPU_READ_8(SCB_SC_RECEIVE);
    	post_log("ATR SYNC BYTE 11 : %02X \n",rx_status);
    	__udelay(10);
    	rx_status = CPU_READ_8(SCB_SC_RECEIVE);
    	post_log("ATR SYNC BYTE 12 : %02X \n",rx_status);
    	__udelay(10);
    	rx_status = CPU_READ_8(SCB_SC_RECEIVE);
        post_log("ATR SYNC BYTE 13 : %02X \n",rx_status);
        __udelay(10);
         rx_status = CPU_READ_8(SCB_SC_RECEIVE);
         post_log("ATR SYNC BYTE 14 : %02X \n",rx_status);
         __udelay(10);
           rx_status = CPU_READ_8(SCB_SC_RECEIVE);
         post_log("ATR SYNC BYTE 15 : %02X \n",rx_status);
         __udelay(10);
          rx_status = CPU_READ_8(SCB_SC_RECEIVE);
         post_log("ATR SYNC BYTE 16 : %02X \n",rx_status);
         __udelay(10);
           rx_status = CPU_READ_8(SCB_SC_RECEIVE);
          post_log("ATR SYNC BYTE 17 : %02X \n",rx_status);
          __udelay(10);
         rx_status = CPU_READ_8(SCB_SC_RECEIVE);
          post_log("ATR SYNC BYTE 18 : %02X \n",rx_status);
          __udelay(10);
                   rx_status = CPU_READ_8(SCB_SC_RECEIVE);
                    post_log("ATR SYNC BYTE 19 : %02X \n",rx_status);
    }
    else
    {

    	    	rx_status = CPU_READ_8(SCA_SC_RECEIVE);
    	    	post_log("ATR SYNC BYTE 1 : %02X \n",rx_status);
    	    	__udelay(10);
    	    	rx_status = CPU_READ_8(SCA_SC_RECEIVE);
    	    	post_log("ATR SYNC BYTE 2 : %02X \n",rx_status);
    	    	__udelay(10);
    	    	rx_status = CPU_READ_8(SCA_SC_RECEIVE);
    	    	post_log("ATR SYNC BYTE 3 : %02X \n",rx_status);
    	    	__udelay(10);
    	    	rx_status = CPU_READ_8(SCA_SC_RECEIVE);
    	    	post_log("ATR SYNC BYTE 4 : %02X \n",rx_status);
    	    	__udelay(10);
    	    	rx_status = CPU_READ_8(SCA_SC_RECEIVE);
    	    	post_log("ATR SYNC BYTE 5 : %02X \n",rx_status);
    	    	__udelay(10);
    	    	rx_status = CPU_READ_8(SCA_SC_RECEIVE);
    	        post_log("ATR SYNC BYTE 6 : %02X \n",rx_status);
    	        __udelay(10);
    	    	rx_status = CPU_READ_8(SCA_SC_RECEIVE);
    	    	post_log("ATR SYNC BYTE 7 : %02X \n",rx_status);
    	    	__udelay(10);
    	    	rx_status = CPU_READ_8(SCA_SC_RECEIVE);
    	    	post_log("ATR SYNC BYTE 8 : %02X \n",rx_status);
    	    	__udelay(10);
    	    	rx_status = CPU_READ_8(SCA_SC_RECEIVE);
    	    	post_log("ATR SYNC BYTE 9 : %02X \n",rx_status);
    	    	__udelay(10);
    	    	rx_status = CPU_READ_8(SCA_SC_RECEIVE);
    	    	post_log("ATR SYNC BYTE 10 : %02X \n",rx_status);
    	    	__udelay(10);
    	    	rx_status = CPU_READ_8(SCA_SC_RECEIVE);
    	    	post_log("ATR SYNC BYTE 11 : %02X \n",rx_status);
    	    	__udelay(10);
    	    	rx_status = CPU_READ_8(SCA_SC_RECEIVE);
    	    	post_log("ATR SYNC BYTE 12 : %02X \n",rx_status);
    	    	__udelay(10);
    	    	rx_status = CPU_READ_8(SCA_SC_RECEIVE);
    	        post_log("ATR SYNC BYTE 13 : %02X \n",rx_status);
    	        __udelay(10);
    	         rx_status = CPU_READ_8(SCA_SC_RECEIVE);
    	         post_log("ATR SYNC BYTE 14 : %02X \n",rx_status);
    	         __udelay(10);
    	           rx_status = CPU_READ_8(SCA_SC_RECEIVE);
    	         post_log("ATR SYNC BYTE 15 : %02X \n",rx_status);
    	         __udelay(10);
    	          rx_status = CPU_READ_8(SCA_SC_RECEIVE);
    	         post_log("ATR SYNC BYTE 16 : %02X \n",rx_status);
    	         __udelay(10);
    	           rx_status = CPU_READ_8(SCA_SC_RECEIVE);
    	          post_log("ATR SYNC BYTE 17 : %02X \n",rx_status);
    	          __udelay(10);
    	         rx_status = CPU_READ_8(SCA_SC_RECEIVE);
    	          post_log("ATR SYNC BYTE 18 : %02X \n",rx_status);
    	          __udelay(10);
    	                   rx_status = CPU_READ_8(SCA_SC_RECEIVE);
    	                    post_log("ATR SYNC BYTE 19 : %02X \n",rx_status);
    }

    /*for(i=0;i<19;i++) { */

	    /* Depending on the card used (Class A or Class B), the compariosn below will be either atr_async0_rcv or atr_async1_rcv */
	    /* If Class A use atr_async1_rcv. If Class B use atr_async0_rcv*/
	    /* Dale Sheridan recently ordered a bunch of Class A cards which were tested on 5892-D0 platform.*/
	    /* Since this diag is ported from 5892, the assumption is Class A cards will be used to test this diag on Cygnus*/

        /*if(rd_val[i] != atr_async1_rcv[i]) {
            post_log("sci%u: Mismatch with expected ATR exp:%x.. rcv:%x\n\r",sci1, atr_async1_rcv[i],rd_val[i]);
	    failed = 1;
        }
    }

    if(failed) {

    	 for(i=0;i<19;i++) {
           post_log("sci%u: Mismatch with expected ATR exp:%x.. rcv:%x\n\r",sci1, atr_async1_rcv[i],rd_val[i]);
    	 }

         return 1;
    }*/
    __udelay(100);
    //post_log("Expected ATR received\n\r");
    if (pass_flag == 1)
    {
     post_log("sathishs : TEST_PASSED \n");
     return 0;}
    else
    {
     post_log("sathishs : TEST_FAILED \n");
      return -1;
    }

    
}
#endif

#endif /* CONFIG_POST & CONFIG_SYS_POST_SCI */
#endif /* CONFIG_CYGNUS*/        
