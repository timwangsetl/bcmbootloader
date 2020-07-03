/*
 * $Copyright Open Broadcom Corporation$ 
 */
#include <common.h>
#include <asm/io.h>
#include <asm/mach-types.h>
#include <config.h>
#include <asm/arch/iproc.h>
#include <asm/system.h>
#include "asm/iproc/reg_utils.h"
#include "asm/iproc/iproc_common.h"

DECLARE_GLOBAL_DATA_PTR;

extern int bcmiproc_eth_register(u8 dev_num);
extern void iproc_save_shmoo_values(void);

/*****************************************
 * board_init -early hardware init
 *****************************************/
int board_init (void)
{
    gd->bd->bi_arch_number = CONFIG_MACH_TYPE;      /* board id for linux */
    gd->bd->bi_boot_params = LINUX_BOOT_PARAM_ADDR; /* adress of boot parameters */

    return 0;
}

/*****************************************************************
 * misc_init_r - miscellaneous platform dependent initializations
 ******************************************************************/
int misc_init_r (void)
{
    return(0);
}

/**********************************************
 * dram_init - sets uboots idea of sdram size
 **********************************************/
int dram_init (void)
{
	ddr_init2();

    gd->ram_size = CONFIG_PHYS_SDRAM_1_SIZE - CONFIG_PHYS_SDRAM_RSVD_SIZE;

    return 0;
}

int board_early_init_f (void)
{
	uint32_t status = 0, otp_status, armclk = 0, max_armclk;
#ifdef CONFIG_ARMCLK
	armclk = CONFIG_ARMCLK; /* Allow ARM clock setting as build option */
#endif
	/* Bump up ARM clock as per OTP settings */
	status = get_otp_mem(0x09, &otp_status);
	if (!status) {
		/* get the ihost_limit_vco_freq[1:0] value */
		//printf("OTP[0x9]: 0x%x\n", otp_status);
		switch((otp_status >>18) & 0x3) {
			case 0: 
				max_armclk = 1200;
				break;
			case 1:
				max_armclk = 800;
				break;
			case 3:
				max_armclk = 1100;
				break;
			case 2:
			default:
				max_armclk = 1000;
		}
	}
	else {
		/* OTP read failed, DON'T stop, move on with default */
		max_armclk = 1000; /* setting for 1GHz */
	}
	
	/* Limit CPU clock to max CPU clock for this SKU */
	if((armclk > max_armclk) || (armclk == 0))
		armclk = max_armclk;

	iproc_config_armpll(armclk);
	/* Above call may return error i.e., failed to set up ARM PLL,
	   but system is still running at default clock, will be reported 
	   in log */

#if defined(CONFIG_GENPLL)
	status = iproc_config_genpll(1);
#endif

	return(status);
}

int board_late_init (void) 
{
	extern ulong mmu_table_addr;    /* MMU on flash fix */
	int status = 0;
    
	/* MMU on flash fix */
	asm volatile ("mcr p15, 0, %0, c2, c0, 0"::"r"(mmu_table_addr)); /*update TTBR0 */
	asm volatile ("mcr p15, 0, r1, c8, c7, 0");  /*invalidate TLB*/
	asm volatile ("mcr p15, 0, r1, c7, c10, 4"); /* DSB */
	asm volatile ("mcr p15, 0, r0, c7, c5, 4"); /* ISB */		
    
	/* Systick initialization(private timer)*/
	iproc_clk_enum();
	glb_tim_init();
        
#ifndef STDK_BUILD
	disable_interrupts();
#else
	gic_disable_interrupt(29);
	irq_install_handler(29, systick_isr, NULL);
	gic_config_interrupt(29, 1, IPROC_INTR_LEVEL_SENSITIVE, 0, IPROC_GIC_DIST_IPTR_CPU0);
	iproc_systick_init(10000);

	/* MMU and cache setup */
	disable_interrupts();
	//printf("Enabling SCU\n");
	scu_enable();

	printf("Enabling icache and dcache\n");
	dcache_enable();
	icache_enable();

	printf("Enabling l2cache\n");
	status = l2cc_enable();
	//printf("Enabling done, status = %d\n", status);

	enable_interrupts();
#endif

#if defined(CONFIG_RUN_DDR_SHMOO2) && defined(CONFIG_SHMOO_REUSE)
	/* Save DDR PHY parameters into flash if Shmoo was performed */
	iproc_save_shmoo_values();
#endif

	return status;
}

int board_eth_init(bd_t *bis)
{
	int rc = -1;
#ifdef CONFIG_BCMIPROC_ETH
	printf("Registering eth\n");
	rc = bcmiproc_eth_register(0);
#endif
	return rc;
}
