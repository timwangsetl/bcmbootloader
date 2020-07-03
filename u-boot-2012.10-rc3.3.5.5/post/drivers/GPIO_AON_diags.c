/*
 * $Copyright Open Broadcom Corporation$
 */

#include <common.h>
#include <post.h>
#include <iproc_gpio.h>
#include <iproc_gpio_cfg.h>



volatile int aonCountLevelHigh;
volatile int aonCountLevelLow;
volatile int aonCountRisingEdge;
volatile int aonCountFalliingEdge;

int numberOfAonGPIOs = 6;
unsigned int  allAonGPIOs = 0x3F;
int receivedAonGpioINT = 0;

extern volatile int AonGpioIntReceived;

#if defined(CONFIG_CYGNUS)
#if CONFIG_POST & CONFIG_SYS_POST_GPIO

static iproc_gpio_chip *testDev;


static void GPIO_testInit(void)
{
    //Perform hardware init
    iprocAonGPIOInit();
    //Get the device handle
    testDev=getAonGPIODev();
    
    iproc_aon_gpio_saveCFGRegisters();

}
static int GPIO_testCleanup(void)
{
    if ( iproc_aon_gpio_restoreRegisters() == -1 )
    {
        post_log("\nClean up failed ");
        return -1;
    }
    return 0;
}
static int GPIO_funcConfigTest( int cfg )
{
    int i,ret = 0;


    for ( i = 0; i < numberOfAonGPIOs; i++ )
    {
        
        iproc_aon_gpio_set_config(testDev,i, cfg);
    }
    for ( i = 0; i < numberOfAonGPIOs; i++ )
    {
        if ( cfg != iproc_aon_gpio_get_config(testDev,i))
        {
            post_log("\nAon GPIO pin %d is not configured as expected", i);
            ret = -1;
        }
    }
    if ( ret == -1 )
    {
        post_log("\nAon GPIO function configuration test failed");
    }
    else
    {
        post_log("\nAon GPIO function configuration test passed");
    }
    return ret;
}
static int GPIO_PullUpDownConfigTest( void )
{
    iproc_gpio_pull_t type;
    int ret = 0;
    int i;
    static char *pullUpDown[]={ "No Pull","Pull Down","Pull Up"};

    for ( type = IPROC_GPIO_PULL_NONE; type <=IPROC_GPIO_PULL_UP;type++ )
    {
        for ( i = 0; i < numberOfAonGPIOs; i++ )
        {
            iproc_aon_gpio_setpull_updown(testDev,i, type);
        }
        for ( i = 0; i < numberOfAonGPIOs; i++ )
        {
            if ( type != iproc_aon_gpio_getpull_updown(testDev,i))
            {
                post_log("\nAon GPIO pin %d can not be configured as %s ", i,pullUpDown[type]);
                ret = -1;
            }
        }
    }
    if ( ret == -1 )
    {
        post_log("\nAon GPIO pull up/down configuration test failed");
    }
    else
    {
        post_log("\nAon GPIO pull up/down configuration test passed");
    }
    return ret;
}
#if 0
static int GPIO_HYsteresisConfigTest( void )
{
    HysteresisEnable type;
    int ret = 0;
    int i;

    for ( type = DISABLE; type <=ENABLE;type++ )
    {
        for ( i = 0; i < numberOfAonGPIOs; i++ )
        {
            iproc_gpio_setHyeteresis(testDev,i, type);
        }
        for ( i = 0; i < numberOfAonGPIOs; i++ )
        {

            if ( type != iproc_aon_gpio_getHyeteresis(testDev,i))
            {
                post_log("\nAon GPIO pin %d can not be configured to enable Hysteresis ", i);
                ret = -1;
            }
        }
    }
    if ( ret == -1 )
    {
        post_log("\nAon GPIO Hysteresis configuration test failed");
    }
    else
    {
        post_log("\nAon GPIO Hysteresis configuration test passed");
    }
    return ret;
}
#endif
static int GPIO_DriveStrengthConfigTest( void )
{
    driveStrengthConfig ds;
    int ret = 0;
    int i;

    for ( ds = d_2mA; ds <=d_16mA;ds++ )
    {
        for ( i = 0; i < numberOfAonGPIOs; i++ )
        {
            iproc_aon_gpio_setDriveStrength(testDev,i, ds);
            if ( ds != iproc_aon_gpio_getDriveStrength(testDev,i))
            {
                post_log("\nAon GPIO pin %d can not be configured drive strength %d ", i,ds);
                ret = -1;
            }
        }
    }
    if ( ret == -1 )
    {
        post_log("\nAon GPIO drive strength configuration test failed");
    }
    else
    {
        post_log("\nAon GPIO drive strength configuration test passed");
    }
    return ret;
}
static int GPIO_InputDisableConfigTest( void )
{
    int ret = 0,i;
    //InputDisableCfg cfg;
    int j;

    //for ( cfg = INPUT_DISABLE; cfg >= INPUT_ENABLE;cfg--)
    for ( j=1; j >= 0 ;j--)
    {
        for ( i = 0; i < numberOfAonGPIOs; i++ )
        {
            iproc_aon_gpio_setInputdisable(testDev,i, j);
        }
        for ( i = 0; i < numberOfAonGPIOs; i++ )
        {

            if ( j != iproc_aon_gpio_getInputdisable(testDev,i))
            {
                post_log("\nAon GPIO pin %d can not be configured to be disabled/enabled %d ", i,j);
                ret = -1;
            }
        }
    }
    if ( ret == -1 )
    {
        post_log("\nAon GPIO input disable test failed");
    }
    else
    {
        post_log("\nAon GPIO input disable test passed");
    }
    return ret;
}
static int GPIO_post_testGPIOconfig(int flags )
{
    int ret = 0;


    //Save initial register settings before test
    iproc_gpio_saveCFGRegisters();
    if ( (flags & POST_AUTO) !=  POST_AUTO )
    {
       ret |= GPIO_funcConfigTest(IPROC_GPIO_GENERAL);
    }
    ret |= GPIO_funcConfigTest(IPROC_GPIO_AUX_FUN);

    ret |= GPIO_PullUpDownConfigTest();

    //ret |= GPIO_HYsteresisConfigTest();

    ret |=GPIO_DriveStrengthConfigTest();

    if ( (flags & POST_AUTO) !=  POST_AUTO )
    {
       ret |= GPIO_InputDisableConfigTest();
    }


    ret |= GPIO_testCleanup();
    return ret;

}
extern int post_get_board_diags_type( void );
int GPIO_AON_outputTest( void )
{
    int i=0;
    int ret=0;
    int val = 0;
 
    post_getUserResponse("\nAre SW9019 on? (Y/N)");
    
    val = 1;

    for ( i = 0; i < numberOfAonGPIOs; i++ )
    {
        setAonGPIOasOutput(i, val);
    }
    for ( i = 0; i < numberOfAonGPIOs; i++ )
    {
        if ( val != getAonGPIOpinValue(i))
        {
            post_log("\nAon GPIO pin %d is not set to output %d as GPIO", i, val);
            ret = -1;
        }
    }

    ret |= post_getUserResponse("\nAre LEDs D8524-D8525 turned on? (Y/N)");

    for ( i = 0; i < numberOfAonGPIOs; i++ )
    {
        setAonGPIOasOutput(i, 1-val);
    }

    for ( i = 0; i < numberOfAonGPIOs; i++ )
    {
        if ( (1-val) != getAonGPIOpinValue(i))
        {
            post_log("\nAon GPIO pin %d is not set to output %d as GPIO", i, 1 - val);
            ret |= -1;
        }
    }

    ret |= post_getUserResponse("\nAre LEDs D8524-D8525 turned off? (Y/N)");


    return ret;


}
int GPIO_AON_inputTest( void )
{
    int i=0;
    int ret =0;
    int retall=0;
    //TEST INPUT


    //SET to off position SW3501 GPIO 16-23 Pull up/Down   RED DOTS on sw is off position
    //    SW1901 GPIO 8-15
    //    SW1902 GPIO 0-7

    post_log("\nMake sure  SW1901,SW1902 and SW3501 are in ( ON position,switch is closed ) ");

    post_getConfirmation("\nAre all switches on? (Y/N)");

    for ( i = 0; i < numberOfAonGPIOs; i++ )
    {
        setAonGPIOasInput(i);
    }

    for ( i = 0; i < numberOfAonGPIOs; i++ )
    {
        if ( 0 != getAonGPIOpinValue(i) )
        {
            post_log("\nAon GPIO pin % is not reading 0 as expected", i);
            ret = -1;
            retall |= -1;
        }
        else
        {
        	post_log("\nAon GPIO pin % is reading 0 as expected", i);
        }
    }
    if ( ret == -1 )
    {
        post_log("\nAon GPIO input low test failed");
    }
    else
    {
        post_log("\nAon GPIO input low test passed");

    }

    post_log("\nput all switches in ( Off position,switch is open )");
    post_getConfirmation("\nAre all switches off? (Y/N)");

    //SET to ON position SW3501 GPIO 16-23 Pull up/Down   RED DOTS on sw is off position
    //    SW1901 GPIO 8-15
    //    SW1902 GPIO 0-7

    ret = 0;

    for ( i = 0; i < numberOfAonGPIOs; i++ )
    {
        if ( 1 != getAonGPIOpinValue(i) )
        {
            post_log("\nAon GPIO pin %  is not reading 1 as expected", i);
            ret |= -1;
            retall |= -1;
        }
        else
        {
        	post_log("\nAon GPIO pin %  is reading 1 as expected", i);
        }
    }

    if ( ret == -1 )
    {
        post_log("\nAon GPIO input high test failed");
    }
    else
    {
        post_log("\nAon GPIO input high test passed");

    }

    return retall;

}

