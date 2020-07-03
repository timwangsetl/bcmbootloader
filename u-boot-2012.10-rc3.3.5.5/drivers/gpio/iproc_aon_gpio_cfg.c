/*
 * $Copyright Open Broadcom Corporation$
 */
#include "config.h"
#include "iproc_gpio.h"
#include "iproc_gpio_cfg.h"
#include  <asm/arch/socregs.h>

#if defined(CONFIG_CYGNUS) 


#define readl(addr) (*(volatile unsigned int*)(addr))
#define writel(b,addr) ((*(volatile unsigned int *) (addr)) = (b))
#define __raw_readl readl
#define __raw_writel writel


static regValuePair curRegValue[]=
{
	{AON_GPIO_CONTROL1,              0},
	{AON_GPIO_CONTROL2,              0},
	{AON_GPIO_CONTROL3,              0},
	{AON_GPIO_CONTROL4,              0},
	{AON_GPIO_CONTROL5,              0},
	{AON_GPIO_CONTROL6,              0},
	{GP_DATA_IN,              0},
	{GP_DATA_OUT,              0},
	{GP_OUT_EN,              0},
	{GP_INT_TYPE,              0},
	{GP_INT_DE,              0},
	{GP_INT_EDGE,              0},    
	{GP_INT_MSK,              0},
	{GP_INT_STAT,              0},
	{GP_INT_MSTAT,              0},
	{GP_INT_CLR,              0},
	{GP_AUX_SEL,              0},
	{GP_INIT_VAL,              0}
};


void iproc_aon_gpio_saveCFGRegisters( void )
{
	int i, size;
	size = sizeof ( curRegValue )/ sizeof( regValuePair );
	for ( i = 0; i < size; i++ )
	{
		curRegValue[i].value = __raw_readl(curRegValue[i].regAddr);
	}
}
int iproc_aon_gpio_restoreRegisters( void )
{
	int i, size;
	size = sizeof ( curRegValue )/ sizeof( regValuePair );
	for ( i = 0; i < size; i++ )
	{
		__raw_writel(curRegValue[i].value, curRegValue[i].regAddr);
	}
	return 0;
}

//set pin configuration: GPIO or iproc function

int iproc_aon_gpio_set_config(iproc_gpio_chip *chip,
			      unsigned int off, unsigned int cfg)
{
    unsigned long aux_sel;
    unsigned int aux_sel_reg;
    unsigned int base = 0;
    
    aux_sel_reg = GP_AUX_SEL_BASE;
    base = (unsigned int)chip->ioaddr; 
    aux_sel = __raw_readl(base + aux_sel_reg);

    switch (cfg) {
        case IPROC_GPIO_GENERAL:
            aux_sel &= ~(1 << off);
            break;            
        case IPROC_GPIO_AUX_FUN:
            aux_sel |= (1 << off);
            break;
        default:
            return -1;
    }
    __raw_writel(aux_sel, base + aux_sel_reg);

    return 0;
}

//Get pin configuration: GPIO or iproc
unsigned iproc_aon_gpio_get_config(iproc_gpio_chip *chip,
				   unsigned int off)
{
    unsigned long aux_sel;
    unsigned int aux_sel_reg;
    unsigned int base = 0;

    aux_sel_reg = GP_AUX_SEL_BASE;
    base = (unsigned int)chip->ioaddr; 
    aux_sel = __raw_readl(base + aux_sel_reg);

    if (aux_sel & (1 << off)) {
        return IPROC_GPIO_AUX_FUN;        
    }
    else {
        return IPROC_GPIO_GENERAL;              
    }        
}


int iproc_aon_gpio_setpull_updown(iproc_gpio_chip *chip,
			    unsigned int off, iproc_gpio_pull_t pull)
{
    unsigned int base;
    
    if (chip->id == IPROC_GPIO_CCG_ID) {
        unsigned long pad_res, res_en;        

        base = (unsigned int)chip->ioaddr;

        pad_res = __raw_readl(base + GP_PAD_RES_BASE);
        res_en = __raw_readl(base + GP_RES_EN_BASE);

        switch (pull) {
            case IPROC_GPIO_PULL_UP:
                pad_res |= (1 << off);
                res_en |= (1 << off);
                break;
            case IPROC_GPIO_PULL_DOWN:
                pad_res &= ~(1 << off);
                res_en |= (1 << off);
                break;
            case IPROC_GPIO_PULL_NONE:           
                res_en &= ~(1 << off);
                break;                
            default:
                return -1;
        }
        __raw_writel(pad_res, base + GP_PAD_RES_BASE);
        __raw_writel(res_en, base + GP_RES_EN_BASE);
    }

    return 0;
}


iproc_gpio_pull_t iproc_aon_gpio_getpull_updown(iproc_gpio_chip *chip,
					unsigned int off)
{

    unsigned int base;
    base = 0;

        unsigned long pad_res, res_en;        

        base = (unsigned int)chip->ioaddr;

        pad_res = __raw_readl(base + GP_PAD_RES_BASE);
        res_en = __raw_readl(base + GP_RES_EN_BASE);

        pad_res &= 1 << off;
        res_en &= 1 << off;

        if (res_en) {
            if (pad_res) {
                return IPROC_GPIO_PULL_UP;
            } else {
                return IPROC_GPIO_PULL_DOWN;
            }
        } else {
            return IPROC_GPIO_PULL_NONE;
        }

}
int iproc_aon_gpio_setHyeteresis(iproc_gpio_chip *chip,
			    unsigned int off, HysteresisEnable enableDisable)
{

    unsigned int reg;
    unsigned long hysteresis;

    reg = AON_GPIO_CONTROL5;
   
    hysteresis = __raw_readl(reg);

        switch (enableDisable) {
            case ENABLE:
            	hysteresis |= (1 << off);
                break;
            case DISABLE:
            	hysteresis &= ~(1 << off);
                break;
            default:
                return -1;
        }
    __raw_writel(hysteresis, reg);

    return 0;
}


