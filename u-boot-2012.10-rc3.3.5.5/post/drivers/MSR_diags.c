/*
 * $Copyright Open Broadcom Corporation$
 */

#include <common.h>
#include <post.h>

#if defined(CONFIG_CYGNUS)
#if CONFIG_POST & CONFIG_SYS_POST_MSR
#include "../../halapis/include/ssp_apis.h"

#define SSPCR0_SCR 				99
#define SSPCR0_CPSDVSR 			100
#define TRACK1_START_SEN_CHAR 	0x25
#define TRACK2_START_SEN_CHAR 	0x3B
#define TRACK3_START_SEN_CHAR 	0x2B
#define TRACK_END_SEN_CHAR 		0x3F
#define ADD_TEST(X)  { #X , X }
#define TEST_PASS  0
#define TEST_FAIL -1
#define TEST_SKIP  2
#define RNE_TIMEOUT   0x5A
#define RNE_DETECTED  0xFF
#define SSP_BUSY	  0xDE
#define TFE_TIMEOUT   0x5A
#define TFE_DETECTED  0xFF
#define DATA_CAPTURE_FAIL 	0x00
#define DATA_CAPTURE_PASS   0x01

typedef struct{

	char* func_name;
	int (*func)(void);
}FUNC_INFO;

void ssp_diag_delay(void)
{
	uint32_t loop = 0xFFF;
	while(loop--);
}
extern bool ssp_get_bsy(SSP_ID id);
extern bool ssp_get_rne(SSP_ID id);
uint32_t wait_for_rne(void)
{
	uint32_t wait = 0xFFF;

	while(ssp_get_bsy(SSP_ID1))
	{
		wait--;
		if(!wait)
			break;
	}
	if(!wait){
		post_log("SSP_BUSY \n");
		return SSP_BUSY;
	}

	else
		wait = 0xFF;

	while(!ssp_get_rne(SSP_ID1))
	{
		wait--;
		ssp_diag_delay();
		if(!wait)
			break;
	}
	if(wait)
		return RNE_DETECTED;
	post_log("RNE_TIMEOUT\n");
	return RNE_TIMEOUT;
}
extern bool ssp_get_tfe(SSP_ID id);
uint32_t wait_for_tfe(void)
{
	uint32_t wait = 0xFFF;
	while(ssp_get_bsy(SSP_ID1))
	{
		wait--;
		if(!wait)
			break;
	}
	if(!wait)
		return SSP_BUSY;
	else
		wait = 0xFFF;
	while(!ssp_get_tfe(SSP_ID1))
	{
		wait--;
		ssp_diag_delay();
		if(!wait)
			break;
	}
	if(wait)
		return TFE_DETECTED;
	return TFE_TIMEOUT;
}
extern uint8_t ssp_get_byte(SSP_ID id);
extern void ssp_put_byte(SSP_ID id, uint8_t d);

uint32_t get_track(char *p)
{
	uint32_t fdata =  0x0;
	uint32_t i = 0;
	while (fdata != TRACK_END_SEN_CHAR)
	{
		ssp_put_byte(SSP_ID1,0x5A);
		if(RNE_DETECTED != wait_for_rne())
		{
			post_log("error : RNE not detected exiting...\n");
			return RNE_TIMEOUT;
		}
		fdata = ssp_get_byte(SSP_ID1);
		p[i++] = fdata;
	}
	p[i-1] = '\0';
	return RNE_DETECTED;
}

int get_full_data(char *p)
{
	uint8_t fdata = 0;
	uint32_t i = 0;
	do
	{
		ssp_put_byte(SSP_ID1,0x5A);
		if(RNE_DETECTED != wait_for_rne())
		{
			post_log("error : RNE not detected exiting...\n");
			return DATA_CAPTURE_FAIL;
		}
		fdata = ssp_get_byte(SSP_ID1);
		p[i++] = fdata;
		if(i > 250)
			return DATA_CAPTURE_FAIL;
	}while(fdata != 0x00);
	p[i-1] = '\0';
	return DATA_CAPTURE_PASS;
}

