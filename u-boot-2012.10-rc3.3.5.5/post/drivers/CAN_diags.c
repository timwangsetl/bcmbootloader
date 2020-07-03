/*
 * CAN_diags.c
 *
 *  Created on: May 8, 2014
 *      Author: eappenm
 */
#include <common.h>
#include <post.h>

#if defined(CONFIG_CYGNUS)
#if CONFIG_POST & CONFIG_SYS_POST_CAN

#include "../../halapis/include/allregread_apis.h"
#include "../../halapis/include/cygnus_types.h"
#include "asm/arch/socregs.h"

#define M_CAN_RAM_SIZE_IN_WORDS             16384
#define CAN_0_MESSAGE_RAM_BASE            0x0180B0000
#define CAN_1_MESSAGE_RAM_BASE            0x0180B1000
#ifdef BIT8
#undef BIT8
#endif
#define BIT8 1
#ifdef BIT16
#undef BIT16
#endif
#define BIT16 2
#ifdef BIT32
#undef BIT32
#endif
#define BIT32 4

#define error_log post_log
#define print_log post_log
#define MEMTOMEM 0
#define MEMTOPERIPH 1
#define ADD_TEST(X)  { #X , X }
#define TEST_PASS  0
#define TEST_FAIL -1
#define TEST_SKIP  2

typedef struct{

	char* func_name;
	int (*func)(void);
}FUNC_INFO;


void CAN_add_delay(uint32_t delay)
{
  volatile uint32_t i;
    	 for(i=0;i<delay;i++);
 }


/*  --------------------------- function ----------------------------------- */
uint32_t CAN_WRITE_REG_api(uint32_t addr,uint32_t data)
{
      cpu_wr_single(addr,data,4);
      post_log(" API-WRITE REG - Address = %0x  Data = %0x\n",addr,data);
      return 0;
}

/*  --------------------------- function ----------------------------------- */
uint32_t CAN_READ_REG_api(uint32_t addr)
{
uint32_t rd_data;

      rd_data=cpu_rd_single(addr,4);
      post_log(" API-READ REG - Address = %0x  Data = %0x\n",addr,rd_data);
      return(rd_data);
}


/*  --------------------------- function ----------------------------------- */
uint32_t can_clk_cdru_pll_ch3_5div_reg (uint32_t pll_sel, uint32_t ch3mdel, uint32_t ch3mdiv, uint32_t ch4mdel, uint32_t ch4mdiv, uint32_t ch5mdel, uint32_t ch5mdiv) { //gen_pll_sel = 0 -> GENPLL, 2 -> LC PLL, rest are invalid
  uint32_t rd_data = 0;
  uint32_t wr_data = 0;
  uint32_t mask = 0;

  switch(pll_sel) {
     case 0 : {
	rd_data = cpu_rd_single(CRMU_GENPLL_CONTROL9, 4);
	mask = 0xE0080200;
	wr_data = rd_data & mask;
	wr_data = (   wr_data				  	 |
            (ch3mdiv << CRMU_GENPLL_CONTROL9__GENPLL_CH3_MDIV_R) |
            (ch3mdel << CRMU_GENPLL_CONTROL9__GENPLL_CH3_MDEL)   |
            (ch4mdiv << CRMU_GENPLL_CONTROL9__GENPLL_CH4_MDIV_R) |
            (ch4mdel << CRMU_GENPLL_CONTROL9__GENPLL_CH4_MDEL)   |
            (ch5mdiv << CRMU_GENPLL_CONTROL9__GENPLL_CH5_MDIV_R) |
            (ch5mdel << CRMU_GENPLL_CONTROL9__GENPLL_CH5_MDEL)
        );
	cpu_wr_single(CRMU_GENPLL_CONTROL9, wr_data, 4);
     }
     case 1 : {
	rd_data = cpu_rd_single(CRMU_LCPLL0_CONTROL3, 4);
        mask = 0xE0080200;
        wr_data = rd_data & mask;
        wr_data = (   wr_data				  	 |
	    (ch3mdiv << CRMU_LCPLL0_CONTROL3__LCPLL0_CH3_MDIV_R) |
	    (ch3mdel << CRMU_LCPLL0_CONTROL3__LCPLL0_CH3_MDEL)   |
	    (ch4mdiv << CRMU_LCPLL0_CONTROL3__LCPLL0_CH4_MDIV_R) |
	    (ch4mdel << CRMU_LCPLL0_CONTROL3__LCPLL0_CH4_MDEL)   |
	    (ch5mdiv << CRMU_LCPLL0_CONTROL3__LCPLL0_CH5_MDIV_R) |
	    (ch5mdel << CRMU_LCPLL0_CONTROL3__LCPLL0_CH5_MDEL)
        );
        cpu_wr_single(CRMU_LCPLL0_CONTROL3, wr_data, 4);
     }
  }
  return 1;
}

/*  --------------------------- function ----------------------------------- */
uint32_t can_clk_cdru_genpll_ctl4_reg(uint32_t ndiv_frac, uint32_t ndiv_int, uint32_t sel_sw) {
  uint32_t rd_data = 0;
  uint32_t wr_data = 0;
  uint32_t mask = 0;
         rd_data = cpu_rd_single(CRMU_GENPLL_CONTROL4, 4);
        mask = 0x40000000;
        wr_data = rd_data & mask;
        wr_data = (   wr_data				  	             |
                  (ndiv_frac << CRMU_GENPLL_CONTROL4__GENPLL_NDIV_FRAC_R)    |
		  (ndiv_int  << CRMU_GENPLL_CONTROL4__GENPLL_NDIV_INT_R)     |
		  (sel_sw    << CRMU_GENPLL_CONTROL4__GENPLL_SEL_SW_SETTING)
                );
        cpu_wr_single(CRMU_GENPLL_CONTROL4, wr_data, 4);
      return 1;
}

/*  --------------------------- function ----------------------------------- */
uint32_t CAN_REG_RD_WR_api(uint32_t addr ,uint32_t data)
{
  uint32_t rd_data[70];
  uint32_t i;
  uint32_t j=0;
  uint32_t rd_data_reg;

              cpu_wr_single(CAN0_CCCR,0x1,4);
              rd_data_reg=cpu_rd_single(CAN0_CCCR,4);
              while (((rd_data_reg) & 0x1) ==0);
              post_log(" API-CCR RD DATA - Address = %0x  Data = %0x\n",CAN0_CCCR,rd_data_reg);

              post_log(" API-Set configuration change bit and init bit \n");

             cpu_wr_single(CAN0_CCCR,0x3,4);
             do {
                 rd_data_reg=cpu_rd_single(CAN0_CCCR,4);
                  post_log(" API-CCR RD DATA - Address = %0x  Data = %0x\n",CAN0_CCCR,rd_data_reg);
                 } while (rd_data_reg !=0x3);

             post_log(" API-CCR RD DATA - Address = %0x  Data = %0x\n",CAN0_CCCR,rd_data_reg);
             post_log(" API-Configuration change bit and init bit is set \n");

             post_log(" API-START REG READ / WRITE TEST \n");


      for(i=0;i<0x0FC;i=i+4)
      {
      rd_data[j]=cpu_rd_single((addr+i),4);
      post_log(" API-READ REGISTER  - Address = %0x  Data = %0x\n",(addr+i),rd_data[j]);
      j++;
      }



//WRITE THE REGS

      for(i=0;i<0x0FC;i=i+4)
      {
       if ((addr+i)==CAN0_CCCR)
       {}
       else
       {
       cpu_wr_single((addr+i),data,4);
       post_log(" API-WRITE REGISTER  - Address = %0x  Data = %0x\n",(addr+i),data);
       }
      }

//READ THE REGS AGAIN

     j=0;
     for(i=0;i<0x0FC;i=i+4)
      {
      rd_data[j]=cpu_rd_single((addr+i),4);
      post_log(" API-READ REGISTER  - Address = %0x  Data = %0x\n",(addr+i),rd_data[j]);
      j++;
      }

     return 0;
  }

/*  --------------------------- function ----------------------------------- */
uint32_t CAN_DPMEM_WR_RD_api(uint32_t addr ,uint32_t data, uint32_t rw)
{
//rw=0 write in DP Mem; rw=1 Read
  uint32_t rd_data[1024];
  uint32_t i;
  uint32_t j=0;



if (rw)   //READ DP MEM
{
   for(i=0;i<4096;i=i+4) //1024 locations of message RAM
      {
      rd_data[j]=cpu_rd_single((addr+i),4);
      post_log(" API-DP MEM READ  - Address = %0x  Data = %0x\n",(addr+i),rd_data[j]);
      j++;
      }
   return (0);

}
//WRITE THE DP MEM
else

{

    data=addr;

    for(i=0;i<4096;i=i+4)     //1024 locations of message RAM
      {
       cpu_wr_single((addr+i),(data+i),4);
      post_log(" API-DP MEM WRITE  - Address = %0x  Data = %0x\n",(addr+i),(data+i));

      }

      return (1);

}

}

