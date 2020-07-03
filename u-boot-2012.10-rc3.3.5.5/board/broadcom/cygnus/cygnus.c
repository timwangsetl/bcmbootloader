#include <common.h>
#include <asm/io.h>
#include <asm/mach-types.h>

#ifdef   CONFIG_CYGNUS_EMULATION
#include <configs/cygnus_emul.h>
#else
#include <configs/cygnus.h>
#endif

#include <asm/arch/iproc.h>
#include <asm/arch/socregs.h>
#include <asm/arch/reg_utils.h>
#include <asm/system.h>

DECLARE_GLOBAL_DATA_PTR;

extern int bcmiproc_eth_register(u8 dev_num);
extern void         save_shmoo_to_flash(void);

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
extern void ddr_init2(void);
int dram_init (void)
{
#ifndef CONFIG_CYGNUS_EMULATION
     ddr_init2();
#endif

    gd->ram_size = CONFIG_PHYS_SDRAM_1_SIZE - CONFIG_PHYS_SDRAM_RSVD_SIZE;

    return 0;
}
extern uint32_t iproc_config_armpll(uint32_t clkmhz);
int board_early_init_f (void)
{
    uint32_t status = 0, armclk;

     armclk = 1000;

    iproc_config_armpll(armclk);

#if defined(CONFIG_GENPLL)
idfasdf
        status = iproc_config_genpll(1);
#endif

        return(status);
}

#ifdef CONFIG_PWM0_FOR_FAN
/* Fixme: Better to have a generic PWM config function to configure any 
   PWM channel */
void bcm95301x_config_pwm0_4_fan(void)
{
	reg32_set_bits(ChipcommonB_GP_AUX_SEL, 0x01); // Select PWM 0 
	reg32_clear_bits(CRU_GPIO_CONTROL0_OFFSET, 1 << 8); // Selct PWM
	reg32_write_masked(ChipcommonB_PWM_PRESCALE, 0x3F, 24); // prescale = 24 + 1
	reg32_write(ChipcommonB_PWM_PERIOD_COUNT0, 5 * 180); 
	reg32_write(ChipcommonB_PWM_DUTYHI_COUNT0, 5 * 135);
	reg32_set_bits(ChipcommonB_PWMCTL, 0x101); // Enable PWM0
}
#endif

#ifdef CONFIG_BOARD_LATE_INIT
extern void iproc_clk_enum(void);
int board_late_init (void) 
{
    int status = 0;

    iproc_clk_enum();

    disable_interrupts();
    
    /* save shmoo data to flash if and only if current flash content is not correct
     * and new shmoo calibration is just finished via ddr_init2 */
    save_shmoo_to_flash();

    return status;
}
#endif

int board_eth_init(bd_t *bis)
{
	int rc = -1;
#ifdef CONFIG_BCMIPROC_ETH
	printf("Registering eth\n");
	rc = bcmiproc_eth_register(0);
#endif
	return rc;
}

