
/*
 * ov5640_cntrl.c
 *
 *  Created on: Apr 4, 2014
 *      Author: eappenm
 */
/*******************************************************************
 *
 *  Broadcom Cygnus Project
 *  File: ov5640_cntrl.c
 *  Description: the cam diagnostic test file
 *
 *  $Copyright (C) 2013 Broadcom Corporation$
 *  Version: $Id: $
 *
 *******************************************************************/

#include <common.h>
#include <post.h>

#if defined(CONFIG_CYGNUS)
#if CONFIG_POST & CONFIG_SYS_POST_CAM

#include "../../halapis/include/allregread_apis.h"
#include "../../halapis/include/cygnus_types.h"
#include "../../arch/arm/include/asm/arch-cygnus/socregs.h"
#include "ov5640_config.h"

#define SCCB_SLAVE_ID  0x3C

void ov5640_set_aetarget(uint32_t target)
{
  int fast_high, fast_low,AE_low,AE_high;
  AE_low = target * 23 / 25; /* 0.92 */
  AE_high = target * 27 / 25; /* 1.08 */
  fast_high = AE_high << 1;
  if (fast_high > 255)
	  fast_high = 255;
  fast_low = AE_low >> 1;
  ov5640_reg_write(SCCB_SLAVE_ID,0x3a0f, AE_high);
  ov5640_reg_write(SCCB_SLAVE_ID,0x3a10, AE_low);
  ov5640_reg_write(SCCB_SLAVE_ID,0x3a1b, AE_high);
  ov5640_reg_write(SCCB_SLAVE_ID,0x3a1e, AE_low);
  ov5640_reg_write(SCCB_SLAVE_ID,0x3a11, fast_high);
  ov5640_reg_write(SCCB_SLAVE_ID,0x3a1f, fast_low);
  //return 0;
}

int ov5640_get_sysclk(void)
{
   int xvclk = 24000000 / 10000;
   int sysclk;
   int temp1, temp2;
   int Multiplier, PreDiv, VCO, SysDiv, Pll_rdiv, Bit_div2x, sclk_rdiv;
   int sclk_rdiv_map[] = {1, 2, 4, 8};
   uint8_t regval = 0;
   ov5640_reg_write_1(SCCB_SLAVE_ID,0x3034);
   temp1 = ov5640_reg_read(0x3034);
   temp2 = temp1 & 0x0f;
   if (temp2 == 8 || temp2 == 10) 
     {
	   Bit_div2x = temp2 / 2;   
	 } else { 
		 post_log("ov5640: unsupported bit mode %d\n", temp2);
		 return -1;
	 }
	 
	 ov5640_reg_write_1(SCCB_SLAVE_ID,0x3035);
     temp1 = ov5640_reg_read(0x3035);
	 SysDiv = temp1 >> 4;
	 if (SysDiv == 0)
	     SysDiv = 16;
	 ov5640_reg_write_1(SCCB_SLAVE_ID,0x3036);
     temp1 = ov5640_reg_read(0x3036);
	 Multiplier = temp1;
	 ov5640_reg_write_1(SCCB_SLAVE_ID,0x3037);
     temp1 = ov5640_reg_read(0x3037);
	 PreDiv = temp1 & 0x0f;
	 Pll_rdiv = ((temp1 >> 4) & 0x01) + 1;
	 ov5640_reg_write_1(SCCB_SLAVE_ID,0x3108);
     temp1 = ov5640_reg_read(0x3108);
	 temp2 = temp1 & 0x03;
	 sclk_rdiv = sclk_rdiv_map[temp2];
	 VCO = xvclk * Multiplier / PreDiv;
	 sysclk = VCO / SysDiv / Pll_rdiv * 2 / Bit_div2x / sclk_rdiv;
	 post_log("The Calculated sysclk is %d \n", sysclk);
	 return sysclk;
}

int ov5640_get_HTS(void)
{
	int HTS;
	ov5640_reg_write_1(SCCB_SLAVE_ID,0x380c);
	HTS = ov5640_reg_read(0x380c);
    ov5640_reg_write_1(SCCB_SLAVE_ID,0x380d);
	HTS = (HTS<<8) + ov5640_reg_read(0x380d);
	return HTS;
}


