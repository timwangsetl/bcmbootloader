/* * (C) Copyright 2002
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
#include <common.h>
#include <post.h>


#if defined(CONFIG_CYGNUS)
#if CONFIG_POST & CONFIG_SYS_POST_BBL

#include "../../halapis/include/bbl_apis.h"
#include "asm/arch/socregs.h"
#include "../../halapis/include/dmu.h"

/*SHA256 includes*/
#include "../../halapis/include/bcm_scapi.h"

#define ADD_TEST(X)  { #X , X }
#define TEST_PASS  0
#define TEST_FAIL -1
#define TEST_SKIP  2
#define UART_LSR_DR  0x01            /* Data ready */



typedef struct{
    
    char* func_name;
    int (*func)(void);
}FUNC_INFO;
void bbl_diag_delay(volatile uint32_t d)
{
    while(d--);
}



/******* Internal test function start ********/

/* Initialize SEC1280 to known pattern */
int bbl_sec1280_wr(void)
{
  volatile uint32_t loop_count    = 0x00;

  uint32_t wr_data  = 0x11111111;
  uint32_t wr_data1 = 0x1111;
  uint32_t wr_data2 = 0x11110000;
  volatile uint32_t temp_dt = 0;
  uint32_t auth_code = 0x100;
  uint32_t result = TEST_PASS;
  uint32_t wr_buff[40];
  uint32_t rd_buff[40];

  /* initialize data buffers with known pattern */
  temp_dt = wr_data;
  /* SEC locations 0-15 */
  for(loop_count = 0 ; loop_count < 16 ; loop_count++){
    wr_buff[loop_count] = temp_dt;
    temp_dt += wr_data;
    rd_buff[loop_count] = 0;
  }
  /* SEC locations 16-31 */
  temp_dt = wr_data;
  for(loop_count = 16 ; loop_count < 32 ; loop_count++){
    temp_dt += wr_data1;
    wr_buff[loop_count] = temp_dt;
    rd_buff[loop_count] = 0;
  }
  /* SEC locations 32-39 */
  temp_dt = wr_data | 0x1111FFFF;
  for(loop_count = 32 ; loop_count < 40 ; loop_count++){
    temp_dt += wr_data2;
    wr_buff[loop_count] = temp_dt;
    rd_buff[loop_count] = 0;
  }

  /* Init required for BBL access */
  bbl_disable_isolation_cell();
  bbl_auth_code(auth_code);
  bbl_auth_check(auth_code);

  /* write SEC0-39 */
  post_log("Write using bbl_write_1280b_mem and read using bbl_rd_indirect_reg\n");
  if(BBL_SUCCESS !=bbl_write_1280b_mem(BBL_SEC0_MEM, &wr_buff[0], 40)){
    post_log("error : bbl_write_1280b_mem failed\n");
    result = TEST_FAIL;
  } 

  /* read back */
  if(BBL_SUCCESS !=bbl_read_1280b_mem(BBL_SEC0_MEM, &rd_buff[0], 40)){
    post_log("error : bbl_read_1280b_mem failed\n");
    result = TEST_FAIL;
  }

  /* display results */
  for(loop_count=0; loop_count < 40; loop_count++)
    {
      post_log("WRITE SEC%02d: %08x  |  READ SEC%02d: %08x", loop_count, wr_buff[loop_count], loop_count, rd_buff[loop_count]);
      if(wr_buff[loop_count] != rd_buff[loop_count])
    post_log("   <---- ERROR!!!\n");
      else
    post_log("\n");
    }

  post_log("\nSEC0-39 successfully initialized\n");

return result;
}

static int bbl_sec1280_check(void)
{ 

  uint32_t wr_buff[40];
  uint32_t rd_buff[40];
  volatile uint32_t loop_count    = 0x00;

  uint32_t wr_data  = 0x11111111;
  uint32_t wr_data1 = 0x1111;
  uint32_t wr_data2 = 0x11110000;
  volatile uint32_t temp_dt = 0;
  uint32_t auth_code = 0x100;
  uint32_t result = TEST_PASS;


  /* Init required for BBL access */
  bbl_disable_isolation_cell();
  bbl_auth_code(auth_code);
  bbl_auth_check(auth_code);

  post_log("Check SEC1280 for data retention after power cycle - assume previously programmed with know pattern\n\n");


  /* initialize data buffers with known pattern - we will verify data read from SEC1280 against this data */
  temp_dt = wr_data;
  /* SEC locations 0-15 */
  for(loop_count = 0 ; loop_count < 16 ; loop_count++){
    wr_buff[loop_count] = temp_dt;
    temp_dt += wr_data;
    rd_buff[loop_count] = 0;
  }
  /* SEC locations 16-31 */
  temp_dt = wr_data;
  for(loop_count = 16 ; loop_count < 32 ; loop_count++){
    temp_dt += wr_data1;
    wr_buff[loop_count] = temp_dt;
    rd_buff[loop_count] = 0;
  }
  /* SEC locations 32-39 */
  temp_dt = wr_data | 0x1111FFFF;
  for(loop_count = 32 ; loop_count < 40 ; loop_count++){
    temp_dt += wr_data2;
    wr_buff[loop_count] = temp_dt;
    rd_buff[loop_count] = 0;
  }

  /* read back */
  if(BBL_SUCCESS !=bbl_read_1280b_mem(BBL_SEC0_MEM, &rd_buff[0], 40)){
    post_log("error : bbl_read_1280b_mem failed\n");
    result = -1;
  }

  /* display results */
  for(loop_count=0; loop_count < 40; loop_count++)
    {
      post_log("READ SEC%02d: %08x  |  VERIFY SEC%02d: %08x", loop_count, rd_buff[loop_count], loop_count, wr_buff[loop_count]);
      if(wr_buff[loop_count] != rd_buff[loop_count])
    {
      post_log("   <---- ERROR!!!\n");
      result = TEST_FAIL;
    }
      else
    post_log("\n");
    }

  if(result) post_log("\nSEC0-39 successfully verified\n");

return result;
}

static int bbl_tamper_p0n0_enable(void)
{
  uint32_t data, data1;


  /* Init required for BBL access */
  bbl_disable_isolation_cell();
  bbl_auth_code(0x100);
  bbl_auth_check(0x100);

  bbl_rd_indirect_reg (BBL_GLITCH_CFG, &data);
  // bbl_rd_indirect_reg (FILTER_THREHOLD_CONFIG1, &data1);

  bbl_wr_indirect_reg(BBL_GLITCH_CFG, data | 1 << 18); /* */
  //bbl_wr_indirect_reg(FILTER_THREHOLD_CONFIG2, 0x7ff ); /*  */

  bbl_rd_indirect_reg (BBL_TAMPER_SRC_ENABLE, &data);
  bbl_rd_indirect_reg (BBL_EN_TAMPERIN, &data1);

  bbl_wr_indirect_reg(BBL_EN_TAMPERIN, data1 | 0x1); /* Enable P0/N0 pair as static tamper inputs */
  bbl_wr_indirect_reg(BBL_TAMPER_SRC_ENABLE, data | 0x201); /* enable P0/N0 as tamper sources */


  post_log("TAMPER_P0 and TAMPER_N0 activated\n");
  return TEST_PASS;

}

