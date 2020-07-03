/*
 * $Copyright Open Broadcom Corporation$
 */

#include <common.h>
#include <configs/iproc_board.h>
#include <asm/arch/iproc.h>
#include "iproc_gpio.h"
#include <asm/arch/socregs.h>

#if defined(CONFIG_CYGNUS)


#ifdef IPROC_GPIO_CCA_INT_EVENT_MASK
#undef IPROC_GPIO_CCA_INT_EVENT_MASK
#endif
#define IPROC_GPIO_CCA_INT_EVENT_MASK  GP_INT_MSK_BASE
#ifdef IPROC_GPIO_CCA_INT_LEVEL_MASK
#undef IPROC_GPIO_CCA_INT_LEVEL_MASK
#endif
#define IPROC_GPIO_CCA_INT_LEVEL_MASK  GP_INT_MSK_BASE 
#ifdef IPROC_GPIO_CCA_INT_EVENT
#undef IPROC_GPIO_CCA_INT_EVENT
#endif
#define IPROC_GPIO_CCA_INT_EVENT       GP_INT_STAT_BASE
#ifdef IPROC_GPIO_CCA_INT_LEVEL
#undef IPROC_GPIO_CCA_INT_LEVEL
#endif
#define IPROC_GPIO_CCA_INT_LEVEL       GP_INT_TYPE_BASE 
#ifdef IPROC_GPIO_CCA_INT_EDGE
#undef IPROC_GPIO_CCA_INT_EDGE
#endif
#define IPROC_GPIO_CCA_INT_EDGE        GP_INT_TYPE_BASE
#ifdef IPROC_GPIO_CCA_DIN
#undef IPROC_GPIO_CCA_DIN
#endif
#define IPROC_GPIO_CCA_DIN             GP_DATA_IN_BASE


#define readl(addr) (*(volatile unsigned int*)(addr))
#define writew(b,addr) ((*(volatile unsigned int *) (addr)) = (b))

extern int aonCountLevelHigh;
extern int aonCountLevelLow;
extern int aonCountRisingEdge;
extern int aonCountFalliingEdge;
extern int numberOfAonGPIOs;

extern int receivedAonGpioINT;

volatile int AonGpioIntReceived = 0;

static iproc_gpio_chip gpioDev;
static int currentIRQType;

static unsigned int _iproc_gpio_readl(iproc_gpio_chip *chip, int reg)
{
    return readl(chip->ioaddr + reg);
}

static void _iproc_gpio_writel(iproc_gpio_chip *chip, unsigned int val, int reg)
{
    writew(val, chip->ioaddr + reg);
}


static int iproc_gpio_to_irq(iproc_gpio_chip *chip, unsigned int pin) {
    return (chip->irq_base + pin);
}

static int iproc_irq_to_gpio(iproc_gpio_chip *chip, unsigned int irq) {
    return (irq - chip->irq_base);
}


static void iproc_gpio_irq_unmask(unsigned int irq, IRQTYPE irq_type)
{
    iproc_gpio_chip *ourchip = &gpioDev;
    int pin;
    unsigned int int_mask;

    pin = iproc_irq_to_gpio(ourchip, irq);
    //irq_type = IRQ_TYPE_EDGE_BOTH;//for now swang //irq_desc[irq].status & IRQ_TYPE_SENSE_MASK;


        unsigned int  event_mask; 

        event_mask = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_INT_EVENT_MASK);
        int_mask = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_INT_LEVEL_MASK);

        if (irq_type & IRQ_TYPE_EDGE_BOTH) {
            event_mask |= 1 << pin;
            _iproc_gpio_writel(ourchip, event_mask, 
                IPROC_GPIO_CCA_INT_EVENT_MASK);
        } else {
            int_mask |= 1 << pin;
            _iproc_gpio_writel(ourchip, int_mask, 
                IPROC_GPIO_CCA_INT_LEVEL_MASK);
        }

}
static void iproc_gpio_irq_mask(unsigned int irq, IRQTYPE irq_type)
{
    iproc_gpio_chip *ourchip = &gpioDev;
    int pin;
    unsigned int int_mask;


    pin = iproc_irq_to_gpio(ourchip, irq);
    //irq_type = IRQ_TYPE_EDGE_BOTH; //for now swang ???//irq_desc[irq].status & IRQ_TYPE_SENSE_MASK;

    if ((ourchip->id == IPROC_GPIO_CCG_ID)) {
        unsigned int  event_mask;
        
        event_mask = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_INT_EVENT_MASK);
        int_mask = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_INT_LEVEL_MASK);

        if (irq_type & IRQ_TYPE_EDGE_BOTH) {
            event_mask &= ~(1 << pin);
            _iproc_gpio_writel(ourchip, event_mask,
                IPROC_GPIO_CCA_INT_EVENT_MASK);
        } else {
            int_mask &= ~(1 << pin);
            _iproc_gpio_writel(ourchip, int_mask, 
                IPROC_GPIO_CCA_INT_LEVEL_MASK);
        }
    }
}

