#include <common.h>
#include <post.h>
#if defined(CONFIG_CYGNUS)
#if CONFIG_POST & CONFIG_SYS_POST_LCD
#include "../../halapis/include/lcd_apis.h"
#include "asm/arch/socregs.h"

DECLARE_GLOBAL_DATA_PTR;

#define ADD_TEST(X)  { #X , X }
#define TEST_PASS  0
#define TEST_FAIL -1
#define TEST_SKIP  2


	#define TIM0 0x57272FC8//0x58283031 - Modified by Sathish
	#define TIM1 0x200D09DF//0x200D0DDF - Modified by Sathish
	#define TIM2 0x31F0008//0x71F0020 - Modified by Sathish
	#define TIM3 0x10002
	#define CNTL (LCD_CNTL_LCDEN | LCD_CNTL_LCDTFT | LCD_CNTL_LCDPWR | LCD_CNTL_LCDVCOMP(3) | LCD_CNTL_WATERMARK )
	#define PPL 800
	#define LPP 480


#define BPP1 1
#define BPP2 2
#define BPP4 4
#define BPP8 8

#define LCD_FRAME_BUFFER_ADDRESS (void *)(gd->fb_base)

struct clcd_config local_config = {
	    .tim0 	= TIM0,
	    .tim1 	= TIM1,
	    .tim2 	= TIM2,
	    .tim3 	= TIM3,
	    .cntl 	= 0x00
};


/* Default Register Value */
#define LCD_DEF_TIM0		0x00
#define LCD_DEF_TIM1		0x00
#define LCD_DEF_TIM2		0x00
#define LCD_DEF_TIM3		0x00
#define LCD_DEF_UBASE		0x00
#define LCD_DEF_LBASE		0x00
#define LCD_DEF_CNTL		0x00
#define LCD_DEF_IMSC		0x00
#define LCD_DEF_RIS			0x00
#define LCD_DEF_MIS			0x00
#define LCD_DEF_ICR			0x00
#define LCD_DEF_UCURR		0x00
#define LCD_DEF_LCURR		0x00
#define LCD_DEF_CURCTR		0x00
#define LCD_DEF_CURCFG		0x00
#define LCD_DEF_CURPALETT0	0x00
#define LCD_DEF_CURPALETT1	0x00
#define LCD_DEF_CURXY		0x00
#define LCD_DEF_CURCLIP		0x00
#define LCD_DEF_CURIMSC		0x00
#define LCD_DEF_CURICR		0x00
#define LCD_DEF_CURRIS		0x00
#define LCD_DEF_CURMIS		0x00
#define LCD_DEF_PERID0		0x11
#define LCD_DEF_PERID1		0x11
#define LCD_DEF_PERID2		0x14
#define LCD_DEF_PERID3		0x00
#define LCD_DEF_CELLID0		0x0D
#define LCD_DEF_CELLID1		0xF0
#define LCD_DEF_CELLID2		0x05
#define LCD_DEF_CELLID3		0xB1


typedef struct{

	char* func_name;
	int (*func)(void);
}FUNC_INFO;

/* Peripheral ID read */
int lcd_prih_id_test(void)
{
	int status = TEST_PASS;
	uint32_t p_id = lcd_get_peripheral_id();

	post_log("Peripheral ID : 0x%X\n",p_id);
	return status;
}

/* PrimeCell ID read */
int lcd_primcell_id_test(void)
{
	int status = TEST_PASS;
	uint32_t p_id = lcd_get_primecell_id();

	post_log("PrimeCell ID : 0x%X\n",p_id);
	return status;
}

#define REG_SET_VAL1 0xAAAAAAAA
#define REG_SET_VAL2 0x55555555
#define REG_SET_VAL3 0xFFFFFFFF

uint32_t read_write_reg_test(uint32_t a)
{
	uint32_t save = cpu_rd_single(a,4);/*Save initial val*/
	uint32_t status = TEST_PASS;

	post_log("address = 0x%X\n",a);
	post_log("value0 = 0x%X\n",cpu_rd_single(a,4));

	cpu_wr_single(a,REG_SET_VAL1,4);
	post_log("value1 = 0x%X\n",cpu_rd_single(a,4));
	if(REG_SET_VAL1 != cpu_rd_single(a,4))
		status = TEST_FAIL;

	cpu_wr_single(a,REG_SET_VAL2,4);
	post_log("value2 = 0x%X\n",cpu_rd_single(a,4));
	if(REG_SET_VAL2 != cpu_rd_single(a,4))
		status = TEST_FAIL;

	cpu_wr_single(a,REG_SET_VAL3,4);
	post_log("value3 = 0x%X\n",cpu_rd_single(a,4));
	if(REG_SET_VAL3 != cpu_rd_single(a,4))
		status = TEST_FAIL;

	cpu_wr_single(a,save,4);/*restore saved val*/
	return status;
}
#define read_write_reg_test(x) read_write_reg_test((uint32_t)x)
uint32_t read_reg_test(uint32_t a)
{
	uint32_t d = cpu_rd_single(a,4);/*Save initial val*/
	uint32_t status = TEST_PASS;
	post_log("address = 0x%X\n",a);
	post_log("value = 0x%X\n",d);
	cpu_wr_single(a,(~d),4);

	if(d != cpu_rd_single(a,4))
	{
		post_log("value = 0x%X\n",cpu_rd_single(a,4));
		status = TEST_FAIL;
	}
	return status;
}
#define read_reg_test(x) read_reg_test((uint32_t)x)

