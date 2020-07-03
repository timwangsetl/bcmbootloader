/*
 * $Copyright Open Broadcom Corporation$
 */


#include <common.h>
#include <post.h>
//#include <iproc_slic.h>


#if CONFIG_POST & CONFIG_SYS_POST_VOIP

int voip_post_test (int flags)
{
    int status=0;

    post_log("\nvoip diags starts  !!!");
#if (defined(CONFIG_NS_PLUS))
    // Not supported boards
    if ( post_get_board_diags_type() & (BCM958522ER | BCM958625HR ))
	{
		printf("\nThis diag is not supported on this platform\n ");
		return 2;
	}

	if (post_get_board_diags_type() &  SVK_BOARDS )
	{
		post_log("\nStrap pins are set to A1:FC  A2:00  A3:07  A4:0f  A5:28  A6:f1  A7:fff");
		post_log("\nConnect a phone to J3401 port marked B1");//A1
		post_getConfirmation("\nReady? (Y/N)");
	}
#else
    post_log("\nStrap pins are set to A1:FC  A2:00  A3:07  A4:0f  A5:28  A6:f1  A7:fff");
    post_log("\nConnect a phone to J3401 port marked B1");//A1
    post_getConfirmation("\nReady? (Y/N)");
#endif

#ifdef CONFIG_NO_SLIC
    post_log("\nSLIC library is not build in. ERROR. Failed.");//A1
    return -1;
#else


    if ( iproc_slic_device_init() == -1 )
    {
        post_log("\nvoip diags failed init  !!!");
        return -1;
    }

    iproc_slic_play_test_tone(0);//channel == 0
#endif


    status = post_getUserResponse("\ndial tone output OK? (Y/N)");

    if ( -1 == status )
    {
        post_log("\nvoip diags failed tone generation test !!!");
        return -1;
    }


    //Start PCM playback

    //iproc_slic_pcm_play();

    //Start the TDM module to play a test tone through PCM
    //iproc_PCM_DMA_tx();

    //status = post_getUserResponse("\nsound output OK? (Y/N)");

    //if ( -1 == status )
    //{
    //    post_log("\nvoip diags failed sound playback test !!!");
    //    return -1;
    //}

    return status;
}
#endif /* CONFIG_POST & CONFIG_SYS_POST_VOIP */
