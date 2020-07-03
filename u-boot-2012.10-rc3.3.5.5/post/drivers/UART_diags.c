/*
 * $Copyright Open Broadcom Corporation$
 */

#include <common.h>
#include <post.h>

#if CONFIG_POST & CONFIG_SYS_POST_UART
#include "asm/arch/socregs.h"

extern void _serial_enter_loopback( const int port );
extern void _serial_exit_loopback( const int port );
extern void iproc_uart_iomux(int op);


static int UART_post_testLoopback(int uartIndex )
{
    char testChar[]= "Hello";
    volatile char *p;
    char t;
    int ret =0;

    p = testChar;
    _serial_enter_loopback(uartIndex);
    
    while(*p)
    {
        _serial_putc_raw(*p,uartIndex);     
#if defined(CONFIG_CYGNUS)
        udelay(10000);
        if (!_serial_tstc(uartIndex)) 
        { 
            ret = -1;
            break;
        }
#endif
        t = _serial_getc(uartIndex);
        if ( t!=*p)
        {
            post_log("\nExpected: %c(0x%2x), Actual:  %c(0x%2x) ", *p,*p, t, t);
            ret = -1;
            break;
        }
        p++;
    }
    _serial_exit_loopback(uartIndex);
    return ret;

}

static void UART_post_init(void)
{
#if defined(CONFIG_CYGNUS)
    iproc_uart_iomux(1);
#endif
}
static void UART_post_end(void)
{
#if defined(CONFIG_CYGNUS)
    iproc_uart_iomux(0);
#endif
}
static int UART_post_TR_test(int uartIndex )
{
    
    unsigned long counter = 0;
    unsigned long timeout = 0;
    char ch;

    post_log("\nPlease connect terminals to console %d !", uartIndex);
    post_log("\nPlease make sure the console is configured as :");
    post_log("\nBaud 115200\n8 bit data 1 bit stop\nno parity, no HW flow control");
    post_log("\nReady to test (Y): ");

    //Get 'Y' from the console to continue the test

    do
    {
        ch = (char)serial_getc();
    }while ( (ch != 'y') && ( ch!='Y'));


    post_log("\nSend test string <can you see me now? press 'Y' to continue> to console %d", uartIndex);
    //Send test string to the console under test
    _serial_puts ("\ncan you see me now?press 'Y' to continue",uartIndex);
    counter = 0;

    do
    {
        counter++;
        if ( counter == 0xFFFFF )
        {
            post_log("\nWaiting on response from console %d",uartIndex);
            counter =0 ;
            timeout++;
            if ( timeout > 0xFFFF )
            {
                break;
            }
        }
        if (_serial_tstc(uartIndex) != 0 )
        {
            ch = (char)_serial_getc(uartIndex);
            if ( ('y' == ch) || ('Y'==ch ))
            {
                break;
            }
        }

    }while (1 );

    if ( timeout > 0xFFFF )
    {
        post_log("\nReceived no response from console %d",uartIndex );
        return -1;
    }
    post_log("\nReceived response from console %d",uartIndex );
    return 1;

}


#if defined(CONFIG_CYGNUS)

static void drain_out(void)
{
    int idx;
    for (idx = CONFIG_UART0_INDEX; idx < CONFIG_UART4_INDEX; idx++)
    {
        while (_serial_tstc(idx))
        {
            _serial_getc(idx);
        }
    }   
}

int UART_post_outer_lp_test(int uartIndex, int *rxIdx)
{
    char testChar[]= "Hello";
    volatile char *p;
    char t;    
    int idx;
    int find_dest = 0xff;

    p = testChar;
    post_log("\n");
    while(*p)
    {
        _serial_putc_raw(*p,uartIndex);
        udelay(1000);
        if (find_dest != 0xff)
        {
           if (!_serial_tstc(find_dest)) {
               post_log("can't get char\n");
               return -1;
           }                     
           t = _serial_getc(find_dest); 
           //post_log("idx:%d,get char :%x\n", find_dest, t);
           if ( t != (*p)) 
           { 
              return -1;
           }
        } else {
            for (idx = CONFIG_UART0_INDEX; idx < CONFIG_UART4_INDEX; idx++)
            {
                if ((idx == CONFIG_UART3_INDEX) || (idx == uartIndex))
                      continue;
                if (!_serial_tstc(idx))
                      continue; 
                t = _serial_getc(idx);
                //post_log("idx:%d,get char :%x\n", idx, t);
                if ( t!=*p) {
                    continue;
                } else {
                    find_dest = idx;
                    * rxIdx = idx;
                    break;
                }
            }
            if (find_dest == 0xff) 
            { 
                return -1;
            }
        }
        p++;
    }
    return 1;
}