/*  --------------------------- function ----------------------------------- */
uint32_t CAN_TEST_MODE_api(uint32_t dummy)
{

  uint32_t rd_data;
  uint32_t wr_data;
 // uint32_t value ,i;


           cpu_wr_single(CAN0_CCCR,0x1,4);
           rd_data=cpu_rd_single(CAN0_CCCR,4);
           while (((rd_data) & 0x1) ==0);
           post_log(" API-CCR RD DATA - Address = %0x  Data = %0x\n",CAN0_CCCR,rd_data);

           post_log(" API-TEST MODE - Set configuration change bit and init bit \n");

           cpu_wr_single(CAN0_CCCR,0x3,4);
           do {
                 rd_data=cpu_rd_single(CAN0_CCCR,4);
                  post_log(" API-CCR RD DATA - Address = %0x  Data = %0x\n",CAN0_CCCR,rd_data);
           } while (rd_data !=0x3);

           post_log(" API-CCR RD DATA - Address = %0x  Data = %0x\n",CAN0_CCCR,rd_data);
           post_log(" API-TEST MODE - Configuration change bit and init bit is set \n");



       	   //Configure CAN for 500 KBIT/s

           // cpu_wr_single(CAN0_BTP,0x00002303,4);
           cpu_wr_single(CAN0_BTP,0x00003EF0,4);  //500 kbits at 40 MHz
           rd_data= cpu_rd_single(CAN0_BTP,4);
           post_log(" API-CAN0_BTP RD DATA - Address = %0x  Data = %0x\n",CAN0_BTP,rd_data);


           //Enable Test Mode
           rd_data = cpu_rd_single(CAN0_CCCR,4);
           wr_data = (rd_data | 0x00000080) ;   //TEST bit "1"
           //wr_data = (rd_data | 0x000000A0) ;   //TEST bit & MON bit "1"

           cpu_wr_single(CAN0_CCCR,wr_data,4);
             post_log(" API-CAN0_CCCR WRITE  - Address = %0x  Data = %0x\n",CAN0_CCCR,wr_data);


            //SET LOOPBACK MODE
            rd_data = cpu_rd_single(CAN0_TEST,4);
            post_log(" API-TEST REG RD DATA - Address = %0x  Data = %0x\n",CAN0_TEST,rd_data);

            wr_data = (rd_data | 0x00000050);  //Loopback mode selected,mcan_tx dominant
            // wr_data = (rd_data | 0x00000070);

            cpu_wr_single(CAN0_TEST,wr_data,4);
            post_log(" API-TEST REG WR DATA - Address = %0x  Data = %0x\n",CAN0_TEST,wr_data);


            rd_data = cpu_rd_single(CAN0_TEST,4);
            post_log(" API-TEST REG RD DATA - Address = %0x  Data = %0x\n",CAN0_TEST,rd_data);

      //Disable the config change mode

           cpu_wr_single(CAN0_CCCR,0x00000000,4);
           while (((cpu_rd_single(CAN0_CCCR,4)) & 0x1) == 1);
           post_log(" API-TEST MODE - Configuration change complete \n");
           rd_data=cpu_rd_single(CAN0_CCCR,4);
            post_log(" API-CCR RD DATA - Address = %0x  Data = %0x\n",CAN0_CCCR,rd_data);

         //Tanuja - Commented in uboot build as this was causing cmpilation error.
           //cycle_delay(200000);
           rd_data = cpu_rd_single(CAN0_TEST,4); //Read the value on Rx
          post_log(" API-READ TEST REG  - Address = %0x  Data = %0x\n",CAN0_TEST,rd_data);

          return 0;
}

/*  --------------------------- function ----------------------------------- -----------------*/
uint32_t CAN0_TX_api(uint32_t brp,uint32_t tseg1,uint32_t tseg2,uint32_t sjw,uint32_t msg_id,uint32_t dlc,uint32_t data0,uint32_t data1)
{

  uint32_t rd_data;
  uint32_t wr_data;
  uint32_t mask,loop_count;


   loop_count = 1000;


	post_log("Enter CAN0 TX mode DATA0 = 0x%x\n", data0);

         cpu_wr_single(CAN0_CCCR,0x1,4);
         post_log("..loop.. 1\n");
         rd_data=cpu_rd_single(CAN0_CCCR,4);
          while ((rd_data & 0x1) ==0)
          {
        	  rd_data=cpu_rd_single(CAN0_CCCR,4);
          }

          CAN_add_delay(6000);
          cpu_wr_single(CAN0_CCCR,0x3,4);

          post_log("..loop.. 2\n");
          rd_data=cpu_rd_single(CAN0_CCCR,4);
          while ((rd_data & 0x3) !=0x03)
          {
               rd_data=cpu_rd_single(CAN0_CCCR,4);
           }

		  post_log("CAN.0. delay 1\n");

           //Configure CAN for 500 KBIT/s

        rd_data = cpu_rd_single(CAN0_BTP, 4);
	    mask = 0x00000000;		//0x03FF3FFF- Data Mask gives default value A33
	    wr_data = rd_data & mask;
  	    wr_data = (wr_data				  	     |
	             (brp     	<< CAN0_BTP__BRP_R) 		     |
                     (tseg1    	<< CAN0_BTP__TSEG1_R)      	     |
                     (tseg2     << CAN0_BTP__TSEG2_R)       	     |
		     (sjw	<< CAN0_BTP__SJW_R)
                   );
		    cpu_wr_single(CAN0_BTP,wr_data,4);
			rd_data= cpu_rd_single(CAN0_BTP,4);
            post_log(" Write data BTP Value:= %0x\n",wr_data);


          /* M_CAN is configured with one dedicated transmit buffer
	            The TX buffer start address is 0x0000.*/

            cpu_wr_single(CAN0_TXBC,0x05000000,4);
            rd_data= cpu_rd_single(CAN0_TXBC,4);
            post_log(" API-CAN0_TXBC RD DATA - Address = %0x  Data = %0x\n",CAN0_TXBC,rd_data);


         /*CONFIGURE MESSAGE RAM*/
         /* The TX buffer has the following configuration:
	           * std ID
	           * data frame
	           * ID = 1
	           * data length = 8
	           * data bytes:
	           * 0x10061979
	           * 0x12101981
	         */


           cpu_wr_single(CAN_0_MESSAGE_RAM_BASE,msg_id,4);
           cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+0x4),dlc,4);
           cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+0x8),data0,4);
           cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+0xC),data1,4);

           cpu_wr_single(CAN_0_MESSAGE_RAM_BASE+0x10,msg_id+0x1,4);
           cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+0x14),dlc,4);
           cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+0x18),data0,4);
           cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+0x1C),data1,4);

           cpu_wr_single(CAN_0_MESSAGE_RAM_BASE+0x20,msg_id+0x2,4);
           cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+0x24),dlc,4);
           cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+0x28),data0,4);
           cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+0x2C),data1,4);

           cpu_wr_single(CAN_0_MESSAGE_RAM_BASE+0x30,msg_id+0x3,4);
           cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+0x34),dlc,4);
           cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+0x38),data0,4);
           cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+0x3C),data1,4);

           cpu_wr_single(CAN_0_MESSAGE_RAM_BASE+0x40,msg_id+0x4,4);
           cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+0x44),dlc,4);
           cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+0x48),data0,4);
           cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+0x4C),data1,4);


           cpu_wr_single(CAN0_TXBTIE,0x0000001f,4);

           ///* Enable transmission complete interrupt */

           cpu_wr_single(CAN0_IE,0x00000a00,4);

          // /* Enable interrupt line 0*/
          cpu_wr_single(CAN0_ILE,0x00000001,4);
          //int. line select by default  int_0 selected

           rd_data = cpu_rd_single(CAN0_CCCR,4);
           wr_data = rd_data & 0xFFFFFFFC;

          cpu_wr_single(CAN0_CCCR,wr_data,4); //Disable the config change mode
            post_log("..loop.. 3\n");
            while (((cpu_rd_single(CAN0_CCCR,4)) & 0x1) == 1);
            post_log(" API- Configuration change complete \n");
            rd_data=cpu_rd_single(CAN0_CCCR,4);
            post_log(" API-CAN0_CCCR RD DATA - Address = %0x  Data = %0x\n",CAN0_CCCR,rd_data);

           /*Enable Transmission*/

           wr_data=0x0000001F;
           cpu_wr_single(CAN0_TXBAR,wr_data,4);
           rd_data= cpu_rd_single(CAN0_TXBAR,4);
           post_log(" API-CAN0_TXBAR RD DATA - Address = %0x  Data = %0x\n",CAN0_TXBAR,rd_data);



        /* Wait for transmission occurred flag*/

          post_log(" Waiting for transmission occurred flag...\n");



		 do {
                 rd_data=cpu_rd_single(CAN0_TXBTO,4);
                 // post_log(" API-CAN0_TXBTO RD DATA - Address = %0x  Data = %0x\n",CAN0_TXBTO,rd_data);
				  loop_count--;
                 } while (((rd_data & 0x00000002) !=0x02)&&(loop_count !=0));
		 
		 if (loop_count != 0)
		 {

           post_log(" Transmission Successful \n");
		 }
		 else
		 {
		 	  post_log(" Transmission FAILURE \n");
			  return 0;

		  }


		  #if 0
		  //alternate check

		  do {
                 rd_data=cpu_rd_single(CAN0_IR,4);
                  post_log(" API-CAN0_IR RD DATA - Address = %0x  Data = %0x\n",CAN0_IR,rd_data);
                 } while ((rd_data & 0x00000200) ==0);
		    post_log(" Transmission Successful \n");
		  #endif



          post_log(" Transmission Successfuli 0x%x \n", CAN_0_MESSAGE_RAM_BASE);

          return 1;
      }