int ov5640_get_VTS(void)
{
	int VTS;
	ov5640_reg_write_1(SCCB_SLAVE_ID,0x380e);
	VTS = ov5640_reg_read(0x380e);
    ov5640_reg_write_1(SCCB_SLAVE_ID,0x380f);
	VTS = (VTS<<8) + ov5640_reg_read(0x380f);
	return VTS;
}


void ov5640_set_bandingfilter(void)
{
	int prev_VTS;
	int band_step60, max_band60, band_step50, max_band50;
	int prev_sysclk;
	int prev_HTS;
	/* read preview PCLK */
	prev_sysclk = ov5640_get_sysclk();
	/* read preview HTS */
	prev_HTS = ov5640_get_HTS();
	/* read preview VTS */
	prev_VTS = ov5640_get_VTS();
	/* calculate banding filter */
	/* 60Hz */
	band_step60 = prev_sysclk * 100/prev_HTS * 100/120;
	ov5640_reg_write(SCCB_SLAVE_ID,0x3a0a, (band_step60 >> 8));
	ov5640_reg_write(SCCB_SLAVE_ID,0x3a0b, (band_step60 & 0xff));
	max_band60 = (int)((prev_VTS-4)/band_step60);
	ov5640_reg_write(SCCB_SLAVE_ID,0x3a0d, max_band60);
	
	/* 50Hz */
	band_step50 = prev_sysclk * 100/prev_HTS;
	ov5640_reg_write(SCCB_SLAVE_ID,0x3a08, (band_step50 >> 8));
	ov5640_reg_write(SCCB_SLAVE_ID,0x3a09, (band_step50 & 0xff));
	max_band50 = (int)((prev_VTS-4)/band_step50);
	ov5640_reg_write(SCCB_SLAVE_ID,0x3a0e, max_band50);
}

/** SM Bus ***************************/

void cam_add_delay(uint32_t delay)
 {
	 uint32_t i;
	 for(i=0;i<delay;i++);
 }
/**************************************/
void CAM_GPIO_output_pad_configure (void)
{

	uint32_t read_val,mask;
	//-	Bit 7  value controls          gpio_71/trigger output
	//-	Bit 9  value controls          gpio_73/stndby output
	//-	Bit 10  - value controls       gpio_74/reset  output

	//Function 1 for chip_gpio98,chip_gpio97
	cpu_wr_single(CRMU_IOMUX_CTRL7,3,4);

#if 1   //change by sathish
	read_val = cpu_rd_single(ASIU_GP_OUT_EN_2,4);
	mask     = (1<<7)| (1<<9)|(1<<10);
	read_val = read_val | mask;
	cpu_wr_single(ASIU_GP_OUT_EN_2, read_val,4);
#endif

	read_val = cpu_rd_single(ASIU_GP_DATA_OUT_2,4);
	mask = 0xfffff97f;
	read_val = read_val & mask;
	cpu_wr_single(ASIU_GP_DATA_OUT_2, read_val,4);

}

void cam_gpio_write ( gpio_type output, enable_disable control)
{

	uint32_t read_value,mask;

	read_value = cpu_rd_single(ASIU_GP_DATA_OUT_2,4);

	switch (output)
	{

	case reset_pin:
		if (control == data_high)
		{
          mask = 1 << 10 | 0 << 9;
          read_value = read_value | mask;
          post_log("Ddeasserting the Reset And Pulling Standby low....\n");
		}
		else
		{
			mask = 1<<10;
			mask = ~mask;
			read_value = read_value & mask;
		}

		break;

	case standby_pin:
		if (control == data_high)
		{
			 mask = 1 << 9;
			 read_value = read_value | mask;

		}
		else
		{
			mask = 1<<9;
			mask = ~mask;
			read_value = read_value & mask;

		}

		break;

	case trigger_pin:
		if (control == data_high)
		{
			 mask = 1 << 7;
			 read_value = read_value | mask;


		}
		else
		{
			mask = 1<<7;
			mask = ~mask;
			read_value = read_value & mask;

		}

		break;


	}

	cpu_wr_single(ASIU_GP_DATA_OUT_2, read_value,4);

}

