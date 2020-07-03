

/*******************************************************************
 *
 *  Copyright 2013
 *  Broadcom Corporation
 *  16215 Alton Parkway
 *  Irvine CA 92619-7013
 *
 *  Broadcom provides the current code only to licensees who
 *  may have the non-exclusive right to use or modify this
 *  code according to the license.
 *  This program may be not sold, resold or disseminated in
 *  any form without the explicit consent from Broadcom Corp
 *
 *******************************************************************
 *
 *  Broadcom Cygnus Project
 *  File: cam_diags.c
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
//#include "../../halapis/include/allregread_apis.h"
#include "../../halapis/include/cygnus_types.h"
#include "asm/arch/socregs.h"
#include "../../halapis/include/audio_apis.h"
#include "../../halapis/include/lcd_apis.h"
#include "ov5640_config.h"


#define SCCB_SLAVE_ID  0x3C
#define IMAGE_START 0x60100000
#define IMAGE_END   0x60200000
#define DATA_END  0x60300000
#define DATA_START 0x60400000

#define CNTL (LCD_CNTL_LCDEN | LCD_CNTL_LCDTFT | LCD_CNTL_LCDPWR | LCD_CNTL_LCDVCOMP(3) | LCD_CNTL_WATERMARK )


typedef struct{

	char* func_name;
	int (*func)(void);
}FUNC_INFO;

struct config_param {
	uint32_t seq_asiu_cam_soft_reset_enable ;
	uint32_t seq_asiu_cam_cpi_data_width ;
	uint32_t seq_asiu_cam_cpi_mode ;
	uint32_t seq_asiu_cam_unpack_option ;
	uint32_t seq_asiu_cam_pack_option ;
	uint32_t cfg_asiu_cam_image_buff_start_addr ;
	uint32_t cfg_asiu_cam_image_buff_end_addr ;
	uint32_t cfg_asiu_cam_data_buff_end_addr ;
	uint32_t cfg_asiu_cam_data_buff_start_addr ;
	uint32_t cfg_asiu_cam_image_line_stride ;
	uint32_t seq_asiu_horz_window_enable ;
	uint32_t cfg_asiu_horz_window_value ;
	uint32_t seq_asiu_verti_window_enable ;
	uint32_t cfg_asiu_verti_window_value ;
	uint32_t seq_asiu_multiple_frames_en ;
	uint32_t cfg_asiu_cam_image_buff_start_addr1 ;
	uint32_t cfg_asiu_cam_image_buff_end_addr1 ;
	uint32_t cam_non_secure_en ;
	uint32_t cpi_clk_neg_edg_en ;
	uint32_t cpi_burst_space_enable ;
};

struct config_param test_config;

#define BIT8 1
#define BIT16 2
#define BIT32 4

#define error_log post_log
#define print_log post_log
#define MEMTOMEM 0
#define MEMTOPERIPH 1
#define ADD_TEST(X)  { #X , X }
#define TEST_PASS  0
#define TEST_FAIL -1
#define TEST_SKIP  2

uint32_t asiu_cam_load_coeff(void);


/*--------------------------- function -----------------------------------*/
//uint32_t asiu_cam_cpi_init_seq(uint32_t seq_asiu_cam_soft_reset_enable,uint32_t seq_asiu_cam_cpi_data_width,uint32_t seq_asiu_cam_cpi_mode,uint32_t cfg_asiu_horz_window_value,uint32_t cfg_asiu_verti_window_value )
uint32_t asiu_cam_cpi_init_seq(void)
{

	 uint32_t reg_value;
	//Deactivate Soft Reset

	if(test_config.seq_asiu_cam_soft_reset_enable == 0)
       cpu_wr_single (CAM_TOP_REGS_CAMCPIS, 0x00000000 , 4 );
	else
       cpu_wr_single (CAM_TOP_REGS_CAMCPIS, 0x00000020 , 4 );

	//Enable the CPI Interface

       cpu_wr_single (CAM_TOP_REGS_CAMCPIS, 0x00000001, 4 );

       if (test_config.seq_asiu_cam_cpi_data_width == 8) //8 bit data width
       cpu_wr_single (CAM_TOP_REGS_CAMCPIR, 0x00000000, 4 );
       else if (test_config.seq_asiu_cam_cpi_data_width == 10) //10 bit data width
       cpu_wr_single (CAM_TOP_REGS_CAMCPIR, 0x00000080 ,4 );
	   else
       cpu_wr_single (CAM_TOP_REGS_CAMCPIR, 0x00000000, 4 );

	if (test_config.seq_asiu_cam_cpi_mode == 0)
       {
      	// MODE=HV
    	cpu_wr_single (CAM_TOP_REGS_CAMCPIF, 0x00000000, 4);
		//cpu_wr_single(CAM_TOP_REGS_CAMCPIW, 0x00000001 ,4);
        //cpu_wr_single(CAM_TOP_REGS_CAMCPIWHC, 0x05000000 ,4);
        //cpu_wr_single(CAM_TOP_REGS_CAMCPIWVC, 0x02D00000 ,4);
	}

       else if (test_config.seq_asiu_cam_cpi_mode == 1)
       	{
       	// Embedded Mode

			cpu_wr_single(CAM_TOP_REGS_CAMCPIW, 0x00000001 ,4);
          		cpu_wr_single(CAM_TOP_REGS_CAMCPIWHC, 0x08000000 ,4);
          		cpu_wr_single(CAM_TOP_REGS_CAMCPIWVC, 0x00100000 ,4);
          		if (test_config.seq_asiu_cam_cpi_data_width == 10)  // defaults to 10 bit and we can disable embedded sync shift

          		{
			cpu_wr_single (CAM_TOP_REGS_CAMCPIF, (0x00000035 |   // Horizontal Sync Mode=HSAL,Vertical Sync Mode=VSAL,Sync Mode=Embedded
                       		           	1 << 12       |   // enable embedded sync shift
                                      			0 << 13       |   // shift the sync by 4 bits to the left
                                      			7 << 16       |   // vsync bit in the 10 bit data
                                      			6 << 20       |   // hsync bit in the 10 bit data
                                      			8 << 24       )   // field bit in the 10 bit data
                                     			, 4);
          		}
			else
         		{
            		cpu_wr_single (CAM_TOP_REGS_CAMCPIF, (0x00000035 |   // Horizontal Sync Mode=HSAL,Vertical Sync Mode=VSAL,Sync Mode=Embedded
                                      			1 << 12       |   // enable embedded sync shift
                                    			1 << 13       |   // shift the sync by 4 bits to the left
                                      			5 << 16       |   // vsync bit in the 8 bit data
                                      			4 << 20       |   // hsync bit in the 8 bit data
                                      			6 << 24       )   // field bit in the 8 bit data
                                      			, 4);
			}
		}


	else if (test_config.seq_asiu_cam_cpi_mode == 2)
       	{
         	//Packet Mode

          if (test_config.seq_asiu_cam_cpi_data_width == 8) //8 bit data width
            cpu_wr_single ( CAM_TOP_REGS_CAMCPIR, 0x00000002,4 );

          else if (test_config.seq_asiu_cam_cpi_data_width == 10)
            cpu_wr_single ( CAM_TOP_REGS_CAMCPIR, 0x00000082 ,4); //10 bit data width
          else
            cpu_wr_single ( CAM_TOP_REGS_CAMCPIR, 0x00000000 ,4);

          cpu_wr_single (CAM_TOP_REGS_CAMCPIF, 0x00000008,4);
          cpu_wr_single(CAM_TOP_REGS_CAMCPIW, 0x00000001 ,4);
          cpu_wr_single(CAM_TOP_REGS_CAMCPIWHC, test_config.cfg_asiu_horz_window_value,4);
          cpu_wr_single(CAM_TOP_REGS_CAMCPIWVC, test_config.cfg_asiu_verti_window_value ,4);

        	}
       else
        	{
          	//JPEG Mode
          cpu_wr_single (CAM_TOP_REGS_CAMCPIF, 0x00000000,4);
          cpu_wr_single(CAM_TOP_REGS_CAMCPIW, 0x00000001 ,4);
          cpu_wr_single(CAM_TOP_REGS_CAMCPIWHC, test_config.cfg_asiu_horz_window_value,4);
          cpu_wr_single(CAM_TOP_REGS_CAMCPIWVC, test_config.cfg_asiu_verti_window_value ,4);
        	}

        // Disable Windowing
        cpu_wr_single (CAM_TOP_REGS_CAMCPIW, 0x00000000,4);

        // Activate CPI Pipeline
        //cpu_wr_single (CAM_TOP_REGS_CAMCPIS, 0x00000003,4);

        //#2000; //Wait for the write to be completed

        // Detect FS, gen ints, capture
         //cpu_wr_single (CAM_TOP_REGS_CAMCMP0, 0x80000302,4 );

        // Detect FE, gen ints, capture
         //cpu_wr_single (CAM_TOP_REGS_CAMCMP1, 0x80000303,4 );

        //Enable Channel 0
        // Image data ID = 0 (Ch0) , bit 7 must be set to make it goto the image buffer
        cpu_wr_single (CAM_TOP_REGS_CAMIDI0,0x00000000,4);

        if (test_config.seq_asiu_cam_cpi_mode == 3)
        cpu_wr_single (CAM_TOP_REGS_CAMIDI0,0x00000000,4);
        else
        cpu_wr_single (CAM_TOP_REGS_CAMIDI0,0x00000080,4);  //bit 7 is set so that the incoming data will be saved into the Image buffer

        // Line stride 120h
        cpu_wr_single (CAM_TOP_REGS_CAMIBLS,0x00000140,4);

        // Start address
        cpu_wr_single (CAM_TOP_REGS_CAMIBSA0,IMAGE_START,4);

        // End address
        cpu_wr_single (CAM_TOP_REGS_CAMIBEA0,IMAGE_END,4);

        // Enable FS & FE interrupt
        //cpu_wr_single (CAM_TOP_REGS_CAMICTL,0x00000003,4);

        // Enable interrupts, 2 EDL's
        cpu_wr_single (CAM_TOP_REGS_CAMDCS,0x00000000,4);

        // Start address
        cpu_wr_single (CAM_TOP_REGS_CAMDBSA0,DATA_START,4);

        // End address
        cpu_wr_single (CAM_TOP_REGS_CAMDBEA0,DATA_END,4);

        //Enable peripheral
        cpu_wr_single (CAM_TOP_REGS_CAMCTL,0x0002050b,4);

		post_log(" configure reset and stndby  == Standby should be 0 and Reset should be 1 \n");
    reg_value = cpu_rd_single(CAM_TOP_REGS_CAMCTL,4);
    reg_value = reg_value & (~(1 << 21)); //Standby is made '0'
    reg_value = reg_value | (1 << 20); //Reset is made high
    reg_value = reg_value | 1; //Master Enable for the Camera Peripheral
    cpu_wr_single(CAM_TOP_REGS_CAMCTL,reg_value,4);
	cam_add_delay(200000);
	post_log(" Reset Made 1 and StandBy made 0 \n");

        // Enable CPI peripheral at the front end
        //cpu_wr_single ( `CAM_TOP_REGS_CAMFIX0,   0x0002_0000 ,4);

        // Enable CPI & make the FSP dump any remaining data on a frame end or linestart
        cpu_wr_single(CAM_TOP_REGS_CAMFIX0, (0x00000000 | 1<<17 | 1<<11 | 1<<10), 4);

        return 0;
}
/*--------------------------- function -----------------------------------*/
/*
uint32_t asiu_cam_test_init_seq(uint32_t seq_asiu_cam_soft_reset_enable,uint32_t seq_asiu_cam_cpi_data_width,uint32_t seq_asiu_cam_cpi_mode,
				uint32_t seq_asiu_cam_unpack_option,uint32_t seq_asiu_cam_pack_option,uint32_t cfg_asiu_cam_image_buff_start_addr,
				uint32_t cfg_asiu_cam_image_buff_end_addr,uint32_t cfg_asiu_cam_data_buff_end_addr,uint32_t cfg_asiu_cam_data_buff_start_addr,
				uint32_t cfg_asiu_cam_image_line_stride, uint32_t seq_asiu_horz_window_enable,uint32_t cfg_asiu_horz_window_value,uint32_t seq_asiu_verti_window_enable,
				uint32_t cfg_asiu_verti_window_value,uint32_t seq_asiu_multiple_frames_en, uint32_t cfg_asiu_cam_image_buff_start_addr1, uint32_t cfg_asiu_cam_image_buff_end_addr1,
				uint32_t cam_non_secure_en , uint32_t cpi_clk_neg_edg_en, uint32_t cpi_burst_space_enable)*/
