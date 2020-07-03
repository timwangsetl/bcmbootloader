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
#if CONFIG_POST & CONFIG_SYS_POST_DS1WM
#include "../../halapis/include/d1wdiags_apis.h"
#include "asm/arch/socregs.h"

#define ADD_TEST(X)  { #X , X }
#define TEST_PASS  0
#define TEST_FAIL -1
#define TEST_SKIP  2




typedef struct{
	
	char* func_name;
	int (*func)(void);
}FUNC_INFO;

int d1w_soft_reset_test(void)
{
	 
	       uint32_t regdata;
	       uint32_t error;
	       uint32_t cnt;
	       error=0;
	       cnt=0;
	       post_log("Starting the ASIU D1W SOFT RESET TEST\n");
	       cpu_wr_single(ASIU_TOP_SW_RESET_CTRL,0x3ef,4);
	       cpu_wr_single(ASIU_TOP_SW_RESET_CTRL,0x3ff,4);
	       d1w_delay(10000);
	       d1w_init(0);

	       regdata = rd(ASIU_D1W_DIN);
	              cnt++;
	              if(regdata != 0x0) {
	                  error++;
	                  //post_log("Error for ASIU_D1W_DIN with : Actual Read Value: 0x%08X     Expected Reset Value: 0x0\n",regdata);
	              }
	              //Regname & Expected Reset Value
	              //REGNAME:ASIU_D1W_DOUT
	              //RESTVAL:0x0
	              regdata = rd(ASIU_D1W_DOUT);
	              cnt++;
	              if(regdata != 0x0) {
	                  error++;
	                  //post_log("Error for ASIU_D1W_DOUT with : Actual Read Value: 0x%08X     Expected Reset Value: 0x0\n",regdata);
	              }
	              //Regname & Expected Reset Value
	              //REGNAME:ASIU_D1W_ADR
	              //RESTVAL:0x0
	              regdata = rd(ASIU_D1W_ADR);
	              cnt++;
	              if(regdata != 0x0) {
	                  error++;
	                  //post_log("Error for ASIU_D1W_ADR with : Actual Read Value: 0x%08X     Expected Reset Value: 0x0\n",regdata);
	              }
	              //Regname & Expected Reset Value
	              //REGNAME:ASIU_D1W_CTL
	              //RESTVAL:0x0
	              regdata = rd(ASIU_D1W_CTL);
	              cnt++;
	              if(regdata != 0x0) {
	                  error++;
	                  //post_log("Error for ASIU_D1W_CTL with : Actual Read Value: 0x%08X     Expected Reset Value: 0x0\n",regdata);
	              }
	            

	               //post_log("Error Cnt for ASIU D1W SOFT RESET: %d\n",error);

	              	                 post_log("ASIU D1W SOFTRESET : TEST PASS\n");
	               return 0;
}

/******* Internal test function start ********/
int d1w_presence_pulse_detect_test(void)
{
	int status = 0;
	post_log("Initializing the D1W module \n");
	d1w_init(0);
	status = rst_prs(0);
	if (status == 0)
		post_log("d1w presence pulse detect test PASSED \n");
	else if (status == -1)
		post_log("d1w presence pulse detect test FAILED \n");

	return status;
}

int d1w_presence_pulse_detect_and_read_rom_test(void)
{
	int status = 0;
	uint32_t rdata,i=0;
		    uint32_t dummy = 0;
	post_log("Initializing the D1W module \n");
	d1w_init(0);
	status = rst_prs(0);
	if (status == 0)
		post_log("d1w presence pulse detect test PASSED \n");
	else if (status == -1)
		post_log("d1w presence pulse detect test FAILED \n");

	d1w_delay(400000000);
			post_log("D1W performing a ROM READ \n");
			write_tx_buf(D1W_ROM_RD);
			d1w_delay(400000000);
			write_tx_buf(0xFF);
			//for(i=0;i<8;i++)
			//{
				rdata = read_rx_buf(dummy);
				post_log("Read data : %d : %08X \n",i,rdata);
			//}
				d1w_delay(400000000);
			status = rst_prs(0);
					if (status == 0)
					{
						post_log("d1w presence pulse detect  PASSED \n");

					}
					else if (status == -1)
					{
						post_log("d1w presence pulse detect  FAILED \n");

					}




	return status;
}