/**********************************/
uint32_t check_memory(uint32_t address)
{

	uint32_t temp,read_value;
	uint32_t error_cnt;
	volatile uint32_t *pointer;

	pointer = (uint32_t*)address;

	post_log("\n....memory prints..\n");

    error_cnt = 0;

	for ( temp = 1 ; temp <= 2500; temp++)
	{
		//read_value = cpu_rd_single(address,4);
		read_value = *pointer;
        if (read_value == 0xdeadbeef)
        {
          post_log("Error for address : 0x%08X Value Read : 0x%08X \n",*pointer,pointer);
          error_cnt++;
        }
		pointer++;
	}

	post_log("\n....end ..\n");
    return error_cnt;
}
/***************************/
void set_memory(uint32_t address)
{
	uint32_t temp,write_value,write_value1;
	volatile uint32_t *pointer;

	pointer = (uint32_t*)address;
	write_value = 0xdeadbeef;

	post_log("\n....set memory..\n");
	post_log("\n....Setting 10000 bytes starting from 0x60100000....with 0xdeadbeef \n");

	for ( temp = 1 ; temp <= 2500; temp++)
	{
		
	   *pointer = write_value;		
		pointer++;
	}


	post_log("\n...set. mem...end ..\n");

}

/************************************/

void cam_smbus_init(uint32_t speed_mode)
 {
	 uint32_t data;

	     data = cpu_rd_single(ChipcommonG_SMBus0_SMBus_Config,4);
	     post_log("ChipcommonG_SMBus0_SMBus_Config = 0x%08X\n", data);
	     data |= (1 << ChipcommonG_SMBus0_SMBus_Config__SMB_EN);
	     cpu_wr_single(ChipcommonG_SMBus0_SMBus_Config, data,4);
	     data = cpu_rd_single(ChipcommonG_SMBus0_SMBus_Config,4);
	     post_log("ChipcommonG_SMBus0_SMBus_Config = 0x%08X\n", data);

	     data = cpu_rd_single(ChipcommonG_SMBus0_SMBus_Config,4);
	     data |= (1 << ChipcommonG_SMBus0_SMBus_Config__RESET);
	     cpu_wr_single(ChipcommonG_SMBus0_SMBus_Config, data,4);
	     data &= ~(1 << ChipcommonG_SMBus0_SMBus_Config__RESET);
	     cpu_wr_single(ChipcommonG_SMBus0_SMBus_Config, data,4);

	      if(speed_mode) {
	            data = cpu_rd_single(ChipcommonG_SMBus0_SMBus_Timing_Config,4);
	            data |= (1 << ChipcommonG_SMBus0_SMBus_Timing_Config__MODE_400);
	            cpu_wr_single(ChipcommonG_SMBus0_SMBus_Timing_Config, data,4);
	            post_log("Configuring SMBUS in 400KHz mode\n");
	        } else {
	            data = cpu_rd_single(ChipcommonG_SMBus0_SMBus_Timing_Config,4);
	            data &= ~(1 << ChipcommonG_SMBus0_SMBus_Timing_Config__MODE_400);
	            cpu_wr_single(ChipcommonG_SMBus0_SMBus_Timing_Config, data,4);
	            post_log("Configuring SMBUS in 100KHz mode\n");
	        }
 }