static int bbl_tamper_p0n0_disable(void)
{
  uint32_t data, data1;


  /* Init required for BBL access */
  bbl_disable_isolation_cell();
  bbl_auth_code(0x100);
  bbl_auth_check(0x100);

  bbl_rd_indirect_reg (BBL_TAMPER_SRC_ENABLE, &data);
  bbl_rd_indirect_reg (BBL_EN_TAMPERIN, &data1);

  bbl_wr_indirect_reg(BBL_TAMPER_SRC_ENABLE, data & ~0x201); /* disable P0/N0 as tampers */
  bbl_wr_indirect_reg(BBL_EN_TAMPERIN, data1 | ~0x1); /* disable P0/N0 pair as static tamper inputs */

  post_log("TAMPER_P0 and TAMPER_N0 disabled as tamper sources with glitch filters enabled\n");
  return TEST_PASS;
}

static int bbl_external_mesh_enable(void)
{
  uint32_t data;
  

  /* Init required for BBL access */
  bbl_disable_isolation_cell();
  bbl_auth_code(0x100);
  bbl_auth_check(0x100);

  /* reset mesh logic */
  bbl_wr_indirect_reg(BBL_EMESH_CONFIG, 1 << 25);
  bbl_wr_indirect_reg(BBL_EMESH_CONFIG, ~(1 << 25));

  bbl_rd_indirect_reg (BBL_MESH_CONFIG, &data);
  bbl_wr_indirect_reg (BBL_MESH_CONFIG, data | 7 << 5); /* Enable internal mesh and dynamic mode */

  bbl_wr_indirect_reg(BBL_EMESH_CONFIG, 0x180ffff); /* n[8:1] are outputs, p[8:1] are inputs, fc enabled and dyn mode */
  bbl_wr_indirect_reg(BBL_EMESH_CONFIG_1, 0x3e8);
  bbl_wr_indirect_reg(BBL_EMESH_PHASE_SEL0, 0xFAC688); /* P1 = phase 1, P2 = phase2, .... */
  bbl_rd_indirect_reg (BBL_GLITCH_CFG, &data);
  bbl_wr_indirect_reg (BBL_GLITCH_CFG, data | 0x1fe << 18); /* Enable glitch filter on external mesh pins */

  bbl_rd_indirect_reg (BBL_TAMPER_SRC_ENABLE, &data);
  bbl_wr_indirect_reg(BBL_TAMPER_SRC_ENABLE, data | 0x1c0000); /* enable external and internal mesh as tamper sources*/

  /* clear any initial false tampers */
  bbl_rd_indirect_reg (BBL_TAMPER_SRC_STAT, &data);
  bbl_wr_indirect_reg (BBL_TAMPER_SRC_CLEAR, data);

  post_log("External Mesh Grid and Internal mesh grid enabled\n");
  post_log("TAMPER_N1 ==> TAMPER_P1, phase1\n");
  post_log("TAMPER_N2 ==> TAMPER_P2, phase2\n");
  post_log("TAMPER_N3 ==> TAMPER_P3, phase3\n");
  post_log("TAMPER_N4 ==> TAMPER_P4, phase4\n");
  post_log("TAMPER_N5 ==> TAMPER_P5, phase5\n");
  post_log("TAMPER_N6 ==> TAMPER_P6, phase6\n");
  post_log("TAMPER_N7 ==> TAMPER_P7, phase7\n");
  post_log("TAMPER_N8 ==> TAMPER_P8, phase8\n");

  return TEST_PASS;
}

static int bbl_external_mesh_disable(void)
{
  uint32_t data;


  /* disable as tamper sources */
  bbl_rd_indirect_reg (BBL_TAMPER_SRC_ENABLE, &data);
  bbl_wr_indirect_reg(BBL_TAMPER_SRC_ENABLE, data & ~0x1c0000); /* disable external and internal mesh as tamper sources*/


  bbl_rd_indirect_reg (BBL_MESH_CONFIG, &data);
  bbl_wr_indirect_reg (BBL_MESH_CONFIG, data & ~(7 << 5)); /* disable internal mesh and dynamic mode */

  /* reset mesh logic */
  bbl_rd_indirect_reg (BBL_EMESH_CONFIG, &data);
  bbl_wr_indirect_reg(BBL_EMESH_CONFIG, data | (1 << 25));
  bbl_wr_indirect_reg(BBL_EMESH_CONFIG, data & ~(1 << 25));

  post_log("External Mesh Grid and Internal mesh grid disabled\n");
  return TEST_PASS;
}

static int bbl_temp_sensor_enable(void)
{
  uint32_t data, data1;
 

  /* set high/low temperature triggers */
  /* LOW TEMP = -10C ==> 0x36B */
  /* HIGH TEMP = 85C ==> 0x2A7 */
  /* DELTA = 0x14 */
  bbl_rd_indirect_reg (BBL_TMON_CONFIG, &data);
  data &= 0x00000003;
  data = data | (0x36B2A750);
  bbl_wr_indirect_reg (BBL_TMON_CONFIG, data);

  bbl_rd_indirect_reg (BBL_TMON_CONFIG, &data1);

  bbl_wr_indirect_reg(BBL_TMON_CONFIG, data1 & ~0x1); /* power up temp monitor */
  // bbl_diag_delay(1000000); /* let TMON warm up */

  /* wait for TMON to read valid data before enabling tamper */
  do
    {
      bbl_rd_indirect_reg (BBL_STAT, &data);
      post_log("Current ADC reading = %03x\n", data & 0x000003FF);
    }
  while(!(data & 0x000003FF));

  bbl_rd_indirect_reg (BBL_TAMPER_SRC_ENABLE, &data);
  bbl_wr_indirect_reg(BBL_TAMPER_SRC_ENABLE, data | 0x7 << 22); /* enable high, low, delta tampers */

  post_log("Temperature monitor enabled with LOW LIMIT = -10C (ADC=0x36B), HIGH LIMIT = 85C (ADC=0x2A7)\n");

  return TEST_PASS;
}

static int bbl_temp_sensor_disable(void)
{
  uint32_t data, data1;


  bbl_rd_indirect_reg (BBL_TAMPER_SRC_ENABLE, &data);
  bbl_rd_indirect_reg (BBL_TMON_CONFIG, &data1);

  bbl_wr_indirect_reg(BBL_TMON_CONFIG, data1 | 0x1); /* power down temp monitor */
  bbl_wr_indirect_reg(BBL_TAMPER_SRC_ENABLE, data & ~(0x7 << 22)); /* disable high, low, delta tampers */

  post_log("Temperature monitor disabled\n");

  return TEST_PASS;
}

static int bbl_freq_monitor_enable(void)
{
  uint32_t data, data1;
  

  /* run FMON auto calibration */
  bbl_wr_indirect_reg(FMON_CNG, 0x02000c30); 
  bbl_wr_indirect_reg(FMON_CNG_1, 0xA4); 
  bbl_diag_delay(100000);
  bbl_wr_indirect_reg(FMON_CNG_1, 0xAC);
  bbl_diag_delay(100000);
  do
    {
      bbl_rd_indirect_reg (BBL_STAT, &data);
    }
  while(!(data & 0x40000));

  /* get FMON calibration value data[18:11]*/
  data = ((data >> 11) & 0xFF);
  data1 = 0x02000c30 | (data << 16);
  bbl_wr_indirect_reg (FMON_CNG, data1);

  /* enable as a tamper source */
  bbl_rd_indirect_reg (BBL_TAMPER_SRC_ENABLE, &data);
  bbl_wr_indirect_reg (BBL_TAMPER_SRC_ENABLE, data | 3 << 25);

  post_log("Frequency monitor calibrated and enabled\n");

  return TEST_PASS;
}