static int iproc_gpio_irq_set_type(unsigned int irq, IRQTYPE type)
{
    iproc_gpio_chip *ourchip = &gpioDev;
    int pin;   
    unsigned int  event_pol, int_pol; 

    currentIRQType = type;

    pin = iproc_irq_to_gpio(ourchip, irq);

    if (ourchip->id == IPROC_GPIO_CCG_ID) {

        switch (type) {
        case IRQ_TYPE_EDGE_RISING:
            event_pol = _iproc_gpio_readl(ourchip, GP_INT_TYPE_BASE);
            event_pol &= ~(1 << pin);
            _iproc_gpio_writel(ourchip, event_pol, GP_INT_TYPE_BASE);
            event_pol = _iproc_gpio_readl(ourchip, GP_INT_DE_BASE);
            event_pol &= ~(1 << pin);
            _iproc_gpio_writel(ourchip, event_pol, GP_INT_DE_BASE);
            event_pol = _iproc_gpio_readl(ourchip, GP_INT_EDGE_BASE);
            event_pol |= (1 << pin);
            _iproc_gpio_writel(ourchip, event_pol, GP_INT_EDGE_BASE);          
            break;
        case IRQ_TYPE_EDGE_FALLING:
            event_pol = _iproc_gpio_readl(ourchip, GP_INT_TYPE_BASE);
            event_pol &= ~(1 << pin);
            _iproc_gpio_writel(ourchip, event_pol, GP_INT_TYPE_BASE);
            event_pol = _iproc_gpio_readl(ourchip, GP_INT_DE_BASE);
            event_pol &= ~(1 << pin);
            _iproc_gpio_writel(ourchip, event_pol, GP_INT_DE_BASE);
            event_pol = _iproc_gpio_readl(ourchip, GP_INT_EDGE_BASE);
            event_pol &= ~(1 << pin);
            _iproc_gpio_writel(ourchip, event_pol, GP_INT_EDGE_BASE);
            break;
        case IRQ_TYPE_LEVEL_HIGH:
            int_pol = _iproc_gpio_readl(ourchip, GP_INT_TYPE_BASE);
            int_pol |= (1 << pin);
            _iproc_gpio_writel(ourchip, int_pol, GP_INT_TYPE_BASE);
            int_pol = _iproc_gpio_readl(ourchip, GP_INT_DE_BASE);
            int_pol &= ~(1 << pin);
            _iproc_gpio_writel(ourchip, int_pol, GP_INT_DE_BASE);
            int_pol = _iproc_gpio_readl(ourchip, GP_INT_EDGE_BASE);
            int_pol |= (1 << pin);
            _iproc_gpio_writel(ourchip, int_pol, GP_INT_EDGE_BASE); 
            break;
        case IRQ_TYPE_LEVEL_LOW:
            int_pol = _iproc_gpio_readl(ourchip, GP_INT_TYPE_BASE);
            int_pol |= (1 << pin);
            _iproc_gpio_writel(ourchip, int_pol, GP_INT_TYPE_BASE);
            int_pol = _iproc_gpio_readl(ourchip, GP_INT_DE_BASE);
            int_pol &= ~(1 << pin);
            _iproc_gpio_writel(ourchip, int_pol, GP_INT_DE_BASE);
            int_pol = _iproc_gpio_readl(ourchip, GP_INT_EDGE_BASE);
            int_pol &= ~(1 << pin);
            _iproc_gpio_writel(ourchip, int_pol, GP_INT_EDGE_BASE);             
            break;
        case IRQ_TYPE_EDGE_BOTH:
            event_pol = _iproc_gpio_readl(ourchip, GP_INT_TYPE_BASE);
            event_pol &= ~(1 << pin);
            _iproc_gpio_writel(ourchip, event_pol, GP_INT_TYPE_BASE);
            event_pol = _iproc_gpio_readl(ourchip, GP_INT_DE_BASE);
            event_pol |= (1 << pin);
            _iproc_gpio_writel(ourchip, event_pol, GP_INT_DE_BASE);
            break;
        default:
            //printf(KERN_ERR "unsupport irq type !\n");
            return -1;
        }
    }

    return 0;
}


