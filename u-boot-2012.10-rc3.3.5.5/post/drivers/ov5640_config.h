/*
 * ov5640_config.h
 *
 *  Created on: Apr 4, 2014
 *      Author: eappenm
 */

#ifndef OV5640_CONFIG_H_
#define OV5640_CONFIG_H_

#include <common.h>
#include <post.h>

#include "../../halapis/include/allregread_apis.h"
#include "../../halapis/include/cygnus_types.h"

#define AE_TARGET 52

typedef enum { reset_pin,standby_pin,trigger_pin} gpio_type;
typedef enum { data_low,data_high} enable_disable;
/**************/
extern int ov5640_common_init(void);
extern int ov5640_isp_init(void);
extern int ov5640_capture_config(void);
extern void cam_add_delay(uint32_t delay);
extern void cam_smbus_init(uint32_t speed_mode);
extern int cam_smbus_slave_presence(uint32_t slave_address);
extern void print_memory(uint32_t address);
extern void set_memory(uint32_t address);
extern void CAM_GPIO_output_pad_configure (void);
extern void cam_gpio_write ( gpio_type output,enable_disable control);
extern int ov5640_power_down(void);
extern int ov5640_reg_read(uint32_t address);
extern int ov5640_reg_write(uint32_t address,uint32_t addr,uint32_t value);
extern int ov5640_reg_write_1(uint32_t address,uint32_t addr);
//extern int ov5640_driver_capability(uint32_t strength);
extern void ov5640_set_bandingfilter(void);
extern int ov5640_get_HTS(void);
extern int ov5640_get_VTS(void);
extern int ov5640_get_sysclk(void);
extern void ov5640_set_aetarget(uint32_t target);

/****************/




struct ov5640_reg {
	u16 reg;
	u8 val;
};


static const struct ov5640_reg global_init[] = {
	//{0x3008, 0x42},  //software powerdown mode. In this mode, all register content is maintained in standby mode

	 //{0x3008, 0x02}, //Software powered up back again
	 {0xFFFF, 0x00}
};