/* LCD read/Write, Read only register test */
int lcd_register_test(void)
{
	int status = TEST_PASS;
	volatile _lcd_reg *r;
	r = (_lcd_reg*)LCD_BASE_ADDRESS;

#if 1
	/* r/w register test */
	if(TEST_PASS !=	read_write_reg_test(&r->tim0))
	{
		post_log("error : Register TIM0 failed read-write test\n");
		status = TEST_FAIL;
	}
	if(TEST_PASS !=	read_write_reg_test(&r->tim1))
	{
		post_log("error : Register TIM1 failed read-write test\n");
		status = TEST_FAIL;
	}
//	if(TEST_PASS !=	read_write_reg_test(&r->tim2))
//	{
//		post_log("error : Register TIM2 failed read-write test\n");
//		status = TEST_FAIL;
//	}
	if(TEST_PASS !=	read_write_reg_test(&r->tim3))
	{
		post_log("error : Register TIM3 failed read-write test\n");
		status = TEST_FAIL;
	}
	if(TEST_PASS !=	read_write_reg_test(&r->ubase))
	{
		post_log("error : Register UBASE failed read-write test\n");
		status = TEST_FAIL;
	}
	if(TEST_PASS !=	read_write_reg_test(&r->lbase))
	{
		post_log("error : Register LBASE failed read-write test\n");
		status = TEST_FAIL;
	}
	if(TEST_PASS !=	read_write_reg_test(&r->cntl))
	{
		post_log("error : Register CNTL failed read-write test\n");
		status = TEST_FAIL;
	}
	if(TEST_PASS !=	read_write_reg_test(&r->imsc))
	{
		post_log("error : Register TIM0 failed read-write test\n");
		status = TEST_FAIL;
	}
	if(TEST_PASS !=	read_write_reg_test(&r->ucurr))
	{
		post_log("error : Register UCURR failed read-write test\n");
		status = TEST_FAIL;
	}
	if(TEST_PASS !=	read_write_reg_test(&r->lcurr))
	{
		post_log("error : Register LCURR failed read-write test\n");
		status = TEST_FAIL;
	}
	if(TEST_PASS !=	read_write_reg_test(&r->curctr))
	{
		post_log("error : Register CURCTR failed read-write test\n");
		status = TEST_FAIL;
	}
	if(TEST_PASS !=	read_write_reg_test(&r->curcfg))
	{
		post_log("error : Register CURCFG failed read-write test\n");
		status = TEST_FAIL;
	}
	if(TEST_PASS !=	read_write_reg_test(&r->curpalett0))
	{
		post_log("error : Register PALETTE0 failed read-write test\n");
		status = TEST_FAIL;
	}
	if(TEST_PASS !=	read_write_reg_test(&r->curpalett1))
	{
		post_log("error : Register PALETTE1 failed read-write test\n");
		status = TEST_FAIL;
	}
	if(TEST_PASS !=	read_write_reg_test(&r->curXY))
	{
		post_log("error : Register CURXY failed read-write test\n");
		status = TEST_FAIL;
	}
	if(TEST_PASS !=	read_write_reg_test(&r->curclip))
	{
		post_log("error : Register CURCLIP failed read-write test\n");
		status = TEST_FAIL;
	}
	if(TEST_PASS !=	read_write_reg_test(&r->curimsc))
	{
		post_log("error : Register CURIMSC failed read-write test\n");
		status = TEST_FAIL;
	}

	/* read only register test */
	if(TEST_PASS !=	read_reg_test(&r->ris))
	{
		post_log("error : Register RIS failed read-write test\n");
		status = TEST_FAIL;
	}
	if(TEST_PASS !=	read_reg_test(&r->mis))
	{
		post_log("error : Register MIS failed read-write test\n");
		status = TEST_FAIL;
	}
	if(TEST_PASS !=	read_reg_test(&r->icr))
	{
		post_log("error : Register ICR failed read-write test\n");
		status = TEST_FAIL;
	}
	if(TEST_PASS !=	read_reg_test(&r->curicr))
	{
		post_log("error : Register CURICR failed read-write test\n");
		status = TEST_FAIL;
	}
	if(TEST_PASS !=	read_reg_test(&r->curris))
	{
		post_log("error : Register CURRUS failed read-write test\n");
		status = TEST_FAIL;
	}
	if(TEST_PASS !=	read_reg_test(&r->curmis))
	{
		post_log("error : Register CURMIS failed read-write test\n");
		status = TEST_FAIL;
	}
	if(TEST_PASS !=	read_reg_test(&r->perID0))
	{
		post_log("error : Register perID0 failed read-write test\n");
		status = TEST_FAIL;
	}
	if(TEST_PASS !=	read_reg_test(&r->perID1))
	{
		post_log("error : Register perID1 failed read-write test\n");
		status = TEST_FAIL;
	}
	if(TEST_PASS !=	read_reg_test(&r->perID2))
	{
		post_log("error : Register perID2 failed read-write test\n");
		status = TEST_FAIL;
	}
	if(TEST_PASS !=	read_reg_test(&r->perID3))
	{
		post_log("error : Register perID3 failed read-write test\n");
		status = TEST_FAIL;
	}
	if(TEST_PASS !=	read_reg_test(&r->cellID0))
	{
		post_log("error : Register cellID0 failed read-write test\n");
		status = TEST_FAIL;
	}
	if(TEST_PASS !=	read_reg_test(&r->cellID1))
	{
		post_log("error : Register cellID0 failed read-write test\n");
		status = TEST_FAIL;
	}
	if(TEST_PASS !=	read_reg_test(&r->cellID2))
	{
		post_log("error : Register cellID2 failed read-write test\n");
		status = TEST_FAIL;
	}
	if(TEST_PASS !=	read_reg_test(&r->cellID3))
	{
		post_log("error : Register cellID3 failed read-write test\n");
		status = TEST_FAIL;
	}
#endif
	post_log("--Test %s--\n",status == TEST_FAIL ? "FAILED":"PASSED");
	return status;
}
/* Register dump */
int lcd_reg_dump(void)
{
	volatile _lcd_reg *r = (_lcd_reg*)LCD_BASE_ADDRESS;
	post_log("tim0 = 0x%X\n",r->tim0);
	post_log("tim1 = 0x%X\n",r->tim1);
	post_log("tim2 = 0x%X\n",r->tim2);
	post_log("tim3 = 0x%X\n",r->tim3);
	post_log("ubase = 0x%X\n",r->ubase);
	post_log("lbase = 0x%X\n",r->lbase);
	post_log("cntl = 0x%X\n",r->cntl);
	post_log("imsc = 0x%X\n",r->imsc);
	post_log("ris = 0x%X\n",r->ris);
	post_log("mis = 0x%X\n",r->mis);
	post_log("icr = 0x%X\n",r->icr);
	post_log("ucurr = 0x%X\n",r->ucurr);
	post_log("lcurr = 0x%X\n",r->lcurr);
	post_log("curctr = 0x%X\n",r->curctr);
	post_log("curcfg = 0x%X\n",r->curcfg);
	post_log("curpalett0 = 0x%X\n",r->curpalett0);
	post_log("curpalett1 = 0x%X\n",r->curpalett1);
	post_log("curXY = 0x%X\n",r->curXY);
	post_log("curclip = 0x%X\n",r->curclip);
	post_log("curimsc = 0x%X\n",r->curimsc);
	post_log("curicr = 0x%X\n",r->curicr);
	post_log("curris = 0x%X\n",r->curris);
	post_log("curmis = 0x%X\n",r->curmis);
	post_log("perID0 = 0x%X\n",r->perID0);
	post_log("perID1 = 0x%X\n",r->perID1);
	post_log("perID2 = 0x%X\n",r->perID2);
	post_log("perID3 = 0x%X\n",r->perID3);
	post_log("cellID0 = 0x%X\n",r->cellID0);
	post_log("cellID1 = 0x%X\n",r->cellID1);
	post_log("cellID2 = 0x%X\n",r->cellID2);
	post_log("cellID3 = 0x%X\n",r->cellID3);
return TEST_PASS;
}