int detect_data_start(void)
{
	uint8_t volatile fdata = 0;
	/* Put some dummy data*/
	ssp_put_byte(SSP_ID1,0x5A);
	ssp_put_byte(SSP_ID1,0x5A);
	if(RNE_DETECTED != wait_for_rne())
	{
		post_log("error : RNE not detected exiting...\n");
		return DATA_CAPTURE_FAIL;
	}
	fdata = ssp_get_byte(SSP_ID1);
	if(fdata == 0x0D)
	{
		post_log("<CR> --> Carriage Return received\n");
		fdata = ssp_get_byte(SSP_ID1);
		if(fdata == 0x0A)
		{
			post_log("<LF> --> Line Feed received\n");
			return DATA_CAPTURE_PASS;
		}
		post_log("Error : received = 0x%X in place of LF\n",fdata);
	}
	else
		post_log("Error : received = 0x%X in place of CR\n",fdata);
	return DATA_CAPTURE_FAIL;
}

void parse_track_info(char *p)
{
	uint32_t track1 = 0,track2 = 0,track3 = 0;
	uint32_t i = 0;
	uint32_t j = 0;
	char t1_data[100];
	char t2_data[100];
	char t3_data[100];

	if(p[i] == TRACK1_START_SEN_CHAR)
	{
		i++;
		track1 = 1;
		while((p[i] != TRACK_END_SEN_CHAR) && (p[i] != '\0'))
		{
			t1_data[j] = p[i];
			i++;
			j++;
		}
		t1_data[j] = '\0';
		j = 0;
		i++;
	}

	if(p[i] == TRACK2_START_SEN_CHAR)
	{
		i++;
		track2 = 1;
		while((p[i] != TRACK_END_SEN_CHAR) && (p[i] != '\0'))
		{
			t2_data[j] = p[i];
			i++;
			j++;
		}
		t2_data[j] = '\0';
		j = 0;
		i++;
	}

	if(p[i] == TRACK3_START_SEN_CHAR)
	{
		i++;
		track3 = 1;
		while((p[i] != TRACK_END_SEN_CHAR) && (p[i] != '\0'))
		{
			t3_data[j] = p[i];
			i++;
			j++;
		}
		t3_data[j] = '\0';
		j = 0;
		i++;
	}
	j = 0;
	i = 0;
	if(track1 == 1)
	{
		post_log("\n\nTRACK - 1 Data ==> %s \n",t1_data);
		post_log("FC  : Fromat code            => %c\n",t1_data[i]);
		i++;
		post_log("PAN : Primary Account Number => ");
		while( (t1_data[i] != '^') && (t1_data[i] != '\0'))
			post_log("%c",t1_data[i++]);

		if(t1_data[i] != '\0')
			i++;

		if(t1_data[i] != '^'){
			post_log("\nNM  : Name                   => ");
			while( (t1_data[i] != '^') && ((t1_data[i] != '\0')))
				post_log("%c",t1_data[i++]);
		}
		if(t1_data[i] != '\0')
			i++;

		if(t1_data[i] != '^'){
			post_log("\nED  : Expiration date        => ");
			j = 4;
			while(j--)
				post_log("%c",t1_data[i++]);
		}

		if(t1_data[i] != '^'){
			post_log("\nSC  : Service code           => ");
			j = 3;
			while(j--)
				post_log("%c",t1_data[i++]);
		}
		post_log("\nDD  : Discretionary data     => ");
		while ((t1_data[i] != '\0') && (i < 80) )
			post_log("%c",t1_data[i++]);
	}
	if(track2 == 1)
	{
		i = 0;
		post_log("\n\nTRACK - 2 Data ==> %s \n",t2_data);
		post_log("PAN : Primary Account Number => ");
		while( (t2_data[i] != '=') && (t2_data[i] != '\0'))
			post_log("%c",t2_data[i++]);

		if(t2_data[i] != '\0')
			i++;

		if((t2_data[i] != '=') && (t2_data[i] != '\0')){
			post_log("\nED  : Expiration date        => ");
			j = 4;
			while(j--)
				post_log("%c",t2_data[i++]);
		}

		if((t2_data[i] != '=') && (t2_data[i] != '\0')){
			post_log("\nSC  : Service code           => ");
			j = 3;
			while(j--)
				post_log("%c",t2_data[i++]);
		}
		post_log("\nDD  : Discretionary data     => ");
		while((t2_data[i] != '\0') && (i < 40))
			post_log("%c",t2_data[i++]);
	}
	if(track3 == 1)
	{
		post_log("\n\nTRACK - 3 Data ==> %s \n",t3_data);
	}

}
extern void ssp_config(_ssp_cfg *cfg);
int msr_card_swipe_test(void)
{ 
    uint32_t read_val;
    int pass_flag = TEST_PASS;
    _ssp_cfg cfg;
    uint32_t counter = 0xFFFFFFF;
    char card_data[400] = {0};
    //Motorola SPI configuration
    //bit rate = Fsspclk / (SSPCPSR * (1 + clock_rate)), Fsspclk = 100MHz for Cygnus we have to configure SSP_CLK_OUT for 100KHz hence SSPCPSR = 20 and scr = 49
    cfg.clk_prescale 	= SSPCR0_CPSDVSR;
    cfg.d_size			= DATA_SIZE_8BIT;
    cfg.format			= MOTOROLA_SPI;
    cfg.lbm				= NORMAL_MODE;
    cfg.ms				= MASTER_MODE;
    cfg.phase			= PHA_0;
    cfg.pol				= POL_0;
    cfg.scr				= SSPCR0_SCR;
    cfg.sod				= 0x00;
    cfg.ssp_id			= SSP_ID1;


    cpu_wr_single(0x301d0bc,0,4);
    uint32_t reg_val = cpu_rd_single(CRMU_IOMUX_CTRL1,4);
    reg_val &= ~(0xF << CRMU_IOMUX_CTRL1__CORE_TO_IOMUX_GPIO15_SEL_R);
    cpu_wr_single(CRMU_IOMUX_CTRL1,reg_val,4);

    reg_val = cpu_rd_single(CRMU_IOMUX_CTRL4,4);
    reg_val &= ~(0xF << CRMU_IOMUX_CTRL4__CORE_TO_IOMUX_SPI1_SEL_R);
    cpu_wr_single(CRMU_IOMUX_CTRL4,reg_val,4);

    post_log("\n Configure GPIO_15 to be an input pin. DataValid from MSR is connected to GPIO15 \n");
    /*read_val = cpu_rd_single(ChipcommonG_GP_OUT_EN,4) & (~(1 << 15));
    cpu_wr_single(ChipcommonG_GP_OUT_EN, read_val,4);*/

    post_log("\n\n INFO: MSR supports 100K max baudrate on its spi interface \n");
    post_log("\n\n Configuring SPI-1 for Motorola mode, 8-bit data, Master, SPO=0, SPH=0, and for 100Kbps baud \n");
    ssp_config(&cfg);

    post_log("\n Spi-1 is Configured. User can go ahead and swipe the card \n");

    /* poll for card data */
    read_val = 0x01;
    while (read_val)
    {
    	read_val = (cpu_rd_single(ChipcommonG_GP_DATA_IN,4) >> 15) & 0x01;
    	ssp_diag_delay();
    	//post_log("DATA_IN = 0x%X\n",cpu_rd_single(ChipcommonG_GP_DATA_IN,4));
    	if(!(counter))
    	{
    		post_log("error : timeout! card not swiped or not detected \n");
    		pass_flag = TEST_FAIL;
    		break;
    	}
    	counter--;
    }
    if(counter)
    {
		/* DAV Hi to SCL transition should happen within 400ms/ */
			if( detect_data_start() )
			{
				if(get_full_data(&card_data[0]))
				{
					post_log("CARD Data => %s \n",card_data);
					parse_track_info(&card_data[0]);

				}
				else
				{
					post_log("Error : Data reception failed, please try again \n");
					pass_flag = TEST_FAIL;
				}

			}
			else{
				post_log("Error : Data Start condition not detected \n");
				pass_flag = TEST_FAIL;
			}
	}
    post_log("\n\nTest - %s\n",pass_flag == TEST_FAIL ? "FAILED" : "PASSED");
    return pass_flag;
}