uint32_t asiu_cam_test_init_seq(void)
{
      uint32_t data1;
      uint32_t wr_data;
      uint32_t rd_data;


      cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0, 4);

      data1 = cpu_rd_single(ASIU_TOP_CLK_GATING_CTRL,4);
      data1 = data1 | (1<< ASIU_TOP_CLK_GATING_CTRL__CAM_CLK_GATE_EN);
      cpu_wr_single(ASIU_TOP_CLK_GATING_CTRL,data1, 4);

        //Enable Camera As Secure Master
	if (test_config.cam_non_secure_en != 1)
	{
		  post_log("CAMERA APIS: CAMERA IS A SECURE MASTER\n");
          rd_data = cpu_rd_single (ASIU_TOP_CAM_AXI_SB_CTRL, 4);
          wr_data = rd_data &  0xffffe3ff;
          cpu_wr_single (ASIU_TOP_CAM_AXI_SB_CTRL, wr_data , 4);
          post_log("ASIU_TOP_CAM_AXI_SB_CTRL= %0x\n", wr_data);
	}
	else
	{
		  post_log("CAMERA APIS: CAMERA IS A NON-SECURE MASTER\n");
	}



      cpu_wr_single(CAM_TOP_REGS_CAMFIX0,0x00000000, 4);

      // Disable Horizontal Windowing
      cpu_wr_single(CAM_TOP_REGS_CAMIHWIN,0x00000000, 4);

      // Disable Vertical Windowing
      cpu_wr_single(CAM_TOP_REGS_CAMIVWIN,0x00000000, 4);

      // Disable Pixel Correction
      cpu_wr_single(CAM_TOP_REGS_CAMICC,0x00000000, 4);

      // Disable Downsizing
      cpu_wr_single(CAM_TOP_REGS_CAMIDC,0x00000000, 4);
      cpu_wr_single(CAM_TOP_REGS_CAMIDPO,0x00000000, 4);

      // Disable Data Control
      cpu_wr_single(CAM_TOP_REGS_CAMDCS,0x00000000, 4);

      //JK//Disable DPCM Decode, Pixel Correction, Down sizing, DPCM Encode, Packing
      cpu_wr_single(CAM_TOP_REGS_CAMIPIPE,0x00000000, 4);

      //Image Buffer Start Address
      cpu_wr_single(CAM_TOP_REGS_CAMIBSA0,0x00000000, 4);

      //Image Buffer End Address
      cpu_wr_single(CAM_TOP_REGS_CAMIBEA0,0x00000000, 4);

      //Image Buffer Line Stride
      cpu_wr_single(CAM_TOP_REGS_CAMIBLS,0x00000000, 4);

      //Data Buffer Start Address
      cpu_wr_single(CAM_TOP_REGS_CAMDBSA0, 0x00000000, 4);

      //Data Buffer End Address
      cpu_wr_single(CAM_TOP_REGS_CAMDBEA0, 0x00000000, 4);

      // Reset & re-initialise
      data1 = cpu_rd_single(CAM_TOP_REGS_CAMCTL,4);
      data1 |= 0x00000004;
      cpu_wr_single(CAM_TOP_REGS_CAMCTL,data1,4);
      data1 &= 0xFFFFFFFB;
      cpu_wr_single(CAM_TOP_REGS_CAMCTL,data1,4);

	  post_log("\n asiu_cam_cpi_init_seq \n");

      /*asiu_cam_cpi_init_seq(seq_asiu_cam_soft_reset_enable,seq_asiu_cam_cpi_data_width,seq_asiu_cam_cpi_mode,cfg_asiu_horz_window_value,cfg_asiu_verti_window_value);*/
      asiu_cam_cpi_init_seq();

      // Outside of init_csi2 to allow 108 onwards to change oet
      data1 = cpu_rd_single (CAM_TOP_REGS_CAMCTL,4);
      data1 &= 0xFFF00FFF;
      data1 |= 0x00020000;
      cpu_wr_single(CAM_TOP_REGS_CAMCTL,data1,4);

      // Extend timeout
      data1 = cpu_rd_single (CAM_TOP_REGS_CAMCTL,4);
      data1 &= 0xFFF00FFF;
      data1 |= 0x000FF000;
      cpu_wr_single (CAM_TOP_REGS_CAMCTL,data1,4);


      // Switch on CPI & line start flushing in FSP
      cpu_wr_single (CAM_TOP_REGS_CAMFIX0, (0x00000000 | 1<<17 | 1<<11 | 1<<10), 4);


     asiu_cam_load_coeff();

     if ((test_config.seq_asiu_cam_unpack_option == 0) && (test_config.seq_asiu_cam_pack_option == 1))
       //cpu_wr_single(CAM_TOP_REGS_CAMIPIPE, 0x00000080,4); //Unpack: None , Pack: PACK8
     {
       if (test_config.seq_asiu_cam_cpi_data_width == 10)
         cpu_wr_single(CAM_TOP_REGS_CAMIPIPE, 0x00000100,4); //Unpack: None , Pack: PACK10
       else
         cpu_wr_single(CAM_TOP_REGS_CAMIPIPE, 0x00000080,4); //Unpack: None , Pack: PACK8
     }


     else if ((test_config.seq_asiu_cam_unpack_option == 0) && (test_config.seq_asiu_cam_pack_option == 0))
       cpu_wr_single(CAM_TOP_REGS_CAMIPIPE, 0x00000000,4); //Unpack: None , Pack: None

     else if ((test_config.seq_asiu_cam_unpack_option == 1) && (test_config.seq_asiu_cam_pack_option == 0))
       //cpu_wr_single(CAM_TOP_REGS_CAMIPIPE, 0x00000003,4); //Unpack: RAW8 , Pack: None
     {
        if (test_config.seq_asiu_cam_cpi_data_width == 10)
         cpu_wr_single(CAM_TOP_REGS_CAMIPIPE, 0x00000004,4); //Unpack: RAW10 , Pack: None
       else
         cpu_wr_single(CAM_TOP_REGS_CAMIPIPE, 0x00000003,4); //Unpack: RAW8 , Pack: None
     }


     else if ((test_config.seq_asiu_cam_unpack_option == 1) && (test_config.seq_asiu_cam_pack_option == 1))
       //cpu_wr_single(CAM_TOP_REGS_CAMIPIPE, 0x00000083,4); //Unpack: RAW8 , Pack: PACK8
     {
	if (test_config.seq_asiu_cam_cpi_data_width == 10)
         cpu_wr_single(CAM_TOP_REGS_CAMIPIPE, 0x00000104,4); //Unpack: RAW10 , Pack: PACK10
       else
         cpu_wr_single(CAM_TOP_REGS_CAMIPIPE, 0x00000083,4); //Unpack: RAW8 , Pack: PACK8
     }

     //Horizontal Windowing
     if (test_config.seq_asiu_horz_window_enable == 1)
     {
       //cpu_wr_single(CAM_TOP_REGS_CAMIHWIN, cfg_asiu_horz_window_value ,4);
       cpu_wr_single(CAM_TOP_REGS_CAMCPIW, 0x00000001 ,4);
       cpu_wr_single(CAM_TOP_REGS_CAMCPIWHC, test_config.cfg_asiu_horz_window_value ,4);
       cpu_wr_single(CAM_TOP_REGS_CAMCPIF, 0x0000000A ,4);
         if (test_config.seq_asiu_cam_cpi_data_width == 8) //8 bit data width
           cpu_wr_single ( CAM_TOP_REGS_CAMCPIR, 0x00000003, 4  );
         else if (test_config.seq_asiu_cam_cpi_data_width == 10)
           cpu_wr_single ( CAM_TOP_REGS_CAMCPIR, 0x00000083 ,4 ); //10 bit data width
         else
           cpu_wr_single ( CAM_TOP_REGS_CAMCPIR, 0x00000003, 4 );
     }

     //Vertical Windowing
     if (test_config.seq_asiu_verti_window_enable == 1)
     {
       //cpu_wr_single(CAM_TOP_REGS_CAMIVWIN, cfg_asiu_verti_window_value ,4);
       cpu_wr_single(CAM_TOP_REGS_CAMCPIW, 0x00000001 ,4);
       cpu_wr_single(CAM_TOP_REGS_CAMCPIWVC, test_config.cfg_asiu_verti_window_value ,4);
       cpu_wr_single(CAM_TOP_REGS_CAMCPIF, 0x0000000A ,4);
         if (test_config.seq_asiu_cam_cpi_data_width == 8) //8 bit data width
           cpu_wr_single (CAM_TOP_REGS_CAMCPIR, 0x00000003, 4  );
         else if (test_config.seq_asiu_cam_cpi_data_width == 10)
           cpu_wr_single (CAM_TOP_REGS_CAMCPIR, 0x00000083 ,4 ); //10 bit data width
         else
           cpu_wr_single (CAM_TOP_REGS_CAMCPIR, 0x00000003, 4 );
     }

     //cpu_wr_single(CAMIBSA0, 0x00000000,4);
     cpu_wr_single(CAM_TOP_REGS_CAMIBSA0,test_config.cfg_asiu_cam_image_buff_start_addr,4);

     cpu_wr_single(CAM_TOP_REGS_CAMIBEA0,test_config.cfg_asiu_cam_image_buff_end_addr,4);

     //cpu_wr_single(`CAMIBLS,  0x00000800,4);
     cpu_wr_single(CAM_TOP_REGS_CAMIBLS,test_config.cfg_asiu_cam_image_line_stride,4);

     //For Multiple Frames Buffer1 Address has to configured
     if (test_config.seq_asiu_multiple_frames_en == 1)
     {
     cpu_wr_single(CAM_TOP_REGS_CAMDBCTL, 0x00000001,4); //Double buffer enable
     cpu_wr_single(CAM_TOP_REGS_CAMICTL, 0x00000030,4);
     cpu_wr_single(CAM_TOP_REGS_CAMIBSA1, test_config.cfg_asiu_cam_image_buff_start_addr1 ,4);
     cpu_wr_single(CAM_TOP_REGS_CAMIBEA1, test_config.cfg_asiu_cam_image_buff_end_addr1 ,4);
     }


     //cpu_wr_single(`CAMDBSA0, 0x00011110,4);
     cpu_wr_single(CAM_TOP_REGS_CAMDBSA0,test_config.cfg_asiu_cam_data_buff_start_addr,4);

     //cpu_wr_single(`CAMDBEA0, 0x00011110,4);
     cpu_wr_single(CAM_TOP_REGS_CAMDBEA0,test_config.cfg_asiu_cam_data_buff_end_addr,4);

     if (test_config.seq_asiu_cam_cpi_mode == 3)
     {
    	 post_log("CAMERA APIS: Enabling JPEG Mode\n");
       cpu_wr_single (CAM_TOP_REGS_CAMDCS,0x00010021,4);
     }
     else
       cpu_wr_single (CAM_TOP_REGS_CAMDCS,0x00000000,4);

     if (test_config.cpi_clk_neg_edg_en)
	{
	  cpu_wr_single (CAM_TOP_REGS_CAMCPIR, ((cpu_rd_single(CAM_TOP_REGS_CAMCPIR,4) & 0xffffffe3) | 0x0000001c) , 4 );
	}
     if (test_config.cpi_burst_space_enable)
     {
       //Burst Space Configuration
       cpu_wr_single (CAM_TOP_REGS_CAMPRI, ((cpu_rd_single(CAM_TOP_REGS_CAMPRI,4) & 0xffff0fff) | 0x00008000) , 4 );

	   //Enable FRAME Start and End Interrupts
       cpu_wr_single (CAM_TOP_REGS_CAMICTL, ((cpu_rd_single(CAM_TOP_REGS_CAMICTL,4) & 0xffffffC4) | 0x0000003B) , 4 );

       
     }

	 //cpu_wr_single (CAM_TOP_REGS_CAMCPIR, ((cpu_rd_single(CAM_TOP_REGS_CAMCPIR,4) & 0xffffffe0) | 0x0000001f) , 4 );
	  cpu_wr_single (CAM_TOP_REGS_CAMCPIR, ((cpu_rd_single(CAM_TOP_REGS_CAMCPIR,4) & 0xfffffff0) | 0x0000000f) , 4 ); //Vsync is active low and hsync is active low

	 //Enable FRAME Start and End Interrupts
       cpu_wr_single (CAM_TOP_REGS_CAMICTL, ((cpu_rd_single(CAM_TOP_REGS_CAMICTL,4) & 0xffffffC4) | 0x0000003B) , 4 );

       //Enalbe Interrupt -- Reg. config. during CPI Capture
       cpu_wr_single (CAM_TOP_REGS_CAMCPIS, ((cpu_rd_single(CAM_TOP_REGS_CAMCPIS,4) & 0x7ffffffc) | 0x80008003) , 4 );

     //Start the camera peripheral processing data from the input
     //cpu_wr_single (CAM_TOP_REGS_CAMCPIS, ((cpu_rd_single(CAM_TOP_REGS_CAMCPIS,4) & 0xfffffffc) | 0x00000003) , 4 );
     //cpu_wr_single (CAM_TOP_REGS_CAMCPIS, 0x00000003, 4);
     return 0;
}