int d1w_read_rom_test(void)
{
	    int status = 0;
	    uint32_t rdata,i;
	    uint32_t dummy = 0;
		post_log("Initializing the D1W module \n");
		d1w_init(0);
		status = rst_prs(0);
		if (status == 0)
			post_log("d1w presence pulse detect  PASSED \n");
		else if (status == -1)
			post_log("d1w presence pulse detect  FAILED \n");

		d1w_delay(10000);
		post_log("D1W performing a ROM READ \n");
		write_tx_buf(D1W_ROM_RD);
		write_tx_buf(0xFF);
		for(i=0;i<8;i++)
		{
			rdata = read_rx_buf(dummy);
			post_log("Read data : %d : %08X \n",i,rdata);
		}

		status = rst_prs(0);
				if (status == 0)
					post_log("d1w presence pulse detect  PASSED \n");
				else if (status == -1)
					post_log("d1w presence pulse detect  FAILED \n");


	return TEST_PASS;
}

int d1w_write_and_read_scratch_pad_test_3(void)
{
	    int status = 0;
	    uint32_t rdata,i=0;
	    uint32_t dummy = 0;
		post_log("Initializing the D1W module \n");
		d1w_init(0);
		status = rst_prs(0);
		if (status == 0)
			post_log("d1w presence pulse detect  PASSED \n");
		else if (status == -1)
			post_log("d1w presence pulse detect  FAILED \n");

		d1w_delay(10000);
		post_log("D1W performing a ROM SKIP COMMAND \n");
		write_tx_buf(D1W_ROM_SKIP);
		post_log("D1W issuing a Scratchpad Write Command \n");
		write_tx_buf(0x4E);
		post_log("D1W writing 8 bytes of data into scratch pad page 3 \n");
		write_tx_buf(0x03);
		write_tx_buf(0xEF);
		write_tx_buf(0xFF);
		write_tx_buf(0xEF);
		write_tx_buf(0xFF);
		write_tx_buf(0xAA);
		write_tx_buf(0xBB);
		write_tx_buf(0xCC);
		write_tx_buf(0xDD);
		status = rst_prs(0);
		if (status == 0)
			post_log("d1w presence pulse detect  PASSED \n");
		else if (status == -1)
			post_log("d1w presence pulse detect  FAILED \n");
		post_log("D1w write to scratch pad page 03 terminated by reset \n");

		post_log("D1W performing a ROM SKIP Command \n");
		write_tx_buf(D1W_ROM_SKIP);
	    post_log("D1W issuing a Scratchpad Read Command \n");
		write_tx_buf(0xBE);
		write_tx_buf(0x03);

		//for(i = 0; i< 9;i++)
		//{
			rdata = read_rx_buf(dummy);
			post_log("Read data : %d : %08X \n",i,rdata);
		//}
		status = rst_prs(0);
		if (status == 0)
				post_log("d1w presence pulse detect  PASSED \n");
		else if (status == -1)
			   post_log("d1w presence pulse detect  FAILED \n");
		//for(i=0;i<8;i++)
		//{
			rdata = read_rx_buf(dummy);
			post_log("Read data : %d : %08X \n",i,rdata);
		//}

		status = rst_prs(0);
				if (status == 0)
					post_log("d1w presence pulse detect  PASSED \n");
				else if (status == -1)
					post_log("d1w presence pulse detect  FAILED \n");
	post_log("D1w read from scratch pad page 03 terminated by reset \n");

	return TEST_PASS;
}

int d1w_write_and_read_scratch_pad_test_4(void)
{
	    int status = 0;
	    uint32_t rdata,i;
	    uint32_t dummy = 0;
		post_log("Initializing the D1W module \n");
		d1w_init(0);
		status = rst_prs(0);
		if (status == 0)
			post_log("d1w presence pulse detect  PASSED \n");
		else if (status == -1)
			post_log("d1w presence pulse detect  FAILED \n");

		d1w_delay(10000);
		post_log("D1W performing a ROM SKIP COMMAND \n");
		write_tx_buf(D1W_ROM_SKIP);
		post_log("D1W issuing a Scratchpad Write Command \n");
		write_tx_buf(0x4E);
		post_log("D1W writing 8 bytes of data into scratch pad page 4 \n");
		write_tx_buf(0x04);
		write_tx_buf(0xEF);
		write_tx_buf(0xFF);
		write_tx_buf(0xEF);
		write_tx_buf(0xFF);
		write_tx_buf(0xAA);
		write_tx_buf(0xBB);
		write_tx_buf(0xCC);
		write_tx_buf(0xDD);
		status = rst_prs(0);
		if (status == 0)
			post_log("d1w presence pulse detect  PASSED \n");
		else if (status == -1)
			post_log("d1w presence pulse detect  FAILED \n");
		post_log("D1w write to scratch pad page 04 terminated by reset \n");

		post_log("D1W performing a ROM SKIP Command \n");
		write_tx_buf(D1W_ROM_SKIP);
	    post_log("D1W issuing a Scratchpad Read Command \n");
		write_tx_buf(0xBE);
		write_tx_buf(0x04);

		for(i = 0; i< 9;i++)
		{
			rdata = read_rx_buf(dummy);
			post_log("Read data : %d : %08X \n",i,rdata);
		}
		status = rst_prs(0);
		if (status == 0)
				post_log("d1w presence pulse detect  PASSED \n");
		else if (status == -1)
			   post_log("d1w presence pulse detect  FAILED \n");
		for(i=0;i<8;i++)
		{
			rdata = read_rx_buf(dummy);
			post_log("Read data : %d : %08X \n",i,rdata);
		}

		status = rst_prs(0);
				if (status == 0)
					post_log("d1w presence pulse detect  PASSED \n");
				else if (status == -1)
					post_log("d1w presence pulse detect  FAILED \n");
	post_log("D1W read from scratch pad page 04 terminated by reset \n");

	return TEST_PASS;
}