/*  --------------------------- function ----------------------------------- -----------------*/
uint32_t CAN1_TX_api(uint32_t brp,uint32_t tseg1,uint32_t tseg2,uint32_t sjw,uint32_t msg_id,uint32_t dlc,uint32_t data0,uint32_t data1)
{

  uint32_t rd_data;
  uint32_t wr_data,loop_count;
  uint32_t mask;


   loop_count = 1000;
	post_log("Enter CAN1 TX mode DATA0 = 0x%x\n", data0);

         cpu_wr_single(CAN1_CCCR,0x1,4);
         post_log("..loop.. 1\n");
         rd_data=cpu_rd_single(CAN1_CCCR,4);
          while ((rd_data & 0x1) ==0)
          {
        	  rd_data=cpu_rd_single(CAN1_CCCR,4);
          }

          CAN_add_delay(6000);
          cpu_wr_single(CAN1_CCCR,0x3,4);

          post_log("..loop.. 2\n");
          rd_data=cpu_rd_single(CAN1_CCCR,4);
          while ((rd_data & 0x3) !=0x03)
          {
               rd_data=cpu_rd_single(CAN1_CCCR,4);
           }

		  post_log("CAN.1. delay 1\n");

           //Configure CAN for 500 KBIT/s

        rd_data = cpu_rd_single(CAN1_BTP, 4);
	    mask = 0x00000000;		//0x03FF3FFF- Data Mask gives default value A33
	    wr_data = rd_data & mask;
  	    wr_data = (wr_data				  	     |
	             (brp     	<< CAN1_BTP__BRP_R) 		     |
                     (tseg1    	<< CAN1_BTP__TSEG1_R)      	     |
                     (tseg2     << CAN1_BTP__TSEG2_R)       	     |
		     (sjw	<< CAN1_BTP__SJW_R)
                   );
		    cpu_wr_single(CAN1_BTP,wr_data,4);
			rd_data= cpu_rd_single(CAN1_BTP,4);
            post_log(" Write data BTP Value:= %0x\n",wr_data);


          /* M_CAN is configured with one dedicated transmit buffer
	            The TX buffer start address is 0x0000.*/

            cpu_wr_single(CAN1_TXBC,0x05001100,4);
            rd_data= cpu_rd_single(CAN1_TXBC,4);
            post_log(" API-CAN0_TXBC RD DATA - Address = %0x  Data = %0x\n",CAN0_TXBC,rd_data);


         /*CONFIGURE MESSAGE RAM*/
         /* The TX buffer has the following configuration:
	           * std ID
	           * data frame
	           * ID = 1
	           * data length = 8
	           * data bytes:
	           * 0x10061979
	           * 0x12101981
	         */


           cpu_wr_single(CAN_1_MESSAGE_RAM_BASE +0x0100,msg_id,4);
           cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+0x0100+0x4),dlc,4);
           cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+0x0100+0x8),data0,4);
           cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+0x0100+0xC),data1,4);

           cpu_wr_single( CAN_1_MESSAGE_RAM_BASE+0x0100+0x10,msg_id+0x1,4);
           cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+0x0100+0x14),dlc,4);
           cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+0x0100+0x18),data0,4);
           cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+0x0100+0x1C),data1,4);

           cpu_wr_single(CAN_1_MESSAGE_RAM_BASE+0x0100+0x20,msg_id+0x2,4);
           cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+0x0100+0x24),dlc,4);
           cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+0x0100+0x28),data0,4);
           cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+0x0100+0x2C),data1,4);

           cpu_wr_single(CAN_1_MESSAGE_RAM_BASE+0x0100+0x30,msg_id+0x3,4);
           cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+0x0100+0x34),dlc,4);
           cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+0x0100+0x38),data0,4);
           cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+0x0100+0x3C),data1,4);

           cpu_wr_single(CAN_1_MESSAGE_RAM_BASE+0x0100+0x40,msg_id+0x4,4);
           cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+0x0100+0x44),dlc,4);
           cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+0x0100+0x48),data0,4);
           cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+0x0100+0x4C),data1,4);


           cpu_wr_single(CAN1_TXBTIE,0x0000001f,4);

           ///* Enable transmission complete interrupt */

           cpu_wr_single(CAN1_IE,0x00000a00,4);

          // /* Enable interrupt line 0*/
          cpu_wr_single(CAN1_ILE,0x00000001,4);
          //int. line select by default  int_0 selected

           rd_data = cpu_rd_single(CAN1_CCCR,4);
           wr_data = rd_data & 0xFFFFFFFC;

          cpu_wr_single(CAN1_CCCR,wr_data,4); //Disable the config change mode
            post_log("..loop.. 3\n");
            while (((cpu_rd_single(CAN1_CCCR,4)) & 0x1) == 1);
            post_log(" API- Configuration change complete \n");
            rd_data=cpu_rd_single(CAN1_CCCR,4);
            post_log(" API-CAN1_CCCR RD DATA - Address = %0x  Data = %0x\n",CAN1_CCCR,rd_data);

           /*Enable Transmission*/

           wr_data=0x0000001F;
           cpu_wr_single(CAN1_TXBAR,wr_data,4);
           rd_data= cpu_rd_single(CAN1_TXBAR,4);
           post_log(" API-CAN1_TXBAR RD DATA - Address = %0x  Data = %0x\n",CAN1_TXBAR,rd_data);



        /* Wait for transmission occurred flag*/

          post_log(" Waiting for transmission occurred flag...\n");



      do {
		rd_data=cpu_rd_single(CAN1_TXBTO,4);
		 //post_log(" API-CAN0_TXBTO RD DATA - Address = %0x	Data = %0x\n",CAN0_TXBTO,rd_data);
		 loop_count--;
		} while (((rd_data & 0x00000002) !=0x02)&&(loop_count !=0));
	  
     if (loop_count != 0)
     {

        post_log(" Transmission Successful \n");
     }
     else
     {
	    post_log(" Transmission FAILURE \n");
	    return 0;
     }


#if 0

		 do {
                 rd_data=cpu_rd_single(CAN1_TXBTO,4);
                  post_log(" API-CAN1_TXBTO RD DATA - Address = %0x  Data = %0x\n",CAN1_TXBTO,rd_data);
                 } while ((rd_data & 0x0000001f) !=0x1f);

           post_log(" Transmission Successful \n");
#endif

		  #if 0
		  //alternate check

		  do {
                 rd_data=cpu_rd_single(CAN1_IR,4);
                  post_log(" API-CAN1_IR RD DATA - Address = %0x  Data = %0x\n",CAN1_IR,rd_data);
                 } while ((rd_data & 0x00000200) ==0);
		    post_log(" Transmission Successful \n");
		  #endif



          post_log(" Transmission Successfuli 0x%x \n", CAN_1_MESSAGE_RAM_BASE);

          return 1;
      }


/*  --------------------------- function ----------------------------------- */
uint32_t CAN1_RX_api(uint32_t brp,uint32_t tseg1,uint32_t tseg2,uint32_t sjw, uint32_t wtr_mrk1)
{

  uint32_t rd_data;
  uint32_t wr_data;
  uint32_t mask;
  uint32_t watermark;

	post_log("Enter CAN1_RX_api\n");


		  cpu_wr_single(CAN1_CCCR,0x1,4);
	      post_log("..loop.. 1\n");
		  rd_data=cpu_rd_single(CAN1_CCCR,4);
		  while ((rd_data & 0x1) ==0)
		     {
		       rd_data=cpu_rd_single(CAN1_CCCR,4);
		           }

		   CAN_add_delay(6000);
		   cpu_wr_single(CAN1_CCCR,0x3,4);

		   post_log("..loop.. 2\n");
		   rd_data=cpu_rd_single(CAN1_CCCR,4);
		   while ((rd_data & 0x3) !=0x03)
		     {
		            rd_data=cpu_rd_single(CAN1_CCCR,4);
		       }

		   post_log("CAN.0. delay 1\n");


         //Configure CAN for 500 KBIT/s

        rd_data = cpu_rd_single(CAN1_BTP, 4);
	    post_log(" rd_data:= %0x\n",rd_data);
	    mask = 0x00000000;		//0x03FF3FFF- Data Mask gives default value A33
	    wr_data =  rd_data & mask;
  	    wr_data = (wr_data				  	     |
	             (brp     	<< CAN1_BTP__BRP_R) 		     |
                     (tseg1    	<< CAN1_BTP__TSEG1_R)      	     |
                     (tseg2     << CAN1_BTP__TSEG2_R)       	     |
		     (sjw	<< CAN1_BTP__SJW_R)
                   );

            cpu_wr_single(CAN1_BTP,wr_data,4);
            rd_data= cpu_rd_single(CAN1_BTP,4);

      post_log(" API-CAN1_BTP RD DATA - Address = %0x  Data = %0x\n",CAN1_BTP,rd_data);

      /* RX FIFO*/

	  watermark = wtr_mrk1 << 16;
	  watermark = watermark | 0x1000;
	  post_log("Water mark1 = %0x\n",watermark);
      cpu_wr_single(CAN1_RXF0C,watermark,4); // Size-10  start address: 0x1000

      cpu_wr_single(CAN1_IE,0x00000002,4);
      cpu_wr_single(CAN1_ILE,0x00000001,4);



      // Configure Filter start addr in RAM

     // cpu_wr_single(CAN1_GFC,0x0000003C,4); // Filter_11 : start address: 0x1800 size: 0x80 end address: 0x09FF
      cpu_wr_single(CAN1_SIDFC,0x00801800,4); // Filter_11 : start address: 0x1800 size: 0x80 end address: 0x09FF
      cpu_wr_single(CAN1_XIDFC,0x00011A00,4); //Filter_29 : start address: 0x1A00 size: 0x40 end address: 0x0BFF
      cpu_wr_single((CAN_1_MESSAGE_RAM_BASE +0xA00),0x1F001000,4);

      rd_data = cpu_rd_single(CAN1_CCCR,4);
      wr_data = rd_data & 0xFFFFFFFC;
                             // cpu_wr_single(CAN1_CCCR,0x00000000,4); //Disable the config change mode
       cpu_wr_single(CAN1_CCCR,wr_data,4); //Disable the config change mode
       post_log("..loop.. 3\n");
       while (((cpu_rd_single(CAN1_CCCR,4)) & 0x1) == 1);
       post_log(" API- Configuration change complete \n");
       rd_data=cpu_rd_single(CAN1_CCCR,4);
       post_log(" API-CAN1_CCCR RD DATA - Address = %0x  Data = %0x\n",CAN1_CCCR,rd_data);

       post_log(" WAIT FOR RECEIVED DATA ......\n");

	   #if 0
		  //alternate check

		  do {
                 rd_data=cpu_rd_single(CAN1_IR,4);
                  post_log(" API-CAN1_IR RD DATA - Address = %0x  Data = %0x\n",CAN1_IR,rd_data);
                 } while ((rd_data & 0x00000002) ==0);
		    post_log(" Transmission Successful \n");

		// can we check newdata - if buffer enabled
	   #endif

       return 0;
  }