static int bbl_freq_monitor_disable(void)
{
  uint32_t data;

  /* disable as a tamper source */
  bbl_rd_indirect_reg (BBL_TAMPER_SRC_ENABLE, &data);
  bbl_wr_indirect_reg (BBL_TAMPER_SRC_ENABLE, ~(data | 3 << 25));

  return TEST_PASS;
}

static int bbl_voltage_monitor_enable(void)
{
  uint32_t data, data1;
 

  bbl_rd_indirect_reg (BBL_TAMPER_SRC_ENABLE, &data);
  bbl_rd_indirect_reg (BBL_TAMPER_SRC_ENABLE_1, &data1);

  bbl_wr_indirect_reg(BBL_TAMPER_SRC_ENABLE, data | BBL_TAMPER_SRC_ENABLE__BBL_Vmon_En);
  bbl_wr_indirect_reg(BBL_TAMPER_SRC_ENABLE_1, data1 | 0x30); /* coincell prot and high voltage en */

  post_log("Voltage monitors enabled: Coincell drops below 2.0V, high/low voltage detection\n");
  return TEST_PASS;
}

static int bbl_voltage_monitor_disable(void)
{
  uint32_t data, data1;


  bbl_rd_indirect_reg (BBL_TAMPER_SRC_ENABLE, &data);
  bbl_rd_indirect_reg (BBL_TAMPER_SRC_ENABLE_1, &data1);

  bbl_wr_indirect_reg(BBL_TAMPER_SRC_ENABLE, data & ~BBL_TAMPER_SRC_ENABLE__BBL_Vmon_En);
  bbl_wr_indirect_reg(BBL_TAMPER_SRC_ENABLE_1, data1 & ~0x30); /* coincell protection and high voltage en */

  post_log("Voltage monitors disabled\n");
  return TEST_PASS;
}

static int bbl_set_rtc(void)
{
  return TEST_PASS;
}

/* Prints active tampers then clears all active tamper events */
static int bbl_clear_tampers(void)
{
  uint32_t data, data1, data2;
  uint32_t status;
  uint32_t i = 0;
  
  do {
	  bbl_rd_indirect_reg (BBL_TAMPER_SRC_STAT, &data);
	  bbl_rd_indirect_reg (BBL_TAMPER_SRC_STAT_1, &data1);
	  post_log("TAMPER_SRC_STAT  = %08x\n", data);
	  post_log("TAMPER_SRC_STAT1 = %08x\n", data1);
	  
	  bbl_rd_indirect_reg (BBL_EMESH_CONFIG, &data2);
	  if(data & ~(3 << 18)) /* see if external mesh caused tamper, if so, need to toggle mesh clr bit */
	    {
	      bbl_wr_indirect_reg(BBL_EMESH_CONFIG, data2 | (1 << 25));
	      bbl_wr_indirect_reg(BBL_EMESH_CONFIG, data2 & ~(1 << 25));
	    } 
	   
	  bbl_wr_indirect_reg(BBL_TAMPER_SRC_CLEAR, data);
	  bbl_wr_indirect_reg(BBL_TAMPER_SRC_CLEAR_1, data1);
	
	  bbl_rd_indirect_reg (BBL_TAMPER_SRC_STAT, &data);
	  bbl_rd_indirect_reg (BBL_TAMPER_SRC_STAT_1, &data1);
	
	  post_log("***** CLEAR TAMPERS *****\n");
	  post_log("TAMPER_SRC_STAT  = %08x\n", data);
	  post_log("TAMPER_SRC_STAT1 = %08x\n", data1);
	  i++;
	  if (i > 5) break;
	  mdelay(1000);
  } while ((data & 0xFFFFF) != 0);
  
  if ((data & 0xFFFFF) != 0) {
  	 post_log("Active tampers still exist\n");
     status=TEST_FAIL;
  } else {
  	 status=TEST_PASS;
  }
  /*if(!(data | data1))
     status=TEST_PASS;
  else
       {
     post_log("Active tampers still exist\n");
     status=TEST_FAIL;
       }*/
  return status;
}

static int bbl_monitor_tampers(void)
{

  char *tamperString;
  uint32_t data, data2, data3, data4, data5, data6, data7, data8, data9, temp;

  while(1)
    {
    bbl_rd_indirect_reg (BBL_TAMPER_SRC, &data3);
        
        switch(data3)
        {
            case 0x01:
            tamperString = "Tamper_N[0]";
            break;
            
            case 0x02:
            tamperString = "Tamper_N[1]";
            break;

            case 0x03:
            tamperString = "Tamper_N[2]";
            break;
            
            case 0x04:
            tamperString = "Tamper_N[3]";
            break;

            case 0x05:
            tamperString = "Tamper_N[4]";
            break;
            
            case 0x06:
            tamperString = "Tamper_N[5]";
            break;

            case 0x07:
            tamperString = "Tamper_N[6]";
            break;
            
            case 0x08:
            tamperString = "Tamper_N[7]";
            break;

            case 0x09:
            tamperString = "Tamper_N[8]";
            break;
            
            case 0x0a:
            tamperString = "Tamper_P[0]";
            break;

            case 0x0b:
            tamperString = "Tamper_P[1]";
            break;

            case 0x0c:
            tamperString = "Tamper_P[2]";
            break;

            case 0x0d:
            tamperString = "Tamper_P[3]";
            break;

            case 0x0e:
            tamperString = "Tamper_P[4]";
            break;

            case 0x0f:
            tamperString = "Tamper_P[5]";
            break;

            case 0x10:
            tamperString = "Tamper_P[6]";
            break;

            case 0x11:
            tamperString = "Tamper_P[7]";
            break;

            case 0x12:
            tamperString = "Tamper_P[8]";
            break;

            case 0x13:
            tamperString = "Hi Freq Mon";
            break;
            
            case 0x14:
            tamperString = "Lo Freq Mon";
            break;

                case 0x15:
            tamperString = "SPL event";
            break;
            
            case 0x16:
            tamperString = "Lo Volt Mon";
            break;
            
            case 0x17:
            tamperString = "Hi Temp Mon";
            break;
            
            case 0x18:
            tamperString = "Lo Temp Mon";
            break;

            case 0x19:
            tamperString = "Rate of Chg";
            break;
            
            case 0x1a:
            tamperString = "Int Grid Fault";
            break;

            case 0x1b:
            tamperString = "Int Grid Fault - Open";
            break;
            
            case 0x1c:
            tamperString = "Ext Grid Fault";
            break;

            case 0x1d:
            tamperString = "Ext Grid Fault - Open";
            break;
            
            case 0x1e:
            tamperString = "Clock Glitch Mon";
            break;

            case 0x1f:
            tamperString = "SPL reset event";
            break;

                case 0x20:
            tamperString = "SEC1280 clear event";
            break;

                case 0x21:
            tamperString = "ECC event";
            break;

                case 0x22:
            tamperString = "FID event";
            break;

                case 0x23:
            tamperString = "Dig_fmon tammper event";
            break;

                case 0x24:
            tamperString = "Reserved";
            break;

                case 0x25:
            tamperString = "High voltage event";
            break;

                case 0x26:
            tamperString = "LDO_o_prot_activate";
            break;
            

            default:
            tamperString = "No latched tampers";
            break;
    }

    bbl_rd_indirect_reg (BBL_RTC_SECOND, &data); 
    bbl_rd_indirect_reg (BBL_INPUT_STATUS, &data2); /* Raw mesh/tamper status */
    bbl_rd_indirect_reg (BBL_STAT, &data4); /* current ADC temp reading */
    bbl_rd_indirect_reg (BBL_TAMPER_TIMESTAMP, &data5); /* Timestamp of last tamper event */
    bbl_rd_indirect_reg (BBL_TAMPER_SRC_ENABLE, &data6); 
    bbl_rd_indirect_reg (BBL_TAMPER_SRC_ENABLE_1, &data7); 
    bbl_rd_indirect_reg (BBL_TAMPER_SRC_STAT, &data8); 
    bbl_rd_indirect_reg (BBL_TAMPER_SRC_STAT_1, &data9); 

    post_log("*********************************\n");
    post_log("     Tamper Monitor Running \n");
    post_log("*********************************\n");
    post_log("RTC SECONDS            :%08x\n", data);
    post_log("ADC TEMP VALUE         :%08x\n", data4 & 0x3FF);
    post_log("RAW TAMPER/MESH STATUS :%08x\n", data2);
    post_log("TAMPER_SRC_ENABLE      :%08x\n", data6);
    post_log("TAMPER_SRC_ENABLE_1    :%08x\n", data7);
    post_log("TAMPER_SRC_STAT        :%08x\n", data8);
    post_log("TAMPER_SRC_STAT_1      :%08x\n", data9);
    post_log("TAMPER TRIGGERED       :%s\n\n", tamperString);
    post_log("TAMPER TIMESTAMP       :%08x\n", data5);
    post_log("*********************************\n");
    post_log("Press any key to exit\n");
        
    bbl_diag_delay(10000000);

     /* Press a key to exit loop */
    temp = cpu_rd_single(ChipcommonG_UART3_UART_LSR, 4);

     if(temp & UART_LSR_DR)
       break;
     }
  return 0;
}