/* Reset value of all registers */
int lcd_reset_value_test(void)
{
	int status = TEST_PASS;
	volatile _lcd_reg *r = (_lcd_reg*)LCD_BASE_ADDRESS;
	lcd_reg_dump();

	lcd_reset();

	if(r->tim0 != LCD_DEF_TIM0)
	{
		post_log("error : TIM0 = 0x%X , default = 0x%X\n",r->tim0,LCD_DEF_TIM0);
		status = TEST_FAIL;
	}

	if(r->tim1 != LCD_DEF_TIM1)
	{
		post_log("error : TIM1 = 0x%X , default = 0x%X\n",r->tim1,LCD_DEF_TIM1);
		status = TEST_FAIL;
	}

	if(r->tim2 != LCD_DEF_TIM2)
	{
		post_log("error : TIM2 = 0x%X , default = 0x%X\n",r->tim2,LCD_DEF_TIM2);
		status = TEST_FAIL;
	}

	if(r->tim3 != LCD_DEF_TIM3)
	{
		post_log("error : TIM3 = 0x%X , default = 0x%X\n",r->tim3,LCD_DEF_TIM3);
		status = TEST_FAIL;
	}

	if(r->ubase != LCD_DEF_UBASE)
	{
		post_log("error : UBASE = 0x%X , default = 0x%X\n",r->ubase,LCD_DEF_UBASE);
		status = TEST_FAIL;
	}

	if(r->lbase != LCD_DEF_LBASE)
	{
		post_log("error : LBASE = 0x%X , default = 0x%X\n",r->lbase,LCD_DEF_LBASE);
		status = TEST_FAIL;
	}

	if(r->cntl != LCD_DEF_CNTL)
	{
		post_log("error : CNTL = 0x%X , default = 0x%X\n",r->cntl,LCD_DEF_CNTL);
		status = TEST_FAIL;
	}

	if(r->imsc != LCD_DEF_IMSC)
	{
		post_log("error : IMSC = 0x%X , default = 0x%X\n",r->imsc,LCD_DEF_IMSC);
		status = TEST_FAIL;
	}

	if(r->ris != LCD_DEF_RIS)
	{
		post_log("error : RIS = 0x%X , default = 0x%X\n",r->ris,LCD_DEF_RIS);
		status = TEST_FAIL;
	}

	if(r->mis != LCD_DEF_MIS)
	{
		post_log("error : MIS = 0x%X , default = 0x%X\n",r->mis,LCD_DEF_MIS);
		status = TEST_FAIL;
	}

	if(r->icr != LCD_DEF_ICR)
	{
		post_log("error : ICR = 0x%X , default = 0x%X\n",r->icr,LCD_DEF_ICR);
		status = TEST_FAIL;
	}

	if(r->ucurr != LCD_DEF_UCURR)
	{
		post_log("error : UCURR = 0x%X , default = 0x%X\n",r->ucurr,LCD_DEF_UCURR);
		status = TEST_FAIL;
	}

	if(r->lcurr != LCD_DEF_LCURR)
	{
		post_log("error : LCURR = 0x%X , default = 0x%X\n",r->lcurr,LCD_DEF_LCURR);
		status = TEST_FAIL;
	}

	if(r->curctr != LCD_DEF_CURCTR)
	{
		post_log("error : CURCTR = 0x%X , default = 0x%X\n",r->curctr,LCD_DEF_CURCTR);
		status = TEST_FAIL;
	}

	if(r->curcfg != LCD_DEF_CURCFG)
	{
		post_log("error : CURCFG = 0x%X , default = 0x%X\n",r->curcfg,LCD_DEF_CURCFG);
		status = TEST_FAIL;
	}

	if(r->curpalett0 != LCD_DEF_CURPALETT0)
	{
		post_log("error : CURPALETT0 = 0x%X , default = 0x%X\n",r->curpalett0,LCD_DEF_CURPALETT0);
		status = TEST_FAIL;
	}

	if(r->curpalett1 != LCD_DEF_CURPALETT1)
	{
		post_log("error : CURPALETT1 = 0x%X , default = 0x%X\n",r->curpalett1,LCD_DEF_CURPALETT1);
		status = TEST_FAIL;
	}

	if(r->curXY != LCD_DEF_CURXY)
	{
		post_log("error : CURXY = 0x%X , default = 0x%X\n",r->curXY,LCD_DEF_CURXY);
		status = TEST_FAIL;
	}

	if(r->curclip != LCD_DEF_CURCLIP)
	{
		post_log("error : CURCLIP = 0x%X , default = 0x%X\n",r->curclip,LCD_DEF_CURCLIP);
		status = TEST_FAIL;
	}

	if(r->curimsc != LCD_DEF_CURIMSC)
	{
		post_log("error : CURIMSC = 0x%X , default = 0x%X\n",r->curimsc,LCD_DEF_CURIMSC);
		status = TEST_FAIL;
	}

	if(r->curicr != LCD_DEF_CURICR)
	{
		post_log("error : CURICR = 0x%X , default = 0x%X\n",r->curicr,LCD_DEF_CURICR);
		status = TEST_FAIL;
	}

	if(r->curris != LCD_DEF_CURRIS)
	{
		post_log("error : CURRIS = 0x%X , default = 0x%X\n",r->curris,LCD_DEF_CURRIS);
		status = TEST_FAIL;
	}

	if(r->curmis != LCD_DEF_CURMIS)
	{
		post_log("error : CURMIS = 0x%X , default = 0x%X\n",r->curmis,LCD_DEF_CURMIS);
		status = TEST_FAIL;
	}

	if(r->perID0 != LCD_DEF_PERID0)
	{
		post_log("error : PERID0 = 0x%X , default = 0x%X\n",r->perID0,LCD_DEF_PERID0);
		status = TEST_FAIL;
	}

	if(r->perID1 != LCD_DEF_PERID1)
	{
		post_log("error : PERID1 = 0x%X , default = 0x%X\n",r->perID1,LCD_DEF_PERID1);
		status = TEST_FAIL;
	}

	if(r->perID2 != LCD_DEF_PERID2)
	{
		post_log("error : TIM0 = 0x%X , default = 0x%X\n",r->perID2,LCD_DEF_PERID2);
		status = TEST_FAIL;
	}

	if(r->perID3 != LCD_DEF_PERID3)
	{
		post_log("error : PERID3 = 0x%X , default = 0x%X\n",r->perID3,LCD_DEF_PERID3);
		status = TEST_FAIL;
	}

	if(r->cellID0 != LCD_DEF_CELLID0)
	{
		post_log("error : CELLID0 = 0x%X , default = 0x%X\n",r->cellID0,LCD_DEF_CELLID0);
		status = TEST_FAIL;
	}

	if(r->cellID1 != LCD_DEF_CELLID1)
	{
		post_log("error : CELLID1 = 0x%X , default = 0x%X\n",r->cellID1,LCD_DEF_CELLID1);
		status = TEST_FAIL;
	}

	if(r->cellID2 != LCD_DEF_CELLID2)
	{
		post_log("error : CELLID2 = 0x%X , default = 0x%X\n",r->cellID2,LCD_DEF_CELLID2);
		status = TEST_FAIL;
	}

	if(r->cellID3 != LCD_DEF_CELLID3)
	{
		post_log("error : CELLID3 = 0x%X , default = 0x%X\n",r->cellID3,LCD_DEF_CELLID3);
		status = TEST_FAIL;
	}
	lcd_reg_dump();
	post_log("--Test %s--\n",status == TEST_FAIL ? "FAILED":"PASSED");
	return status;
}

