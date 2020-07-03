/*
 * $Copyright Open Broadcom Corporation$
 */


#include <common.h>
#include <post.h>
#include <iproc_i2s.h>
#include <iproc_gsio.h>

#if CONFIG_POST & CONFIG_SYS_POST_SPDIF

int spdif_post_test (int flags)
{
    int status = 0;
#if (defined(CONFIG_NS_PLUS))

    if ( post_get_board_diags_type() & HR_ER_BOARDS )
	{
		printf("\nThis diag is not supported on this platform\n ");
		return 2;
	}
#endif

    post_log("\nSPDIF diags starts  !!!");


    if ( (flags & POST_SEMI_AUTO) !=  POST_SEMI_AUTO )
    {
        post_log("\nSet strap pin: A2=0, A3=0x3, A4=0xf, A5=0x28, A6=0xf4", 0);
        post_getConfirmation("\nReady? (Y/N)");

    }
    else
    {
        post_set_strappins("A2 0%A3 3%A4 f%A5 38%A6 f4%");
    }



    if ( WM8750_SPI_Enable() != 0 )
    {
        post_log("\nSPI not present, can't configure WM8750\n");
        goto failed;
    }

    // Reset the codec
    WM8750_reset_codec();

    WM8750_poweron_channels();

    WM8750_configure_sigal_path();

    WM8750_configure_volume();

    WM8750_configure_format_mode();

    post_log("\nConnect an audio input device to the upper port of the J3301 and start an audio playback\n ");
    post_log(" Connect an audio output device to the top optical output port(J3208) with a SPDIF cable,\n ");
    post_getConfirmation("\nReady? (Y/N)");

    iproc_i2s_recording_start();


    post_log("\nAudio is recorded. start playback");

    //Disable the codec from playback recording data
    WM8750_disable_ADC_data();


    iproc_i2s_playback_recorded_audio();


    status = post_getUserResponse("\nSound output OK? (Y/N)");

    if ( -1 == status )
    {
       post_log("\nTest Failed !");
    }
    else
    {
       post_log("\nTest Passed !");
    }

    post_log("\n\nI2S test done\n");

    //I2S__w8955_post_test(0);
    return status;

failed:
    post_log("\nTest Failed !");
    return -1;
}




#endif /* CONFIG_POST & CONFIG_SYS_POST_SPDIF */