static const struct ov5640_reg configscript_common1[] = {
	{0x3008,0x82},
	{0x3008,0x42},
	{0x3000,0xff},
	{0x3002,0xff},
	{0x3000,0x00},
	{0x3002,0x00},
	{0x3004,0xCf},
	{0x3006,0xff},
	{0x3016,0x02},
	{0x3017,0xff},
	{0x3018,0xff},
	{0x3034,0x18},     //set 8-bit mode instead of 10-bit mode which is the default - from
	{0x300e,0x58},     //from
	{0x302c,0x02},
	{0x3103,0x03},
	{0x3035,0x21},
	{0x3036,0x46},
	{0x460c,0x22},
	{0x3824,0x02},
	//    //Configuration to Generate 20 KHz VCM clock
	/*VCM Control */
	{0x3600, 0x08},  //from
	{0x3601, 0x33},  //from
	{0x3604, 0x04},  //from
	{0x3605, 0xCD},  //from
	{0x3c07,0x08},
	{0x3820,0x40},
	{0x3821,0x00},
	{0x3814,0x31},
	{0x3815,0x31},
	{0x3803,0x04},
	{0x3807,0x9b},
	//{0x3808,0x02},
	//{0x3809,0x80},
	//{0x380a,0x01},
    //{0x380B,0xE0},
	{0x3808,0x01},
	{0x3809,0x40},
	{0x380a,0x00},
    {0x380B,0xF0},
	{0x380C,0x07},
	{0x380D,0x68},
	{0x380E,0x03},
	{0x380f,0xd8},
	{0x3813,0x06},
	{0x3037,0x13},
	{0x3108,0x01},
	{0x3800,0x00},
	{0x3801,0x00},
	{0x3802,0x00},
	{0x3803,0x04},
//	{0x3804,0x04},
//	{0x3805,0xf6},
//	{0x3806,0x02},
//	{0x3807,0xbc},
//	{0x3808,0x05},
//	{0x3809,0x00},
//	{0x380a,0x02},
//	{0x380b,0xd0},
//	{0x380c,0x06},
//	{0x380d,0x40},
//	{0x380e,0x02},
//	{0x380f,0xea},
	{0x3810,0x00},
    {0x3811,0x00},
    {0x3812,0x00},
	{0x3813,0x06},
    {0x471b,0x02},
	{0x4740,0x22}, //sathishs
	//{0x503D,0x80},
	//{0x4741,0x07}, //Test Pattern Generator
	//{0x4300,0x61},
	//{0x501f,0x01},
	{0x471d,0x01},
	{0x4745,0x00},
	
	/*AEC Controls */
	{0x3a13, 0x43},  //from
	{0x3a18, 0x00},  //from
	{0x3a19, 0xf8},  //from
	/*???? */
	//{0x3635, 0x13},
    //{0x3636, 0x03},
    //{0x3634, 0x40},
    //{0x3622, 0x01},
	{0x3635, 0x1c},  //from
	{0x3634, 0x40},  //from
	{0x3622, 0x01},  //from
	/*50/60Hz Detector */
	{0x3c01, 0x34},  //from
	{0x3c04, 0x28},  //from
	{0x3c05, 0x98},  //from
	{0x3c06, 0x00},  //from

	{0x3c07, 0x08},  //from
	{0x3c08, 0x00},  //from
	{0x3c09, 0x1c},  //from
	{0x3c0a, 0x9c},  //from
	{0x3c0b, 0x40},  //from

	//	/*AEC/AGC */
	{0x3a02, 0x03},  //from
	{0x3a03, 0xd8},  //from
	{0x3a08, 0x01},  //from
	{0x3a09, 0x27},  //from
	{0x3a0a, 0x00},  //from
	{0x3a0b, 0xf6},  //from
	{0x3a0e, 0x03},  //from
	{0x3a0d, 0x04},  //from
	{0x3a14, 0x03},  //from
	{0x3a15, 0xd8},  //from
//
//	/*BLC Control */
	{0x4001, 0x02},  //from
	{0x4004, 0x02},  //from
//
//	/*ISP Control */
	{0x5000, 0xa7},  //from
	{0x5001, 0x01},	  //from	/*isp scale down  Special Effects */

		//		/*AWB Control */
		{0x5180, 0xff},
		{0x5181, 0x56},
		{0x5182, 0x00},
		{0x5183, 0x14},
		{0x5184, 0x25},
		{0x5185, 0x24},
		{0x5186, 0x10},
		{0x5187, 0x14},
		{0x5188, 0x10},
		{0x5189, 0x81},
		{0x518a, 0x5a},
		{0x518b, 0xb6},
		{0x518c, 0xa9},
		{0x518d, 0x4c},
		{0x518e, 0x34},
		{0x518f, 0x60},
		{0x5190, 0x48},
		{0x5191, 0xf8},
		{0x5192, 0x04},
		{0x5193, 0x70},
		{0x5194, 0xf0},
		{0x5195, 0xf0},
		{0x5196, 0x03},
		{0x5197, 0x01},
		{0x5198, 0x04},
		{0x5199, 0x9b},
		{0x519a, 0x04},
		{0x519b, 0x00},
		{0x519c, 0x09},
		{0x519d, 0x1e},
		{0x519e, 0x38},
	//LENC setting
	/* LSC OV5640LENCsetting */
		{0x5800, 0x32},
		{0x5801, 0x1d},
		{0x5802, 0x19},
		{0x5803, 0x18},
		{0x5804, 0x1d},
		{0x5805, 0x38},
		{0x5806, 0x12},
		{0x5807, 0x0a},
		{0x5808, 0x07},
		{0x5809, 0x07},
		{0x580a, 0x0b},
		{0x580b, 0x0f},
		{0x580c, 0x0e},
		{0x580d, 0x05},
		{0x580e, 0x01},
		{0x580f, 0x00},
		{0x5810, 0x03},
		{0x5811, 0x0a},
		{0x5812, 0x0c},
		{0x5813, 0x04},
		{0x5814, 0x00},
		{0x5815, 0x00},
		{0x5816, 0x03},
		{0x5817, 0x0a},
		{0x5818, 0x12},
		{0x5819, 0x09},
		{0x581a, 0x06},
		{0x581b, 0x05},
		{0x581c, 0x09},
		{0x581d, 0x12},
		{0x581e, 0x32},
		{0x581f, 0x18},
		{0x5820, 0x14},
		{0x5821, 0x13},
		{0x5822, 0x17},
		{0x5823, 0x2d},
		{0x5824, 0x28},
		{0x5825, 0x2a},
		{0x5826, 0x28},
		{0x5827, 0x28},
		{0x5828, 0x2a},
		{0x5829, 0x28},
		{0x582a, 0x25},
		{0x582b, 0x24},
		{0x582c, 0x24},
		{0x582d, 0x08},
		{0x582e, 0x26},
		{0x582f, 0x42},
		{0x5830, 0x40},
		{0x5831, 0x42},
		{0x5832, 0x06},
		{0x5833, 0x26},
		{0x5834, 0x26},
		{0x5835, 0x44},
		{0x5836, 0x24},
		{0x5837, 0x2a},
		{0x5838, 0x4a},
		{0x5839, 0x2a},
		{0x583a, 0x0c},
		{0x583b, 0x2c},
		{0x583c, 0x28},
		{0x583d, 0xce},
	    {0x3b00, 0x00},
	    {0x3b08, 0x01},
	    {0x3004, 0xCf},
	    {0x3006, 0xc3},
	    {0x3a00, 0x78},
	    {0x3503, 0x00},
	    {0x4004, 0x06},
	   // {0x5001, 0x83}, 
	    	{0x3008,0x02},
        {0xFFFF,0x00}
};