/*--------------------------- function -----------------------------------*/
uint32_t asiu_cam_load_coeff(void)
{

	int i;
	post_log("CAMERA APIS: Loading coefficients...\n");
     //Specifies the address at which to start writing coefficients
     cpu_wr_single (CAM_TOP_REGS_CAMIDCA, 0x00000000, 4);
     for (i=0;i<2;i=i+1)
	{
        // 00
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1FC20000, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0FFE0FFF, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x00400000, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x00000000, 4);
        // 01
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1F521F88,4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0FEE0FF8,4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x00C80083,4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1FF81FFD,4);
        // 02
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1EF11F1F, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0FCF0FE1, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0157010F, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1FE91FF1, 4);
        // 03
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1E9D1EC5, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0FA30FBB, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x01EB01A0, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1FD51FE0, 4);
        // 04
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1E571E78, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0F690F88, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x02850238, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1FBB1FC8, 4);
        // 05
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1E1E1E39, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0F220F47, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x032402D4, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1F9C1FAC, 4);
        // 06
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1DF01E05, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0ED00EFB, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x03C60375, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1F7A1F8B, 4);
        // 07
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1DCE1DDE, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0E720EA2, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x046C0419, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1F541F67, 4);
        // 08
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1DB61DC0, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0E0A0E40, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x051404C0, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1F2C1F40, 4);
        // 09
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1DA71DAD, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0D990D33, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x05BD0568, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1F031F18, 4);
        // 0A
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1DA21DA3, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0D1E0D5D, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x06680613, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1ED81EED, 4);
        // 0B
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1DA41DA2, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0C9C0CDE, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x071206BD, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1EAE1EC3, 4);
        // 0C
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1DAE1DA8, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0C120C58, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x07BC0768, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1e841e98, 4);
        // 0D
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1DBF1DB6, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0B810BCA, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x08650811, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1E5B1E6F, 4);
        // 0E
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1DD51DC9, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0AEB0B37, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x090B08B9, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1E351E47, 4);
        // 0F
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1DF11DE3, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0A4F0A9D, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x09AF095E, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1E111E22, 4);
        // 10
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1E111E00, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x09AF0A00, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0A4F0A00, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1DF11E00, 4);
        // 11
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1E351E22, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x090B095E, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0AEB0A9D, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1DD51DE3, 4);
        // 12
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1E5B1E47, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x086508B9, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0B810B37, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1DBF1DC9, 4);
        // 13
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1E841E6F, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x07BC0811, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0C120BCA, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1DAE1DB6, 4);
        // 14
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1EAE1E98, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x07120768, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0C9C0C58, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1DA41DA8, 4);
        // 15
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1ED81EC3, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x066806BD, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0D1E0CDE, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1DA21DA2, 4);
        // 16
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1F031EED, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x05BD0613, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0D990D5D, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1DA71DA3, 4);
        // 17
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1F2C1F18, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x05140568, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0E0A0DD3, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1DB61DAD, 4);
        // 18
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1F541F40, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x046C04C0, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0E720E40, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1DCE1DC0, 4);
        // 19
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1F7A1F67, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x03C60419, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0ED00EA2, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1DF01DDE, 4);
        // 1A
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1F9C1F8B, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x03240375, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0F220EFB, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1E1E1E05, 4);
        // 1B
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1FBB1FAC, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x028502D4, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0F690F47, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1E571E39, 4);
        // 1C
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1FD51FC8, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x01EB0238, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0FA30F88, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1E9D1E78, 4);
        // 1D
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1FE91FE0, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x015701A0, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0FCF0FBB, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1EF11EC5, 4);
        // 1E
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1FF81FF1, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x00C8010F, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0FEE0FE1, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1F521F1F, 4);
        // 1F
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x00001FFD, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x00400083, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x0FFE0FF8, 4);
        cpu_wr_single(CAM_TOP_REGS_CAMIDCD,0x1FC21F88, 4);
        // All done
        post_log("CAMERA C APIS: Coefficients loaded\n");
     }
     return 0;
}