int msr_power_up_message_test(void)
{
    uint32_t read_val;
    char msg[200] = {0};

    uint8_t volatile fdata = 0;
    _ssp_cfg cfg;
    uint32_t END = 0;
    uint32_t i = 0;
//#ifdef CONFIG_CYGNUS_EMULATION
  //  uint32_t counter = 0xFF;
//#else
    uint32_t counter = 0xFFFFFFFF;
//#endif
    uint32_t spi_fail = 0;

    //Motorola SPI configuration
    //bit rate = Fsspclk / (SSPCPSR * (1 + clock_rate)), Fsspclk = 100MHz for Cygnus we have to configure SSP_CLK_OUT for 100KHz hence SSPCPSR = 20 and scr = 49
    cfg.clk_prescale 	= SSPCR0_CPSDVSR;
    cfg.d_size			= DATA_SIZE_8BIT;
    cfg.format			= MOTOROLA_SPI;
    cfg.lbm				= NORMAL_MODE;
    cfg.ms				= MASTER_MODE;
    cfg.phase			= PHA_0;
    cfg.pol				= POL_0;
    cfg.scr				= SSPCR0_SCR;
    cfg.sod				= 0x00;
    cfg.ssp_id			= SSP_ID1;

    uint32_t reg_val = cpu_rd_single(CRMU_IOMUX_CTRL1,4);
    reg_val &= ~(0xF << CRMU_IOMUX_CTRL1__CORE_TO_IOMUX_GPIO15_SEL_R);
    cpu_wr_single(CRMU_IOMUX_CTRL1,reg_val,4);

    post_log("\n Configure GPIO_15 to be an input pin. DataValid from MSR is connected to GPIO15 \n");
    read_val = (cpu_rd_single(ChipcommonG_GP_OUT_EN,4) & (~(1 << 15)));
    cpu_wr_single(ChipcommonG_GP_OUT_EN, read_val,4);

    post_log("\n\n INFO: MSR supports 100K max baudrate on its spi interface \n");
    post_log("\n\n Configuring SPI-1 for Motorola mode, 8-bit data, Master, SPO=0, SPH=0, and for 100Kbps baud \n");
    ssp_config(&cfg);

    post_log("\n Spi-1 is Configured. User can go ahead and reset the card reader to get the power_up message \n");

    /* poll for card data */
    read_val = 0x00;
    while (!read_val)
    {
    	read_val = ((cpu_rd_single(ChipcommonG_GP_DATA_IN,4) >> 15) & 0x01);
    	//post_log("DATA_IN = 0x%X\n",cpu_rd_single(ChipcommonG_GP_DATA_IN,4));
    	ssp_diag_delay();
    	if(!(counter))
    	{
    		post_log("error : timeout! card reader reset timeout  \n");
    		break;
    	}
    	counter--;
    }

    if(counter)
    {
    	counter = 40;
		//ssp_put_byte(SSP_ID1,0x5A);
		/* DAV Hi to SCL transition should happen within 400ms*/
		while(!END)
		{
	    	ssp_put_byte(SSP_ID1,0x5A);
	    	if(RNE_DETECTED != wait_for_rne())
	    	{
	    		spi_fail = 1;
	    		post_log("error : RNE not detected exiting...\n");
	    		END = 1;
	    		break;
	    	}

			fdata = ssp_get_byte(SSP_ID1);

			if(fdata == 0x0A)
			{
				post_log("RXed <LF>\n");
				ssp_put_byte(SSP_ID1,0x5A);
		    	if(RNE_DETECTED != wait_for_rne())
		    	{
		    		spi_fail = 1;
		    		post_log("error : RNE not detected exiting...\n");
		    		END = 1;
		    		break;
		    	}

				fdata = ssp_get_byte(SSP_ID1);
				if(fdata == 0x00)
				{
					post_log("RXed <00h>\n");
					post_log("power up message end\n");
					END = 1;
					break;
				}

			}
			msg[i++] = fdata;
		}
		if(spi_fail)
			post_log("error : SPI Communication failed\n");
		else
			post_log("message received from card reader : %s\n",msg);
    }

return TEST_PASS;
}