HysteresisEnable iproc_aon_gpio_getHyeteresis(iproc_gpio_chip *chip,
					unsigned int off)
{

    unsigned long hysteresis;

    unsigned int reg;

    reg = AON_GPIO_CONTROL5;       
    hysteresis = __raw_readl(reg);
    hysteresis &= 1 << off;

    if (hysteresis)
    {
        return ENABLE;
    }
    else
    {
        return DISABLE;
    }
}
int iproc_aon_gpio_setSlewrate(iproc_gpio_chip *chip,
			    unsigned int off, SlewRateCfg type)
{

    unsigned long slewRate;
    unsigned int reg;

    reg = AON_GPIO_CONTROL6;


        slewRate = __raw_readl(reg);

        switch (type) {
            case SLEWED_EDGE:
            	slewRate |= (1 << off);
                break;
            case FAST_EDGE:
            	slewRate &= ~(1 << off);
                break;
            default:
                return -1;
        }
        __raw_writel(slewRate, reg);

    return 0;
}


SlewRateCfg iproc_aon_gpio_getSlewrate(iproc_gpio_chip *chip,
					unsigned int off)
{


    unsigned long slewRate;
    unsigned int reg;

    reg = AON_GPIO_CONTROL6;

        slewRate = __raw_readl(reg);

        slewRate &= 1 << off;

        if (slewRate)
        {
            return SLEWED_EDGE;
        }
        else
        {
            return FAST_EDGE;
        }
}

int iproc_aon_gpio_setDriveStrength(iproc_gpio_chip *chip,
			    unsigned int off, driveStrengthConfig ds)
{
    unsigned long ds0,ds1,ds2;

    ds2 = __raw_readl(AON_GPIO_CONTROL1);
    ds1 = __raw_readl(AON_GPIO_CONTROL2);
    ds0 = __raw_readl(AON_GPIO_CONTROL3);   

        switch (ds) {
            case d_2mA:
            	ds2 &= ~(1 << off);
            	ds1 &= ~(1 << off);
            	ds0 &= ~(1 << off);
                break;
            case d_4mA:
            	ds2 &= ~(1 << off);
            	ds1 &= ~(1 << off);
            	ds0 |= (1 << off);
                break;
            case d_6mA:
            	ds2 &= ~(1 << off);
            	ds1 |=  (1 << off);
            	ds0 &= ~(1 << off);
                break;
            case d_8mA:
            	ds2 &= ~(1 << off);
            	ds1 |=  (1 << off);
            	ds0 |=  (1 << off);
                break;
            case d_10mA:
            	ds2 |=  (1 << off);
            	ds1 &= ~(1 << off);
            	ds0 &= ~(1 << off);
                break;
            case d_12mA:
            	ds2 |=  (1 << off);
            	ds1 &= ~(1 << off);
            	ds0 |=  (1 << off);
                break;
            case d_14mA:
            	ds2 |=  (1 << off);
            	ds1 |=  (1 << off);
            	ds0 &=  ~(1 << off);
                break;
            case d_16mA:
            	ds2 |=  (1 << off);
            	ds1 |=  (1 << off);
            	ds0 |=  (1 << off);
                break;

            default:
                return -1;
        }
    __raw_writel(ds2, AON_GPIO_CONTROL1);
    __raw_writel(ds1, AON_GPIO_CONTROL2);
    __raw_writel(ds0, AON_GPIO_CONTROL3);

    return 0;
}


driveStrengthConfig iproc_aon_gpio_getDriveStrength(iproc_gpio_chip *chip,
					unsigned int offset)
{
    unsigned long ds0,ds1,ds2, ds;

    ds2 = __raw_readl(AON_GPIO_CONTROL1);
    ds1 = __raw_readl(AON_GPIO_CONTROL2);
    ds0 = __raw_readl(AON_GPIO_CONTROL3);   


        ds0 = (( 1 << offset ) & ds0 ) >> offset;
        ds1 = (( 1 << offset ) & ds1 ) >> offset;
        ds2 = (( 1 << offset ) & ds2 ) >> offset;
        ds = ((ds2  << 2 ) | (ds1 << 1 ) | ds0 );
        return ( (driveStrengthConfig)ds);

}
int iproc_aon_gpio_setInputdisable (iproc_gpio_chip *chip,
			    unsigned int off, InputDisableCfg enableDisable)
{
    unsigned long inputDisabledCfg;
    unsigned int reg;

    reg = AON_GPIO_CONTROL4;
  
        inputDisabledCfg = __raw_readl(reg);

        switch (enableDisable) {
            case INPUT_DISABLE:
            	inputDisabledCfg |= (1 << off);
                break;
            case INPUT_ENABLE:
            	inputDisabledCfg &= ~(1 << off);
                break;
            default:
                return -1;
        }
        __raw_writel(inputDisabledCfg, reg);

    return 0;
}


InputDisableCfg iproc_aon_gpio_getInputdisable(iproc_gpio_chip *chip,
					unsigned int off)
{

	unsigned long inputDisabledCfg;
    unsigned int reg;

    reg = AON_GPIO_CONTROL4;


        inputDisabledCfg = __raw_readl(reg);

        inputDisabledCfg &= 1 << off;

        if (inputDisabledCfg)
        {
            return INPUT_DISABLE;
        }
        else
        {
            return INPUT_ENABLE;
        }
}

#endif