/******************************************/
int cam_smbus_slave_presence(uint32_t slave_address)
{
	 /*uint32_t data,slave_addr;

	 for(slave_addr = 0;slave_addr <= 0x7F;slave_addr++)
	 {
	 cpu_wr_single(ChipcommonG_SMBus0_SMBus_Master_Data_Write, (slave_addr<<1), 4);

	 cpu_wr_single(ChipcommonG_SMBus0_SMBus_Master_Command, ((0 << 9) | (1<<31)), 4);

	  do {
	        data = cpu_rd_single(ChipcommonG_SMBus0_SMBus_Master_Command, 4);
	         data &= (1 << 31);

	      } while (data);

	     data = cpu_rd_single(ChipcommonG_SMBus0_SMBus_Master_Command, 4);
	     data = (data >> 25) & 0x7;

	     if(data == 0)
	     {
	       post_log("Slave present with address: 0x%08x\n", slave_addr);
	       return 0;
	      }
	      //else
	      //{
	       // post_log("Error status for write tranasction - : 0x%08x - Addr : 0x%08X\n", data, slave_address);
	       // return -1;
	     // }
	 }*/

	uint32_t data,slave_address1;
	post_log("Inside SMbus Diag \n");
		 for(slave_address1=0;slave_address1<=0x7F;slave_address1++)
		 {
		 cpu_wr_single(ChipcommonG_SMBus0_SMBus_Master_Data_Write, (slave_address1<<1), 4);
		            cpu_wr_single(ChipcommonG_SMBus0_SMBus_Master_Command, ((0 << ChipcommonG_SMBus0_SMBus_Master_Command__SMBUS_PROTOCOL_R) | (1<<31)), 4);
		            do {
		                data = cpu_rd_single(ChipcommonG_SMBus0_SMBus_Master_Command, 4);
		                data &= (1 << 31);
		            } while (data);
		            data = cpu_rd_single(ChipcommonG_SMBus0_SMBus_Master_Command, 4);
		            data = (data >> 25) & 0x7;

		            if(data == 0) {
		                post_log("Slave present with address: 0x%08x\n", slave_address1);
		                //return 0;
		            } //else {
		               // post_log("Error status for write tranasction - : 0x%08x - Addr : 0x%08X\n", data, slave_address1);
		                //return -1;
		            //}
		 }
}
/*******************************/
int ov5640_reg_read(uint32_t address)
{
	uint32_t data,slave_addr;
	uint8_t value;

	slave_addr = SCCB_SLAVE_ID;

 	   cpu_wr_single(ChipcommonG_SMBus0_SMBus_Master_Data_Write, ((slave_addr << 1) | 1 | (1 << 31)), 4);
 	  //cpu_wr_single(ChipcommonG_SMBus0_SMBus_Master_Data_Write,3, 4);
       //cpu_wr_single(ChipcommonG_SMBus0_SMBus_Master_Data_Write, ((address & 0xFF00)>>8), 4);
       //cpu_wr_single(ChipcommonG_SMBus0_SMBus_Master_Data_Write, ((address & 0x00FF)|(1 << 31)), 4);

       /* byte read */
       cpu_wr_single(ChipcommonG_SMBus0_SMBus_Master_Command, ((0x4 << 9) | (1<<31)|(0x01)), 4);

       //dummy read
       data = cpu_rd_single(ChipcommonG_SMBus0_SMBus_Master_Command, 4);

       do {
           data = cpu_rd_single(ChipcommonG_SMBus0_SMBus_Master_Command, 4);
           data &= (1 << 31);
           } while (data);

        /*check for smb_errors*/
        data = cpu_rd_single(ChipcommonG_SMBus0_SMBus_Master_Command, 4);
        data = (data >> 25) & 0x7;

        if(data != 0)
        {
            post_log("\n\rError occured ...!!!\n\r");
            return -1;
        }
        else
 		{

 	       do
 	       {
              data = cpu_rd_single(ChipcommonG_SMBus0_SMBus_Master_Data_Read, 4);
              value = data & 0x000000FF;
              data = (data & 0xc0000000);
           } while (data != 0xc0000000);
 	      //post_log("DATA : %08X \n",data);
 		    //data = cpu_rd_single(ChipcommonG_SMBus0_SMBus_Master_Data_Read, 4);
 		    //value = data & 0x000000FF;
			post_log("Read register Value : %d \n",value);
 		    return value;
 		}


}

int ov5640_reg_write_1(uint32_t slave_addr,uint32_t address)
{
	uint32_t data;

	post_log("Chosen Slave Address : %02X \n",slave_addr);
	post_log("Chosen Register Address Byte 1: %02X \n",((address & 0xFF00)>>8));
	post_log("Chosen Register Address Byte 2: %02X \n",((address & 0x00FF)));

 	   cpu_wr_single(ChipcommonG_SMBus0_SMBus_Master_Data_Write, ((slave_addr << 1) | 0), 4);
 	   //cpu_wr_single(ChipcommonG_SMBus0_SMBus_Master_Data_Write, 3, 4);
 	   //cpu_wr_single(ChipcommonG_SMBus0_SMBus_Master_Data_Write, (address & 0x00FF), 4);
       cpu_wr_single(ChipcommonG_SMBus0_SMBus_Master_Data_Write, ((address & 0xFF00)>>8), 4);
       cpu_wr_single(ChipcommonG_SMBus0_SMBus_Master_Data_Write, (address & 0x00FF) | (1 << 31), 4);
       //cpu_wr_single(ChipcommonG_SMBus0_SMBus_Master_Data_Write, (value | (1 << 31)), 4);
       /* block write */
       cpu_wr_single(ChipcommonG_SMBus0_SMBus_Master_Command, ((0x5 << 9) | (1<<31)), 4);

       //dummy read
       data = cpu_rd_single(ChipcommonG_SMBus0_SMBus_Master_Command, 4);

       do {
           data = cpu_rd_single(ChipcommonG_SMBus0_SMBus_Master_Command, 4);
           data &= (1 << 31);
           } while (data);

        /*check for smb_errors*/
        data = cpu_rd_single(ChipcommonG_SMBus0_SMBus_Master_Command, 4);
        data = (data >> 25) & 0x7;

        if(data != 0)
        {
            post_log("\n\rError occured ...!!!\n\r");
            return -1;
        }
        else
 		{


			post_log("\n i2c reg write ok.. for reg %x for value   \n",address);
 		    return 0;
 		}


}

