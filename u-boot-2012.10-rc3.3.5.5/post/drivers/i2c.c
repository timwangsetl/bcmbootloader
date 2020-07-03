/*
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * I2C test
 *
 * For verifying the I2C bus, a full I2C bus scanning is performed.
 *
 * #ifdef CONFIG_SYS_POST_I2C_ADDRS
 *   The test is considered as passed if all the devices and only the devices
 *   in the list are found.
 *   #ifdef CONFIG_SYS_POST_I2C_IGNORES
 *     Ignore devices listed in CONFIG_SYS_POST_I2C_IGNORES.  These devices
 *     are optional or not vital to board functionality.
 *   #endif
 * #else [ ! CONFIG_SYS_POST_I2C_ADDRS ]
 *   The test is considered as passed if any I2C device is found.
 * #endif
 */

#include <common.h>
#include <post.h>
#include <i2c.h>

#if CONFIG_POST & CONFIG_SYS_POST_I2C

#if (defined(CONFIG_NS_PLUS))
#define SHADOW_CPY_BUFFER  0x70000000
#elif (defined(CONFIG_CYGNUS))
static u32 SHADOW_CPY_BUFFER = 0x70000000;
#else
#define SHADOW_CPY_BUFFER  0x89000000
#endif
#if 0
static int i2c_ignore_device(unsigned int chip)
{
#ifdef CONFIG_SYS_POST_I2C_IGNORES
	const unsigned char i2c_ignore_list[] = CONFIG_SYS_POST_I2C_IGNORES;
	int i;

	for (i = 0; i < sizeof(i2c_ignore_list); i++)
		if (i2c_ignore_list[i] == chip)
			return 1;
#endif

	return 0;
}
#endif
extern int do_i2c(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[]);
int i2c_post_test (int flags)
{
#if 1
        char argv_buf[7*10];
        char * test_argv[7];
        int  i =0,j = 0;
        u32  addr_off;
        int ret = 0;

#if (defined(CONFIG_NS_PLUS))

    if ( post_get_board_diags_type() & HR_ER_BOARDS )
	{
		printf("\nThis diag is not supported on this platform\n ");
		return 2;
	}
#endif

		printf("\nRun diags: Start I2C test\n");
        memset(&argv_buf, 0, 7*10);
        for(i=0; i<7; i++)
		test_argv[i] = &argv_buf[i*10];

#ifndef CONFIG_CYGNUS
        if ( (flags & POST_AUTO) !=  POST_AUTO )
        {
            post_log("\nPlease set strap pin: A2=0, A3=0x3, A4=0xf, A5=0x4f, A6=0xf3", 0);
            post_getConfirmation("\nReady? (Y/N)");
        }
        else
        {
            post_set_strappins("A2 0%A3 3%A4 f%A5 5f%A6 f3%");
        }
#endif
        printf("\n\nI2C Reset & Init\n");
        strcpy(test_argv[0], "i2c");
        strcpy(test_argv[1], "reset");
        do_i2c(0, 0, 2, test_argv);
	udelay(200000);
#if defined(CONFIG_CYGNUS)

        for (i = 0 ; i < CONFIG_SYS_MAX_I2C_BUS; i++) 
        {
        	i2c_set_bus_num(i);
        	switch (i)
        	{
        		case 0: 
        			SHADOW_CPY_BUFFER     = 0x70000000;
        		    break;
        		case 1: 
        			SHADOW_CPY_BUFFER     = 0x70100000;
        		    break;
        		default:
        			printf("Bad i2c bus: %d\n", i);
        			return 0;
        			break;        		   
        	}

        addr_off = 3;
#else
        addr_off = 0;
#endif
        printf("\n\nI2C Write (memory fill) -- i2c mw chip address[.0, .1, .2] value [count]\n");
        printf("I2C Write -- i2c mw %2x 10.1 68 10, i2c_addr=0x%2x, mem_addr=0x10, value=0x68, count=0x10\n", 0x50+addr_off, 0x50+addr_off);
        strcpy(test_argv[0], "i2c");
        strcpy(test_argv[1], "mw");
        sprintf(test_argv[2], "%2x", 0x50+addr_off);
        strcpy(test_argv[3], "10.1");
        strcpy(test_argv[4], "68");
        strcpy(test_argv[5], "10");
        do_i2c(0, 0, 6, test_argv);
		udelay(20000);
#if defined(CONFIG_CYGNUS)
        i2c_set_bus_num(i);
#endif
        printf("\n\nI2C Read -- i2c md chip address[.0, .1, .2] [# of objects]\n");
        printf("I2C Read -- i2c md %2x 10.1 10, i2c_addr=0x%2x, mem_addr=0x10, count=0x10\n\n", 0x50+addr_off, 0x50+addr_off);
        strcpy(test_argv[0], "i2c");
        strcpy(test_argv[1], "md");
        sprintf(test_argv[2], "%2x", 0x50+addr_off);
        strcpy(test_argv[3], "10.1");
        strcpy(test_argv[4], "10");
        do_i2c(0, 0, 5, test_argv);
		udelay(20000);

		for ( j = 0; j< 0x10; j++ )
		{
		    if ( *((unsigned char*)SHADOW_CPY_BUFFER+j) != 0x68 )
		    {
		        post_log("\"Failed, at location 0x%x, data ", SHADOW_CPY_BUFFER+ j, *((unsigned char*)SHADOW_CPY_BUFFER+j));	
			post_log("\ni2c bus %d test failed\n", i);
		        ret = -1;
                        break;
		    }
		}
                if (j >= 0x10) { post_log("\ni2c bus %d test passed\n", i);}
#if defined(CONFIG_CYGNUS)
        }
#endif
	return ret;

#else
	unsigned int i;
#ifndef CONFIG_SYS_POST_I2C_ADDRS
	/* Start at address 1, address 0 is the general call address */
	for (i = 1; i < 128; i++) {
		if (i2c_ignore_device(i))
			continue;
		if (i2c_probe (i) == 0)
			return 0;
	}

	/* No devices found */
	return -1;
#else
	unsigned int ret  = 0;
	int j;
	unsigned char i2c_addr_list[] = CONFIG_SYS_POST_I2C_ADDRS;

	/* Start at address 1, address 0 is the general call address */
	for (i = 1; i < 128; i++) {
		if (i2c_ignore_device(i))
			continue;
		if (i2c_probe(i) != 0)
			continue;

		for (j = 0; j < sizeof(i2c_addr_list); ++j) {
			if (i == i2c_addr_list[j]) {
				i2c_addr_list[j] = 0xff;
				break;
			}
		}

		if (j == sizeof(i2c_addr_list)) {
			ret = -1;
			post_log("I2C: addr %02x not expected\n", i);
		}
	}

	for (i = 0; i < sizeof(i2c_addr_list); ++i) {
		if (i2c_addr_list[i] == 0xff)
			continue;
		post_log("I2C: addr %02x did not respond\n", i2c_addr_list[i]);
		ret = -1;
	}

	return ret;
#endif // CONFIG_SYS_POST_I2C_ADDRS

#endif //
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_I2C */