static int bbl_soft_reset(void)
{
  uint32_t data;

  data = 1;
  bbl_wr_indirect_reg (BBL_CONFIG_1, data); 
  bbl_diag_delay(1000000);

  post_log("BBL soft reset complete\n");

  return TEST_PASS;
}

static int bbl_sec1280_rd_wr(void)
{
  int loop_count    = 0x00;
  uint32_t rd_data  = 0x00;
  uint32_t wr_data  = 0xDEADBEEF;
  uint32_t wr_data1 = 0x5A5A5A5A;
  uint32_t temp_dt = 0;
  uint32_t wr_buff[40];
  uint32_t wr_buff1[40];
  uint32_t rd_buff[40];
  uint32_t result = TEST_PASS;
  uint32_t auth_code = 0x100;
  //bbl_auth_check(auth_code);
  bbl_auth_code(auth_code);


  temp_dt = wr_data;
  for(loop_count = 0 ; loop_count < 40 ; loop_count++){
     wr_buff[loop_count] = temp_dt++;
     rd_buff[loop_count] = 0;
  }
  bbl_auth_check(auth_code);
  post_log("Write using bbl_write_1280b_mem and read using bbl_rd_indirect_reg\n");
  if(BBL_SUCCESS !=bbl_write_1280b_mem(BBL_SEC0_MEM, &wr_buff[0], 40)){
    post_log("error : bbl_write_1280b_mem failed\n");
    result = -1;
  } 
  bbl_auth_check(0x00);
  if(BBL_SUCCESS !=bbl_read_1280b_mem(BBL_SEC0_MEM, &rd_buff[0], 40)){
    post_log("error : bbl_read_1280b_mem failed\n");
    result = -1;
  }

  for(loop_count = 0 ; loop_count < 40 ; loop_count++){
    if(wr_buff[loop_count] != rd_buff[loop_count]){
      post_log("error : data comparison failed - data rd[%d] = 0x%X , data wr[%d] = 0x%X\n",loop_count,rd_buff[loop_count],loop_count,wr_buff[loop_count]);
      result = -1;
      break;
    }
  }

  /*Read known pattern using indirect read API*/
  for(loop_count = BBL_SEC0_MEM ; loop_count <= BBL_SEC39_MEM ; loop_count++ ){
    if(BBL_SUCCESS !=bbl_rd_indirect_reg (( (loop_count * 4) + BBL_SEC0),&rd_data)){
      post_log("error : bbl_rd_indirect_reg return -1\n");
      result = -1;
      break;
    }
    if(wr_buff[loop_count] != rd_data){
      post_log("error : data comparison failed using bbl_rd_indirect_reg - data rd = 0x%X , data wr[%d] = 0x%X\n",rd_data,loop_count,wr_buff[loop_count]);
      result = -1;        
    } 
  }

  post_log("Write using bbl_wr_indirect_reg and read using bbl_read_1280b_mem\n");
  temp_dt = wr_data1;
  for(loop_count = 0 ; loop_count < 40 ; loop_count++){
     wr_buff[loop_count] = temp_dt++;
     rd_buff[loop_count] = 0;
  }
  bbl_auth_check(auth_code);
  /*Write known pattern using indirect write API*/
  for(loop_count = BBL_SEC0_MEM ; loop_count <= BBL_SEC39_MEM ; loop_count++ ){
    if(BBL_SUCCESS !=bbl_wr_indirect_reg (( (loop_count * 4) + BBL_SEC0),wr_buff[loop_count])){
      post_log("error : bbl_wr_indirect_reg return -1\n");
      result = -1;
      break;
    }
  }  

  if(BBL_SUCCESS !=bbl_read_1280b_mem(BBL_SEC0_MEM, &rd_buff[0], 40)){
    post_log("error : bbl_read_1280b_mem failed\n");
    result = -1;
  }

  for(loop_count = 0 ; loop_count < 40 ; loop_count++){
    if(wr_buff[loop_count] != rd_buff[loop_count]){
      post_log("error : data comparison failed - data rd[%d] = 0x%X , data wr[%d] = 0x%X\n",loop_count,rd_buff[loop_count],loop_count,wr_buff[loop_count]);
      result = -1;
      break;
    }
  }

  post_log("sec mem access on auth check failure\n");
  temp_dt = wr_data;
  for(loop_count = 0 ; loop_count < 40 ; loop_count++){
     wr_buff1[loop_count] = temp_dt++;
     rd_buff[loop_count] = 0;
  }
   /* Auth check will fail here*/
  /* Try to write some other data on auth check failure */
  post_log("expecting write error...\n");
  bbl_auth_check(~auth_code);
  if(BBL_SUCCESS !=bbl_write_1280b_mem(BBL_SEC0_MEM, &wr_buff1[0], 40)){
    post_log("good : bbl_write_1280b_mem returns error as expected\n");
  }
  else{
    post_log("error : bbl_write_1280b_mem returns pass(0) even after auth failure\n");
    result = -1;
  } 
  bbl_auth_check(0x00);

  if(BBL_SUCCESS !=bbl_read_1280b_mem(BBL_SEC0_MEM, &rd_buff[0], 40)){
    post_log("error : bbl_read_1280b_mem failed\n");
    result = -1;
  }

  /* Previous write should not change the BBL SEC memory because of auth check failure, Read SEC 1280 memory and confirm it */
  for(loop_count = 0 ; loop_count < 40 ; loop_count++){
    if(wr_buff[loop_count] != rd_buff[loop_count]){
      post_log("error : data comparison failed - data rd[%d] = 0x%X , data wr[%d] = 0x%X\n",loop_count,rd_buff[loop_count],loop_count,wr_buff[loop_count]);
      result = -1;
      break;
    }
  }
  bbl_auth_check(0x00);
  post_log("--Test %s--\n",result == -1 ? "FAILED":"PASSED");
  return result;
}