int UART_post_test (int flags)
{
    int ret,tot_test = 1;
    int i;
    char ch;
    int rxIdx;

    post_log("\nUART diags starts  !");

    UART_post_init();

    post_log("\nUART loopback test..."); 
    
    for ( i = CONFIG_UART0_INDEX; i <= CONFIG_UART4_INDEX; i++)
    {
        /*use UART3 as the default console*/
        if (i == CONFIG_UART3_INDEX)
        {
            continue;
        }
#if 1
        if (i == CONFIG_UART4_INDEX) 
        {
            /*some issues for uart4 on svk*/
            _serial_exit_loopback(CONFIG_UART4_INDEX);  
            udelay(10000);  
        }
#endif
        drain_out();
        ret = UART_post_testLoopback(i);        
        if ( 0 == ret )
        {
            post_log("\nUART %d Loopback test successful!",i);
        }
        else
        {
            post_log("\nUART %d Loopback test failed!",i);
            tot_test = -1;
        }
    }

#if 1
     /*some issues for uart4 on svk*/
    _serial_exit_loopback(CONFIG_UART4_INDEX);  
    udelay(10000);  
#endif
    drain_out();

    post_log("\nwill you select outer loopback test (Y/N): ");
    do 
    {
        ch = (char)serial_getc();
    } while ((ch !='y') && (ch !='Y') && (ch !='N') && (ch !='n'));

    if ((ch == 'y') || ( ch =='Y')) {  
            
        for ( i = CONFIG_UART0_INDEX; i <= CONFIG_UART4_INDEX; i++)
        {
            /*use UART3 as the default console*/
            if (i == CONFIG_UART3_INDEX)
            {
                continue;
            }
            drain_out();
            ret = UART_post_outer_lp_test(i, &rxIdx);    
            if ( 1 == ret )
            {
                post_log("\nUART %d outer loopback test (%d->%d) successful!",i, i, rxIdx);
            }
            else
            {
                post_log("\nUART %d outer loopback test failed!",i);
                tot_test = -1;
            }
        }

    } else {
        for ( i = CONFIG_UART0_INDEX; i <= CONFIG_UART4_INDEX; i++)
        {
            /*use UART3 as the default console*/
            if (i == CONFIG_UART3_INDEX)
            {
                continue;
            }

            ret = UART_post_TR_test(i);
    
            if ( 1 == ret )
            {
                post_log("\nUART %d TX/RX test successful!",i);
            }
            else
            {
                post_log("\nUART %d TX/RX test failed!",i);
                tot_test = -1;
            }
        }
    }
    UART_post_end();
    if (1 == tot_test)
        post_log("\nSuccessful, UART diags done  !\n");
    else {
        post_log("\nFailed, UART diags done  !\n");
        return -1;
    }
    return 0;
}
#else

int UART_post_test (int flags)
{
    int ret;
    int i;

#if (defined(CONFIG_NS_PLUS))

    if ( post_get_board_diags_type() & HR_ER_BOARDS )
    {
        printf("\nThis diag is not supported on this platform\n ");
        return 2;
    }
#endif

    post_log("\nPlease set strap pin A4 to 0x18, A5 to 0x18");
    post_getConfirmation("\nReady? (Y/N)");


    post_log("\nUART diags starts  !");

    UART_post_init();

    post_log("\nUART loopback test...");

#if defined(CONFIG_SABER2)
    for ( i = CONFIG_UART1_INDEX; i <= CONFIG_UART3_INDEX; i++)
#else
    for ( i = CONFIG_UART1_INDEX; i <= CONFIG_UART2_INDEX; i++)
#endif
    {
        ret = UART_post_testLoopback(i);

        if ( 0 == ret )
        {
            post_log("\nUART %d Loopback test successful!",i-1);
        }
        else
        {
            post_log("\nUART %d Loopback test failed!",i-1);
        }
    }
	
#if (defined(CONFIG_SABER2))
    for ( i = CONFIG_UART0_INDEX; i <= CONFIG_UART0_INDEX; i++)
#else
    for ( i = CONFIG_UART1_INDEX; i <= CONFIG_UART2_INDEX; i++)
#endif
    {
        ret = UART_post_TR_test(i);

        if ( 1 == ret )
        {
            post_log("\nUART %d TX/RX test successful!",i-1);
        }
        else
        {
            post_log("\nUART %d TX/RX test failed!",i-1);
        }
    }
    UART_post_end();
    post_log("\nSuccessful, UART diags done  !\n");
    return 0;
}
#endif

#endif /* CONFIG_POST & CONFIG_SYS_POST_QSPI */