void  iproc_aon_gpio_irq_handler(void *data)
{
   
    unsigned int  event_pol,  in;
    unsigned int  event_mask, event,tmp = 0;
    int iter;
    iproc_gpio_chip *ourchip = &gpioDev;

    event_mask = _iproc_gpio_readl(ourchip, GP_INT_MSK_BASE);
    event_pol = _iproc_gpio_readl(ourchip, GP_INT_TYPE_BASE);  
    in = _iproc_gpio_readl(ourchip, GP_DATA_IN_BASE);
    event = _iproc_gpio_readl(ourchip, GP_INT_STAT_BASE);

	tmp = event_mask & event;
    //6 is the number of GPIOs
    for (iter = 0; iter < numberOfAonGPIOs; iter ++)
    {   
        if (tmp & ( 1 << iter ))
        {
       	    if (tmp  & (1 << iter) & (~event_pol)) /*edge triggered*/
	        {
		        if (in & (1 << iter))
		        {
		            aonCountRisingEdge |= 1 << iter;
		            puts("\nRising edge interrupt");
		        }
		        else
		        {
		            aonCountFalliingEdge |= 1 << iter;
		            puts("\nFalling edge interrupt");
		        }
		        receivedAonGpioINT = iter;
	        }
	        else
	        {
	            if ( in & ( 1 << iter ))
	            {
	                aonCountLevelHigh |= 1 << iter;
	                puts("\nLevel high interrupt");
	
	            }
	            else
	            {
	                aonCountLevelLow |= 1 << iter;
	                puts("\nLevel low interrupt");
	
	            }
	        }
	        printf(" Aon GPIO pin %d\n", iter);
	        /* clear the interrupt*/
	        *(volatile unsigned long*)(GP_INT_CLR) |= (1<<iter);
	        AonGpioIntReceived = 1;
	    }
    }

    return ;
}

//set a GPIO pin as an input
static int iproc_gpiolib_input(iproc_gpio_chip *ourchip, unsigned gpio)
{
    unsigned int  val;
    unsigned int  nBitMask = 1 << gpio;


    //iproc_gpio_lock(ourchip, flags);

    val = _iproc_gpio_readl(ourchip, GP_OUT_EN_BASE);
    val &= ~nBitMask;
    _iproc_gpio_writel(ourchip, val, GP_OUT_EN_BASE);

    //iproc_gpio_unlock(ourchip, flags);
    return 0;
}

//set a gpio pin as a output
static int iproc_gpiolib_output(iproc_gpio_chip *ourchip,
                  unsigned gpio, int value)
{


    unsigned long val;
    unsigned int  nBitMask = 1 << gpio;

    //iproc_gpio_lock(ourchip, flags);

    val = _iproc_gpio_readl(ourchip, GP_OUT_EN_BASE);
    val |= nBitMask;
    _iproc_gpio_writel(ourchip, val, GP_OUT_EN_BASE);

    //iproc_gpio_unlock(ourchip, flags);
    return 0;
}

//set the gpio pin ( by gpio ) to value ( only for output, input is read only
static void iproc_gpiolib_set(iproc_gpio_chip *ourchip,
                unsigned gpio, int value)
{


    unsigned long val;
    unsigned int  nBitMask = 1 << gpio;

    //iproc_gpio_lock(ourchip, flags);


    /* determine the GPIO pin direction 
     */ 
    val = _iproc_gpio_readl(ourchip, GP_OUT_EN_BASE);
    //printf("en:%x\n", val);
    val &= nBitMask;

    /* this function only applies to output pin
     */ 
    if (!val)
        return;

    val = _iproc_gpio_readl(ourchip, GP_DATA_OUT_BASE);
    
    if ( value == 0 ){
        /* Set the pin to zero */
        val &= ~nBitMask;
    }else{
        /* Set the pin to 1 */
        val |= nBitMask;
    }    
    _iproc_gpio_writel(ourchip, val, GP_DATA_OUT_BASE);
    
    //printf("addr:%x out:%x\n", ourchip->ioaddr+ GP_DATA_OUT_BASE, val);
    //iproc_gpio_unlock(ourchip, flags);

}