bool simulate_tamper(void)
{
  uint32_t tmpr_src_en = 0,tmpr_src_stat = 0, poll_count = 10000;
  

  post_log("Disabling all tamper src en\n");
  if(BBL_SUCCESS !=bbl_set_tamper_src_en(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)){
    post_log("error : bbl_set_tamper_src_en\n");
 
  }
 
  if(BBL_SUCCESS !=bbl_get_tamper_src_en(&tmpr_src_en)){
    post_log("error : bbl_get_tamper_src_en\n");

  }
  post_log("get tamper src en : 0x%X\n",tmpr_src_en); 

  post_log("Clearing all tamper src stat\n");
  if(BBL_SUCCESS !=bbl_tamper_src_clr((~0),(~0),(~0),(~0),(~0),(~0),(~0),(~0),(~0),(~0),(~0))){
    post_log("error : bbl_tamper_src_clr\n");
  
  }
  bbl_diag_delay(100);
  if(BBL_SUCCESS !=bbl_tamper_src_clr(0,0,0,0,0,0,0,0,0,0,0)){
    post_log("error : bbl_tamper_src_clr\n");

  }
 
  if(BBL_SUCCESS !=bbl_get_tamper_src_status(&tmpr_src_stat)){
    post_log("error : bbl_get_tamper_src_status\n");
    
  }
  post_log("get tamper src stat : 0x%X\n", tmpr_src_stat); 
  
  post_log("Setting tamper src en\n");
  if(BBL_SUCCESS !=bbl_set_tamper_src_en(0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0)){
    post_log("error : bbl_set_tamper_src_en\n");
 
  }

  if(BBL_SUCCESS !=bbl_get_tamper_src_en(&tmpr_src_en)){
    post_log("error : bbl_get_tamper_src_en\n");

  }
  post_log("get tamper src en : 0x%X\n",tmpr_src_en); 

  post_log("Configuring mesh\n");
  if(BBL_SUCCESS !=bbl_mesh_config(0, 0, 0, 1, 1, 0)){
    post_log("error : bbl_mesh_config\n");
    
  }

  if(BBL_SUCCESS !=bbl_dbg_config(0,0,1))
    return FALSE;

  do{
    if(BBL_SUCCESS !=bbl_check_tamper_src_status(BBL_IGRID)){
      post_log("Tamper event generated..\n");
      break;
    }
  }while(poll_count--);

    if(poll_count == -1){
    post_log("error : tamper simulation failed, poll count : %d\n",poll_count );

    }

   if(BBL_SUCCESS !=bbl_get_tamper_src_status(&tmpr_src_stat)){
    post_log("error : bbl_get_tamper_src_status\n");

   }
   post_log("get tamper src stat : 0x%X\n", tmpr_src_stat); 

   post_log("Clearing mesh config\n");
   if(BBL_SUCCESS !=bbl_mesh_config(0, 0, 0, 0, 0, 0)){
    post_log("error : bbl_mesh_config\n");

   }

   post_log("Disabling all tamper src en\n");
   if(BBL_SUCCESS != bbl_set_tamper_src_en(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)){
    post_log("error : bbl_set_tamper_src_en\n");

   }
 
   if(BBL_SUCCESS != bbl_get_tamper_src_en(&tmpr_src_en)){
    post_log("error : bbl_get_tamper_src_en\n");
    
   }
   post_log("get tamper src en : 0x%X\n",tmpr_src_en); 

   bbl_dbg_config(0,0,0);
  
  post_log("Clearing all tamper src stat\n");
  if(BBL_SUCCESS != bbl_tamper_src_clr((~0),(~0),(~0),(~0),(~0),(~0),(~0),(~0),(~0),(~0), (~0))){
    post_log("error : bbl_tamper_src_clr\n");
   
  }
  bbl_diag_delay(100);
  if(BBL_SUCCESS != bbl_tamper_src_clr(0,0,0,0,0,0,0,0,0,0,0)){
    post_log("error : bbl_tamper_src_clr\n");
 
  }
 
  if(BBL_SUCCESS != bbl_get_tamper_src_status(&tmpr_src_stat)){
    post_log("error : bbl_get_tamper_src_status\n");
   
  }
  post_log("get tamper src stat : 0x%X\n", tmpr_src_stat); 
 
  return TRUE;
}

static int bbl_simulate_tamper(void)
{
  uint32_t loop_count   = 0x00;
  uint32_t wr_data = 0xDEADBEEF;
  uint32_t wr_buff[40];
  uint32_t rd_buff[40];
  int result = TEST_PASS; /*pass*/ 
  uint32_t temp_dt = 0;
 // uint32_t max_loop = 50;
  uint32_t auth_code = 0x200;

  temp_dt = wr_data;
  for(loop_count = 0 ; loop_count < 40 ; loop_count++){
     wr_buff[loop_count] = temp_dt++;
     rd_buff[loop_count] = 0;
  }
 // bbl_auth_check(0x10);
  bbl_auth_code(auth_code);

//  while((SPRU_IS_POWERED_BY_AON != dmu_bbl_interface_active()))
//  {
//    if(!(max_loop--)){
//        post_log("error : SPRU is powered by AON is not set even after 50 times polling\n");
//        return TEST_FAIL;
//    }
//  }


  post_log("write and read to BBL SEC1280 memory area\n");

  /*Write known pattern*/ 
  bbl_auth_check(auth_code);
  if(BBL_SUCCESS != bbl_write_1280b_mem(BBL_SEC0_MEM, &wr_buff[0], 40)){
    post_log("error : bbl_write_1280b_mem failed\n");
    result = -1;
  } 
  bbl_auth_check(0x00);
  if(BBL_SUCCESS != bbl_read_1280b_mem(BBL_SEC0_MEM, &rd_buff[0], 40)){
    post_log("error : bbl_read_1280b_mem failed\n");
    result = -1;
  }

  for(loop_count = 0 ; loop_count < 40 ; loop_count++){
    if(wr_buff[loop_count] != rd_buff[loop_count]){
      post_log("error : data comparison failed - data rd[%d] = 0x%X , data wr[%d] = 0x%X\n",loop_count,rd_buff[loop_count],loop_count,wr_buff[loop_count]);
      result = -1;
      break;
    }
  }

  post_log("simulating tamper\n");
  bbl_auth_check(auth_code);
  if(!simulate_tamper()){
    post_log("error : simulate_tamper\n");
    result = TEST_FAIL;
  }

  /* all SEC1280 mem shoud be cleared due to the previous tamper generation */
  if(BBL_SUCCESS != bbl_read_1280b_mem(BBL_SEC0_MEM, &rd_buff[0], 40)){
    post_log("error : bbl_read_1280b_mem failed\n");
    result = -1;
  }

  for(loop_count = 0 ; loop_count < 40 ; loop_count++){
    if(0 != rd_buff[loop_count]){
      post_log("error : data not cleared - location : %d bank, data rd = 0x%X\n",loop_count,rd_buff[loop_count]);
      result = -1;        
    }
  }
  bbl_auth_check(0x00);
  post_log("--Test %s--\n",result == TEST_PASS ? "PASSED":"FAILED");
  return result;
}