/* Palette RAM read/write test */
int lcd_palette_RAM_test(void)
{
	int status = TEST_PASS;
	uint32_t l = LCD_PALETTE_RAM_SIZE/2, v = 0, rv1 = 0 , rv2 = 0, check = 0;
	uint32_t loop_index = 0;
	volatile _lcd_reg *r = (_lcd_reg*)LCD_BASE_ADDRESS;

/*Palette RAM 32 x 128 read write test*/
	post_log("Palette RAM 32 x 128 read write\n");
	for(loop_index = 0 ; loop_index < l ; loop_index+=1)
	{
		//post_log("A = 0x%X\n",&r->palette[loop_index]);
		v = cpu_rd_single((uint32_t)&r->palette[loop_index],4);
		//post_log("V0 = 0x%X\n",v);
		cpu_wr_single((uint32_t)&r->palette[loop_index],(~v),4);
		check = (~v) & 0xFFFFFFFF;
		rv1 = cpu_rd_single((uint32_t)&r->palette[loop_index],4);
		//post_log("V1 = 0x%X\n",rv1);

		if(check != rv1)
		{
			post_log("error : palette RAM location 0x%X read-write failed read -> 0x%X , expected -> 0x%X\n",(uint32_t)(&(r->palette) + (loop_index * 4)), rv1,check );
			status = TEST_FAIL;
			break;
		}
		cpu_wr_single((uint32_t)&r->palette[loop_index],(~rv1),4);
		check = (~rv1) & 0xFFFFFFFF;
		rv2 = cpu_rd_single((uint32_t)&r->palette[loop_index],4);
		//post_log("V2 = 0x%X\n",rv2);
		if(check != rv2)
		{
			post_log("error : palette RAM location 0x%X read-write failed read -> 0x%X , expected -> 0x%X\n",(uint32_t)(&(r->palette) + (loop_index * 4)), rv2, check );
			status = TEST_FAIL;
			break;
		}
	}
	post_log("--Test %s--\n",status == TEST_FAIL ? "FAILED":"PASSED");
	return status;
}