static const struct ov5640_reg VGA_640_480_15fps[] = {
 {0x3008, 0x42},  //software powerdown mode. In this mode, all register content is maintained in standby mode
 {0x3c07, 0x08},  //Light meter 1 threshold 
 {0x3820, 0x41},  //Sensor Vflip
 {0x3821, 0x07},  //ISP mirror, sensor mirror and Horizontal binning enable
 {0x3814, 0x31},  //Horizontal odd and even subsample increment 
 {0x3815, 0x31},  //Vertical odd and even subsample increment
 {0x3800, 0x00},  //x addr start high byte
 {0x3801, 0x00},  //x addr start low byte
 {0x3802, 0x00},  //y addr start high byte 
 {0x3803, 0x04},  //y addr start low byte
 {0x3804, 0x0a},  //x addr end high byte
 {0x3805, 0x3f},  //x addr end low byte
 {0x3806, 0x07},  //y addr end high byte
 {0x3807, 0x9b},  //y addr end low byte
 {0x3808, 0x02},  //DVP Horizontal output width high byte
 {0x3809, 0x80},  //DVP Horizontal output width low byte
 {0x380a, 0x01},  //DVP Vertical output width high byte
 {0x380b, 0xe0},  //DVP Vertical output width high byte
 {0x380c, 0x07},  //Total Horizontal size high byte 
 {0x380d, 0x68},  //Total Horizontal size low byte
 {0x380e, 0x03},  //Total Vertical size high byte
 {0x380f, 0xd8},  //Total Vertical size low byte
 {0x3813, 0x06},  //ISP Vertical offset low byte
 {0x3818, 0x02},		 //programmed as per VGA timing diagram in OV5640 datasheet - high byte
 {0x3819, 0x80}, //hsync width - low byte
 {0x4740, 0x23},  
 {0x471d, 0x01},    //{0x471d, 0x10}
 {0x4745, 0x00},  
 {0x4709, 0x01},   //modified by sathishs
 {0x470a, 0x16},   //high byte
 {0x470b, 0x38},   //low byte
 {0x3618, 0x00},
 {0x3612, 0x29},
 {0x3709, 0x52},
 {0x370c, 0x03},
 {0x3a02, 0x0b},  //60 Hz AEC Maximum exposure output limit high byte
 {0x3a03, 0x88},  //60 Hz AEC Maximum exposure output limit low byte
 {0x3a14, 0x0b},  //50 Hz AEC Maximum exposure output limit high byte
 {0x3a15, 0x88},  //50 Hz AEC Maximum exposure output limit low byte   
 {0x4004, 0x02},  //specify the line number BLC process
 {0x3002, 0x1c},  //Enable blocks
 {0x3006, 0xc3},  //Enable blocks
 {0x4713, 0x03},  //JPG mode select 3
 {0x4407, 0x04},  //JPEF CTRL 07
 {0x460b, 0x35},
 {0x460c, 0x22},  //Clock is based on PLL clock divider
 {0x4837, 0x22},  //PCLK period 
 {0x3824, 0x02},  //[4:0] is PLL clock divider
 {0x5001, 0xa3},  //Majority of ISP features enabled
 {0x3034, 0x18},  //MIPI - 8bit mode - no need to configure though
 {0x3035, 0x21},  //SC PLL CTRL 1
 {0x3036, 0x46},  //PLL Multiplier
 {0x3037, 0x13},  //divided by 2 and pre divider is 3
 {0x3503, 0x00},  //Auto AGC and AEC
 {0x3008, 0x02}, //Software powered up back again
 {0xFFFF, 0x00}
};


static const struct ov5640_reg power_down[] = {
		{0x3008, 0x42},  /*  power down - 0x82- soft reset      */
		{0xFFFF, 0x00}
	};

static const struct ov5640_reg capture_config[] = {

	      //{0x3b00,0x00},  //may be 80 also - try it out
	      //{0x3b08,0x01}, //FREX request - may not be needed - sathish to figure out
		//{0x3004, 0xff},
		//{0x3006, 0xc3},
		//{0x3a00, 0x78},  /*  aec control      */
		//{0x3503, 0x00},	 /* AGC on, AEC on */
		//{0x4004, 0x06},	 /* BLC line number */
		//{0x5001, 0x83},		// SDE on scale off UV average off CMX on AWB on
		{0xFFFF, 0x00}
	};


#endif /* OV5640_CONFIG_H_ */



