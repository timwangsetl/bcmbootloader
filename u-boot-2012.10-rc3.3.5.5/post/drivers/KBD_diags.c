#include <common.h>
#include <post.h>
#if defined(CONFIG_CYGNUS)
#if CONFIG_POST & CONFIG_SYS_POST_KEYPAD
#include "../../halapis/include/kbd_apis.h"
#include "asm/arch/socregs.h"


#define ADD_TEST(X)  { #X , X }
#define TEST_PASS  0
#define TEST_FAIL -1
#define TEST_SKIP  2

#define DEFAULT_ZERO	0x00

typedef struct{

	char* func_name;
	int (*func)(void);
}FUNC_INFO;

_kbd_cfg kbd_test_config = {
		.kpcr 	= ROW_WIDTH(4) | COLUMN_WIDTH(4) | KEYPAD_EN | MODE_PULL_UP,
		.kpior	= ROW_OP_CTRL(0x0f),
		.kpemr0	= 0x00,
		.kpemr1	= 0x00,
		.kpemr2	= 0x00,
		.kpemr3	= 0x00,
		.kpimr0	= 0x00,
		.kpimr1	= 0x00
};


/*
_kbd_cfg kbd_test_config = {
		.kpcr 	= ROW_WIDTH(4) | COLUMN_WIDTH(4) | KEYPAD_EN | MODE_PULL_UP,
		.kpior	= ROW_OP_CTRL(0x0f),
		.kpemr0	= KPEMR(0,0,1) | KPEMR(0,1,1) | KPEMR(0,2,1) | KPEMR(0,3,1) | KPEMR(1,0,1) | KPEMR(1,1,1) | KPEMR(1,2,1) | KPEMR(1,3,1),
		.kpemr1	= KPEMR(0,0,1) | KPEMR(0,1,1) | KPEMR(0,2,1) | KPEMR(0,3,1) | KPEMR(1,0,1) | KPEMR(1,1,1) | KPEMR(1,2,1) | KPEMR(1,3,1),
		.kpemr2	= 0x00,
		.kpemr3	= 0x00,
		.kpimr0	= KPIMR_EN(0,0) | KPIMR_EN(0,1) | KPIMR_EN(0,2) | KPIMR_EN(0,3) | \
				  KPIMR_EN(1,0) | KPIMR_EN(1,1) | KPIMR_EN(1,2) | KPIMR_EN(1,3) | \
				  KPIMR_EN(2,0) | KPIMR_EN(2,1) | KPIMR_EN(2,2) | KPIMR_EN(2,3) | \
				  KPIMR_EN(3,0) | KPIMR_EN(3,1) | KPIMR_EN(3,2) | KPIMR_EN(3,3),
		.kpimr1	= 0x00
};

*/

/*int keymap[4][4] = {  {'D','C','B','A'},
	                  {'#','9','6','3'},
	                  {'0','8','5','2'},
	                  {'*','7','4','1'} };*/
int keymap[4][4] = {  {'D','#','0','*'},
	                  {'C','9','8','7'},
	                  {'B','6','5','4'},
	                  {'A','3','2','1'} };

void kbd_diag_delay(uint32_t d)
{
	while(d--);
}
/* Register dump */
int kbd_register_dump(void)
{
	volatile _kbd_reg *r = (_kbd_reg*)KBD_BASE_ADDRESS;

	post_log("kpcr   = 0x%X\n",r->kpcr);
	post_log("kpior  = 0x%X\n",r->kpior);
	post_log("kpemr0 = 0x%X\n",r->kpemr0);
	post_log("kpemr1 = 0x%X\n",r->kpemr1);
	post_log("kpemr2 = 0x%X\n",r->kpemr2);
	post_log("kpemr3 = 0x%X\n",r->kpemr3);
	post_log("kpssr0 = 0x%X\n",r->kpssr0);
	post_log("kpssr1 = 0x%X\n",r->kpssr1);
	post_log("kpimr0 = 0x%X\n",r->kpimr0);
	post_log("kpimr1 = 0x%X\n",r->kpimr1);
	post_log("kpicr0 = 0x%X\n",r->kpicr0);
	post_log("kpicr1 = 0x%X\n",r->kpicr1);
	post_log("kpisr0 = 0x%X\n",r->kpisr0);
	post_log("kpisr1 = 0x%X\n",r->kpisr1);
	post_log("kpbdctr = 0x%X\n",r->kpbdctr);
	return TEST_PASS;
}

