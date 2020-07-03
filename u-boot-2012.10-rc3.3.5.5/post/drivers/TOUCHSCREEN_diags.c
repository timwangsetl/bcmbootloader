/* * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <post.h>
#if defined(CONFIG_CYGNUS)
#if CONFIG_POST & CONFIG_SYS_POST_TOUCHSCREEN
#include "../../halapis/include/touchscreen_apis.h"
#include "asm/arch/socregs.h"

#define ADD_TEST(X)  { #X , X }
#define TEST_PASS  0
#define TEST_FAIL -1
#define TEST_SKIP  2

typedef struct{

	char* func_name;
	int (*func)(void);
}FUNC_INFO;


int touchscreen_soft_reset_test(void)
{
	int overall_status = TEST_PASS;
	       uint32_t regdata;
	       uint32_t error;
	       uint32_t cnt;
	       error=0;
	       cnt=0;
	       post_log("Starting the ASIU TOUCH SCREEN SOFT RESET TEST\n");

	touch_screen_sw_reset();
	touch_screen_clk_gate_disable();

	regdata = rd(TSCRegCtl1);
	       cnt++;
	       if(regdata != 0x528070a) {
	           error++;
	           post_log("Error for TSCRegCtl1 with : Actual Read Value: 0x%08X     Expected Reset Value: 0x528070a\n",regdata);
	       }
	       //Regname & Expected Reset Value
	       //REGNAME:TSCRegCtl2
	       //RESTVAL:0x57f
	       regdata = rd(TSCRegCtl2);
	       cnt++;
	       if(regdata != 0x57f) {
	           error++;
	           post_log("Error for TSCRegCtl2 with : Actual Read Value: 0x%08X     Expected Reset Value: 0x57f\n",regdata);
	       }
	       //Regname & Expected Reset Value
	       //REGNAME:TS_Interrupt_Thres
	       //RESTVAL:0xc
	       regdata = rd(TS_Interrupt_Thres);
	       cnt++;
	       if(regdata != 0xc) {
	           error++;
	           post_log("Error for TS_Interrupt_Thres with : Actual Read Value: 0x%08X     Expected Reset Value: 0xc\n",regdata);
	       }
	       //Regname & Expected Reset Value
	       //REGNAME:TS_Interrupt_Mask
	       //RESTVAL:0x0
	       regdata = rd(TS_Interrupt_Mask);
	       cnt++;
	       if(regdata != 0x0) {
	           error++;
	           post_log("Error for TS_Interrupt_Mask with : Actual Read Value: 0x%08X     Expected Reset Value: 0x0\n",regdata);
	       }
	       //Regname & Expected Reset Value
	       //REGNAME:TS_Interrupt_Status
	       //RESTVAL:0x0
	       regdata = rd(TS_Interrupt_Status);
	       cnt++;
	       if(regdata != 0x0) {
	           error++;
	           post_log("Error for TS_Interrupt_Status with : Actual Read Value: 0x%08X     Expected Reset Value: 0x0\n",regdata);
	       }
	       //Regname & Expected Reset Value
	       //REGNAME:TS_Controller_Status
	       //RESTVAL:0x2
	       regdata = rd(TS_Controller_Status);
	       cnt++;
	       if(regdata != 0x2) {
	           error++;
	           post_log("Error for TS_Controller_Status with : Actual Read Value: 0x%08X     Expected Reset Value: 0x2\n",regdata);
	       }
	       //Regname & Expected Reset Value
	       //REGNAME:TS_FIFO_Data
	       //RESTVAL:0xffffffff
	       regdata = rd(TS_FIFO_Data);
	       cnt++;
	       if(regdata != 0xffffffff) {
	           error++;
	           post_log("Error for TS_FIFO_Data with : Actual Read Value: 0x%08X     Expected Reset Value: 0xffffffff\n",regdata);
	       }
	       //Regname & Expected Reset Value
	       //REGNAME:TS_Analog_Control
	       //RESTVAL:0x0
	       regdata = rd(TS_Analog_Control);
	       cnt++;
	       if(regdata != 0x0) {
	           error++;
	           post_log("Error for TS_Analog_Control with : Actual Read Value: 0x%08X     Expected Reset Value: 0x0\n",regdata);
	       }
	       //Regname & Expected Reset Value
	       //REGNAME:TS_Aux_Data
	       //RESTVAL:0x0
	       regdata = rd(TS_Aux_Data);
	       cnt++;
	       if(regdata != 0x0) {
	           error++;
	           post_log("Error for TS_Aux_Data with : Actual Read Value: 0x%08X     Expected Reset Value: 0x0\n",regdata);
	       }
	       //Regname & Expected Reset Value
	       //REGNAME:TS_Debounce_Cntr_Stat
	       //RESTVAL:0x0
	       regdata = rd(TS_Debounce_Cntr_Stat);
	       cnt++;
	       if(regdata != 0x0) {
	           error++;
	           post_log("Error for TS_Debounce_Cntr_Stat with : Actual Read Value: 0x%08X     Expected Reset Value: 0x0\n",regdata);
	       }
	       //Regname & Expected Reset Value
	       //REGNAME:TS_Scan_Cntr_Stat
	       //RESTVAL:0x1
	       regdata = rd(TS_Scan_Cntr_Stat);
	       cnt++;
	       if(regdata != 0x1) {
	           error++;
	           post_log("Error for TS_Scan_Cntr_Stat with : Actual Read Value: 0x%08X     Expected Reset Value: 0x1\n",regdata);
	       }
	       //Regname & Expected Reset Value
	       //REGNAME:TS_Rem_Cntr_Stat
	       //RESTVAL:0x0
	       regdata = rd(TS_Rem_Cntr_Stat);
	       cnt++;
	       if(regdata != 0x0) {
	           error++;
	           post_log("Error for TS_Rem_Cntr_Stat with : Actual Read Value: 0x%08X     Expected Reset Value: 0x0\n",regdata);
	       }
	       //Regname & Expected Reset Value
	       //REGNAME:TS_Settling_Timer_Stat
	       //RESTVAL:0x0
	       regdata = rd(TS_Settling_Timer_Stat);
	       cnt++;
	       if(regdata != 0x0) {
	           error++;
	           post_log("Error for TS_Settling_Timer_Stat with : Actual Read Value: 0x%08X     Expected Reset Value: 0x0\n",regdata);
	       }
	       //Regname & Expected Reset Value
	       //REGNAME:TS_Spare_Reg
	       //RESTVAL:0xffff
	       regdata = rd(TS_Spare_Reg);
	       cnt++;
	       if(regdata != 0xffff) {
	           error++;
	           post_log("Error for TS_Spare_Reg with : Actual Read Value: 0x%08X     Expected Reset Value: 0xffff\n",regdata);
	       }
	       //Regname & Expected Reset Value
	       //REGNAME:TS_Soft_Bypass_Control
	       //RESTVAL:0x0
	       regdata = rd(TS_Soft_Bypass_Control);
	       cnt++;
	       if(regdata != 0x0) {
	           error++;
	           post_log("Error for TS_Soft_Bypass_Control with : Actual Read Value: 0x%08X     Expected Reset Value: 0x0\n",regdata);
	       }
	       //Regname & Expected Reset Value
	       //REGNAME:TS_Soft_Bypass_Data
	       //RESTVAL:0x0
	       regdata = rd(TS_Soft_Bypass_Data);
	       cnt++;
	       if(regdata != 0x0) {
	           error++;
	           post_log("Error for TS_Soft_Bypass_Data with : Actual Read Value: 0x%08X     Expected Reset Value: 0x0\n",regdata);
	       }
	        if(error)
		   {
	          overall_status = TEST_FAIL;
	          post_log("ASIU TOUCH SCREEN SOFT RESET TEST : TEST FAIL\n");
		   }
		   else
		   {
			  overall_status = TEST_PASS;
	          post_log("ASIU TOUCH SCREEN SOFT RESET TEST : TEST PASS\n");
		   }

	        post_log("Error Cnt for ASIU TOUCH SCREEN SOFT RESET TEST : %d\n",error);
	        return overall_status;
}


int basic_touch_test(void)
{
	
/*	 uint32_t timer_in = 0xffff0Bff;//(0xA << TSCRegCtl1__touch_timeout_L) | (0x9 << TSCRegCtl1__settling_timeout_L) | (0x28 << TSCRegCtl1__debounce_timeout_L) | (0x5 << TSCRegCtl1__scanning_period_L);
	 uint32_t avg_data = 0x5;
	 uint32_t mux_sel = 0;
	 uint32_t scan_en = 0;
	 uint32_t fifo_threshold = 1;
	 uint32_t fifo_data;
	 uint32_t mask_in = 5; 
*/

	uint32_t read_data;
	uint32_t x_data;
	uint32_t y_data,i;

	cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL, 0,4);
	touch_screen_sw_reset();
    touch_screen_clk_gate_disable();

	//touch_screen_powerdown();
	//touch_screen_powerup();
    //Touch Screen Power Up register
    cpu_wr_single(0x180A6004,0x502,4);

    //Timer Config
    cpu_wr_single(0x180A6000,0xffff0Bff,4);

    //Analog Control
    cpu_wr_single(0x180A601C,0x0000013,4);

    //Interrupt Threshold
    cpu_wr_single(0x180A6008,1,4);

    //Interrupt Mask REgister
    cpu_wr_single(0x180A600C,0x85,4);




	/*touch_screen_INTR_MASK_api(mask_in);  //Enable the  pen down irq and fifo threshold irq

	    touch_screen_xinput_check_api(fifo_threshold);

	touch_screen_CONFIG_api(timer_in, avg_data, mux_sel, scan_en);*/

    //Enable Scanning
    cpu_wr_single(0x180A6004,0x10502,4);

    for(i=0;i<=1000000;i++);

    post_log("Please touch the Touch Screen Once \n");

    read_data = cpu_rd_single(0x180A6014,4); //TS Controller Status register

    do
    {
    	read_data = cpu_rd_single(0x180A6014,4);
    	read_data = read_data & 0x6;
    	post_log("TS Controller Status Register : %08X \n",read_data);
    	for(i=0;i<=1000000;i++);
    } while ((read_data != 0x4));

    //do
    //{
    	read_data = cpu_rd_single(0x180A6018,4);
    	post_log("The Read Value of FIFO is %08X \n",read_data);
    //	for(i=0;i<=1000000;i++);
   // } while (read_data != 0xFFFFFFFF);





	/*fifo_data = check_fifo_data_api(mask_in,fifo_threshold);
	post_log("FIFO READ DATA : %08X \n",fifo_data);*/

    x_data = read_data & 0xFFFF;
	y_data = (read_data & 0xFFFF0000) >> 16;

    post_log("ADC X_data : %08X \n",x_data);
	post_log("ADC Y data : %08X \n",y_data);

  return TEST_PASS;
}