/* Cursor image RAM read/write test */
int lcd_cur_image_RAM_test(void)
{
	int status = TEST_PASS;
	uint32_t l = LCD_CUR_IMG_RAM_SIZE, v = 0, rv1 = 0 , rv2 = 0;
	uint32_t loop_index = 0;
	volatile _lcd_reg *r;
	r = (_lcd_reg*)LCD_BASE_ADDRESS;

	/*Cursor image  RAM 32 x 256 read write test*/
	post_log("Cursor image  RAM 32 x 256 read write\n");
	for(loop_index = 0 ; loop_index < l ; loop_index++)
	{
		//post_log("A = 0x%X\n",&r->palette[loop_index]);
		v = cpu_rd_single((uint32_t)&r->curimg[loop_index],4);
		//post_log("V0 = 0x%X\n",v);
		cpu_wr_single((uint32_t)&r->curimg[loop_index],(~v),4);
		rv1 = cpu_rd_single((uint32_t)&r->curimg[loop_index],4);
		//post_log("V1 = 0x%X\n",rv1);
		if(v != ~(rv1))
		{
			post_log("error : cur_image RAM location 0x%X read-write failed read -> 0x%X , expected -> 0x%X\n",(uint32_t)(r->curimg + (loop_index * 4)), rv1,~v );
			status = TEST_FAIL;
			break;
		}
		cpu_wr_single((uint32_t)&r->curimg[loop_index],(~rv1),4);
		rv2 = cpu_rd_single((uint32_t)&r->curimg[loop_index],4);
		//post_log("V2 = 0x%X\n",rv2);
		if(v != rv2)
		{
			post_log("error : cur_image RAM location 0x%X read-write failed read -> 0x%X , expected -> 0x%X\n",(uint32_t)(r->curimg + (loop_index * 4)), rv2, v );
			status = TEST_FAIL;
			break;
		}
	}
	post_log("--Test %s--\n",status == TEST_FAIL ? "FAILED":"PASSED");
	return status;
}
void load_image(uint8_t *a, uint8_t *d, uint32_t pixels , uint32_t bpp)
{
   uint32_t i = 0;
   uint32_t tot_bytes = ( pixels * bpp ) / 8;
   post_log("Total image data bytes : 0x%X\n",tot_bytes);

   for(i = 0 ; i < tot_bytes ; i++)
     {
          *a = *d;
          a++;
          d++;
     }
}