/* Reset value of all registers */
int kbd_reset_value_test(void)
{
	int status = TEST_PASS;
	volatile _kbd_reg *r = (_kbd_reg*)KBD_BASE_ADDRESS;

	post_log("address r->kpimr0 = 0x%X\n",&r->kpimr0);

	kbd_set_config(&kbd_test_config);
	kbd_ctrl_init();
	kbd_reset();

	if(r->kpcr != DEFAULT_ZERO)
	{
		post_log("error : kpcr = 0x%X , default = 0x%X\n",r->kpcr,DEFAULT_ZERO);
		status = TEST_FAIL;
	}

//	if(r->kpior != DEFAULT_ZERO)
//	{
//		post_log("error : kpior = 0x%X , default = 0x%X\n",r->kpior,DEFAULT_ZERO);
//		status = TEST_FAIL;
//	}

	if(r->kpemr0 != DEFAULT_ZERO)
	{
		post_log("error : kpemr0 = 0x%X , default = 0x%X\n",r->kpemr0,DEFAULT_ZERO);
		status = TEST_FAIL;
	}

	if(r->kpemr1 != DEFAULT_ZERO)
	{
		post_log("error : kpemr1 = 0x%X , default = 0x%X\n",r->kpemr1,DEFAULT_ZERO);
		status = TEST_FAIL;
	}

	if(r->kpemr2 != DEFAULT_ZERO)
	{
		post_log("error : kpemr2 = 0x%X , default = 0x%X\n",r->kpemr2,DEFAULT_ZERO);
		status = TEST_FAIL;
	}

	if(r->kpemr3 != DEFAULT_ZERO)
	{
		post_log("error : kpemr3 = 0x%X , default = 0x%X\n",r->kpemr3,DEFAULT_ZERO);
		status = TEST_FAIL;
	}

	if(r->kpssr0 != DEFAULT_ZERO)
	{
		post_log("error : kpssr0 = 0x%X , default = 0x%X\n",r->kpssr0,DEFAULT_ZERO);
		status = TEST_FAIL;
	}

	if(r->kpssr1 != DEFAULT_ZERO)
	{
		post_log("error : kpssr1 = 0x%X , default = 0x%X\n",r->kpssr1,DEFAULT_ZERO);
		status = TEST_FAIL;
	}

	if(r->kpimr0 != DEFAULT_ZERO)
	{
		post_log("error : kpimr0 = 0x%X , default = 0x%X\n",r->kpimr0,DEFAULT_ZERO);
		status = TEST_FAIL;
	}

	if(r->kpimr1 != DEFAULT_ZERO)
	{
		post_log("error : kpimr1 = 0x%X , default = 0x%X\n",r->kpimr1,DEFAULT_ZERO);
		status = TEST_FAIL;
	}

	if(r->kpicr0 != DEFAULT_ZERO)
	{
		post_log("error : kpicr0 = 0x%X , default = 0x%X\n",r->kpicr0,DEFAULT_ZERO);
		status = TEST_FAIL;
	}

	if(r->kpicr1 != DEFAULT_ZERO)
	{
		post_log("error : kpicr1 = 0x%X , default = 0x%X\n",r->kpicr1,DEFAULT_ZERO);
		status = TEST_FAIL;
	}

	if(r->kpisr0 != DEFAULT_ZERO)
	{
		post_log("error : kpisr0 = 0x%X , default = 0x%X\n",r->kpisr0,DEFAULT_ZERO);
		status = TEST_FAIL;
	}

	if(r->kpisr1 != DEFAULT_ZERO)
	{
		post_log("error : kpisr1 = 0x%X , default = 0x%X\n",r->kpisr1,DEFAULT_ZERO);
		status = TEST_FAIL;
	}

	if(r->kpbdctr != DEFAULT_ZERO)
	{
		post_log("error : kpbdctr = 0x%X , default = 0x%X\n",r->kpbdctr,DEFAULT_ZERO);
		status = TEST_FAIL;
	}
	post_log("--Test %s--\n",status == TEST_PASS ? "PASSED" : "FAILED");
	return status;
}

int kbd_get_key_press_pullup_polling(void)
{
	char c;
	/* Configure pullup_polling*/
	kbd_test_config.kpcr = ROW_WIDTH(4) | COLUMN_WIDTH(4) | KEYPAD_EN | STATUS_FILTER_ENABLE | STATUS_FILTER_TYP(5) | COLUMN_FILTER_EN | COLUMN_FILTER_TYPE(7) | MODE_PULL_UP;
	kbd_test_config.kpemr0	= 0x00;
	kbd_test_config.kpemr1	= 0x00;
	kbd_test_config.kpemr2	= 0x00;
	kbd_test_config.kpemr3	= 0x00;
	kbd_test_config.kpimr0	= 0x00;
	kbd_test_config.kpimr1	= 0x00;

	kbd_set_config(&kbd_test_config);
	kbd_ctrl_init();
	kbd_diag_delay(500);
	kbd_register_dump();

	if(KBD_KEY_DETECTED != kbd_key_pressed_stat())
	{
		post_log("error : key not detected in mode - pullup_polling\n");
		return TEST_FAIL;
	}

	c = kbd_extract_key_press_status();
	if(c == KBD_NO_KEY_FOUND)
	{
		post_log("error : key not detected in mode - pullup_polling\n");
		return TEST_FAIL;
	}

	post_log("Key found ==> %c\n",c);
	return TEST_PASS;
}