/*  
** bbl kek read write test
** return test pass/fail 
*/
static int bbl_kek_rd_wr(void)
{
  int loop_count   = 0x00;
  uint32_t wr_data = 0xDEADBEEF;
  uint32_t bbl_kek_length = BBL_KEK_LENGTH/32;
  uint32_t bbl_kek[BBL_KEK_LENGTH/32]; 
  uint32_t bbl_kek_read[BBL_KEK_LENGTH/32];
//  uint32_t max_loop = 50;
  uint32_t auth_code = 0xffff;
  //bbl_auth_check(0x10);
  bbl_auth_code(auth_code);

//  while((SPRU_IS_POWERED_BY_AON != dmu_bbl_interface_active()))
//  {
//    if(!(max_loop--)){
//        post_log("error : SPRU is powered by AON is not set even after 50 times polling\n");
//        return TEST_FAIL;
//    }
//  }

  for(loop_count = 0 ; loop_count < bbl_kek_length ; loop_count++){
  bbl_kek[loop_count]      = (wr_data + loop_count);
  bbl_kek_read[loop_count] = 0x00;
  }   

  if(BBL_SUCCESS != bbl_write_kek(&bbl_kek[0], auth_code)){
    post_log("BBL KEK Write failed \n");
    post_log("--Test FAILED--\n");
    return TEST_FAIL; 
  }
  post_log("BBL KEK write done \n");

  if(BBL_KEK_CRC_CHECK_FAILED != bbl_read_kek(&bbl_kek_read[0])){
    post_log("BBL KEK Read failed \n");
    post_log("--Test FAILED--\n");
    return TEST_FAIL; 
  }  
  post_log("BBL KEK read done \n");

  for(loop_count = 0 ; loop_count < bbl_kek_length ; loop_count++){
    if(bbl_kek[loop_count] != bbl_kek_read[loop_count]){
      post_log("Access to KEK SEC bank %d failed, data written = 0x%X - data read = 0x%X\n",(loop_count + 1) ,bbl_kek[loop_count], bbl_kek_read[loop_count]);
      post_log("--Test FAILED--\n");
      return TEST_FAIL; 
    }
  }
  post_log("--Test PASSED--\n");
  return TEST_PASS;
}

static int bbl_rtc_set_div_secs(void)
{
  uint32_t div   = 0;
  uint32_t secs  = 0;
  uint32_t rdiv  = 0;
  uint32_t rsecs = 0;
  int result = TEST_PASS;
//  uint32_t max_loop = 50;

  bbl_auth_check(0x10);
  bbl_auth_code(0x10);

//  while((SPRU_IS_POWERED_BY_AON != dmu_bbl_interface_active()))
//  {
//    if(!(max_loop--)){
//        post_log("error : SPRU is powered by AON is not set even after 50 times polling\n");
//        return TEST_FAIL;
//    }
//  }

  bbl_rtc_control(BBL_RTC_STOP); /* stop the clock */ 

  if(BBL_SUCCESS != bbl_set_time(div, secs)){
    post_log("bbl set time failed\n");
    return TEST_FAIL;
  }

  post_log("delay....\n");
  bbl_rtc_control(BBL_RTC_START); /* start the TRC clock */
#ifdef CONFIG_CYGNUS_EMULATION 
  bbl_diag_delay(1000000);

  bbl_diag_delay(1000);

#else
  bbl_diag_delay(10000000);

#endif


  if(BBL_SUCCESS != bbl_get_rtc_div(&rdiv)){
    post_log("bbl get rtc div failed\n");
    result = TEST_FAIL;
  }

  if(BBL_SUCCESS != bbl_get_rtc_secs(&rsecs)){
    post_log("bbl get rtc secs failed\n");
    result = TEST_FAIL;
  }
  
  if( rsecs == secs ){
    post_log("sec set is : 0x%X , sec get is : 0x%X\n",secs, rsecs);
    result = TEST_FAIL;
  }
  post_log("get RTC DIV  : %d\n",rdiv);
  post_log("get RTC SECS : %d\n",rsecs);
  post_log("-- Test %s --\n", result == TEST_PASS ? "PASSED" : "FAILED");
  return result;
}

static int bbl_kek_validity(void)
{
  int loop_count   = 0x00;
  int result = TEST_PASS;
  uint8_t bbl_kek_wr[BBL_TOTAL_KEK_BANK  * 8] = {1,2,3,4,5,6,7,8,9,0xa,0xb,0xc,0xd,0xe,0xf,0}; 
  uint32_t kek_array[BBL_TOTAL_KEK_BANK];
  uint8_t expect[32] = {0x5e,0x97,0xe9,0xd2,0x3b,0xb2,0x3f,0xa1,0x51,0xec,0x07,0x05,0xd6,0x29,0xd7,0x75,0x14,0x11,0x59,0x74,0xe9,0x72,0xce,0x2d,0x8a,0xb0,0x89,0x90,0xde,0x79,0xb2,0x64};
  uint32_t *p_sha = (uint32_t *)&expect[0];
  uint32_t *p_kek_wr = (uint32_t *)&bbl_kek_wr[0];
 // uint32_t max_loop = 50;
  uint32_t auth_code = 0xFFFFFFFF;
  post_log("KEK wr = 0x");
  for (loop_count = 0 ; loop_count < 4 ; loop_count++)
  {
    post_log("%X",p_kek_wr[loop_count]);

  }
  post_log("\n");
  //bbl_auth_check(0x10);
  bbl_auth_code(auth_code);

//  while((SPRU_IS_POWERED_BY_AON != dmu_bbl_interface_active()))
//  {
//    if(!(max_loop--)){
//        post_log("error : SPRU is powered by AON is not set even after 50 times polling\n");
//        return TEST_FAIL;
//    }
//  }

  if(BBL_SUCCESS != bbl_write_kek(p_kek_wr,auth_code)){
    post_log("error : bbl_write_kek(p_kek_wr failed \n");
    result = TEST_FAIL; 
  }
  bbl_auth_check(auth_code);
  if(BBL_SUCCESS != bbl_write_1280b_mem(BBL_KEK_SHA256_OFFSET, p_sha,1)){
    post_log("error : bbl_read_1280b_mem\n");
    result = TEST_FAIL;
  }
  bbl_auth_check(0x00);
  if(BBL_SUCCESS != bbl_kek_valid(&kek_array[0])){
    post_log("error : bbl_kek_valid returns FALSE\n");
    result = TEST_FAIL;
  }
  else
    post_log("good : bbl_kek_valid returns TRUE\n");

  post_log("KEK returned from kek valid function\n");
  for (loop_count = 0 ; loop_count < 4 ; loop_count++)
  {
    post_log("%X",kek_array[loop_count]);

  }
  //Corrupt the KEK
  post_log("Corrupting the KEK and expecting failure\n");
  bbl_kek_wr[4] = 0;
  if(BBL_SUCCESS != bbl_write_kek(p_kek_wr,auth_code)){
    post_log("error : bbl_write_kek(p_kek_wr failed \n");
    result = TEST_FAIL;
  }
  bbl_auth_check(auth_code);
  if(BBL_SUCCESS != bbl_write_1280b_mem(BBL_KEK_SHA256_OFFSET, p_sha,1)){
    post_log("error : bbl_read_1280b_mem\n");
    result = TEST_FAIL;
  }
  bbl_auth_check(0x00);
  if(BBL_KEK_CRC_CHECK_FAILED != bbl_kek_valid(&kek_array[0])){
    post_log("error : bbl_kek_valid returns != BBL_KEK_CRC_CHECK_FAILED\n");
    result = TEST_FAIL;
  }

  post_log("Checking Invalid kek type 1 all 0's\n");
  for (loop_count = 0 ; loop_count < 16 ; loop_count++)
  {
      bbl_kek_wr[loop_count] = 0x00;

  }
  if(BBL_SUCCESS != bbl_write_kek(p_kek_wr,auth_code)){
    post_log("error : bbl_write_kek(p_kek_wr failed \n");
    result = TEST_FAIL;
  }
  bbl_auth_check(auth_code);
  if(BBL_SUCCESS != bbl_write_1280b_mem(BBL_KEK_SHA256_OFFSET, p_sha,1)){
    post_log("error : bbl_read_1280b_mem\n");
    result = TEST_FAIL;
  }
  bbl_auth_check(0x00);
  if(BBL_KEK_IS_NOT_VALID != bbl_kek_valid(&kek_array[0])){
    post_log("error : bbl_kek_valid returns != BBL_KEK_IS_NOT_VALID\n");
    result = TEST_FAIL;
  }

  post_log("Checking Invalid kek type 2 all 0's\n");
  for (loop_count = 0 ; loop_count < 16 ; loop_count++)
  {
      bbl_kek_wr[loop_count] = 0xFF;
  }

  if(BBL_SUCCESS != bbl_write_kek(p_kek_wr,auth_code)){
    post_log("error : bbl_write_kek(p_kek_wr failed \n");
    result = TEST_FAIL;
  }
  bbl_auth_check(auth_code);
  if(BBL_SUCCESS != bbl_write_1280b_mem(BBL_KEK_SHA256_OFFSET, p_sha,1)){
    post_log("error : bbl_read_1280b_mem\n");
    result = TEST_FAIL;
  }
  bbl_auth_check(0x00);
  if(BBL_KEK_IS_NOT_VALID != bbl_kek_valid(&kek_array[0])){
    post_log("error : bbl_kek_valid returns != BBL_KEK_IS_NOT_VALID\n");
    result = TEST_FAIL;
  }

  post_log("--Test %s--\n",result == TEST_FAIL ? "FAILED" : "PASSED");
  return result;
  
}

