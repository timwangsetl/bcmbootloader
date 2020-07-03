/*
 * $Copyright Open Broadcom Corporation$
 */

#include <common.h>
#include <post.h>

#if defined(CONFIG_CYGNUS)
#if CONFIG_POST & CONFIG_SYS_POST_SRAM

#include "asm/arch/socregs.h"
#include "../../halapis/include/reg_access.h"


volatile uint32_t *sram_start_address;
volatile uint32_t *addr = (uint32_t*)0xE8000000;
volatile uint32_t read_value;
extern int do_mem_md ( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);

int SRAM_post_test(int flags)
{

	 char * cmd_argv[7];
	uint32_t error_cnt,i;

	error_cnt = 0;
	sram_start_address = (uint32_t *)0xE8000000;

    post_log("Entering the External SRAM access test \n");
    cpu_wr_single(0x301d0bc,0,4);

    cpu_wr_single(0x301d0d8,0x103330,4);

    cpu_wr_single(0x301d0e4,0x120,4);

    cpu_wr_single(0x18000a50,0xFCE8,4);

    cpu_wr_single(0x18000a54,0,4);

    cpu_wr_single(0x18000a58,0,4);

    cpu_wr_single(0x18045014,0x25143,4);

    cpu_wr_single(0x18045018,0,4);

    cpu_wr_single(0x18045010,0x400000,4);

    post_log("=============================================================\n");
    post_log("Writing the Entire 512 KB SRAM with pattern 0xAAAAAAAA \n");
    for(addr = sram_start_address; addr < (uint32_t *)0xE8080000; addr++) //0xEBFFFFFC
        {
        	*addr = 0xAAAAAAAA;
        	read_value = *addr;
        	read_value = 0;
        	cpu_wr_single((uint32_t)addr,0xAAAAAAAA,4);
        	if (addr == sram_start_address)
        	{
        	cmd_argv[0] = "md";
        	    	sprintf(cmd_argv[1], "0x%x ", addr);
        	    	do_mem_md(0, 0, 2, cmd_argv);
        	}
        }

    post_log("External SRAM Write Done With Pattern 0xAAAAAAAA\n");
    addr = sram_start_address;
    if (addr == sram_start_address)
            	{
            	cmd_argv[0] = "md";
            	    	sprintf(cmd_argv[1], "0x%x ", addr);
            	    	do_mem_md(0, 0, 2, cmd_argv);
            	}
    for(i=0;i<800000000;i++);
    for(i=0;i<800000000;i++);
    for(i=0;i<800000000;i++);


    post_log("Reading the Entire 512 KB SRAM AND LOOKING FOR PATTERN 0xAAAAAAAA \n");
    for(addr = sram_start_address; addr < (uint32_t *)0xE8080000; addr++) //0xEBFFFFFC
            {


        	read_value = cpu_rd_single((uint32_t)addr,4);
        	if (read_value != 0xAAAAAAAA)
        	{
        		error_cnt++;
        		post_log("Error occurred\n");
        	    break;
        	}
        }

     //post_log("Pattern 0xAAAAAAAA for Entire SRAM PASSES \n");
     for(i=0;i<800000000;i++);
         for(i=0;i<800000000;i++);
         for(i=0;i<800000000;i++);
     post_log("=============================================================\n");
     post_log("=============================================================\n");
    post_log("Writing the Entire 512 KB SRAM with pattern 0x55555555 \n");
    for(addr = sram_start_address; addr < (uint32_t *)0xE8080000; addr++) //0xEBFFFFFC
        {
        	*addr = 0x55555555;
        	read_value = *addr;
        	read_value = 0;
        	cpu_wr_single((uint32_t)addr,0x55555555,4);
        	if (addr == sram_start_address)
        	{
        	cmd_argv[0] = "md";
        	    	sprintf(cmd_argv[1], "0x%x ", addr);
        	    	do_mem_md(0, 0, 2, cmd_argv);
        	}
        }

    post_log("External SRAM Write Done With Pattern 0x55555555 \n");
    addr = sram_start_address;
        if (addr == sram_start_address)
                	{
                	cmd_argv[0] = "md";
                	    	sprintf(cmd_argv[1], "0x%x ", addr);
                	    	do_mem_md(0, 0, 2, cmd_argv);
                	}
        for(i=0;i<800000000;i++);
            for(i=0;i<800000000;i++);
            for(i=0;i<800000000;i++);
    post_log("Reading the Entire 512 KB SRAM AND LOOKING FOR PATTERN 0x55555555 \n");
    for(addr = sram_start_address; addr < (uint32_t *)0xE8080000; addr++) //0xEBFFFFFC
            {


        	read_value = cpu_rd_single((uint32_t)addr,4);
        	if (read_value != 0x55555555)
        	{
        		error_cnt++;
        		post_log("Error occurred\n");
        	    break;
        	}
        }
    //for(i=0;i<800000000;i++);
     //       for(i=0;i<800000000;i++);
     //       for(i=0;i<800000000;i++);
    //post_log("Pattern 0x55555555 for Entire SRAM PASSES \n");
    for(i=0;i<800000000;i++);
        for(i=0;i<800000000;i++);
        for(i=0;i<800000000;i++);
    post_log("=============================================================\n");
    post_log("=============================================================\n");
    post_log("Writing the Entire 512 KB SRAM with pattern the respective ADDRESS Locations \n");
    for(addr = sram_start_address; addr < (uint32_t *)0xE8080000; addr++) //0xEBFFFFFC
    {
        *addr = (uint32_t)addr; //0xAAAAAAAA
    	read_value = *addr;;
    	read_value = 0;
    	cpu_wr_single((uint32_t)addr,*addr,4);
    	if (addr == sram_start_address)
    	{
    	cmd_argv[0] = "md";
    	    	sprintf(cmd_argv[1], "0x%x ", addr);
    	    	do_mem_md(0, 0, 2, cmd_argv);
    	}
    }
    post_log("External SRAM Write Done With Respective Addresses\n");
    addr = sram_start_address;
        if (addr == sram_start_address)
                	{
                	cmd_argv[0] = "md";
                	    	sprintf(cmd_argv[1], "0x%x ", addr);
                	    	do_mem_md(0, 0, 2, cmd_argv);
                	}
        for(i=0;i<800000000;i++);
            for(i=0;i<800000000;i++);
            for(i=0;i<800000000;i++);
    post_log("Reading the Entire 512 KB SRAM AND LOOKING FOR RESPECTIVE ADDRESSES \n");
    for(addr = sram_start_address; addr < (uint32_t *)0xE8080000; addr++) //0xEBFFFFFC
        {


    	read_value = cpu_rd_single((uint32_t)addr,4);
    	if (read_value != *addr)
    	{
    		error_cnt++;
    		post_log("Error occurred While Reading Back SRAM contents With Respective Addresses\n");
    	    break;
    	}
    }

    post_log("=============================================================\n");
    post_log("=============================================================\n");
    //post_log("Total ERROR COUNT : %d \n",error_cnt);
    if (error_cnt == 0)
    {
    	post_log("\nEXTERNAL SRAM ACCESS TEST : PASSED\n");
    	 post_log("=============================================================\n");
    	return 0;
    }
    else
    {
    	post_log("\nEXTERNAL SRAM ACCESS TEST : FAILED\n");
    	 post_log("=============================================================\n");
    	    	return -1;
    }
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_SCI */
#endif /* CONFIG_CYGNUS*/