int kbd_get_key_press_pullup_intrrupt(void)
{
	char c;
	/* Configure pullup_polling*/
	kbd_test_config.kpcr 	= ROW_WIDTH(4)  | COLUMN_WIDTH(4) | KEYPAD_EN     | STATUS_FILTER_ENABLE | STATUS_FILTER_TYP(5) | COLUMN_FILTER_EN | COLUMN_FILTER_TYPE(7) | MODE_PULL_UP;
	kbd_test_config.kpemr0	= KPEMR(0,0,1)  | KPEMR(0,1,1)    | KPEMR(0,2,1)  | KPEMR(0,3,1)  | KPEMR(1,0,1)  | KPEMR(1,1,1)  | KPEMR(1,2,1)  | KPEMR(1,3,1);
	kbd_test_config.kpemr1	= KPEMR(0,0,1)  | KPEMR(0,1,1)    | KPEMR(0,2,1)  | KPEMR(0,3,1)  | KPEMR(1,0,1)  | KPEMR(1,1,1)  | KPEMR(1,2,1)  | KPEMR(1,3,1);
	kbd_test_config.kpemr2	= 0x00;
	kbd_test_config.kpemr3	= 0x00;
	kbd_test_config.kpimr0	= KPIMR_EN(0,0) | KPIMR_EN(0,1)   | KPIMR_EN(0,2) | KPIMR_EN(0,3) | KPIMR_EN(1,0) | KPIMR_EN(1,1) | KPIMR_EN(1,2) | KPIMR_EN(1,3) | \
			  	  	  	  	  KPIMR_EN(2,0) | KPIMR_EN(2,1)   | KPIMR_EN(2,2) | KPIMR_EN(2,3) | KPIMR_EN(3,0) | KPIMR_EN(3,1) | KPIMR_EN(3,2) | KPIMR_EN(3,3) ;
	kbd_test_config.kpimr1	= 0x00;

	kbd_set_config(&kbd_test_config);
	kbd_ctrl_init();
	kbd_diag_delay(500);
	kbd_register_dump();

	if(KBD_KEY_DETECTED != kbd_key_pressed_int())
	{
		post_log("error : key not detected in mode - pullup_interrupt\n");
		return TEST_FAIL;
	}

	c = kbd_extract_key_press_int();
	if(c == KBD_NO_KEY_FOUND)
	{
		post_log("error : key not detected in mode - pullup_interrupt\n");
		return TEST_FAIL;
	}

	post_log("Key found ==> %c\n",c);
	return TEST_PASS;
}

int kbd_get_key_press_pulldown_polling(void)
{
	char c;
	/* Configure pullup_polling*/
	kbd_test_config.kpcr 	= ROW_WIDTH(4) | COLUMN_WIDTH(4) | KEYPAD_EN | STATUS_FILTER_ENABLE | STATUS_FILTER_TYP(5) | COLUMN_FILTER_EN | COLUMN_FILTER_TYPE(7);
	kbd_test_config.kpemr0	= 0x00;
	kbd_test_config.kpemr1	= 0x00;
	kbd_test_config.kpemr2	= 0x00;
	kbd_test_config.kpemr3	= 0x00;
	kbd_test_config.kpimr0	= 0x00;
	kbd_test_config.kpimr1	= 0x00;

	kbd_set_config(&kbd_test_config);
	kbd_ctrl_init();
	kbd_diag_delay(500);
	kbd_register_dump();

	if(KBD_KEY_DETECTED != kbd_key_pressed_stat())
	{
		post_log("error : key not detected in mode - pulldown_polling\n");
		return TEST_FAIL;
	}

	c = kbd_extract_key_press_status();
	if(c == KBD_NO_KEY_FOUND)
	{
		post_log("error : key not detected in mode - pulldown_polling\n");
		return TEST_FAIL;
	}

	post_log("Key found ==> %c\n",c);
	return TEST_PASS;
}

int kbd_get_key_press_pulldown_intrrupt(void)
{
	char c;
	/* Configure pullup_polling*/
	kbd_test_config.kpcr 	= ROW_WIDTH(4) | COLUMN_WIDTH(4) | KEYPAD_EN | STATUS_FILTER_ENABLE | STATUS_FILTER_TYP(5) | COLUMN_FILTER_EN | COLUMN_FILTER_TYPE(7);
	kbd_test_config.kpemr0	= KPEMR(0,0,2)  | KPEMR(0,1,2)    | KPEMR(0,2,2)  | KPEMR(0,3,2)  | KPEMR(1,0,2)  | KPEMR(1,1,2)  | KPEMR(1,2,2)  | KPEMR(1,3,2);
	kbd_test_config.kpemr1	= KPEMR(0,0,2)  | KPEMR(0,1,2)    | KPEMR(0,2,2)  | KPEMR(0,3,2)  | KPEMR(1,0,2)  | KPEMR(1,1,2)  | KPEMR(1,2,2)  | KPEMR(1,3,2);

	kbd_test_config.kpemr2	= 0x00;
	kbd_test_config.kpemr3	= 0x00;
	kbd_test_config.kpimr0	= KPIMR_EN(0,0) | KPIMR_EN(0,1)   | KPIMR_EN(0,2) | KPIMR_EN(0,3) | KPIMR_EN(1,0) | KPIMR_EN(1,1) | KPIMR_EN(1,2) | KPIMR_EN(1,3) | \
			  	  	  	  	  KPIMR_EN(2,0) | KPIMR_EN(2,1)   | KPIMR_EN(2,2) | KPIMR_EN(2,3) | KPIMR_EN(3,0) | KPIMR_EN(3,1) | KPIMR_EN(3,2) | KPIMR_EN(3,3) ;
	kbd_test_config.kpimr1	= 0x00;

	kbd_set_config(&kbd_test_config);
	kbd_ctrl_init();
	kbd_diag_delay(500);
	kbd_register_dump();

	if(KBD_KEY_DETECTED != kbd_key_pressed_int())
	{
		post_log("error : key not detected in mode - pulldown_interrupt\n");
		return TEST_FAIL;
	}

	c = kbd_extract_key_press_int();
	if(c == KBD_NO_KEY_FOUND)
	{
		post_log("error : key not detected in mode - pulldown_interrupt\n");
		return TEST_FAIL;
	}

	post_log("Key found ==> %c\n",c);
	return TEST_PASS;
}