void fill_image_palette_data(uint32_t length,uint32_t height, uint32_t bar_w, uint32_t bpp, uint8_t *image)
{

	uint32_t max_val = (1 << bpp) ,i = 0,j = 0, k = 0,l = 0, h = 0;
	uint8_t d = 0;
	uint32_t bpb = 8/bpp;

	for(h = 0 ; h < height ; h++)
	{
		for(l = 0 ; l < length ; l++)
		{
			d = j;
			for(i = 0 ; i < bpb; i++)
				d = d | (d << (bpp * i));

			image[l + (h * length)] = d;
			for(k = 1 ; k < bar_w ; k++ )
				image[++l + (h * length)] = d;

			j++;
			if(j == max_val)
				j = 0;
		}
	}
}


void fill_image_data(uint32_t length,uint32_t height, uint32_t bar_w, uint32_t bpp, uint8_t *image)
{
	
	uint32_t max_val = (1 << bpp) ,i = 0,j = 0, k = 0,l = 0, h = 0;
	uint8_t d = 0;
	uint32_t bpb = 8/bpp;

	for(h = 0 ; h < height ; h++)
	{
		for(l = 0 ; l < length ; l++)
		{
			d = j;
			for(i = 0 ; i < bpb; i++)
				d = d | (d << (bpp * i));

			image[l + (h * length)] = d;
			for(k = 1 ; k < bar_w ; k++ )
				image[++l + (h * length)] = d;

			j++;
			if(j == max_val)
				j = 0;
		}
	}
}


int lcd_1bpp_image_test(void)
{
	int result = TEST_PASS;
	void *lcd_base = (void *)0x80000000;

	uint32_t bpp 					= BPP1;
	uint16_t pal_array[(1 << BPP1)] = {0x801F, 0xFC00}; /*Red , Blue*/

	uint32_t tot_bytes_perline 		= ( PPL * bpp ) / 8 ;
	//uint8_t image[tot_bytes_perline * LPP];
	uint32_t col_bar_length = tot_bytes_perline / (1 << bpp) ;

    lcd_reset();
    local_config.cntl 	= (CNTL | LCD_CNTL_BPP1);
    fill_image_palette_data(tot_bytes_perline,LPP,col_bar_length,bpp, lcd_base);

	//load_image((uint8_t*)lcd_base, &image[0], (LPP * PPL) , bpp);

	if(LCD_FAIL == lcd_palette_programming(&pal_array[0], (1 << bpp)))
	{
			post_log("error : Paltette programing failed\n");
			result = TEST_FAIL;
	}

	lcd_intr_clear(LCD_INTR_MBERROR | LCD_INTR_VcompIM | LCD_INTR_LNBUIM | LCD_INTR_FUFIM);
	lcd_intr_enable(0x08);
    lcd_set_config(&local_config);
	lcd_ctrl_init(lcd_base);

	return result;
}


int lcd_2bpp_image_test(void)
{
	int result = TEST_PASS;
	//void *lcd_base = (void *)(gd->fb_base);
	void *lcd_base = (void *)0x80000000;

	uint32_t bpp 					= BPP2;
	uint16_t pal_array[(1 << BPP2)] = {0x801F, 0x83E0, 0xFC00, 0xFFFF}; /*red, Green, Blue, Black*/

	uint32_t tot_bytes_perline 		= ( PPL * bpp ) / 8 ;
	//uint8_t image[tot_bytes_perline * LPP];
	uint32_t col_bar_length = tot_bytes_perline / (1 << bpp) ;

    lcd_reset();
    local_config.cntl 	= (CNTL | LCD_CNTL_BPP2);
    fill_image_palette_data(tot_bytes_perline,LPP,col_bar_length,bpp, lcd_base);

    post_log("Total Bytes per line : %d\n",tot_bytes_perline);

	//load_image((uint8_t*)lcd_base, &image[0], (LPP * PPL) , bpp);

	if(LCD_FAIL == lcd_palette_programming(&pal_array[0], (1 << bpp)))
	{
			post_log("error : Paltette programing failed\n");
			result = TEST_FAIL;
	}
	lcd_intr_clear(LCD_INTR_MBERROR | LCD_INTR_VcompIM | LCD_INTR_LNBUIM | LCD_INTR_FUFIM);
	lcd_intr_enable(0x08);
    lcd_set_config(&local_config);
	lcd_ctrl_init(lcd_base);

	return result;
}