/*********************************/
int ov5640_reg_write(uint32_t slave_addr,uint32_t address,uint32_t value)
{
	uint32_t data;

	post_log("Chosen Slave Address : %02X \n",slave_addr);
	post_log("Chosen Register Address Byte 1: %02X \n",((address & 0xFF00)>>8));
	post_log("Chosen Register Address Byte 2: %02X \n",((address & 0x00FF)));

 	   cpu_wr_single(ChipcommonG_SMBus0_SMBus_Master_Data_Write, ((slave_addr << 1) | 0), 4);
 	   //cpu_wr_single(ChipcommonG_SMBus0_SMBus_Master_Data_Write, 3, 4);
 	   //cpu_wr_single(ChipcommonG_SMBus0_SMBus_Master_Data_Write, (address & 0x00FF), 4);
       cpu_wr_single(ChipcommonG_SMBus0_SMBus_Master_Data_Write, ((address & 0xFF00)>>8), 4);
       cpu_wr_single(ChipcommonG_SMBus0_SMBus_Master_Data_Write, (address & 0x00FF), 4);
       cpu_wr_single(ChipcommonG_SMBus0_SMBus_Master_Data_Write, (value | (1 << 31)), 4);
       /* block write */
       cpu_wr_single(ChipcommonG_SMBus0_SMBus_Master_Command, ((0x7 << 9) | (1<<31)), 4);

       //dummy read
       data = cpu_rd_single(ChipcommonG_SMBus0_SMBus_Master_Command, 4);

       do {
           data = cpu_rd_single(ChipcommonG_SMBus0_SMBus_Master_Command, 4);
           data &= (1 << 31);
           } while (data);

        /*check for smb_errors*/
        data = cpu_rd_single(ChipcommonG_SMBus0_SMBus_Master_Command, 4);
        data = (data >> 25) & 0x7;

        if(data != 0)
        {
            post_log("\n\rError occured ...!!!\n\r");
            return -1;
        }
        else
 		{


			post_log("\n i2c reg write ok.. for reg %x for value %x   \n",address,value);
 		    return 0;
 		}


}
/********************************/
static int ov5640_reg_writes( const struct ov5640_reg reglist[])
{
	int err = 0, index;
	uint32_t slave_addr;

	slave_addr = SCCB_SLAVE_ID;

	for (index = 0; ((reglist[index].reg != 0xFFFF) && (err == 0)); index++)
	{
		err |= ov5640_reg_write(slave_addr, reglist[index].reg,reglist[index].val);
		cam_add_delay(2000);
		//soft delay of 10 microsecond added.
		//plus ..smbus_default_idle time is 50 micor second

	}
	return 0;
}
/*******************************/
int ov5640_common_init(void)
{

	int ret = 0;

    post_log("Global initialization of the Sensor starts Here \n");
	ret = ov5640_reg_writes(global_init);
	post_log("Global initialization of the Sensor Ends Here \n");

	return ret;
}
/**************************/
/*******************************/
int ov5640_isp_init(void)
{

	int ret = 0;

    post_log("VGA specific initialization of the sensor starts here \n");
	ret = ov5640_reg_writes(configscript_common1);
	 post_log("VGA specific initialization of the sensor Ends here \n");

	return ret;
}

/**************************/
/*******************************/
int ov5640_power_down(void)
{

	int ret = 0;

	ret = ov5640_reg_writes(power_down);

	return ret;
}
/**************************/
/*******************************/
int ov5640_capture_config(void)
{

	int ret = 0;

	ret = ov5640_reg_writes(capture_config);

	return ret;
}
/**************************/

#endif
#endif