int GPIO_AON_interruptTest( void )
{
    int i,ret=0;
    volatile unsigned long delay;

    aonCountLevelHigh = 0;
    aonCountLevelLow = 0;
    aonCountRisingEdge = 0;
    aonCountFalliingEdge = 0;

    //Set all gpio pins to init state
    initAonGPIOState();
    registerGPIOEdgeInterrupt();
    post_log("\nGPIO rising edge interrupt test");

    post_log("\nMake sure  SW1901,SW1902 and SW3501 are in ( ON position,switch is closed )");

    post_getConfirmation("\nAre all switches on? (Y/N)");
    for ( i = 0; i < numberOfAonGPIOs; i++)
    {
        setAonGPIOasInput(i);
        enableAonGPIOInterrupt( i, IRQ_TYPE_EDGE_RISING );

    }
    post_log("\nPut all switches to OFF (open) position SEQUENTIALLY");
    post_log("\nWaiting for interrupt...");
    delay = 0xFFFFFFF;

    AonGpioIntReceived= 0;
    while( (aonCountRisingEdge & allAonGPIOs)!=allAonGPIOs )
    {
    	if ( AonGpioIntReceived == 1)
    	{
    		AonGpioIntReceived = 0;
    		delay = 0xFFFFFFF;
    		post_log("\nReceived int on %d, Waiting for interrupt...",receivedAonGpioINT);
    	}
        if ( delay == 0 )
        {
            ret = -1;
            break;
        }
        delay--;
    }
    if ( ret == 0 )
    {
        post_log("\nReceived all %d rising edge interrupts, passed",AonGpioIntReceived);
    }
    else
    {
        for ( i = 0; i < AonGpioIntReceived; i++)
        {
            if ( (aonCountRisingEdge & ( 1 << i )) == 0 )
            {
                post_log("\nDid not receive interrupt for AON GPIO %d", i);
            }
            else
            {
            	post_log("\nReceived interrupt for AON GPIO %d", i);
            }
        }
        post_log("\nRising edge test failed");
    }


    //Set all gpio pins to init state
    initAonGPIOState();

    post_log("\nAon GPIO falling edge interrupt test");
    post_getConfirmation("\nReady? (Y/N)");



    for ( i = 0; i < AonGpioIntReceived; i++)
    {
        setAonGPIOasInput(i);
        AonGpioIntReceived = 0;
        //post_log("\nSet GPIO %d to off", i);
        enableAonGPIOInterrupt( i, IRQ_TYPE_EDGE_FALLING );
    }
    post_log("\nPut each of the switches to ON( closed) position SEQUENTIALLY");
    post_log("\nWaiting for interrupt...");
    delay = 0xFFFFFFF;
    AonGpioIntReceived = 0;
    ret = 0;
    while( (aonCountFalliingEdge & allAonGPIOs)!=allAonGPIOs )
    {
    	if ( AonGpioIntReceived == 1)
    	{
    		AonGpioIntReceived = 0;
    		delay = 0xFFFFFFF;
    		post_log("\nReceived int on %d, Waiting for interrupt...",receivedAonGpioINT);
    	}
        if ( delay == 0 )
        {
            ret = -1;
            break;
        }
        delay--;
    }
    if ( ret == 0 )
    {
        post_log("\nReceived all %d Falling edge interrupts, passed",numberOfAonGPIOs);
    }
    else
    {
        for ( i = 0; i < numberOfAonGPIOs; i++)
        {
            if ( (aonCountFalliingEdge & ( 1 << i )) == 0 )
            {
                post_log("\nDid not receive interrupt for Aon GPIO %d", i);
            }
            else
            {
            	post_log("\nReceived interrupt for Aon GPIO %d", i);
            }
        }
        post_log("\nFalling edge test failed");
    }


    //Set all gpio pins to init state
    initAonGPIOState();
    registerGPIOLevelInterrupt();
    post_log("\nGPIO level low interrupt test");

    post_log("\nMake sure SW1901,SW1902 and SW3501 are in ( ON position,switch is closed )");

    post_getConfirmation("\nReady? (Y/N)");

    for ( i = 0; i < numberOfAonGPIOs; i++)
    {
        setAonGPIOasInput(i);
        //post_log("\nSet pin %d off,Wait for interrupt...");
        enableAonGPIOInterrupt( i, IRQ_TYPE_LEVEL_LOW );
    }

    ret = 0;
    delay = 0xFFFFFF;
    while( (aonCountLevelLow & allAonGPIOs)!=allAonGPIOs )
    {
    	if ( delay == 0 )
    	{
    		ret = -1;
            for ( i = 0; i < numberOfAonGPIOs; i++)
            {
                if ( (aonCountLevelLow & ( 1 << i )) == 0 )
                {
                    post_log("\nDid not receive interrupt for Aon GPIO %d", i);
                }
                else
                {
                	//post_log("\nReceived interrupt for Aon GPIO %d", i);
                }
            }

            break;
    	}
    	delay --;
    }
    if ( ret == 0 )
    {
        post_log("\nReceived all %d level low interrupts, passed",numberOfAonGPIOs);
    }
    else
    {
        post_log("\nLevel low interrupt test failed");

    }
    
    post_log("\nGPIO level high interrupt test");

    post_log("\nMake sure SW1901,SW1902 and SW3501 are in ( OFF position,switch is open )");

    post_getConfirmation("\nReady? (Y/N)");

    for ( i = 0; i < numberOfAonGPIOs; i++)
    {
        setAonGPIOasInput(i);
        enableAonGPIOInterrupt( i, IRQ_TYPE_LEVEL_HIGH);
    }

    ret = 0;
    delay = 0xFFFFFF;
    while( (aonCountLevelHigh & allAonGPIOs)!=allAonGPIOs )
    {
    	if ( delay == 0 )
    	{
    		ret = -1;
            for ( i = 0; i < numberOfAonGPIOs; i++)
            {
                if ( (aonCountLevelHigh & ( 1 << i )) == 0 )
                {
                    post_log("\nDid not receive interrupt for Aon GPIO %d", i);
                }
                else
				{
					post_log("\nReceived interrupt for Aon GPIO %d", i);
				}
            }
            break;
    	}
    	delay --;
    }
    if ( ret == 0 )
    {
        post_log("\nReceived all %d level high interrupts, passed", numberOfAonGPIOs);
    }
    else
    {
        post_log("\nLevel high interrupt test failed");

    }

    initAonGPIOState();

    return ret;
}