/****************************************************************/
void cpi_config_for_capture(void)
{

	//uint32_t dummy_read;

	// default initialisation.
	test_config.seq_asiu_cam_soft_reset_enable = 0;
	test_config.seq_asiu_cam_cpi_data_width = 0;
	test_config.seq_asiu_cam_cpi_mode = 0;
	test_config.seq_asiu_cam_unpack_option = 0;
	test_config.seq_asiu_cam_pack_option = 0;
	test_config.cfg_asiu_cam_image_buff_start_addr = 0;
	test_config.cfg_asiu_cam_image_buff_end_addr = 0;
	test_config.cfg_asiu_cam_data_buff_end_addr = 0;
	test_config.cfg_asiu_cam_data_buff_start_addr = 0;
	test_config.cfg_asiu_cam_image_line_stride = 0;
	test_config.seq_asiu_horz_window_enable = 0;
	test_config.cfg_asiu_horz_window_value = 0;
	test_config.seq_asiu_verti_window_enable = 0;
	test_config.cfg_asiu_verti_window_value = 0;
	test_config.seq_asiu_multiple_frames_en = 0;
	test_config.cfg_asiu_cam_image_buff_start_addr1 = 0;
	test_config.cfg_asiu_cam_image_buff_end_addr1 = 0;
	test_config.cam_non_secure_en = 0;
	test_config.cpi_clk_neg_edg_en = 0;
	test_config.cpi_burst_space_enable = 0;

	//test specific init.
	// cpi mode = hv mode, data width = 8, format yuv422,single buffer , only image buffer and no data buffer
	// camera selected as secure master, line stride = 0, pack /unpack none , no downsizing, no windowing
	// sensor configured for 1M pixel and image buffer allocated with max space
	test_config.seq_asiu_cam_cpi_data_width = 8;
	test_config.seq_asiu_cam_cpi_mode = 0;
	test_config.cfg_asiu_cam_image_buff_start_addr = IMAGE_START;
	test_config.cfg_asiu_cam_image_buff_end_addr = IMAGE_END;
	test_config.cfg_asiu_cam_data_buff_end_addr = DATA_END;
	test_config.cfg_asiu_cam_data_buff_start_addr = DATA_START;
	test_config.cfg_asiu_cam_image_line_stride = 0x140;
	test_config.seq_asiu_cam_pack_option = 1;
	test_config.seq_asiu_multiple_frames_en = 0;



	asiu_cam_test_init_seq();




}