/******* Internal test function end ********/
/*  
** Master BBL diag function
** return test pass/fail 
*/
int test_sha256(void)
{
  crypto_lib_handle Handle[3];
  crypto_lib_handle *ptr = Handle;
  //bcm_crypto_init(&Handle[0]);
  bcm_crypto_init(ptr);
  int i =0;
  int result = 0;
  uint8_t outputSha256[40]; 
  uint32_t expect[40] = {0x5e,0x97,0xe9,0xd2,0x3b,0xb2,0x3f,0xa1,0x51,0xec,0x07,0x05,0xd6,0x29,0xd7,0x75,0x14,0x11,0x59,0x74,0xe9,0x72,0xce,0x2d,0x8a,0xb0,0x89,0x90,0xde,0x79,0xb2,0x64};
  uint8_t data[20] = {1,2,3,4,5,6,7,8,9,0xa,0xb,0xc,0xd,0xe,0xf,0};
  post_log("Calling bcm_get_hmac_sha256_hash\n");
//  if(BCM_SCAPI_STATUS_OK != bcm_get_hmac_sha256_hash(&Handle[0], (uint8_t *)&data[0],16,NULL,0,(uint8_t*)&outputSha256[0],FALSE)){
  if(BCM_SCAPI_STATUS_OK != bcm_get_hmac_sha256_hash((crypto_lib_handle *)&Handle, (uint8_t *)&data,16,NULL,0,(uint8_t*)&outputSha256,FALSE)){
    post_log("ERROR\n");
    result = -1;
  }
  else{
    post_log("SHA256 = 0x");
    for(i = 0 ; i < 32 ; i++){
      post_log("%X",outputSha256[i]);
    }
    for(i = 0 ; i < 32 ; i++){
      if(outputSha256[i] != expect[i]){
        post_log("\nSHA256 is not data failed at spum_sha256[%d] = 0x%X, exp_sha256[%d] = 0x%X\n",i,outputSha256[i],i,expect[i]);
        result = -1;
      }
    }
  } 
  post_log("Test - %s\n",result == -1 ? "FAILED" : "PASSED");
  return result;
}

#if 0

int BBL_post_test(int flags, post_result *t_result)
{
  int test_no;
  int loop_count = 0, total_tests = 0, total_regression = 0;
  int status = TEST_PASS;
  FUNC_INFO regression[]={
                     ADD_TEST(  bbl_sec1280_rd_wr    ),
                     ADD_TEST(  bbl_kek_rd_wr        ),
                     ADD_TEST(  bbl_rtc_set_div_secs ),
                     ADD_TEST(  bbl_simulate_tamper  ),
                     ADD_TEST(  bbl_kek_validity     )
                   };   

  FUNC_INFO function[]={
                     ADD_TEST(  bbl_sec1280_rd_wr    ),
                     ADD_TEST(  bbl_sec1280_wr    ),
                     ADD_TEST(  bbl_kek_rd_wr        ),
                     ADD_TEST(  bbl_rtc_set_div_secs ),
                     ADD_TEST(  bbl_simulate_tamper  ),
                     ADD_TEST(  test_sha256          ),
                     ADD_TEST(  bbl_kek_validity     )
                   };   
  total_tests = (sizeof(function) / sizeof(function[0]));
  total_regression = (sizeof(regression) / sizeof(regression[0]));
  if(flags & POST_REGRESSION){
    post_log("Total Regression Tests : %d\n",total_regression); 
    for(loop_count = 0 ; loop_count  < total_regression ; loop_count++ ){
      
      post_log("\n---Executing test %2d - %s\n",(loop_count + 1), regression[loop_count].func_name);
      
      status = (*regression[loop_count].func)();
      switch(status)
      {
        case TEST_PASS:
        t_result->t_pass++;
        break;
        case TEST_FAIL:
        t_result->t_fail++;
        break;
        case TEST_SKIP:
        t_result->t_skip++;
        break;
      }
      post_log("---Test %2d - %s - %s\n\n",(loop_count + 1), regression[loop_count].func_name,status == -1? "FAILED" : "PASSED");
   }
    return 0;
  }
  else{
    
    post_log("Total Number of Test : %d",total_tests);    
    do{
      for(loop_count = 0 ; loop_count  < total_tests ; loop_count++ )
        post_log("\n%2d %s",(loop_count + 1), function[loop_count].func_name);
      post_log("\n%2d exit",(total_tests + 1));
      test_no = post_getUserInput ("\nEnter Test Number : ");
      if(test_no == (total_tests + 1)){
      }
      else if(test_no > total_tests )
        post_log("ERROR - invalid entry\n");  
      else
        status += (*function[test_no - 1].func)();
    }while(test_no != (total_tests + 1));

    return status;
  }
}
#endif
#if 0
static int test_bcm_IsBBLAvailable()
{
uint32_t bbl_return =  0x00;//bcm_IsBBLAvailable();

if(BBL_AVAILABLE == bbl_return)
    post_log("bcm_IsBBLAvailable() return = BBL_AVAILABLE\n");

else if(BBL_NOT_AVAILABLE == bbl_return)
    post_log("bcm_IsBBLAvailable() return = BBL_NOT_AVAILABLE\n");

else if(BBL_AVAILABILITY_CHECK_FAILED == bbl_return)
    post_log("bcm_IsBBLAvailable() return = BBL_AVAILABILITY_CHECK_FAILED\n");
}