int d1w_write_and_read_scratch_pad_test_5(void)
{
	    int status = 0;
	    uint32_t rdata,i;
	    uint32_t dummy = 0;
		post_log("Initializing the D1W module \n");
		d1w_init(0);
		status = rst_prs(0);
		if (status == 0)
			post_log("d1w presence pulse detect  PASSED \n");
		else if (status == -1)
			post_log("d1w presence pulse detect  FAILED \n");

		d1w_delay(1000);
		post_log("D1W performing a ROM SKIP COMMAND \n");
		write_tx_buf(D1W_ROM_SKIP);
		post_log("D1W issuing a Scratchpad Write Command \n");
		write_tx_buf(0x4E);
		post_log("D1W writing 8 bytes of data into scratch pad page 5 \n");
		write_tx_buf(0x05);
		write_tx_buf(0xEF);
		write_tx_buf(0xFF);
		write_tx_buf(0xEF);
		write_tx_buf(0xFF);
		write_tx_buf(0xAA);
		write_tx_buf(0xBB);
		write_tx_buf(0xCC);
		write_tx_buf(0xDD);
		status = rst_prs(0);
		if (status == 0)
			post_log("d1w presence pulse detect  PASSED \n");
		else if (status == -1)
			post_log("d1w presence pulse detect  FAILED \n");
		post_log("D1w write to scratch pad page 05 terminated by reset \n");

		post_log("D1W performing a ROM SKIP Command \n");
		write_tx_buf(D1W_ROM_SKIP);
	    post_log("D1W issuing a Scratchpad Read Command \n");
		write_tx_buf(0xBE);
		write_tx_buf(0x05);

		for(i = 0; i< 9;i++)
		{
			rdata = read_rx_buf(dummy);
			post_log("Read data : %d : %08X \n",i,rdata);
		}
		status = rst_prs(0);
		if (status == 0)
				post_log("d1w presence pulse detect  PASSED \n");
		else if (status == -1)
			   post_log("d1w presence pulse detect  FAILED \n");
		for(i=0;i<8;i++)
		{
			rdata = read_rx_buf(dummy);
			post_log("Read data : %d : %08X \n",i,rdata);
		}

		status = rst_prs(0);
				if (status == 0)
					post_log("d1w presence pulse detect  PASSED \n");
				else if (status == -1)
					post_log("d1w presence pulse detect  FAILED \n");
	post_log("D1w read from scratch pad page 05 terminated by reset \n");

	return TEST_PASS;
}

int d1w_write_and_read_scratch_pad_test_6(void)
{
	    int status = 0;
	    uint32_t rdata,i;
	    uint32_t dummy = 0;
		post_log("Initializing the D1W module \n");
		d1w_init(0);
		status = rst_prs(0);
		if (status == 0)
			post_log("d1w presence pulse detect  PASSED \n");
		else if (status == -1)
			post_log("d1w presence pulse detect  FAILED \n");

		d1w_delay(1000);
		post_log("D1W performing a ROM SKIP COMMAND \n");
		write_tx_buf(D1W_ROM_SKIP);
		post_log("D1W issuing a Scratchpad Write Command \n");
		write_tx_buf(0x4E);
		post_log("D1W writing 8 bytes of data into scratch pad page 6 \n");
		write_tx_buf(0x06);
		write_tx_buf(0xEF);
		write_tx_buf(0xFF);
		write_tx_buf(0xEF);
		write_tx_buf(0xFF);
		write_tx_buf(0xAA);
		write_tx_buf(0xBB);
		write_tx_buf(0xCC);
		write_tx_buf(0xDD);
		status = rst_prs(0);
		if (status == 0)
			post_log("d1w presence pulse detect  PASSED \n");
		else if (status == -1)
			post_log("d1w presence pulse detect  FAILED \n");
		post_log("D1w write to scratch pad page 06 terminated by reset \n");

		post_log("D1W performing a ROM SKIP Command \n");
		write_tx_buf(D1W_ROM_SKIP);
	    post_log("D1W issuing a Scratchpad Read Command \n");
		write_tx_buf(0xBE);
		write_tx_buf(0x06);

		for(i = 0; i< 9;i++)
		{
			rdata = read_rx_buf(dummy);
			post_log("Read data : %d : %08X \n",i,rdata);
		}
		status = rst_prs(0);
		if (status == 0)
				post_log("d1w presence pulse detect  PASSED \n");
		else if (status == -1)
			   post_log("d1w presence pulse detect  FAILED \n");
		for(i=0;i<8;i++)
		{
			rdata = read_rx_buf(dummy);
			post_log("Read data : %d : %08X \n",i,rdata);
		}

		status = rst_prs(0);
				if (status == 0)
					post_log("d1w presence pulse detect  PASSED \n");
				else if (status == -1)
					post_log("d1w presence pulse detect  FAILED \n");
	post_log("D1w read from scratch pad page 06 terminated by reset \n");

	return TEST_PASS;
}