static void disable_cpi_interface(void)
{
uint32_t read_data,val,i;

	val = 0;
	i = 0;


    /* shut down */
 	read_data = cpu_rd_single(CAM_TOP_REGS_CAMCTL,4);
 	read_data = read_data | (1<<4);
 	cpu_wr_single(CAM_TOP_REGS_CAMCTL,read_data, 4);


	while ((val & 0x00000100) == 0) {
		val = cpu_rd_single(CAM_TOP_REGS_CAMSTA,4);
		/* wait for OES = 1 */
		/* 10 microsecond delay */
		cam_add_delay(2000);

		if (++i  == 10) {
			post_log("\n error in unicam shutdown:STA = 0x%x\n", val);
			break;
		}

		}

	read_data = cpu_rd_single(CAM_TOP_REGS_CAMCTL,4);
 	read_data = read_data & (~(1<<4));
 	cpu_wr_single(CAM_TOP_REGS_CAMCTL,read_data, 4);

	/***** peripheral soft reset enable */

    read_data = cpu_rd_single(CAM_TOP_REGS_CAMCTL,4);
 	read_data = read_data | (1<<2);
 	cpu_wr_single(CAM_TOP_REGS_CAMCTL,read_data, 4);

	/* 10 microsecond delay */
	cam_add_delay(2000);

	/***** peripheral soft reset disable */
	read_data = cpu_rd_single(CAM_TOP_REGS_CAMCTL,4);
 	read_data = read_data & (~(1<<2));
 	cpu_wr_single(CAM_TOP_REGS_CAMCTL,read_data, 4);



	cpu_wr_single (CAM_TOP_REGS_CAMIBLS,0x00000000,4);
    cpu_wr_single (CAM_TOP_REGS_CAMIBSA0,0x00000000,4);
    cpu_wr_single (CAM_TOP_REGS_CAMIBEA0,0x00000000,4);

	/***** peripheral disable */
	read_data = cpu_rd_single(CAM_TOP_REGS_CAMCTL,4);
 	read_data = read_data & (~(1<<0));
 	cpu_wr_single(CAM_TOP_REGS_CAMCTL,read_data, 4);

    /* cpi disable */
	cpu_wr_single (CAM_TOP_REGS_CAMCPIS,(cpu_rd_single(CAM_TOP_REGS_CAMCPIS,4) & 0xfffffffc),4 );



}