int msr_ssp_loop_back_test(void)
{
    char recv[30] = {0};
    int status = TEST_PASS;
 
    _ssp_cfg cfg;
    uint32_t send_i = 0;
    uint32_t recv_i = 0;
    uint32_t counter = 20;
    uint32_t length = counter,i = 0;

    char send[20] = {0x12, 0x34, 0xFF, 0xFF, 0xAB, 0xCD, 0xDE, 0xFA, 0x5A, 0x5A, 0xA5, 0xA5, 0xAB, 0xCD, 0x56, 0x78, 0x1A, 0x2B, 0x9D, 0x6F};

    //Motorola SPI configuration
    //bit rate = Fsspclk / (SSPCPSR * (1 + clock_rate)), Fsspclk = 100MHz for Cygnus we have to configure SSP_CLK_OUT for 100KHz hence SSPCPSR = 20 and scr = 49
    cfg.clk_prescale 	= SSPCR0_CPSDVSR;
    cfg.d_size			= DATA_SIZE_8BIT;
    cfg.format			= MOTOROLA_SPI;
    cfg.lbm				= LOOP_BACK_MODE;
    cfg.ms				= MASTER_MODE;
    cfg.phase			= PHA_0;
    cfg.pol				= POL_0;
    cfg.scr				= SSPCR0_SCR;
    cfg.sod				= 0x00;
    cfg.ssp_id			= SSP_ID1;

    post_log("\nConfiguring SPI-1 for Motorola mode, 8-bit data, Master, SPO=0, SPH=0, and for 100Kbps baud \n");
    ssp_config(&cfg);

	/* DAV Hi to SCL transition should happen within 400ms*/
	while(counter)
	{
		counter--;
		if(TFE_DETECTED != wait_for_tfe())
		{
			
			post_log("error : TFE not detected exiting...\n");
			break;
		}
		ssp_put_byte(SSP_ID1,send[send_i++]);

		if(RNE_DETECTED != wait_for_rne())
		{
			
			post_log("error : RNE not detected exiting...\n");
			break;
		}

		recv[recv_i++] = ssp_get_byte(SSP_ID1);
	}

	post_log("message send     : ");
	for(i = 0 ; i < length ; i++)
		post_log("0x%X ",send[i]);

	post_log("\nmessage received : ");
	for(i = 0 ; i < length ; i++)
		post_log("0x%X ",recv[i]);

	for(i = 0 ; i < length ; i++)
	{
		if(recv[i] != send[i])
		{
			post_log("\nsend message != received message\n");
			status = TEST_FAIL;
			break;
		}
	}
	post_log("\n--Test %s--\n",status == -1 ? "FAILED":"PASSED");
	return status;
}




int MSR_post_test(int flags)
{
#if 1
	int status = TEST_PASS;
    if(TEST_FAIL == msr_card_swipe_test())
	      status = TEST_FAIL;
    return status;

#else

	uint32_t test_no;
	int loop_count = 0, total_tests = 0 , total_reg_tests = 0;
	int status = 0,autorun_status = 0,pass = 0,fail = 0,skip = 0;
	int ret = 0x00,max_loop = 50;

	FUNC_INFO function[]=			{
				 	 	 	 	 	 	 	 ADD_TEST(  msr_power_up_message_test   ),
				 	 	 	 	 	 	 	 ADD_TEST(  msr_card_swipe_test       	),
				 	 	 	 	 	 	 	 ADD_TEST(  msr_ssp_loop_back_test		)
									} ;

	FUNC_INFO regression_func[]	=	{
											ADD_TEST(  msr_ssp_loop_back_test   )
									} ;
	total_tests = (sizeof(function) / sizeof(function[0]));
	total_reg_tests = (sizeof(regression_func) / sizeof(regression_func[0]));
#if 0
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
	else
#endif
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

