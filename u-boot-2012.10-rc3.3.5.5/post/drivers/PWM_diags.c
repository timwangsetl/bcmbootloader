 /*
 * $Copyright Open Broadcom Corporation$
 */


#include <common.h>
#include <post.h>
#include <iproc_pwmc.h>

#if CONFIG_POST & CONFIG_SYS_POST_PWM
#if defined(CONFIG_CYGNUS)

#include "../../halapis/include/pwmtamper_apis.h"
#include "../../halapis/include/iomux_apis.h"
#include "asm/arch/socregs.h"

#define ADD_TEST(X)  { #X , X }
#define TEST_PASS  0
#define TEST_FAIL -1
#define TEST_SKIP  2

/* Default Register Value */

typedef struct{

	char* func_name;
	int (*func)(void);
}FUNC_INFO;


int pwm_soft_reset_test(void)
{
	int overall_status = TEST_PASS;
	uint32_t regdata;
	       uint32_t error;
	       uint32_t cnt;
		   uint32_t waive;
		   waive = 0;
	       error=0;
	       cnt=0;
	       post_log("Starting the ASIU PWM SOFT RESET TEST\n");
	     wr(ASIU_TOP_CLK_GATING_CTRL,0x7ff);
	 regdata = rd(ASIU_PWM_CONTROL);
	       cnt++;
	       if(regdata != 0x3f3f0000) {
	           waive++;
	           post_log("Waived for ASIU_PWM_CONTROL with : Actual Read Value: 0x%08X     Expected Reset Value: 0x3f3f0000\n",regdata);
	       }
	       //Regname & Expected Reset Value
	       //REGNAME:ASIU_PRESCALE_CONTROL
	       //RESTVAL:0x0
	       regdata = rd(ASIU_PRESCALE_CONTROL);
	       cnt++;
	       if(regdata != 0x0) {
	           error++;
	           post_log("Error for ASIU_PRESCALE_CONTROL with : Actual Read Value: 0x%08X     Expected Reset Value: 0x0\n",regdata);
	       }
	       //Regname & Expected Reset Value
	       //REGNAME:ASIU_PWM_VIBRA
	       //RESTVAL:0x0
	       regdata = rd(ASIU_PWM_VIBRA);
	       cnt++;
	       if(regdata != 0x0) {
	           error++;
	           post_log("Error for ASIU_PWM_VIBRA with : Actual Read Value: 0x%08X     Expected Reset Value: 0x0\n",regdata);
	       }
	       //Regname & Expected Reset Value
	       //REGNAME:ASIU_PWM_DMA_CONTROL
	       //RESTVAL:0x0
	       regdata = rd(ASIU_PWM_DMA_CONTROL);
	       cnt++;
	       if(regdata != 0x0) {
	           error++;
	           post_log("Error for ASIU_PWM_DMA_CONTROL with : Actual Read Value: 0x%08X     Expected Reset Value: 0x0\n",regdata);
	       }
	       //Regname & Expected Reset Value
	       //REGNAME:ASIU_PWM_DMA_INTERVAL_CTL0
	       //RESTVAL:0x0
	       regdata = rd(ASIU_PWM_DMA_INTERVAL_CTL0);
	       cnt++;
	       if(regdata != 0x0) {
	           error++;
	           post_log("Error for ASIU_PWM_DMA_INTERVAL_CTL0 with : Actual Read Value: 0x%08X     Expected Reset Value: 0x0\n",regdata);
	       }
	       //Regname & Expected Reset Value
	       //REGNAME:ASIU_PWM_DMA_INTERVAL_CTL1
	       //RESTVAL:0x0
	       regdata = rd(ASIU_PWM_DMA_INTERVAL_CTL1);
	       cnt++;
	       if(regdata != 0x0) {
	           error++;
	           post_log("Error for ASIU_PWM_DMA_INTERVAL_CTL1 with : Actual Read Value: 0x%08X     Expected Reset Value: 0x0\n",regdata);
	       }
	       //Regname & Expected Reset Value
	       //REGNAME:ASIU_PWM_MONITOR
	       //RESTVAL:0x0
	       regdata = rd(ASIU_PWM_MONITOR);
	       cnt++;
	       if(regdata != 0x0) {
	           error++;
	           post_log("Error for ASIU_PWM_MONITOR with : Actual Read Value: 0x%08X     Expected Reset Value: 0x0\n",regdata);
	       }
	       //Regname & Expected Reset Value
	       //REGNAME:ASIU_PWM_SHUTDOWN
	       //RESTVAL:0x0
	       regdata = rd(ASIU_PWM_SHUTDOWN);
	       cnt++;
	       if(regdata != 0x0) {
	           error++;
	           post_log("Error for ASIU_PWM_SHUTDOWN with : Actual Read Value: 0x%08X     Expected Reset Value: 0x0\n",regdata);
	       }
	       if(error)
		   {
	          overall_status = TEST_FAIL;
	          post_log("ASIU PWM SOFT RESET TEST: TEST FAIL\n");
		   }
		   else
		   {
			  overall_status = TEST_PASS;
	          post_log("ASIU PWM SOFT RESET TEST : TEST PASS\n");
		   }

	        post_log("Error Cnt for ASIU PWM SOFT RESET TEST : %d\n",error);
			post_log("Waive Cnt for ASIU PWM SOFT RESET TEST : %d\n",waive);
	        return overall_status;
}