/*  --------------------------- function ----------------------------------- */
uint32_t CAN0_RX_api(uint32_t brp,uint32_t tseg1,uint32_t tseg2,uint32_t sjw, uint32_t wtr_mrk1)
{

  uint32_t rd_data;
  uint32_t wr_data;
  uint32_t mask;
  uint32_t watermark;

	post_log("Enter CAN0_RX_api\n");


		  cpu_wr_single(CAN0_CCCR,0x1,4);
	      post_log("..loop.. 1\n");
		  rd_data=cpu_rd_single(CAN0_CCCR,4);
		  while ((rd_data & 0x1) ==0)
		     {
		       rd_data=cpu_rd_single(CAN0_CCCR,4);
		           }

		   CAN_add_delay(6000);
		   cpu_wr_single(CAN0_CCCR,0x3,4);

		   post_log("..loop.. 2\n");
		   rd_data=cpu_rd_single(CAN0_CCCR,4);
		   while ((rd_data & 0x3) !=0x03)
		     {
		            rd_data=cpu_rd_single(CAN0_CCCR,4);
		       }

		   post_log("CAN.0. delay 1\n");


         //Configure CAN for 500 KBIT/s

        rd_data = cpu_rd_single(CAN0_BTP, 4);
	    post_log(" rd_data:= %0x\n",rd_data);
	    mask = 0x00000000;		//0x03FF3FFF- Data Mask gives default value A33
	    wr_data =  rd_data & mask;
  	    wr_data = (wr_data				  	     |
	             (brp     	<< CAN0_BTP__BRP_R) 		     |
                     (tseg1    	<< CAN0_BTP__TSEG1_R)      	     |
                     (tseg2     << CAN0_BTP__TSEG2_R)       	     |
		     (sjw	<< CAN0_BTP__SJW_R)
                   );

            cpu_wr_single(CAN0_BTP,wr_data,4);
            rd_data= cpu_rd_single(CAN0_BTP,4);

      post_log(" API-CAN1_BTP RD DATA - Address = %0x  Data = %0x\n",CAN0_BTP,rd_data);

      /* RX FIFO*/

	  watermark = wtr_mrk1 << 16;
	  watermark = watermark | 0x0100;
	  post_log("Water mark1 = %0x\n",watermark);
      cpu_wr_single(CAN0_RXF0C,watermark,4); // Size-10  start address: 0x1000

      cpu_wr_single(CAN0_IE,0x00000002,4);
      cpu_wr_single(CAN0_ILE,0x00000001,4);



      // Configure Filter start addr in RAM

     // cpu_wr_single(CAN1_GFC,0x0000003C,4); // Filter_11 : start address: 0x1800 size: 0x80 end address: 0x09FF
      cpu_wr_single(CAN0_SIDFC,0x00800800,4); // Filter_11 : start address: 0x1800 size: 0x80 end address: 0x09FF
      cpu_wr_single(CAN0_XIDFC,0x00010A00,4); //Filter_29 : start address: 0x1A00 size: 0x40 end address: 0x0BFF
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE +0xA00),0x1F001000,4);

      rd_data = cpu_rd_single(CAN0_CCCR,4);
      wr_data = rd_data & 0xFFFFFFFC;
                             // cpu_wr_single(CAN1_CCCR,0x00000000,4); //Disable the config change mode
       cpu_wr_single(CAN0_CCCR,wr_data,4); //Disable the config change mode
       post_log("..loop.. 3\n");
       while (((cpu_rd_single(CAN0_CCCR,4)) & 0x1) == 1);
       post_log(" API- Configuration change complete \n");
       rd_data=cpu_rd_single(CAN0_CCCR,4);
       post_log(" API-CAN0_CCCR RD DATA - Address = %0x  Data = %0x\n",CAN0_CCCR,rd_data);

       post_log(" WAIT FOR RECEIVED DATA ......\n");

	   #if 0
		  //alternate check

		  do {
                 rd_data=cpu_rd_single(CAN1_IR,4);
                  post_log(" API-CAN1_IR RD DATA - Address = %0x  Data = %0x\n",CAN1_IR,rd_data);
                 } while ((rd_data & 0x00000002) ==0);
		    post_log(" Transmission Successful \n");

		// can we check newdata - if buffer enabled
	   #endif

       return 0;
  }

/*  --------------------------- function ----------------------------------- */
uint32_t CAN0_FD_RX_api(uint32_t brp,uint32_t tseg1,uint32_t tseg2,uint32_t sjw,uint32_t brp_fd,uint32_t tseg1_fd,uint32_t tseg2_fd,uint32_t sjw_fd,uint32_t can_mode,uint32_t wtr_mrk0)
{

  uint32_t rd_data;
  uint32_t wr_data;
//  uint32_t value;
  uint32_t mask;
//  static uint32_t i;
  uint32_t watermark;

    post_log(" IN FD CONFIG TASK");
    cpu_wr_single(CAN0_CCCR,0x1,4);
    rd_data=cpu_rd_single(CAN0_CCCR,4);
    while (((rd_data) & 0x1) ==0);
    post_log(" API-CCR RD DATA - Address = %0x  Data = %0x\n",CAN0_CCCR,rd_data);

    post_log(" API- Set configuration change bit and init bit \n");

    cpu_wr_single(CAN0_CCCR,0x3,4);
    do {
        rd_data=cpu_rd_single(CAN0_CCCR,4);
         post_log(" API-CCR RD DATA - Address = %0x  Data = %0x\n",CAN0_CCCR,rd_data);
        } while (rd_data !=0x3);

    post_log(" API-CCR RD DATA - Address = %0x  Data = %0x\n",CAN0_CCCR,rd_data);
    post_log(" API-TEST MODE - Configuration change bit and init bit is set \n");

    wr_data = 0xa03; // Configuring for long and fast mode
    cpu_wr_single(CAN0_CCCR,wr_data,4);
    rd_data=cpu_rd_single(CAN0_CCCR,4);
    post_log(" API-CCR RD DATA - Address = %0x  Data = %0x\n",CAN0_CCCR,rd_data);

// Configure CAN for 500 KBIT/s

    rd_data = cpu_rd_single(CAN0_BTP, 4);
    post_log(" rd_data:= %0x\n",rd_data);
    mask = 0x00000000;		//0x03FF3FFF- Data Mask gives default value A33
    wr_data = rd_data & mask;
    wr_data = (wr_data				  	     |
              (brp     	<< CAN0_BTP__BRP_R) 		     |
              (tseg1   	<< CAN0_BTP__TSEG1_R)                |
              (tseg2    << CAN0_BTP__TSEG2_R)       	     |
              (sjw	<< CAN0_BTP__SJW_R)
              );
    post_log(" Write data BTP Value:= %0x\n",wr_data);
    cpu_wr_single(CAN0_BTP,wr_data,4);
    rd_data= cpu_rd_single(CAN0_BTP,4);
    post_log(" API-CAN0_BTP RD DATA - Address = %0x  Data = %0x\n",CAN0_BTP,rd_data);

//Configure CAN for FD mode

     rd_data = cpu_rd_single(CAN0_FBTP, 4);
     post_log(" rd_data:= %0x\n",rd_data);
     mask = 0x00000000;		//0x03FF3FFF- Data Mask gives default value A33
     wr_data = rd_data & mask;
     wr_data = (wr_data			        |
               (brp_fd     << CAN0_FBTP__FBRP_R)     |
               (tseg1_fd   << CAN0_FBTP__FTSEG1_R)   |
               (tseg2_fd   << CAN0_FBTP__FTSEG2_R)   |
               (sjw_fd     << CAN0_FBTP__FSJW_R)
               );
     post_log(" Write data FBTP Value:= %0x\n",wr_data);

     cpu_wr_single(CAN0_FBTP,wr_data,4);
     rd_data= cpu_rd_single(CAN0_FBTP,4);
     post_log(" API-CAN0_FBTP RD DATA - Address = %0x  Data = %0x\n",CAN0_FBTP,rd_data);

/* RX FIFO*/

     watermark = wtr_mrk0 << 16;
     post_log("Water mark0 = %0x\n",watermark);
     cpu_wr_single(CAN0_RXF0C,watermark,4); // Size-10  start address: 0x1000

     rd_data= cpu_rd_single(CAN0_RXESC,4);
     post_log(" API-CAN0_RXESC RD DATA - Address = %0x  Data = %0x\n",CAN0_RXESC,rd_data);
     wr_data = 0x00000777 | rd_data;
     cpu_wr_single(CAN0_RXESC,wr_data,4); // Configuring the Rx buffer for max data size in FD mode
     post_log(" Write data RXESC Value:= %0x\n",wr_data);

///* Enable transmission complete interrupt */

    cpu_wr_single(CAN0_IE,0x00000002,4);

// /* Enable interrupt line 1*/

    cpu_wr_single(CAN0_ILE,0x00000002,4);
    cpu_wr_single(CAN0_ILS,0xFFFFFFFF,4);

// Configure Filter start addr in RAM

     cpu_wr_single(CAN0_SIDFC,0x00800800,4); // Filter_11 : start address: 0x1800 size: 0x80 end address: 0x09FF
     cpu_wr_single(CAN0_XIDFC,0x00010A00,4); // Filter_29 : start address: 0x0A00 size: 0x40 end address: 0x0BFF

// Configure Filter
     cpu_wr_single((CAN_0_MESSAGE_RAM_BASE +0xA00),0x1F000020,4);

     rd_data= cpu_rd_single(CAN0_CCCR,4);
     wr_data = rd_data & 0xFFFFFFF0;
     cpu_wr_single(CAN0_CCCR,wr_data,4); //Disable the config change mode
     while (((cpu_rd_single(CAN0_CCCR,4)) & 0x1) == 1);
     post_log(" API- Configuration change complete \n");
     rd_data=cpu_rd_single(CAN0_CCCR,4);
     post_log(" API-CAN0_CCCR RD DATA - Address = %0x  Data = %0x\n",CAN0_CCCR,rd_data);

     post_log(" WAIT FOR RECEIVED DATA ......\n");

     return 0;

}