extern void iproc_aon_gpio_iomux(int op);

int GPIO_AON_post_test (int flags)
{
    int ret = 0;
    int status = 0;

    iproc_aon_gpio_iomux(1);
    post_log("\nAON GPIO diags starts  !!!");
    //Init for test
    GPIO_testInit();

    //Test GPIO configurations in the DMU
    ret = GPIO_post_testGPIOconfig(flags);
    status |= ret;

    if ( -1 == ret )
    {
        post_log("\nAon GPIO configuration tests failed");
    }
    else
    {
        post_log("\nAon GPIO configuration tests passed");
    }

    if ( (flags & POST_AUTO) ==  POST_AUTO )
    {
        post_log("\nAon GPIO diags done  !!!\n");
        return ret;
    }
#if 0
    ret = GPIO_funcConfigTest(IPROC_GPIO_GENERAL);
    status |= ret;
    if ( -1 == ret )
    {
        post_log("\nAon GPIO configuration tests failed");
    }
    else
    {
        post_log("\nAon GPIO configuration tests passed");
    }
#endif

    ret = GPIO_AON_outputTest();
    status |= ret;
    if ( -1 == ret )
    {
        post_log("\nAON GPIO output tests failed");
    }
    else
    {
        post_log("\nAON GPIO output tests passed");
    }
#if 0
    ret = GPIO_AON_inputTest();
    status |= ret;
    if ( -1 == ret )
    {
        post_log("\nAON GPIO input tests failed");
    }
    else
    {
        post_log("\nAON GPIO input tests passed");
    }

    ret = GPIO_AON_interruptTest();
    status |= ret;
    if ( -1 == ret )
    {
        post_log("\nAON GPIO interrupt test failed");
    }
    else
    {
        post_log("\nAON GPIO interrupt test passed");
    }

    GPIO_funcConfigTest(IPROC_GPIO_AUX_FUN);
#endif
    post_log("\nAON GPIO diags done  !!!\n");

    iproc_aon_gpio_iomux(0);
    
    return status;
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_PWM */
#endif