int pwm0_test_diag(void)
{
	  uint32_t status = 0;
	  uint32_t pwmctl_data = 0x3FFF0001;
	  pwminit();

	  setPwmCtl(0x3FFF0000);

	  adddelay(10000);

	  setPrescale(0x0);

	  adddelay(10000);

	  setPeriod(0x0,0xC8);

	  adddelay(10000);

	  setHighPeriod(0x0,0x64);

	  adddelay(10000);

	  setPwmCtl(pwmctl_data);


	return status;
}

int pwm1_test_diag(void)
{
	  uint32_t status = 0;
	  uint32_t pwmctl_data = 0x3FFF0002;
	  pwminit();

	  setPwmCtl(0x3FFF0000);

	  adddelay(10000);

	  setPrescale(0x0);

	  adddelay(10000);

	  setPeriod(0x1,100);

	  adddelay(10000);

	  setHighPeriod(0x1,50);

	  adddelay(10000);

	  setPwmCtl(pwmctl_data);


	return status;
}

int pwm2_test_diag(void)
{
	  uint32_t status = 0;
	  uint32_t pwmctl_data = 0x3FFF0004;
	  pwminit();

	  setPwmCtl(0x3FFF0000);

	  adddelay(10000);

	  setPrescale(0x0);

	  adddelay(10000);

	  setPeriod(0x2,100);

	  adddelay(10000);

	  setHighPeriod(0x2,50);

	  adddelay(10000);

	  setPwmCtl(pwmctl_data);


	return status;
}

int pwm3_speaker_output(void)
{
	  uint32_t status = 0;
	  uint32_t pwmctl_data = 0x3FFF0008;
	  
	  iomux_select_pwm3_if_en_interface();
	  
	  pwminit();

	  setPwmCtl(0x3FFF0000);

	  adddelay(10000);

	  setPrescale(0x0);

	  adddelay(10000);

	  setPeriod(0x3,0xC8);

	  adddelay(10000);

	  setHighPeriod(0x3,0x64);

	  adddelay(10000);

	  setPwmCtl(pwmctl_data);


	return status;
}

int pwm4_test_diag(void)
{
	  uint32_t status = 0;
	  uint32_t pwmctl_data = 0x3FFF0010;
	  pwminit();

	  setPwmCtl(0x3FFF0000);

	  adddelay(10000);

	  setPrescale(0x0);

	  adddelay(10000);

	  setPeriod(0x4,100);

	  adddelay(10000);

	  setHighPeriod(0x4,50);

	  adddelay(10000);

	  setPwmCtl(pwmctl_data);


	return status;
}

int pwm5_test_diag(void)
{
	  uint32_t status = 0;

	  uint32_t pwmctl_data = 0x3FFF0020;

	  pwminit();

	  setPwmCtl(0x3FFF0000);

	  adddelay(10000);

	  setPrescale(0x0);

	  adddelay(10000);

	  setPeriod(0x5,100);

	  adddelay(10000);

	  setHighPeriod(0x5,50);

	  adddelay(10000);

	  setPwmCtl(pwmctl_data);

      return status;
}