/*  --------------------------- function ----------------------------------- */
uint32_t CAN1_FD_RX_api(uint32_t brp,uint32_t tseg1,uint32_t tseg2,uint32_t sjw,uint32_t brp_fd,uint32_t tseg1_fd,uint32_t tseg2_fd,uint32_t sjw_fd,uint32_t can_mode,uint32_t wtr_mrk1)
{

  uint32_t rd_data;
  uint32_t wr_data;
 // uint32_t value;
  uint32_t mask;
//  static uint32_t i;
  uint32_t watermark;

    cpu_wr_single(CAN1_CCCR,0x1,4);
    rd_data=cpu_rd_single(CAN1_CCCR,4);
    while (((rd_data) & 0x1) ==0);
    post_log(" API-CCR RD DATA - Address = %0x  Data = %0x\n",CAN1_CCCR,rd_data);

    post_log(" API- Set configuration change bit and init bit \n");

    cpu_wr_single(CAN1_CCCR,0x3,4);
    do {
        rd_data=cpu_rd_single(CAN1_CCCR,4);
         post_log(" API-CCR RD DATA - Address = %0x  Data = %0x\n",CAN1_CCCR,rd_data);
        } while (rd_data !=0x3);

    post_log(" API-CCR RD DATA - Address = %0x  Data = %0x\n",CAN1_CCCR,rd_data);
    post_log(" API-TEST MODE - Configuration change bit and init bit is set \n");

    wr_data = 0xa03; // Configuring for Long and fast mode
    cpu_wr_single(CAN1_CCCR,wr_data,4);
    rd_data=cpu_rd_single(CAN1_CCCR,4);
    post_log(" API-CCR RD DATA - Address = %0x  Data = %0x\n",CAN1_CCCR,rd_data);

// Configure CAN for 500 KBIT/s

    rd_data = cpu_rd_single(CAN1_BTP, 4);
    post_log(" rd_data:= %0x\n",rd_data);
    mask = 0x00000000;		//0x03FF3FFF- Data Mask gives default value A33
    wr_data = rd_data & mask;
    wr_data = (wr_data				  	     |
              (brp     	<< CAN1_BTP__BRP_R) 		     |
              (tseg1   	<< CAN1_BTP__TSEG1_R)                |
              (tseg2    << CAN1_BTP__TSEG2_R)       	     |
              (sjw	<< CAN1_BTP__SJW_R)
              );
    post_log(" Write data BTP Value:= %0x\n",wr_data);
    cpu_wr_single(CAN1_BTP,wr_data,4);
    rd_data= cpu_rd_single(CAN1_BTP,4);
    post_log(" API-CAN1_BTP RD DATA - Address = %0x  Data = %0x\n",CAN1_BTP,rd_data);

//Configure CAN for FD mode

     rd_data = cpu_rd_single(CAN1_FBTP, 4);
     post_log(" rd_data:= %0x\n",rd_data);
     mask = 0x00000000;		//0x03FF3FFF- Data Mask gives default value A33
     wr_data = rd_data & mask;
     wr_data = (wr_data			        |
               (brp_fd     << CAN1_FBTP__FBRP_R)     |
               (tseg1_fd   << CAN1_FBTP__FTSEG1_R)   |
               (tseg2_fd   << CAN1_FBTP__FTSEG2_R)   |
               (sjw_fd     << CAN1_FBTP__FSJW_R)
               );
     post_log(" Write data FBTP Value:= %0x\n",wr_data);

     cpu_wr_single(CAN1_FBTP,wr_data,4);
     rd_data= cpu_rd_single(CAN1_FBTP,4);
     post_log(" API-CAN1_FBTP RD DATA - Address = %0x  Data = %0x\n",CAN1_FBTP,rd_data);

/* RX FIFO*/

     watermark = wtr_mrk1 << 16;
     watermark = watermark | 0x1000;
     post_log("Water mark1 = %0x\n",watermark);
     cpu_wr_single(CAN1_RXF0C,watermark,4); // Size-10  start address: 0x1000

     rd_data= cpu_rd_single(CAN1_RXESC,4);
     post_log(" API-CAN1_RXESC RD DATA - Address = %0x  Data = %0x\n",CAN1_RXESC,rd_data);
     wr_data = 0x00000777 | rd_data;
     cpu_wr_single(CAN1_RXESC,wr_data,4); // Configuring the Rx buffer for max data size in FD mode
     post_log(" Write data RXESC Value:= %0x\n",wr_data);

///* Enable transmission complete interrupt */

    cpu_wr_single(CAN1_IE,0x0000002,4);

// /* Enable interrupt line 1*/

    cpu_wr_single(CAN1_ILE,0x00000002,4);
    cpu_wr_single(CAN1_ILS,0xFFFFFFFF,4);

// Configure Filter start addr in RAM

     cpu_wr_single(CAN1_SIDFC,0x00801800,4); // Filter_11 : start address: 0x1800 size: 0x80 end address: 0x09FF
     cpu_wr_single(CAN1_XIDFC,0x00011A00,4); // Filter_29 : start address: 0x0A00 size: 0x40 end address: 0x0BFF

// Configure Filter
     cpu_wr_single((CAN_1_MESSAGE_RAM_BASE +0xA00),0x1F001020,4);

     rd_data= cpu_rd_single(CAN1_CCCR,4);
     wr_data = rd_data & 0xFFFFFFF0;
     cpu_wr_single(CAN1_CCCR,wr_data,4); //Disable the config change mode
     while (((cpu_rd_single(CAN1_CCCR,4)) & 0x1) == 1);
     post_log(" API- Configuration change complete \n");
     rd_data=cpu_rd_single(CAN1_CCCR,4);
     post_log(" API-CAN1_CCCR RD DATA - Address = %0x  Data = %0x\n",CAN1_CCCR,rd_data);

     post_log(" WAIT FOR RECEIVED DATA ......\n");

     return 0;

}