int lcd_4bpp_image_test(void)
{
	int result = TEST_PASS;
	//void *lcd_base = (void *)(gd->fb_base);
	void *lcd_base = (void *)0x80000000;


	uint32_t bpp 					= BPP4;
	uint16_t pal_array[(1 << BPP4)] = {0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000, 0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000}; /*red, Green, Blue, Black*/

	uint32_t tot_bytes_perline 		= ( PPL * bpp ) / 8 ;
	//uint8_t image[tot_bytes_perline * LPP];
	uint32_t col_bar_length = tot_bytes_perline / (1 << bpp) ;

    lcd_reset();
    local_config.cntl 	= (CNTL | LCD_CNTL_BPP4);
    fill_image_palette_data(tot_bytes_perline,LPP,col_bar_length,bpp, lcd_base);

    post_log("Total Bytes per line : %d\n",tot_bytes_perline);

	//load_image((uint8_t*)lcd_base, &image[0], (LPP * PPL) , bpp);

	if(LCD_FAIL == lcd_palette_programming(&pal_array[0], (1 << bpp)))
	{
			post_log("error : Paltette programing failed\n");
			result = TEST_FAIL;
	}
	lcd_intr_clear(LCD_INTR_MBERROR | LCD_INTR_VcompIM | LCD_INTR_LNBUIM | LCD_INTR_FUFIM);
	lcd_intr_enable(0x08);
    lcd_set_config(&local_config);
	lcd_ctrl_init(lcd_base);

	return result;

}
int lcd_8bpp_image_test(void)
{
	int result = TEST_PASS;
	//void *lcd_base = (void *)(gd->fb_base);
	void *lcd_base = (void *)0x80000000;
	

	uint32_t bpp 					= BPP8;
	uint16_t pal_array[(1 << BPP8)] = {
			0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000, 0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000,
			0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000, 0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000,
			0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000, 0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000,
			0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000, 0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000,
			0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000, 0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000,
			0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000, 0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000,
			0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000, 0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000,
			0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000, 0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000,
			0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000, 0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000,
			0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000, 0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000,
			0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000, 0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000,
			0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000, 0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000,
			0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000, 0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000,
			0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000, 0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000,
			0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000, 0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000,
			0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000, 0x801F, 0x83FF, 0x83E0, 0xFFE0, 0xFC00, 0xFC1F, 0xFFFF, 0x8000
	};

	uint32_t tot_bytes_perline 		= ( PPL * bpp ) / 8 ;
	//uint8_t image[tot_bytes_perline * LPP];
	uint32_t col_bar_length = tot_bytes_perline / (1 << BPP4) ;

    lcd_reset();
    local_config.cntl 	= (CNTL | LCD_CNTL_BPP8);
    fill_image_palette_data(tot_bytes_perline,LPP,col_bar_length,bpp, lcd_base);

    post_log("Total Bytes per line : %d\n",tot_bytes_perline);

	//load_image((uint8_t*)lcd_base, &image[0], (LPP * PPL) , bpp);

	if(LCD_FAIL == lcd_palette_programming(&pal_array[0], (1 << bpp)))
	{
			post_log("error : Paltette programing failed\n");
			result = TEST_FAIL;
	}
	lcd_intr_clear(LCD_INTR_MBERROR | LCD_INTR_VcompIM | LCD_INTR_LNBUIM | LCD_INTR_FUFIM);
	lcd_intr_enable(0x08);
    lcd_set_config(&local_config);
	lcd_ctrl_init(lcd_base);

	return result;
}
void lcd_load_true_col(uint16_t *f_buff, uint16_t *image, uint32_t width , uint32_t length, uint32_t barwidth, uint32_t bpp)
{
	uint16_t *fb = (uint16_t *)f_buff;
	uint16_t *im = (uint16_t *)image;
	int i , j , col_i = 0;

	uint32_t bar_w = 0;
	if(fb == NULL)
		post_log("error : image format not supported\n");

	for(i = 0 ; i < width ; i++)
	{
		bar_w = barwidth;
		col_i = 0;
		for(j = 0 ; j < length ; j++)
		{
			if(bpp == LCD_CNTL_BPP16_565)
			{
				*fb = im[col_i];
				fb++;
			}

			if(bpp == LCD_CNTL_BPP24)
			{
				*fb = im[col_i];
				fb++;
				*fb = im[col_i + 1];
				fb++;
			}
			if((j+1) == bar_w )
			{
				col_i++;
				if(bpp == LCD_CNTL_BPP24)
					col_i++;
				bar_w += barwidth;
			}
		}
	}
}

void lcd_load_true_col_24bpp(uint32_t *f_buff, uint32_t *image, uint32_t width , uint32_t length, uint32_t barwidth, uint32_t bpp)
{
	uint32_t *fb = (uint32_t *)f_buff;
	uint32_t *im = (uint32_t *)image;
	int i , j , col_i = 0;

	uint32_t bar_w = 0;
	if(fb == NULL)
		post_log("error : image format not supported\n");

	for(i = 0 ; i < width ; i++)
	{
		bar_w = barwidth;
		col_i = 0;
		for(j = 0 ; j < length ; j++)
		{

			*fb = im[col_i];
			fb++;

			if((j+1) == bar_w )
			{
				col_i++;
				bar_w += barwidth;
			}
		}
	}
}

int lcd_16bpp_image_test(void)
{
	int result = TEST_PASS;
	//void *lcd_base = (void *)(gd->fb_base);
	void *lcd_base = (void *)0x80000000;

	uint32_t bar_w = PPL / 8;
	/*16 BPP, 656*/
	uint16_t smpl_img_bar[8] = {0x1F, 0x7FF, 0x7E0, 0xFFE0, 0xF800,  0xF81F , 0xFFFF , 0x0};

    lcd_reset();

    local_config.cntl 	= (CNTL | LCD_CNTL_BPP16_565);
	lcd_load_true_col(lcd_base, &smpl_img_bar[0], PPL,LPP,bar_w,LCD_CNTL_BPP16_565);
	lcd_intr_clear(LCD_INTR_MBERROR | LCD_INTR_VcompIM | LCD_INTR_LNBUIM | LCD_INTR_FUFIM);
	lcd_intr_enable(LCD_INTR_VcompIM);
    lcd_set_config(&local_config);

    lcd_ctrl_init(lcd_base);
    post_log("--Test %s--\n",result == TEST_FAIL ? "FAILED":"PASSED");
    return result;
}