void  cam_diag_generate_ref_clock ( void )
{

	uint32_t wr_data;

    audio_sw_reset();
                audio_io_mux_select();
                asiu_audio_clock_gating_disable();
                asiu_audio_pad_enable();

                wr_data = cpu_rd_single(AUD_FMM_IOP_OUT_I2S_0_MCLK_CFG_0_REG,4);
                                                wr_data = wr_data & 0xffffff00;
                                                wr_data = wr_data |01;
                                                cpu_wr_single (AUD_FMM_IOP_OUT_I2S_0_MCLK_CFG_0_REG, wr_data, 4);

                                                post_log("I2S 0 clock configure \n");

                                                wr_data = cpu_rd_single(AUD_FMM_IOP_OUT_I2S_1_MCLK_CFG_0_REG,4);
                                                wr_data = wr_data & 0xffffff00;
                                                wr_data = wr_data |00;
                                                cpu_wr_single (AUD_FMM_IOP_OUT_I2S_1_MCLK_CFG_0_REG, wr_data, 4);

                                                post_log("I2S 1 clock configure \n");

	asiu_audio_gen_pll_pwr_on(1);
	//audio_ext_test_clock_en   = 0 , user_macro = 5 , all other parameters are made 0
	asiu_audio_gen_pll_group_id_config (0,0,0,0,0,0, 0, 5, 0, 0  );

}