/*  --------------------------- function ----------------------------------- -----------------*/
uint32_t CAN0_FD_TX_api(uint32_t brp,uint32_t tseg1,uint32_t tseg2,uint32_t sjw,uint32_t brp_fd,uint32_t tseg1_fd,uint32_t tseg2_fd,uint32_t sjw_fd,uint32_t can_mode,uint32_t msg_id,uint32_t dlc,uint32_t data0,uint32_t data1,uint32_t data2,uint32_t data3)
{

  uint32_t rd_data;
  uint32_t wr_data;
  uint32_t value;
  uint32_t mask;
  uint32_t temp;
//  static uint32_t i;

    cpu_wr_single(CAN0_CCCR,0x1,4);
    rd_data=cpu_rd_single(CAN0_CCCR,4);
    while (((rd_data) & 0x1) ==0);
    post_log(" API-CCR RD DATA - Address = %0x  Data = %0x\n",CAN0_CCCR,rd_data);

    post_log(" API- Set configuration change bit and init bit \n");

    cpu_wr_single(CAN0_CCCR,0x3,4);
    do {
        rd_data=cpu_rd_single(CAN0_CCCR,4);
         post_log(" API-CCR RD DATA - Address = %0x  Data = %0x\n",CAN0_CCCR,rd_data);
        } while (rd_data !=0x3);

    post_log(" API-CCR RD DATA - Address = %0x  Data = %0x\n",CAN0_CCCR,rd_data);
    post_log(" API-TEST MODE - Configuration change bit and init bit is set \n");

    wr_data = can_mode ? 0x3a03 : 0x1503;
    cpu_wr_single(CAN0_CCCR,wr_data,4);
    rd_data=cpu_rd_single(CAN0_CCCR,4);
    post_log(" API-CCR RD DATA - Address = %0x  Data = %0x\n",CAN0_CCCR,rd_data);

//Configure CAN for 500 KBIT/s

    rd_data = cpu_rd_single(CAN0_BTP, 4);
    post_log(" rd_data:= %0x\n",rd_data);
    mask = 0x00000000;		//0x03FF3FFF- Data Mask gives default value A33
    wr_data = rd_data & mask;
    wr_data = (wr_data			  	     |
              (brp    	<< CAN0_BTP__BRP_R)  	     |
              (tseg1	<< CAN0_BTP__TSEG1_R)        |
              (tseg2    << CAN0_BTP__TSEG2_R) 	     |
              (sjw	<< CAN0_BTP__SJW_R)
              );
    post_log(" Write data BTP Value:= %0x\n",wr_data);

    cpu_wr_single(CAN0_BTP,wr_data,4);
    rd_data= cpu_rd_single(CAN0_BTP,4);
    post_log(" API-CAN0_BTP RD DATA - Address = %0x  Data = %0x\n",CAN0_BTP,rd_data);

//Configure CAN for FD mode

    if (can_mode == 1)
    {
       rd_data = cpu_rd_single(CAN0_FBTP, 4);
       post_log(" rd_data:= %0x\n",rd_data);
       mask = 0x00000000;		//0x03FF3FFF- Data Mask gives default value A33
       wr_data = rd_data & mask;
       wr_data = (wr_data			       |
                 (brp_fd     << CAN0_FBTP__FBRP_R)     |
                 (tseg1_fd   << CAN0_FBTP__FTSEG1_R)   |
                 (tseg2_fd   << CAN0_FBTP__FTSEG2_R)   |
                 (sjw_fd     << CAN0_FBTP__FSJW_R)
                 );
       post_log(" Write data FBTP Value:= %0x\n",wr_data);

       cpu_wr_single(CAN0_FBTP,wr_data,4);
       rd_data= cpu_rd_single(CAN0_FBTP,4);
       post_log(" API-CAN0_FBTP RD DATA - Address = %0x  Data = %0x\n",CAN0_FBTP,rd_data);
    }

/* M_CAN is configured with one dedicated transmit buffer
     The TX buffer start address is 0x0000.*/

    cpu_wr_single(CAN0_TXBC,0x05000000,4);
    rd_data= cpu_rd_single(CAN0_TXBC,4);
    post_log(" API-CAN0_TXBC RD DATA - Address = %0x  Data = %0x\n",CAN0_TXBC,rd_data);

    cpu_wr_single(CAN_0_MESSAGE_RAM_BASE,msg_id,4);
    value = 0x4;
    cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),dlc,4);
    value = value + 0x4;
    cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data0,4);
    value = value + 0x4;
    cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data1,4);
    value = value + 0x4;
    temp = dlc >> 16;
    wr_data = 0;
    if (temp > 8)
    {
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data2,4);
      value = value + 0x4;
      wr_data = 0x1;
    }
    if (temp > 9)
    {
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data3,4);
      value = value + 0x4;
      wr_data = 0x2;
    }
    if (temp > 10)
    {
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data0,4);
      value = value + 0x4;
      wr_data = 0x3;
    }
    if (temp > 11)
    {
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data1,4);
      value = value + 0x4;
      wr_data = 0x4;
    }
    if (temp > 12)
    {
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data2,4);
      value = value + 0x4;
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data3,4);
      value = value + 0x4;
      wr_data = 0x5;
    }
    if (temp > 13)
    {
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data0,4);
      value = value + 0x4;
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data1,4);
      value = value + 0x4;
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data2,4);
      value = value + 0x4;
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data3,4);
      value = value + 0x4;
      wr_data = 0x6;
    }
    if (temp > 14)
    {
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data0,4);
      value = value + 0x4;
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data1,4);
      value = value + 0x4;
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data2,4);
      value = value + 0x4;
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data3,4);
      value = value + 0x4;
      wr_data = 0x7;
    }

///* Enable transmission complete interrupt tx message 1*/
    cpu_wr_single(CAN_0_MESSAGE_RAM_BASE+value,msg_id+0x1,4);
    value = value + 0x4;
    cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),dlc,4);
    value = value + 0x4;
    cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data0,4);
    value = value + 0x4;
    cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data1,4);
    value = value + 0x4;
    if (temp > 8)
    {
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data2,4);
      value = value + 0x4;
      wr_data = 0x1;
    }
    if (temp > 9)
    {
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data3,4);
      value = value + 0x4;
      wr_data = 0x2;
    }
    if (temp > 10)
    {
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data0,4);
      value = value + 0x4;
      wr_data = 0x3;
    }
    if (temp > 11)
    {
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data1,4);
      value = value + 0x4;
      wr_data = 0x4;
    }
    if (temp > 12)
    {
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data2,4);
      value = value + 0x4;
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data3,4);
      value = value + 0x4;
      wr_data = 0x5;
    }
    if (temp > 13)
    {
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data0,4);
      value = value + 0x4;
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data1,4);
      value = value + 0x4;
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data2,4);
      value = value + 0x4;
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data3,4);
      value = value + 0x4;
      wr_data = 0x6;
    }
    if (temp > 14)
    {
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data0,4);
      value = value + 0x4;
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data1,4);
      value = value + 0x4;
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data2,4);
      value = value + 0x4;
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data3,4);
      value = value + 0x4;
      wr_data = 0x7;
    }

    cpu_wr_single(CAN_0_MESSAGE_RAM_BASE+value,msg_id+0x2,4);
    value = value + 0x4;
    cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),dlc,4);
    value = value + 0x4;
    cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data0,4);
    value = value + 0x4;
    cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data1,4);
    value = value + 0x4;
    if (temp > 8)
    {
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data2,4);
      value = value + 0x4;
    }
    if (temp > 9)
    {
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data3,4);
      value = value + 0x4;
    }
    if (temp > 10)
    {
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data0,4);
      value = value + 0x4;
      wr_data = 0x3;
    }
    if (temp > 11)
    {
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data1,4);
      value = value + 0x4;
      wr_data = 0x4;
    }
    if (temp > 12)
    {
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data2,4);
      value = value + 0x4;
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data3,4);
      value = value + 0x4;
      wr_data = 0x5;
    }
    if (temp > 13)
    {
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data0,4);
      value = value + 0x4;
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data1,4);
      value = value + 0x4;
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data2,4);
      value = value + 0x4;
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data3,4);
      value = value + 0x4;
      wr_data = 0x6;
    }
    if (temp > 14)
    {
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data0,4);
      value = value + 0x4;
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data1,4);
      value = value + 0x4;
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data2,4);
      value = value + 0x4;
      cpu_wr_single((CAN_0_MESSAGE_RAM_BASE+value),data3,4);
      value = value + 0x4;
      wr_data = 0x7;
    }

    cpu_wr_single(CAN0_TXBTIE,0x00000001,4);
    cpu_wr_single(CAN0_TXESC,wr_data,4);

///* Enable transmission complete interrupt */

    cpu_wr_single(CAN0_IE,0x00000a00,4);

// /* Enable interrupt line 1*/

    cpu_wr_single(CAN0_ILE,0x00000002,4);
    cpu_wr_single(CAN0_ILS,0xFFFFFFFF,4);

    rd_data= cpu_rd_single(CAN0_CCCR,4);
    wr_data = rd_data & 0xFFFFFFF0;
    cpu_wr_single(CAN0_CCCR,wr_data,4); //Disable the config change mode
    while (((cpu_rd_single(CAN0_CCCR,4)) & 0x1) == 1);
    post_log(" API- Configuration change complete \n");
    rd_data=cpu_rd_single(CAN0_CCCR,4);
    post_log(" API-CAN0_CCCR RD DATA - Address = %0x  Data = %0x\n",CAN0_CCCR,rd_data);

/*Enable Transmission*/

    wr_data=0x00000007;
    cpu_wr_single(CAN0_TXBAR,wr_data,4);
    post_log(" API-CAN0_TXBAR WR DATA - Address = %0x  Data = %0x\n",CAN0_TXBAR,wr_data);

    rd_data= cpu_rd_single(CAN0_TXBAR,4);
    post_log(" API-CAN0_TXBAR RD DATA - Address = %0x  Data = %0x\n",CAN0_TXBAR,rd_data);

/* Wait for transmission occurred flag*/
    post_log(" Waiting for transmission occurred flag...\n");

    return 0;
}

