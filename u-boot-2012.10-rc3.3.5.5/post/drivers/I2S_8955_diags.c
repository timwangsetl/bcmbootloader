/*
 * $Copyright Open Broadcom Corporation$
 */


#include <common.h>
#include <post.h>
#include <iproc_i2s.h>
#include <iproc_gsio.h>

#if CONFIG_POST & CONFIG_SYS_POST_I2S_W8955

int I2S__w8955_post_test (int flags)
{
    int status = 0;
    int ret=0;
    u32 sel; u64 i;


#if (defined(CONFIG_NS_PLUS))

    if ( post_get_board_diags_type() & HR_ER_BOARDS )
	{
		printf("\nThis diag is not supported on this platform\n ");
		return 2;
	}
#endif


    post_log("\nI2S - WM8955 diags starts  !!!");

    if ( (flags & POST_SEMI_AUTO) !=  POST_SEMI_AUTO )
    {

        post_log("\nSet strap pin: A2=0, A3=0x3, A4=0xf, A5=0x28, A6=0xf8", 0);

    }
    else
    {
        post_set_strappins("A2 0%A3 3%A4 f%A5 38%A6 f8%");
    }
    post_log("\nPlug in the audio plack device into wm8955 audio jack");
    post_getConfirmation("\nReady? (Y/N)");

#if (defined(CONFIG_NS_PLUS))
    post_log("\nEnable On-Chip I2S Audio PLL");
    iproc_i2s_enable_pll();
#else
    post_log("\nUse external PLL");
    post_log("\nmake sure the internal XTAL clk is connected (J3204 pin 2 and 3)");
    post_getConfirmation("\nReady? (Y/N)");
#endif
    
    WM8955_SPI_Enable();

	// Reset the codec
    WM8955_reset_codec();

    WM8955_poweron_channels();


    WM8955_configure_sigal_path();

    WM8955_configure_volume();

    WM8955_configure_format_mode();

    iproc_i2s_DMA_tx();

    //Wait here until get a Y/N
    status = post_getUserResponse("\nSound output OK? (Y/N)");

    //iproc_i2s_DMA_stop();

    if ( -1 == status )
    {
       post_log("\nTest Failed !");
    }
    else
    {
       post_log("\nTest Passed !");
    }

    post_log("\n\nI2S test done\n");
    return status;

failed:
    post_log("\nTest Failed !");
    return -1;
}