void lcd_configuration(void)
{  
  //configured for 320 x 240 image size
  
	//#define TIM0 0x57272FC8//0x58283031 - Modified by Sathish
	//#define TIM1 0x200D09DF//0x200D0DDF - Modified by Sathish
	//#define TIM2 0x031F0008 //0x31F0008//0x71F0020 - // 0x071F0028
	//#define TIM3 0x10002
    uint32_t lcd_ctrl;
    uint32_t reg_data = cpu_rd_single(ASIU_TOP_LCD_AXI_SB_CTRL,4);
    reg_data = reg_data & (0xfeffffff);
    cpu_wr_single(ASIU_TOP_LCD_AXI_SB_CTRL,reg_data,4);

    lcd_ctrl    = (CNTL | LCD_CNTL_BPP16_565);
    lcd_ctrl	= lcd_ctrl & ~LCD_CNTL_LCDEN;/* first control with disable */ 
  //LCD Timing 0
  uint32_t tim0 = 0x57272F4C;
  uint32_t tim1 = 0x200D08EF;
  uint32_t tim2 = 0x031F0008;
  uint32_t tim3 = 0x10002;
  cpu_wr_single(0x180A0000,tim0,4);
  cpu_wr_single(0x180A0004,tim1,4);
  cpu_wr_single(0x180A0008,tim2,4);
  cpu_wr_single(0x180A000c,tim3,4);
  cpu_wr_single(0x180A0010,0x60100000,4);
  cpu_wr_single(0x180A0014,0x60100000,4);
  lcd_ctrl	= lcd_ctrl | LCD_CNTL_LCDEN;

 
  //LCD_CTL
  cpu_wr_single(0x180A0018,lcd_ctrl,4);
}

void lcd_pwm_init(void)
{
	//LCD Related configuration Starts Here
	uint32_t pwmctl_data = 0x3FFF0001;
	uint32_t reg_val;

	 //LCD clock Gate enabled
    uint32_t register_data = cpu_rd_single(LCD_CLK_GATING_CTRL,4);
    register_data = (register_data | (1 << LCD_CLK_GATE_EN) | (1 << DSI_CLK_GATE_EN));
    cpu_wr_single(LCD_CLK_GATING_CTRL,register_data,4);


	post_log("lcd_clk_gate - ASIU_TOP_CLK_GATING_CTR -> 0x%X \n",register_data);

	cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL, 0,4);
	  
    reg_val = cpu_rd_single(CRMU_IOMUX_CTRL3,4);

    post_log("CRMU_IOMUX_CTRL3 = 0x%X\n",reg_val);

    reg_val &= ~( 0x0F << CRMU_IOMUX_CTRL3__CORE_TO_IOMUX_PWM0_SEL_R );
    cpu_wr_single(CRMU_IOMUX_CTRL3, reg_val,4);

    post_log("CRMU_IOMUX_CTRL3 = 0x%X\n",cpu_rd_single(CRMU_IOMUX_CTRL3,4));


	  pwminit1();
	  setPwmCtl1(0x3FFF0000);
	  setPrescale1(0x0);
	  setPeriod1(0x0,0x7D0);
	  setHighPeriod1(0x0,0x3E8);
	  setPwmCtl1(pwmctl_data);
}

void setPwmCtl1(uint32_t data)
{
        cpu_wr_single(ASIU_PWM_CONTROL, data,4);

        post_log("ASIU_PWM_CONTROL value written %08X \n",data);

}

/*---------------function-------------------------------------------*/
void setPeriod1(uint32_t sigNo,uint32_t data)
{
   switch(sigNo){
           case 0:
                    cpu_wr_single(ASIU_PWM0_PERIOD_COUNT, data,4);

                    post_log("ASIU_PWM0_PERIOD_COUNT value written %08X \n",data);

                    break;
          case 1:
                   cpu_wr_single(ASIU_PWM1_PERIOD_COUNT, data,4);

                   post_log("ASIU_PWM1_PERIOD_COUNT value written %08X \n",data);

                    break;
          case 2:
                   cpu_wr_single(ASIU_PWM2_PERIOD_COUNT, data,4);

                   post_log("ASIU_PWM2_PERIOD_COUNT value written %08X \n",data);

                   break;
          case 3:
                  cpu_wr_single(ASIU_PWM3_PERIOD_COUNT, data,4);

                  post_log("ASIU_PWM3_PERIOD_COUNT value written %08X \n",data);

                  break;
          case 4:
                  cpu_wr_single(ASIU_PWM4_PERIOD_COUNT, data ,4);

                  post_log("ASIU_PWM4_PERIOD_COUNT value written %08X \n",data);

                  break;
          case 5:
                  cpu_wr_single(ASIU_PWM5_PERIOD_COUNT, data,4);

                  post_log("ASIU_PWM5_PERIOD_COUNT value written %08X \n",data);

                  break;



   }
}

/*-----------------------------function---------------------------*/

void setHighPeriod1(uint32_t sigNo,uint32_t data)
{
   switch(sigNo){
            case 0:
                  cpu_wr_single(ASIU_PWM0_DUTY_CYCLE_HIGH, data,4);

                  post_log("ASIU_PWM0_DUTY_CYCLE_HIGH value written %08X \n",data);

                   break;

            case 1:
                  cpu_wr_single(ASIU_PWM1_DUTY_CYCLE_HIGH, data,4);

                  post_log("ASIU_PWM1_DUTY_CYCLE_HIGH value written %08X \n",data);

                  break;

            case 2:
                   cpu_wr_single(ASIU_PWM2_DUTY_CYCLE_HIGH, data,4);

                   post_log("ASIU_PWM2_DUTY_CYCLE_HIGH value written %08X \n",data);

                  break;

            case 3:
                  cpu_wr_single(ASIU_PWM3_DUTY_CYCLE_HIGH, data,4);

                  post_log("ASIU_PWM3_DUTY_CYCLE_HIGH value written %08X \n",data);

                  break;

            case 4:
                  cpu_wr_single(ASIU_PWM4_DUTY_CYCLE_HIGH, data,4);

                  post_log("ASIU_PWM4_DUTY_CYCLE_HIGH value written %08X \n",data);

                  break;

            case 5:
                  cpu_wr_single(ASIU_PWM5_DUTY_CYCLE_HIGH, data,4);

                  post_log("ASIU_PWM5_DUTY_CYCLE_HIGH value written %08X \n",data);

                  break;

        }
}