int kbd_enter_keypad_pullup_polling_mode(void)
{
	char c;
	char a[5];
	int i = 0;


	/* Configure pullup_polling*/
	kbd_test_config.kpcr = ROW_WIDTH(4) | COLUMN_WIDTH(4) | KEYPAD_EN | STATUS_FILTER_ENABLE | STATUS_FILTER_TYP(3) | COLUMN_FILTER_EN | COLUMN_FILTER_TYPE(2) | MODE_PULL_UP;
	//kbd_test_config.kpcr = ROW_WIDTH(4) | COLUMN_WIDTH(4) | KEYPAD_EN | MODE_PULL_UP;
	kbd_test_config.kpemr0	= 0x00;
	kbd_test_config.kpemr1	= 0x00;
	kbd_test_config.kpemr2	= 0x00;
	kbd_test_config.kpemr3	= 0x00;
	kbd_test_config.kpimr0	= 0x00;
	kbd_test_config.kpimr1	= 0x00;
	//kbd_test_config.kpior	= 0x0F000F00;

	kbd_set_config(&kbd_test_config);
	kbd_ctrl_init();
	kbd_diag_delay(500);
	post_log("kpior = 0x%X\n",keypad_read(KEYPAD_TOP_REGS_KPIOR));
	keypad_write(0x0F000F00, KEYPAD_TOP_REGS_KPIOR);
	kbd_register_dump();

	post_log(" You are entering keypad pull-up polling mode, \n Press key to get displayed here \n press key in the sequence '121' to exit the mode \n");

	while(1)
	{
		c = kbd_extract_key_press_status();

		if(c != KBD_NO_KEY_FOUND)
		{
			post_log("%c",c);

			if( (c == '1') || (c == '2') )
			{
				if( (i == 2) && (c == '1') )
				{
					if( (a[0] == '1') && (a[1] == '2') )
						return TEST_PASS;
					i = 0;
				}
				else if((i == 1) && (c == '2'))
					a[i++] = c;



				else if ((i == 0) && (c == '1'))
					a[i++] = c;

				else
				{
					if(c == '1')
					{
						a[0] = c;
						i = 1;
					}
					else
						i = 0;
				}

			}
			else
				i = 0;

			kbd_diag_delay(80000000);
			kbd_diag_delay(80000000);
//			kbd_diag_delay(80000000);
		}

		//test_no = post_getUserInput ("\nEnter");
	}

	return TEST_PASS;
}

int kbd_enter_keypad_pullup_interrupt_mode(void)
{
	char c;
	char a[5];
	int i = 0;

	/* Configure pullup_polling*/
	//kbd_test_config.kpcr 	= ROW_WIDTH(4) | COLUMN_WIDTH(4) | KEYPAD_EN | STATUS_FILTER_ENABLE | STATUS_FILTER_TYP(5) | COLUMN_FILTER_EN | COLUMN_FILTER_TYPE(7) | MODE_PULL_UP;
	kbd_test_config.kpcr 	= ROW_WIDTH(4) | COLUMN_WIDTH(4) | KEYPAD_EN | MODE_PULL_UP;
	kbd_test_config.kpemr0	= KPEMR(0,0,2)  | KPEMR(0,1,2)    | KPEMR(0,2,2)  | KPEMR(0,3,2)  | KPEMR(1,0,2)  | KPEMR(1,1,2)  | KPEMR(1,2,2)  | KPEMR(1,3,2);
	kbd_test_config.kpemr1	= KPEMR(0,0,2)  | KPEMR(0,1,2)    | KPEMR(0,2,2)  | KPEMR(0,3,2)  | KPEMR(1,0,2)  | KPEMR(1,1,2)  | KPEMR(1,2,2)  | KPEMR(1,3,2);
	kbd_test_config.kpemr2	= 0x00;
	kbd_test_config.kpemr3	= 0x00;
	kbd_test_config.kpimr0	= KPIMR_EN(0,0) | KPIMR_EN(0,1)   | KPIMR_EN(0,2) | KPIMR_EN(0,3) | KPIMR_EN(1,0) | KPIMR_EN(1,1) | KPIMR_EN(1,2) | KPIMR_EN(1,3) | \
			  	  	  	  	  KPIMR_EN(2,0) | KPIMR_EN(2,1)   | KPIMR_EN(2,2) | KPIMR_EN(2,3) | KPIMR_EN(3,0) | KPIMR_EN(3,1) | KPIMR_EN(3,2) | KPIMR_EN(3,3) ;
	kbd_test_config.kpimr1	= 0x00;
	//kbd_test_config.kpior	= 0x0F000F00;

	kbd_set_config(&kbd_test_config);
	kbd_ctrl_init();
	post_log("kpior = 0x%X\n",keypad_read(KEYPAD_TOP_REGS_KPIOR));
	keypad_write(0x0F000F00, KEYPAD_TOP_REGS_KPIOR);
	kbd_diag_delay(500);
	kbd_register_dump();

	post_log(" You are entering keypad pull-up interrupt mode, \n Press key to get displayed here \n press key in the sequence '121' to exit the mode \n");

	while(1)
	{
		c = kbd_extract_key_press_int();

		if(c != KBD_NO_KEY_FOUND)
		{
			//kbd_register_dump();
			post_log("%c",c);

			if( (c == '1') || (c == '2') )
			{
				if( (i == 2) && (c == '1') )
				{
					if( (a[0] == '1') && (a[1] == '2') )
						return TEST_PASS;
					i = 0;
				}
				else if((i == 1) && (c == '2'))
					a[i++] = c;



				else if ((i == 0) && (c == '1'))
					a[i++] = c;

				else
				{
					if(c == '1')
					{
						a[0] = c;
						i = 1;
					}
					else
						i = 0;
				}

			}
			else
				i = 0;
//
//			kbd_diag_delay(80000000);
//			kbd_diag_delay(80000000);
//			kbd_diag_delay(80000000);
		}
	}
	return TEST_PASS;
}
int kbd_enter_keypad_pulldown_polling_mode(void)
{
	char c;
	char a[5];
	int i = 0;

	/* Configure pullup_polling*/
	kbd_test_config.kpcr = ROW_WIDTH(4) | COLUMN_WIDTH(4) | KEYPAD_EN | STATUS_FILTER_ENABLE | STATUS_FILTER_TYP(5) | COLUMN_FILTER_EN | COLUMN_FILTER_TYPE(7);
	kbd_test_config.kpemr0	= 0x00;
	kbd_test_config.kpemr1	= 0x00;
	kbd_test_config.kpemr2	= 0x00;
	kbd_test_config.kpemr3	= 0x00;
	kbd_test_config.kpimr0	= 0x00;
	kbd_test_config.kpimr1	= 0x00;
	//kbd_test_config.kpior	= 0x0F000000;

	kbd_set_config(&kbd_test_config);
	kbd_ctrl_init();
	post_log("kpior = 0x%X\n",keypad_read(KEYPAD_TOP_REGS_KPIOR));
	keypad_write(0x0F000000, KEYPAD_TOP_REGS_KPIOR);

	kbd_diag_delay(500);
	kbd_register_dump();

	post_log(" You are entering keypad pull-down polling mode, \n Press key to get displayed here \n press key in the sequence '121' to exit the mode \n");

	while(1)
	{
		c = kbd_extract_key_press_status();

		if(c != KBD_NO_KEY_FOUND)
		{
			post_log("%c\n",c);

			if( (c == '1') || (c == '2') )
			{
				if( (i == 2) && (c == '1') )
				{
					if( (a[0] == '1') && (a[1] == '2') )
						return TEST_PASS;
					i = 0;
				}
				else if(i < 2)
					a[i++] = c;

				else
					i = 0;
			}
			else
				i = 0;
		}
		kbd_diag_delay(80000000);
		kbd_diag_delay(80000000);
	}
	return TEST_PASS;
}