int TOUCHSCREEN_post_test(int flags)
{
#if 1
	int status = TEST_PASS;
    if(TEST_FAIL == basic_touch_test())
	      status = TEST_FAIL;
    return status;

#else
	uint32_t test_no;
    int loop_count = 0, total_tests = 0 , total_reg_tests = 0;
    int status = 0,autorun_status = 0,pass = 0,fail = 0,skip = 0;

	int ret = 0x00,max_loop = 50;



    FUNC_INFO function[]=	{
	     	 	 //ADD_TEST(  touchscreen_soft_reset_test    ),
	     	 	 ADD_TEST( basic_touch_test )

				} ;

    FUNC_INFO regression_func[]=	{
    		     ADD_TEST(  touchscreen_soft_reset_test    ),

				} ;
    total_tests = (sizeof(function) / sizeof(function[0]));
    total_reg_tests = (sizeof(regression_func) / sizeof(regression_func[0]));
    if(flags & POST_REGRESSION ){

    for(loop_count = 0 ; loop_count  < total_reg_tests ; loop_count++ ){
      post_log("\nExecuting test %2d - %s\n",(loop_count + 1), regression_func[loop_count].func_name);

    status = (*regression_func[loop_count].func)();
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

    }
    return 0;
    }
else{
    post_log("Number of test : %d",total_tests);

    do{
         for(loop_count = 0 ; loop_count  < total_tests ; loop_count++ )
         post_log("\n%2d %s",(loop_count + 1), function[loop_count].func_name);
         post_log("\n%2d run all tests",(total_tests + 1));
         post_log("\n%2d exit\n",(total_tests + 2));
         test_no = post_getUserInput ("\nEnter Test Number : ");

         if(test_no > (total_tests + 2) )
             post_log("ERROR - invalid entry\n");
         else{
                  if(test_no == (total_tests + 1)){
                      autorun_status = 0;
                      pass = 0;
                      fail = 0;
                      skip = 0;
                      for(loop_count = 0 ; loop_count  < total_tests ; loop_count++ ){
                    	  post_log("\nExecuting test no : %2d. %s\n",(loop_count + 1),function[loop_count].func_name);
                    	  autorun_status = (*function[loop_count].func)();
                    	  switch(autorun_status)
                    	  {
                    	  case TEST_PASS:
                    		  pass++;
                    		  break;
                    	  case TEST_FAIL:
                    		  fail++;
                    		  break;
                    	  case TEST_SKIP:
                    		  skip++;
                    		  break;
                    	  }
                      }
                      post_log("Report TOTAL Test => %d - PASS => %d - FAIL => %d - SKIP => %d\n",(pass + fail + skip), pass,fail,skip);
                      if(fail)
                        status = -1;
                  }

                  else if(test_no < (total_tests + 1)){
          	      if(TEST_FAIL == (*function[test_no - 1].func)())
	       	      status = -1;
                  }
             }

    }while(test_no != (total_tests + 2));


    return status;
}
return 0;
#endif
}
#endif
#endif