/*  --------------------------- function ----------------------------------- -----------------*/
uint32_t CAN1_FD_TX_api(uint32_t brp,uint32_t tseg1,uint32_t tseg2,uint32_t sjw,uint32_t brp_fd,uint32_t tseg1_fd,uint32_t tseg2_fd,uint32_t sjw_fd,uint32_t can_mode,uint32_t msg_id,uint32_t dlc,uint32_t data0,uint32_t data1,uint32_t data2,uint32_t data3)
{

  uint32_t rd_data;
  uint32_t wr_data;
  uint32_t value;
  uint32_t mask;
  uint32_t temp;
//  static uint32_t i;

    cpu_wr_single(CAN1_CCCR,0x1,4);
    rd_data=cpu_rd_single(CAN1_CCCR,4);
    while (((rd_data) & 0x1) ==0);
    post_log(" API-CCR RD DATA - Address = %0x  Data = %0x\n",CAN1_CCCR,rd_data);

    post_log(" API- Set configuration change bit and init bit \n");

    cpu_wr_single(CAN1_CCCR,0x3,4);
    do {
        rd_data=cpu_rd_single(CAN1_CCCR,4);
         post_log(" API-CCR RD DATA - Address = %0x  Data = %0x\n",CAN1_CCCR,rd_data);
        } while (rd_data !=0x3);

    post_log(" API-CCR RD DATA - Address = %0x  Data = %0x\n",CAN1_CCCR,rd_data);
    post_log(" API-TEST MODE - Configuration change bit and init bit is set \n");

    wr_data = can_mode ? 0x3a03 : 0x1503;
    cpu_wr_single(CAN1_CCCR,wr_data,4);
    rd_data=cpu_rd_single(CAN1_CCCR,4);
    post_log(" API-CCR RD DATA - Address = %0x  Data = %0x\n",CAN1_CCCR,rd_data);

//Configure CAN for 500 KBIT/s

     rd_data = cpu_rd_single(CAN1_BTP, 4);
     post_log(" rd_data:= %0x\n",rd_data);
     mask = 0x00000000;		//0x03FF3FFF- Data Mask gives default value A33
     wr_data = rd_data & mask;
     wr_data = (wr_data			  	     |
               (brp    	<< CAN1_BTP__BRP_R)  	     |
               (tseg1	<< CAN1_BTP__TSEG1_R)        |
               (tseg2   << CAN1_BTP__TSEG2_R) 	     |
               (sjw	<< CAN1_BTP__SJW_R)
               );
     post_log(" Write data BTP Value:= %0x\n",wr_data);

     cpu_wr_single(CAN1_BTP,wr_data,4);
     rd_data= cpu_rd_single(CAN1_BTP,4);
     post_log(" API-CAN1_BTP RD DATA - Address = %0x  Data = %0x\n",CAN1_BTP,rd_data);

//Configure CAN for FD mode

     if (can_mode == 1)
     {
        rd_data = cpu_rd_single(CAN1_FBTP, 4);
        post_log(" rd_data:= %0x\n",rd_data);
        mask = 0x00000000;		//0x03FF3FFF- Data Mask gives default value A33
        wr_data = rd_data & mask;
        wr_data = (wr_data			        |
                  (brp_fd     << CAN1_FBTP__FBRP_R)     |
                  (tseg1_fd   << CAN1_FBTP__FTSEG1_R)   |
                  (tseg2_fd   << CAN1_FBTP__FTSEG2_R)   |
                  (sjw_fd     << CAN1_FBTP__FSJW_R)
                  );
        post_log(" Write data FBTP Value:= %0x\n",wr_data);

        cpu_wr_single(CAN1_FBTP,wr_data,4);
        rd_data= cpu_rd_single(CAN1_FBTP,4);
        post_log(" API-CAN1_FBTP RD DATA - Address = %0x  Data = %0x\n",CAN1_FBTP,rd_data);
     }

/* M_CAN is configured with one dedicated transmit buffer
     The TX buffer start address is 0x0000.*/

     cpu_wr_single(CAN1_TXBC,0x02001000,4);
     rd_data= cpu_rd_single(CAN1_TXBC,4);
     post_log(" API-CAN1_TXBC RD DATA - Address = %0x  Data = %0x\n",CAN1_TXBC,rd_data);

     cpu_wr_single(CAN_1_MESSAGE_RAM_BASE,msg_id,4);
     value = 0x4;
     cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),dlc,4);
     value = value + 0x4;
     cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data0,4);
     value = value + 0x4;
     cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data1,4);
     value = value + 0x4;
     temp = dlc >> 16;
     wr_data = 0;
     if (temp > 8)
     {
       cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data2,4);
       value = value + 0x4;
       wr_data = 0x1;
     }
     if (temp > 9)
     {
       cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data3,4);
       value = value + 0x4;
       wr_data = 0x2;
     }
     if (temp > 10)
     {
       cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data0,4);
       value = value + 0x4;
       wr_data = 0x3;
     }
     if (temp > 11)
     {
       cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data1,4);
       value = value + 0x4;
       wr_data = 0x4;
     }
     if (temp > 12)
     {
       cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data2,4);
       value = value + 0x4;
       cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data3,4);
       value = value + 0x4;
       wr_data = 0x5;
     }
     if (temp > 13)
     {
       cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data0,4);
       value = value + 0x4;
       cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data1,4);
       value = value + 0x4;
       cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data2,4);
       value = value + 0x4;
       cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data3,4);
       value = value + 0x4;
       wr_data = 0x6;
     }
     if (temp > 14)
     {
       cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data0,4);
       value = value + 0x4;
       cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data1,4);
       value = value + 0x4;
       cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data2,4);
       value = value + 0x4;
       cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data3,4);
       value = value + 0x4;
       wr_data = 0x7;
     }

///* Enable transmission complete interrupt tx message 1*/
     cpu_wr_single(CAN_1_MESSAGE_RAM_BASE+value,msg_id+0x1,4);
     value = value + 0x4;
     cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),dlc,4);
     value = value + 0x4;
     cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data0,4);
     value = value + 0x4;
     cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data1,4);
     value = value + 0x4;
     temp = dlc >> 16;
     if (temp > 8)
     {
       cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data2,4);
       value = value + 0x4;
       wr_data = 0x1;
     }
     if (temp > 9)
     {
       cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data3,4);
       value = value + 0x4;
       wr_data = 0x2;
     }
     if (temp > 10)
     {
       cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data0,4);
       value = value + 0x4;
       wr_data = 0x3;
     }
     if (temp > 11)
     {
       cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data1,4);
       value = value + 0x4;
       wr_data = 0x4;
     }
     if (temp > 12)
     {
       cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data2,4);
       value = value + 0x4;
       cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data3,4);
       value = value + 0x4;
       wr_data = 0x5;
     }
     if (temp > 13)
     {
       cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data0,4);
       value = value + 0x4;
       cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data1,4);
       value = value + 0x4;
       cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data2,4);
       value = value + 0x4;
       cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data3,4);
       value = value + 0x4;
       wr_data = 0x6;
     }
     if (temp > 14)
     {
       cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data0,4);
       value = value + 0x4;
       cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data1,4);
       value = value + 0x4;
       cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data2,4);
       value = value + 0x4;
       cpu_wr_single((CAN_1_MESSAGE_RAM_BASE+value),data3,4);
       value = value + 0x4;
       wr_data = 0x7;
     }

     cpu_wr_single(CAN1_TXBTIE,0x00000001,4);
     cpu_wr_single(CAN1_TXESC,wr_data,4);

///* Enable transmission complete interrupt */

     cpu_wr_single(CAN1_IE,0x00000a00,4);

// /* Enable interrupt line 1*/

     cpu_wr_single(CAN1_ILE,0x00000002,4);
     cpu_wr_single(CAN1_ILS,0xFFFFFFFF,4);

     rd_data= cpu_rd_single(CAN1_CCCR,4);
     wr_data = rd_data & 0xFFFFFFF0;
     cpu_wr_single(CAN1_CCCR,wr_data,4); //Disable the config change mode
     while (((cpu_rd_single(CAN1_CCCR,4)) & 0x1) == 1);
     post_log(" API- Configuration change complete \n");
     rd_data=cpu_rd_single(CAN1_CCCR,4);
     post_log(" API-CAN1_CCCR RD DATA - Address = %0x  Data = %0x\n",CAN1_CCCR,rd_data);

/*Enable Transmission*/

     wr_data=0x00000003;
     cpu_wr_single(CAN1_TXBAR,wr_data,4);
     post_log(" API-CAN1_TXBAR WR DATA - Address = %0x  Data = %0x\n",CAN1_TXBAR,wr_data);

     rd_data= cpu_rd_single(CAN1_TXBAR,4);
     post_log(" API-CAN1_TXBAR RD DATA - Address = %0x  Data = %0x\n",CAN1_TXBAR,rd_data);

/* Wait for transmission occurred flag*/
     post_log(" Waiting for transmission occurred flag...\n");

     return 0;
}
/*********************/
static int can_transfer_frame_0(void)