//get the gpio pin value
static int iproc_gpiolib_get(iproc_gpio_chip *ourchip, unsigned gpio)
{


    unsigned int val, offset;
    unsigned int  nBitMask = 1 << gpio;    

    //iproc_gpio_lock(ourchip, flags);
    /* determine the GPIO pin direction 
     */ 
    offset = _iproc_gpio_readl(ourchip, GP_OUT_EN_BASE);
    offset &= nBitMask;

    //output pin
    if (offset){
        val = _iproc_gpio_readl(ourchip, GP_DATA_OUT_BASE);
    }
    //input pin
    else {
        val = _iproc_gpio_readl(ourchip, GP_DATA_IN_BASE);    
    }
    val >>= gpio;
    val &= 1;
    //printf("\nGPIO %d, input value reg = 0x%x,,value= %d ,offset %d", gpio, *((unsigned long*)0x18000060),val, offset);
    //iproc_gpio_unlock(ourchip, flags);

    return val;
}


void iprocAonGPIOInit( void )
{
    gpioDev.id = IPROC_GPIO_CCG_ID;
    gpioDev.ioaddr = (void *)(GP_DATA_IN -0x800);//(void*)IPROC_GPIO_CCA_BASE;
    gpioDev.dmu_ioaddr = 0;//(void*)NORTHSTAR_DMU_BASE;
    gpioDev.intr_ioaddr =(void *) GP_DATA_IN;//(void*)CCA_CHIPID ;
    gpioDev.irq_base = 0;

}

iproc_gpio_chip *getAonGPIODev ( void )
{
    return &gpioDev;
}

void setAonGPIOasInput( int pin )
{
    if ( (pin < numberOfAonGPIOs ) && ( pin >= 0 ))
    {
        iproc_gpiolib_input(&gpioDev, pin );
    }
}
void setAonGPIOasOutput( int pin, int value )
{
    if ( (pin < numberOfAonGPIOs ) && ( pin >= 0 ))
    {
        //Set the pin to be output
        iproc_gpiolib_output(&gpioDev,pin,value);
        //set value
        iproc_gpiolib_set(&gpioDev,pin,value);
    }
}
int getAonGPIOpinValue ( int pin )
{
    if ( (pin < numberOfAonGPIOs ) && ( pin >= 0 ))
    {
        return iproc_gpiolib_get(&gpioDev, pin );
    }
    return -1;
}
void enableAonGPIOInterrupt(int pin, IRQTYPE irqtype )
{
    unsigned int irq;
    unsigned long temp;


    if ( (pin < numberOfAonGPIOs ) && ( pin >= 0 ))
    {
        //Map pin to IRQ number
        irq = iproc_gpio_to_irq(&gpioDev,pin);
        iproc_gpio_irq_set_type(irq,irqtype);
        iproc_gpio_irq_unmask(irq, irqtype);

        //while ( tempDelay < 0xFFFF )
        //{
        //    tempDelay++;
        //}
        //post_log("\n Enable Interrupt");
        //Enable GPIO interrupt at top level
        temp = readl(gpioDev.intr_ioaddr + CCA_INT_MASK_BASE);
        writew ( (temp |IPROC_CCA_INT_F_GPIOINT),gpioDev.intr_ioaddr + CCA_INT_MASK_BASE);
    }

}
void disableAonGPIOInterrupt(int pin, IRQTYPE irqtype )
{
    unsigned int irq;
    unsigned long temp;

    if ( (pin < numberOfAonGPIOs ) && ( pin >= 0 ))
    {
        //Map pin to IRQ number
        irq = iproc_gpio_to_irq(&gpioDev,pin);
        //iproc_gpio_irq_set_type(irq,irqtype);
        iproc_gpio_irq_mask(irq,irqtype);

        //Disable GPIO interrupt at top level
        temp = readl(gpioDev.intr_ioaddr + CCA_INT_MASK_BASE);
        writew ( (temp & ~IPROC_CCA_INT_F_GPIOINT),gpioDev.intr_ioaddr + CCA_INT_MASK_BASE);
    }

}

extern void iproc_aon_gpio_iomux(int op);
void initAonGPIOState ( void )
{

#ifndef CONFIG_CYGNUS_EMULATION
    iproc_aon_gpio_iomux(1);
#endif

    _iproc_gpio_writel(&gpioDev, 0xFF, IPROC_GPIO_CCA_INT_EVENT);

    _iproc_gpio_writel(&gpioDev, 0x00, IPROC_GPIO_CCA_INT_LEVEL);

    _iproc_gpio_writel(&gpioDev, 0x00, IPROC_GPIO_CCA_INT_LEVEL_MASK);

    _iproc_gpio_writel(&gpioDev, 0x00, IPROC_GPIO_CCA_INT_EVENT_MASK);

    _iproc_gpio_writel(&gpioDev, 0x00, IPROC_GPIO_CCA_INT_EDGE);

}

#endif