int kbd_enter_keypad_pulldown_interrupt_mode(void)
{
	char c;
	char a[5];
	int i = 0;

	/* Configure pullup_polling*/
	//kbd_test_config.kpcr 	= ROW_WIDTH(4) | COLUMN_WIDTH(4) | KEYPAD_EN | STATUS_FILTER_ENABLE | STATUS_FILTER_TYP(5) | COLUMN_FILTER_EN | COLUMN_FILTER_TYPE(7) ;
	kbd_test_config.kpcr 	= ROW_WIDTH(4) | COLUMN_WIDTH(4) | KEYPAD_EN;
	kbd_test_config.kpemr0	= KPEMR(0,0,1)  | KPEMR(0,1,1)    | KPEMR(0,2,1)  | KPEMR(0,3,1)  | KPEMR(1,0,1)  | KPEMR(1,1,1)  | KPEMR(1,2,1)  | KPEMR(1,3,1);
	kbd_test_config.kpemr1	= KPEMR(0,0,1)  | KPEMR(0,1,1)    | KPEMR(0,2,1)  | KPEMR(0,3,1)  | KPEMR(1,0,1)  | KPEMR(1,1,1)  | KPEMR(1,2,1)  | KPEMR(1,3,1);
	kbd_test_config.kpemr2	= 0x00;
	kbd_test_config.kpemr3	= 0x00;
	kbd_test_config.kpimr0	= KPIMR_EN(0,0) | KPIMR_EN(0,1)   | KPIMR_EN(0,2) | KPIMR_EN(0,3) | KPIMR_EN(1,0) | KPIMR_EN(1,1) | KPIMR_EN(1,2) | KPIMR_EN(1,3) | \
			  	  	  	  	  KPIMR_EN(2,0) | KPIMR_EN(2,1)   | KPIMR_EN(2,2) | KPIMR_EN(2,3) | KPIMR_EN(3,0) | KPIMR_EN(3,1) | KPIMR_EN(3,2) | KPIMR_EN(3,3) ;
	kbd_test_config.kpimr1	= 0x00;
	//kbd_test_config.kpior	= 0x0F000000;

	kbd_set_config(&kbd_test_config);
	kbd_ctrl_init();
	post_log("kpior = 0x%X\n",keypad_read(KEYPAD_TOP_REGS_KPIOR));
	keypad_write(0x0F000F00, KEYPAD_TOP_REGS_KPIOR);
	kbd_diag_delay(500);
	kbd_register_dump();

	post_log(" You are entering keypad pull-down interrupt mode, \n Press key to get displayed here \n press key in the sequence '121' to exit the mode \n");
	while(1)
	{
		c = kbd_extract_key_press_int();

		if(c != KBD_NO_KEY_FOUND)
		{
			kbd_register_dump();
			post_log("%c\n",c);

			if( (c == '1') || (c == '2') )
			{
				if( (i == 2) && (c == '1') )
				{
					if( (a[0] == '1') && (a[1] == '2') )
						return TEST_PASS;
					i = 0;
				}
				else if(i < 2)
					a[i++] = c;

				else
					i = 0;
			}
			else
				i = 0;
		}
		kbd_diag_delay(1000);
	}

	return TEST_PASS;
}