/*----------------function--------------------------------------*/
void setPrescale1(uint32_t data)
{
  cpu_wr_single(ASIU_PRESCALE_CONTROL, data,4);

  post_log("ASIU_PRESCALE_CONTROL value written : %08X \n",data);

}


void pwminit1(void)
{
	cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL, 0,4);

		    post_log("ASIU SOFT RESET INITIATED \n");

			//ASIU SW RESET
			cpu_wr_single(ASIU_TOP_SW_RESET_CTRL,0x000003ff,4);

			post_log("ASIU SOFT RESET DONE \n");


}

/**************************************************/

static int cam_capture_frame(void)
{
	int test = 0;
	int speed_mode = 0;
	int read_value;
	uint32_t error_value;

	uint32_t reg_value;

	read_value = 0;
	error_value = 0;


    post_log("\n  CAM.Diags - configure. ref. clock  \n");

    cpu_wr_single(0x301d0bc,0,4);
    cpu_wr_single(CRMU_IOMUX_CTRL7,0,4);
    // activate cam_ref_pll_clock ,25Mhz
    cam_diag_generate_ref_clock();

    //Remove the clock gating of the Camera Module
       reg_value = cpu_rd_single(0x180AA024,4);
       reg_value = reg_value | (1 << 2);
       cpu_wr_single(0x180AA024,reg_value,4);


       //Camera PAD Enable
    reg_value = cpu_rd_single(ASIU_TOP_PAD_CTRL_0,4);
    reg_value = reg_value | 0x100;
    cpu_wr_single(ASIU_TOP_PAD_CTRL_0,reg_value,4);



    post_log(" unicam_cpi interface init\n");
	set_memory(IMAGE_START);
	//print_memory(IMAGE_START);

    	cpi_config_for_capture();

	

	/*post_log(" smbus init in 100 KHz\n");
	cam_smbus_init(speed_mode);

    post_log(" 10 us delay\n");
	cam_add_delay(2000);

	post_log(" checking.. i2c interface to ov5640\n");
	test = cam_smbus_slave_presence(SCCB_SLAVE_ID);*/

	post_log(" smbus init in 400 KHz\n");
		cam_smbus_init(1);

	    post_log(" 10 us delay\n");
		cam_add_delay(2000);

		post_log(" checking.. i2c interface to ov5640\n");
		test = cam_smbus_slave_presence(SCCB_SLAVE_ID);

	post_log(" ov5640 init..\n");
	ov5640_common_init();
	cam_add_delay(2000);

	post_log(" ov5640 ..isp..init\n");
	ov5640_isp_init();

    //ov5640_set_bandingfilter();

	//set AE target
	//ov5640_set_aetarget(AE_TARGET);
    
	//Night mode enabled
	//ov5640_reg_write(SCCB_SLAVE_ID,0x3a00,0x04);
	
	post_log(" ov5640 ..capture..config\n");
	ov5640_capture_config();


	//cam_add_delay(2000000);


    post_log(" waiting for frame start intrupt \n");
	while ( !(cpu_rd_single(CAM_TOP_REGS_CAMISTA,4) & 0x00000001))
		{
		//wait for frame start interrupt
		}
	post_log(" frame start intr received \n");

	//cpu_wr_single (CAM_TOP_REGS_CAMISTA,(cpu_rd_single(CAM_TOP_REGS_CAMISTA,4) | 0x00000001) , 4 );

    post_log(" waiting for frame end intrupt \n");
	while ( !(cpu_rd_single(CAM_TOP_REGS_CAMISTA,4) & 0x00000002))
			{
			//wait for frame end interrupt
			}
	post_log(" frame end intr received \n");

	//cpu_wr_single (CAM_TOP_REGS_CAMISTA,(cpu_rd_single(CAM_TOP_REGS_CAMISTA,4) | 0x00000002) , 4 );

    // check for the DDR contents where the image gets stored
	error_value = check_memory(IMAGE_START);

	if (error_value != 0 )
	{
		test = -1;
		return test;
	}
	else
	{
		post_log("DDR contents have indeed been modified by contents from OV5640 Camera Sensor \n");
		reg_value = cpu_rd_single(0x180A1104,4);
		post_log("Frame Start Interrupt : %d\n", (reg_value & 0x00000001)); 
		reg_value = (reg_value & 0x00000001);
		if (reg_value != 1)
		  {
			 post_log("Frame Start Interrupt Not Received \n");
             test = -1;
			 return test;
		  }
		reg_value = cpu_rd_single(0x180A1104,4);
		post_log("Frame End Interrupt : %d\n", ((reg_value & 0x00000002) >> 1));
		reg_value = ((reg_value & 0x00000002) >> 1);
		if (reg_value != 1)
		  {
			 post_log("Frame End Interrupt Not Received \n");
             test = -1;
			 return test;
		  }
	}

 return test;

}
/*************************/
/*
** camera - frame capture test
** return test pass/fail
*/
int CAM_post_test(int flags)
{
  int test_no;
  int loop_count = 0, total_tests = 0, total_regression = 0;
  int status;

  #if 1
     status = TEST_PASS;
        if(TEST_FAIL == cam_capture_frame())
    	      status = TEST_FAIL;
        return status;
  #else

  FUNC_INFO regression[] = {

    		               };

  FUNC_INFO function[]  = {
			                 ADD_TEST(  cam_capture_frame      )

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
 return 0;
#endif
}
#endif
#endif