#if BEFORE_CAPTUR
int I2S__w8955_post_test (int flags)
{
    int status = 0;
    int ret=0;
    u32 sel; u64 i;

    post_log("\nI2S - WM8955 diags starts  !!!");
    post_log("\nSet strap pin: A2=0, A3=0x3, A4=0xf, A5=0x28, A6=0xf8", 0);
    post_log("\nmake sure the internal XTAL clk is connected (J3204 pin 2 and 3)");
    post_getConfirmation("\nReady? (Y/N)");

    if ( cca_spi_init() != SPI_ERR_NONE )
    {
        post_log("\nSPI not present, can't configure WM8750\n");
        goto failed;
    }

    // Reset the codec
    status = WM8955_write(WM8955_RESET, 0);

    if(status < 0)
    {
        post_log("\nWrite to WM8750 failed\n");
        goto failed;
    }
    udelay(100*1000);


    //Left Mixer 1, Left DAC to Left Mixer
    gsio_write_wm8955(WM8955_L_MIXER_1, 0x100);

    udelay(100*1000);
    //Right Mixer 2, Right DAC to Right Mixer
    gsio_write_wm8955(WM8955_R_MIXER_2, 0x100);

    udelay(100*1000);
    //Enable both DACS and mixers
    gsio_write_wm8955(WM8955_PWR2, 0x1f8);

    udelay(100*1000);
    //Enable 50Ohm divider
    WM8955_write(WM8955_PWR1, 0x80);

    udelay(100*1000);
    //Unmute DAC
    gsio_write_wm8955(WM8955_DAC_CTRL, 0x0);

    udelay(100*1000);
    // Normal mode ( CLK )
    // SR =  01010    16Khz sample rate
    // SR = 00000 48Khz
    //MCLK/2 enabled
    //gsio_write_wm8955(WM8955_SRATE_CTRL, 0x54);

    //SRATE 48Khz
    gsio_write_wm8955(WM8955_SRATE_CTRL, 0x40);  //?????


    udelay(100*1000);

    //CLKOUT pin enable, source of CLKOUT, MCLK
    //Dont enable CLKOUT for now. there is a wire supplues the clk ?
    //gsio_write_wm8955(CLK_PLL_CTRL, 0x80);

    //I2S format, 16 bit data, codec in slave mode
    gsio_write_wm8955(AUDIO_INT_CTRL, 0x2);


    udelay(100*1000);
    // Enable headphone switch
    //gsio_write_wm8955(0x18, (0x3 << 5));


    /* Send sincwave through I2S */
    //post_getConfirmation("\nReady to play loop back (Y/N)");

    //Wait here until get a Y/N
    //post_getUserResponse("\nSound output OK? (Y/N)");


    /* Configure I2S */
    iproc_i2s_init(1);

    iproc_i2s_DMA_init();


    /* Send sincwave through I2S */
    post_getConfirmation("\nReady to stream? (Y/N)");

    iproc_i2s_DMA_start();

    //Wait here until get a Y/N
    status = post_getUserResponse("\nSound output OK? (Y/N)");

    iproc_i2s_DMA_stop();

    if ( -1 == status )
    {
       post_log("\nTest Failed !");
    }
    else
    {
       post_log("\nTest Passed !");
    }

    post_log("\n\nI2S test done\n");
    return 0;

failed:
    post_log("\nTest Failed !");
    return -1;
}
#endif
#if 0 //disable it for now
int I2S_post_test (int flags)
{
    int status = 0;
    int ret=0;
    u32 sel; u64 i;

    post_log("\nI2S - WM8955 diags starts  !!!");
    post_log("\nSet strap pin: A2=0, A3=0x3, A4=0xf, A5=0x28, A6=0xf0", 0);
    post_log("\nmake sure the 24MHz XTAL is connected (J3204 pin 1 and 2)");
    post_getConfirmation("\nReady? (Y/N)");

    if ( cca_spi_init() != SPI_ERR_NONE )
    {
        post_log("\nSPI not present, can't configure WM8955\n");
        goto failed;
    }

	// Reset the codec
//	gsio_write_wm8955(WM8955_RESET, 0);
	gsio_write_wm8955(0xf, 0);
	udelay(100*1000);

	gsio_write_wm8955(0x22, 0x100);
	gsio_write_wm8955(0x25, 0x100);

	gsio_write_wm8955(0x1a, 0x1f8);
	gsio_write_wm8955(0x19, 0x80);
	gsio_write_wm8955(0x5, 0x0);

	gsio_write_wm8955(0x8, 0x54);
	gsio_write_wm8955(0x2b, 0x80);

	gsio_write_wm8955(0x7, 0x2);


#if 1
    /* Master */
    //status = gsio_write_wm8955 (AUDIO_INTERFACE_FORMAT_REG, MASTER_MODE | I2S_FORMAT | AUDIO_DATA_16bit);

	//Put the codec in slave mode
	status = gsio_write_wm8955 (AUDIO_INTERFACE_FORMAT_REG,  (0 << 6)|I2S_FORMAT | AUDIO_DATA_16bit);
    status = gsio_write_wm8955 (CLK_PLL_REG,CLK_OUT_ENABLE );

    post_log("\n\nSelect WM8955 mclk: 1 - Normal-Div2; 2 - Normal-Div4; 3 - External-24MHz\n"); 
    sel = post_getUserInput("Mode? (1/2/3) : ");
   
    /* External 24Mhz */
    if(sel == 3)
    {
        status = gsio_write_wm8955 (SAMPLE_RATE_REG, USB_MODE | MCLK_DIV_BY_2);
    }
    else     /* PLL Get MCLK = 12M clock, output 12.288M clock */
    {
        post_log("\n Enable PLL \n");
        status = gsio_write_wm8955 (CLK_PLL_REG, CLK_FROM_PLL|CLK_OUT_ENABLE|PLL_OUT_DIV_BY_2 |PLL_ENABLE );

        printf("\n\nPLL_Ctrl_1 = %04x\n", (8<<5) | PLL_FRA_2);
        status = gsio_write_wm8955 (0x2c, (8<<5) | PLL_FRA_2);

        printf("\n\nPLL_Ctrl_2 = %04x\n", PLL_FRA_1);
        status = gsio_write_wm8955 (0x2d, PLL_FRA_1);

        printf("\n\nPLL_Ctrl_3 = %04x\n", PLL_FRA_0);
        status = gsio_write_wm8955 (0x2e, PLL_FRA_0);

        status = gsio_write_wm8955 (0x3b, (1<<7) );

        if(sel == 1)
            status = gsio_write_wm8955 (SAMPLE_RATE_REG, MCLK_DIV_BY_2 | NORMAL_MODE);
        else if(sel == 2)
            status = gsio_write_wm8955 (SAMPLE_RATE_REG, MCLK_DIV_BY_2 | CLKOUT_DIV_BY_2 | NORMAL_MODE);
    }

    /* Program WM8955 through SPI */
    /* Increase gain */
    status = gsio_write_wm8955 (LOUT2_OUTPUT_REG,MAX_6_DB | VOLUME_UPDATE | zero_cross_enable );
    status = gsio_write_wm8955 (ROUT2_OUTPUT_REG,MAX_6_DB | VOLUME_UPDATE | zero_cross_enable );     

    /* 50K Divider */
    status = gsio_write_wm8955 (0x19, (1<<7)  );

    /* Mixer */
    printf("\n\nMixer Config\n");
    status = gsio_write_wm8955 (MIXER_LEFT_1, (1<<8) );        
    status = gsio_write_wm8955 (MIXER_RIGHT_2, (1<<8) );        

    status = gsio_write_wm8955 (POWER_MANAGEMENT_REG2, DAC_LEFT_ON | DAC_RIGHT_ON | LOUT2_BUF_ON |ROUT2_BUF_ON | (0x3<<5));

    status = gsio_write_wm8955 (DAC_CONTROL_REG, 0 );       // No Mute, No de-emphasize

    /* post_log("\n\nSelect WM8955 LR2 Invert: 1 - No; 2 - Yes\n");
    sel = post_getUserInput("Invert? (1/2) : ");
    if(sel == 2)
        status = gsio_write_wm8955 (0x17, (1<<1) | (3<<6) );            
    */
#endif

    if(status < 0) 
    {
        post_log("\nWrite to WM8955 failed\n");
        goto failed;
    }
 
    /* Configure I2S */
    iproc_i2s_init();

    iproc_i2s_DMA_init();
#if 0
    iproc_i2s_tx_test();

#endif

    /* Send sincwave through I2S */
    post_getConfirmation("\nReady to stream? (Y/N)");

    iproc_i2s_DMA_start();


#if 0
    iproc_i2s_play();

    /* Refill Tx Fifo */
    for(i=0; i<300000000; i++)
        iproc_i2s_refill_fifo();
#endif

    //Wait here until get a Y/N
    status = post_getUserResponse("\nSound output OK? (Y/N)");

    iproc_i2s_DMA_stop();

    if ( -1 == status )
    {
       post_log("\nTest Failed !");
    }
    else
    {
       post_log("\nTest Passed !");
    }

    post_log("\n\nI2S test done\n");
    return 0;

failed:
    post_log("\nTest Failed !");
    return -1;    	 
}
#endif

#endif /* CONFIG_POST & CONFIG_SYS_POST_I2S_W8955 */