int kbd_enter_keypad_pulldown_interrupt_mode_dbg1(void)
{
	char c;
	char a[5];
	int i = 0;

	/* Configure pullup_polling*/
	kbd_test_config.kpcr 	= ROW_WIDTH(4) | COLUMN_WIDTH(4) | KEYPAD_EN | STATUS_FILTER_ENABLE | STATUS_FILTER_TYP(5) | COLUMN_FILTER_EN | COLUMN_FILTER_TYPE(7) | SWAP_ROW_COL ;
	kbd_test_config.kpemr0	= KPEMR(0,0,1)  | KPEMR(0,1,1)    | KPEMR(0,2,1)  | KPEMR(0,3,1)  | KPEMR(1,0,1)  | KPEMR(1,1,1)  | KPEMR(1,2,1)  | KPEMR(1,3,1);
	kbd_test_config.kpemr1	= KPEMR(0,0,1)  | KPEMR(0,1,1)    | KPEMR(0,2,1)  | KPEMR(0,3,1)  | KPEMR(1,0,1)  | KPEMR(1,1,1)  | KPEMR(1,2,1)  | KPEMR(1,3,1);
	kbd_test_config.kpemr2	= 0x00;
	kbd_test_config.kpemr3	= 0x00;
	kbd_test_config.kpimr0	= KPIMR_EN(0,0) | KPIMR_EN(0,1)   | KPIMR_EN(0,2) | KPIMR_EN(0,3) | KPIMR_EN(1,0) | KPIMR_EN(1,1) | KPIMR_EN(1,2) | KPIMR_EN(1,3) | \
			  	  	  	  	  KPIMR_EN(2,0) | KPIMR_EN(2,1)   | KPIMR_EN(2,2) | KPIMR_EN(2,3) | KPIMR_EN(3,0) | KPIMR_EN(3,1) | KPIMR_EN(3,2) | KPIMR_EN(3,3) ;
	kbd_test_config.kpimr1	= 0x00;

	kbd_set_config(&kbd_test_config);
	kbd_ctrl_init();
	kbd_diag_delay(500);
	kbd_register_dump();

	post_log(" You are entering keypad pull-down interrupt mode, \n Press key to get displayed here \n press key in the sequence '121' to exit the mode \n");
	while(1)
	{
		c = kbd_extract_key_press_int();

		if(c != KBD_NO_KEY_FOUND)
		{
			post_log("%c\n",c);

			if( (c == '1') || (c == '2') )
			{
				if( (i == 2) && (c == '1') )
				{
					if( (a[0] == '1') && (a[1] == '2') )
						return TEST_PASS;
					i = 0;
				}
				else if(i < 2)
					a[i++] = c;

				else
					i = 0;
			}
			else
				i = 0;
		}
		kbd_diag_delay(1000);
	}

	return TEST_PASS;
}

int kbd_enter_keypad_pulldown_interrupt_mode_dbg2(void)
{
	char c;
	char a[5];
	int i = 0;

	/* Configure pullup_polling*/
	kbd_test_config.kpcr 	= ROW_WIDTH(4) | COLUMN_WIDTH(4) | KEYPAD_EN ;
	kbd_test_config.kpemr0	= KPEMR(0,0,1)  | KPEMR(0,1,1)    | KPEMR(0,2,1)  | KPEMR(0,3,1)  | KPEMR(1,0,1)  | KPEMR(1,1,1)  | KPEMR(1,2,1)  | KPEMR(1,3,1);
	kbd_test_config.kpemr1	= KPEMR(0,0,1)  | KPEMR(0,1,1)    | KPEMR(0,2,1)  | KPEMR(0,3,1)  | KPEMR(1,0,1)  | KPEMR(1,1,1)  | KPEMR(1,2,1)  | KPEMR(1,3,1);
	kbd_test_config.kpemr2	= 0x00;
	kbd_test_config.kpemr3	= 0x00;
	kbd_test_config.kpimr0	= KPIMR_EN(0,0) | KPIMR_EN(0,1)   | KPIMR_EN(0,2) | KPIMR_EN(0,3) | KPIMR_EN(1,0) | KPIMR_EN(1,1) | KPIMR_EN(1,2) | KPIMR_EN(1,3) | \
			  	  	  	  	  KPIMR_EN(2,0) | KPIMR_EN(2,1)   | KPIMR_EN(2,2) | KPIMR_EN(2,3) | KPIMR_EN(3,0) | KPIMR_EN(3,1) | KPIMR_EN(3,2) | KPIMR_EN(3,3) ;
	kbd_test_config.kpimr1	= 0x00;
	kbd_test_config.kpior	= 0x00;

	kbd_set_config(&kbd_test_config);
	kbd_ctrl_init();
	kbd_diag_delay(500);
	kbd_register_dump();

	post_log(" You are entering keypad pull-down interrupt mode, \n Press key to get displayed here \n press key in the sequence '121' to exit the mode \n");
	while(1)
	{
		c = kbd_extract_key_press_int();

		if(c != KBD_NO_KEY_FOUND)
		{
			post_log("%c\n",c);

			if( (c == '1') || (c == '2') )
			{
				if( (i == 2) && (c == '1') )
				{
					if( (a[0] == '1') && (a[1] == '2') )
						return TEST_PASS;
					i = 0;
				}
				else if(i < 2)
					a[i++] = c;

				else
					i = 0;
			}
			else
				i = 0;
		}
		kbd_diag_delay(1000);
	}

	return TEST_PASS;
}