/*int spru_tamper_test_input_n0(void)
{
	uint32_t tampermode,tamperpn,tampernumber;
	uint32_t error;
	int status = 0;
	tampermode = 0;
	tamperpn = 0;
	tampernumber = 0;
	spru_init();
	status = sprutamper(tampermode,tamperpn,tampernumber);
	post_log("Look for the LED glowing \n");
	error = spru_tamper_status();
	if (error)
		status = - 1;

	  return status;
}

int spru_tamper_test_input_n1(void)
{
	uint32_t tampermode,tamperpn,tampernumber;
	uint32_t error;
	int status = 0;
	tampermode = 0;
	tamperpn = 0;
	tampernumber = 1;
	status = spru_init();
	if (status == -1)
		return -1;
	status = sprutamper(tampermode,tamperpn,tampernumber);
	post_log("Look for the LED glowing \n");
	error = spru_tamper_status();
		if (error)
			status = - 1;
	return status;
}

int spru_tamper_test_input_n2(void)
{
	uint32_t tampermode,tamperpn,tampernumber;
	uint32_t error;
		int status = 0;
	tampermode = 0;
	tamperpn = 0;
	tampernumber = 2;
	status = spru_init();
		if (status == -1)
			return -1;
	status = sprutamper(tampermode,tamperpn,tampernumber);
	post_log("Look for the LED glowing \n");
	error = spru_tamper_status();
			if (error)
				status = - 1;
	return status;
}

int spru_tamper_test_input_n3(void)
{
	uint32_t tampermode,tamperpn,tampernumber;
	uint32_t error;
		int status = 0;
	tampermode = 0;
	tamperpn = 0;
	tampernumber = 3;
	status = spru_init();
		if (status == -1)
			return -1;
	status = sprutamper(tampermode,tamperpn,tampernumber);
	post_log("Look for the LED glowing \n");
	error = spru_tamper_status();
			if (error)
				status = - 1;
	return status;
}

int spru_tamper_test_input_n4(void)
{
	uint32_t tampermode,tamperpn,tampernumber;
	uint32_t error;
		int status = 0;
	tampermode = 0;
	tamperpn = 0;
	tampernumber = 4;
	status = spru_init();
		if (status == -1)
			return -1;
	status = sprutamper(tampermode,tamperpn,tampernumber);
	post_log("Look for the LED glowing \n");
	error = spru_tamper_status();
			if (error)
				status = - 1;
	return status;
}

int spru_tamper_test_input_n5(void)
{
	uint32_t tampermode,tamperpn,tampernumber;
	uint32_t error;
		int status = 0;
	tampermode = 0;
	tamperpn = 0;
	tampernumber = 5;
	status = spru_init();
		if (status == -1)
			return -1;
	status = sprutamper(tampermode,tamperpn,tampernumber);
	post_log("Look for the LED glowing \n");
	error = spru_tamper_status();
			if (error)
				status = - 1;
	return status;
}

int spru_tamper_test_input_n6(void)
{
	uint32_t tampermode,tamperpn,tampernumber;
	uint32_t error;
		int status = 0;
	tampermode = 0;
	tamperpn = 0;
	tampernumber = 6;
	status = spru_init();
		if (status == -1)
			return -1;
	status = sprutamper(tampermode,tamperpn,tampernumber);
	post_log("Look for the LED glowing \n");
	error = spru_tamper_status();
			if (error)
				status = - 1;
	return status;
}

int spru_tamper_test_input_n7(void)
{
	uint32_t tampermode,tamperpn,tampernumber;
	uint32_t error;
		int status = 0;
	tampermode = 0;
	tamperpn = 0;
	tampernumber = 7;
	status = spru_init();
		if (status == -1)
			return -1;
	status = sprutamper(tampermode,tamperpn,tampernumber);
	post_log("Look for the LED glowing \n");
	error = spru_tamper_status();
			if (error)
				status = - 1;
	return status;
}

int spru_tamper_test_input_n8(void)
{
	uint32_t tampermode,tamperpn,tampernumber;
	uint32_t error;
		int status = 0;
	tampermode = 0;
	tamperpn = 0;
	tampernumber = 8;
	status = spru_init();
		if (status == -1)
			return -1;
	status = sprutamper(tampermode,tamperpn,tampernumber);
	post_log("Look for the LED glowing \n");
	error = spru_tamper_status();
			if (error)
				status = - 1;
	return status;
}

int spru_tamper_test_input_p0(void)
{
	uint32_t tampermode,tamperpn,tampernumber;
	uint32_t error;
		int status = 0;
	tampermode = 0;
	tamperpn = 1;
	tampernumber = 0;
	status = spru_init();
		if (status == -1)
			return -1;
	status = sprutamper(tampermode,tamperpn,tampernumber);
	post_log("Look for the LED glowing \n");
	error = spru_tamper_status();
			if (error)
				status = - 1;
	return status;
}

int spru_tamper_test_input_p1(void)
{
	uint32_t tampermode,tamperpn,tampernumber;
	uint32_t error;
		int status = 0;
	tampermode = 0;
	tamperpn = 1;
	tampernumber = 1;
	status = spru_init();
		if (status == -1)
			return -1;
	status = sprutamper(tampermode,tamperpn,tampernumber);
	post_log("Look for the LED glowing \n");
	error = spru_tamper_status();
			if (error)
				status = - 1;
	return status;
}

int spru_tamper_test_input_p2(void)
{
	uint32_t tampermode,tamperpn,tampernumber;
	uint32_t error;
		int status = 0;
	tampermode = 0;
	tamperpn = 1;
	tampernumber = 2;
	status = spru_init();
		if (status == -1)
			return -1;
	status = sprutamper(tampermode,tamperpn,tampernumber);
	post_log("Look for the LED glowing \n");
	error = spru_tamper_status();
			if (error)
				status = - 1;
	return status;
}

int spru_tamper_test_input_p3(void)
{
	uint32_t tampermode,tamperpn,tampernumber;
	uint32_t error;
		int status = 0;
	tampermode = 0;
	tamperpn = 1;
	tampernumber = 3;
	status = spru_init();
		if (status == -1)
			return -1;
	status = sprutamper(tampermode,tamperpn,tampernumber);
	post_log("Look for the LED glowing \n");
	error = spru_tamper_status();
			if (error)
				status = - 1;
	return status;
}

int spru_tamper_test_input_p4(void)
{
	uint32_t tampermode,tamperpn,tampernumber;
	uint32_t error;
		int status = 0;
	tampermode = 0;
	tamperpn = 1;
	tampernumber = 4;
	status = spru_init();
		if (status == -1)
			return -1;
	status = sprutamper(tampermode,tamperpn,tampernumber);
	post_log("Look for the LED glowing \n");
	error = spru_tamper_status();
			if (error)
				status = - 1;
	return status;
}

int spru_tamper_test_input_p5(void)
{
	uint32_t tampermode,tamperpn,tampernumber;
	uint32_t error;
		int status = 0;
	tampermode = 0;
	tamperpn = 1;
	tampernumber = 5;
	status = spru_init();
		if (status == -1)
			return -1;
	status = sprutamper(tampermode,tamperpn,tampernumber);
	post_log("Look for the LED glowing \n");
	error = spru_tamper_status();
			if (error)
				status = - 1;
	return status;
}

int spru_tamper_test_input_p6(void)
{
	uint32_t tampermode,tamperpn,tampernumber;
	uint32_t error;
		int status = 0;
	tampermode = 0;
	tamperpn = 1;
	tampernumber = 6;
	status = spru_init();
		if (status == -1)
			return -1;
	status = sprutamper(tampermode,tamperpn,tampernumber);
	post_log("Look for the LED glowing \n");
	error = spru_tamper_status();
			if (error)
				status = - 1;
	return status;
}

int spru_tamper_test_input_p7(void)
{
	uint32_t tampermode,tamperpn,tampernumber;
	uint32_t error;
		int status = 0;
	tampermode = 0;
	tamperpn = 1;
	tampernumber = 7;
	status = spru_init();
		if (status == -1)
			return -1;
	status = sprutamper(tampermode,tamperpn,tampernumber);
	post_log("Look for the LED glowing \n");
	error = spru_tamper_status();
			if (error)
				status = - 1;
	return status;
}

int spru_tamper_test_input_p8(void)
{
	uint32_t tampermode,tamperpn,tampernumber;
	uint32_t error;
		int status = 0;
	tampermode = 0;
	tamperpn = 1;
	tampernumber = 8;
	status = spru_init();
		if (status == -1)
			return -1;
	status = sprutamper(tampermode,tamperpn,tampernumber);
	post_log("Look for the LED glowing \n");
	error = spru_tamper_status();
			if (error)
				status = - 1;
	return status;
}

int spru_tamper_test_active_n1(void)
{
	uint32_t tampermode,tamperpn,tampernumber;
	uint32_t error;
		int status = 0;
	tampermode = 1;
	tamperpn = 0;
	tampernumber = 1;
	status = spru_init();
		if (status == -1)
			return -1;
	status = sprutamper(tampermode,tamperpn,tampernumber);
	post_log("Look for the LED glowing \n");
	error = spru_tamper_status();
			if (error)
				status = - 1;
	return status;
}

int spru_tamper_test_active_n2(void)
{
	uint32_t tampermode,tamperpn,tampernumber;
	uint32_t error;
		int status = 0;
	tampermode = 1;
	tamperpn = 0;
	tampernumber = 2;
	status = spru_init();
		if (status == -1)
			return -1;
	status = sprutamper(tampermode,tamperpn,tampernumber);
	post_log("Look for the LED glowing \n");
	error = spru_tamper_status();
			if (error)
				status = - 1;
	return status;
}

int spru_tamper_test_active_n3(void)
{
	uint32_t tampermode,tamperpn,tampernumber;
	uint32_t error;
		int status = 0;
	tampermode = 1;
	tamperpn = 0;
	tampernumber = 3;
	status = spru_init();
		if (status == -1)
			return -1;
	status = sprutamper(tampermode,tamperpn,tampernumber);
	post_log("Look for the LED glowing \n");
	error = spru_tamper_status();
			if (error)
				status = - 1;
	return status;
}

int spru_tamper_test_active_n4(void)
{
	uint32_t tampermode,tamperpn,tampernumber;
	uint32_t error;
		int status = 0;
	tampermode = 1;
	tamperpn = 0;
	tampernumber = 4;
	status = spru_init();
		if (status == -1)
			return -1;
	status = sprutamper(tampermode,tamperpn,tampernumber);
	post_log("Look for the LED glowing \n");
	error = spru_tamper_status();
			if (error)
				status = - 1;
	return status;
}

int spru_tamper_test_active_n5(void)
{
	uint32_t tampermode,tamperpn,tampernumber;
	uint32_t error;
		int status = 0;
	tampermode = 1;
	tamperpn = 0;
	tampernumber = 5;
	status = spru_init();
		if (status == -1)
			return -1;
	status = sprutamper(tampermode,tamperpn,tampernumber);
	post_log("Look for the LED glowing \n");
	error = spru_tamper_status();
			if (error)
				status = - 1;
	return status;
}

int spru_tamper_test_active_n6(void)
{
	uint32_t tampermode,tamperpn,tampernumber;
	uint32_t error;
		int status = 0;
	tampermode = 1;
	tamperpn = 0;
	tampernumber = 6;
	status = spru_init();
		if (status == -1)
			return -1;
	status = sprutamper(tampermode,tamperpn,tampernumber);
	post_log("Look for the LED glowing \n");
	error = spru_tamper_status();
			if (error)
				status = - 1;
	return status;
}

int spru_tamper_test_active_n7(void)
{
	uint32_t tampermode,tamperpn,tampernumber;
	uint32_t error;
	int status = 0;
	tampermode = 1;
	tamperpn = 0;
	tampernumber = 7;
	status = spru_init();
		if (status == -1)
			return -1;
	status = sprutamper(tampermode,tamperpn,tampernumber);
	post_log("Look for the LED glowing \n");
	error = spru_tamper_status();
			if (error)
				status = - 1;
	return status;
}

int spru_tamper_test_active_n8(void)
{
	uint32_t tampermode,tamperpn,tampernumber;
	uint32_t error;
		int status = 0;
	tampermode = 1;
	tamperpn = 0;
	tampernumber = 8;
	status = spru_init();
		if (status == -1)
			return -1;
	status = sprutamper(tampermode,tamperpn,tampernumber);
	post_log("Look for the LED glowing \n");
	error = spru_tamper_status();
			if (error)
				status = - 1;
	return status;
}*/