int lcd_24bpp_image_test(void)
{
	int result = TEST_PASS;
	//void *lcd_base = (void *)(gd->fb_base);
	void *lcd_base = (void *)0x80000000;
	//uint32_t image[64 * 64];
	/*16 BPP, 656*/
	uint32_t smpl_img_bar[8] = {0xFF, 0xFFFF, 0xFF00, 0xFFFF00, 0xFF0000,  0xFF00FF , 0xFFFFFF , 0x0};
	uint32_t bar_w = PPL / 8;

    lcd_reset();

    post_log("Entering the LCD Configuration \n");
    local_config.cntl 	= (CNTL | LCD_CNTL_BPP24);
    lcd_load_true_col_24bpp(lcd_base, &smpl_img_bar[0], PPL,LPP,bar_w,LCD_CNTL_BPP24);
    post_log("Entering the LCD Configuration 1 \n");
	lcd_intr_clear(LCD_INTR_MBERROR | LCD_INTR_VcompIM | LCD_INTR_LNBUIM | LCD_INTR_FUFIM);
	post_log("Entering the LCD Configuration 2 \n");
	lcd_intr_enable(LCD_INTR_VcompIM);
	post_log("Entering the LCD Configuration 3 \n");
    lcd_set_config(&local_config);
    post_log("Entering the LCD Configuration 4 \n");
    lcd_ctrl_init(lcd_base);
    post_log("Entering the LCD Configuration 5 \n");

//    int i = 0;
//    for(i = 0 ; i < 10000 ; i++);

    post_log("--Test %s--\n",result == TEST_FAIL ? "FAILED":"PASSED");
    return result;
}

int lcd_display_fb_address(void)
{
	void *lcd_base = (void *)(gd->fb_base);
	post_log("Frame Buffer Address : 0x%X\n",LCD_FRAME_BUFFER_ADDRESS);
	post_log("Frame Buffer Address : 0x%X\n",(uint32_t)lcd_base);
	return TEST_PASS;
}

int lcd_broadcom_logo(void)
{

	return 0;
}


/******* Internal test function start ********/
int LCD_post_test(int flags)
{
#if 1



    int status = TEST_PASS;

	uint32_t reg_data = cpu_rd_single(ASIU_TOP_LCD_AXI_SB_CTRL,4);
    reg_data = reg_data & (0xfeffffff);
    cpu_wr_single(ASIU_TOP_LCD_AXI_SB_CTRL,reg_data,4);

    lcd_board_specific_init();


    if(TEST_FAIL == lcd_1bpp_image_test())
	      status = TEST_FAIL;
    return status;

#else

	uint32_t test_no;
    int loop_count = 0, total_tests = 0 , total_reg_tests = 0;
    int status = 0,autorun_status = 0,pass = 0,fail = 0,skip = 0;



	uint32_t reg_data = cpu_rd_single(ASIU_TOP_LCD_AXI_SB_CTRL,4);
    reg_data = reg_data & (0xfeffffff);
    cpu_wr_single(ASIU_TOP_LCD_AXI_SB_CTRL,reg_data,4);

    lcd_board_specific_init();

    FUNC_INFO function[]	=	{
	     	 	 	 	 	 	 ADD_TEST( lcd_prih_id_test       ),
	     	 	 	 	 	 	 ADD_TEST( lcd_primcell_id_test   ),
	     	 	 	 	 	 	 ADD_TEST( lcd_display_fb_address ),
	     	 	 	 	 	 	 ADD_TEST( lcd_register_test      ),
	     	 	 	 	 	 	 ADD_TEST( lcd_reset_value_test   ),
	     	 	 	 	 	 	 ADD_TEST( lcd_palette_RAM_test   ),
	     	 	 	 	 	 	 ADD_TEST( lcd_cur_image_RAM_test ),
	     	 	 	 	 	 	 ADD_TEST( lcd_1bpp_image_test 	  ),
	     	 	 	 	 	 	 ADD_TEST( lcd_2bpp_image_test 	  ),
	     	 	 	 	 	 	 ADD_TEST( lcd_4bpp_image_test 	  ),
	     	 	 	 	 	 	 ADD_TEST( lcd_8bpp_image_test 	  ),
	     	 	 	 	 	 	 ADD_TEST( lcd_16bpp_image_test	  ),
	     	 	 	 	 	 	 ADD_TEST( lcd_24bpp_image_test	  )

								};

    FUNC_INFO regression_func[]	=	{
								 ADD_TEST( lcd_prih_id_test       ),
								 ADD_TEST( lcd_primcell_id_test   ),
								 //ADD_TEST( lcd_register_test      ),
								 ADD_TEST( lcd_reset_value_test   )
								 //ADD_TEST( lcd_1bpp_image_test 	  )

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
                    	  	  case TEST_FAIL:    lcd_clk_gate();
                    	      lcd_asiu_mipi_dsi_gen_pll_pwr_on_config();
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