#endif

int BBL_post_test(int flags)
{
    uint32_t test_no;
    int loop_count = 0, total_tests = 0 ;
    int status = 0,autorun_status = 0,pass = 0,fail = 0,skip = 0, data;
    struct DMA_Controller dmac;

	struct DmuBlockEnable dbe;
	int max_loop = 50;


    initDMA(&dmac);

    //CLK Gating ctrl
//    wr(ASIU_TOP_CLK_GATING_CTRL,0x7ff);
//    wr(CRMU_CLOCK_GATE_CONTROL,0xC000073f);
//    wr(CRMU_CLOCK_GATE_CTRL,0x1);
//
    while(SPRU_IS_POWERED_BY_AON != dmu_bbl_interface_active())
    {
      if(!(max_loop--)){
          post_log("error : SPRU is powered by AON is not set even after 50 times polling\n");
          return TEST_FAIL;
      }
    }

    dmu_bbl_if_reset_cycle();
  /* Init required for BBL access */
  bbl_disable_isolation_cell();
  bbl_auth_code(0x100);
  bbl_auth_check(0x100);
  cpu_wr_single (SPRU_BBL_CMD, 0x0, 4);
  cpu_wr_single (SPRU_BBL_CMD, 0x400, 4);
  /* init alarm polarity and CMOS output */
  bbl_rd_indirect_reg (BBL_CONFIG, (uint32_t *)&data);
  bbl_wr_indirect_reg(BBL_CONFIG, data | 0x5); 


    dbe.crypto = 1;
     bcm_dmu_block_enable(dbe);


    FUNC_INFO function[]=   {
                 ADD_TEST(  bbl_sec1280_rd_wr    ),
                 ADD_TEST(  bbl_sec1280_wr       ),
                 ADD_TEST(  bbl_sec1280_check   ),
                 ADD_TEST(  bbl_kek_rd_wr        ),
                 ADD_TEST(  bbl_rtc_set_div_secs ),
                 ADD_TEST(  bbl_simulate_tamper  ),
                 ADD_TEST(  test_sha256          ),
                 ADD_TEST(  bbl_tamper_p0n0_enable         ),
                 ADD_TEST(  bbl_tamper_p0n0_disable   ),
                 ADD_TEST(  bbl_external_mesh_enable        ),
                 ADD_TEST(  bbl_external_mesh_disable       ),
                 ADD_TEST(  bbl_temp_sensor_enable          ),
                 ADD_TEST(  bbl_temp_sensor_disable          ),
                 ADD_TEST(  bbl_freq_monitor_enable          ),
                 ADD_TEST(  bbl_freq_monitor_disable          ),
                 ADD_TEST(  bbl_voltage_monitor_enable          ),
                 ADD_TEST(  bbl_voltage_monitor_disable          ),
                 ADD_TEST(  bbl_monitor_tampers          ),
                 ADD_TEST(  bbl_set_rtc          ),      
                 ADD_TEST(  bbl_clear_tampers          ),
                 ADD_TEST(  bbl_kek_validity     ),
                 ADD_TEST(  bbl_soft_reset      )
                } ;

    FUNC_INFO regression_func[]=    {
                 ADD_TEST(  bbl_sec1280_rd_wr    ),
                 ADD_TEST(  bbl_sec1280_wr    ),
                 ADD_TEST(  bbl_kek_rd_wr        ),
                 ADD_TEST(  bbl_rtc_set_div_secs ),
                 ADD_TEST(  bbl_simulate_tamper  ),
                 ADD_TEST(  bbl_kek_validity     )
                } ;


    total_tests = (sizeof(function) / sizeof(function[0]));
    
#if 0
    int total_reg_tests = 0;
	FUNC_INFO regression_func[]=  {
				   ADD_TEST(  bbl_sec1280_rd_wr    ),
				   ADD_TEST(  bbl_sec1280_wr	),
				   ADD_TEST(  bbl_kek_rd_wr 	   ),
				   ADD_TEST(  bbl_rtc_set_div_secs ),
				   ADD_TEST(  bbl_simulate_tamper  ),
				   ADD_TEST(  bbl_kek_validity	   )
				  } ;
	total_reg_tests = (sizeof(regression_func) / sizeof(regression_func[0]));

   if(flags & POST_REGRESSION ){

    for(loop_count = 0 ; loop_count  < total_reg_tests ; loop_count++ ){
      post_log("\nExecuting test %2d - %s\n",(loop_count + 1), regression_func[loop_count].func_name);

    status = (*regression_func[loop_count].func)();

    switch(status)
    {
      case TEST_PASS:
      pass++;
      break;
      case TEST_FAIL:
      fail++;
      break;
      case TEST_SKIP:
      skip++;
      break;
    }
     post_log("Report TOTAL Test => %d - PASS => %d - FAIL => %d - SKIP => %d\n",(pass + fail + skip), pass,fail,skip);
     if(fail)
          status = -1;

    }
    return 0;
    }
else{
#endif

    post_log("Number of test : %d",total_tests);

    do{
         for(loop_count = 0 ; loop_count  < total_tests ; loop_count++ ) {
             post_log("\n%2d %s",(loop_count + 1), function[loop_count].func_name);
         }
        
         post_log("\n%2d run all tests except reset",(total_tests + 1));
         post_log("\n%2d exit\n",(total_tests + 2));
         test_no = post_getUserInput ("\nEnter Test Number : ");

         if (test_no > (total_tests + 2)) {
             post_log("ERROR - invalid entry\n");
         } else {
                  if (test_no == (total_tests + 1)) {
                      autorun_status = 0;
                      pass = 0;
                      fail = 0;
                      skip = 0;
                      for (loop_count = 0 ; loop_count  < total_tests - 1; loop_count++ ) {
                          post_log("\nExecuting test no : %2d. %s\n",(loop_count + 1),function[loop_count].func_name);
                          autorun_status = (*function[loop_count].func)();
                          switch(autorun_status) {
                              case TEST_PASS:
                                  pass++;
                                  break;
                              case TEST_FAIL:
                              	  post_log("\nTest failed for test no: %2d\n", (loop_count + 1));
                                  fail++;
                                  break;
                              case TEST_SKIP:
                                  skip++;
                                  break;
                              default:                              	  
                                  pass++;
                                  break;
                          }
                      }
                      post_log("Report TOTAL Test => %d - PASS => %d - FAIL => %d - SKIP => %d\n",(pass + fail + skip), pass,fail,skip);
                      if (fail)
                        status = -1;
                  } else if(test_no < (total_tests + 1)){
                      if(TEST_FAIL == (*function[test_no - 1].func)())
                          status = -1;
                  }
             }
    } while(test_no != (total_tests + 2));

    bcm_dmu_block_disable(dbe);
    return status;
}

#endif
#endif