/******* Internal test function start ********/
int PWM_post_test(int flags)
{
#if 1
	int status = TEST_PASS;
    if(TEST_FAIL == pwm3_speaker_output())
	      status = TEST_FAIL;
    return status;

#else


	uint32_t test_no;
    int loop_count = 0, total_tests = 0 , total_reg_tests = 0;
    int status = 0,autorun_status = 0,pass = 0,fail = 0,skip = 0;


    //lcd_clk_gate();
    //lcd_asiu_mipi_dsi_gen_pll_pwr_on_config();
    //lcd_ctrl_init(0x61000000,&config);

    FUNC_INFO function[]	=	{
    		                     //ADD_TEST( pwm_soft_reset_test ),

	     	 	 	 	 	 	 ADD_TEST( pwm3_speaker_output       )


								};

    FUNC_INFO regression_func[]	=	{
    		                                 ADD_TEST( pwm_soft_reset_test ),



									};

    total_tests = (sizeof(function) / sizeof(function[0]));
    total_reg_tests = (sizeof(regression_func) / sizeof(regression_func[0]));

    if(flags & POST_REGRESSION )
    {
    	for(loop_count = 0 ; loop_count  < total_reg_tests ; loop_count++ )
    	{
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
    else
    {
    	post_log("Number of test : %d",total_tests);
    	do{
    		for(loop_count = 0 ; loop_count  < total_tests ; loop_count++ )
    			post_log("\n%2d %s",(loop_count + 1), function[loop_count].func_name);
    		post_log("\n%2d run all tests",(total_tests + 1));
    		post_log("\n%2d exit\n",(total_tests + 2));
    		test_no = post_getUserInput ("\nEnter Test Number : ");

    		if(test_no > (total_tests + 2) )
    			post_log("ERROR - invalid entry\n");
    		else
    		{
                  if(test_no == (total_tests + 1))
                  {
                      autorun_status = 0;
                      pass = 0;
                      fail = 0;
                      skip = 0;
                      for(loop_count = 0 ; loop_count  < total_tests ; loop_count++ )
                      {
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

                  else if(test_no < (total_tests + 1))
                  {
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


#else
#define IPROC_PWM_CHANNEL_0     0
#define IPROC_PWM_CHANNEL_1     1
#define IPROC_PWM_CHANNEL_2     2
#define IPROC_PWM_CHANNEL_3     3

#define CH0     1
#define CH1     2
#define CH2     4
#define CH3     8
#define CH_ALL  0xF

#if (defined(CONFIG_NS_PLUS))
#define REFCLK 25000000
#else
#define REFCLK 1000000
#endif

static pwm_config testPWMConfig;
static iproc_pwmc  *testPWCDev;

static void PWM_post_init( void )
{
	//This is an undocumented register in the register programmer's guide
	//To turn on AUX function
#if (defined(CONFIG_NS_PLUS))
	*(volatile unsigned long*)0x18030028 |= 0xF;
#else
	*(volatile unsigned long*)0x18001028 |= 0xF;
#endif
	iproc_pwmc_init();
}
static void PWM_post_deinit( void )
{
	//This is an undocumented register in the register programmer's guide
	//To turn off AUX function
	*(volatile unsigned long*)0x18001028 &= ~0xF;
	iproc_pwmc_deinit();
}

static int PWM_post_configPWM(unsigned long dutyPercent, float freqInHz,
		int polarity, int ch  )
{
	int i;
	//Setup config packet to config duty cycle
	//freq and plority
	testPWMConfig.config_mask =	PWM_CONFIG_DUTY_TICKS |
								PWM_CONFIG_PERIOD_TICKS |
								PWM_CONFIG_POLARITY |
								PWM_CONFIG_START |
								PWM_CONFIG_STOP;



    if ( freqInHz > 500000 || freqInHz < 0.238 )
    {
    	post_log("\nRequested frequency is out of range !\n");
    	return -1;
    }
    if ( dutyPercent > 100)
    {
    	post_log("\nRequested frequency is out of range !\n");
    	return -1;
    }
	testPWMConfig.period_ticks = REFCLK / freqInHz;
	testPWMConfig.duty_ticks = testPWMConfig.period_ticks * ((float)dutyPercent/100);


	testPWMConfig.polarity = polarity;

	for ( i = 0; i <4; i++ )
	{
		if ( (1 << i) & ch )
		{
			iproc_pwmc_config(testPWCDev,  i ,&testPWMConfig);
		}
	}

	return 0;
}
static void PWM_post_startPWM(int ch)
{
	int i;
	for ( i = 0; i < 4; i++ )
	{
		if ( (1 << i) & ch )
		{
			iproc_pwmc_start(testPWCDev, i);
		}
	}
}
static void PWM_post_stopPWM(int ch)
{
	int i;
	for ( i = 0; i < 4; i++ )
	{
		if ( (1 << i) & ch )
		{
			iproc_pwmc_stop(testPWCDev, i);
		}
	}
}
int PWM_post_test (int flags)
{
	int ret=0,i;
#if (defined(CONFIG_NS_PLUS))

    if ( post_get_board_diags_type() & HR_ER_BOARDS )
	{
		printf("\nThis diag is not supported on this platform\n ");
		return 2;
	}
#endif


	post_log("\nPWM diags starts  !!!");
	post_log("\nPlease setup scope to measure duty cycles and frequency...");
	post_log("\nMake sure scope probe is connected to pins at J1801");
	post_log("\nMake sure SW1801 PWM all switches are set to OFF position");

	post_getUserResponse("\nReady? (Y/N)");

    if ( (flags & POST_SEMI_AUTO) !=  POST_SEMI_AUTO )
    {

        post_log("\nMake sure strap pin A4 is set to 0xF4");
        post_getConfirmation("\nReady? (Y/N)");

    }
    else
    {
        post_set_strappins("A2 0%A3 3%A4 f4%A5 38%A6 f4%");
    }





	//init
	PWM_post_init();
	//Get PWM handle
	testPWCDev = iproc_pwmc_getDeviceHandle();

	for ( i = 0; i < 4; i++ )
	{
		post_log("\nMake sure SW1801 PWM %d is set to ON position,all other pins are off", i);
		post_getUserResponse("\nReady? (Y/N)");
		post_log("\nGenerate signals to ch %d with duty cycle:50%, Freq 3000 Hz",i);
		post_log("\nYou should be hearing an audible tone also");
		//Config  duty cycle %, freq in Hz and Polarity
		PWM_post_configPWM( 50, 3000, 0,1<<i );
		//Turn on the channel
		PWM_post_startPWM(1<<i);
		ret |=post_getUserResponse("\nSignal generated correctly? (Y/N)");
		//Turn off the channel
		PWM_post_stopPWM(1<<i);
	}

	for ( i = 0; i < 4; i++ )
	{
		post_log("\nMake sure SW1801 PWM %d is set to ON position,all other pins are off", i);
		post_getUserResponse("\nReady? (Y/N)");
		post_log("\nGenerate signals to ch %d with duty cycle: 30%, Freq 30KHz",i);
		//Config  duty cycle %, freq in Hz and Polarity
		PWM_post_configPWM( 70, 30000, 0,1<<i );
		//Turn on the channel
		PWM_post_startPWM(1<<i );
		ret |=post_getUserResponse("\nSignal generated correctly? (Y/N)");

		//Turn off the channel
		PWM_post_stopPWM(1<<i );
	}

	for ( i = 0; i < 4; i++ )
	{
		post_log("\nMake sure SW1801 PWM %d is set to ON position,all other pins are off", i);
		post_getUserResponse("\nReady? (Y/N)");
		post_log("\nGenerate signals to ch %d with duty cycle: 80%, Freq 50KHz",i);
		//Config  duty cycle %, freq in Hz and Polarity
		PWM_post_configPWM( 80, 50000,1,1<<i );
		//Turn on the channel
		PWM_post_startPWM(1<<i );
		ret |= post_getUserResponse("\nSignal generated correctly? (Y/N)");

		//Turn off the channel
		PWM_post_stopPWM(1<<i );
	}

	if ( ret == 0 )
	{
		post_log("\nTests are successful ");
	}
	else
	{
		post_log("\nTest failed");
                ret = -1;
	}

    post_log("\nSuccessful PWM diags done  !!!\n");

    PWM_post_deinit();
    return ret;
}
#endif 
#endif /* CONFIG_POST & CONFIG_SYS_POST_PWM */
