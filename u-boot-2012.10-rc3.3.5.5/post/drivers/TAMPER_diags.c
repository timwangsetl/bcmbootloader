/*
 * $Copyright Open Broadcom Corporation$
 */


#include <common.h>
#include <post.h>

#if CONFIG_POST & CONFIG_SYS_POST_TAMPER
#if defined(CONFIG_CYGNUS)
#include "asm/arch/socregs.h"
#include "asm/arch/reg_utils.h"
#define rd(x) reg32_read((volatile uint32_t *)x)
#define wr(x, v) reg32_write((volatile uint32_t *)x, v)


#define BBL_ISO_DISABLE_FLAG  (~((1 << CRMU_ISO_CELL_CONTROL__CRMU_ISO_PDBBL) | (1 << CRMU_ISO_CELL_CONTROL__CRMU_ISO_PDBBL_TAMPER)))
#define RST_CMD (1 << SPRU_BBL_CMD__IND_SOFT_RST_N)
#define WR_CMD  (1 << SPRU_BBL_CMD__IND_WR)
#define RD_CMD  (1 << SPRU_BBL_CMD__IND_RD)

static int has_error = 0;

static int bbl_wr(u32 addr, u32 v)
{
	u32 cmd;
	
	wr(SPRU_BBL_WDATA, v);	
    cmd = (addr & (0xFFFFFFFF >> (32 - SPRU_BBL_CMD__BBL_ADDR_WIDTH))) | WR_CMD | RST_CMD;
    wr(SPRU_BBL_CMD, cmd);
	udelay(1000*2);	
    if (rd(SPRU_BBL_STATUS) & (1 << SPRU_BBL_STATUS__ACC_DONE))
    	return 0;
    
    printf("fail to write bbl cmd for addr 0x%x\n", addr);
    has_error = 1;
    return -1;   	 
}

static u32 bbl_rd(u32 addr)
{   
	u32 cmd;
    cmd = (addr & (0xFFFFFFFF >> (32 - SPRU_BBL_CMD__BBL_ADDR_WIDTH))) | RD_CMD | RST_CMD;
    wr(SPRU_BBL_CMD, cmd);
    udelay(1000*2);	
    if (rd(SPRU_BBL_STATUS) & (1 << SPRU_BBL_STATUS__ACC_DONE)) {
        return 	rd(SPRU_BBL_RDATA);   	
    }   
    printf("fail to write bbl cmd for addr 0x%x \n", addr);
    has_error = 1;
    return 0xFFFFFFFF;
}
static void bbl_init(void)
{
    u32 v;
    wr(SPRU_BBL_CMD, 0);
    udelay(10000);
    wr(SPRU_BBL_CMD, RST_CMD);
    v = rd(CRMU_ISO_CELL_CONTROL);
    v &= BBL_ISO_DISABLE_FLAG;
    wr(CRMU_ISO_CELL_CONTROL, v);
    wr(CRMU_BBL_AUTH_CODE, 0x12345678);
    wr(CRMU_BBL_AUTH_CHECK, 0x12345678);
}

static void pwm_init(void)
{
    wr(ASIU_PWM_CONTROL, 0x3FFF0000);
    wr(ASIU_PWM1_PERIOD_COUNT, 0x2710);
    wr(ASIU_PWM2_PERIOD_COUNT, 0x2710);
    wr(ASIU_PWM1_DUTY_CYCLE_HIGH, 0x1388);
    wr(ASIU_PWM2_DUTY_CYCLE_HIGH, 0x1388);
    wr(ASIU_PWM_CONTROL, 0x3FFF0006); //Enable PWM1&PWM2  
}

static void gpio_aon_init(void)
{
	wr(GP_OUT_EN, 0x3F);
}

static void write_to_sec1280(void)
{ 
	int i;
	for (i = 0; i < 10; i++) {
	  bbl_wr(0x200 + i*4, i + 1);
	  udelay(2000);
	  //printf("sec1280 addr:0x%02x v:0x%08x\n",i, bbl_rd(0x200 + i*4));
	}
}
static int check_sec1280(void)
{
	int i;
	int rv = 0;
	u32 v;
	for (i = 0; i < 10; i++) {
		v = bbl_rd(0x200 + i*4);
		printf("sec1280 addr:0x%02x v:0x%08x\n", i, v);
		if (v == (i + 1))
			rv = -1;
	}
	return rv;
}
#define BBL_MESH_CONFIG__lfsr_avd_stuck 3