int d1w_write_and_read_scratch_pad_test_7(void)
{
	    int status = 0;
	    uint32_t rdata,i;
	    uint32_t dummy = 0;
		post_log("Initializing the D1W module \n");
		d1w_init(0);
		status = rst_prs(0);
		if (status == 0)
			post_log("d1w presence pulse detect  PASSED \n");
		else if (status == -1)
			post_log("d1w presence pulse detect  FAILED \n");

		d1w_delay(1000);
		post_log("D1W performing a ROM SKIP COMMAND \n");
		write_tx_buf(D1W_ROM_SKIP);
		post_log("D1W issuing a Scratchpad Write Command \n");
		write_tx_buf(0x4E);
		post_log("D1W writing 8 bytes of data into scratch pad page 7 \n");
		write_tx_buf(0x07);
		write_tx_buf(0xEF);
		write_tx_buf(0xFF);
		write_tx_buf(0xEF);
		write_tx_buf(0xFF);
		write_tx_buf(0xAA);
		write_tx_buf(0xBB);
		write_tx_buf(0xCC);
		write_tx_buf(0xDD);
		status = rst_prs(0);
		if (status == 0)
			post_log("d1w presence pulse detect  PASSED \n");
		else if (status == -1)
			post_log("d1w presence pulse detect  FAILED \n");
		post_log("D1w write to scratch pad page 07 terminated by reset \n");

		post_log("D1W performing a ROM SKIP Command \n");
		write_tx_buf(D1W_ROM_SKIP);
	    post_log("D1W issuing a Scratchpad Read Command \n");
		write_tx_buf(0xBE);
		write_tx_buf(0x07);

		for(i = 0; i< 9;i++)
		{
			rdata = read_rx_buf(dummy);
			post_log("Read data : %d : %08X \n",i,rdata);
		}
		status = rst_prs(0);
		if (status == 0)
				post_log("d1w presence pulse detect  PASSED \n");
		else if (status == -1)
			   post_log("d1w presence pulse detect  FAILED \n");
		for(i=0;i<8;i++)
		{
			rdata = read_rx_buf(dummy);
			post_log("Read data : %d : %08X \n",i,rdata);
		}

		status = rst_prs(0);
				if (status == 0)
					post_log("d1w presence pulse detect  PASSED \n");
				else if (status == -1)
					post_log("d1w presence pulse detect  FAILED \n");
	post_log("D1w read from scratch pad page 07 terminated by reset \n");

	return TEST_PASS;
}

int D1WDIAGS_post_test(int flags)
{

   
    int status = 0;
#if 0
	  uint32_t test_no;
		int loop_count = 0, total_tests = 0 , total_reg_tests = 0;
		int status = 0,autorun_status = 0,pass = 0,fail = 0,skip = 0;
#endif	  



#if 1
     status = TEST_PASS;
        if(TEST_FAIL == d1w_presence_pulse_detect_and_read_rom_test())
    	      status = TEST_FAIL;
        return status;
#else





    FUNC_INFO function[]=	{

	     	 	 ADD_TEST(  d1w_presence_pulse_detect_test    ),
	     	 	 ADD_TEST( d1w_presence_pulse_detect_and_read_rom_test ),
	     	 	 ADD_TEST( d1w_write_and_read_scratch_pad_test_3 )

				} ;

    FUNC_INFO regression_func[]=	{
    		                 ADD_TEST(  d1w_soft_reset_test    ),
    			     	 	 ADD_TEST(  d1w_presence_pulse_detect_test    ),
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