int kbd_enter_keypad_pullup_interrupt_mode_dbg3(void)
{
	char c;
	char a[5];
	int i = 0;

	/* Configure pullup_polling*/
	kbd_test_config.kpcr 	= ROW_WIDTH(4) | COLUMN_WIDTH(4) | KEYPAD_EN | MODE_PULL_UP;
	kbd_test_config.kpemr0	= KPEMR(0,0,2)  | KPEMR(0,1,2)    | KPEMR(0,2,2)  | KPEMR(0,3,2)  | KPEMR(1,0,2)  | KPEMR(1,1,2)  | KPEMR(1,2,2)  | KPEMR(1,3,2);
	kbd_test_config.kpemr1	= KPEMR(0,0,2)  | KPEMR(0,1,2)    | KPEMR(0,2,2)  | KPEMR(0,3,2)  | KPEMR(1,0,2)  | KPEMR(1,1,2)  | KPEMR(1,2,2)  | KPEMR(1,3,2);
	kbd_test_config.kpemr2	= 0x00;
	kbd_test_config.kpemr3	= 0x00;
	kbd_test_config.kpimr0	= KPIMR_EN(0,0) | KPIMR_EN(0,1)   | KPIMR_EN(0,2) | KPIMR_EN(0,3) | KPIMR_EN(1,0) | KPIMR_EN(1,1) | KPIMR_EN(1,2) | KPIMR_EN(1,3) | \
			  	  	  	  	  KPIMR_EN(2,0) | KPIMR_EN(2,1)   | KPIMR_EN(2,2) | KPIMR_EN(2,3) | KPIMR_EN(3,0) | KPIMR_EN(3,1) | KPIMR_EN(3,2) | KPIMR_EN(3,3) ;
	kbd_test_config.kpimr1	= 0x00;

	kbd_set_config(&kbd_test_config);
	kbd_ctrl_init();
	kbd_diag_delay(500);
	kbd_register_dump();

	post_log(" You are entering keypad pull-up interrupt mode, \n Press key to get displayed here \n press key in the sequence '121' to exit the mode \n");

	while(1)
	{
		c = kbd_extract_key_press_int();

		if(c != KBD_NO_KEY_FOUND)
		{
			post_log("%c\n",c);

			if( (c == '1') || (c == '2') )
			{
				if( (i == 2) && (c == '1') )
				{
					if( (a[0] == '1') && (a[1] == '2') )
						return TEST_PASS;
					i = 0;
				}
				else if(i < 2)
					a[i++] = c;

				else
					i = 0;
			}
			else
				i = 0;
		}
		kbd_diag_delay(1000);
	}
	return TEST_PASS;
}
int kbd_drive_rowlines_r1_r3(void)
{
	/* Configure pullup_polling*/
	kbd_test_config.kpcr 	= ROW_WIDTH(4) | COLUMN_WIDTH(4) | KEYPAD_EN | MODE_PULL_UP;
	kbd_test_config.kpemr0	= KPEMR(0,0,2)  | KPEMR(0,1,2)    | KPEMR(0,2,2)  | KPEMR(0,3,2)  | KPEMR(1,0,2)  | KPEMR(1,1,2)  | KPEMR(1,2,2)  | KPEMR(1,3,2);
	kbd_test_config.kpemr1	= KPEMR(0,0,2)  | KPEMR(0,1,2)    | KPEMR(0,2,2)  | KPEMR(0,3,2)  | KPEMR(1,0,2)  | KPEMR(1,1,2)  | KPEMR(1,2,2)  | KPEMR(1,3,2);
	kbd_test_config.kpemr2	= 0x00;
	kbd_test_config.kpemr3	= 0x00;
	kbd_test_config.kpimr0	= KPIMR_EN(0,0) | KPIMR_EN(0,1)   | KPIMR_EN(0,2) | KPIMR_EN(0,3) | KPIMR_EN(1,0) | KPIMR_EN(1,1) | KPIMR_EN(1,2) | KPIMR_EN(1,3) | \
			  	  	  	  	  KPIMR_EN(2,0) | KPIMR_EN(2,1)   | KPIMR_EN(2,2) | KPIMR_EN(2,3) | KPIMR_EN(3,0) | KPIMR_EN(3,1) | KPIMR_EN(3,2) | KPIMR_EN(3,3) ;
	kbd_test_config.kpimr1	= 0x00;

	kbd_set_config(&kbd_test_config);
	kbd_ctrl_init();
	kbd_diag_delay(500);
	kbd_register_dump();

	uint32_t d = 0;
	/* To write KBD registers */
	post_log("KPIOR Initial data = 0x%X\n",kbd_read_reg(KEYPAD_TOP_REGS_KPIOR));
	post_log("Driving Row lines to R0 = 1, R1 = 0, R2 = 1, R3 = 0\n");
	d = (0xFF << KEYPAD_TOP_REGS_KPIOR__RowOContrl_R) | (0x0A << KEYPAD_TOP_REGS_KPIOR__RowIOData_R);
	kbd_write_reg(KEYPAD_TOP_REGS_KPIOR, d);
	return TEST_PASS;
}