{
uint32_t brp,wtr_mrk1,tseg1,tseg2,sjw,msg_id,dlc,data0,data1,rd_data,tx_status;
int status = TEST_PASS;


            //can_clk_cdru_pll_ch3_5div_reg(0,0,40,0,32,0,100);//pll_sel,mdiv-ch3,ch4,ch5

			can_clk_cdru_genpll_ctl4_reg(0,160,1);//ndiv_frac,ndiv_int,sw_sel     ------genpll settings
			CAN_add_delay(6000);

			can_clk_cdru_pll_ch3_5div_reg(0,0,0,0,0,0,200);//pll_sel,mdiv-ch3,ch4,ch5

			CAN_add_delay(6000);

			cpu_wr_single(CRMU_IOMUX_CTRL6 ,0x00001100 ,4);
			cpu_wr_single(ASIU_TOP_PAD_CTRL_1,0x00000080 ,4);

			CAN_add_delay(6000);

			data1 = cpu_rd_single(ASIU_TOP_CLK_GATING_CTRL,4);
            data1 = data1 | (1<< ASIU_TOP_CLK_GATING_CTRL__CAN_CLK_GATE_EN);
            cpu_wr_single(ASIU_TOP_CLK_GATING_CTRL,data1, 4);

			CAN_add_delay(6000);

			cpu_wr_single(0x180b1008,0,4);
			cpu_wr_single(0x180b100c,0,4);
			cpu_wr_single(0x180b1018,0,4);
			cpu_wr_single(0x180b101c,0,4);
#if 0
			brp = 0;
			
			wtr_mrk1 = 0x0202;
			tseg1 = 0x5;
			tseg2 = 0x3;
			sjw = 0;
#endif

			brp = 4;
		   
			wtr_mrk1 = 0x0202;
			tseg1 = 0x4;
			tseg2 = 0x2;
			sjw = 0x2;


			msg_id = 0x5F001000;
			//dlc = 4<<16;
			dlc = 8 << 16;
			data0 = 0xdeadbeef;
			data1 = 0x55aa55aa;


			post_log("Enter CAN Test\n");

			//post_log("CAN1 in RX mode\n");
		    CAN1_RX_api(brp,tseg1,tseg2,sjw,wtr_mrk1);
			post_log("CAN0 in TX mode\n");
			//CAN0_TX_api(brp,tseg1-1,tseg2-1,sjw,msg_id,dlc,data0,data1);
			tx_status =	CAN0_TX_api(brp,tseg1,tseg2,sjw,msg_id,dlc,data0,data1);

            if (tx_status == 0)
            {
				post_log("Trasmission ERROR..\n");
				status = TEST_FAIL;
				return status;
            }
				
			
			post_log("CAN configurations done\n");
			CAN_add_delay(6000);
			//if (cpu_rd_single(0x180b1008,4) == data0)
			post_log ("\n tx data 0 -%8x",data0);
			post_log ("\n tx data 1 -%8x",data1);

			rd_data = cpu_rd_single(0x180b1008,4);
			post_log("\n rd data %8x",rd_data);
			if ( rd_data != data0)
			{
				status = TEST_FAIL;
			}

			rd_data = cpu_rd_single(0x180b100c,4);
			post_log("\n rd data %8x",rd_data);
			if ( rd_data != data1)
			{
				status = TEST_FAIL;
			}

			rd_data = cpu_rd_single(0x180b1018,4);
			post_log("\n rd data %8x",rd_data);
			if ( rd_data != data0)
			{
				status = TEST_FAIL;
			}

			rd_data = cpu_rd_single(0x180b101c,4);
			post_log("\n rd data %8x",rd_data);
			if ( rd_data != data1)
			{
				status = TEST_FAIL;
			}

			if (status == 0)
			{
				post_log("\n Transmit and received data matches \n");
			}
			else
			{
				post_log("\n Transmit and received data DO NOT match \n");
			}

			return status;
}

static int can_transfer_frame_1(void)

{
uint32_t brp,wtr_mrk1,tseg1,tseg2,sjw,msg_id,dlc,data0,data1,rd_data,tx_status;
int status= TEST_PASS;


            //can_clk_cdru_pll_ch3_5div_reg(0,0,40,0,32,0,100);//pll_sel,mdiv-ch3,ch4,ch5

			can_clk_cdru_genpll_ctl4_reg(0,160,1);//ndiv_frac,ndiv_int,sw_sel     ------genpll settings
			CAN_add_delay(6000);

			can_clk_cdru_pll_ch3_5div_reg(0,0,0,0,0,0,200);//pll_sel,mdiv-ch3,ch4,ch5

			CAN_add_delay(6000);

			cpu_wr_single(CRMU_IOMUX_CTRL6 ,0x00001100 ,4);
			cpu_wr_single(ASIU_TOP_PAD_CTRL_1,0x00000080 ,4);

			CAN_add_delay(6000);

			data1 = cpu_rd_single(ASIU_TOP_CLK_GATING_CTRL,4);
            data1 = data1 | (1<< ASIU_TOP_CLK_GATING_CTRL__CAN_CLK_GATE_EN);
            cpu_wr_single(ASIU_TOP_CLK_GATING_CTRL,data1, 4);

			CAN_add_delay(6000);


			cpu_wr_single(0x180b0108,0,4);
			cpu_wr_single(0x180b010c,0,4);
			cpu_wr_single(0x180b0118,0,4);
			cpu_wr_single(0x180b011c,0,4);


			brp = 4;
		    
			wtr_mrk1 = 0x0202;
			tseg1 = 0x4;
			tseg2 = 0x2;
			sjw = 0x2;


			msg_id = 0x5F001000;
			//dlc = 4<<16;
			dlc = 8 << 16;
			data0 = 0xaabbccdd;
			data1 = 0xeeff55aa;


			post_log("Enter CAN Test\n");


		    CAN0_RX_api(brp,tseg1,tseg2,sjw,wtr_mrk1);
			post_log("CAN1 in TX mode\n");
			tx_status = CAN1_TX_api(brp,tseg1,tseg2,sjw,msg_id,dlc,data0,data1);

			if (tx_status == 0)
					  {
						  post_log("Trasmission ERROR..\n");
						  status = TEST_FAIL;
						  return status;
					  }

			
			post_log("CAN configurations done\n");
			CAN_add_delay(6000);

			post_log ("\n tx data 0 -%8x",data0);
			post_log ("\n tx data 1 -%8x",data1);

			rd_data = cpu_rd_single(0x180b0108,4);
			post_log("\n rd data %8x",rd_data);
			if ( rd_data != data0)
			{
				status = TEST_FAIL;
			}
			rd_data = cpu_rd_single(0x180b010c,4);
			post_log("\n rd data %8x",rd_data);
			if ( rd_data != data1)
			{
				status = TEST_FAIL;
			}
			rd_data = cpu_rd_single(0x180b0118,4);
			post_log("\n rd data %8x",rd_data);
			if ( rd_data != data0)
			{
				status = TEST_FAIL;
			}
			rd_data = cpu_rd_single(0x180b011c,4);
			post_log("\n rd data %8x",rd_data);
			if ( rd_data != data1)
			{
				status = TEST_FAIL;
			}

			if (status == 0)
			{
				post_log("\n Transmit and received data matches \n");
			}
			else
			{
				post_log("\n Transmit and received data DO NOT match \n");
			}
			return status;
}

int can_loop_back_test(void)
{
	int can,status = 0;
	post_log("\n\nSelect CAN Tests: 0 - CAN_0_TX_CAN1_RX; 1 - CAN_1_TX_CAN0_RX \n");
	can = post_getUserInput("Test select ? (0/1) : ");

	if (can == 0)
		status = can_transfer_frame_0();

	if (can == 1)
		status = can_transfer_frame_1();


	return status;

}

/****************************************************/

int CAN_post_test(int flags)
{
  int status = TEST_PASS;

#if 1

     status = TEST_PASS;
        if(TEST_FAIL == can_loop_back_test())
    	      status = TEST_FAIL;
        return status;


#else
  int test_no;
  int loop_count = 0, total_tests = 0, total_regression = 0;

  FUNC_INFO regression[] = {

    		               };

  FUNC_INFO function[]  = {
			                 ADD_TEST(  can_transfer_frame_0      ),
			                 ADD_TEST(  can_transfer_frame_1      )

    		              };


  total_tests = (sizeof(function) / sizeof(function[0]));
  total_regression = (sizeof(regression) / sizeof(regression[0]));
  if(flags & POST_REGRESSION){
    post_log("Total Regression Tests : %d\n",total_regression);
    for(loop_count = 0 ; loop_count  < total_regression ; loop_count++ ){

      post_log("\n---Executing test %2d - %s\n",(loop_count + 1), regression[loop_count].func_name);

      status = (*regression[loop_count].func)();
      switch(status)
      {
        case TEST_PASS:
        t_result->t_pass++;
        break;
        case TEST_FAIL:
        t_result->t_fail++;
        break;
        case TEST_SKIP:
        t_result->t_skip++;
        break;
      }
      post_log("---Test %2d - %s - %s\n\n",(loop_count + 1), regression[loop_count].func_name,status == -1? "FAILED" : "PASSED");
   }
    return 0;
  }
  else{

    post_log("Total Number of Test : %d",total_tests);
    do{
      for(loop_count = 0 ; loop_count  < total_tests ; loop_count++ )
        post_log("\n%2d %s",(loop_count + 1), function[loop_count].func_name);
      post_log("\n%2d exit",(total_tests + 1));
      test_no = post_getUserInput ("\nEnter Test Number : ");
      if(test_no == (total_tests + 1)){
      }
      else if(test_no > total_tests )
        post_log("ERROR - invalid entry\n");
      else
        status += (*function[test_no - 1].func)();
    }while(test_no != (total_tests + 1));

    return status;
  }
#endif
}

#endif
#endif