static void emesh_en(void)
{
	u32 v;
	bbl_init();
    
    v = bbl_rd(BBL_MESH_CONFIG);
    v |= (1 << BBL_MESH_CONFIG__imesh_mfm_en) \
      | (1 << BBL_MESH_CONFIG__en_mesh_dyn)   \
      | (1 << BBL_MESH_CONFIG__en_intmesh_fc) \
      | (1 << BBL_MESH_CONFIG__lfsr_avd_stuck); 
    bbl_wr(BBL_MESH_CONFIG, v);

    v = bbl_rd(BBL_TAMPER_SRC_STAT);
    bbl_wr(BBL_TAMPER_SRC_CLEAR, v);
    
    bbl_wr(BBL_TAMPER_SRC_CLEAR, 0);

    v = bbl_rd(BBL_INTERRUPT_stat);
    bbl_wr(BBL_INTERRUPT_clr, v);

    bbl_wr(BBL_EN_TAMPERIN, 0x001); //Set P0/N0 as input pairs
    bbl_wr(BBL_TAMPER_SRC_ENABLE, 0x3FDFE|(1<<30)|(7<<18));  
    
    //bbl emesh enable
    v = 0xFF;
    v |= (0xFF << 8);
    v |=(1 << BBL_EMESH_CONFIG__bbl_mesh_clr)  \
      | (1 << BBL_EMESH_CONFIG__en_extmesh_dyn) \
      | (1 << BBL_EMESH_CONFIG__en_extmesh_fc) ;
    bbl_wr(BBL_EMESH_CONFIG, v);

    v = bbl_rd(BBL_EMESH_CONFIG_1);
    v |= 0x8;
    bbl_wr(BBL_EMESH_CONFIG_1, v);
    
    v = 0xFFFFFF;
    //printf ("PHASE = 0x%08x\n", v);
    bbl_wr(BBL_EMESH_PHASE_SEL0, v);    
    bbl_wr(BBL_EMESH_PHASE_SEL1, 0);
}

static int emesh_test (void)
{
	u32 v;
	int sn;
	int rv;
	int i;
	
	has_error = 0;
	printf("\n");
    
    emesh_en();   
    ///////////////////////
    i = 0;
    do {
	    v = bbl_rd(BBL_TAMPER_SRC_STAT);
	    bbl_wr(BBL_TAMPER_SRC_CLEAR, v);
	    udelay(1000);
	    i++;
	    if (i > 2000) break;
    }while ((v& 0xFFFFF) != 0);
    bbl_wr(BBL_TAMPER_SRC_CLEAR, 0);
    //printf("delayed %d ms\n", i);
    
    v = bbl_rd(BBL_INTERRUPT_stat);
    bbl_wr(BBL_INTERRUPT_clr, v);
    mdelay(10);
    
    v = bbl_rd(BBL_EMESH_CONFIG_1);
    v &= ~0x8;
    bbl_wr(BBL_EMESH_CONFIG_1, v);
    mdelay(1000);

    write_to_sec1280(); 
    //mdelay(1000);
   (void)check_sec1280();    
    
    //////////////////////////// 
    emesh_en();
    
    //set pwm_tamp switch by aon io;
    gpio_aon_init();
    sn = 13;
    wr(GP_DATA_OUT, (sn-1) | 0x30);  //aon_gpio4/5 = 0x11
    //enable pwm
    pwm_init();
    
    mdelay(2000);
    
    printf("\nTo check status\n");
    v = bbl_rd(BBL_TAMPER_SRC_STAT);
    printf ("tamper src stat (0x%08x)\n", v);
    v = bbl_rd(BBL_INTERRUPT_stat);
    printf( "intr stat (0x%08x)\n", v);
    
    if ((0 == check_sec1280()) && (has_error == 0)) {
    	rv = 0;
    } else {
        rv = -1;	
    }
    
    //stop

    v = bbl_rd(BBL_EMESH_CONFIG_1);
    v &= ~0x8;
    bbl_wr(BBL_EMESH_CONFIG_1, v);
    
    wr(GP_OUT_EN, 0);
    wr(ASIU_PWM_CONTROL, 0x3FFF0000);
       
    return rv;
}


int TAMPER_post_test (int flags)
{
    return emesh_test();	
}

#endif 
#endif /* CONFIG_POST & CONFIG_SYS_POST_TAMPER */