int kbd_drive_rowlines_r0_r2(void)
{
	/* Configure pullup_polling*/
	kbd_test_config.kpcr 	= ROW_WIDTH(4) | COLUMN_WIDTH(4) | KEYPAD_EN | MODE_PULL_UP;
	kbd_test_config.kpemr0	= KPEMR(0,0,2)  | KPEMR(0,1,2)    | KPEMR(0,2,2)  | KPEMR(0,3,2)  | KPEMR(1,0,2)  | KPEMR(1,1,2)  | KPEMR(1,2,2)  | KPEMR(1,3,2);
	kbd_test_config.kpemr1	= KPEMR(0,0,2)  | KPEMR(0,1,2)    | KPEMR(0,2,2)  | KPEMR(0,3,2)  | KPEMR(1,0,2)  | KPEMR(1,1,2)  | KPEMR(1,2,2)  | KPEMR(1,3,2);
	kbd_test_config.kpemr2	= 0x00;
	kbd_test_config.kpemr3	= 0x00;
	kbd_test_config.kpimr0	= KPIMR_EN(0,0) | KPIMR_EN(0,1)   | KPIMR_EN(0,2) | KPIMR_EN(0,3) | KPIMR_EN(1,0) | KPIMR_EN(1,1) | KPIMR_EN(1,2) | KPIMR_EN(1,3) | \
			  	  	  	  	  KPIMR_EN(2,0) | KPIMR_EN(2,1)   | KPIMR_EN(2,2) | KPIMR_EN(2,3) | KPIMR_EN(3,0) | KPIMR_EN(3,1) | KPIMR_EN(3,2) | KPIMR_EN(3,3) ;
	kbd_test_config.kpimr1	= 0x00;

	kbd_set_config(&kbd_test_config);
	kbd_ctrl_init();
	kbd_diag_delay(500);
	kbd_register_dump();

	uint32_t d = 0;
	/* To write KBD registers */
	post_log("KPIOR Initial data = 0x%X\n",kbd_read_reg(KEYPAD_TOP_REGS_KPIOR));
	post_log("Driving Row lines to R0 = 1, R1 = 0, R2 = 1, R3 = 0\n");
	d = (0xFF << KEYPAD_TOP_REGS_KPIOR__RowOContrl_R) | (0x05 << KEYPAD_TOP_REGS_KPIOR__RowIOData_R);
	kbd_write_reg(KEYPAD_TOP_REGS_KPIOR, d);
	return TEST_PASS;
}

int kbd_test(void)
{
	 uint32_t v,flag_to_quit;
	  uint32_t bit;
	  uint32_t row,col;

	  cpu_wr_single(0x301d0bc,0,4);
	  keypad_init();

	  post_log("Please press a key \n");
	  post_log("Press '*' and '#' to exit \n");

	  while (1)
	    {
	       flag_to_quit = 0;
	       v = keypad_read(KEYPAD_TOP_REGS_KPISR0);
	       if (v)
	          {
		        keypad_write(0xFFFFFFFF,KEYPAD_TOP_REGS_KPICR0);
			    v = keypad_read(KEYPAD_TOP_REGS_KPSSR0);

			    bit = 0;
			    for (bit = 0;bit < 32;bit++)
			        {
				       if ((v & (1 << bit)) != 0)
				           continue;

				       row = bit % 8;
				       col = bit / 8;

				       if ((keymap[row][col] == '*') || (keymap[row][col] == '#'))
				              flag_to_quit = flag_to_quit + 1;

					   post_log("The Pressed Key is %c \n",keymap[row][col]);
					}
			    }

			if (flag_to_quit >= 2)
			  {
				  post_log("'#' and '*' pressed at the same time, quit! \n");
				  break;
			  }


		  }
	   return TEST_PASS;

}

/******* Internal test function start ********/
int KBD_post_test(int flags)
{
#if 1
	kbd_board_specific_init();
	int status = TEST_PASS;
    if(TEST_FAIL == kbd_enter_keypad_pullup_interrupt_mode())
	      status = TEST_FAIL;
    return status;

#else

	uint32_t test_no;
    int loop_count = 0, total_tests = 0 , total_reg_tests = 0;
    int status = 0,autorun_status = 0,pass = 0,fail = 0,skip = 0;
    kbd_reset();
    kbd_diag_delay(500);

    kbd_board_specific_init();
    FUNC_INFO function[]		=	{
    							 	 ADD_TEST( kbd_register_dump	      					 ),
    							 	 ADD_TEST( kbd_reset_value_test 	     				 ),
    							 	 ADD_TEST( kbd_enter_keypad_pullup_polling_mode 		 ),
    							 	 ADD_TEST( kbd_enter_keypad_pullup_interrupt_mode 		 ),
    							 	 ADD_TEST( kbd_enter_keypad_pulldown_polling_mode 		 ),
    							 	 ADD_TEST( kbd_enter_keypad_pulldown_interrupt_mode 	 ),
    							 	 ADD_TEST( kbd_test)

									};

    FUNC_INFO regression_func[]	=	{
	 	 	 	 	 	 	 	 	 ADD_TEST( kbd_reset_value_test      )
	 	 	 	 	 	 	 	 	 //ADD_TEST(  ),
	 	 	 	 	 	 	 	 	 //ADD_TEST(  )
									};

    total_tests = (sizeof(function) / sizeof(function[0]));
    total_reg_tests = (sizeof(regression_func) / sizeof(regression_func[0]));
#if 0
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
#endif
#endif

