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
#if CONFIG_POST & CONFIG_SYS_POST_SMAU

#include "../../include/aes_all.h"
#include "../../halapis/include/smau_apis.h"
#include "../../halapis/include/cygnus_types.h"
#include "../../halapis/include/reg_access.h"
#include "../../arch/arm/include/asm/arch-cygnus/socregs.h"
#include "../../halapis/include/dma_apis.h"
#include "../../halapis/include/tz_apis.h"
#include "../../halapis/include/dmu.h"
#include "../../halapis/include/bcm_scapi.h"
#include "../../halapis/include/spum_api.h"
#include "../../halapis/include/bcm_scapi_defines.h"
struct DMA_Controller dmac1;

#define error_log post_log
#define debug_log post_log
#define TEST_PASS 0x00
#define TEST_FAIL -1
#define TEST_SKIP  2
#define POST_REGRESSION 1

typedef struct _post_result {
     int t_pass;
     int t_fail;
     int t_skip;
} post_result;

uint64_t exec_time[100] = {
							0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
							0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
							0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
							0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
							0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
						  };

#define ADD_TEST(X)  { #X , X }
extern volatile uint32_t ns_data_abort_count;
extern int disable_log;
typedef struct{
	
	char* func_name;
	int (*func)(void);
}FUNC_INFO;
crypto_lib_handle *pHandle;
fips_rng_context rngctx;
//DMA Configuration
#define MAX_4MB_4BYTE_16DAT_SRC1_DEST1 		0x00
#define MAX_256KB_4BYTE_1DAT_SRC0_DEST1 	0x01
#define MAX_2MB_4BYTE_8DAT_SRC1_DEST1 		0x02
#define MAX_4MB_4BYTE_16DAT_SRC0_DEST0 		0x03
#define MAX_4MB_8BYTE_8DAT_SRC1_DEST0 		0x04

#define MAX_16MB_16BYTE_16DAT_SRC1_DEST1	0x05
#define MAX_8MB_8BYTE_16DAT_SRC1_DEST1		0x06
#define MAX_2MB_8BYTE_4DAT_SRC0_DEST1		0x07
#define MAX_8MB_8BYTE_16DAT_SRC0_DEST0 		0x08
#define MAX_4MB_4BYTE_16DAT_SRC0_DEST1 		0x09

#define DMA_EXECUTION_MEMORY 0x02000000
#define MEMORY_4KB  0x1000

struct DMA_chConfig config_smau_dma[]=
{
	{
	      /*srcAddr*/0x0,
          /*srcIncr*/1,
          /*config.srcId*/0,
          /*config.srcBurstSz*/DMA_4ByteBeat,
          /*config.srcWidth*/DMA_16DataTfr, //srcWidth */ source Bytes per Beat
          /*config.srcMaster*/0,

	      /*dstAddr*/0x0,
          /*config.dstIncr*/1,
          /*config.dstId*/0,
          /*config.dstBurstSz*/DMA_4ByteBeat,
          /*config.dstWidth*/DMA_16DataTfr,
          /*config.dstMaster*/0,

          /*config.transType*/0,
          /*config.transSize*/400,
          /*config.nonsec_mode*/DMA_SecureState,

          /*config.perip_type*/DMA_Memtomem,
          /*config.breq_only*/0,
          /*config.src_cache_ctrl*/0,
          /*config.dst_cache_ctrl*/0
	},
	//Source from the same address
	{
		      /*srcAddr*/0x0,
	          /*srcIncr*/0,
	          /*config.srcId*/0,
	          /*config.srcBurstSz*/DMA_4ByteBeat,
	          /*config.srcWidth*/DMA_1DataTfr, //srcWidth */ source Bytes per Beat
	          /*config.srcMaster*/0,

		      /*dstAddr*/0x0,
	          /*config.dstIncr*/1,
	          /*config.dstId*/0,
	          /*config.dstBurstSz*/DMA_4ByteBeat,
	          /*config.dstWidth*/DMA_1DataTfr,
	          /*config.dstMaster*/0,

	          /*config.transType*/0,
	          /*config.transSize*/400,
	          /*config.nonsec_mode*/DMA_SecureState,

	          /*config.perip_type*/DMA_Memtomem,
	          /*config.breq_only*/0,
	          /*config.src_cache_ctrl*/0,
	          /*config.dst_cache_ctrl*/0
	},

	{
	      /*srcAddr*/0x0,
          /*srcIncr*/1,
          /*config.srcId*/0,
          /*config.srcBurstSz*/DMA_4ByteBeat,
          /*config.srcWidth*/DMA_8DataTfr, //srcWidth */ source Bytes per Beat
          /*config.srcMaster*/0,

	      /*dstAddr*/0x0,
          /*config.dstIncr*/1,
          /*config.dstId*/0,
          /*config.dstBurstSz*/DMA_8ByteBeat,
          /*config.dstWidth*/DMA_4DataTfr,
          /*config.dstMaster*/0,

          /*config.transType*/0,
          /*config.transSize*/400,
          /*config.nonsec_mode*/0,

          /*config.perip_type*/0,
          /*config.breq_only*/0,
          /*config.src_cache_ctrl*/0,
          /*config.dst_cache_ctrl*/0
	},

	//Source and Destination is not incrementing same address
	{
		      /*srcAddr*/0x0,
	          /*srcIncr*/0,
	          /*config.srcId*/0,
	          /*config.srcBurstSz*/DMA_4ByteBeat,
	          /*config.srcWidth*/DMA_16DataTfr, //srcWidth */ source Bytes per Beat
	          /*config.srcMaster*/0,

		      /*dstAddr*/0x0,
	          /*config.dstIncr*/0,
	          /*config.dstId*/0,
	          /*config.dstBurstSz*/DMA_4ByteBeat,
	          /*config.dstWidth*/DMA_16DataTfr,
	          /*config.dstMaster*/0,

	          /*config.transType*/0,
	          /*config.transSize*/400,
	          /*config.nonsec_mode*/DMA_SecureState,

	          /*config.perip_type*/DMA_Memtomem,
	          /*config.breq_only*/0,
	          /*config.src_cache_ctrl*/0,
	          /*config.dst_cache_ctrl*/0
	},
	{
		      /*srcAddr*/0x0,
	          /*srcIncr*/1,
	          /*config.srcId*/0,
	          /*config.srcBurstSz*/DMA_8ByteBeat,
	          /*config.srcWidth*/DMA_8DataTfr, //srcWidth */ source Bytes per Beat
	          /*config.srcMaster*/0,

		      /*dstAddr*/0x0,
	          /*config.dstIncr*/0,
	          /*config.dstId*/0,
	          /*config.dstBurstSz*/DMA_8ByteBeat,
	          /*config.dstWidth*/DMA_8DataTfr,
	          /*config.dstMaster*/0,

	          /*config.transType*/0,
	          /*config.transSize*/400,
	          /*config.nonsec_mode*/DMA_SecureState,

	          /*config.perip_type*/DMA_Memtomem,
	          /*config.breq_only*/0,
	          /*config.src_cache_ctrl*/0,
	          /*config.dst_cache_ctrl*/0
	},
	{
		      /*srcAddr*/0x0,
	          /*srcIncr*/1,
	          /*config.srcId*/0,
	          /*config.srcBurstSz*/DMA_16ByteBeat,
	          /*config.srcWidth*/DMA_16DataTfr, //srcWidth */ source Bytes per Beat
	          /*config.srcMaster*/0,

		      /*dstAddr*/0x0,
	          /*config.dstIncr*/1,
	          /*config.dstId*/0,
	          /*config.dstBurstSz*/DMA_16ByteBeat,
	          /*config.dstWidth*/DMA_16DataTfr,
	          /*config.dstMaster*/0,

	          /*config.transType*/0,
	          /*config.transSize*/400,
	          /*config.nonsec_mode*/DMA_SecureState,

	          /*config.perip_type*/DMA_Memtomem,
	          /*config.breq_only*/0,
	          /*config.src_cache_ctrl*/0,
	          /*config.dst_cache_ctrl*/0
	},
	{
	      /*srcAddr*/0x0,
          /*srcIncr*/1,
          /*config.srcId*/0,
          /*config.srcBurstSz*/DMA_8ByteBeat,
          /*config.srcWidth*/DMA_16DataTfr, //srcWidth */ source Bytes per Beat
          /*config.srcMaster*/0,

	      /*dstAddr*/0x0,
          /*config.dstIncr*/1,
          /*config.dstId*/0,
          /*config.dstBurstSz*/DMA_8ByteBeat,
          /*config.dstWidth*/DMA_16DataTfr,
          /*config.dstMaster*/0,

          /*config.transType*/0,
          /*config.transSize*/400,
          /*config.nonsec_mode*/DMA_SecureState,

          /*config.perip_type*/DMA_Memtomem,
          /*config.breq_only*/0,
          /*config.src_cache_ctrl*/0,
          /*config.dst_cache_ctrl*/0
	},
	{
		      /*srcAddr*/0x0,
	          /*srcIncr*/0,
	          /*config.srcId*/0,
	          /*config.srcBurstSz*/DMA_8ByteBeat,
	          /*config.srcWidth*/DMA_4DataTfr, //srcWidth */ source Bytes per Beat
	          /*config.srcMaster*/0,

		      /*dstAddr*/0x0,
	          /*config.dstIncr*/1,
	          /*config.dstId*/0,
	          /*config.dstBurstSz*/DMA_8ByteBeat,
	          /*config.dstWidth*/DMA_4DataTfr,
	          /*config.dstMaster*/0,

	          /*config.transType*/0,
	          /*config.transSize*/400,
	          /*config.nonsec_mode*/DMA_SecureState,

	          /*config.perip_type*/DMA_Memtomem,
	          /*config.breq_only*/0,
	          /*config.src_cache_ctrl*/0,
	          /*config.dst_cache_ctrl*/0
	},
	{
		      /*srcAddr*/0x0,
	          /*srcIncr*/0,
	          /*config.srcId*/0,
	          /*config.srcBurstSz*/DMA_8ByteBeat,
	          /*config.srcWidth*/DMA_4DataTfr, //srcWidth */ source Bytes per Beat
	          /*config.srcMaster*/0,

		      /*dstAddr*/0x0,
	          /*config.dstIncr*/0,
	          /*config.dstId*/0,
	          /*config.dstBurstSz*/DMA_8ByteBeat,
	          /*config.dstWidth*/DMA_4DataTfr,
	          /*config.dstMaster*/0,

	          /*config.transType*/0,
	          /*config.transSize*/400,
	          /*config.nonsec_mode*/DMA_SecureState,

	          /*config.perip_type*/DMA_Memtomem,
	          /*config.breq_only*/0,
	          /*config.src_cache_ctrl*/0,
	          /*config.dst_cache_ctrl*/0
	},
	{
	      /*srcAddr*/0x0,
          /*srcIncr*/0,
          /*config.srcId*/0,
          /*config.srcBurstSz*/DMA_4ByteBeat,
          /*config.srcWidth*/DMA_16DataTfr, //srcWidth */ source Bytes per Beat
          /*config.srcMaster*/0,

	      /*dstAddr*/0x0,
          /*config.dstIncr*/1,
          /*config.dstId*/0,
          /*config.dstBurstSz*/DMA_4ByteBeat,
          /*config.dstWidth*/DMA_16DataTfr,
          /*config.dstMaster*/0,

          /*config.transType*/0,
          /*config.transSize*/400,
          /*config.nonsec_mode*/DMA_SecureState,

          /*config.perip_type*/DMA_Memtomem,
          /*config.breq_only*/0,
          /*config.src_cache_ctrl*/0,
          /*config.dst_cache_ctrl*/0
	}
};

smau_window_config window_cfg;
/****** AES Engine ******/
/* Tow seperate Key set for Engine1 and Engine 2 each key length is 128/192/256bit long based on 
the AES type used AES 128/192/256 */
uint8_t key0[] = "broadcom_swdiag_smau_key_engine0";
uint8_t key1[] = "broadcom_swdiag_smau_key_engine1";

/****** DDR ARD_AAD ******/
/*Random number used for AES and SHA-2*/
uint32_t ard[]   = {  0x5A5A5A5A  ,  0xDEADBEEF  ,  0xA5A5A5A5 };

/****** DDR HSALT ******/
/* Random number, used in padding hash data block for SHA-2 */
uint32_t hsalt[] = {  0xDEADBEEF  ,  0x5A5A5A5A  ,  0xDEADBEEF , 0x5A5A5A5A };

/****** DATA Referance ******/
#define REF_DATA 0xDEADBEEF



uint32_t enable_dmac_acp(uint32_t enable_override , uint32_t awcache , uint32_t awuser , uint32_t arcache , uint32_t aruser )
{
	uint32_t AWCACHE 	= awcache ;
	uint32_t AWUSER 	= awuser ;
	uint32_t ARCACHE 	= arcache ;
	uint32_t ARUSER 	= aruser ;
	uint32_t ADDR33bit 	= 1 ;
	uint32_t bypassct 	= enable_override ;
	post_log("DMAC_M0_IDM_IO_CONTROL_DIRECT = 0x%X\n",reg32_read(DMAC_M0_IDM_IO_CONTROL_DIRECT));
	post_log("Writing to DMAC_M0_IDM_IO_CONTROL_DIRECT\n");
	reg32_write(DMAC_M0_IDM_IO_CONTROL_DIRECT , (ARCACHE << 20) | (AWCACHE << 16) | (ARUSER <<9) | (AWUSER <<4) | (ADDR33bit <<3) | (bypassct<<2) | 0x1);
	post_log("DMAC_M0_IDM_IO_CONTROL_DIRECT = 0x%X\n",reg32_read(DMAC_M0_IDM_IO_CONTROL_DIRECT));
	return 0;
	//enable_dmac_acp(0x1 /*drive_idm_val*/ , 0x2 /*awcache*/, 0x1 /*awuser*/, 0x2 /*arcache*/, 0x1 /*aruser*/);
}



/******************************************************************************************************/
/************************************** SMAU Diagnostics **********************************************/
/******************************************************************************************************/
static int SMAU_ddr_disable_window(smau_window_type_e window_number, uint32_t w_start_addr, uint32_t w_end_addr)
{
    int test_status         = TEST_PASS;
    smau_window_config window_cfg;

   /********* Cipher Window Configuration ***********************/
	window_cfg.window_num                   = window_number;
	window_cfg.data_start_addr              = w_start_addr;
	window_cfg.data_end_addr                = w_end_addr;
	window_cfg.window_en                    = 0x0;
	
    /********* AES configuration *********************************/
	window_cfg.aes_en                       = 0x0;
	window_cfg.aes_key_len                  = 0x00;
	window_cfg.aes_key0                     = (uint32_t*)key0;
	window_cfg.aes_key1                     = (uint32_t*)key1;
	window_cfg.ecb_mode_data_scramble       = 0x0;
	window_cfg.ctr_mode                     = 0x0;
	window_cfg.enable_random_aes_ctr        = 0x0;
	window_cfg.disable_iv_scramble          = 0x0;
	window_cfg.aad                          = 0x0;
	window_cfg.ard                          = (uint32_t *)&ard;
	window_cfg.cid				= 0x00;
	window_cfg.init_counter			= 0xDEAD;
    /********** Hash Algorithm Configuration ********************/
	window_cfg.hash_start_addr              = 0x00;
	window_cfg.hash_end_addr                = 0x00;
	window_cfg.non_secure                   = 0x0;
	window_cfg.sha_en                       = 0x0;
	window_cfg.disable_sha_out_scramble     = 0x0;
	window_cfg.hsalt                        = &hsalt[0];
	window_cfg.data2hash_offset             = 0x0;

    /***********************************************************/
    /************ Configuring DDR Cipher Window ****************/
    /***********************************************************/

    post_log("\nDisabling Cipher Window%d ",window_number);  
    /* Window Configuration based on the window_cfg data */
    smau_set_window_config(window_cfg); 
    return test_status;

}

/*==================================================================================================
*Function    : static int SMAU_ddr_plain_text
*Parameter   : data , data_start_addr , data_end_addr
*Description : Plain Text data read write to DDR
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
static int SMAU_ddr_plain_text(uint32_t win_no, uint32_t data, uint32_t start_addr, uint32_t end_addr)
{
    uint32_t wdata, rdata;
    uint32_t data_start_addr,data_end_addr;
    smau_window_config window_cfg;
    uint32_t addr = 0x00;
 
    /********* Cipher Window Configuration ***********************/
	window_cfg.window_num                   = win_no;
	window_cfg.data_start_addr              = start_addr;
	window_cfg.data_end_addr                = end_addr;
	window_cfg.window_en                    = 0x1;
	
    /********* AES configuration *********************************/
	window_cfg.aes_en                       = 0x0;
	window_cfg.aes_key_len                  = 0x0;
	window_cfg.aes_key0                     = (uint32_t *)&key0;
	window_cfg.aes_key1                     = (uint32_t *)&key1;
	window_cfg.ecb_mode_data_scramble       = 0x0;
	window_cfg.ctr_mode                     = 0x0;
	window_cfg.enable_random_aes_ctr        = 0x0;
	window_cfg.disable_iv_scramble          = 0x0;
	window_cfg.aad                          = 0x0;
	window_cfg.ard                          = &ard[0];
	
    /********** Hash Algorithm Configuration ********************/
	window_cfg.hash_start_addr              = 0x00;
	window_cfg.hash_end_addr                = 0x00;
	window_cfg.non_secure                   = 0x0;
	window_cfg.sha_en                       = 0x0;
	window_cfg.disable_sha_out_scramble     = 0x0;
	window_cfg.hsalt                        = &hsalt[0];
	window_cfg.data2hash_offset             = 0x0;
	


    /******************************************************************************************************/
    /************************ Test Case :Plain Text data read write to DDR ********************************/
    /******************************************************************************************************/
    data_start_addr		= start_addr;
    data_end_addr		= end_addr;
    wdata				= data;
    
    /* Window Configuration based on the window_cfg data */
    smau_set_window_config(window_cfg);     
    
    post_log("\nPlain Text data read write to DDR\n");
    post_log("Window Range 0x%X to 0x%X\n",window_cfg.data_start_addr,window_cfg.data_end_addr); 
    post_log("Writing known data : 0x%X to DDR - Address : 0x%X to 0x%X in DDR\n",data,data_start_addr,data_end_addr);
    

    for(addr = data_start_addr; addr < data_end_addr; addr+=4) {
        wr(addr, wdata++);
    }
    
    wdata = data;
    post_log("Reading DDR...\n");
    for(addr = data_start_addr; addr < data_end_addr; addr+=4) {
        rdata = rd(addr);
        if(rdata != wdata) {
            error_log("DDR data not expected - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n", addr, rdata, wdata);
            SMAU_ddr_disable_window(win_no, start_addr, end_addr);
            return TEST_FAIL;
        } else {
            //debug_log("DDR data - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n", addr, rdata, wdata);
        }
        wdata++;
    }
   SMAU_ddr_disable_window(win_no, start_addr, end_addr);
   smau_cache_invalidate();    
   return TEST_PASS;
}
bool check_aes(char *key, uint32_t key_l,uint32_t *in_data, uint32_t *out_data)
{
aes_context t_ctx;
int count = 0; 
unsigned char r_key[40];
unsigned char* aes_in_data = (unsigned char*)in_data;
unsigned char aes_out_data[40];
unsigned char aes_r_in[40];
unsigned char *aes_smau = (unsigned char*)out_data;
bool status = 1;


for(count = 0 ; count < (key_l/8) ; count++)
{
r_key[count] = key[((key_l/8) -1)-count];

}  
for(count = 0 ; count < 16 ; count++)
{
aes_r_in[count] = aes_in_data[15-count];

}


 
aes_setkey_enc( &t_ctx, (const unsigned char *)&r_key,key_l  );
aes_crypt_ecb( &t_ctx,AES_ENCRYPT,(unsigned char*)&aes_r_in,(unsigned char*)&aes_out_data );

for(count = 0 ; count < 16 ; count++)
{
//post_log("DATA[%d] = 0x%X\n",count , aes_in_data[count]) ;   
//post_log("AES[%d] = 0x%X - SMAU[%d] = 0x%X\n",(15 - count), aes_out_data[15 - count],count , aes_smau[count]);
  if(aes_smau[count] != aes_out_data[15-count]){
   post_log("DATA[%d] = 0x%X\n",count , aes_in_data[count]) ;   
   post_log("AES[%d] = 0x%X - SMAU[%d] = 0x%X\n",(15 - count), aes_out_data[15 - count],count , aes_smau[count]);
   status = 0;
  }
	//return 0;
}


return status;

}


bool r_check_aes(char *key, uint32_t key_l,uint32_t *in_data, uint32_t *out_data)
{
aes_context t_ctx;
int count = 0; 
unsigned char r_key[40];
unsigned char* aes_in_data = (unsigned char*)in_data;
unsigned char aes_out_data[40];
unsigned char *aes_smau = (unsigned char*)out_data;


post_log("reverse key\n");
for(count = 0 ; (count * 8 * 4) < key_l ; count++)
{
r_key[count] = key[((key_l/8) -1)-count];

} 


aes_setkey_enc( &t_ctx, (const unsigned char *)&r_key,key_l );
aes_crypt_ecb( &t_ctx,AES_ENCRYPT,(unsigned char *)aes_in_data,(unsigned char*)&aes_out_data );

for(count = 0 ; count < 16 ; count++)
{
post_log("DATA[%d] = 0x%X\n",count, aes_in_data[count]) ;   
post_log("AES[%d] = 0x%X - SMAU[%d] = 0x%X\n",count , aes_out_data[count],count , aes_smau[count]);
  //if(aes_smau[count] != aes_data[15-count])
	//return 0;
}


return 1;

}

/*==================================================================================================
*Function    : static int SMAU_ddr_aes_ecb(void)
*Parameter   : window_number, data , data_start_addr , data_end_addr , key_length , t_key0 , t_key1, 
	       bool sha_en, uint32_t sha_start_addr, sha_end_addr, *t_ard, t_cid
*Description : Test Case 1 : DDR Cipher Window , AES - ECB <key_length> bit Key data access Test
	       Test Case 2 : DDR Cipher Window disable and access Encripted Data 
	       Test Case 3 : DDR Cipher Window , AES - ECB <key_length> bit Key data access on AES re-enable
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
static int SMAU_ddr_aes_ecb(smau_window_type_e window_number, uint32_t data, uint32_t w_start_addr, uint32_t w_end_addr, uint32_t key_length, uint32_t *t_key0, uint32_t *t_key1, bool sha_en, uint32_t sha_start_addr, uint32_t sha_end_addr, uint32_t *t_ard,uint32_t t_cid)
{
    uint32_t wdata, rdata;
    uint32_t data_start_addr,data_end_addr;
    smau_window_config window_cfg;
    uint32_t addr = 0x00 , in_data[10], out_data[10];
    uint32_t count = 0;
    uint32_t aes_check_counter = 0;
    bool select_key = 0; 

    /********* Cipher Window Configuration ***********************/
	window_cfg.window_num                   = window_number;
	window_cfg.data_start_addr              = w_start_addr;
	window_cfg.data_end_addr                = w_end_addr;
	window_cfg.window_en                    = 0x1;
	
    /********* AES configuration *********************************/
	window_cfg.aes_en                       = 0x1;
	window_cfg.aes_key_len                  = key_length;
	window_cfg.aes_key0                     = t_key0;
	window_cfg.aes_key1                     = t_key1;
	window_cfg.ecb_mode_data_scramble       = 0x0;
	window_cfg.ctr_mode                     = 0x0;
	window_cfg.enable_random_aes_ctr        = 0x0;
	window_cfg.disable_iv_scramble          = 0x0;
	window_cfg.aad                          = 0x0;
	window_cfg.ard                          = t_ard;
	window_cfg.cid				            = t_cid;
	window_cfg.init_counter			        = 0xDEAD;
    /********** Hash Algorithm Configuration ********************/
	window_cfg.hash_start_addr              = sha_start_addr;
	window_cfg.hash_end_addr                = sha_end_addr;
	window_cfg.non_secure                   = 0x0;
	window_cfg.sha_en                       = sha_en;
	window_cfg.disable_sha_out_scramble     = 0x0;
	window_cfg.hsalt                        = &hsalt[0];
	window_cfg.data2hash_offset             = 0x0;

    /******************************************************************************************************/
    /************ Test Case 1 : DDR Cipher Window , AES - ECB 128 bit Key data access Test ****************/
    /******************************************************************************************************/
    data_start_addr			= (w_start_addr - 0x10);
    data_end_addr		    = (w_end_addr + 0x10);
    wdata				    = data;
    
    /* Window Configuration based on the window_cfg data */
    smau_set_window_config(window_cfg); 

    post_log("\nCipher Window Range 0x%X to 0x%X\n",window_cfg.data_start_addr,window_cfg.data_end_addr); 
    post_log("Writing known data : 0x%X to DDR - Address : 0x%X to 0x%X in DDR\n",data,data_start_addr,data_end_addr);
    post_log("\nCase 1 : DDR Cipher Window%d , AES - ECB  %dbit Key data access Test\n", window_number,( 128 + (key_length * 64) ));

    for(addr = data_start_addr; addr < data_end_addr; addr+=4) {
	wr(addr, wdata++);
    }
    count = 0;
    wdata = data;
    post_log("Reading DDR...\n");
   
  for(addr = data_start_addr; addr < data_end_addr; addr+=4) {
        rdata = rd(addr);
        if(rdata != wdata) {
            error_log("DDR data not expected - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n", addr, rdata, wdata);
            SMAU_ddr_disable_window(window_number, w_start_addr, w_end_addr);
            return TEST_FAIL;
        } else {
            //debug_log("DDR data - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n", addr, rdata, wdata);
        }
        wdata++;
    }
    smau_cache_invalidate();


    /******************************************************************************************************/
    /********** Test Case 2 : DDR Cipher Window disable and access Encripted Data Now *********************/
    /******************************************************************************************************/	
    post_log("\nCase 2 : DDR Cipher Window%d disable and Read Encrypted Data Now\n",window_number);
    window_cfg.aes_en = 0x0;
    smau_set_window_config(window_cfg);
    wdata  =  data;
    count = 0;
    post_log("Reading DDR... \n");
    for(addr = data_start_addr; addr < data_end_addr; addr+=4) {
        rdata = rd(addr);
        if(addr >= window_cfg.data_start_addr && addr <=(window_cfg.data_end_addr| 0xFFF)) {
	    if(rdata == wdata) {
                error_log("error1 : DDR data not expected - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n", addr, rdata, wdata);
                SMAU_ddr_disable_window(window_number, w_start_addr, w_end_addr);
                return TEST_FAIL;
            }
            else {
                if(aes_check_counter < 32){
	            aes_check_counter++;
		    out_data[count] = rdata;
		    in_data[count++] = wdata; 
		    if(count == 4){
		       count = 0;
		       if(select_key == 0){
		           select_key = 1;
		    	   if(!check_aes((char *)t_key0, ( 128 + (key_length * 64) ) ,&in_data[0], &out_data[0])){
                               SMAU_ddr_disable_window(window_number, w_start_addr, w_end_addr);
                      	       return TEST_FAIL;
		    	   }
		       }			
		       else if(select_key == 1){
		           select_key = 0;
	            	   if(!check_aes((char *)t_key1, ( 128 + (key_length * 64) ) ,&in_data[0], &out_data[0])){
                               SMAU_ddr_disable_window(window_number, w_start_addr, w_end_addr);
		      	       return TEST_FAIL;
		           }
 		       }
                   }
               }
	   }
         } 	 
         else {
               if(rdata != wdata) {
                 error_log("\nerror2 : DDR data not expected - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n", addr, rdata, wdata);
                 SMAU_ddr_disable_window(window_number, w_start_addr, w_end_addr);
                 return TEST_FAIL;
               } 
               else {
                    //debug_log("DDR data - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n", addr, rdata, wdata);
               }
         }
      wdata++;
    }  

    /******************************************************************************************************/
    /************ Test Case 3 : DDR Cipher Window , AES - ECB 128 bit Key data access on AES re-enable*****/
    /******************************************************************************************************/
    post_log("\nCase 3 : DDR Cipher Window%d re-enable and access Data Now\n",window_number);
    window_cfg.aes_en = 0x1;
    smau_set_window_config(window_cfg);
    smau_cache_invalidate();
    wdata = data;
    
    post_log("Reading DDR...\n");
    for(addr = data_start_addr; addr < data_end_addr; addr+=4) {
        rdata = rd(addr);
        if(rdata != wdata) {
            error_log("DDR data not expected - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n", addr, rdata, wdata);
            SMAU_ddr_disable_window(window_number, w_start_addr, w_end_addr);
            return TEST_FAIL;
            break;
        } else {
            //debug_log("DDR data - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n", addr, rdata, wdata);
        }
        wdata++;
    }

   SMAU_ddr_disable_window(window_number, w_start_addr, w_end_addr);    smau_cache_invalidate();
   smau_cache_invalidate();
   return TEST_PASS;
}


/*==================================================================================================
*Function    : static int SMAU_ddr_aes_ecb_transfer
*Parameter   : window_number, data , data_start_addr , data_end_addr , key_length , t_key0 , t_key1, 
	       bool sha_en, uint32_t sha_start_addr, sha_end_addr, *t_ard, t_cid
*Description : Test Case 1 : DDR Cipher Window , AES - ECB <key_length> bit Key data access Test
	       Test Case 2 : DDR Cipher Window disable and access Encripted Data 
	       Test Case 3 : DDR Cipher Window , AES - ECB <key_length> bit Key data access on AES re-enable
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
static int SMAU_ddr_aes_ecb_transfer(smau_window_type_e window_number, uint32_t data, uint32_t w_start_addr, uint32_t w_end_addr, uint32_t key_length, bool sha_en, uint32_t sha_start_addr, uint32_t sha_end_addr, uint32_t *t_ard,uint32_t t_cid, uint32_t prefetch_l)
{
    uint32_t wdata, rdata;
    uint32_t data_start_addr,data_end_addr;
    smau_window_config window_cfg;
    uint32_t addr = 0x00;
    uint32_t *t_key0 = (uint32_t *)&key0; /* Key Engine 0 */
    uint32_t *t_key1 = (uint32_t *)&key1; /* Key Engine 1 */


    /********* Cipher Window Configuration ***********************/
	window_cfg.window_num                   = window_number;
	window_cfg.data_start_addr              = w_start_addr;
	window_cfg.data_end_addr                = w_end_addr;
	window_cfg.window_en                    = 0x1;
	
    /********* AES configuration *********************************/
	window_cfg.aes_en                       = 0x1;
	window_cfg.aes_key_len                  = key_length;
	window_cfg.aes_key0                     = t_key0;
	window_cfg.aes_key1                     = t_key1;
	window_cfg.ecb_mode_data_scramble       = 0x0;
	window_cfg.ctr_mode                     = 0x0;
	window_cfg.enable_random_aes_ctr        = 0x0;
	window_cfg.disable_iv_scramble          = 0x0;
	window_cfg.aad                          = 0x0;
	window_cfg.ard                          = t_ard;
	window_cfg.cid				= t_cid;
	window_cfg.init_counter			= 0xDEAD;
    /********** Hash Algorithm Configuration ********************/
	window_cfg.hash_start_addr              = sha_start_addr;
	window_cfg.hash_end_addr                = sha_end_addr;
	window_cfg.non_secure                   = 0x0;
	window_cfg.sha_en                       = sha_en;
	window_cfg.disable_sha_out_scramble     = 0x0;
	window_cfg.hsalt                        = &hsalt[0];
	window_cfg.data2hash_offset             = 0x0;

    /******************************************************************************************************/
    /************ Test Case 1 : DDR Cipher Window , AES - ECB 128 bit Key data access Test ****************/
    /******************************************************************************************************/
    data_start_addr  = (w_start_addr);
    data_end_addr    = (w_start_addr + 128);
    wdata			 = data;
    
    /* Window Configuration based on the window_cfg data */
    smau_set_window_config(window_cfg); 
    post_log("\nCipher Window Range 0x%X to 0x%X\n",window_cfg.data_start_addr,window_cfg.data_end_addr); 
    post_log("Writing known data : 0x%X to DDR - Address : 0x%X to 0x%X in DDR\n",data,data_start_addr,data_end_addr);
    post_log("\nCase 1 : DDR Cipher Window%d , AES - ECB  %dbit Key data access Test\n", window_number,( 128 + (key_length * 64) ));

    for(addr = data_start_addr; addr < data_end_addr; addr+=4) {
	wr(addr, wdata);
    }

    smau_prefetch_length(prefetch_l);
    if(wdata != rd((data_start_addr))){
            	error_log("DDR data not expected - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n", addr, rd((data_start_addr)), wdata);
                SMAU_ddr_disable_window(window_number, w_start_addr, w_end_addr);
            	return TEST_FAIL;
    }

  
    /******************************************************************************************************/
    /********** Test Case 2 : DDR Cipher Window disable and access Encripted Data Now *********************/
    /******************************************************************************************************/	
    post_log("\nCase 2 : DDR Cipher Window%d disable and Read Encrypted Data Now\n",window_number);
    window_cfg.aes_en = 0x0;
    smau_set_window_config(window_cfg);
    wdata  =  data;
    post_log("Reading DDR... 0x%X\n",data_start_addr);
    for(addr = data_start_addr; addr < (data_start_addr + ((prefetch_l+1) * 32)); addr+=4) {
          rdata = rd(addr);
	  if(rdata != wdata) {
            	error_log("DDR data not expected - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n Data shoud return from CACHE previous read", addr, rdata, wdata);
                SMAU_ddr_disable_window(window_number, w_start_addr, w_end_addr);
            	return TEST_FAIL;
          } 

          else {
            //debug_log("DDR data - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n", addr, rdata, wdata);
          }
        
    } /*Expected CACHE HIT*/

    rdata = rd(addr);
    if(rdata == wdata) {
       	error_log("DDR data not expected - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n Data shoud return from CACHE previous read", addr, rdata, wdata);
        SMAU_ddr_disable_window(window_number, w_start_addr, w_end_addr);
      	return TEST_FAIL;
    } 
    else {
            //debug_log("DDR data - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n", addr, rdata, wdata);
    }/* Expected CACHE MISS */


    /******************************************************************************************************/
    /************ Test Case 3 : DDR Cipher Window , AES - ECB 128 bit Key data access on AES re-enable*****/
    /******************************************************************************************************/
    post_log("\nCase 3 : DDR Cipher Window%d re-enable and access Data Now\n",window_number);
    window_cfg.aes_en = 0x1;
    smau_set_window_config(window_cfg);
    smau_cache_invalidate();
    wdata = data;
    
    post_log("Reading DDR...\n");
    for(addr = data_start_addr; addr < data_end_addr; addr+=4) {
        rdata = rd(addr);
        if(rdata != wdata) {
            error_log("DDR data not expected - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n", addr, rdata, wdata);
            SMAU_ddr_disable_window(window_number, w_start_addr, w_end_addr);
            return TEST_FAIL;
        } else {
            //debug_log("DDR data - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n", addr, rdata, wdata);
        }
    }
   SMAU_ddr_disable_window(window_number, w_start_addr, w_end_addr);
   smau_cache_invalidate();
   return TEST_PASS;
}


/*==================================================================================================
*Function    : static int SMAU_ddr_aes_ctr
*Parameter   : window_number, data , data_start_addr , data_end_addr , key_length , key0 , key1, ard, cid
*Description : Test Case 1 : DDR Cipher Window , AES - ECB <key_length> bit Key data access Test
	       Test Case 2 : DDR Cipher Window disable and access Encripted Data 
	       Test Case 3 : DDR Cipher Window , AES - ECB <key_length> bit Key data access on AES re-enable
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
static int SMAU_ddr_aes_ctr(smau_window_type_e window_number, uint32_t data, uint32_t w_start_addr, uint32_t w_end_addr, uint32_t sha_start_addr, uint32_t sha_end_addr, uint32_t key_length, uint32_t *t_key0, uint32_t *t_key1, uint32_t *t_ard,uint32_t t_cid,bool random_aes)
{
    uint32_t wdata, rdata;
    uint32_t data_start_addr,data_end_addr;
    smau_window_config window_cfg;
    uint32_t addr = 0x00;
    
    /********* Cipher Window Configuration ***********************/
	window_cfg.window_num                   = window_number;
	window_cfg.data_start_addr              = w_start_addr;
	window_cfg.data_end_addr                = w_end_addr;
	window_cfg.window_en                    = 0x1;
	
    /********* AES configuration *********************************/
	window_cfg.aes_en                       = 0x1;
	window_cfg.aes_key_len                  = key_length;
	window_cfg.aes_key0                     = t_key0;
	window_cfg.aes_key1                     = t_key1;
	window_cfg.ecb_mode_data_scramble       = 0x0;
	window_cfg.ctr_mode                     = 0x1;
	window_cfg.enable_random_aes_ctr        = random_aes;
	window_cfg.disable_iv_scramble          = 0x0;
	window_cfg.aad                          = 0x0;
	window_cfg.ard                          = t_ard;
	window_cfg.cid							= t_cid;
	window_cfg.init_counter					= 0xDEAD;
    /********** Hash Algorithm Configuration ********************/
	window_cfg.hash_start_addr              = sha_start_addr;
	window_cfg.hash_end_addr                = sha_end_addr;
	window_cfg.non_secure                   = 0x0;
	window_cfg.sha_en                       = 0x1;
	window_cfg.disable_sha_out_scramble     = 0x0;
	window_cfg.hsalt                        = &hsalt[0];
	window_cfg.data2hash_offset             = 0x0;

    /******************************************************************************************************/
    /************ Test Case 1 : DDR Cipher Window , AES - ECB 128 bit Key data access Test ****************/
    /******************************************************************************************************/
    data_start_addr			    = (w_start_addr - 0x10);
    data_end_addr		        = (w_end_addr + 0x10);
    wdata				    	= data;
    
    /* Window Configuration based on the window_cfg data */
    smau_set_window_config(window_cfg); 
    post_log("\nCipher Window Range 0x%X to 0x%X\n",window_cfg.data_start_addr,window_cfg.data_end_addr); 
    post_log("Writing known data : 0x%X to DDR - Address : 0x%X to 0x%X in DDR\n",data,data_start_addr,data_end_addr);
    post_log("\nCase 1 : DDR Cipher Window%d , AES - CTR  %dbit Key data access Test\n", window_number,( 128 + (key_length * 64) ));

    for(addr = data_start_addr; addr < data_end_addr; addr+=4) {
        wr(addr, wdata++);
    }
    
    wdata = data;

    post_log("Reading DDR...\n");
    for(addr = data_start_addr; addr < data_end_addr; addr+=4) {
        rdata = rd(addr);
        if(rdata != wdata) {
            error_log("DDR data not expected - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n", addr, rdata, wdata);
            SMAU_ddr_disable_window(window_number, w_start_addr, w_end_addr);
            return TEST_FAIL;
        } else {
            //debug_log("DDR data - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n", addr, rdata, wdata);
        }
        wdata++;
    }
   
    /******************************************************************************************************/
    /********** Test Case 2 : DDR Cipher Window disable and access Encripted Data Now *********************/
    /******************************************************************************************************/	
    post_log("\nCase 2 : DDR Cipher Window%d disable and Read Encrypted Data Now\n",window_number);
    window_cfg.aes_en = 0x0;
    smau_set_window_config(window_cfg);
    smau_cache_invalidate();
    wdata  =  data;

    post_log("Reading DDR...\n");
    for(addr = data_start_addr; addr < data_end_addr; addr+=4) {
        rdata = rd(addr);
        if(addr >= window_cfg.data_start_addr && addr <= (window_cfg.data_end_addr | 0xFFF)) {
          if(rdata == wdata) {
            error_log("DDR data not expected - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n", addr, rdata, wdata);
            SMAU_ddr_disable_window(window_number, w_start_addr, w_end_addr);
            return TEST_FAIL;
          } 
          else {
            //debug_log("DDR data - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n", addr, rdata, wdata);
          }
        } 
        else {
          if(rdata != wdata) {
            error_log("DDR data not expected - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n", addr, rdata, wdata);
            SMAU_ddr_disable_window(window_number, w_start_addr, w_end_addr);
            return TEST_FAIL;
          } 
          else {
            //debug_log("DDR data - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n", addr, rdata, wdata);
          }
        }
      wdata++;
    }  
    
    /******************************************************************************************************/
    /************ Test Case 3 : DDR Cipher Window , AES - ECB 128 bit Key data access on AES re-enable*****/
    /******************************************************************************************************/
    post_log("\nCase 3 : DDR Cipher Window%d re-enable and access Data Now\n",window_number);
    window_cfg.aes_en = 0x1;
    smau_set_window_config(window_cfg);
    smau_cache_invalidate();
    wdata			  = data;
    
    post_log("Reading DDR...\n");
    for(addr = data_start_addr; addr < data_end_addr; addr+=4) {
        rdata = rd(addr);
        if(rdata != wdata) {
            error_log("DDR data not expected - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n", addr, rdata, wdata);
            SMAU_ddr_disable_window(window_number, w_start_addr, w_end_addr);
            return TEST_FAIL;
        } else {
            //debug_log("DDR data - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n", addr, rdata, wdata);
        }
        wdata++;
    }

    SMAU_ddr_disable_window(window_number, w_start_addr, w_end_addr);
    smau_cache_invalidate();
    return TEST_PASS;
}

/*==================================================================================================
*Function    : static int SMAU_ddr_rw_test_aes_ctr_128(void)
*Parameter   : none
*Description : Test Case 1 : DDR Cipher Window , AES - ECB 128 bit Key data access Test
			   Test Case 2 : DDR Cipher Window disable and access Encripted Data 
			   Test Case 3 : DDR Cipher Window , AES - ECB 128 bit Key data access on AES re-enable
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
static int SMAU_ddr_rw_aes_ctr_128_rand(void)
{
    int test_status = TEST_PASS;
    uint32_t *t_key0 = (uint32_t *)&key0; /* Key Engine 0 */
    uint32_t *t_key1 = (uint32_t *)&key1; /* Key Engine 1 */
       
    /******************************************************************************************************/
    /************ Test Case 1 : DDR Cipher Window0, AES - ECB 128 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ctr(SMAU_DDR_CW0, 0xDEADBEEF, 0x80041000, 0x80042000, 0x80060000, 0x80061000, AES_128, t_key0, t_key1, &ard[0], 0x5A5A5A5A, 0x01);

    /******************************************************************************************************/
    /************ Test Case 2 : DDR Cipher Window1, AES - ECB 128 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ctr(SMAU_DDR_CW1, 0xDEADBEEF, 0x80043000, 0x80044000, 0x80062000, 0x80063000, AES_128, t_key0, t_key1, &ard[0], 0xABCDABCD, 0x01);

    /******************************************************************************************************/
    /************ Test Case 3 : DDR Cipher Window2, AES - ECB 128 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ctr(SMAU_DDR_CW2, 0xDEADBEEF, 0x80045000, 0x80046000, 0x80064000, 0x80065000, AES_128, t_key0, t_key1, &ard[0], 0xDEADBEEF, 0x01);

    /******************************************************************************************************/
    /************ Test Case 4 : DDR Cipher Window3, AES - ECB 128 bit Key ddata_end_addrata access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ctr(SMAU_DDR_CW3, 0xDEADBEEF, 0x80047000, 0x80049000, 0x80066000, 0x80067000, AES_128, t_key0, t_key1, &ard[0], 0xA5A5A5A5, 0x01);
 
    post_log("\n-- Test %s --\n",test_status == TEST_PASS ? "PASSED" : "FAILED");
    return test_status;
}/* End SMAU_ddr_rw_test */

/*==================================================================================================
*Function    : static int SMAU_ddr_rw_test_aes_ctr_192(void)
*Parameter   : none
*Description : Test Case 1 : DDR Cipher Window , AES - CTR 192 bit Key data access Test
	       Test Case 2 : DDR Cipher Window disable and access Encripted Data 
	       Test Case 3 : DDR Cipher Window , AES - CTR 192 bit Key data access on AES re-enable
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
static int SMAU_ddr_rw_aes_ctr_192_rand(void)
{
    int test_status = TEST_PASS;
    uint32_t *t_key0 = (uint32_t *)&key0; /* Key Engine 0 */
    uint32_t *t_key1 = (uint32_t *)&key1; /* Key Engine 1 */
       

    /******************************************************************************************************/
    /************ Test Case 1 : DDR Cipher Window0, AES - CTR 192 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ctr(SMAU_DDR_CW0, 0xDEADBEEF, 0x80041000, 0x80042000, 0x80060000, 0x80061000, AES_192, t_key0, t_key1, &ard[0], 0x5A5A5A5A, 0x01);

    /******************************************************************************************************/
    /************ Test Case 2 : DDR Cipher Window1, AES - CTR 192 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ctr(SMAU_DDR_CW1, 0xDEADBEEF, 0x80043000, 0x80044000, 0x80062000, 0x80063000, AES_192, t_key0, t_key1, &ard[0], 0xABCDABCD, 0x01);

    /******************************************************************************************************/
    /************ Test Case 3 : DDR Cipher Window2, AES - CTR 192 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ctr(SMAU_DDR_CW2, 0xDEADBEEF, 0x80045000, 0x80046000, 0x80064000, 0x80065000, AES_192, t_key0, t_key1, &ard[0], 0xDEADBEEF, 0x01);

    /******************************************************************************************************/
    /************ Test Case 4 : DDR Cipher Window3, AES - CTR 192 bit Key ddata_end_addrata access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ctr(SMAU_DDR_CW3, 0xDEADBEEF, 0x80047000, 0x80049000, 0x80066000, 0x80067000, AES_192, t_key0, t_key1, &ard[0], 0xA5A5A5A5, 0x01);
 
    post_log("\n-- Test %s --\n",test_status == TEST_PASS ? "PASSED" : "FAILED");
    return test_status;
}/* End SMAU_ddr_rw_test_aes_ctr_192*/


/*==================================================================================================
*Function    : static int SMAU_ddr_rw_test_aes_ctr_256(void)
*Parameter   : none
*Description : Test Case 1 : DDR Cipher Window , AES - CTR 256 bit Key data access Test
	       Test Case 2 : DDR Cipher Window disable and access Encripted Data 
	       Test Case 3 : DDR Cipher Window , AES - CTR 256 bit Key data access on AES re-enable
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
static int SMAU_ddr_rw_aes_ctr_256_rand(void)
{
    int test_status = TEST_PASS;
    uint32_t *t_key0 = (uint32_t *)&key0; /* Key Engine 0 */
    uint32_t *t_key1 = (uint32_t *)&key1; /* Key Engine 1 */
       
    /******************************************************************************************************/
    /************ Test Case 1 : DDR Cipher Window0, AES - CTR 256 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ctr(SMAU_DDR_CW0, 0xDEADBEEF, 0x80041000, 0x80042000, 0x80060000, 0x80061000, AES_256, t_key0, t_key1, &ard[0], 0x5A5A5A5A, 0x01);

    /******************************************************************************************************/
    /************ Test Case 2 : DDR Cipher Window1, AES - CTR 256 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ctr(SMAU_DDR_CW1, 0xDEADBEEF, 0x80043000, 0x80044000, 0x80062000, 0x80063000, AES_256, t_key0, t_key1, &ard[0], 0xABCDABCD, 0x01);

    /******************************************************************************************************/
    /************ Test Case 3 : DDR Cipher Window2, AES - CTR 256 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ctr(SMAU_DDR_CW2, 0xDEADBEEF, 0x80045000, 0x80046000, 0x80064000, 0x80065000, AES_256, t_key0, t_key1, &ard[0], 0xDEADBEEF, 0x01);

    /******************************************************************************************************/
    /************ Test Case 4 : DDR Cipher Window3, AES - CTR 256 bit Key ddata_end_addrata access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ctr(SMAU_DDR_CW3, 0xDEADBEEF, 0x80047000, 0x80049000, 0x80066000, 0x80067000, AES_256, t_key0, t_key1, &ard[0], 0xA5A5A5A5, 0x01);

 
    post_log("\n-- Test %s --\n",test_status == TEST_PASS ? "PASSED" : "FAILED");
    return test_status;
}/* End SMAU_ddr_rw_test_aes_ctr_256 */
/*==================================================================================================
*Function    : static int SMAU_ddr_rw_test_aes_ctr_128(void)
*Parameter   : none
*Description : Test Case 1 : DDR Cipher Window , AES - ECB 128 bit Key data access Test
			   Test Case 2 : DDR Cipher Window disable and access Encripted Data 
			   Test Case 3 : DDR Cipher Window , AES - ECB 128 bit Key data access on AES re-enable
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
static int SMAU_ddr_rw_aes_ctr_128(void)
{
    int test_status = TEST_PASS;
    uint32_t *t_key0 = (uint32_t *)&key0; /* Key Engine 0 */
    uint32_t *t_key1 = (uint32_t *)&key1; /* Key Engine 1 */
       
    /******************************************************************************************************/
    /************ Test Case 1 : DDR Cipher Window0, AES - ECB 128 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ctr(SMAU_DDR_CW0, 0xDEADBEEF, 0x80041000, 0x80042000, 0x80060000, 0x80061000, AES_128, t_key0, t_key1, &ard[0], 0x5A5A5A5A, 0x00);

    /******************************************************************************************************/
    /************ Test Case 2 : DDR Cipher Window1, AES - ECB 128 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ctr(SMAU_DDR_CW1, 0xDEADBEEF, 0x80043000, 0x80044000, 0x80062000, 0x80063000, AES_128, t_key0, t_key1, &ard[0], 0xABCDABCD, 0x00);

    /******************************************************************************************************/
    /************ Test Case 3 : DDR Cipher Window2, AES - ECB 128 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ctr(SMAU_DDR_CW2, 0xDEADBEEF, 0x80045000, 0x80046000, 0x80064000, 0x80065000, AES_128, t_key0, t_key1, &ard[0], 0xDEADBEEF, 0x00);

    /******************************************************************************************************/
    /************ Test Case 4 : DDR Cipher Window3, AES - ECB 128 bit Key ddata_end_addrata access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ctr(SMAU_DDR_CW3, 0xDEADBEEF, 0x80047000, 0x80049000, 0x80066000, 0x80067000, AES_128, t_key0, t_key1, &ard[0], 0xA5A5A5A5, 0x00);

 
    post_log("\n-- Test %s --\n",test_status == TEST_PASS ? "PASSED" : "FAILED");
    return test_status;
}/* End SMAU_ddr_rw_test */

/*==================================================================================================
*Function    : static int SMAU_ddr_rw_test_aes_ctr_192(void)
*Parameter   : none
*Description : Test Case 1 : DDR Cipher Window , AES - CTR 192 bit Key data access Test
	       Test Case 2 : DDR Cipher Window disable and access Encripted Data 
	       Test Case 3 : DDR Cipher Window , AES - CTR 192 bit Key data access on AES re-enable
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
static int SMAU_ddr_rw_aes_ctr_192(void)
{
    int test_status = TEST_PASS;
    uint32_t *t_key0 = (uint32_t *)&key0; /* Key Engine 0 */
    uint32_t *t_key1 = (uint32_t *)&key1; /* Key Engine 1 */
       
    /******************************************************************************************************/
    /************ Test Case 1 : DDR Cipher Window0, AES - CTR 192 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ctr(SMAU_DDR_CW0, 0xDEADBEEF, 0x80041000, 0x80042000, 0x80060000, 0x80061000, AES_192, t_key0, t_key1, &ard[0], 0x5A5A5A5A, 0x00);

    /******************************************************************************************************/
    /************ Test Case 2 : DDR Cipher Window1, AES - CTR 192 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ctr(SMAU_DDR_CW1, 0xDEADBEEF, 0x80043000, 0x80044000, 0x80062000, 0x80063000, AES_192, t_key0, t_key1, &ard[0], 0xABCDABCD, 0x00);

    /******************************************************************************************************/
    /************ Test Case 3 : DDR Cipher Window2, AES - CTR 192 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ctr(SMAU_DDR_CW2, 0xDEADBEEF, 0x80045000, 0x80046000, 0x80064000, 0x80065000, AES_192, t_key0, t_key1, &ard[0], 0xDEADBEEF, 0x00);

    /******************************************************************************************************/
    /************ Test Case 4 : DDR Cipher Window3, AES - CTR 192 bit Key ddata_end_addrata access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ctr(SMAU_DDR_CW3, 0xDEADBEEF, 0x80047000, 0x80049000, 0x80066000, 0x80067000, AES_192, t_key0, t_key1, &ard[0], 0xA5A5A5A5, 0x00);

 
    post_log("\n-- Test %s --\n",test_status == TEST_PASS ? "PASSED" : "FAILED");
    return test_status;
}/* End SMAU_ddr_rw_test_aes_ctr_192*/


/*==================================================================================================
*Function    : static int SMAU_ddr_rw_test_aes_ctr_256(void)
*Parameter   : none
*Description : Test Case 1 : DDR Cipher Window , AES - CTR 256 bit Key data access Test
	       Test Case 2 : DDR Cipher Window disable and access Encripted Data 
	       Test Case 3 : DDR Cipher Window , AES - CTR 256 bit Key data access on AES re-enable
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
static int SMAU_ddr_rw_aes_ctr_256(void)
{
    int test_status = TEST_PASS;
    uint32_t *t_key0 = (uint32_t *)&key0; /* Key Engine 0 */
    uint32_t *t_key1 = (uint32_t *)&key1; /* Key Engine 1 */
       
    /******************************************************************************************************/
    /************ Test Case 1 : DDR Cipher Window0, AES - CTR 256 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ctr(SMAU_DDR_CW0, 0xDEADBEEF, 0x80041000, 0x80042000, 0x80060000, 0x80061000, AES_256, t_key0, t_key1,&ard[0], 0x5A5A5A5A, 0x00);

    /******************************************************************************************************/
    /************ Test Case 2 : DDR Cipher Window1, AES - CTR 256 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ctr(SMAU_DDR_CW1, 0xDEADBEEF, 0x80043000, 0x80044000, 0x80062000, 0x80063000, AES_256, t_key0, t_key1,&ard[0], 0xABCDABCD, 0x00);

    /******************************************************************************************************/
    /************ Test Case 3 : DDR Cipher Window2, AES - CTR 256 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ctr(SMAU_DDR_CW2, 0xDEADBEEF, 0x80045000, 0x80046000, 0x80064000, 0x80065000, AES_256, t_key0, t_key1,&ard[0], 0xDEADBEEF, 0x00);

    /******************************************************************************************************/
    /************ Test Case 4 : DDR Cipher Window3, AES - CTR 256 bit Key ddata_end_addrata access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ctr(SMAU_DDR_CW3, 0xDEADBEEF, 0x80047000, 0x80049000, 0x80066000, 0x80067000, AES_256, t_key0, t_key1,&ard[0], 0xA5A5A5A5, 0x00);

 
    post_log("\n-- Test %s --\n",test_status == TEST_PASS ? "PASSED" : "FAILED");
    return test_status;
}/* End SMAU_ddr_rw_test_aes_ctr_256 */

/*==================================================================================================
*Function    : static int SMAU_ddr_rw_test_aes_ecb_128(void)
*Parameter   : none
*Description : Test Case 1 : DDR Cipher Window , AES - ECB 128 bit Key data access Test
			   Test Case 2 : DDR Cipher Window disable and access Encripted Data 
			   Test Case 3 : DDR Cipher Window , AES - ECB 128 bit Key data access on AES re-enable
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
static int SMAU_ddr_rw_aes_ecb_128(void)
{
    int test_status = TEST_PASS;
    uint32_t *t_key0 = (uint32_t *)&key0; /* Key Engine 0 */
    uint32_t *t_key1 = (uint32_t *)&key1; /* Key Engine 1 */
       
    /******************************************************************************************************/
    /************ Test Case 1 : DDR Cipher Window0, AES - ECB 128 bit Key data access Test ****************/
    /******************************************************************************************************/

    test_status += SMAU_ddr_aes_ecb(SMAU_DDR_CW0, 0xDEADBEEF, 0x80002000, 0x80003000, AES_128, t_key0, t_key1, 0x00 , 0x80002000 , 0x80003000 , &ard[0] , 0x00);
                   
    /******************************************************************************************************/
    /************ Test Case 2 : DDR Cipher Window1, AES - ECB 128 bit Key data access Test ****************/
    /******************************************************************************************************/
    //smau_crypto_debug_enable();
    test_status += SMAU_ddr_aes_ecb(SMAU_DDR_CW1, 0xDEADBEEF, 0x80005000, 0x80006000, AES_128, t_key0, t_key1, 0x00 , 0x80005000 , 0x80006000 , &ard[0] , 0x00);

    /******************************************************************************************************/
    /************ Test Case 3 : DDR Cipher Window2, AES - ECB 128 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ecb(SMAU_DDR_CW2, 0xDEADBEEF, 0x80008000, 0x80009000, AES_128, t_key0, t_key1, 0x00 , 0x80008000 , 0x80009000 , &ard[0] , 0x00);

    /******************************************************************************************************/
    /************ Test Case 4 : DDR Cipher Window3, AES - ECB 128 bit Key ddata_end_addrata access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ecb(SMAU_DDR_CW3, 0xDEADBEEF, 0x80012000, 0x80013000, AES_128, t_key0, t_key1, 0x00 , 0x80012000 , 0x80013000 , &ard[0] , 0x00);

 
    post_log("\n-- Test %s --\n",test_status == TEST_PASS ? "PASSED" : "FAILED");
    return test_status;
}/* End SMAU_ddr_rw_test */

/*==================================================================================================
*Function    : static int SMAU_ddr_rw_test_trust_zone(void)
*Parameter   : none
*Description : Test Case 1 : DDR Cipher Window , AES - ECB 128 bit Key data access Test
			   Test Case 2 : DDR Cipher Window disable and access Encripted Data 
			   Test Case 3 : DDR Cipher Window , AES - ECB 128 bit Key data access on AES re-enable
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
static int SMAU_ddr_rw_trust_zone(void)
{
    int test_status = TEST_PASS;
    uint32_t *t_key0 = (uint32_t *)&key0; /* Key Engine 0 */
    uint32_t *t_key1 = (uint32_t *)&key1; /* Key Engine 1 */
    
    smau_set_trustzone_window(SMAU_DDR_CW0, 0x80002000, 0x80003000) ;

    /******************************************************************************************************/
    /************ Test Case 1 : DDR Cipher Window0, AES - ECB 128 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ecb(SMAU_DDR_CW0, 0xDEADBEEF, 0x80002000, 0x80003000, AES_128, t_key0, t_key1, 0x00 , 0x80002000 , 0x80003000 , &ard[0] , 0x00);
                   
    post_log("\n-- Test %s --\n",test_status == TEST_PASS ? "PASSED" : "FAILED");
    return test_status;
}/* End SMAU_ddr_rw_test */




/*==================================================================================================
*Function    : static int SMAU_ddr_rw_test_aes_ecb_128(void)
*Parameter   : none
*Description : Test Case 1 : DDR Cipher Window , AES - ECB 128 bit Key data access Test
			   Test Case 2 : DDR Cipher Window disable and access Encripted Data 
			   Test Case 3 : DDR Cipher Window , AES - ECB 128 bit Key data access on AES re-enable
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
static int SMAU_ddr_rw_aes_ecb_128_cache_ctrl(void)
{
    int test_status = TEST_PASS;
    uint32_t *t_key0 = (uint32_t *)&key0; /* Key Engine 0 */
    uint32_t *t_key1 = (uint32_t *)&key1; /* Key Engine 1 */
    uint32_t value = 0x00;
     
    smau_cache_invalidate();   
    smau_cache_enable(SMAU_CACHE_ENABLE); //enable Cache
    post_log("\nEnable SMAU Cache");

    value = smau_rd( SMAU_CACHE_CONTROL );
    post_log("\nRegister SMAU_CACHE_CONTROL = 0x%X",value);

    value = smau_rd( SMAU_CACHE_CTRL_STATUS1 );
    post_log("\nRegister SMAU_CACHE_CONTROL_STATUS1 = 0x%X",value);

    //value = smau_rd( SMAU_CACHE_CTRL_STATUS2 );
    //post_log("\nRegister SMAU_CACHE_CONTROL_STATUS2 = 0x%X",value);

    //value = smau_rd( SMAU_CACHE_CTRL_STATUS3 );
    //post_log("\nRegister SMAU_CACHE_CONTROL_STATUS3 = 0x%X",value);

    /******************************************************************************************************/
    /************ Test Case 1 : DDR Cipher Window0, AES - ECB 128 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ecb_transfer(SMAU_DDR_CW0, 0xDEADBEEF, 0x80002000, 0x80003000, AES_128,0x00 , 0x00 , 0x00 , &ard[0] , 0x00 , 0x00);
    value = smau_rd( SMAU_CACHE_CTRL_STATUS1 );
    post_log("\nRegister SMAU_CACHE_CONTROL_STATUS1 = 0x%X",value);
    test_status += SMAU_ddr_aes_ecb_transfer(SMAU_DDR_CW0, 0xDEADBEEF, 0x80002000, 0x80003000, AES_128,0x00 , 0x00 , 0x00 , &ard[0] , 0x00 , 0x01);

    
    value = smau_rd( SMAU_CACHE_CONTROL );
    post_log("\nRegister SMAU_CACHE_CONTROL = 0x%X",value);

    value = smau_rd( SMAU_CACHE_CTRL_STATUS1 );
    post_log("\nRegister SMAU_CACHE_CONTROL_STATUS1 = 0x%X",value);

    //value = smau_rd( SMAU_CACHE_CTRL_STATUS2 );
    //post_log("\nRegister SMAU_CACHE_CONTROL_STATUS2 = 0x%X",value);

    //value = smau_rd( SMAU_CACHE_CTRL_STATUS3 );
    //post_log("\nRegister SMAU_CACHE_CONTROL_STATUS3 = 0x%X",value);

    smau_cache_invalidate();
    smau_cache_enable(SMAU_CACHE_DISABLE);
    post_log("\nDisable SMAU Cache");

    value = smau_rd( SMAU_CACHE_CONTROL );
    post_log("\nRegister SMAU_CACHE_CONTROL = 0x%X",value);

    value = smau_rd( SMAU_CACHE_CTRL_STATUS1 );
    post_log("\nRegister SMAU_CACHE_CONTROL_STATUS1 = 0x%X",value);

    //value = smau_rd( SMAU_CACHE_CTRL_STATUS2 );
    //post_log("\nRegister SMAU_CACHE_CONTROL_STATUS2 = 0x%X",value);

    //value = smau_rd( SMAU_CACHE_CTRL_STATUS3 );
    //post_log("\nRegister SMAU_CACHE_CONTROL_STATUS3 = 0x%X",value);

    /******************************************************************************************************/
    /************ Test Case 1 : DDR Cipher Window0, AES - ECB 128 bit Key data access Test ****************/
    /******************************************************************************************************/

    test_status += SMAU_ddr_aes_ecb(SMAU_DDR_CW0, 0xDEADBEEF, 0x80002000, 0x80003000, AES_128, t_key0, t_key1, 0x00 , 0x00 , 0x00 , &ard[0], 0x00);
                   
    value = smau_rd( SMAU_CACHE_CONTROL );
    post_log("\nRegister SMAU_CACHE_CONTROL = 0x%X",value);

    value = smau_rd( SMAU_CACHE_CTRL_STATUS1 );
    post_log("\nRegister SMAU_CACHE_CONTROL_STATUS1 = 0x%X",value);

    //value = smau_rd( SMAU_CACHE_CTRL_STATUS2 );
    //post_log("\nRegister SMAU_CACHE_CONTROL_STATUS2 = 0x%X",value);

    //value = smau_rd( SMAU_CACHE_CTRL_STATUS3 );
    //post_log("\nRegister SMAU_CACHE_CONTROL_STATUS3 = 0x%X",value);

    post_log("\n-- Test %s --\n",test_status == TEST_PASS ? "PASSED" : "FAILED");
    return test_status;
}/* End SMAU_ddr_rw_test */
/*==================================================================================================
*Function    : static int SMAU_ddr_rw_test_aes_ecb_192(void)
*Parameter   : none
*Description : Test Case 1 : DDR Cipher Window , AES - ECB 192 bit Key data access Test
			   Test Case 2 : DDR Cipher Window disable and access Encripted Data 
			   Test Case 3 : DDR Cipher Window , AES - ECB 192 bit Key data access on AES re-enable
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
static int SMAU_ddr_rw_aes_ecb_192(void)
{
    int test_status = TEST_PASS;
    uint32_t *t_key0 = (uint32_t *)&key0; /* Key Engine 0 */
    uint32_t *t_key1 = (uint32_t *)&key1; /* Key Engine 1 */
       
    /******************************************************************************************************/
    /************ Test Case 1 : DDR Cipher Window0, AES - ECB 192 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ecb(SMAU_DDR_CW0, 0xDEADBEEF, 0x80002000, 0x80003000, AES_192, t_key0, t_key1, 0x00 , 0x80002000 , 0x80003000 , &ard[0] , 0x00);

    /******************************************************************************************************/
    /************ Test Case 2 : DDR Cipher Window1, AES - ECB 192 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ecb(SMAU_DDR_CW1, 0xDEADBEEF, 0x80005000, 0x80006000, AES_192, t_key0, t_key1, 0x00 , 0x80005000 , 0x80006000 , &ard[0] , 0x00);

    /******************************************************************************************************/
    /************ Test Case 3 : DDR Cipher Window2, AES - ECB 192 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ecb(SMAU_DDR_CW2, 0xDEADBEEF, 0x80008000, 0x80009000, AES_192, t_key0, t_key1, 0x00 , 0x80008000 , 0x80009000 , &ard[0] , 0x00);

    /******************************************************************************************************/
    /************ Test Case 4 : DDR Cipher Window3, AES - ECB 192 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ecb(SMAU_DDR_CW3, 0xDEADBEEF, 0x80012000, 0x80013000, AES_192, t_key0, t_key1, 0x00 , 0x80012000 , 0x80013000 , &ard[0] , 0x00);

 
    post_log("-- Test %s --\n",test_status == TEST_PASS ? "PASSED" : "FAILED");
    return test_status;
}/* End SMAU_ddr_rw_test */

/*==================================================================================================
*Function    : static int SMAU_ddr_rw_test_aes_ecb_256(void)
*Parameter   : none
*Description : Test Case 1 : DDR Cipher Window , AES - ECB 256 bit Key data access Test
			   Test Case 2 : DDR Cipher Window disable and access Encripted Data 
			   Test Case 3 : DDR Cipher Window , AES - ECB 256 bit Key data access on AES re-enable
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
static int SMAU_ddr_rw_aes_ecb_256(void)
{
    int test_status = TEST_PASS;
    uint32_t *t_key0 = (uint32_t *)&key0; /* Key Engine 0 */
    uint32_t *t_key1 = (uint32_t *)&key1; /* Key Engine 1 */
       
    /******************************************************************************************************/
    /************ Test Case 1 : DDR Cipher Window0, AES - ECB 256 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ecb(SMAU_DDR_CW0, 0xDEADBEEF, 0x80002000, 0x80003000, AES_256, t_key0, t_key1, 0x00 , 0x80002000 , 0x80003000 , &ard[0] , 0x00);

    /******************************************************************************************************/
    /************ Test Case 2 : DDR Cipher Window1, AES - ECB 256 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ecb(SMAU_DDR_CW1, 0xDEADBEEF, 0x80005000, 0x80006000, AES_256, t_key0, t_key1, 0x00 , 0x80005000 , 0x80006000 , &ard[0] , 0x00);

    /******************************************************************************************************/
    /************ Test Case 3 : DDR Cipher Window2, AES - ECB 256 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ecb(SMAU_DDR_CW2, 0xDEADBEEF, 0x80008000, 0x80009000, AES_256, t_key0, t_key1, 0x00 , 0x80008000 , 0x80009000 , &ard[0] , 0x00);

    /******************************************************************************************************/
    /************ Test Case 4 : DDR cipher Window3, AES - ECB 256 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ecb(SMAU_DDR_CW3, 0xDEADBEEF, 0x80012000, 0x80013000, AES_256, t_key0, t_key1, 0x00 , 0x80012000 , 0x80013000 , &ard[0] , 0x00);

 
    post_log("-- Test %s --\n",test_status == TEST_PASS ? "PASSED" : "FAILED");
    return test_status;
}/* End SMAU_ddr_rw_test */

/*==================================================================================================
*Function    : static int SMAU_ddr_rw_test_aes_ecb_128_sha(void)
*Parameter   : none
*Description : Test Case 1 : DDR Cipher Window , AES - ECB 128 bit Key data access Test
			   Test Case 2 : DDR Cipher Window disable and access Encripted Data 
			   Test Case 3 : DDR Cipher Window , AES - ECB 128 bit Key data access on AES re-enable
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
static int SMAU_ddr_rw_aes_ecb_128_sha(void)
{
    int test_status = TEST_PASS;
    uint32_t *t_key0 = (uint32_t *)&key0; /* Key Engine 0 */
    uint32_t *t_key1 = (uint32_t *)&key1; /* Key Engine 1 */
       
    /******************************************************************************************************/
    /************ Test Case 1 : DDR Cipher Window0, AES - ECB 128 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ecb(SMAU_DDR_CW0, 0xDEADBEEF, 0x80002000, 0x80003000, AES_128, t_key0, t_key1, 0x01, 0x80052000, 0x80053000 , &ard[0], 0x5A5A5A5A);

    /******************************************************************************************************/
    /************ Test Case 2 : DDR Cipher Window1, AES - ECB 128 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ecb(SMAU_DDR_CW1, 0xDEADBEEF, 0x80005000, 0x80006000, AES_128, t_key0, t_key1, 0x01, 0x80054000, 0x80055000 , &ard[0], 0x5A5A5A5A);

    /******************************************************************************************************/
    /************ Test Case 3 : DDR Cipher Window2, AES - ECB 128 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ecb(SMAU_DDR_CW2, 0xDEADBEEF, 0x80008000, 0x80009000, AES_128, t_key0, t_key1, 0x01, 0x80056000, 0x80057000 , &ard[0], 0x5A5A5A5A);

    /******************************************************************************************************/
    /************ Test Case 4 : DDR Cipher Window3, AES - ECB 128 bit Key ddata_end_addrata access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ecb(SMAU_DDR_CW3, 0xDEADBEEF, 0x80012000, 0x80013000, AES_128, t_key0, t_key1, 0x01, 0x80058000, 0x80059000 , &ard[0], 0x5A5A5A5A);

 
    post_log("\n-- Test %s --\n",test_status == TEST_PASS ? "PASSED" : "FAILED");
    return test_status;
}/* End SMAU_ddr_rw_test */

/*==================================================================================================
*Function    : static int SMAU_ddr_rw_test_aes_ecb_192_sha(void)
*Parameter   : none
*Description : Test Case 1 : DDR Cipher Window , AES - ECB 192 bit Key data access Test
			   Test Case 2 : DDR Cipher Window disable and access Encripted Data 
			   Test Case 3 : DDR Cipher Window , AES - ECB 192 bit Key data access on AES re-enable
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
static int SMAU_ddr_rw_aes_ecb_192_sha(void)
{
    int test_status = TEST_PASS;
    uint32_t *t_key0 = (uint32_t *)&key0; /* Key Engine 0 */
    uint32_t *t_key1 = (uint32_t *)&key1; /* Key Engine 1 */
       
    /******************************************************************************************************/
    /************ Test Case 1 : DDR Cipher Window0, AES - ECB 192 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ecb(SMAU_DDR_CW0, 0xDEADBEEF, 0x80002000, 0x80003000, AES_192, t_key0, t_key1, 0x01, 0x80052000, 0x80053000 ,&ard[0], 0x5A5A5A5A);

    /******************************************************************************************************/
    /************ Test Case 2 : DDR Cipher Window1, AES - ECB 192 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ecb(SMAU_DDR_CW1, 0xDEADBEEF, 0x80005000, 0x80006000, AES_192, t_key0, t_key1, 0x01, 0x80054000, 0x80055000 , &ard[0], 0x5A5A5A5A);

    /******************************************************************************************************/
    /************ Test Case 3 : DDR Cipher Window2, AES - ECB 192 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ecb(SMAU_DDR_CW2, 0xDEADBEEF, 0x80008000, 0x80009000, AES_192, t_key0, t_key1, 0x01, 0x80056000, 0x80057000 , &ard[0], 0x5A5A5A5A);

    /******************************************************************************************************/
    /************ Test Case 4 : DDR Cipher Window3, AES - ECB 192 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ecb(SMAU_DDR_CW3, 0xDEADBEEF, 0x80012000, 0x80013000, AES_192, t_key0, t_key1, 0x01, 0x80058000, 0x80059000 , &ard[0], 0x5A5A5A5A);

 
    post_log("-- Test %s --\n",test_status == TEST_PASS ? "PASSED" : "FAILED");
    return test_status;
}/* End SMAU_ddr_rw_test */

/*==================================================================================================
*Function    : static int SMAU_ddr_rw_test_aes_ecb_256_sha(void)
*Parameter   : none
*Description : Test Case 1 : DDR Cipher Window , AES - ECB 256 bit Key data access Test
			   Test Case 2 : DDR Cipher Window disable and access Encripted Data 
			   Test Case 3 : DDR Cipher Window , AES - ECB 256 bit Key data access on AES re-enable
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
static int SMAU_ddr_rw_aes_ecb_256_sha(void)
{
    int test_status = TEST_PASS;
    uint32_t *t_key0 = (uint32_t *)&key0; /* Key Engine 0 */
    uint32_t *t_key1 = (uint32_t *)&key1; /* Key Engine 1 */
       
    /******************************************************************************************************/
    /************ Test Case 1 : DDR Cipher Window0, AES - ECB 256 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ecb(SMAU_DDR_CW0, 0xDEADBEEF, 0x80002000, 0x80003000, AES_256, t_key0, t_key1, 0x01, 0x80052000, 0x80053000 , &ard[0], 0x5A5A5A5A);

    /******************************************************************************************************/
    /************ Test Case 2 : DDR Cipher Window1, AES - ECB 256 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ecb(SMAU_DDR_CW1, 0xDEADBEEF, 0x80005000, 0x80006000, AES_256, t_key0, t_key1, 0x01, 0x80054000, 0x80055000 , &ard[0], 0x5A5A5A5A);

    /******************************************************************************************************/
    /************ Test Case 3 : DDR Cipher Window2, AES - ECB 256 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ecb(SMAU_DDR_CW2, 0xDEADBEEF, 0x80008000, 0x80009000, AES_256, t_key0, t_key1, 0x01, 0x80056000, 0x80057000 , &ard[0], 0x5A5A5A5A);

    /******************************************************************************************************/
    /************ Test Case 4 : DDR cipher Window3, AES - ECB 256 bit Key data access Test ****************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_aes_ecb(SMAU_DDR_CW3, 0xDEADBEEF, 0x80012000, 0x80013000, AES_256, t_key0, t_key1, 0x01, 0x80058000, 0x80059000 , &ard[0], 0x5A5A5A5A);

 
    post_log("-- Test %s --\n",test_status == TEST_PASS ? "PASSED" : "FAILED");
    return test_status;
}/* End SMAU_ddr_rw_test */



/*==================================================================================================
*Function    : static int SMAU_ddr_rw_test_plaintext(void)
*Parameter   : none
*Description : Plain text read write DDR
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
static int SMAU_ddr_rw_plaintext(void)
{
    int test_status = TEST_PASS;
       
    /******************************************************************************************************/
    /**************************** Test Case : Plain text read write DDR Test ******************************/
    /******************************************************************************************************/
    test_status += SMAU_ddr_plain_text(SMAU_DDR_CW0, 0x5A5A5A5A, 0x80002000,0x80003000);
 
    post_log("-- Test %s --\n",test_status == TEST_PASS ? "PASSED" : "FAILED");
    return test_status;
}/* End SMAU_ddr_rw_test_plaintext */


static int SMAU_reg_read(void)
{
uint32_t value = 0x00;
value = smau_rd( SMAU_DDR_TZ_BASE );
post_log("Register SMAU_DDR_TZ_BASE = 0x%X ",value);

value = smau_rd( SMAU_DDR_TZ_END );
post_log("\n Register SMAU_DDR_TZ_END = 0x%X ",value);  

value = smau_rd( SMAU_FLASH0_TZ_BASE );
post_log("\n Register SMAU_FLASH0_TZ_BASE = 0x%X ",value);  

value = smau_rd( SMAU_FLASH0_TZ_END );
post_log("\n Register SMAU_FLASH0_TZ_END = 0x%X ",value);  
post_log("\n-- Test PASSED --\n");
return 0;
} 

static int SMAU_ddr_peep_window(void)
{
    uint32_t addr;
    int test_status = TEST_PASS;
    uint32_t wdata, rdata;
    uint32_t data = 0x5A5A5A5A;
    uint32_t data_start_addr, data_end_addr;
    smau_cache_invalidate();
    smau_cache_enable(SMAU_CACHE_DISABLE);/*Disable Cache*/
    smau_window_config window_cfg;

   /********* Cipher Window Configuration ***********************/
	window_cfg.window_num                   = SMAU_DDR_CW0;
	window_cfg.data_start_addr              = 0x80002000;
	window_cfg.data_end_addr                = 0x80003000;
	window_cfg.window_en                    = 0x1;
	
    /********* AES configuration *********************************/
	window_cfg.aes_en                       = 0x0;
	window_cfg.aes_key_len                  = AES_128;
	window_cfg.aes_key0                     = (uint32_t *)&key0;
	window_cfg.aes_key1                     = (uint32_t *)&key1;
	window_cfg.ecb_mode_data_scramble       = 0x0;
	window_cfg.ctr_mode                     = 0x0;
	window_cfg.enable_random_aes_ctr        = 0x0;
	window_cfg.disable_iv_scramble          = 0x0;
	window_cfg.aad                          = 0x0;
	window_cfg.ard                          = &ard[0];
	window_cfg.cid				= 0x0;
	window_cfg.init_counter			= 0xDEAD;
    /********** Hash Algorithm Configuration ********************/
	window_cfg.hash_start_addr              = 0x80002000;
	window_cfg.hash_end_addr                = 0x80003000;
	window_cfg.non_secure                   = 0x0;
	window_cfg.sha_en                       = 0x0;
	window_cfg.disable_sha_out_scramble     = 0x0;
	window_cfg.hsalt                        = &hsalt[0];
	window_cfg.data2hash_offset             = 0x0;
    
    smau_set_window_config(window_cfg);

    data_start_addr = 0x80002000;
    data_end_addr   = 0x80002010;
    
    wdata = data;

    post_log("Writing plain text data in DDR\n");
    for(addr = data_start_addr; addr < data_end_addr; addr+=4) {
        wr(addr, wdata++);
    }

 smau_cache_invalidate();  
    post_log("Reading DDR data using peep window \n");
    wdata = data;
    smau_set_peep_window(window_cfg.data_start_addr,window_cfg.data_end_addr);
    for(addr = data_start_addr; addr < data_end_addr; addr+=4) {
        rdata = rd(addr);
        if(rdata != wdata) {
            error_log("error1: DDR data not expected - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n", addr, rdata, wdata);
            smau_clear_peep_window();
            test_status = TEST_FAIL;
            break;
        } else {
            //debug_log("DDR data - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n", addr, rdata, wdata);
        }
        wdata++;    
    }
   /*Enable aes encryption*/
    smau_clear_peep_window();
    window_cfg.aes_en = 0x1;
    smau_set_window_config(window_cfg);
    wdata = data;
    post_log("Writing Encrypted data in DDR\n");
    for(addr = data_start_addr; addr < data_end_addr; addr+=4) {
        wr(addr, wdata++);
    }
   
    wdata = data;
    post_log("Reading DDR decrypted data \n");
    for(addr = data_start_addr; addr < data_end_addr; addr+=4) {
        rdata = rd(addr);
        if(rdata != wdata) {
            error_log("error2: DDR data not expected - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n", addr, rdata, wdata);
            smau_clear_peep_window();
            test_status = TEST_FAIL;
            break;     
        } else { 
            //debug_log("DDR data - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n", addr, rdata, wdata);
        }
        wdata++; 
    }

    /*Configure peep window */
    smau_set_peep_window(window_cfg.data_start_addr,window_cfg.data_end_addr);
    wdata = data;
    post_log("Reading DDR data using peep window \n");
    for(addr = data_start_addr; addr < data_end_addr; addr+=4) {
        rdata = rd(addr);
        if(rdata == wdata) {
            error_log("error3: DDR data not expected - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n", addr, rdata, wdata);
	    post_log("\n-- Test FAIL --\n");
            smau_clear_peep_window();
            test_status = TEST_FAIL;
            break;
        } else {
            //debug_log("DDR data - addr=0x%08X - rdata=0x%08X - wdata=0x%08X\n", addr, rdata, wdata);
        }
        wdata++; 
    }
    smau_clear_peep_window();
    post_log("-- Test %s --\n",test_status == TEST_PASS ? "PASSED" : "FAILED");
    return test_status;
}
static int SMAU_config_aes_ecb(smau_window_type_e window_number, uint32_t w_start_addr, uint32_t w_end_addr, uint32_t key_length, uint32_t *t_key0, uint32_t *t_key1)
{

    smau_window_config window_cfg;

   /********* Cipher Window Configuration ***********************/
	window_cfg.window_num                   = window_number;
	window_cfg.data_start_addr              = w_start_addr;
	window_cfg.data_end_addr                = w_end_addr;
	window_cfg.window_en                    = 0x1;
	
    /********* AES configuration *********************************/
	window_cfg.aes_en                       = 0x1;
	window_cfg.aes_key_len                  = key_length;
	window_cfg.aes_key0                     = t_key0;
	window_cfg.aes_key1                     = t_key1;
	window_cfg.ecb_mode_data_scramble       = 0x0;
	window_cfg.ctr_mode                     = 0x0;
	window_cfg.enable_random_aes_ctr        = 0x0;
	window_cfg.disable_iv_scramble          = 0x0;
	window_cfg.aad                          = 0x0;
	window_cfg.ard                          = (uint32_t *)&ard;
	window_cfg.cid							= 0x00;
	window_cfg.init_counter					= 0xDEAD;
    /********** Hash Algorithm Configuration ********************/
	window_cfg.hash_start_addr              = 0x00;
	window_cfg.hash_end_addr                = 0x00;
	window_cfg.non_secure                   = 0x0;
	window_cfg.sha_en                       = 0x0;
	window_cfg.disable_sha_out_scramble     = 0x0;
	window_cfg.hsalt                        = &hsalt[0];
	window_cfg.data2hash_offset             = 0x0;

    /***********************************************************/
    /************ Configuring DDR Cipher Window ****************/
    /***********************************************************/

    post_log("\nConfig Cipher Window Range 0x%X to 0x%X\n as AES-ECB- %d",window_cfg.data_start_addr,window_cfg.data_end_addr,( 128 + (key_length * 64) ));  
    /* Window Configuration based on the window_cfg data */

    return smau_set_window_config(window_cfg);;

}

/*==================================================================================================
*Function    : static int SMAU_ddr_rw_test_aes_ecb_128(void)
*Parameter   : none
*Description : Test Case 1 : DDR Cipher Window , AES - ECB 128 bit Key data access Test
			   Test Case 2 : DDR Cipher Window disable and access Encripted Data 
			   Test Case 3 : DDR Cipher Window , AES - ECB 128 bit Key data access on AES re-enable
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
static int SMAU_ddr_config_aes_ecb_128(void)
{
    int test_status = TEST_PASS;
    uint32_t *t_key0 = (uint32_t *)&key0; /* Key Engine 0 */
    uint32_t *t_key1 = (uint32_t *)&key1; /* Key Engine 1 */
       
    test_status += SMAU_config_aes_ecb(SMAU_DDR_CW0, 0x60001000, 0x60002000, AES_128, t_key0, t_key1);
 
    post_log("\n-- Test %s --\n",test_status == TEST_PASS ? "PASSED" : "FAILED");
    return test_status;
}/* End SMAU_ddr_rw_test */

static int SMAU_qspi_config_aes_ecb_128(void)
{
    int test_status = TEST_PASS;
    uint32_t *t_key0 = (uint32_t *)&key0; /* Key Engine 0 */
    uint32_t *t_key1 = (uint32_t *)&key1; /* Key Engine 1 */
       
    test_status += SMAU_config_aes_ecb(SMAU_FLASH_CW1, 0xF0400000, 0xF4001000, AES_128, t_key0, t_key1);
 
    post_log("\n-- Test %s --\n",test_status == TEST_PASS ? "PASSED" : "FAILED");
    return test_status;
}/* End SMAU_ddr_rw_test */

static int SMAU_nand_config_aes_ecb_128(void)
{
    int test_status = TEST_PASS;
    uint32_t *t_key0 = (uint32_t *)&key0; /* Key Engine 0 */
    uint32_t *t_key1 = (uint32_t *)&key1; /* Key Engine 1 */
       
    test_status += SMAU_config_aes_ecb(SMAU_FLASH_CW2, 0xE0000000, 0xE0001000, AES_128, t_key0, t_key1);
 
    post_log("\n-- Test %s --\n",test_status == TEST_PASS ? "PASSED" : "FAILED");
    return test_status;
}/* End SMAU_ddr_rw_test */

static int SMAU_ddr_disable_aes_ecb_window(void)
{
    int test_status = TEST_PASS;
       
    test_status = SMAU_ddr_disable_window(SMAU_DDR_CW0, 0x60001000, 0x60002000)  ;  
   
    post_log("\n-- Test %s --\n",test_status == TEST_PASS ? "PASSED" : "FAILED");
    return test_status;
}/* End SMAU_ddr_rw_test */


static int SMAU_qspi_disable_aes_ecb_window(void)
{
    int test_status = TEST_PASS;
       
    test_status = SMAU_ddr_disable_window(SMAU_FLASH_CW1, 0xF0400000, 0xF0401000)  ;  
   
    post_log("\n-- Test %s --\n",test_status == TEST_PASS ? "PASSED" : "FAILED");
    return test_status;
}/* End SMAU_ddr_rw_test */

static int SMAU_nand_disable_aes_ecb_window(void)
{
    int test_status = TEST_PASS;
       
    test_status = SMAU_ddr_disable_window(SMAU_FLASH_CW2, 0xE0000000, 0xE0001000)  ;  
   
    post_log("\n-- Test %s --\n",test_status == TEST_PASS ? "PASSED" : "FAILED");
    return test_status;
}/* End SMAU_ddr_rw_test */


/*==================================================================================================
*Description : The order of priority for security is PEEP > Cipher-Text Window > Secure Trustzone
*Parameter   :
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
static int SMAU_configure_aes_ecb_window(uint32_t window, uint32_t start, uint32_t end, uint32_t key_l, uint32_t *key0, uint32_t *key1, uint32_t data_scramble, uint32_t secure)
{
	/********* Cipher Window Configuration ***********************/
	window_cfg.window_num                   = window;
	window_cfg.data_start_addr              = start;
	window_cfg.data_end_addr                = end;
	window_cfg.window_en                    = ENABLE;

	/********* AES configuration *********************************/
	window_cfg.aes_en                       = ENABLE;
	window_cfg.enable_random_aes_ctr        = DISABLE;
	window_cfg.ctr_mode                     = DISABLE;

	window_cfg.aes_key_len                  = key_l;
	window_cfg.aes_key0                     = key0;
	window_cfg.aes_key1                     = key1;

	window_cfg.ecb_mode_data_scramble       = data_scramble;
	window_cfg.disable_iv_scramble          = DISABLE;
	window_cfg.aad                          = 0x0;
	window_cfg.ard                          = (uint32_t *)&ard;
	window_cfg.cid							= 0x00;
	window_cfg.init_counter					= 0xDEAD;
	/********** Hash Algorithm Configuration ********************/
	window_cfg.hash_start_addr              = start;
	window_cfg.hash_end_addr                = end;
	window_cfg.non_secure                   = secure;
	window_cfg.sha_en                       = DISABLE;
	window_cfg.disable_sha_out_scramble     = DISABLE;
	window_cfg.hsalt                        = &hsalt[0];

	/***********************************************************/
	/************ Configuring DDR Cipher Window ****************/
	/***********************************************************/
	post_log("Config Cipher Window Range 0x%X to 0x%X as AES-ECB- %d\n",window_cfg.data_start_addr,window_cfg.data_end_addr,( 128 + (window_cfg.aes_key_len * 64) ));
	/* Window Configuration based on the window_cfg data */
	return smau_set_window_config(window_cfg);
}

static int SMAU_disable_aes_ecb_encryption(uint32_t window, uint32_t start, uint32_t end, uint32_t key_l, uint32_t *key0, uint32_t *key1, uint32_t data_scramble, uint32_t secure)
{
	/********* Cipher Window Configuration ***********************/
	window_cfg.window_num                   = window;
	window_cfg.data_start_addr              = start;
	window_cfg.data_end_addr                = end;
	window_cfg.window_en                    = ENABLE;

	/********* AES configuration *********************************/
	window_cfg.aes_en                       = DISABLE;
	window_cfg.enable_random_aes_ctr        = DISABLE;
	window_cfg.ctr_mode                     = DISABLE;

	window_cfg.aes_key_len                  = key_l;
	window_cfg.aes_key0                     = key0;
	window_cfg.aes_key1                     = key1;

	window_cfg.ecb_mode_data_scramble       = data_scramble;
	window_cfg.disable_iv_scramble          = DISABLE;
	window_cfg.aad                          = 0x0;
	window_cfg.ard                          = (uint32_t *)&ard;
	window_cfg.cid							= 0x00;
	window_cfg.init_counter					= 0xDEAD;
	/********** Hash Algorithm Configuration ********************/
	window_cfg.hash_start_addr              = start;
	window_cfg.hash_end_addr                = end;
	window_cfg.non_secure                   = secure;
	window_cfg.sha_en                       = DISABLE;
	window_cfg.disable_sha_out_scramble     = DISABLE;
	window_cfg.hsalt                        = &hsalt[0];

	/***********************************************************/
	/************ Configuring DDR Cipher Window ****************/
	/***********************************************************/
	post_log("Config Cipher Window Range 0x%X to 0x%X as AES-ECB- %d\n",window_cfg.data_start_addr,window_cfg.data_end_addr,( 128 + (window_cfg.aes_key_len * 64) ));
	/* Window Configuration based on the window_cfg data */
	return smau_set_window_config(window_cfg);
}
#if 0
static int SMAU_disable_window(uint32_t window)
{
	/********* Cipher Window Configuration ***********************/
	window_cfg.window_num                   = window;
	window_cfg.data_start_addr              = 0x00;
	window_cfg.data_end_addr                = 0x00;
	window_cfg.window_en                    = DISABLE;

	/********* AES configuration *********************************/
	window_cfg.aes_en                       = DISABLE;
	window_cfg.ctr_mode                     = DISABLE;
	window_cfg.enable_random_aes_ctr        = DISABLE;

	window_cfg.aes_key_len                  = AES_128;
	window_cfg.aes_key0                     = key0;
	window_cfg.aes_key1                     = key1;

	window_cfg.ecb_mode_data_scramble       = DISABLE;
	window_cfg.disable_iv_scramble          = DISABLE;
	window_cfg.aad                          = 0x0;
	window_cfg.ard                          = (uint32_t *)&ard;
	window_cfg.cid							= 0x00;
	window_cfg.init_counter					= 0xDEAD;
	/********** Hash Algorithm Configuration ********************/
	window_cfg.hash_start_addr              = 0x0;
	window_cfg.hash_end_addr                = 0x0;
	window_cfg.non_secure                   = 0x0;
	window_cfg.sha_en                       = DISABLE;
	window_cfg.disable_sha_out_scramble     = DISABLE;
	window_cfg.hsalt                        = &hsalt[0];
	/***********************************************************/
	/************ Configuring DDR Cipher Window ****************/
	/***********************************************************/
	post_log("Cipher Window %d disabled\n",window);
	/* Window Configuration based on the window_cfg data */
	return smau_set_window_config(window_cfg);
}
#endif
extern void smau_disable_window(uint32_t window_number);
static void SMAU_disable_window(uint32_t window)
{
	post_log("Cipher Window %d disabled\n",window);
	/* Window Configuration based on the window_cfg data */
	smau_disable_window(window);
}

static int SMAU_configure_aes_ctr_window(uint32_t window, uint32_t start, uint32_t end, uint32_t key_l, uint32_t *key0, uint32_t *key1, uint32_t random_ctr, uint32_t hash_start, uint32_t hash_end, uint32_t secure )
{
	/********* Cipher Window Configuration ***********************/
	window_cfg.window_num                   = window;
	window_cfg.data_start_addr              = start;
	window_cfg.data_end_addr                = end;
	window_cfg.window_en                    = ENABLE;

	/********* AES configuration *********************************/
	window_cfg.aes_en                       = ENABLE;
	window_cfg.ctr_mode                     = ENABLE;
	window_cfg.enable_random_aes_ctr        = random_ctr;

	window_cfg.aes_key_len                  = key_l;
	window_cfg.aes_key0                     = key0;
	window_cfg.aes_key1                     = key1;

	window_cfg.ecb_mode_data_scramble       = DISABLE;
	window_cfg.disable_iv_scramble          = DISABLE;
	window_cfg.aad                          = 0x0;
	window_cfg.ard                          = (uint32_t *)&ard;
	window_cfg.cid							= 0x00;
	window_cfg.init_counter					= 0xDEAD;
	/********** Hash Algorithm Configuration ********************/
	window_cfg.hash_start_addr              = hash_start;
	window_cfg.hash_end_addr                = hash_end;
	window_cfg.non_secure                   = secure;
	window_cfg.sha_en                       = ENABLE;
	window_cfg.disable_sha_out_scramble     = DISABLE;
	window_cfg.hsalt                        = &hsalt[0];
	/***********************************************************/
	/************ Configuring DDR Cipher Window ****************/
	/***********************************************************/
	post_log("Config Cipher Window Range 0x%X to 0x%X as AES-CTR- %d random counter - %s\n",window_cfg.data_start_addr,window_cfg.data_end_addr,( 128 + (window_cfg.aes_key_len * 64) ), random_ctr ? "enable" : "disable");
	/* Window Configuration based on the window_cfg data */
	return smau_set_window_config(window_cfg);
}
/*------------------------ Test end : SMAU_multiple_non_overlaping_window_access ----------------------------------*/
/*==================================================================================================
*Description : The order of priority  PEEP > Cipher-Text Window > Secure Trustzone
*Parameter   :
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
static int __SMAU_write_invalidate_cache(void)
{
	int result = TEST_PASS;
	uint32_t save_tz_base    = 0x00;
	uint32_t save_tz_end     = 0x00;
	uint32_t save_tz_enable  = 0x00;

	uint8_t common_key0[]     = "CW1_key0_aeskey0_diagnostics_aes";
	uint8_t common_key1[]     = "CW1_key1_aeskey1_diagnostics_aes";

	uint32_t DDR_CW1_start = 0x90001000;
	uint32_t DDR_CW1_end   = 0x90003000;
	//uint32_t hash_start_1  = 0x90010000;
	//uint32_t hash_end_1	   = 0x90013000;

	uint32_t KB4_Aligner   = 0x1000;

	uint32_t reg_index  = 0x00,reg_index1 = 0;
	uint32_t DATA1  = 0xDEADBEEF;
	
	uint32_t DATA_CLEAR = 0x00000000;
	uint32_t rdata  = 0x00;

	uint32_t total_cache = 0x2000; //8Kb
	uint32_t prefetch_count = 0x00;
	uint32_t total_pfetch_byte = 0x00;	

	uint32_t TEST_AREA_START = DDR_CW1_start;
	uint32_t TEST_AREA_END   = DDR_CW1_end + KB4_Aligner;

	//Save TZ default settings
	get_tz_params(SMAU_DDR_CW0, &save_tz_base, &save_tz_end);
	post_log("current tz base : 0x%X , current tz end : 0x%X\n",save_tz_base,save_tz_end);
	save_tz_enable = ((save_tz_base >> SMAU_DDR_TZ_BASE__ENABLE) & 0x01);

	//Clean test area
	post_log("clearing the test area\n");
	for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Clear test area
		wr(reg_index, DATA_CLEAR);

	//for(reg_index = hash_start_1 ; reg_index < hash_end_1 ; reg_index += 4) //Clear test area
		//wr(reg_index, DATA_CLEAR);

	//Configure Window

	//SMAU_configure_aes_ctr_window(SMAU_DDR_CW1, DDR_CW1_start, DDR_CW1_end, AES_192, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE, hash_start_1, hash_end_1, NON_SECURE);
	SMAU_configure_aes_ecb_window(SMAU_DDR_CW0, DDR_CW1_start, DDR_CW1_end, AES_128, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE,  SECURE );

	post_log("executing test entity1\n");
	/*----- test entity1 ------*/
	for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4)//Write Known Data to test area
		wr(reg_index, DATA1);

	prefetch_count = 0x00;
	total_pfetch_byte = ((prefetch_count + 1) * 32);
	uint32_t cache_hit = 0x00,cache_miss = 0x00,value = 0x00;

	post_log("filling each cache line(32byte) using single read and verifying by reading from cache\n");
	post_log("address range : 0x%X to 0x%X\n",TEST_AREA_START,(TEST_AREA_START + total_cache - 1));

	for(reg_index = TEST_AREA_START ; reg_index < (TEST_AREA_START + total_cache) ; reg_index += total_pfetch_byte) //Read Known Data from test area
	{
		disable_log = 1;
		SMAU_configure_aes_ecb_window(SMAU_DDR_CW0, DDR_CW1_start, DDR_CW1_end, AES_128, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE,  SECURE );
		disable_log = 0;
		rdata = rd(reg_index);
		if(rdata != DATA1)
		{
			post_log("error (encryption mode ): data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,DATA1,rdata);
			result = TEST_FAIL;
			break;
		}
		disable_log = 1;
		SMAU_disable_aes_ecb_encryption(SMAU_DDR_CW0, DDR_CW1_start, DDR_CW1_end, AES_128, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE,  SECURE );
		disable_log = 0;
		for(reg_index1 = reg_index ; reg_index1 < (reg_index + total_pfetch_byte ) ; reg_index1 += 4) //Read Known Data from test area
		{
			cache_hit++;
			rdata = rd(reg_index1);

			if(rdata != DATA1)
			{
				post_log("error (non-encryption mode ): data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index1,DATA1,rdata);
				result = TEST_FAIL;
				break;
			}
		    value = smau_rd( SMAU_CACHE_CTRL_STATUS1 );
		    //post_log("Cache hit count = 0x%X\n",cache_hit);
		    //post_log("Register SMAU_CACHE_CONTROL_STATUS1 = 0x%X\n",value);
		    if(cache_hit != (value & 0xFFFF))
		    {
		    	post_log("error : expected cache hit count = 0x%X , actual = 0x%X\n",cache_hit,(value & 0xFFFF));
				result = TEST_FAIL;
		    	break;
		    }
		}
		cache_miss++;
		if(cache_miss != ( (value >> 16) & 0xFFFF))
		{
			post_log("error : expected cache miss count = 0x%X , actual = 0x%X\n",cache_miss,( (value >> 16) & 0xFFFF));
			result = TEST_FAIL;
			break;
		}

		if(reg_index1 != (reg_index + total_pfetch_byte))
		{
			result = TEST_FAIL;
			break;
		}
	}
    value = smau_rd( SMAU_CACHE_CTRL_STATUS1 );
    post_log("Register SMAU_CACHE_CONTROL_STATUS1 = 0x%X\n",value);
    post_log("Cache hit count = 0x%X\n",(value & 0xFFFF));
    post_log("Cache miss count = 0x%X\n",( (value >> 16) & 0xFFFF));
	post_log("last address accessed from cache : 0x%X\n",reg_index1);
	post_log("Expecting SMAU CACHE Full (8Kb)\n");
	post_log("Reading entire cache...\n");

	disable_log = 1;
	SMAU_disable_aes_ecb_encryption(SMAU_DDR_CW0, DDR_CW1_start, DDR_CW1_end, AES_128, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE,  SECURE );
	disable_log = 0;


	for(reg_index = TEST_AREA_START ; reg_index < (TEST_AREA_START + total_cache) ; reg_index += total_pfetch_byte) //Read Known Data from test area
	{
			rdata = rd(reg_index);
			if(rdata != DATA1)
			{
				post_log("error (non-encryption mode ): data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index1,DATA1,rdata);
				result = TEST_FAIL;
				break;
			}
	}
	post_log("Now invalidate each cache line by single write and verify \n");
	for(reg_index = TEST_AREA_START ; reg_index < (TEST_AREA_START + total_cache) ; reg_index += total_pfetch_byte) //Read Known Data from test area
	{
		disable_log = 1;
		SMAU_configure_aes_ecb_window(SMAU_DDR_CW0, DDR_CW1_start, DDR_CW1_end, AES_128, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE,  SECURE );
		disable_log = 0;

		wr(reg_index,DATA1);

		disable_log = 1;
		SMAU_disable_aes_ecb_encryption(SMAU_DDR_CW0, DDR_CW1_start, DDR_CW1_end, AES_128, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE,  SECURE );
		disable_log = 0;

		for(reg_index1 = reg_index ; reg_index1 < (reg_index + total_pfetch_byte ) ; reg_index1 += 4) //Read Known Data from test area
		{
			rdata = rd(reg_index1);
			if(rdata == DATA1)
			{
				post_log("error (non-encryption mode ): data error @  0x%X , data expected != 0x%X\n",reg_index1,rdata);
				result = TEST_FAIL;
				break;
			}
		}
		if(reg_index1 != (reg_index + total_pfetch_byte))
		{
			result = TEST_FAIL;
			break;
		}
	}

	/*-------Restore configs ------*/
	smau_clear_peep_window();
	smau_cache_invalidate();
	smau_set_trustzone_window(SMAU_DDR_CW0, save_tz_base , save_tz_end);
	smau_trustzone_window_control(SMAU_DDR_CW0, save_tz_enable);

	//post_log("\n-- Test %s --\n",result == TEST_PASS ? "PASSED" : "FAILED");
    return result;
}
extern void disable_all_window(void);
static int SMAU_write_invalidate_cache(void)
{
	int result = TEST_PASS, value = 0x00;
	//Cache config
	//struct DmuBlockEnable dbe;
	//dbe.smau_s0_idm = 1;
	//bcm_dmu_block_disable(dbe);
	//bcm_dmu_block_enable(dbe);

	smau_clear_peep_window();
	disable_all_window();
	smau_prefetch_length(0x0);
	smau_partial_mode(ENABLE);
	smau_cache_enable(ENABLE);
    value = smau_rd( SMAU_CACHE_CONTROL );
    post_log("\nRegister SMAU_CACHE_CONTROL = 0x%X",value);
    smau_wr(SMAU_CACHE_CTRL_STATUS1,0x00);
    value = smau_rd( SMAU_CACHE_CTRL_STATUS1 );
    post_log("\nRegister SMAU_CACHE_CONTROL_STATUS1 = 0x%X\n",value);

    if(smau_cache_invalidate())
		post_log("smau_cache_invalidate failed\n");
	post_log("Enabling Cache...\n");
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_write_invalidate_cache();

	smau_cache_enable(DISABLE);
	post_log("\n-- Test %s --\n",result == TEST_PASS ? "PASSED" : "FAILED");
	return result;
}
/*------------------------ Test end : SMAU_multiple_non_overlaping_window_access ----------------------------------*/
#if 0
/*------------------------ Test end : SMAU_multiple_non_overlaping_window_access ----------------------------------*/

static int __SMAU_write_invalidate_cache(void)
{
	int result = TEST_PASS;
	uint32_t save_tz_base    = 0x00;
	uint32_t save_tz_end     = 0x00;
	uint32_t save_tz_enable  = 0x00;

	uint8_t common_key0[]     = "CW1_key0_aeskey0_diagnostics_aes";
	uint8_t common_key1[]     = "CW1_key1_aeskey1_diagnostics_aes";

	uint32_t DDR_CW1_start = 0x90001000;
	uint32_t DDR_CW1_end   = 0x90003000;
	uint32_t hash_start_1  = 0x90010000;
	uint32_t hash_end_1	   = 0x90013000;

	uint32_t KB4_Aligner   = 0x1000;

	uint32_t reg_index  = 0x00,reg_index1 = 0;
	uint32_t DATA1  = 0xDEADBEEF;
	uint32_t DATA2  = 0x5A5A5A5A;
	uint32_t DATA_CLEAR = 0x00000000;
	uint32_t rdata  = 0x00;

	uint32_t total_cache = 0x2000; //8Kb
	uint32_t prefetch_count = 0x00;
	uint32_t total_pfetch_byte = 0x00;
	uint32_t tcount_to_fill = 0x00;
	uint32_t prefetch_limit = 0x1000; //4Kb


	uint32_t TEST_AREA_START = DDR_CW1_start;
	uint32_t TEST_AREA_END   = DDR_CW1_end + KB4_Aligner;

	//Save TZ default settings
	get_tz_params(SMAU_DDR_CW0, &save_tz_base, &save_tz_end);
	post_log("current tz base : 0x%X , current tz end : 0x%X\n",save_tz_base,save_tz_end);
	save_tz_enable = ((save_tz_base >> SMAU_DDR_TZ_BASE__ENABLE) & 0x01);

	//Clean test area
	post_log("clearing the test area\n");
	for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Clear test area
		wr(reg_index, DATA_CLEAR);

	//for(reg_index = hash_start_1 ; reg_index < hash_end_1 ; reg_index += 4) //Clear test area
		//wr(reg_index, DATA_CLEAR);

	//Configure Window

	//SMAU_configure_aes_ctr_window(SMAU_DDR_CW1, DDR_CW1_start, DDR_CW1_end, AES_192, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE, hash_start_1, hash_end_1, NON_SECURE);
	SMAU_configure_aes_ecb_window(SMAU_DDR_CW0, DDR_CW1_start, DDR_CW1_end, AES_128, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE,  SECURE );

	post_log("executing test entity1\n");
	/*----- test entity1 ------*/
	for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4)//Write Known Data to test area
		wr(reg_index, DATA1);

	prefetch_count = 0x00;
	total_pfetch_byte = ((prefetch_count + 1) * 32);
	tcount_to_fill = total_cache / total_pfetch_byte;
	uint32_t cache_hit = 0x00,cache_miss = 0x00,value;

	post_log("filling each cache line(32byte) using single read and verifying by reading from cache\n");
	post_log("address range : 0x%X to 0x%X\n",TEST_AREA_START,(TEST_AREA_START + total_cache - 1));

	for(reg_index = TEST_AREA_START ; reg_index < (TEST_AREA_START + total_cache) ; reg_index += total_pfetch_byte) //Read Known Data from test area
	{
		disable_log = 1;
		SMAU_configure_aes_ecb_window(SMAU_DDR_CW0, DDR_CW1_start, DDR_CW1_end, AES_128, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE,  SECURE );
		disable_log = 0;
		rdata = rd(reg_index);
		if(rdata != DATA1)
		{
			post_log("error (encryption mode ): data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,DATA1,rdata);
			result = TEST_FAIL;
			break;
		}
		disable_log = 1;
		SMAU_disable_aes_ecb_encryption(SMAU_DDR_CW0, DDR_CW1_start, DDR_CW1_end, AES_128, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE,  SECURE );
		disable_log = 0;
		for(reg_index1 = reg_index ; reg_index1 < (reg_index + total_pfetch_byte ) ; reg_index1 += 4) //Read Known Data from test area
		{
			cache_hit++;
			rdata = rd(reg_index1);

			if(rdata != DATA1)
			{
				post_log("error (non-encryption mode ): data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index1,DATA1,rdata);
				result = TEST_FAIL;
				break;
			}
		    value = smau_rd( SMAU_CACHE_CTRL_STATUS1 );
		    //post_log("Cache hit count = 0x%X\n",cache_hit);
		    //post_log("Register SMAU_CACHE_CONTROL_STATUS1 = 0x%X\n",value);
		    if(cache_hit != (value & 0xFFFF))
		    {
		    	post_log("error : expected cache hit count = 0x%X , actual = 0x%X\n",cache_hit,(value & 0xFFFF));
				result = TEST_FAIL;
		    	break;
		    }
		}
		cache_miss++;
		if(cache_miss != ( (value >> 16) & 0xFFFF))
		{
			post_log("error : expected cache miss count = 0x%X , actual = 0x%X\n",cache_miss,( (value >> 16) & 0xFFFF));
			result = TEST_FAIL;
			break;
		}

		if(reg_index1 != (reg_index + total_pfetch_byte))
		{
			result = TEST_FAIL;
			break;
		}
	}
    value = smau_rd( SMAU_CACHE_CTRL_STATUS1 );
    post_log("Register SMAU_CACHE_CONTROL_STATUS1 = 0x%X\n",value);
    post_log("Cache hit count = 0x%X\n",(value & 0xFFFF));
    post_log("Cache miss count = 0x%X\n",( (value >> 16) & 0xFFFF));
	post_log("last address accessed from cache : 0x%X\n",reg_index1);
	post_log("Expecting SMAU CACHE Full (8Kb)\n");
	post_log("Reading entire cache...\n");

	disable_log = 1;
	SMAU_disable_aes_ecb_encryption(SMAU_DDR_CW0, DDR_CW1_start, DDR_CW1_end, AES_128, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE,  SECURE );
	disable_log = 0;


	for(reg_index = TEST_AREA_START ; reg_index < (TEST_AREA_START + total_cache) ; reg_index += total_pfetch_byte) //Read Known Data from test area
	{
			rdata = rd(reg_index);
			if(rdata != DATA1)
			{
				post_log("error (non-encryption mode ): data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index1,DATA1,rdata);
				result = TEST_FAIL;
				break;
			}
	}
	post_log("Now invalidate each cache line by single write and verify \n");
	for(reg_index = TEST_AREA_START ; reg_index < (TEST_AREA_START + total_cache) ; reg_index += total_pfetch_byte) //Read Known Data from test area
	{
		disable_log = 1;
		SMAU_configure_aes_ecb_window(SMAU_DDR_CW0, DDR_CW1_start, DDR_CW1_end, AES_128, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE,  SECURE );
		disable_log = 0;

		wr(reg_index,DATA1);

		disable_log = 1;
		SMAU_disable_aes_ecb_encryption(SMAU_DDR_CW0, DDR_CW1_start, DDR_CW1_end, AES_128, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE,  SECURE );
		disable_log = 0;

		for(reg_index1 = reg_index ; reg_index1 < (reg_index + total_pfetch_byte ) ; reg_index1 += 4) //Read Known Data from test area
		{
			rdata = rd(reg_index1);
			if(rdata == DATA1)
			{
				post_log("error (non-encryption mode ): data error @  0x%X , data expected != 0x%X\n",reg_index1,rdata);
				result = TEST_FAIL;
				break;
			}
		}
		if(reg_index1 != (reg_index + total_pfetch_byte))
		{
			result = TEST_FAIL;
			break;
		}
	}

	/*-------Restore configs ------*/
	smau_clear_peep_window();
	smau_cache_invalidate();
	smau_set_trustzone_window(SMAU_DDR_CW0, save_tz_base , save_tz_end);
	smau_trustzone_window_control(SMAU_DDR_CW0, save_tz_enable);

	//post_log("\n-- Test %s --\n",result == TEST_PASS ? "PASSED" : "FAILED");
    return result;
}

static int SMAU_cache_operation(void)
{
	int result = TEST_PASS, value = 0x00;
	//Cache config
	//struct DmuBlockEnable dbe;
	//dbe.smau_s0_idm = 1;
	//bcm_dmu_block_disable(dbe);
	//bcm_dmu_block_enable(dbe);

	smau_clear_peep_window();
	disable_all_window();
	smau_prefetch_length(0x0);
	smau_partial_mode(ENABLE);
	smau_cache_enable(ENABLE);
    value = smau_rd( SMAU_CACHE_CONTROL );
    post_log("\nRegister SMAU_CACHE_CONTROL = 0x%X",value);
    value = smau_rd( SMAU_CACHE_CTRL_STATUS1 );
    post_log("\nRegister SMAU_CACHE_CONTROL_STATUS1 = 0x%X\n",value);

    if(smau_cache_invalidate())
		post_log("smau_cache_invalidate failed\n");
	post_log("Enabling Cache...\n");
	result += __SMAU_cache_operation();

	smau_cache_enable(DISABLE);
	post_log("\n-- Test %s --\n",result == TEST_PASS ? "PASSED" : "FAILED");
	return result;
}


#endif


/*==================================================================================================
*Description : The order of priority  PEEP > Cipher-Text Window > Secure Trustzone
*Parameter   :
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
static int __SMAU_multiple_non_overlaping_window_access(void)
{
	int result = TEST_PASS;
	uint32_t save_tz_base    = 0x00;
	uint32_t save_tz_end     = 0x00;
	uint32_t save_tz_enable  = 0x00;



	uint8_t common_key0[]     = "CW1_key0_aeskey0_diagnostics_aes";
	uint8_t common_key1[]     = "CW1_key1_aeskey1_diagnostics_aes";

	uint32_t DDR_CW0_start = 0x90000000;
	uint32_t DDR_CW0_end   = 0x90000000;


	uint32_t DDR_CW1_start = 0x90001000;
	uint32_t DDR_CW1_end   = 0x90002000;
	uint32_t hash_start_1  = 0x90010000;
	uint32_t hash_end_1	   = 0x90011000;

	uint32_t DDR_CW2_start = 0x90003000;
	uint32_t DDR_CW2_end   = 0x90004000;

	uint32_t DDR_CW3_start = 0x90005000;
	uint32_t DDR_CW3_end   = 0x90005000;
	uint32_t hash_start_2  = 0x90012000;
	uint32_t hash_end_2	   = 0x90012000;

	uint32_t DDR_TZ_start  = 0x90006000;
	uint32_t DDR_TZ_end    = 0x9000a000;

	uint32_t PEEP_start    = 0x9000b000;
	uint32_t PEEP_end      = 0x9000f000;

	uint32_t KB4_Aligner   = 0x1000;

	uint32_t reg_index  = 0x00,reg_index1 = 0;
	uint32_t DATA1  = 0xDEADBEEF;
	uint32_t DATA2  = 0x5A5A5A5A;
	uint32_t DATA_CLEAR = 0x00000000;
	uint32_t rdata  = 0x00;


	uint32_t TEST_AREA_START = DDR_CW0_start;
	uint32_t TEST_AREA_END   = PEEP_end + KB4_Aligner;

	//Save TZ default settings
	get_tz_params(SMAU_DDR_CW0, &save_tz_base, &save_tz_end);
	post_log("current tz base : 0x%X , current tz end : 0x%X\n",save_tz_base,save_tz_end);
	save_tz_enable = ((save_tz_base >> SMAU_DDR_TZ_BASE__ENABLE) & 0x01);

	//Clean test area
	post_log("clearing the test area\n");
	for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Clear test area
		wr(reg_index, DATA_CLEAR);

	for(reg_index = hash_start_1 ; reg_index < hash_end_2 ; reg_index += 4) //Clear test area
		wr(reg_index, DATA_CLEAR);

	//Configure Window
	SMAU_configure_aes_ecb_window(SMAU_DDR_CW0, DDR_CW0_start, DDR_CW0_end, AES_128, (uint32_t*)common_key0, (uint32_t*)common_key1, ENABLE,  SECURE );
	SMAU_configure_aes_ctr_window(SMAU_DDR_CW1, DDR_CW1_start, DDR_CW1_end, AES_192, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE, hash_start_1, hash_end_1, NON_SECURE);
	SMAU_configure_aes_ecb_window(SMAU_DDR_CW2, DDR_CW2_start, DDR_CW2_end, AES_256, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE, NON_SECURE );
	SMAU_configure_aes_ctr_window(SMAU_DDR_CW3, DDR_CW3_start, DDR_CW3_end, AES_256, (uint32_t*)common_key0, (uint32_t*)common_key1, ENABLE,  hash_start_2, hash_end_2, SECURE);
	smau_set_trustzone_window(SMAU_DDR_CW0, DDR_TZ_start , DDR_TZ_end);
	smau_trustzone_window_control(SMAU_DDR_CW0, TZ_ENABLE);
	smau_set_peep_window(PEEP_start, PEEP_end);
	smau_cache_invalidate();

	post_log("memory access test start\n");
	post_log("Test case 1\n");
	for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Write Known Data to test area
		wr(reg_index, DATA1);

	for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Read Known Data from test area
	{
		rdata = rd(reg_index);
		if(rdata != DATA1)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,DATA1,rdata);
			result = TEST_FAIL;
			break;
		}
	}
	/*----- One test entity ------*/
	post_log("Test case 2\n");
	for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Write Known Data to test area
		wr(reg_index, DATA1);

	for(reg_index1 = reg_index ; reg_index1 >= TEST_AREA_START ; reg_index1 -= 4) //Write Known Data to test area
		wr(reg_index1, DATA2);

	for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Read Known Data from test area
	{
		rdata = rd(reg_index);
		if(rdata != DATA2)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,DATA1,rdata);
			result = TEST_FAIL;
			break;
		}
	}
	/*----- One test entity ------*/
	post_log("Test case 3\n");

	for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Write Known Data to test area
	{
		rdata = rd(reg_index);
		wr((reg_index ), (rdata + DATA1));
		reg_index1 -= 4;
	}
	for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Read Known Data from test area
	{
		rdata = rd(reg_index);
		if(rdata != (DATA2 + DATA1))
		{
			post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,DATA1,rdata);
			result = TEST_FAIL;
			break;
		}
	}
	/*-------Restore configs ------*/
	smau_clear_peep_window();
	smau_cache_invalidate();
	smau_set_trustzone_window(SMAU_DDR_CW0, save_tz_base , save_tz_end);
	smau_trustzone_window_control(SMAU_DDR_CW0, save_tz_enable);

	//post_log("\n-- Test %s --\n",result == TEST_PASS ? "PASSED" : "FAILED");
    return result;
}

static int SMAU_multiple_non_overlaping_window_access(void)
{
	int result = TEST_PASS;
	//Cache config
	smau_clear_peep_window();
	disable_all_window();
	smau_cache_invalidate();
	smau_prefetch_length(0x04);
	smau_partial_mode(ENABLE);
	smau_cache_enable(ENABLE);
	post_log("Enabling Cache...\n");
	result += __SMAU_multiple_non_overlaping_window_access();

	post_log("Disabling Cache...\n");
	smau_cache_enable(DISABLE);\
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_multiple_non_overlaping_window_access();

	post_log("\n-- Test %s --\n",result == TEST_PASS ? "PASSED" : "FAILED");
	return result;
}
/*------------------------ Test end : SMAU_multiple_non_overlaping_window_access ----------------------------------*/

/*------------------------ Test start : SMAU_peep_window_secure_nonsecure_access ----------------------------------*/
/*==================================================================================================
*Description : The order of priority  PEEP > Cipher-Text Window > Secure Trustzone
*Parameter   :
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
extern void nonsec_switch(void);
extern void sec_switch(void);

static int __SMAU_peep_window_secure_nonsecure_access(void)
{
	int result = TEST_PASS;
	uint32_t save_tz_base    = 0x00;
	uint32_t save_tz_end     = 0x00;
	uint32_t save_tz_enable  = 0x00;

	uint8_t common_key0[]     = "CW1_key0_aeskey0_diagnostics_aes";

	uint32_t PEEP_start      = 0x90000000;
	uint32_t PEEP_end        = 0x90001000;

	uint32_t DDR_TZ_start    = 0x8FFFF000;
	uint32_t DDR_TZ_end      = 0x90003000;

	uint32_t DDR_CW0_start   = 0x8FFFE000;
	uint32_t DDR_CW0_end     = 0x90004000;

	uint32_t KB4_Aligner     = 0x1000;
	volatile uint32_t exp_data_abort_counter = 0x00;

	volatile uint32_t reg_index  = 0x00;
	uint32_t DATA1  = 0xDEADBEEF;
	uint32_t DATA2  = 0x5A5A5A5A;
	//uint32_t DATA_CLEAR = 0x00000000;
	volatile uint32_t rdata  = 0x00;

	uint32_t TEST_AREA_START = DDR_CW0_start;
	uint32_t TEST_AREA_END   = DDR_CW0_end + KB4_Aligner;

    uint32_t apby_sec  = CYGNUS_periph_APBy_ccg_uart4 |
    					 CYGNUS_periph_APBy_ccg_uart3 |
    					 CYGNUS_periph_APBy_ccg_uart2 |
    					 CYGNUS_periph_APBy_ccg_uart1 |
    					 CYGNUS_periph_APBy_ccg_uart0 ;
    tz_set_periph_security(apby_sec, STATE_NON_SECURE);//UART Nonsecure

    //Save TZ default value
	get_tz_params(SMAU_DDR_CW0, &save_tz_base, &save_tz_end);
	post_log("current tz base : 0x%X , current tz end : 0x%X\n",save_tz_base,save_tz_end);
	save_tz_enable = ((save_tz_base >> SMAU_DDR_TZ_BASE__ENABLE) & 0x01);

	//Clear the test area
//	post_log("cleaning the test area\n");
//	for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Clear test area
//		wr(reg_index, DATA_CLEAR);

	//Window configuration
	SMAU_configure_aes_ecb_window(SMAU_DDR_CW0, DDR_CW0_start, DDR_CW0_end, AES_128, (uint32_t*)common_key0, (uint32_t*)common_key0, ENABLE,  NON_SECURE );
	smau_set_trustzone_window(SMAU_DDR_CW1, DDR_TZ_start , DDR_TZ_end);
	smau_trustzone_window_control(SMAU_DDR_CW2, TZ_ENABLE);
	smau_set_peep_window(PEEP_start, PEEP_end);
    smau_cache_invalidate();

    //Write and verify know data1
	for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Write Known Data to test area
		wr(reg_index, DATA1);

	post_log("Try reading from entire Cipher text window\n");
	for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Write Known Data to test area
	{
		rdata = rd(reg_index);
		if((reg_index < PEEP_start) || (reg_index >= (PEEP_end + KB4_Aligner)))
		{
			if(DATA1 != rdata)
			{
				rdata = rd(reg_index);
				post_log("error : Data = 0x%X @ 0x%X\n",rdata,reg_index);
				result = TEST_FAIL;
				break;
			}
		}
		else
		{
			if(DATA1 == rdata)
			{
				rdata = rd(reg_index);
				post_log("error : Data = 0x%X @ 0x%X\n",rdata,reg_index);
				result = TEST_FAIL;
				break;
			}
		}
	}

	//Enter in to Non-Sec world
    post_log("\nExecuting context switch using smc instruction\n");
	post_log("Entering in to NON_SECURE World...\n");
	ns_data_abort_count = 0;
	exp_data_abort_counter = 0;
	nonsec_switch();/*Entering in to non-secure world*/
	post_log("\n************** Non Secure World Entry **************\n");

	//Try writing to secure region TZ area but this area should be Non_secure because
	//range is overlapped by  cipher text window config
	post_log("Try writing to secure region TZ area but this area should be Non_secure because as the address range \n" \
			 "is overlapped by Non-Secure Cipher text window\n\n");
	for(reg_index = DDR_TZ_start ; reg_index < (DDR_TZ_end + KB4_Aligner)  ; reg_index += 4) //Write Known Data to test area
		wr(reg_index, DATA2);

	if(ns_data_abort_count)
	{
		post_log("error : data abort not expected here..\n");
		result = TEST_FAIL;
	}
	for(reg_index = DDR_TZ_start ; reg_index < (DDR_TZ_end + KB4_Aligner) ; reg_index += 4)
	{
		rdata = rd(reg_index);
		if((reg_index >= PEEP_start) && (reg_index < (PEEP_end + KB4_Aligner)))
		{//Secure
			exp_data_abort_counter++;
		}
		else
		{
			if(DATA2 != rdata)
			{
				rdata = rd(reg_index);
				post_log("error : Data = 0x%X @ 0x%X\n",rdata,reg_index);
				result = TEST_FAIL;
				break;
			}
		}
	}
	if(exp_data_abort_counter != ns_data_abort_count)
	{
		post_log("error : expected number of data abort is : 0x%X , actual number of data abort : 0x%X \n\n", exp_data_abort_counter , ns_data_abort_count);
		result = TEST_FAIL;
	}
	ns_data_abort_count = 0;
	exp_data_abort_counter = 0;
	//Try writing to secure region (peep window is always secure) but write should have the security
	//settings of Cipher text Window which is Non Secure here hence write should go through
	post_log("Try writing to peep window region (peep window is always secure) but write should have the security\n" \
			 "settings of Cipher text Window which is Non Secure here hence write should go through\n\n");
	for(reg_index = PEEP_start ; reg_index < (PEEP_end + KB4_Aligner) ; reg_index += 4) //Write Known Data to test area
		wr(reg_index, DATA2);

	if(ns_data_abort_count)
	{
		post_log("error : data abort not expected here..\n");
		result = TEST_FAIL;
	}
	for(reg_index = PEEP_start ; reg_index < (PEEP_end + KB4_Aligner) ; reg_index += 4)
	{
		rdata = rd(reg_index);
		exp_data_abort_counter++;
	}
	if(exp_data_abort_counter != ns_data_abort_count)
	{
		post_log("error : expected number of data abort is : 0x%X , actual number of data abort : 0x%X \n\n", exp_data_abort_counter , ns_data_abort_count);
		result = TEST_FAIL;
	}
	ns_data_abort_count = 0;
	exp_data_abort_counter = 0;
	rdata = 0;
	//Try writing to secure region (peep window is always secure) but write should have the security
	//settings of Cipher text Window which is Non Secure here hence write should go through
	post_log("Try writing to entire Cipher text window\n");
	for(reg_index = DDR_CW0_start ; reg_index < (DDR_CW0_end + KB4_Aligner) ; reg_index += 4) //Write Known Data to test area
		wr(reg_index, DATA2);

	if(ns_data_abort_count)
	{
		post_log("error : data abort not expected here..");
		result = TEST_FAIL;
	}
	for(reg_index = DDR_CW0_start ; reg_index < (DDR_CW0_end + KB4_Aligner) ; reg_index += 4) //Write Known Data to test area
	{
		rdata = rd(reg_index);
		if((reg_index >= PEEP_start) && (reg_index < (PEEP_end + KB4_Aligner)))
		{//Secure
			exp_data_abort_counter++;
		}
		else
		{
			if(DATA2 != rdata)
			{
				rdata = rd(reg_index);
				post_log("error : Data = 0x%X @ 0x%X\n",rdata,reg_index);
				result = TEST_FAIL;
				break;
			}
		}
	}
	if(exp_data_abort_counter != ns_data_abort_count)
	{
		post_log("error : expected number of data abort is : 0x%X , actual number of data abort : 0x%X \n\n", exp_data_abort_counter , ns_data_abort_count);
		result = TEST_FAIL;
	}

	post_log("\nComing back to SECURE World...\n");
    sec_switch();/*Entering in to secure world*/
	post_log("************** Non Secure World Exit **************\n\n");

	post_log("Try reading from entire Cipher text window\n");
	for(reg_index = DDR_CW0_start ; reg_index < (DDR_CW0_end + KB4_Aligner) ; reg_index += 4) //Write Known Data to test area
	{
		rdata = rd(reg_index);
		if((reg_index < PEEP_start) || (reg_index >= (PEEP_end + KB4_Aligner)) )
		{
			if(rdata != DATA2)
			{
				post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,DATA2,rdata);
				result = TEST_FAIL;
				break;
			}
		}
		else
		{
			if(rdata == DATA1)
			{
				post_log("error : data error @  0x%X , data read = 0x%X\n",reg_index,DATA2,rdata);
				result = TEST_FAIL;
				break;
			}
		}
	}
	/*-------Restore configs ------*/
	smau_clear_peep_window();
	smau_cache_invalidate();
	smau_set_trustzone_window(SMAU_DDR_CW0, save_tz_base , save_tz_end);
	smau_trustzone_window_control(SMAU_DDR_CW0, save_tz_enable);

	//post_log("\n-- Test %s --\n",result == TEST_PASS ? "PASSED" : "FAILED");
    return result;
}
static int SMAU_peep_window_secure_nonsecure_access(void)
{
	int result = TEST_PASS;
	//Cache config
	smau_clear_peep_window();
	disable_all_window();
	smau_cache_invalidate();
	smau_prefetch_length(0x04);
	smau_partial_mode(ENABLE);
	smau_cache_enable(ENABLE);
	post_log("Enabling Cache...\n");
	result += __SMAU_peep_window_secure_nonsecure_access();
	post_log("Disabling Cache...\n");
	smau_clear_peep_window();
	disable_all_window();
	smau_cache_enable(DISABLE);
	result += __SMAU_peep_window_secure_nonsecure_access();

	post_log("\n-- Test %s --\n",result == TEST_PASS ? "PASSED" : "FAILED");
	return result;
}
/*------------------------ Test end : SMAU_peep_window_secure_nonsecure_access ----------------------------------*/

/*------------------------ Test start : SMAU_peep_window_secure_nonsecure_access ----------------------------------*/
/*==================================================================================================
*Description : The order of priority  PEEP > Cipher-Text Window > Secure Trustzone
*Parameter   :
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
static int __SMAU_cipher_window_secure_nonsecure_access(void)
{
	uint8_t common_key0[]     = "CW1_key0_aeskey0_diagnostics_aes";
	uint8_t common_key1[]     = "CW1_key1_aeskey1_diagnostics_aes";

	uint32_t DDR_CW0_start = 0x90000000;
	uint32_t DDR_CW0_end   = 0x90005000;

	uint32_t DDR_CW1_start = 0x90003000;
	uint32_t DDR_CW1_end   = 0x90008000;
	uint32_t hash_start_1  = 0xa0000000;//0x90100000;
	uint32_t hash_end_1    = 0xa0005000;//0x90105000;

	uint32_t DDR_CW2_start = 0x90007000;
	uint32_t DDR_CW2_end   = 0x9000c000;

	uint32_t DDR_CW3_start = 0x9000a000;
	uint32_t DDR_CW3_end   = 0x9000f000;
	uint32_t hash_start_2  = 0xa0006000;//0x90106000;
	uint32_t hash_end_2    = 0xa000b000;//0x9010b000;

	uint32_t DDR_TZ_start  = 0x8FFFF000;
	uint32_t DDR_TZ_end    = 0x90010000;

	int result = TEST_PASS;
	uint32_t save_tz_base    = 0x00;
	uint32_t save_tz_end     = 0x00;
	uint32_t save_tz_enable  = 0x00;

	uint32_t KB4_Aligner     = 0xFFF;

	volatile uint32_t exp_data_abort_counter = 0x00;
	volatile uint32_t reg_index  = 0x00;
	volatile uint32_t rdata  = 0x00;

	uint32_t DATA1  = 0xDEADBEEF;
	uint32_t DATA2  = 0x5A5A5A5A;
	uint32_t data1_counter = 0x00;
	uint32_t data2_counter = 0x00;
	uint32_t DATA_CLEAR = 0x00000000;

	uint32_t test_area_counter = 0;
	uint32_t error = 0;

	uint32_t TEST_AREA_START = DDR_TZ_start;
	uint32_t TEST_AREA_END   = (DDR_TZ_end | KB4_Aligner);

    uint32_t apby_sec  = CYGNUS_periph_APBy_ccg_uart4 |
    					 CYGNUS_periph_APBy_ccg_uart3 |
    					 CYGNUS_periph_APBy_ccg_uart2 |
    					 CYGNUS_periph_APBy_ccg_uart1 |
    					 CYGNUS_periph_APBy_ccg_uart0 ;
    tz_set_periph_security(apby_sec, STATE_NON_SECURE);//configure UART as Nonsecure

	get_tz_params(SMAU_DDR_CW0, &save_tz_base, &save_tz_end);
	post_log("current tz base : 0x%X , current tz end : 0x%X\n",save_tz_base,save_tz_end);
	save_tz_enable = ((save_tz_base >> SMAU_DDR_TZ_BASE__ENABLE) & 0x01);

//Clear the test area
	post_log("clearing the test area\n");
	for(reg_index = TEST_AREA_START ; reg_index <= TEST_AREA_END ; reg_index += 4) //Clear test area
		wr(reg_index, DATA_CLEAR);

	for(reg_index = hash_start_1 ; reg_index <= (hash_end_2 | KB4_Aligner) ; reg_index += 4) //Clear test area
		wr(reg_index, DATA_CLEAR);

//Window Configuration
	SMAU_configure_aes_ecb_window(SMAU_DDR_CW0, DDR_CW0_start, DDR_CW0_end, AES_128, (uint32_t*)common_key0, (uint32_t*)common_key1, ENABLE,  SECURE );
	SMAU_configure_aes_ctr_window(SMAU_DDR_CW1, DDR_CW1_start, DDR_CW1_end, AES_192, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE, hash_start_1, hash_end_1, NON_SECURE);
	SMAU_configure_aes_ecb_window(SMAU_DDR_CW2, DDR_CW2_start, DDR_CW2_end, AES_256, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE, SECURE );
	SMAU_configure_aes_ctr_window(SMAU_DDR_CW3, DDR_CW3_start, DDR_CW3_end, AES_256, (uint32_t*)common_key0, (uint32_t*)common_key1, ENABLE,  hash_start_2, hash_end_2, NON_SECURE);

	smau_set_trustzone_window(SMAU_DDR_CW0, DDR_TZ_start , DDR_TZ_end);
	smau_trustzone_window_control(SMAU_DDR_CW0, TZ_ENABLE);
	smau_clear_peep_window();
	smau_cache_invalidate();

//Write Know data to test area- read and check
    post_log("Expected Memory security\n0x%X to 0x%X - Secure\n0x%X to 0x%X - Non-secure\n0x%X to 0x%X - Secure\n0x%X to 0x%X - Non-secure\n0x%X to 0x%X - Secure\n",DDR_TZ_start,(DDR_CW1_start - 1),DDR_CW1_start,(DDR_CW2_start - 1),DDR_CW2_start,(DDR_CW3_start - 1),DDR_CW3_start,(DDR_CW3_end | KB4_Aligner),((DDR_CW3_end | KB4_Aligner) + 1),(DDR_TZ_end | KB4_Aligner));
	for(reg_index = TEST_AREA_START ; reg_index <= TEST_AREA_END ; reg_index += 4) //Write Known Data to test area
		wr(reg_index, DATA1);

	post_log("Try reading from entire Cipher text window\n");
	for(reg_index = TEST_AREA_START ; reg_index <= TEST_AREA_END ; reg_index += 4) //Write Known Data to test area
	{
		rdata = rd(reg_index);
		if(DATA1 != rdata)
		{
			rdata = rd(reg_index);
			post_log("error : Data = 0x%X @ 0x%X\n",rdata,reg_index);
			result = TEST_FAIL;
			break;
		}
	}

//Non-Secure world switch
	post_log("\nExecuting context switch using smc instruction\n");
	post_log("Entering in to NON_SECURE World...\n");
	nonsec_switch();/*Entering in to non-secure world*/
	post_log("\n************** Non Secure World Entry **************\n");

//Try writing different data to test area
    ns_data_abort_count    = 0x00;
	exp_data_abort_counter = 0x00;
	rdata 				   = 0x00;
	post_log("Try memory access to entire Cipher text window from non-secure cpu\n");
	for(reg_index = TEST_AREA_START ; reg_index <= TEST_AREA_END ; reg_index += 4) //Write Known Data to test area
	{
		wr(reg_index,DATA2);

		if( ((reg_index >= DDR_CW1_start) && (reg_index < DDR_CW2_start) ));//Non-Secure

		else if( (reg_index >= DDR_CW3_start) && (reg_index <= (DDR_CW3_end | KB4_Aligner) ));//Non-Secure

		else//Secure
			exp_data_abort_counter += 1;//Data abort count

		if(!(reg_index % 0x1000)){
		post_log(".");
		//post_log("address : 0x%X\n",reg_index);
		}
	}
	post_log("\n\n");
	if(exp_data_abort_counter != ns_data_abort_count)
	{
		post_log("error : expected number of data abort is : 0x%X , actual number of data abort : 0x%X \n\n", exp_data_abort_counter , ns_data_abort_count);
		result = TEST_FAIL;
	}
	else
		post_log("info : expected number of data abort is : 0x%X , actual number of data abort : 0x%X \n\n", exp_data_abort_counter , ns_data_abort_count);

//read data from test area
	ns_data_abort_count    = 0x00;
	exp_data_abort_counter = 0x00;
	rdata 				   = 0x00;
	for(reg_index = TEST_AREA_START ; reg_index <= TEST_AREA_END ; reg_index += 4) //Write Known Data to test area
	{
		rdata = rd(reg_index);
		if( ((reg_index >= DDR_CW1_start) && (reg_index < DDR_CW2_start) ))
		{//Non-Secure
			if( rdata != DATA2 )
			{
				post_log("error : Data = 0x%X @ 0x%X not expected\n",rdata,reg_index);
				result = TEST_FAIL;
				break;
			}
		}
		else if( (reg_index >= DDR_CW3_start) && (reg_index <= (DDR_CW3_end | KB4_Aligner) ))
		{//Non-Secure
			if( rdata != DATA2 )
			{
				post_log("error : Data = 0x%X @ 0x%X not expected\n",rdata,reg_index);
				result = TEST_FAIL;
				break;
			}
		}
		else
		{//Secure
			exp_data_abort_counter += 1;
			if( (rdata == DATA2) || (rdata == DATA1) )
			{
				post_log("error : Data = 0x%X @ 0x%X not expected\n",rdata,reg_index);
				result = TEST_FAIL;
				break;
			}
		}

		if(!(reg_index % 0x500)){
		post_log(".");
		//post_log("address : 0x%X\n",reg_index);
		}
	}

	post_log("\n\n");
	if(exp_data_abort_counter != ns_data_abort_count)
	{
		post_log("error : expected number of data abort is : 0x%X , actual number of data abort : 0x%X \n\n", exp_data_abort_counter , ns_data_abort_count);
		result = TEST_FAIL;
	}
//Exit from Non-Secure world
	post_log("\nComing back to SECURE World...\n");
    sec_switch();/*Entering in to secure world*/
	post_log("************** Non Secure World Exit **************\n\n");
	post_log("Try memory access to entire Cipher text window from secure cpu\n");
//read data from test area
	for(reg_index = TEST_AREA_START ; reg_index <= TEST_AREA_END ; reg_index += 4)
	{
		rdata = 0x00;
		rdata = rd(reg_index);
		test_area_counter++;
		if( ((reg_index >= DDR_CW1_start) && (reg_index < DDR_CW2_start) ))
		{//Non-Secure
			if( rdata != DATA2 )
			{
				post_log("error : Data = 0x%X @ 0x%X not expected\n",rdata,reg_index);
				result = TEST_FAIL;
				error++;
				break;
			}
			else
				data2_counter++;
		}
		else if( (reg_index >= DDR_CW3_start) && (reg_index <= (DDR_CW3_end | KB4_Aligner) ))
		{//Non-Secure
			if( rdata != DATA2 )
			{
				post_log("error : Data = 0x%X @ 0x%X not expected\n",rdata,reg_index);
				result = TEST_FAIL;
				error++;
				break;
			}
			else
				data2_counter++;
		}
		else//Secure
		{
			if( rdata != DATA1 )
			{
				post_log("error : Data = 0x%X @ 0x%X not expected\n",rdata,reg_index);
				result = TEST_FAIL;
				error++;
				break;
			}
			else
				data1_counter++;
		}

		if(!(reg_index % 0x500)){
		post_log(".");
		//post_log("address : 0x%X\n",reg_index);
		}
	}
// Calculate count
	uint32_t calc_data1 = ( (DDR_CW1_start - DDR_TZ_start) + (DDR_CW3_start - DDR_CW2_start) + ( ((DDR_TZ_end | KB4_Aligner) +1)  - ( (DDR_CW3_end | KB4_Aligner) + 1)));
	uint32_t calc_data2 = ((DDR_CW2_start - DDR_CW1_start) + ( ((DDR_CW3_end | KB4_Aligner) + 1) - DDR_CW3_start));
	post_log("\ntotal  - count = 0x%X\n",test_area_counter);
	post_log("error  - count = 0x%X\n",error);
	post_log("data 1 - count = 0x%X\n",data1_counter);
	post_log("data 2 - count = 0x%X\n",data2_counter);
	post_log("Calculated data 1 - count = 0x%X\nCalculated data 2 - count = 0x%X\n",(calc_data1/4),(calc_data2/4));

	if(test_area_counter != (data1_counter + data2_counter)){
		post_log("error : total test area = 0x%X should be equal to DATA1 count + DATA2 count = 0x%X\n",test_area_counter,(data1_counter + data2_counter));
		result = TEST_FAIL;
	}
	if(error){
		post_log("error : expected error data count = 0 but actual error count = 0x%X\n",error);
		result = TEST_FAIL;
	}
	if(data1_counter != (calc_data1/4)){
		post_log("error : expected data1 count = 0x%X but actual count = 0x%X\n",(calc_data1/4),data1_counter);
		result = TEST_FAIL;
	}
	if(data2_counter != (calc_data2/4)){
		post_log("error : expected data2 count = 0x%X but actual count = 0x%X\n",(calc_data2/4),data2_counter);
		result = TEST_FAIL;
	}

	/*-------Restore configs ------*/
	smau_clear_peep_window();
	smau_cache_invalidate();
	smau_set_trustzone_window(SMAU_DDR_CW0, save_tz_base , save_tz_end);
	smau_trustzone_window_control(SMAU_DDR_CW0, save_tz_enable);

	//post_log("\n-- Test %s --\n",result == TEST_PASS ? "PASSED" : "FAILED");
    return result;
}

static int SMAU_cipher_window_secure_nonsecure_access(void)
{
	int result = TEST_PASS;
	//Cache config
	smau_clear_peep_window();
	disable_all_window();
	smau_cache_invalidate();
	smau_prefetch_length(0x04);
	smau_partial_mode(ENABLE);
	smau_cache_enable(ENABLE);
	post_log("Enabling Cache...\n");
	result += __SMAU_cipher_window_secure_nonsecure_access();
	post_log("Disabling Cache...\n");
	smau_clear_peep_window();
	disable_all_window();
	smau_cache_enable(DISABLE);
	result += __SMAU_cipher_window_secure_nonsecure_access();

	post_log("\n-- Test %s --\n",result == TEST_PASS ? "PASSED" : "FAILED");
	return result;
}
/*------------------------ Test end : SMAU_peep_window_secure_nonsecure_access ----------------------------------*/


/*------------------------ Test start : SMAU_multiple_overlaping_window_access ----------------------------------*/
/*==================================================================================================
*Description : The order of priority  PEEP > Cipher-Text Window > Secure Trustzone
*Parameter   :
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
static int __SMAU_multiple_overlaping_window_access(void)
{
	int result = TEST_PASS;
	uint32_t save_tz_base    = 0x00;
	uint32_t save_tz_end     = 0x00;
	uint32_t save_tz_enable  = 0x00;
	uint32_t max_count = 0;

	uint8_t common_key0[]     = "CW1_key0_aeskey0_diagnostics_aes";
	uint8_t common_key1[]     = "CW1_key1_aeskey1_diagnostics_aes";

	uint32_t DDR_CW0_start = 0x90000000;
	uint32_t DDR_CW0_end   = 0x90005000;

	uint32_t DDR_CW1_start = 0x90003000;
	uint32_t DDR_CW1_end   = 0x90008000;
	uint32_t hash_start_1  = 0xa0000000;
	uint32_t hash_end_1	   = 0xa0005000;

	uint32_t DDR_CW2_start = 0x90007000;
	uint32_t DDR_CW2_end   = 0x9000c000;

	uint32_t DDR_CW3_start = 0x9000a000;
	uint32_t DDR_CW3_end   = 0x9000f000;
	uint32_t hash_start_2  = 0xa0006000;
	uint32_t hash_end_2	   = 0xa000b000;

	uint32_t DDR_TZ_start  = 0x8FFFF000;
	uint32_t DDR_TZ_end    = 0x90010000;

	uint32_t PEEP_start    = DDR_CW1_start;
	uint32_t PEEP_end      = 0x90002000;

	uint32_t KB4_Aligner   = 0x1000;

	uint32_t reg_index  = 0x00, loop = 0x00;
	uint32_t DATA1  = 0xDEADBEEF;
	uint32_t DATA2  = 0x5A5A5A5A;
	uint32_t DATA3  = 0xA5A5A5A5;
	uint32_t DATA_CLEAR = 0x00000000;
	uint32_t rdata  = 0x00;


	uint32_t TEST_AREA_START = DDR_TZ_start;
	uint32_t TEST_AREA_END   = DDR_TZ_end + KB4_Aligner;

	uint32_t AES_CW0_DATA0_K0[5];
	uint32_t AES_CW0_DATA0_K1[5];
	uint32_t AES_CW1_DATA0_K0[5];
	uint32_t AES_CW1_DATA0_K1[5];

	uint32_t AES_CW2_DATA0_K0[5];
	uint32_t AES_CW2_DATA0_K1[5];
	uint32_t AES_CW3_DATA0_K0[5];
	uint32_t AES_CW3_DATA0_K1[5];

	//Save TZ default value
	get_tz_params(SMAU_DDR_CW0, &save_tz_base, &save_tz_end);
	post_log("current tz base : 0x%X , current tz end : 0x%X\n",save_tz_base,save_tz_end);
	save_tz_enable = ((save_tz_base >> SMAU_DDR_TZ_BASE__ENABLE) & 0x01);

	//Clean the test area
	post_log("clearing the test area\n");
	for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Clear test area
		wr(reg_index, DATA_CLEAR);

	for(reg_index = hash_start_1 ; reg_index < hash_end_2 ; reg_index += 4) //Clear test area
		wr(reg_index, DATA_CLEAR);


	SMAU_configure_aes_ecb_window(SMAU_DDR_CW0, DDR_CW0_start, DDR_CW0_end, AES_128, (uint32_t*)common_key0, (uint32_t*)common_key1, ENABLE,  SECURE );
	SMAU_configure_aes_ecb_window(SMAU_DDR_CW2, DDR_CW2_start, DDR_CW2_end, AES_256, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE, NON_SECURE );

	SMAU_configure_aes_ctr_window(SMAU_DDR_CW1, DDR_CW1_start, DDR_CW1_end, AES_192, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE, hash_start_1, hash_end_1, NON_SECURE);
	SMAU_configure_aes_ctr_window(SMAU_DDR_CW3, DDR_CW3_start, DDR_CW3_end, AES_256, (uint32_t*)common_key0, (uint32_t*)common_key1, ENABLE,  hash_start_2, hash_end_2, SECURE);

	smau_set_trustzone_window(SMAU_DDR_CW0, DDR_TZ_start , DDR_TZ_end);
	smau_trustzone_window_control(SMAU_DDR_CW0, TZ_ENABLE);

	smau_set_peep_window(PEEP_start, PEEP_end);
	smau_cache_invalidate();

	/*----- One test entity ------*/
	for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Write Known Data to test area
		wr(reg_index, DATA1);

    //Collect Window Data
	reg_index =  DDR_CW0_start;
	smau_set_peep_window(reg_index, reg_index);
	AES_CW0_DATA0_K0[0] = rd(reg_index);
	AES_CW0_DATA0_K0[1] = rd(reg_index + 4);
	AES_CW0_DATA0_K0[2] = rd(reg_index + 8);
	AES_CW0_DATA0_K0[3] = rd(reg_index + 12);

	reg_index += 16;
	AES_CW0_DATA0_K1[0] = rd(reg_index);
	AES_CW0_DATA0_K1[1] = rd(reg_index + 4);
	AES_CW0_DATA0_K1[2] = rd(reg_index + 8);
	AES_CW0_DATA0_K1[3] = rd(reg_index + 12);

	if(!(memcmp(AES_CW0_DATA0_K0,AES_CW0_DATA0_K1,16))){
		post_log("error : data error @  0x%X , W0 encrypted data with k0 key == W0 encrypted data with k1 key\n",DDR_CW0_start);
		for(loop = 0 ; loop < 4 ; loop++)
			post_log("AES_CW0_DATA0_K0[%d] = 0x%X , AES_CW0_DATA0_K1[%d] = 0x%X \n",loop,AES_CW0_DATA0_K0[loop],loop,AES_CW0_DATA0_K1[loop]);

		result = TEST_FAIL;
	}

	reg_index =  DDR_CW1_start;
	smau_set_peep_window(reg_index, reg_index);
	AES_CW1_DATA0_K0[0] = rd(reg_index);
	AES_CW1_DATA0_K0[1] = rd(reg_index + 4);
	AES_CW1_DATA0_K0[2] = rd(reg_index + 8);
	AES_CW1_DATA0_K0[3] = rd(reg_index + 12);

	reg_index += 16;
	AES_CW1_DATA0_K1[0] = rd(reg_index);
	AES_CW1_DATA0_K1[1] = rd(reg_index + 4);
	AES_CW1_DATA0_K1[2] = rd(reg_index + 8);
	AES_CW1_DATA0_K1[3] = rd(reg_index + 12);

	if(!(memcmp(AES_CW1_DATA0_K0,AES_CW0_DATA0_K0,16))){
		post_log("error : window overlap error @  0x%X , as per the window priority rule data in this location should be W1 encryption\n",DDR_CW1_start);
		for(loop = 0 ; loop < 4 ; loop++)
		{
			post_log("AES_CW1_DATA0_K0[%d] = 0x%X , AES_CW0_DATA0_K0[%d] = 0x%X \n",loop,AES_CW1_DATA0_K0[loop],loop,AES_CW0_DATA0_K0[loop]);
			result = TEST_FAIL;
			break;
		}
	}

	if(!(memcmp(AES_CW1_DATA0_K0,AES_CW1_DATA0_K1,16))){
		post_log("error : data error @  0x%X , W1 encrypted data with k0 key == W1 encrypted data with k1 key\n",DDR_CW1_start);
		for(loop = 0 ; loop < 4 ; loop++)
		{
			post_log("AES_CW1_DATA0_K0[%d] = 0x%X , AES_CW1_DATA0_K1[%d] = 0x%X \n",loop,AES_CW1_DATA0_K0[loop],loop,AES_CW1_DATA0_K1[loop]);
			result = TEST_FAIL;
			break;
		}
	}

	reg_index =  DDR_CW2_start;
	smau_set_peep_window(reg_index, reg_index);
	AES_CW2_DATA0_K0[0] = rd(reg_index);
	AES_CW2_DATA0_K0[1] = rd(reg_index + 4);
	AES_CW2_DATA0_K0[2] = rd(reg_index + 8);
	AES_CW2_DATA0_K0[3] = rd(reg_index + 12);

	reg_index += 16;
	AES_CW2_DATA0_K1[0] = rd(reg_index);
	AES_CW2_DATA0_K1[1] = rd(reg_index + 4);
	AES_CW2_DATA0_K1[2] = rd(reg_index + 8);
	AES_CW2_DATA0_K1[3] = rd(reg_index + 12);

	if(!(memcmp(AES_CW2_DATA0_K0,AES_CW1_DATA0_K0,16))){
		post_log("error : window overlap error @  0x%X , as per the window priority rule data in this location should be W2 encryption\n",DDR_CW2_start);
		for(loop = 0 ; loop < 4 ; loop++)
		{
			post_log("AES_CW2_DATA0_K0[%d] = 0x%X , AES_CW1_DATA0_K0[%d] = 0x%X \n",loop,AES_CW2_DATA0_K0[loop],loop,AES_CW1_DATA0_K0[loop]);
			result = TEST_FAIL;
			break;
		}
	}

	if(!(memcmp(AES_CW2_DATA0_K0,AES_CW2_DATA0_K1,16))){
		post_log("error : data error @  0x%X , W2 encrypted data with k0 key == W2 encrypted data with k1 key\n",DDR_CW2_start);
		for(loop = 0 ; loop < 4 ; loop++)
		{
			post_log("AES_CW2_DATA0_K0[%d] = 0x%X , AES_CW2_DATA0_K1[%d] = 0x%X \n",loop,AES_CW2_DATA0_K0[loop],loop,AES_CW2_DATA0_K1[loop]);
			result = TEST_FAIL;
			break;
		}
	}

	reg_index =  DDR_CW3_start;
	smau_set_peep_window(reg_index, reg_index);
	AES_CW3_DATA0_K0[0] = rd(reg_index);
	AES_CW3_DATA0_K0[1] = rd(reg_index + 4);
	AES_CW3_DATA0_K0[2] = rd(reg_index + 8);
	AES_CW3_DATA0_K0[3] = rd(reg_index + 12);

	reg_index += 16;
	AES_CW3_DATA0_K1[0] = rd(reg_index);
	AES_CW3_DATA0_K1[1] = rd(reg_index + 4);
	AES_CW3_DATA0_K1[2] = rd(reg_index + 8);
	AES_CW3_DATA0_K1[3] = rd(reg_index + 12);

	if(!(memcmp(AES_CW3_DATA0_K0,AES_CW2_DATA0_K0,16))){
		post_log("error : window overlap error @  0x%X , as per the window priority rule data in this location should be W3 encryption\n",DDR_CW3_start);
		for(loop = 0 ; loop < 4 ; loop++)
		{
			post_log("AES_CW3_DATA0_K0[%d] = 0x%X , AES_CW2_DATA0_K0[%d] = 0x%X \n",loop,AES_CW3_DATA0_K0[loop],loop,AES_CW2_DATA0_K0[loop]);
			result = TEST_FAIL;
			break;
		}
	}

	if(!(memcmp(AES_CW3_DATA0_K0,AES_CW3_DATA0_K1,16))){
		post_log("error : data error @  0x%X , W3 encrypted data with k0 key == W3 encrypted data with k1 key\n",DDR_CW3_start);
		for(loop = 0 ; loop < 4 ; loop++)
		{
			post_log("AES_CW3_DATA0_K0[%d] = 0x%X , AES_CW3_DATA0_K1[%d] = 0x%X \n",loop,AES_CW3_DATA0_K0[loop],loop,AES_CW3_DATA0_K1[loop]);
			result = TEST_FAIL;
			break;
		}
	}

	post_log("test case 1\n");
	smau_set_peep_window(PEEP_start, PEEP_end);
	for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Read Known Data from test area
	{
		rdata = rd(reg_index);
		if( (reg_index >= PEEP_start) && (reg_index < (KB4_Aligner + PEEP_end)) )
		{
			if(rdata == DATA1)
			{
				post_log("error : data error @  0x%X , data expected != 0x%X \n",reg_index,DATA1);
				result = TEST_FAIL;
				break;
			}
		}
		else
		{
			if(rdata != DATA1)
			{
				post_log("error : data error @  0x%X , data expected != 0x%X \n",reg_index,DATA1,rdata);
				result = TEST_FAIL;
				break;
			}
		}

	}
	post_log("test case 2\n");
	/*----- One test entity ------*/
	for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Write Known Data to test area
		wr(reg_index, DATA2);

	for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Read Known Data from test area
	{
		rdata = rd(reg_index);
		if( (reg_index >= PEEP_start) && (reg_index < (KB4_Aligner + PEEP_end)) )
		{
			if(rdata == DATA2)
			{
				post_log("error : data error @  0x%X , data expected != 0x%X \n",reg_index,rdata);
				result = TEST_FAIL;
				break;
			}
		}
		else
		{
			if(rdata != DATA2)
			{
				post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,DATA2,rdata);
				result = TEST_FAIL;
				break;
			}
		}

	}
	post_log("test case 3\n");	/*----- One test entity ------*/
	for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Read Known Data from test area
	{
		max_count = 10;
		while(max_count--)
		{
			wr(reg_index,DATA3);
			rdata = rd(reg_index);
		}
		if( (reg_index >= PEEP_start) && (reg_index < (KB4_Aligner + PEEP_end)) )
		{
			if(rdata == DATA3)
			{
				post_log("error : data error @  0x%X , data expected != 0x%X\n",reg_index,rdata);
				result = TEST_FAIL;
				break;
			}
		}
		else
		{
			if(rdata != DATA3)
			{
				post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,DATA3,rdata);
				result = TEST_FAIL;
				break;
			}
		}

	}
	post_log("test case 4\n");
	smau_clear_peep_window();
	for(reg_index = PEEP_start ; reg_index < (KB4_Aligner + PEEP_end) ; reg_index += 4) //Read Known Data from test area
	{
		rdata    = rd(reg_index);
		if(rdata != DATA2)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,DATA_CLEAR,rdata);
			result = TEST_FAIL;
			break;
		}
	}

	/*-------Restore configs ------*/
	smau_clear_peep_window();
	smau_cache_invalidate();
	smau_set_trustzone_window(SMAU_DDR_CW0, save_tz_base , save_tz_end);
	smau_trustzone_window_control(SMAU_DDR_CW0, save_tz_enable);

	return result;
}

static int SMAU_multiple_overlaping_window_access(void)
{
	int result = TEST_PASS;
	//Cache config
	smau_clear_peep_window();
	disable_all_window();
	smau_cache_invalidate();
	smau_prefetch_length(0x04);
	smau_partial_mode(ENABLE);
	smau_cache_enable(ENABLE);
	post_log("Enabling Cache...\n");
	result += __SMAU_multiple_overlaping_window_access();

	post_log("Disabling Cache...\n");
	smau_cache_enable(DISABLE);
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_multiple_overlaping_window_access();

	post_log("\n-- Test %s --\n",result == TEST_PASS ? "PASSED" : "FAILED");
	return result;
}
/*------------------------ Test end : SMAU_multiple_overlaping_window_access ----------------------------------*/
void disable_all_window(void)
{
	SMAU_disable_window(SMAU_DDR_CW0);
	SMAU_disable_window(SMAU_DDR_CW1);
	SMAU_disable_window(SMAU_DDR_CW2);
	SMAU_disable_window(SMAU_DDR_CW3);
	SMAU_disable_window(SMAU_FLASH_CW0);
	SMAU_disable_window(SMAU_FLASH_CW1);
	SMAU_disable_window(SMAU_FLASH_CW2);
	SMAU_disable_window(SMAU_FLASH_CW3);
}
#if 1
/*------------------------ Test start : SMAU_multiple_overlaping_window_access ----------------------------------*/
/*==================================================================================================
*Description : The order of priority  PEEP > Cipher-Text Window > Secure Trustzone
*Parameter   :
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
static int __SMAU_stress_cpu_path_1(void)
{
	int result = TEST_PASS;
	uint32_t save_tz_base    = 0x00;
	uint32_t save_tz_end     = 0x00;
	uint32_t save_tz_enable  = 0x00;
	

	uint8_t common_key0[]     = "CW1_key0_aeskey0_diagnostics_aes";
	uint8_t common_key1[]     = "CW1_key1_aeskey1_diagnostics_aes";

	uint32_t DDR_CW0_start = 0x8FFF9000;
	uint32_t DDR_CW0_end   = 0x9000A000;

	uint32_t DDR_CW1_start = 0x8FFFB000;
	uint32_t DDR_CW1_end   = 0x90008000;
	uint32_t hash_start_1  = 0xA0000000;
	uint32_t hash_end_1	   = 0xA000E000;

	uint32_t DDR_CW2_start = 0x8FFFD000;
	uint32_t DDR_CW2_end   = 0x90006000;

	uint32_t DDR_CW3_start = 0x8FFFE000;
	uint32_t DDR_CW3_end   = 0x90004000;
	uint32_t hash_start_2  = 0xA000F000;
	uint32_t hash_end_2	   = 0xA0014000;

	uint32_t DDR_TZ_start  = 0x8FFF7000;
	uint32_t DDR_TZ_end    = 0x9000C000;

	uint32_t PEEP_start    = 0x90000000;
	uint32_t PEEP_end      = 0x90002000;

	uint32_t KB4_Aligner   = 0x1000;

	uint32_t reg_index  = 0x00,reg_index1 = 0x00;
	uint32_t DATA1  	= 0xDEADBEEF;
	uint32_t DATA2  	= 0x5A5A5A5A;
	//uint32_t DATA_CLEAR = 0x00000000;
	uint32_t rdata  	= 0x00 , rdata1 = 0x00;


	uint32_t TEST_AREA_START = DDR_TZ_start;
	uint32_t TEST_AREA_END   = DDR_TZ_end + KB4_Aligner;


	//Save TZ default value
	get_tz_params(SMAU_DDR_CW0, &save_tz_base, &save_tz_end);
	post_log("current tz base : 0x%X , current tz end : 0x%X\n",save_tz_base,save_tz_end);
	save_tz_enable = ((save_tz_base >> SMAU_DDR_TZ_BASE__ENABLE) & 0x01);

//	disable_all_window();

	//Clean the test area
//	post_log("clearing the test area\n");
//	for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Clear test area
//		wr(reg_index, DATA_CLEAR);
//
//	for(reg_index = hash_start_1 ; reg_index < hash_end_2 ; reg_index += 4) //Clear test area
//		wr(reg_index, DATA_CLEAR);

	SMAU_configure_aes_ecb_window(SMAU_DDR_CW0, DDR_CW0_start, DDR_CW0_end, AES_128, (uint32_t*)common_key0, (uint32_t*)common_key1, ENABLE,  SECURE );
	SMAU_configure_aes_ecb_window(SMAU_DDR_CW2, DDR_CW2_start, DDR_CW2_end, AES_256, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE, NON_SECURE );

	SMAU_configure_aes_ctr_window(SMAU_DDR_CW1, DDR_CW1_start, DDR_CW1_end, AES_192, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE, hash_start_1, hash_end_1, NON_SECURE);
	SMAU_configure_aes_ctr_window(SMAU_DDR_CW3, DDR_CW3_start, DDR_CW3_end, AES_256, (uint32_t*)common_key0, (uint32_t*)common_key1, ENABLE,  hash_start_2, hash_end_2, SECURE);

	smau_set_trustzone_window(SMAU_DDR_CW0, DDR_TZ_start , DDR_TZ_end);
	smau_trustzone_window_control(SMAU_DDR_CW0, TZ_ENABLE);

	smau_set_peep_window(PEEP_start, PEEP_end);
	smau_cache_invalidate();

		post_log("test case 1\n");
		post_log("Write Known DATA, Read Known DATA,start-end 0x%X to 0x%X\n",TEST_AREA_START, (TEST_AREA_END - 1));
		for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Write Known Data to test area
			wr(reg_index, DATA1);

		for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Read Known Data from test area
		{
			rdata = rd(reg_index);
			if((reg_index < PEEP_start) || (reg_index >= (PEEP_end + KB4_Aligner)))
			{
				if(rdata != DATA1)
				{
					post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,DATA1,rdata);
					result = TEST_FAIL;
					break;
				}
			}
			else
			{
				if(rdata == DATA1)
				{
					post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,DATA1,rdata);
					result = TEST_FAIL;
					break;
				}
			}
		}
		post_log("test case 2\n");
		post_log("Read Known DATA from both end, 0x%X to 0x%X\n",TEST_AREA_START, (TEST_AREA_END - 1));
		for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Read Known Data from test area
		{
			rdata  = rd(reg_index);
			reg_index1 = ((TEST_AREA_END - 4) - (reg_index - TEST_AREA_START));
			rdata1 = rd(reg_index1);
			if((reg_index < PEEP_start) || (reg_index >= (PEEP_end + KB4_Aligner)))
			{
				if(rdata != DATA1)
				{
					post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,DATA1,rdata);
					result = TEST_FAIL;
					break;
				}

			}
			else
			{
				if(rdata == DATA1)
				{
					post_log("error : data error @  0x%X , data expected != 0x%X\n",reg_index,DATA1);
					result = TEST_FAIL;
					break;
				}
			}

			if((reg_index1 < PEEP_start) || reg_index1 >= (PEEP_end + KB4_Aligner))
			{
				if(rdata1 != DATA1)
				{
					post_log("error (reverse): data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index1,DATA1,rdata1);
					result = TEST_FAIL;
					break;
				}
			}
			else
			{
				if(rdata1 == DATA1)
				{
					post_log("error (reverse): data error @  0x%X , data expected != 0x%X\n",reg_index1,DATA1);
					result = TEST_FAIL;
					break;
				}
			}
		}
		post_log("test case 3\n");
		post_log("Write-Read Known DATA start-end-start, 0x%X to 0x%X\n",TEST_AREA_START, (TEST_AREA_END - 1));
		for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Read Known Data from test area
		{
			wr(reg_index, DATA2);
			rdata = rd(reg_index);
			if((reg_index < PEEP_start) || (reg_index >= (PEEP_end + KB4_Aligner)))
			{
				if(rdata != DATA2)
				{
					post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,DATA2,rdata);
					result = TEST_FAIL;
					break;
				}
			}
			else
			{
				if(rdata == DATA2)
				{
					post_log("error : data error @  0x%X , data expected != 0x%X\n",reg_index,DATA2);
					result = TEST_FAIL;
					break;
				}
			}

		}

		for(reg_index = (TEST_AREA_END - 4) ; reg_index >= TEST_AREA_START ; reg_index -= 4) //Read Known Data from test area
		{
			wr(reg_index, DATA2);
			rdata = rd(reg_index);
			if((reg_index < PEEP_start) || (reg_index >= (PEEP_end + KB4_Aligner)))
			{
				if(rdata != DATA2)
				{
					post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,DATA2,rdata);
					result = TEST_FAIL;
					break;
				}
			}
			else
			{
				if(rdata == DATA2)
				{
					post_log("error : data error @  0x%X , data expected != 0x%X\n",reg_index,DATA2);
					result = TEST_FAIL;
					break;
				}
			}

		}
		post_log("test case 4\n");
		post_log("Write Known DATA start-end-start, 0x%X to 0x%X\n",TEST_AREA_START, (TEST_AREA_END - 1));
		post_log("Read Known DATA start-end-start, 0x%X to 0x%X\n",TEST_AREA_START, (TEST_AREA_END - 1));
		for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Read Known Data from test area
			wr(reg_index, DATA1);

		for(reg_index = (TEST_AREA_END - 4) ; reg_index >= TEST_AREA_START ; reg_index -= 4) //Read Known Data from test area
			wr(reg_index, DATA1);

		for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Read Known Data from test area
		{
			rdata = rd(reg_index);
			if((reg_index < PEEP_start) || (reg_index >= (PEEP_end + KB4_Aligner)))
			{
				if(rdata != DATA1)
				{
					post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,DATA1,rdata);
					result = TEST_FAIL;
					break;
				}
			}
			else
			{
				if(rdata == DATA1)
				{
					post_log("error : data error @  0x%X , data expected != 0x%X\n",reg_index,DATA1);
					result = TEST_FAIL;
					break;
				}
			}

		}
		for(reg_index = (TEST_AREA_END - 4) ; reg_index >= TEST_AREA_START ; reg_index -= 4) //Read Known Data from test area
		{
			rdata = rd(reg_index);
			if((reg_index < PEEP_start) || (reg_index >= (PEEP_end + KB4_Aligner)))
			{
				if(rdata != DATA1)
				{
					post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,DATA1,rdata);
					result = TEST_FAIL;
					break;
				}
			}
			else
			{
				if(rdata == DATA1)
				{
					post_log("error : data error @  0x%X , data expected != 0x%X\n",reg_index,DATA1);
					result = TEST_FAIL;
					break;
				}
			}
		}
	/*-------Restore configs ------*/
	smau_clear_peep_window();
	smau_cache_invalidate();
	smau_set_trustzone_window(SMAU_DDR_CW0, save_tz_base , save_tz_end);
	smau_trustzone_window_control(SMAU_DDR_CW0, save_tz_enable);

	return result;
}

static int __SMAU_stress_cpu_path_2(void)
{
	int result = TEST_PASS;
	uint32_t save_tz_base    = 0x00;
	uint32_t save_tz_end     = 0x00;
	uint32_t save_tz_enable  = 0x00;


	uint8_t common_key0[]     = "CW1_key0_aeskey0_diagnostics_aes";
	uint8_t common_key1[]     = "CW1_key1_aeskey1_diagnostics_aes";

	uint32_t DDR_CW0_start = 0x80000000;
	uint32_t DDR_CW0_end   = 0x8000A000;

	uint32_t DDR_CW1_start = 0x90000000;
	uint32_t DDR_CW1_end   = 0x90004000;
	uint32_t hash_start_1  = 0xA0000000;
	uint32_t hash_end_1	   = 0xA0004000;

	uint32_t DDR_CW2_start = 0x90004000;
	uint32_t DDR_CW2_end   = 0x90007000;

	uint32_t DDR_CW3_start = 0x90007000;
	uint32_t DDR_CW3_end   = 0x9000A000;
	uint32_t hash_start_2  = 0xA0005000;
	uint32_t hash_end_2	   = 0xA0008000;

	uint32_t plain_text_start = 0x80010000;
	uint32_t plain_text_end   = 0x8001B000;

	uint32_t KB4_Aligner   = 0x1000;

	uint32_t reg_index  = 0x00,reg_index1 = 0x00,reg_index2 = 0x00,reg_index3 = 0x00;
	uint32_t DATA1  	= 0xDEADBEEF;

	uint8_t byte_DATA1  = 0x5A;
	uint8_t byte_DATA2  = 0xA5;
	uint16_t byte2_DATA1  = 0xDEAD;
	uint16_t byte2_DATA2  = 0xBEEF;
	uint32_t DATA_CLEAR = 0x00000000;
	uint32_t rdata  	= 0x00;
	uint8_t  b_rdata  	= 0x00;
	uint16_t  b2_rdata  	= 0x00;


	uint32_t TEST_AREA_START = DDR_CW1_start;
	uint32_t TEST_AREA_END   = DDR_CW3_end + KB4_Aligner;

	//Clean the test area
	post_log("clearing the test area\n");
	for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Clear test area
		wr(reg_index, DATA_CLEAR);

	for(reg_index = hash_start_1 ; reg_index < hash_end_2 ; reg_index += 4) //Clear test area
		wr(reg_index, DATA_CLEAR);

	SMAU_configure_aes_ecb_window(SMAU_DDR_CW0, DDR_CW0_start, DDR_CW0_end, AES_128, (uint32_t*)common_key0, (uint32_t*)common_key1, ENABLE,  SECURE );
	SMAU_configure_aes_ecb_window(SMAU_DDR_CW2, DDR_CW2_start, DDR_CW2_end, AES_256, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE, NON_SECURE );

	SMAU_configure_aes_ctr_window(SMAU_DDR_CW1, DDR_CW1_start, DDR_CW1_end, AES_192, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE, hash_start_1, hash_end_1, NON_SECURE);
	SMAU_configure_aes_ctr_window(SMAU_DDR_CW3, DDR_CW3_start, DDR_CW3_end, AES_256, (uint32_t*)common_key0, (uint32_t*)common_key1, ENABLE,  hash_start_2, hash_end_2, SECURE);

	smau_cache_invalidate();

		post_log("test case 1\n");
		for(reg_index = DDR_CW0_start ; reg_index < (DDR_CW0_end + KB4_Aligner) ; reg_index += 4) //Write Known Data to test area
			wr(reg_index, DATA1);

		for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Write Known Data to test area
			rdata = rd(reg_index);

		for(reg_index = TEST_AREA_START,reg_index1 = DDR_CW0_start ; reg_index < TEST_AREA_END ; reg_index += 4,reg_index1 += 4) //Read Known Data from test area
		{
			//rdata = rd(reg_index1);
			wr(reg_index,rd(reg_index1));
		}
		for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Read Known Data from test area
		{
			rdata = rd(reg_index);
			if(rdata != DATA1)
			{
				post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,DATA1,rdata);
				result = TEST_FAIL;
				break;
			}
		}

		post_log("test case 2\n");
		for(reg_index = DDR_CW0_start ; reg_index < (DDR_CW0_end + KB4_Aligner) ; reg_index += 1) //Write Known Data to test area
			CPU_WRITE_SINGLE(reg_index, byte_DATA2 , 1);

		for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 1) //Write Known Data to test area
			b_rdata = CPU_READ_SINGLE(reg_index,1);

		for(reg_index = TEST_AREA_START,reg_index1 = DDR_CW0_start,reg_index2 = plain_text_start ; reg_index < TEST_AREA_END ; reg_index += 1,reg_index1 += 1,reg_index2 += 1) //Read Known Data from test area
		{
			//b_rdata = CPU_READ_SINGLE(reg_index1,1);
			CPU_WRITE_SINGLE(reg_index,CPU_READ_SINGLE(reg_index1,1),1);
			//b_rdata = CPU_READ_SINGLE(reg_index,1);
			CPU_WRITE_SINGLE(reg_index2,CPU_READ_SINGLE(reg_index,1),1);
		}
		for(reg_index = plain_text_start ; reg_index < plain_text_end ; reg_index += 1) //Read Known Data from test area
		{
			b_rdata = CPU_READ_SINGLE(reg_index,1);
			if(b_rdata != byte_DATA2)
			{
				post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,byte_DATA2,b_rdata);
				result = TEST_FAIL;
				break;
			}
		}

		post_log("test case 3\n");
		for(reg_index = DDR_CW0_start ; reg_index < (DDR_CW0_end + KB4_Aligner) ; reg_index += 1) //Write Known Data to test area
			CPU_WRITE_SINGLE(reg_index, byte_DATA1 , 1);

		for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 1) //Write Known Data to test area
			b_rdata = CPU_READ_SINGLE(reg_index,1);

		for(reg_index = TEST_AREA_START,reg_index1 = DDR_CW0_start ; reg_index < TEST_AREA_END ; reg_index += 1,reg_index1 += 1) //Read Known Data from test area
		{
			//b_rdata = CPU_READ_SINGLE(reg_index1,1);
			CPU_WRITE_SINGLE(reg_index,CPU_READ_SINGLE(reg_index1,1),1);

		}
		for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 1) //Read Known Data from test area
		{
			b_rdata = CPU_READ_SINGLE(reg_index,1);
			if(b_rdata != byte_DATA1)
			{
				post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,byte_DATA1,b_rdata);
				result = TEST_FAIL;
				break;
			}
		}

		post_log("test case 4\n");
		for(reg_index = DDR_CW0_start ; reg_index < (DDR_CW0_end + KB4_Aligner) ; reg_index += 2) //Write Known Data to test area
			CPU_WRITE_SINGLE(reg_index, byte2_DATA1 , 2);

		for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 2) //Write Known Data to test area
			b2_rdata = CPU_READ_SINGLE(reg_index,2);

		for(reg_index = TEST_AREA_START,reg_index1 = DDR_CW0_start ; reg_index < TEST_AREA_END ; reg_index += 2,reg_index1 += 2) //Read Known Data from test area
		{
			//b2_rdata = CPU_READ_SINGLE(reg_index1,2);
			CPU_WRITE_SINGLE(reg_index,CPU_READ_SINGLE(reg_index1,2),2);

		}
		for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 2) //Read Known Data from test area
		{
			b2_rdata = CPU_READ_SINGLE(reg_index,2);
			if(b2_rdata != byte2_DATA1)
			{
				post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,byte2_DATA1,b2_rdata);
				result = TEST_FAIL;
				break;
			}
		}

		post_log("test case 5\n");
		for(reg_index = DDR_CW0_start ; reg_index < (DDR_CW0_end + KB4_Aligner) ; reg_index += 2) //Write Known Data to test area
			CPU_WRITE_SINGLE(reg_index, byte2_DATA2 , 2);

		for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 2) //Write Known Data to test area
			b2_rdata = CPU_READ_SINGLE(reg_index,2);

		for(reg_index = TEST_AREA_START,reg_index1 = DDR_CW0_start,reg_index2 = plain_text_start ; reg_index < TEST_AREA_END ; reg_index += 2,reg_index1 += 2,reg_index2 += 2) //Read Known Data from test area
		{
			//b2_rdata = CPU_READ_SINGLE(reg_index1,2);
			CPU_WRITE_SINGLE(reg_index,CPU_READ_SINGLE(reg_index1,2),2);
			//b2_rdata = CPU_READ_SINGLE(reg_index,2);
			CPU_WRITE_SINGLE(reg_index2,CPU_READ_SINGLE(reg_index,2),2);
		}
		for(reg_index = plain_text_start ; reg_index < plain_text_end ; reg_index += 2) //Read Known Data from test area
		{
			b2_rdata = CPU_READ_SINGLE(reg_index,2);
			if(b2_rdata != byte2_DATA2)
			{
				post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,byte2_DATA2,b2_rdata);
				result = TEST_FAIL;
				break;
			}
		}
		DDR_CW0_start = 0x8FFFD000;
		DDR_CW0_end   = 0x8FFFF000;

		DDR_CW1_start = 0x90000000;
		DDR_CW1_end   = 0x90002000;
		hash_start_1  = 0xA0000000;
		hash_end_1	  = 0xA0002000;

		DDR_CW2_start = 0x90003000;
		DDR_CW2_end   = 0x90005000;

		DDR_CW3_start = 0x90006000;
		DDR_CW3_end   = 0x90008000;
		hash_start_2  = 0xA0003000;
		hash_end_2	  = 0xA0005000;

		plain_text_start = 0x80010000;
		plain_text_end   = 0x80013000;

		uint32_t plain_text_dest_start = 0x80012000;
		uint32_t plain_text_dest_end   = 0x80015000;

		uint32_t reg_index4, reg_index5;

		TEST_AREA_START = DDR_CW0_start;
		TEST_AREA_END   = DDR_CW3_end + KB4_Aligner;

		//Clean the test area
		post_log("clearing the test area\n");
		for(reg_index = plain_text_dest_start ; reg_index < plain_text_dest_end ; reg_index += 4) //Clear test area
			wr(reg_index, DATA_CLEAR);

		for(reg_index = hash_start_1 ; reg_index < hash_end_2 ; reg_index += 4) //Clear test area
			wr(reg_index, DATA_CLEAR);

		SMAU_configure_aes_ecb_window(SMAU_DDR_CW0, DDR_CW0_start, DDR_CW0_end, AES_128, (uint32_t*)common_key0, (uint32_t*)common_key1, ENABLE,  SECURE );
		SMAU_configure_aes_ecb_window(SMAU_DDR_CW2, DDR_CW2_start, DDR_CW2_end, AES_256, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE, NON_SECURE );

		SMAU_configure_aes_ctr_window(SMAU_DDR_CW1, DDR_CW1_start, DDR_CW1_end, AES_192, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE, hash_start_1, hash_end_1, NON_SECURE);
		SMAU_configure_aes_ctr_window(SMAU_DDR_CW3, DDR_CW3_start, DDR_CW3_end, AES_256, (uint32_t*)common_key0, (uint32_t*)common_key1, ENABLE,  hash_start_2, hash_end_2, SECURE);

		smau_cache_invalidate();

		post_log("test case 6\n");
		for(reg_index = plain_text_start ; reg_index < plain_text_end ; reg_index += 4) //Write Known Data to test area
			wr(reg_index, 0xBEEFDEAD);

		for(reg_index = TEST_AREA_START ; reg_index < TEST_AREA_END ; reg_index += 4) //Write Known Data to test area
			rdata = rd(reg_index);

		for(reg_index1 = DDR_CW0_start,reg_index2 = DDR_CW1_start,reg_index3 = DDR_CW2_start,reg_index4 = DDR_CW3_start,
			reg_index = plain_text_start,reg_index5 = plain_text_dest_start; reg_index1 < (DDR_CW0_end + KB4_Aligner) ; reg_index += 4,reg_index1 += 4,reg_index2 += 4,reg_index3 += 4,reg_index4 += 4,reg_index5 += 4) //Read Known Data from test area
		{
			wr(reg_index1,rd(reg_index)); //plain text src -> CW0

			wr(reg_index2,rd(reg_index1)); //CW0 -> CW1

			wr(reg_index3,rd(reg_index2)); //CW1 -> CW2

			wr(reg_index4,rd(reg_index3)); //CW2 -> CW3

			wr(reg_index1,rd(reg_index4)); //CW3 -> CW0

			wr(reg_index5,rd(reg_index1)); //CW0 -> plain text dest

		}
		for(reg_index = plain_text_dest_start ; reg_index < plain_text_dest_end ; reg_index += 4) //Read Known Data from test area
		{
			if(0xBEEFDEAD != rd(reg_index))
			{
				post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,0xBEEFDEAD,rd(reg_index));
				result = TEST_FAIL;
				break;
			}
		}

	/*-------Restore configs ------*/
	smau_clear_peep_window();
	smau_cache_invalidate();
	smau_set_trustzone_window(SMAU_DDR_CW0, save_tz_base , save_tz_end);
	smau_trustzone_window_control(SMAU_DDR_CW0, save_tz_enable);

	return result;
}

static int SMAU_stress_cpu_path(void)
{
	int result = TEST_PASS;
	//Cache config
	smau_clear_peep_window();
	disable_all_window();
	smau_cache_invalidate();

	smau_partial_mode(ENABLE);
	smau_cache_enable(ENABLE);
	post_log("Enabling Cache...\n");

	smau_prefetch_length(0x00);
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress_cpu_path_1();
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress_cpu_path_2();

	smau_prefetch_length(0x01);
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress_cpu_path_2();
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress_cpu_path_1();

	smau_prefetch_length(0x03);
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress_cpu_path_1();
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress_cpu_path_2();

	smau_prefetch_length(0x05);
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress_cpu_path_1();
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress_cpu_path_2();

	smau_prefetch_length(0x02);
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress_cpu_path_2();
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress_cpu_path_1();

	smau_prefetch_length(0x04);
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress_cpu_path_1();
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress_cpu_path_2();

	smau_prefetch_length(0x07);
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress_cpu_path_1();
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress_cpu_path_2();

	smau_prefetch_length(0x06);
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress_cpu_path_2();
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress_cpu_path_1();

	post_log("Disabling Cache...\n");
	smau_cache_enable(DISABLE);
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress_cpu_path_1();
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress_cpu_path_2();

	post_log("\n-- Test %s --\n",result == TEST_PASS ? "PASSED" : "FAILED");
	return result;
}
#endif
extern uint32_t SetupDMAChain2(struct DMA_Controller *dmac, uint32_t ch );
uint32_t start_dma(uint32_t ch, struct DMA_Controller *dmac , uint32_t dma_src , uint32_t dma_dest , uint32_t prog_addr , uint32_t config_index,uint32_t size)
{
	uint32_t execution_space = ((MEMORY_4KB * ch) + DMA_EXECUTION_MEMORY);
	uint32_t dma_error = setDMACh(dmac,ch);
	if (CYGNUS_STATUS_OK != dma_error)
	{
		post_log("error : setDMACh returns %d\n",dma_error);
		return dma_error;
	}
	config_smau_dma[config_index].transSize = size;
	configureDMACh(dmac,&config_smau_dma[config_index],ch);

	//setExecuteAddr(dmac,prog_addr,ch);
	setExecuteAddr(dmac,execution_space,ch);
	setSrcAddr(dmac,dma_src,ch);
	setDestAddr(dmac,dma_dest,ch);
	//post_log ("Setup DMA - channel -> %d\n",ch);
	SetupDMAChain2(dmac,ch);
	post_log("Completed Setup DMA - channel -> %d\n",ch);

	return CYGNUS_STATUS_OK;
}
/*------------------------ Test end : SMAU_multiple_overlaping_window_access ----------------------------------*/
/*==================================================================================================
*Description : The order of priority  PEEP > Cipher-Text Window > Secure Trustzone
*Parameter   :
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
extern uint32_t waitForDmaChComplete( uint32_t ch , uint32_t sec_nonsec);
extern uint32_t Sec_startDmaCh(struct DMA_Controller *dmac, uint32_t ch , uint32_t secure);

static int __SMAU_stress_DMA_path(void)
{
	int result = TEST_PASS;
	uint32_t save_tz_base    	   = 0x00;
	uint32_t save_tz_end     	   = 0x00;
	uint32_t save_tz_enable  	   = 0x00;
	uint32_t max_count 			   = 0;

	uint8_t common_key0[]     	   = "CW1_key0_aeskey0_diagnostics_aes";
	uint8_t common_key1[]     	   = "CW1_key1_aeskey1_diagnostics_aes";

	uint32_t DDR_CW2_start 		   = 0x90600000;
	uint32_t DDR_CW2_end   		   = 0x907FF000;

	uint32_t DDR_CW3_start 		   = 0x90800000;
	uint32_t DDR_CW3_end   		   = 0x909FF000;
	uint32_t hash_start_1          = 0xA0000000;
	uint32_t hash_end_1	   		   = 0xA01FF000;

	uint32_t DDR_CW0_start		   = 0x90000000;
	uint32_t DDR_CW0_end   		   = 0x901FF000;

	uint32_t DDR_CW1_start 		   = 0x90200000;
	uint32_t DDR_CW1_end   		   = 0x903FF000;
	uint32_t hash_start_2  		   = 0xA0200000;
	uint32_t hash_end_2	   		   = 0xA03FF000;

	uint32_t DDR_TZ_start  		   = 0x90400000;
	//uint32_t DDR_TZ_end    		   = 0x905FF000;

	uint32_t src_data_1            = 0x80000000;
	uint32_t src_data_2            = DDR_CW1_start;


	uint32_t data_size 		       = 0x00003000; //0x00200000;
	uint32_t test2_size 		   = 0x00003000; //0x40000;
	uint32_t test3_size 		   = 0x00003000; //0x00200000;
	uint32_t test4_size 		   = 0x00003000; //0x40000;


	uint32_t dest_data_1           = DDR_CW0_start;
	uint32_t dest_data_2           = DDR_CW2_start;
	uint32_t dest_data_3           = DDR_CW3_start;
	uint32_t dest_data_4           = DDR_TZ_start;
	uint32_t dest_data_5           = 0x90A00000;


	uint32_t random_number = 0x00;
	uint32_t ref_data = 0x00;
	uint32_t reg_index  = 0x00;
	uint32_t DATA1  	= 0xBEEFDEAD;
	uint32_t DATA2  	= 0x5A5A5A5A;
	uint32_t rdata  	= 0x00;
	uint32_t dont_use[16];
	uint32_t data1_count = 0;
	uint32_t data2_count = 0;

	struct DMA_Controller dmac_inst;// NULL;
	struct DMA_Controller *dmac = &dmac_inst;// NULL;
	int ch = 0x01;
	int dma_error = 0x00;
	//int *prog_space = DMA_EXECUTION_MEMORY;//malloc(0x4000);
	uint32_t prog_space = DMA_EXECUTION_MEMORY;

	bcm_rng_raw_generate (pHandle, (uint8_t  *)dont_use, 16 , NULL, NULL);
	bcm_rng_raw_generate (pHandle, (uint8_t  *)dont_use, 16 , NULL, NULL);

    bcm_rng_raw_generate (pHandle, (uint8_t *)&random_number, 1 , NULL, NULL);
    DATA1 = random_number;
    bcm_rng_raw_generate (pHandle, (uint8_t *)&random_number, 1 , NULL, NULL);
    DATA2 = random_number;

    post_log("Random Number 1 = 0x%X\nRandom Number 2 = 0x%X\n",DATA1,DATA2);
	//Init DMA
	post_log("InitDMA \n");
	initDMA(dmac);
	//Clean the test area

	/*Considering the CW1 as Source */
	SMAU_configure_aes_ecb_window(SMAU_DDR_CW0, DDR_CW0_start, DDR_CW0_end, AES_128, (uint32_t*)common_key0, (uint32_t*)common_key1, ENABLE,  SECURE );
	SMAU_configure_aes_ecb_window(SMAU_DDR_CW2, DDR_CW2_start, DDR_CW2_end, AES_256, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE, NON_SECURE );

	SMAU_configure_aes_ctr_window(SMAU_DDR_CW1, DDR_CW1_start, DDR_CW1_end, AES_192, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE, hash_start_1, hash_end_1, NON_SECURE);
	SMAU_configure_aes_ctr_window(SMAU_DDR_CW3, DDR_CW3_start, DDR_CW3_end, AES_256, (uint32_t*)common_key0, (uint32_t*)common_key1, ENABLE,  hash_start_2, hash_end_2, SECURE);
	smau_cache_invalidate();

	post_log("Filling the DMA source memory\n");
	ref_data = DATA2;
	for(reg_index = src_data_2 ; reg_index < (src_data_2 + data_size ) ; reg_index += 4) //Write Known Data to test area
		wr(reg_index, ref_data++);

	ref_data = DATA1;
	for(reg_index = src_data_1 ; reg_index < (src_data_1 + data_size) ; reg_index += 4) //Write Known Data to test area
		wr(reg_index, ref_data++);
#if 1

	post_log("test case 1\n");
	//Configure 5 DMA channels
	ch = DMA_Chan1;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_1 , prog_space , 0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);

	ch = DMA_Chan2;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_2 , prog_space , 0, (data_size )))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);

	ch = DMA_Chan3;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_3 , prog_space , 0, (data_size )))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);


	ch = DMA_Chan4;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_4 , prog_space , 0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan4,DMA_SecureState);

	ch = DMA_Chan5;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_5 , prog_space , 0, (data_size )))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan5,DMA_SecureState);

	Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan4,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan5,DMA_SecureState);



	waitForDmaChComplete(DMA_Chan1,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan1);
	waitForDmaChComplete(DMA_Chan2,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan2);
	waitForDmaChComplete(DMA_Chan3,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan3);
	waitForDmaChComplete(DMA_Chan4,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan4);
	waitForDmaChComplete(DMA_Chan5,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan5);

	post_log("Checking the test area...\n");
	ref_data = DATA2;
	for(reg_index = dest_data_1 ; reg_index < (dest_data_1 + data_size ) ; reg_index += 4) //Read Known Data from test area
	{
		rdata = rd(reg_index);
		if(rdata != ref_data++)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,DATA2,rdata);
			result = TEST_FAIL;
			break;
		}
		if( !(reg_index % 0x1000) )
			post_log("#");
	}
	post_log("\n");
#if 1
	ref_data = DATA1;
	for(reg_index = dest_data_2 ; reg_index < (dest_data_2 + data_size ) ; reg_index += 4) //Read Known Data from test area
	{
		rdata = rd(reg_index);
		if(rdata != ref_data++)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,DATA1,rdata);
			result = TEST_FAIL;
			break;
		}
		if( !(reg_index % 0x1000) )
			post_log("#");
	}
	post_log("\n");
	ref_data = DATA2;
	for(reg_index = dest_data_3 ; reg_index < (dest_data_3 + data_size  ) ; reg_index += 4) //Read Known Data from test area
	{
		rdata = rd(reg_index);
		if(rdata != ref_data++)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,DATA2,rdata);
			result = TEST_FAIL;
			break;
		}
		if( !(reg_index % 0x1000) )
			post_log("#");
	}
	post_log("\n");
	ref_data = DATA1;
	for(reg_index = dest_data_4 ; reg_index < (dest_data_4 + data_size  ) ; reg_index += 4) //Read Known Data from test area
	{
		rdata = rd(reg_index);
		if(rdata != ref_data++)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,DATA1,rdata);
			result = TEST_FAIL;
			break;
		}
		if( !(reg_index % 0x1000) )
			post_log("#");
	}
	post_log("\n");
	ref_data = DATA2;
	for(reg_index = dest_data_5 ; reg_index < (dest_data_5 + data_size  ) ; reg_index += 4) //Read Known Data from test area
	{
		rdata = rd(reg_index);
		if(rdata != ref_data++)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,DATA2,rdata);
			result = TEST_FAIL;
			break;
		}
		if( !(reg_index % 0x1000) )
			post_log("#");
	}
	post_log("\n");
#endif
	freeDMACh(dmac, DMA_Chan1);
	freeDMACh(dmac, DMA_Chan2);
	freeDMACh(dmac, DMA_Chan3);
	freeDMACh(dmac, DMA_Chan4);
	freeDMACh(dmac, DMA_Chan5);

	post_log("test case 2\n");//Source from the same address

	//Initialize the destination memory
	//Initialize the src memory
    bcm_rng_raw_generate (pHandle, (uint8_t *)&random_number, 1 , NULL, NULL);
    DATA1 = random_number;
    bcm_rng_raw_generate (pHandle, (uint8_t *)&random_number, 1 , NULL, NULL);
    DATA2 = random_number;
    post_log("Random Number 1 = 0x%X\nRandom Number 2 = 0x%X\n",DATA1,DATA2);
    wr(src_data_2, DATA1);
	wr(src_data_1, DATA2);

	data_size = test2_size;

	//Configure 5 DMA channels, //Source from the same address
	ch = DMA_Chan1;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_1 , prog_space , 1, data_size))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);

	ch = DMA_Chan2;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_2 , prog_space , 1, data_size))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);

	ch = DMA_Chan3;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_3 , prog_space , 1, data_size))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);


	ch = DMA_Chan4;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_4 , prog_space , 1, data_size))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan4,DMA_SecureState);

	ch = DMA_Chan5;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_5 , prog_space , 1, data_size))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan5,DMA_SecureState);

	Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan4,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan5,DMA_SecureState);


	waitForDmaChComplete(DMA_Chan1,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan1);
	waitForDmaChComplete(DMA_Chan2,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan2);
	waitForDmaChComplete(DMA_Chan3,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan3);
	waitForDmaChComplete(DMA_Chan4,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan4);
	waitForDmaChComplete(DMA_Chan5,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan5);

	post_log("Checking the test area...\n");
	ref_data = DATA1;
	for(reg_index = dest_data_1 ; reg_index < (dest_data_1 + data_size) ; reg_index += 4) //Read Known Data from test area
	{
		rdata = rd(reg_index);
		if(rdata != ref_data)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,DATA1,rdata);
			result = TEST_FAIL;
			break;
		}
		if( !(reg_index % 0x1000) )
			post_log("#");
	}
	post_log("\n");
	ref_data = DATA2;
	for(reg_index = dest_data_2 ; reg_index < (dest_data_2 + data_size) ; reg_index += 4) //Read Known Data from test area
	{
		rdata = rd(reg_index);
		if(rdata != ref_data)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,DATA2,rdata);
			result = TEST_FAIL;
			break;
		}
		if( !(reg_index % 0x1000) )
			post_log("#");
	}
	post_log("\n");
	ref_data = DATA1;
	for(reg_index = dest_data_3 ; reg_index < (dest_data_3 + data_size) ; reg_index += 4) //Read Known Data from test area
	{
		rdata = rd(reg_index);
		if(rdata != ref_data)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,DATA1,rdata);
			result = TEST_FAIL;
			break;
		}
		if( !(reg_index % 0x1000) )
			post_log("#");
	}
	post_log("\n");
	ref_data = DATA2;
	for(reg_index = dest_data_4 ; reg_index < (dest_data_4 + data_size) ; reg_index += 4) //Read Known Data from test area
	{
		rdata = rd(reg_index);
		if(rdata != ref_data)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,DATA2,rdata);
			result = TEST_FAIL;
			break;
		}
		if( !(reg_index % 0x1000) )
			post_log("#");
	}
	post_log("\n");
	ref_data = DATA1;
	for(reg_index = dest_data_5 ; reg_index < (dest_data_5 + data_size) ; reg_index += 4) //Read Known Data from test area
	{
		rdata = rd(reg_index);
		if(rdata != ref_data)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X ,data read = 0x%X\n",reg_index,DATA1,rdata);
			result = TEST_FAIL;
			break;
		}
		if( !(reg_index % 0x1000) )
			post_log("#");
	}
	post_log("\n");
	 	src_data_1            = DDR_CW1_start;
	 	src_data_2            = DDR_CW2_start;
	 	data_size 		      = test3_size;
	 	dest_data_1           = 0x90100000; /*Destination contain both the address*/

	 	freeDMACh(dmac, DMA_Chan0);
		freeDMACh(dmac, DMA_Chan1);
		freeDMACh(dmac, DMA_Chan2);
		freeDMACh(dmac, DMA_Chan3);
		freeDMACh(dmac, DMA_Chan4);
		freeDMACh(dmac, DMA_Chan5);
		freeDMACh(dmac, DMA_Chan6);
		freeDMACh(dmac, DMA_Chan7);

		post_log("test case 3\n");
		//Initialize the src memory
	    bcm_rng_raw_generate (pHandle, (uint8_t *)&random_number, 1 , NULL, NULL);
	    DATA1 = random_number;
	    bcm_rng_raw_generate (pHandle, (uint8_t *)&random_number, 1 , NULL, NULL);
	    DATA2 = random_number;
	    post_log("Random Number 1 = 0x%X\nRandom Number 2 = 0x%X\n",DATA1,DATA2);

		ref_data = DATA2;
		for(reg_index = src_data_2 ; reg_index < (src_data_2 + data_size) ; reg_index += 4) //Write Known Data to test area
			wr(reg_index, ref_data);

		ref_data = DATA1;
		for(reg_index = src_data_1 ; reg_index < (src_data_1 + data_size) ; reg_index += 4) //Write Known Data to test area
			wr(reg_index, ref_data);


		ch = DMA_Chan1;
		post_log("Configuring DMA channel : 0x%X\n",ch);
		if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , 2, data_size))
		{
			post_log("error : start_dma channel - %d returns %d",ch,dma_error);
			result = TEST_FAIL;
		}
		//Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);

		ch = DMA_Chan2;
		post_log("Configuring DMA channel : 0x%X\n",ch);
		if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_1 , prog_space , 2, data_size))
		{
			post_log("error : start_dma channel - %d returns %d",ch,dma_error);
			result = TEST_FAIL;
		}
		//Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);

		ch = DMA_Chan3;
		post_log("Configuring DMA channel : 0x%X\n",ch);
		if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , 2, data_size))
		{
			post_log("error : start_dma channel - %d returns %d",ch,dma_error);
			result = TEST_FAIL;
		}
		//Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);


		ch = DMA_Chan4;
		post_log("Configuring DMA channel : 0x%X\n",ch);
		if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_1 , prog_space , 2, data_size))
		{
			post_log("error : start_dma channel - %d returns %d",ch,dma_error);
			result = TEST_FAIL;
		}
		//Sec_startDmaCh(dmac,DMA_Chan4,DMA_SecureState);

		ch = DMA_Chan5;
		post_log("Configuring DMA channel : 0x%X\n",ch);
		if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , 2, data_size))
		{
			post_log("error : start_dma channel - %d returns %d",ch,dma_error);
			result = TEST_FAIL;
		}
		//Sec_startDmaCh(dmac,DMA_Chan5,DMA_SecureState);

		ch = DMA_Chan6;
		post_log("Configuring DMA channel : 0x%X\n",ch);
		if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_1 , prog_space , 2, data_size))
		{
			post_log("error : start_dma channel - %d returns %d",ch,dma_error);
			result = TEST_FAIL;
		}
		//Sec_startDmaCh(dmac,DMA_Chan6,DMA_SecureState);

		ch = DMA_Chan7;
		post_log("Configuring DMA channel : 0x%X\n",ch);
		if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , 2, data_size))
		{
			post_log("error : start_dma channel - %d returns %d",ch,dma_error);
			result = TEST_FAIL;
		}
		//Sec_startDmaCh(dmac,DMA_Chan7,DMA_SecureState);

		ch = DMA_Chan0;
		post_log("Configuring DMA channel : 0x%X\n",ch);
		if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_1 , prog_space , 2, data_size))
		{
			post_log("error : start_dma channel - %d returns %d",ch,dma_error);
			result = TEST_FAIL;
		}
		//Sec_startDmaCh(dmac,DMA_Chan0,DMA_SecureState);

		Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);
		Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);
		Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);
		Sec_startDmaCh(dmac,DMA_Chan4,DMA_SecureState);
		Sec_startDmaCh(dmac,DMA_Chan5,DMA_SecureState);
		Sec_startDmaCh(dmac,DMA_Chan6,DMA_SecureState);
		Sec_startDmaCh(dmac,DMA_Chan7,DMA_SecureState);
		Sec_startDmaCh(dmac,DMA_Chan0,DMA_SecureState);

		waitForDmaChComplete(DMA_Chan1,DMA_SecureState);
		post_log("DMA channel - %d Completed\n",DMA_Chan1);
		waitForDmaChComplete(DMA_Chan2,DMA_SecureState);
		post_log("DMA channel - %d Completed\n",DMA_Chan2);
		waitForDmaChComplete(DMA_Chan3,DMA_SecureState);
		post_log("DMA channel - %d Completed\n",DMA_Chan3);
		waitForDmaChComplete(DMA_Chan4,DMA_SecureState);
		post_log("DMA channel - %d Completed\n",DMA_Chan4);
		waitForDmaChComplete(DMA_Chan5,DMA_SecureState);
		post_log("DMA channel - %d Completed\n",DMA_Chan5);
		waitForDmaChComplete(DMA_Chan6,DMA_SecureState);
		post_log("DMA channel - %d Completed\n",DMA_Chan6);
		waitForDmaChComplete(DMA_Chan7,DMA_SecureState);
		post_log("DMA channel - %d Completed\n",DMA_Chan7);
		waitForDmaChComplete(DMA_Chan0,DMA_SecureState);
		post_log("DMA channel - %d Completed\n",DMA_Chan0);

		post_log("Checking the test area...\n");


		for(reg_index = dest_data_1 ; reg_index < (dest_data_1 + data_size ) ; reg_index += 4) //Read Known Data from test area
		{
			rdata = rd(reg_index);ref_data = DATA2;
			if((rdata != DATA1) && (rdata != DATA2))
			{
				post_log("error : data error @  0x%X , data expected = 0x%X or 0x%X,data read = 0x%X\n",reg_index,DATA1,DATA2,rdata);
				result = TEST_FAIL;
				break;
			}
			else
			{
				if(rdata == DATA1)
					data1_count++;
				if(rdata == DATA2)
					data2_count++;
			}
			if( !(reg_index % 0x1000) )
				post_log("#");
		}
		post_log("\ndata 1 counter = 0x%X\ndata 2 counter = 0x%X\n",data1_count,data2_count);
#endif
		post_log("test case 4\n");
		src_data_1            = DDR_CW1_start;
	 	src_data_2            = DDR_CW2_start;
	 	data_size 		      = test4_size;
	 	dest_data_1           = 0x90100000; /*Destination contain both the address*/

	 	freeDMACh(dmac, DMA_Chan0);
		freeDMACh(dmac, DMA_Chan1);
		freeDMACh(dmac, DMA_Chan2);
		freeDMACh(dmac, DMA_Chan3);
		freeDMACh(dmac, DMA_Chan4);
		freeDMACh(dmac, DMA_Chan5);
		freeDMACh(dmac, DMA_Chan6);
		freeDMACh(dmac, DMA_Chan7);
	    bcm_rng_raw_generate (pHandle, (uint8_t *)&random_number, 1 , NULL, NULL);
	    DATA1 = random_number;
	    bcm_rng_raw_generate (pHandle, (uint8_t *)&random_number, 1 , NULL, NULL);
	    DATA2 = random_number;
	    post_log("Random Number 1 = 0x%X\nRandom Number 2 = 0x%X\n",DATA1,DATA2);

		wr(src_data_1, DATA2);
		wr(src_data_2, DATA1);

		//Configure 5 DMA channels, //Source from the same address
		ch = DMA_Chan1;
		post_log("Configuring DMA channel : 0x%X\n",ch);
		if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , 3, data_size))
		{
			post_log("error : start_dma channel - %d returns %d",ch,dma_error);
			result = TEST_FAIL;
		}
		//Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);

		ch = DMA_Chan2;
		post_log("Configuring DMA channel : 0x%X\n",ch);
		if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_1 , prog_space , 3, data_size))
		{
			post_log("error : start_dma channel - %d returns %d",ch,dma_error);
			result = TEST_FAIL;
		}
		//Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);

		ch = DMA_Chan3;
		post_log("Configuring DMA channel : 0x%X\n",ch);
		if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , 3, data_size))
		{
			post_log("error : start_dma channel - %d returns %d",ch,dma_error);
			result = TEST_FAIL;
		}
		//Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);


		ch = DMA_Chan4;
		post_log("Configuring DMA channel : 0x%X\n",ch);
		if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_1 , prog_space , 3, data_size))
		{
			post_log("error : start_dma channel - %d returns %d",ch,dma_error);
			result = TEST_FAIL;
		}
		//Sec_startDmaCh(dmac,DMA_Chan4,DMA_SecureState);

		ch = DMA_Chan5;
		post_log("Configuring DMA channel : 0x%X\n",ch);
		if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , 3, data_size))
		{
			post_log("error : start_dma channel - %d returns %d",ch,dma_error);
			result = TEST_FAIL;
		}
		//Sec_startDmaCh(dmac,DMA_Chan5,DMA_SecureState);

		ch = DMA_Chan6;
		post_log("Configuring DMA channel : 0x%X\n",ch);
		if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_1 , prog_space , 3, data_size))
		{
			post_log("error : start_dma channel - %d returns %d",ch,dma_error);
			result = TEST_FAIL;
		}
		//Sec_startDmaCh(dmac,DMA_Chan6,DMA_SecureState);

		ch = DMA_Chan7;
		post_log("Configuring DMA channel : 0x%X\n",ch);
		if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , 3, data_size))
		{
			post_log("error : start_dma channel - %d returns %d",ch,dma_error);
			result = TEST_FAIL;
		}
		//Sec_startDmaCh(dmac,DMA_Chan7,DMA_SecureState);

		ch = DMA_Chan0;
		post_log("Configuring DMA channel : 0x%X\n",ch);
		if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_1 , prog_space , 3, data_size))
		{
			post_log("error : start_dma channel - %d returns %d",ch,dma_error);
			result = TEST_FAIL;
		}
		//Sec_startDmaCh(dmac,DMA_Chan0,DMA_SecureState);
		Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);
		Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);
		Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);
		Sec_startDmaCh(dmac,DMA_Chan4,DMA_SecureState);
		Sec_startDmaCh(dmac,DMA_Chan5,DMA_SecureState);
		Sec_startDmaCh(dmac,DMA_Chan6,DMA_SecureState);
		Sec_startDmaCh(dmac,DMA_Chan7,DMA_SecureState);
		Sec_startDmaCh(dmac,DMA_Chan0,DMA_SecureState);

		max_count = 100;
		while(max_count--)
		{
			rdata = rd(dest_data_1);
		}

		waitForDmaChComplete(DMA_Chan1,DMA_SecureState);
		post_log("DMA channel - %d Completed\n",DMA_Chan1);
		waitForDmaChComplete(DMA_Chan2,DMA_SecureState);
		post_log("DMA channel - %d Completed\n",DMA_Chan2);
		waitForDmaChComplete(DMA_Chan3,DMA_SecureState);
		post_log("DMA channel - %d Completed\n",DMA_Chan3);
		waitForDmaChComplete(DMA_Chan4,DMA_SecureState);
		post_log("DMA channel - %d Completed\n",DMA_Chan4);
		waitForDmaChComplete(DMA_Chan5,DMA_SecureState);
		post_log("DMA channel - %d Completed\n",DMA_Chan5);
		waitForDmaChComplete(DMA_Chan6,DMA_SecureState);
		post_log("DMA channel - %d Completed\n",DMA_Chan6);
		waitForDmaChComplete(DMA_Chan7,DMA_SecureState);
		post_log("DMA channel - %d Completed\n",DMA_Chan7);
		waitForDmaChComplete(DMA_Chan0,DMA_SecureState);
		post_log("DMA channel - %d Completed\n",DMA_Chan0);

		post_log("Checking the test area...\n");

		rdata = rd(dest_data_1);
		if((rdata != DATA1) && (rdata != DATA2))
		{
			post_log("error : data error @  0x%X , data expected = 0x%X or 0x%X,data read = 0x%X\n",reg_index,DATA1,DATA2,rdata);
			result = TEST_FAIL;
		}

	/*-------Restore configs ------*/
	smau_clear_peep_window();
	smau_cache_invalidate();
	smau_set_trustzone_window(SMAU_DDR_CW0, save_tz_base , save_tz_end);
	smau_trustzone_window_control(SMAU_DDR_CW0, save_tz_enable);

	return result;
}

static int SMAU_stress_DMA_path(void)
{
	int result = TEST_PASS;
	//Cache config
	smau_clear_peep_window();
	disable_all_window();
	smau_cache_invalidate();
	smau_partial_mode(ENABLE);
	smau_cache_enable(ENABLE);
	post_log("Enabling Cache...\n");

	smau_prefetch_length(0x00);
	result += __SMAU_stress_DMA_path();

	smau_prefetch_length(0x01);
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress_DMA_path();

	smau_prefetch_length(0x03);
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress_DMA_path();

	smau_prefetch_length(0x05);
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress_DMA_path();

	smau_prefetch_length(0x02);
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress_DMA_path();

	smau_prefetch_length(0x04);
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress_DMA_path();

	smau_prefetch_length(0x07);
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress_DMA_path();

	smau_prefetch_length(0x06);
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress_DMA_path();

	post_log("Disabling Cache...\n");
	smau_cache_enable(DISABLE);
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress_DMA_path();

	post_log("\n-- Test %s --\n",result == TEST_PASS ? "PASSED" : "FAILED");
	return result;
}

static int SMAU_stress_DMA_ACP_path(void)
{
	int result = TEST_PASS;
	//Cache config
	enable_dmac_acp(0x1 /*drive_idm_val*/ , 0x2 /*awcache*/, 0x1 /*awuser*/, 0x2 /*arcache*/, 0x1 /*aruser*/);
	smau_cache_invalidate();
	smau_partial_mode(ENABLE);
	smau_cache_enable(ENABLE);
	post_log("Enabling Cache...\n");
	smau_prefetch_length(0x00);
	result += __SMAU_stress_DMA_path();
	smau_prefetch_length(0x01);
	result += __SMAU_stress_DMA_path();
	smau_prefetch_length(0x03);
	result += __SMAU_stress_DMA_path();
	smau_prefetch_length(0x05);
	result += __SMAU_stress_DMA_path();
	smau_prefetch_length(0x02);
	result += __SMAU_stress_DMA_path();
	smau_prefetch_length(0x04);
	result += __SMAU_stress_DMA_path();
	smau_prefetch_length(0x07);
	result += __SMAU_stress_DMA_path();
	smau_prefetch_length(0x06);
	result += __SMAU_stress_DMA_path();
	post_log("Disabling Cache...\n");
	smau_cache_enable(DISABLE);
	result += __SMAU_stress_DMA_path();

	post_log("\n-- Test %s --\n",result == TEST_PASS ? "PASSED" : "FAILED");
	return result;
}

static int __SMAU_stress(void)
{
	int result = TEST_PASS;
	uint32_t save_tz_base    	   = 0x00;
	uint32_t save_tz_end     	   = 0x00;
	uint32_t save_tz_enable  	   = 0x00;
	uint32_t max_count 			   = 0;

	uint8_t common_key0[]     	   = "CW1_key0_aeskey0_diagnostics_aes";
	uint8_t common_key1[]     	   = "CW1_key1_aeskey1_diagnostics_aes";

	uint32_t DDR_CW2_start 		   = 0x90600000;
	uint32_t DDR_CW2_end   		   = 0x907FF000;

	uint32_t DDR_CW3_start 		   = 0x90800000;
	uint32_t DDR_CW3_end   		   = 0x909FF000;
	uint32_t hash_start_1          = 0xA0000000;
	uint32_t hash_end_1	   		   = 0xA01FF000;

	uint32_t DDR_CW0_start		   = 0x90000000;
	uint32_t DDR_CW0_end   		   = 0x901FF000;

	uint32_t DDR_CW1_start 		   = 0x90200000;
	uint32_t DDR_CW1_end   		   = 0x903FF000;
	uint32_t hash_start_2  		   = 0xA0200000;
	uint32_t hash_end_2	   		   = 0xA03FF000;

	uint32_t src_data_1            = DDR_CW2_start;
	uint32_t src_data_2            = DDR_CW0_start;
	uint32_t src_data_3            = DDR_CW1_start;
	uint32_t data_size 		       = 0x00003000;//0x00200000;
	uint32_t dest_data_1           = DDR_CW0_start;

	uint32_t random_number = 0x00;
#if 0
	uint32_t ref_data = 0x00;
#endif

	uint32_t reg_index  = 0x00,reg_index1 = 0x00,reg_index2 = 0x00;
	uint32_t DATA1  	= 0xBEEFDEAD;
	uint32_t DATA2  	= 0x5A5A5A5A;
	uint32_t DATA3  	= 0x5A5A5A5A;
	uint32_t rdata  	= 0x00 ;
	uint32_t dont_use[16];
	uint32_t data1_count = 0x00, data2_count = 0x00, data3_count = 0x00;
    uint64_t DATA64_1 = 0x00;
    uint64_t DATA64_2 = 0x00;
    uint64_t DATA64_3 = 0x00;
    uint64_t random_64;
    uint64_t read64 = 0x00;

	struct DMA_Controller dmac_inst;// NULL;
	struct DMA_Controller *dmac = &dmac_inst;// NULL;
	int ch = 0x01;
	int dma_error = 0x00;
	//int *prog_space = DMA_EXECUTION_MEMORY; //prog_space; //malloc(0x4000);
	uint32_t prog_space = DMA_EXECUTION_MEMORY;

	bcm_rng_raw_generate (pHandle, (uint8_t  *)dont_use, 16 , NULL, NULL);
	bcm_rng_raw_generate (pHandle, (uint8_t  *)dont_use, 16 , NULL, NULL);

	//Init DMA
	post_log("InitDMA \n");
	initDMA(dmac);
	//Clean the test area

	/*Considering the CW1 as Source */
	SMAU_configure_aes_ecb_window(SMAU_DDR_CW0, DDR_CW0_start, DDR_CW0_end, AES_128, (uint32_t*)common_key0, (uint32_t*)common_key1, ENABLE,  SECURE );
	SMAU_configure_aes_ecb_window(SMAU_DDR_CW2, DDR_CW2_start, DDR_CW2_end, AES_256, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE, NON_SECURE );

	SMAU_configure_aes_ctr_window(SMAU_DDR_CW1, DDR_CW1_start, DDR_CW1_end, AES_192, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE, hash_start_1, hash_end_1, NON_SECURE);
	SMAU_configure_aes_ctr_window(SMAU_DDR_CW3, DDR_CW3_start, DDR_CW3_end, AES_256, (uint32_t*)common_key0, (uint32_t*)common_key1, ENABLE,  hash_start_2, hash_end_2, SECURE);
	smau_cache_invalidate();

    bcm_rng_raw_generate (pHandle, (uint8_t *)&random_number, 1 , NULL, NULL);
    DATA1 = random_number;
    bcm_rng_raw_generate (pHandle, (uint8_t *)&random_number, 1 , NULL, NULL);
    DATA2 = random_number;
    bcm_rng_raw_generate (pHandle, (uint8_t *)&random_number, 1 , NULL, NULL);
    DATA3 = random_number;
	post_log("Random Number 1 = 0x%X\nRandom Number 2 = 0x%X\nRandom Number 3 = 0x%X\n",DATA1,DATA2,DATA3);
	max_count = 20;
	reg_index  = src_data_1;
	reg_index1 = src_data_2;
	reg_index2 = src_data_3;
	while(max_count--)
	{
		wr(reg_index, DATA1);
		wr(reg_index1, DATA2);
		wr(reg_index2, DATA3);

		reg_index  += 4;
		reg_index1 += 4;
		reg_index2 += 4;
	}

	post_log("Filling the DMA source memory...\n");
	freeDMACh(dmac, DMA_Chan1);
	freeDMACh(dmac, DMA_Chan2);
	freeDMACh(dmac, DMA_Chan3);
	ch = DMA_Chan1;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , src_data_1 , prog_space , MAX_4MB_4BYTE_16DAT_SRC0_DEST1, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);

	ch = DMA_Chan2;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , src_data_2 , prog_space , MAX_4MB_4BYTE_16DAT_SRC0_DEST1, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);

	ch = DMA_Chan3;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_3 , src_data_3 , prog_space , MAX_4MB_4BYTE_16DAT_SRC0_DEST1, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//ec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);

	waitForDmaChComplete(DMA_Chan1,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan1);
	waitForDmaChComplete(DMA_Chan2,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan2);
	waitForDmaChComplete(DMA_Chan3,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan3);
	freeDMACh(dmac, DMA_Chan1);
	freeDMACh(dmac, DMA_Chan2);
	freeDMACh(dmac, DMA_Chan3);
#if 1
#if 0
	for(reg_index = src_data_1 ; reg_index < (src_data_1 + data_size ) ; reg_index += 4) //Write Known Data to test area
		wr(reg_index, ref_data);
	ref_data = DATA2;
	for(reg_index = src_data_2 ; reg_index < (src_data_2 + data_size ) ; reg_index += 4) //Write Known Data to test area
		wr(reg_index, ref_data);
	ref_data = DATA3;
	for(reg_index = src_data_3 ; reg_index < (src_data_3 + data_size ) ; reg_index += 4) //Write Known Data to test area
		wr(reg_index, ref_data);
#endif
	post_log("test case 1\n");
	//Configure 5 DMA channels
	ch = DMA_Chan1;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_1 , prog_space , 0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);

	ch = DMA_Chan2;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , 0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);

	ch = DMA_Chan3;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_1 , prog_space , 0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);


	ch = DMA_Chan4;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , 0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan4,DMA_SecureState);

	ch = DMA_Chan5;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_1 , prog_space , 0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan5,DMA_SecureState);

	ch = DMA_Chan6;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , 0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan6,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan4,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan5,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan6,DMA_SecureState);


	for(reg_index = src_data_3 + 0x00100000,reg_index1 = dest_data_1 + 0x500; reg_index < (src_data_3 + data_size) ; reg_index += 4,reg_index1 += 4) //Write Known Data to test area
		wr(reg_index1, rd(reg_index));

	waitForDmaChComplete(DMA_Chan1,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan1);
	waitForDmaChComplete(DMA_Chan2,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan2);
	waitForDmaChComplete(DMA_Chan3,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan3);
	waitForDmaChComplete(DMA_Chan4,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan4);
	waitForDmaChComplete(DMA_Chan5,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan5);
	waitForDmaChComplete(DMA_Chan6,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan6);

	post_log("Checking the test area...\n");

	for(reg_index = dest_data_1 ; reg_index < (dest_data_1 + data_size) ; reg_index += 4) //Read Known Data from test area
	{
		rdata = rd(reg_index);
		if((rdata != DATA1) && (rdata != DATA2) && (rdata != DATA3))
		{
			post_log("error : data error @  0x%X , data expected = 0x%X or 0x%X or 0x%X,data read = 0x%X\n",reg_index,DATA1,DATA2,DATA3,rdata);
			result = TEST_FAIL;
			break;
		}
		else
		{
			if(rdata == DATA1)
				data1_count++;
			if(rdata == DATA2)
				data2_count++;
			if(rdata == DATA3)
				data3_count++;
		}
		if( !(reg_index % 0x1000) )
			post_log("#");
	}
	post_log("\nData 1 count = 0x%X\nData 2 count = 0x%X\nData 3 count = 0x%X\n",data1_count,data2_count,data3_count);
	freeDMACh(dmac, DMA_Chan1);
	freeDMACh(dmac, DMA_Chan2);
	freeDMACh(dmac, DMA_Chan3);
	freeDMACh(dmac, DMA_Chan4);
	freeDMACh(dmac, DMA_Chan5);
	freeDMACh(dmac, DMA_Chan6);
	post_log("test case 2\n");
	//Configure 5 DMA channels
	ch = DMA_Chan1;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_1 , prog_space , 0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);

	ch = DMA_Chan2;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , 0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);

	ch = DMA_Chan3;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_1 , prog_space , 0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);


	ch = DMA_Chan4;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , 0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan4,DMA_SecureState);

	ch = DMA_Chan5;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_1 , prog_space , 0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan5,DMA_SecureState);

	ch = DMA_Chan6;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , 0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan6,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan4,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan5,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan6,DMA_SecureState);

	for(reg_index = src_data_3,reg_index1 = (dest_data_1 + data_size - 4) ; reg_index < ((src_data_1 + data_size)/2) ; reg_index += 4,reg_index1 -= 4) //Write Known Data to test area
		wr(reg_index1, rd(reg_index));

	waitForDmaChComplete(DMA_Chan1,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan1);
	waitForDmaChComplete(DMA_Chan2,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan2);
	waitForDmaChComplete(DMA_Chan3,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan3);
	waitForDmaChComplete(DMA_Chan4,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan4);
	waitForDmaChComplete(DMA_Chan5,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan5);
	waitForDmaChComplete(DMA_Chan6,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan6);

	post_log("Checking the test area...\n");

	for(reg_index = dest_data_1 ; reg_index < (dest_data_1 + data_size) ; reg_index += 4) //Read Known Data from test area
	{
		rdata = rd(reg_index);
		if((rdata != DATA1) && (rdata != DATA2) && (rdata != DATA3))
		{
			post_log("error : data error @  0x%X , data expected = 0x%X or 0x%X or 0x%X,data read = 0x%X\n",reg_index,DATA1,DATA2,DATA3,rdata);
			result = TEST_FAIL;
			break;
		}
		else
		{
			if(rdata == DATA1)
				data1_count++;
			if(rdata == DATA2)
				data2_count++;
			if(rdata == DATA3)
				data3_count++;
		}
		if( !(reg_index % 0x1000) )
			post_log("#");
	}
	post_log("\nData 1 count = 0x%X\nData 2 count = 0x%X\nData 2 count = 0x%X\n",data1_count,data2_count,data3_count);


	post_log("test case 3\n");
	freeDMACh(dmac, DMA_Chan1);
	freeDMACh(dmac, DMA_Chan2);
	freeDMACh(dmac, DMA_Chan3);
	freeDMACh(dmac, DMA_Chan4);
	freeDMACh(dmac, DMA_Chan5);
	freeDMACh(dmac, DMA_Chan6);
	//Configure 5 DMA channels
    max_count = 2000;
    bcm_rng_raw_generate (pHandle, (uint8_t *)&random_number, 1 , NULL, NULL);
    DATA1 = random_number;
    bcm_rng_raw_generate (pHandle, (uint8_t *)&random_number, 1 , NULL, NULL);
    DATA2 = random_number;
    bcm_rng_raw_generate (pHandle, (uint8_t *)&random_number, 1 , NULL, NULL);
    DATA3 = random_number;
	post_log("Random Number 1 = 0x%X\nRandom Number 2 = 0x%X\nRandom Number 3 = 0x%X\n",DATA1,DATA2,DATA3);
	post_log("Filling the DMA source memory...\n");
	reg32_write(src_data_1,DATA1);
	reg32_write(src_data_2,DATA2);
	reg32_write(src_data_3,DATA3);

	ch = DMA_Chan1;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_1 , prog_space , MAX_4MB_4BYTE_16DAT_SRC0_DEST0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);

	ch = DMA_Chan2;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , MAX_4MB_4BYTE_16DAT_SRC0_DEST0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);

	ch = DMA_Chan3;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_1 , prog_space , MAX_4MB_4BYTE_16DAT_SRC0_DEST0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);


	ch = DMA_Chan4;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , MAX_4MB_4BYTE_16DAT_SRC0_DEST0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan4,DMA_SecureState);

	ch = DMA_Chan5;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_1 , prog_space , MAX_4MB_4BYTE_16DAT_SRC0_DEST0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan5,DMA_SecureState);

	ch = DMA_Chan6;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , MAX_4MB_4BYTE_16DAT_SRC0_DEST0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan6,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan4,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan5,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan6,DMA_SecureState);

	max_count = 2000;
	while(max_count--) //Write Known Data to test area
		reg32_write(dest_data_1,(reg32_read(src_data_3)));

	waitForDmaChComplete(DMA_Chan1,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan1);
	waitForDmaChComplete(DMA_Chan2,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan2);
	waitForDmaChComplete(DMA_Chan3,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan3);
	waitForDmaChComplete(DMA_Chan4,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan4);
	waitForDmaChComplete(DMA_Chan5,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan5);
	waitForDmaChComplete(DMA_Chan6,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan6);

	post_log("Checking the test area...\n");
	rdata = reg32_read(dest_data_1);
	if( (rdata != DATA1) && (rdata != DATA2) && (rdata != DATA3) )
	{
		post_log("error : data @ 0x%X is 0x%llX\n",dest_data_1,rdata);
		result = TEST_FAIL;
	}


	post_log("test case 4\n");
	freeDMACh(dmac, DMA_Chan1);
	freeDMACh(dmac, DMA_Chan2);
	freeDMACh(dmac, DMA_Chan3);
	freeDMACh(dmac, DMA_Chan4);
	freeDMACh(dmac, DMA_Chan5);
	freeDMACh(dmac, DMA_Chan6);

	//Configure 5 DMA channels

    max_count = 2000;
    bcm_rng_raw_generate (pHandle, (uint8_t  *)&random_64, 2 , NULL, NULL);
    DATA64_1 = random_64;
    bcm_rng_raw_generate (pHandle, (uint8_t  *)&random_64, 2 , NULL, NULL);
    DATA64_2 = random_64;
    bcm_rng_raw_generate (pHandle, (uint8_t  *)&random_64, 2 , NULL, NULL);
    DATA64_3 = random_64;
	post_log("Random Number 1 = 0x%llX\nRandom Number 2 = 0x%llX\nRandom Number 3 = 0x%llX\n",DATA64_1,DATA64_2,DATA64_3);
	post_log("Filling the DMA source memory...\n");
	reg64_write(src_data_1,DATA64_1);
	reg64_write(src_data_2,DATA64_2);
	reg64_write(src_data_3,DATA64_3);

	ch = DMA_Chan1;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_1 , prog_space , MAX_8MB_8BYTE_16DAT_SRC0_DEST0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);

	ch = DMA_Chan2;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , MAX_8MB_8BYTE_16DAT_SRC0_DEST0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);

	ch = DMA_Chan3;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_1 , prog_space , MAX_8MB_8BYTE_16DAT_SRC0_DEST0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);


	ch = DMA_Chan4;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , MAX_8MB_8BYTE_16DAT_SRC0_DEST0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan4,DMA_SecureState);

	ch = DMA_Chan5;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_1 , prog_space , MAX_8MB_8BYTE_16DAT_SRC0_DEST0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan5,DMA_SecureState);

	ch = DMA_Chan6;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , MAX_8MB_8BYTE_16DAT_SRC0_DEST0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan6,DMA_SecureState);
	max_count = 2000;
	Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan4,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan5,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan6,DMA_SecureState);

	while(max_count--) //Write Known Data to test area
	{
		reg64_write(dest_data_1,reg64_read(src_data_3));
		read64 = reg64_read(dest_data_1);
		if( (read64 != DATA64_1) && (read64 != DATA64_2) && (read64 != DATA64_3) )
		{
			post_log("error : data @ 0x%X is 0x%llX\n",dest_data_1,read64);
			result = TEST_FAIL;
		}
	}

	waitForDmaChComplete(DMA_Chan1,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan1);
	waitForDmaChComplete(DMA_Chan2,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan2);
	waitForDmaChComplete(DMA_Chan3,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan3);
	waitForDmaChComplete(DMA_Chan4,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan4);
	waitForDmaChComplete(DMA_Chan5,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan5);
	waitForDmaChComplete(DMA_Chan6,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan6);

	post_log("Checking the test area...\n");
	read64 = reg64_read(dest_data_1);
	if( (read64 != DATA64_1) && (read64 != DATA64_2) && (read64 != DATA64_3) )
	{
		post_log("error : data @ 0x%X is 0x%llX\n",dest_data_1,read64);
		result = TEST_FAIL;
	}

#endif
	post_log("test case 5\n");
	freeDMACh(dmac, DMA_Chan1);
	freeDMACh(dmac, DMA_Chan2);
	freeDMACh(dmac, DMA_Chan3);
	freeDMACh(dmac, DMA_Chan4);
	freeDMACh(dmac, DMA_Chan5);
	freeDMACh(dmac, DMA_Chan6);

	//Configure 5 DMA channels
    DATA64_1 = 0x00;
    DATA64_2 = 0x00;
    DATA64_3 = 0x00;

    max_count = 2000;
    bcm_rng_raw_generate (pHandle, (uint8_t  *)&random_64, 2 , NULL, NULL);
    DATA64_1 = random_64;
    bcm_rng_raw_generate (pHandle, (uint8_t  *)&random_64, 2 , NULL, NULL);
    DATA64_2 = random_64;
    bcm_rng_raw_generate (pHandle, (uint8_t  *)&random_64, 2 , NULL, NULL);
    DATA64_3 = random_64;
	post_log("Random Number 1 = 0x%llX\nRandom Number 2 = 0x%llX\nRandom Number 3 = 0x%llX\n",DATA64_1,DATA64_2,DATA64_3);
	post_log("Filling the DMA source memory...\n");

	max_count = 20;
	reg_index  = src_data_1;
	reg_index1 = src_data_2;
	reg_index2 = src_data_3;
	while(max_count--)
	{
		reg64_write(reg_index,  DATA64_1);
		reg64_write(reg_index1, DATA64_2);
		reg64_write(reg_index2, DATA64_3);

		reg_index  += 8;
		reg_index1 += 8;
		reg_index2 += 8;
	}
	//post_log("CPU Write : 0x%llX\n",reg64_read(src_data_3));
	freeDMACh(dmac, DMA_Chan1);
	freeDMACh(dmac, DMA_Chan2);
	freeDMACh(dmac, DMA_Chan3);
	ch = DMA_Chan1;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , src_data_1 , prog_space , MAX_2MB_8BYTE_4DAT_SRC0_DEST1, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);

	ch = DMA_Chan2;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , src_data_2 , prog_space , MAX_2MB_8BYTE_4DAT_SRC0_DEST1, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);

	ch = DMA_Chan3;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_3 , src_data_3 , prog_space , MAX_2MB_8BYTE_4DAT_SRC0_DEST1, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);

	waitForDmaChComplete(DMA_Chan1,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan1);
	waitForDmaChComplete(DMA_Chan2,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan2);
	waitForDmaChComplete(DMA_Chan3,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan3);
	freeDMACh(dmac, DMA_Chan1);
	freeDMACh(dmac, DMA_Chan2);
	freeDMACh(dmac, DMA_Chan3);
#if 0
	post_log("CPU Write : 0x%llX\n",reg64_read(src_data_3));
	for(reg_index = src_data_1,reg_index1 = src_data_2,reg_index2 = src_data_3; reg_index < (src_data_1 + data_size ) ; reg_index += 8,reg_index1 += 8,reg_index2 += 8) //Write Known Data to test area
	{
		read64 = reg64_read(reg_index);
		if(DATA64_1 != read64)
			post_log("error : data failure at : 0x%X - 0x%llX\n",reg_index,read64);
		read64 = reg64_read(reg_index1);
		if(DATA64_2 != read64)
			post_log("error : data failure at : 0x%X - 0x%llX\n",reg_index1,read64);
		read64 = reg64_read(reg_index2);
		if(DATA64_3 != read64)
			post_log("error : data failure at : 0x%X - 0x%llX\n",reg_index2,read64);
	}


	for(reg_index = src_data_1,reg_index1 = src_data_2,reg_index2 = src_data_3; reg_index < (src_data_1 + data_size ) ; reg_index += 8,reg_index1 += 8,reg_index2 += 8) //Write Known Data to test area
	{
		reg64_write(reg_index,DATA64_1);
		reg64_write(reg_index1,DATA64_2);
		reg64_write(reg_index2,DATA64_3);
	}
#endif
	ch = DMA_Chan1;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_1 , prog_space , MAX_4MB_8BYTE_8DAT_SRC1_DEST0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);

	ch = DMA_Chan2;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , MAX_4MB_8BYTE_8DAT_SRC1_DEST0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);

	ch = DMA_Chan3;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_1 , prog_space , MAX_4MB_8BYTE_8DAT_SRC1_DEST0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);


	ch = DMA_Chan4;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , MAX_4MB_8BYTE_8DAT_SRC1_DEST0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan4,DMA_SecureState);

	ch = DMA_Chan5;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_1 , prog_space , MAX_4MB_8BYTE_8DAT_SRC1_DEST0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan5,DMA_SecureState);

	ch = DMA_Chan6;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , MAX_4MB_8BYTE_8DAT_SRC1_DEST0, (data_size)))
	{
		post_log("error : start_dma channel - %d returns %d",ch,dma_error);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan6,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan4,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan5,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan6,DMA_SecureState);

	reg_index = src_data_3;
	max_count = 1000;
#if 1
	while(max_count--) //Write Known Data to test area
	{
		//post_log("CPU read dest : 0x%llX\n",reg64_read(dest_data_1));
		//read64 = reg64_read(reg_index);
		//post_log("CPU read src : 0x%llX\n",read64);
		//reg64_write(dest_data_1,read64);
		reg64_write(dest_data_1,reg64_read(reg_index));
		read64 = reg64_read(dest_data_1);
		if( (read64 != DATA64_1) && (read64 != DATA64_2) && (read64 != DATA64_3) )
		{
			post_log("error : data @ 0x%X is 0x%llX\n",dest_data_1,read64);
			result = TEST_FAIL;
		}
		reg_index += 8;
	}
#endif
	waitForDmaChComplete(DMA_Chan1,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan1);
	waitForDmaChComplete(DMA_Chan2,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan2);
	waitForDmaChComplete(DMA_Chan3,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan3);
	waitForDmaChComplete(DMA_Chan4,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan4);
	waitForDmaChComplete(DMA_Chan5,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan5);
	waitForDmaChComplete(DMA_Chan6,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan6);

	post_log("Checking the test area...\n");
    read64 = reg64_read(dest_data_1);
	if( (read64 != DATA64_1) && (read64 != DATA64_2) && (read64 != DATA64_3) )
	{
		post_log("error : data @ 0x%X is 0x%llX\n",dest_data_1,read64);
		result = TEST_FAIL;
	}

	freeDMACh(dmac, DMA_Chan0);
	freeDMACh(dmac, DMA_Chan1);
	freeDMACh(dmac, DMA_Chan2);
	freeDMACh(dmac, DMA_Chan3);
	freeDMACh(dmac, DMA_Chan4);
	freeDMACh(dmac, DMA_Chan5);
	freeDMACh(dmac, DMA_Chan6);
	freeDMACh(dmac, DMA_Chan7);

	/*-------Restore configs ------*/
	smau_clear_peep_window();
	smau_cache_invalidate();
	smau_set_trustzone_window(SMAU_DDR_CW0, save_tz_base , save_tz_end);
	smau_trustzone_window_control(SMAU_DDR_CW0, save_tz_enable);

	return result;
}
static int SMAU_stress_test(void)
{
	int result = TEST_PASS;
	//Cache config
	smau_clear_peep_window();
	disable_all_window();
	smau_cache_invalidate();
	smau_partial_mode(ENABLE);
	smau_cache_enable(ENABLE);
	post_log("Enabling Cache...\n");
	smau_prefetch_length(0x00);
	result += __SMAU_stress();

	smau_prefetch_length(0x01);
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress();

	smau_prefetch_length(0x03);
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress();

	smau_prefetch_length(0x05);
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress();

	smau_prefetch_length(0x02);
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress();

	smau_prefetch_length(0x04);
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress();

	smau_prefetch_length(0x07);
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress();

	smau_prefetch_length(0x06);
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress();

	post_log("Disabling Cache...\n");
	smau_cache_enable(DISABLE);
	smau_clear_peep_window();
	disable_all_window();
	result += __SMAU_stress();

	post_log("\n-- Test %s --\n",result == TEST_PASS ? "PASSED" : "FAILED");
	return result;
}

static int __SMAU_stress_cache(void)
{
	int result = TEST_PASS;
	uint32_t save_tz_base    	   = 0x00;
	uint32_t save_tz_end     	   = 0x00;
	uint32_t save_tz_enable  	   = 0x00;


	uint8_t common_key0[]     	   = "CW1_key0_aeskey0_diagnostics_aes";
	uint8_t common_key1[]     	   = "CW1_key1_aeskey1_diagnostics_aes";

	uint32_t DDR_CW0_start 		   = 0x90000000;
	uint32_t DDR_CW0_end   		   = 0x90FFF000;

	uint32_t DDR_CW1_start 		   = 0x91000000;
	uint32_t DDR_CW1_end   		   = 0x91FFF000;
	uint32_t hash_start_1          = 0xA0000000;
	uint32_t hash_end_1	   		   = 0xA0FFF000;

	uint32_t DDR_CW2_start		   = 0x92000000;
	uint32_t DDR_CW2_end   		   = 0x92FFF000;

	uint32_t DDR_CW3_start 		   = 0x93000000;
	uint32_t DDR_CW3_end   		   = 0x93FFF000;
	uint32_t hash_start_2  		   = 0xA1000000;
	uint32_t hash_end_2	   		   = 0xA1FFF000;

	uint32_t DDR_TZ_start  		   = 0x94000000;

	uint32_t Plain_text_start      = 0x95000000;

	uint32_t src_data_1            = DDR_CW0_start;
	uint32_t src_data_2            = Plain_text_start;
	uint32_t src_data_3            = DDR_CW0_start;
	uint32_t src_data_4            = DDR_CW0_start;

	uint32_t dest_data_1           = DDR_CW2_start;
	uint32_t dest_data_2           = DDR_TZ_start;
	uint32_t dest_data_3           = DDR_CW3_start;
	uint32_t dest_data_4           = DDR_CW1_start;

	uint32_t data_size 		       = 0x00003000; //0x00800000;
	uint32_t DATA1  			   = 0xBEEFDEAD;
	uint32_t DATA2  			   = 0x5A5A5A5A;

	struct DMA_Controller dmac_inst;// NULL;
	struct DMA_Controller *dmac = &dmac_inst;// NULL;
	int ch = 0x01;
	//int *prog_space = DMA_EXECUTION_MEMORY; //malloc(0x4000);
	uint32_t prog_space = DMA_EXECUTION_MEMORY;

	uint32_t random_number = 0x00;
	uint32_t dont_use[16];

	uint32_t ref_data   = 0x00,ref_data1,ref_data2,ref_data3;
	uint32_t reg_index  = 0x00,reg_index1,reg_index2,reg_index3;
	uint32_t rdata  	= 0x00;
	uint32_t loop1		= 0;
	uint32_t loop2		= 0;


	//Init DMA
	post_log("InitDMA \n");
	initDMA(dmac);
	if(BCM_SCAPI_STATUS_OK != bcm_rng_raw_generate (pHandle, (uint8_t  *)dont_use, 16 , NULL, NULL))
		post_log("bcm_rng_raw_generate 1 failed");
	if(BCM_SCAPI_STATUS_OK != bcm_rng_raw_generate (pHandle, (uint8_t  *)dont_use, 16 , NULL, NULL))
		post_log("bcm_rng_raw_generate 2 failed");

	/*Considering the CW1 as Source */
	SMAU_configure_aes_ecb_window(SMAU_DDR_CW0, DDR_CW0_start, DDR_CW0_end, AES_128, (uint32_t*)common_key0, (uint32_t*)common_key1, ENABLE,  SECURE );
	SMAU_configure_aes_ecb_window(SMAU_DDR_CW2, DDR_CW2_start, DDR_CW2_end, AES_256, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE, NON_SECURE );
	SMAU_configure_aes_ctr_window(SMAU_DDR_CW1, DDR_CW1_start, DDR_CW1_end, AES_192, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE, hash_start_1, hash_end_1, NON_SECURE);
	SMAU_configure_aes_ctr_window(SMAU_DDR_CW3, DDR_CW3_start, DDR_CW3_end, AES_256, (uint32_t*)common_key0, (uint32_t*)common_key1, ENABLE,  hash_start_2, hash_end_2, SECURE);
	smau_cache_invalidate();

	//Generate Random Number and Fill the DMA source memory
    bcm_rng_raw_generate (pHandle, (uint8_t *)&random_number, 1 , NULL, NULL);
    DATA1 = random_number;
    bcm_rng_raw_generate (pHandle, (uint8_t *)&random_number, 1 , NULL, NULL);
    DATA2 = random_number;
	post_log("Random Number 1 = 0x%X\nRandom Number 2 = 0x%X\n",DATA1,DATA2);

	post_log("Filling the DMA source memory...\n");
	ref_data = DATA1;

	for(reg_index = src_data_1 ; reg_index < (src_data_1 + data_size ) ; reg_index += 4) //Write Known Data to test area
		wr(reg_index, ref_data++);
	ref_data = DATA2;
	for(reg_index = src_data_2 ; reg_index < (src_data_2 + data_size ) ; reg_index += 4) //Write Known Data to test area
		wr(reg_index, ref_data++);

	post_log("test case 1\n");
	//Configure 5 DMA channels
	ch = DMA_Chan1;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);

	ch = DMA_Chan2;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_2 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);

	ch = DMA_Chan3;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_3 , dest_data_3 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);


	ch = DMA_Chan4;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_4 , dest_data_4 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan4,DMA_SecureState);

	ch = DMA_Chan5;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan5,DMA_SecureState);

	ch = DMA_Chan6;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_2 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan6,DMA_SecureState);

	ch = DMA_Chan7;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_3 , dest_data_3 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan7,DMA_SecureState);

	ch = DMA_Chan0;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_4 , dest_data_4 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan0,DMA_SecureState);

	Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan4,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan5,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan6,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan7,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan0,DMA_SecureState);
#if 1
	for(loop1 = 0 ; loop1 < 50 ; loop1++)
	{
		for(loop2 = 0 ; loop2 <= 7 ; loop2++)
			smau_prefetch_length(loop2);
	}
#endif

	post_log("Waiting for DMA to complete....\n");
	waitForDmaChComplete(DMA_Chan1,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan1);
	waitForDmaChComplete(DMA_Chan2,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan2);
	waitForDmaChComplete(DMA_Chan3,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan3);
	waitForDmaChComplete(DMA_Chan4,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan4);
	waitForDmaChComplete(DMA_Chan5,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan5);
	waitForDmaChComplete(DMA_Chan6,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan6);
	waitForDmaChComplete(DMA_Chan7,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan7);
	waitForDmaChComplete(DMA_Chan0,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan0);

	ref_data  = DATA1;
	ref_data1 = DATA2;
	ref_data2 = DATA1;
	ref_data3 = DATA1;
	post_log("Checking the test area...\n");
	for(reg_index = dest_data_1,reg_index1 = dest_data_2,reg_index2 = dest_data_3,reg_index3 = dest_data_4 ; reg_index < (dest_data_1 + data_size) ; reg_index += 4,reg_index1 += 4,reg_index2 += 4,reg_index3 += 4) //Read Known Data from test area
	{
		rdata = rd(reg_index);

		if(rdata != ref_data++)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X data read = 0x%X\n",reg_index,ref_data,rdata);
			result = TEST_FAIL;
			break;
		}

		rdata = rd(reg_index2);
		if(rdata != ref_data2++)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X data read = 0x%X\n",reg_index2,ref_data,rdata);
			result = TEST_FAIL;
			break;
		}

		rdata = rd(reg_index3);
		if(rdata != ref_data3++)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X data read = 0x%X\n",reg_index3,ref_data,rdata);
			result = TEST_FAIL;
			break;
		}

		rdata = rd(reg_index1);
		if(rdata != ref_data1++)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X data read = 0x%X\n",reg_index1,ref_data,rdata);
			result = TEST_FAIL;
			break;
		}
		if( !(reg_index % 0x1000) )
			post_log("#");
	}

	post_log("\n");
	post_log("test case 2\n");
	freeDMACh(dmac, DMA_Chan1);
	freeDMACh(dmac, DMA_Chan2);
	freeDMACh(dmac, DMA_Chan3);
	freeDMACh(dmac, DMA_Chan4);
	freeDMACh(dmac, DMA_Chan5);
	freeDMACh(dmac, DMA_Chan6);
	freeDMACh(dmac, DMA_Chan7);
	freeDMACh(dmac, DMA_Chan0);
	//Configure 5 DMA channels
	ch = DMA_Chan1;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);

	ch = DMA_Chan2;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_2 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);

	ch = DMA_Chan3;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_3 , dest_data_3 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);


	ch = DMA_Chan4;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_4 , dest_data_4 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan4,DMA_SecureState);

	ch = DMA_Chan5;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan5,DMA_SecureState);

	ch = DMA_Chan6;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_2 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan6,DMA_SecureState);

	ch = DMA_Chan7;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_3 , dest_data_3 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan7,DMA_SecureState);

	ch = DMA_Chan0;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_4 , dest_data_4 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan0,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan4,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan5,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan6,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan7,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan0,DMA_SecureState);
#if 0
	for(loop1 = 0 ; loop1 < 50 ; loop1++)
	{
		for(loop2 = 1 ; loop2 < 7 ; loop2++)
		{
		    if(1 == (loop2 % 2))
		    	smau_partial_mode(ENABLE);
		    else
		    	smau_partial_mode(DISABLE);
		}
	}
#endif
	waitForDmaChComplete(DMA_Chan1,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan1);
	waitForDmaChComplete(DMA_Chan2,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan2);
	waitForDmaChComplete(DMA_Chan3,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan3);
	waitForDmaChComplete(DMA_Chan4,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan4);
	waitForDmaChComplete(DMA_Chan5,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan5);
	waitForDmaChComplete(DMA_Chan6,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan6);
	waitForDmaChComplete(DMA_Chan7,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan7);
	waitForDmaChComplete(DMA_Chan0,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan0);

	ref_data  = DATA1;
	ref_data1 = DATA2;
	ref_data2 = DATA1;
	ref_data3 = DATA1;
	post_log("Checking the test area...\n");
	for(reg_index = dest_data_1,reg_index1 = dest_data_2,reg_index2 = dest_data_3,reg_index3 = dest_data_4 ; reg_index < (dest_data_1 + data_size) ; reg_index += 4,reg_index1 += 4,reg_index2 += 4,reg_index3 += 4) //Read Known Data from test area
	{
		rdata = rd(reg_index);

		if(rdata != ref_data++)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X data read = 0x%X\n",reg_index,ref_data,rdata);
			result = TEST_FAIL;
			break;
		}

		rdata = rd(reg_index2);
		if(rdata != ref_data2++)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X data read = 0x%X\n",reg_index2,ref_data,rdata);
			result = TEST_FAIL;
			break;
		}

		rdata = rd(reg_index3);
		if(rdata != ref_data3++)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X data read = 0x%X\n",reg_index3,ref_data,rdata);
			result = TEST_FAIL;
			break;
		}

		rdata = rd(reg_index1);
		if(rdata != ref_data1++)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X data read = 0x%X\n",reg_index1,ref_data,rdata);
			result = TEST_FAIL;
			break;
		}
		if( !(reg_index % 0x1000) )
			post_log("#");
	}


	post_log("test case 3\n");
	freeDMACh(dmac, DMA_Chan1);
	freeDMACh(dmac, DMA_Chan2);
	freeDMACh(dmac, DMA_Chan3);
	freeDMACh(dmac, DMA_Chan4);
	freeDMACh(dmac, DMA_Chan5);
	freeDMACh(dmac, DMA_Chan6);
	freeDMACh(dmac, DMA_Chan7);
	freeDMACh(dmac, DMA_Chan0);
	//Configure 5 DMA channels
	ch = DMA_Chan1;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);

	ch = DMA_Chan2;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_2 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);

	ch = DMA_Chan3;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_3 , dest_data_3 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);


	ch = DMA_Chan4;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_4 , dest_data_4 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan4,DMA_SecureState);

	ch = DMA_Chan5;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan5,DMA_SecureState);

	ch = DMA_Chan6;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_2 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan6,DMA_SecureState);

	ch = DMA_Chan7;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_3 , dest_data_3 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan7,DMA_SecureState);

	ch = DMA_Chan0;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_4 , dest_data_4 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan0,DMA_SecureState);

	Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan4,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan5,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan6,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan7,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan0,DMA_SecureState);

	for(loop1 = 0 ; loop1 < 50 ; loop1++)
	{
		for(loop2 = 1 ; loop2 < 7 ; loop2++)
			smau_cache_invalidate();
	}

	waitForDmaChComplete(DMA_Chan1,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan1);
	waitForDmaChComplete(DMA_Chan2,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan2);
	waitForDmaChComplete(DMA_Chan3,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan3);
	waitForDmaChComplete(DMA_Chan4,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan4);
	waitForDmaChComplete(DMA_Chan5,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan5);
	waitForDmaChComplete(DMA_Chan6,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan6);
	waitForDmaChComplete(DMA_Chan7,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan7);
	waitForDmaChComplete(DMA_Chan0,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan0);

	ref_data  = DATA1;
	ref_data1 = DATA2;
	ref_data2 = DATA1;
	ref_data3 = DATA1;
	post_log("Checking the test area...\n");
	for(reg_index = dest_data_1,reg_index1 = dest_data_2,reg_index2 = dest_data_3,reg_index3 = dest_data_4 ; reg_index < (dest_data_1 + data_size) ; reg_index += 4,reg_index1 += 4,reg_index2 += 4,reg_index3 += 4) //Read Known Data from test area
	{
		rdata = rd(reg_index);

		if(rdata != ref_data++)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X data read = 0x%X\n",reg_index,ref_data,rdata);
			result = TEST_FAIL;
			break;
		}

		rdata = rd(reg_index2);
		if(rdata != ref_data2++)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X data read = 0x%X\n",reg_index2,ref_data,rdata);
			result = TEST_FAIL;
			break;
		}

		rdata = rd(reg_index3);
		if(rdata != ref_data3++)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X data read = 0x%X\n",reg_index3,ref_data,rdata);
			result = TEST_FAIL;
			break;
		}

		rdata = rd(reg_index1);
		if(rdata != ref_data1++)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X data read = 0x%X\n",reg_index1,ref_data,rdata);
			result = TEST_FAIL;
			break;
		}
		if( !(reg_index % 0x1000) )
			post_log("#");
	}

	post_log("\ntest case 4\n");
	freeDMACh(dmac, DMA_Chan1);
	freeDMACh(dmac, DMA_Chan2);
	freeDMACh(dmac, DMA_Chan3);
	freeDMACh(dmac, DMA_Chan4);
	freeDMACh(dmac, DMA_Chan5);
	freeDMACh(dmac, DMA_Chan6);
	freeDMACh(dmac, DMA_Chan7);
	freeDMACh(dmac, DMA_Chan0);
	//Configure 5 DMA channels
	ch = DMA_Chan1;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);

	ch = DMA_Chan2;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_2 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);

	ch = DMA_Chan3;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_3 , dest_data_3 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);


	ch = DMA_Chan4;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_4 , dest_data_4 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan4,DMA_SecureState);

	ch = DMA_Chan5;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_1 , dest_data_1 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan5,DMA_SecureState);

	ch = DMA_Chan6;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_2 , dest_data_2 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan6,DMA_SecureState);

	ch = DMA_Chan7;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_3 , dest_data_3 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan7,DMA_SecureState);

	ch = DMA_Chan0;
	post_log("Configuring DMA channel : 0x%X\n",ch);
	if(CYGNUS_STATUS_OK != start_dma(ch, dmac , src_data_4 , dest_data_4 , prog_space , 6, (data_size)))
	{
		post_log("error : start_dma channel - %d Failed",ch);
		result = TEST_FAIL;
	}
	//Sec_startDmaCh(dmac,DMA_Chan0,DMA_SecureState);

	Sec_startDmaCh(dmac,DMA_Chan1,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan2,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan3,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan4,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan5,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan6,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan7,DMA_SecureState);
	Sec_startDmaCh(dmac,DMA_Chan0,DMA_SecureState);
#if 1
	for(loop1 = 0 ; loop1 < 50 ; loop1++)
	{
		for(loop2 = 1 ; loop2 < 7 ; loop2++)
		{
		    if(1 == (loop2 % 2))
		    	smau_cache_enable(ENABLE);
		    else
		    	smau_cache_enable(DISABLE);
		}
	}
#endif
	waitForDmaChComplete(DMA_Chan1,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan1);
	waitForDmaChComplete(DMA_Chan2,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan2);
	waitForDmaChComplete(DMA_Chan3,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan3);
	waitForDmaChComplete(DMA_Chan4,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan4);
	waitForDmaChComplete(DMA_Chan5,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan5);
	waitForDmaChComplete(DMA_Chan6,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan6);
	waitForDmaChComplete(DMA_Chan7,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan7);
	waitForDmaChComplete(DMA_Chan0,DMA_SecureState);
	post_log("DMA channel - %d Completed\n",DMA_Chan0);

	ref_data  = DATA1;
	ref_data1 = DATA2;
	ref_data2 = DATA1;
	ref_data3 = DATA1;
	post_log("Checking the test area...\n");
	for(reg_index = dest_data_1,reg_index1 = dest_data_2,reg_index2 = dest_data_3,reg_index3 = dest_data_4 ; reg_index < (dest_data_1 + data_size) ; reg_index += 4,reg_index1 += 4,reg_index2 += 4,reg_index3 += 4) //Read Known Data from test area
	{
		rdata = rd(reg_index);

		if(rdata != ref_data++)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X data read = 0x%X\n",reg_index,ref_data,rdata);
			result = TEST_FAIL;
			break;
		}

		rdata = rd(reg_index2);
		if(rdata != ref_data2++)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X data read = 0x%X\n",reg_index2,ref_data,rdata);
			result = TEST_FAIL;
			break;
		}

		rdata = rd(reg_index3);
		if(rdata != ref_data3++)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X data read = 0x%X\n",reg_index3,ref_data,rdata);
			result = TEST_FAIL;
			break;
		}

		rdata = rd(reg_index1);
		if(rdata != ref_data1++)
		{
			post_log("error : data error @  0x%X , data expected = 0x%X data read = 0x%X\n",reg_index1,ref_data,rdata);
			result = TEST_FAIL;
			break;
		}
		if( !(reg_index % 0x1000) )
			post_log("#");
	}
	/*-------Restore configs ------*/
	smau_clear_peep_window();
	smau_cache_invalidate();
	smau_set_trustzone_window(SMAU_DDR_CW0, save_tz_base , save_tz_end);
	smau_trustzone_window_control(SMAU_DDR_CW0, save_tz_enable);
post_log("\n");
	return result;
}
static int SMAU_stress_cache(void)
{
	int result = TEST_PASS;
	//Cache config
	smau_clear_peep_window();
	disable_all_window();
	smau_cache_invalidate();
	smau_partial_mode(ENABLE);
	smau_cache_enable(ENABLE);
	post_log("Enabling Cache...\n");
	smau_prefetch_length(0x00);
	result += __SMAU_stress_cache();

	post_log("\n-- Test %s --\n",result == TEST_PASS ? "PASSED" : "FAILED");
	return result;
}


static int SMAU_all_stress_test(void)
{
	int result = TEST_PASS;
	uint32_t loop_count,loop_count1 = 0;
	uint32_t pass = 0 , fail = 0 , skip = 0;

    FUNC_INFO test[]=	{
					ADD_TEST(  SMAU_write_invalidate_cache 				  ),
					ADD_TEST(  SMAU_peep_window_secure_nonsecure_access   ),
					ADD_TEST(  SMAU_cipher_window_secure_nonsecure_access ),
					ADD_TEST(  SMAU_stress_cpu_path						  ),
					ADD_TEST(  SMAU_stress_DMA_path 					  ),
					ADD_TEST(  SMAU_stress_test							  ),
					/*ADD_TEST(  SMAU_stress_DMA_ACP_path                   ), */
					ADD_TEST(  SMAU_stress_cache                          ),
					ADD_TEST(  SMAU_multiple_non_overlaping_window_access ),
					ADD_TEST(  SMAU_multiple_overlaping_window_access     ),


					ADD_TEST(  SMAU_write_invalidate_cache 				  ),
					ADD_TEST(  SMAU_peep_window_secure_nonsecure_access   ),
					ADD_TEST(  SMAU_cipher_window_secure_nonsecure_access ),
					ADD_TEST(  SMAU_stress_cpu_path						  ),
					ADD_TEST(  SMAU_stress_DMA_path 					  ),
					ADD_TEST(  SMAU_stress_test							  ),
					/*ADD_TEST(  SMAU_stress_DMA_ACP_path                   ), */
					ADD_TEST(  SMAU_stress_cache                          ),
					ADD_TEST(  SMAU_multiple_non_overlaping_window_access ),
					ADD_TEST(  SMAU_multiple_overlaping_window_access     ),



					ADD_TEST(  SMAU_write_invalidate_cache 				  ),
					ADD_TEST(  SMAU_peep_window_secure_nonsecure_access   ),
					ADD_TEST(  SMAU_cipher_window_secure_nonsecure_access ),
					ADD_TEST(  SMAU_stress_cpu_path						  ),
					ADD_TEST(  SMAU_stress_DMA_path 					  ),
					ADD_TEST(  SMAU_stress_test							  ),
					/*ADD_TEST(  SMAU_stress_DMA_ACP_path                   ), */
					ADD_TEST(  SMAU_stress_cache                          ),
					ADD_TEST(  SMAU_multiple_non_overlaping_window_access ),
					ADD_TEST(  SMAU_multiple_overlaping_window_access     ),

					ADD_TEST(  SMAU_write_invalidate_cache 				  ),
					ADD_TEST(  SMAU_peep_window_secure_nonsecure_access   ),
					ADD_TEST(  SMAU_cipher_window_secure_nonsecure_access ),
					ADD_TEST(  SMAU_stress_cpu_path						  ),
					ADD_TEST(  SMAU_stress_DMA_path 					  ),
					ADD_TEST(  SMAU_stress_test							  ),
					/*ADD_TEST(  SMAU_stress_DMA_ACP_path                   ), */
					ADD_TEST(  SMAU_stress_cache                          ),
					ADD_TEST(  SMAU_multiple_non_overlaping_window_access ),
					ADD_TEST(  SMAU_multiple_overlaping_window_access     ),

					ADD_TEST(  SMAU_write_invalidate_cache 				  ),
					ADD_TEST(  SMAU_peep_window_secure_nonsecure_access   ),
					ADD_TEST(  SMAU_cipher_window_secure_nonsecure_access ),
					ADD_TEST(  SMAU_stress_cpu_path						  ),
					ADD_TEST(  SMAU_stress_DMA_path 					  ),
					ADD_TEST(  SMAU_stress_test							  ),
					/*ADD_TEST(  SMAU_stress_DMA_ACP_path                   ), */
					ADD_TEST(  SMAU_stress_cache                          ),
					ADD_TEST(  SMAU_multiple_non_overlaping_window_access ),
					ADD_TEST(  SMAU_multiple_overlaping_window_access     ),


				} ;
    uint32_t total_tests = (sizeof(test) / sizeof(test[0]));
    int test_status[total_tests];

    for(loop_count = 0 ; loop_count  < total_tests ; loop_count++ )
    {
    	test_status[loop_count] = TEST_SKIP;
    }

    for(loop_count = 0 ; loop_count  < total_tests ; loop_count++ )
    {
    	post_log("\nExecuting test no : %2d. %s\n",(loop_count + 1),test[loop_count].func_name);
    	result = (*test[loop_count].func)();
    	switch(result)
    	{
    		case TEST_PASS:
    			test_status[loop_count] = TEST_PASS;
    			pass++;
    			break;
    		case TEST_FAIL:
    			test_status[loop_count] = TEST_FAIL;
    			fail++;
    			break;
    		case TEST_SKIP:
    			test_status[loop_count] = TEST_SKIP;
    			skip++;
    			break;
    	}
        for(loop_count1 = 0 ; loop_count1  < total_tests ; loop_count1++ )
        {
        	post_log("\%2d. %s => ",(loop_count1 + 1),test[loop_count1].func_name);
        	if(test_status[loop_count1] == TEST_PASS)
        		post_log("PASS\n");
        	else if(test_status[loop_count1] == TEST_FAIL)
        	{
        		post_log("FAIL\n");
        		result = TEST_FAIL;
        	}
        	else if(test_status[loop_count1] == TEST_SKIP)
        		post_log("SKIP\n");
        }
    }
    for(loop_count = 0 ; loop_count  < total_tests ; loop_count++ )
    {
    	post_log("\%2d. %s => ",(loop_count + 1),test[loop_count].func_name);
    	if(test_status[loop_count] == TEST_PASS)
    		post_log("PASS\n");
    	else if(test_status[loop_count] == TEST_FAIL)
    	{
    		post_log("FAIL\n");
    		result = TEST_FAIL;
    	}
    	else if(test_status[loop_count] == TEST_SKIP)
    		post_log("SKIP\n");
    }
    post_log("Report TOTAL Test => %d - PASS => %d - FAIL => %d - SKIP => %d\n",total_tests, pass,fail,skip);
    return result;
}


#define DDR_AES_ECB_128_XIP			0x61000000
#define DDR_AES_ECB_192_XIP			0x61010000
#define DDR_AES_ECB_256_XIP			0x61020000
#define DDR_PLAIN_TEXT_XIP			0x62000000

#define NAND_AES_ECB_128_XIP		0xE0000000
#define NAND_AES_ECB_192_XIP		0xE0010000
#define NAND_AES_ECB_256_XIP		0xE0020000
#define NAND_PLAIN_TEXT_XIP			0xE0030000

#define QSPI_AES_ECB_128_XIP		0xF0500000
#define QSPI_AES_ECB_192_XIP		0xF0510000
#define QSPI_AES_ECB_256_XIP		0xF0520000
#define QSPI_PLAIN_TEXT_XIP		    0xF0600000

#define DDR_PLAIN_TEXT_INDEX        0x00
#define DDR_XIP_ECB128_INDEX 		0x01
#define DDR_CW_PLAIN_TEXT_INDEX     0x02

#define NAND_PLAIN_TEXT_INDEX       0x03
#define NAND_XIP_ECB128_INDEX 		0x04
#define NAND_CW_PLAIN_TEXT_INDEX    0x05

#define QSPI_PLAIN_TEXT_INDEX       0x06
#define QSPI_XIP_ECB128_INDEX 		0x07
#define QSPI_CW_PLAIN_TEXT_INDEX    0x08

#define ARRAY_INDEX_CALCULATION     0x09

#define XIP_PASS 					0xF0F0F0F0
#define XIP_FAIL 					0xA1A1A1A1
#define DDR_RESULT_LOCATION 		0x9FFFF000

static int SMAU_DDR_xip_configure_aes_ecb(void)
{
	uint8_t common_key0[]     = "CW1_key0_aeskey0_diagnostics_aes";
	uint8_t common_key1[]     = "CW1_key0_aeskey0_diagnostics_aes";

	SMAU_configure_aes_ecb_window(SMAU_DDR_CW0, DDR_AES_ECB_128_XIP, (DDR_AES_ECB_128_XIP + 0xA000), AES_128, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE,  SECURE );
	SMAU_disable_aes_ecb_encryption(SMAU_DDR_CW1, DDR_PLAIN_TEXT_XIP,(DDR_PLAIN_TEXT_XIP + 0xA000), AES_128, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE,  SECURE );
	return TEST_PASS;

}

static int SMAU_NAND_xip_configure_aes_ecb(void)
{
	uint8_t common_key0[]     = "CW1_key0_aeskey0_diagnostics_aes";
	uint8_t common_key1[]     = "CW1_key0_aeskey0_diagnostics_aes";

	SMAU_configure_aes_ecb_window(SMAU_FLASH_CW1, NAND_AES_ECB_128_XIP, (NAND_AES_ECB_128_XIP + 0xA000), AES_128, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE,  SECURE );
	SMAU_disable_aes_ecb_encryption(SMAU_FLASH_CW2, NAND_PLAIN_TEXT_XIP,(NAND_PLAIN_TEXT_XIP + 0xA000), AES_128, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE,  SECURE );
	return TEST_PASS;

}

static int SMAU_QSPI_xip_configure_aes_ecb(void)
{
	uint8_t common_key0[]     = "CW1_key0_aeskey0_diagnostics_aes";
	uint8_t common_key1[]     = "CW1_key0_aeskey0_diagnostics_aes";

	SMAU_configure_aes_ecb_window(SMAU_FLASH_CW1, QSPI_AES_ECB_128_XIP, (QSPI_AES_ECB_128_XIP + 0xA000), AES_128, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE,  SECURE );
	SMAU_disable_aes_ecb_encryption(SMAU_FLASH_CW2, QSPI_PLAIN_TEXT_XIP, (QSPI_PLAIN_TEXT_XIP + 0xA000), AES_128, (uint32_t*)common_key0, (uint32_t*)common_key1, DISABLE,  SECURE );
	return TEST_PASS;

}


uint32_t xip( uint32_t (*function)(void), uint64_t *execution_time )
{
	uint32_t result = 0x00;
	uint64_t time_start = 0x00, timer_end = 0x00;

	//iproc_systick_init(0x0000);
	// Get Start time here
	time_start = get_ticks();
	post_log("time_start = 0x%llX\n",time_start);
	result = function();
	// Get end time here
	timer_end = get_ticks();
	post_log("timer_end = 0x%llX\n",timer_end);
	*execution_time = (timer_end - time_start);

	return result;
}

static int SMAU_XIP_DDR(int number_ofexec)
{
	uint32_t (*function)(void);
	//uint64_t exec_time[5] 	= {0x00,0x00,0x00,0x00,0x00};
	int result 		= TEST_PASS;

	disable_all_window();
	function = (uint32_t(*)(void))DDR_PLAIN_TEXT_XIP;
	post_log("Running Plain text XIP DDR\n");
	if(XIP_PASS != xip(function,&exec_time[DDR_PLAIN_TEXT_INDEX + (number_ofexec * ARRAY_INDEX_CALCULATION)])){
		post_log("error - Plain text XIP DDR - Failed\n");
		 result	= TEST_FAIL;
	}

	SMAU_DDR_xip_configure_aes_ecb();
	function = (uint32_t(*)(void))DDR_AES_ECB_128_XIP;
	post_log("Running Secure XIP DDR AES ECB-128 \n");
	if(XIP_PASS != xip(function,&exec_time[DDR_XIP_ECB128_INDEX + (number_ofexec * ARRAY_INDEX_CALCULATION)])){
		post_log("error - Secure XIP DDR AES ECB-128 - Failed\n");
		 result	= TEST_FAIL;
	}

	function = (uint32_t(*)(void))DDR_PLAIN_TEXT_XIP;
	post_log("Running CW - PLAIN TEXT \n");
	if(XIP_PASS != xip(function,&exec_time[DDR_CW_PLAIN_TEXT_INDEX + (number_ofexec * ARRAY_INDEX_CALCULATION)])){
		post_log("error - CW - PLAIN TEXT - Failed\n");
		 result	= TEST_FAIL;
	}

	post_log("---------------------- DDR-XIP -----------------------\n");
	post_log("| %-20s | %-20llu\n","PLAIN TEXT",exec_time[DDR_PLAIN_TEXT_INDEX + (number_ofexec * ARRAY_INDEX_CALCULATION)]);
	post_log("| %-20s | %-20llu\n","CW AES ECB-128",exec_time[DDR_XIP_ECB128_INDEX + (number_ofexec * ARRAY_INDEX_CALCULATION)]);
	post_log("| %-20s | %-20llu\n","CW PLAIN TEXT",exec_time[DDR_CW_PLAIN_TEXT_INDEX + (number_ofexec * ARRAY_INDEX_CALCULATION)]);
	//post_log("| %20s | 0x%-20X\n","AES ECB-256",exec_time[DDR_XIP_ECB256_INDEX]);
	post_log("------------------------------------------------------\n");

    return result;
}

static int SMAU_XIP_NAND(int number_ofexec)
{
	uint32_t (*function)(void);
	//uint64_t exec_time[5] 	= {0x00,0x00,0x00,0x00,0x00};
	int result 		= TEST_PASS;

	disable_all_window();
	function = (uint32_t(*)(void))NAND_PLAIN_TEXT_XIP;
	post_log("Running Plain text XIP NAND\n");
	if(XIP_PASS != xip(function,&exec_time[NAND_PLAIN_TEXT_INDEX + (number_ofexec * ARRAY_INDEX_CALCULATION)])){
		post_log("error - Plain text XIP NAND - Failed\n");
		 result	= TEST_FAIL;
	}

	SMAU_NAND_xip_configure_aes_ecb();
	function =(uint32_t(*)(void)) NAND_AES_ECB_128_XIP;
	post_log("Running Secure XIP NAND AES ECB-128 \n");
	if(XIP_PASS != xip(function,&exec_time[NAND_XIP_ECB128_INDEX + (number_ofexec * ARRAY_INDEX_CALCULATION)])){
		post_log("error - Secure XIP NAND AES ECB-128 - Failed\n");
		 result	= TEST_FAIL;
	}

	function = (uint32_t(*)(void))NAND_PLAIN_TEXT_XIP;
	post_log("Running CW - PLAIN TEXT \n");
	if(XIP_PASS != xip(function,&exec_time[NAND_CW_PLAIN_TEXT_INDEX + (number_ofexec * ARRAY_INDEX_CALCULATION)])){
		post_log("error - CW - PLAIN TEXT - Failed\n");
		 result	= TEST_FAIL;
	}

	post_log("---------------------- NAND-XIP -----------------------\n");
	post_log("| %-20s | %-20llu\n","PLAIN TEXT",exec_time[NAND_PLAIN_TEXT_INDEX + (number_ofexec * ARRAY_INDEX_CALCULATION)]);
	post_log("| %-20s | %-20llu\n","AES ECB-128",exec_time[NAND_XIP_ECB128_INDEX + (number_ofexec * ARRAY_INDEX_CALCULATION)]);
	post_log("| %-20s | %-20llu\n","CW PLAIN TEXT",exec_time[NAND_CW_PLAIN_TEXT_INDEX + (number_ofexec * ARRAY_INDEX_CALCULATION)]);
	//post_log("| %20s | 0x%-20X\n","AES ECB-256",exec_time[NAND_XIP_ECB256_INDEX]);
	post_log("------------------------------------------------------\n");

    return result;
}


static int SMAU_XIP_QSPI(int number_ofexec)
{
	uint32_t (*function)(void);
	//uint64_t exec_time[5] 	= {0x00,0x00,0x00,0x00,0x00};
	int result 		= TEST_PASS;

	disable_all_window();
	function = (uint32_t(*)(void))QSPI_PLAIN_TEXT_XIP;
	post_log("Running Plain text XIP QSPI\n");
	if(XIP_PASS != xip(function,&exec_time[QSPI_PLAIN_TEXT_INDEX + (number_ofexec * ARRAY_INDEX_CALCULATION)])){
		post_log("error - Plain text XIP QSPI - Failed\n");
		 result	= TEST_FAIL;
	}

	SMAU_QSPI_xip_configure_aes_ecb();
	function = (uint32_t(*)(void))QSPI_AES_ECB_128_XIP;
	post_log("Running Secure XIP QSPI AES ECB-128 \n");
	if(XIP_PASS != xip(function,&exec_time[QSPI_XIP_ECB128_INDEX + (number_ofexec * ARRAY_INDEX_CALCULATION)])){
		post_log("error - Secure XIP QSPI AES ECB-128 - Failed\n");
		 result	= TEST_FAIL;
	}


	function = (uint32_t(*)(void))QSPI_PLAIN_TEXT_XIP;
	post_log("Running CW - PLAIN TEXT \n");
	if(XIP_PASS != xip(function,&exec_time[QSPI_CW_PLAIN_TEXT_INDEX + (number_ofexec * ARRAY_INDEX_CALCULATION)])){
		post_log("error - CW - PLAIN TEXT - Failed\n");
		 result	= TEST_FAIL;
	}

	post_log("---------------------- QSPI-XIP ----------------------\n");
	post_log("| %-20s | %-20llu\n","PLAIN TEXT",exec_time[QSPI_PLAIN_TEXT_INDEX + (number_ofexec * ARRAY_INDEX_CALCULATION)]);
	post_log("| %-20s | %-20llu\n","AES ECB-128",exec_time[QSPI_XIP_ECB128_INDEX + (number_ofexec * ARRAY_INDEX_CALCULATION)]);
	post_log("| %-20s | %-20llu\n","CW PLAIN TEXT",exec_time[QSPI_CW_PLAIN_TEXT_INDEX + (number_ofexec * ARRAY_INDEX_CALCULATION)]);
	//post_log("| %20s | 0x%-20X\n","AES ECB-256",exec_time[QSPI_XIP_ECB256_INDEX]);
	post_log("------------------------------------------------------\n");

    return result;
}
extern void enable_mmu(uint32_t address);
extern void init_pagetable(uint32_t addr);
extern void enable_L1_cache(void);
extern void disable_L1_cache(void);
void enable_acp_aligner(uint32_t bypass_acpal , uint32_t dis_break_burst , uint32_t dis_early_resp);

static int xip_ddr_with_l1(void)
{
	int result = TEST_PASS;

	uint32_t (*function)(void);
	uint64_t exec_t = 0x00;

	/* SMAU config */
	smau_cache_invalidate();
	smau_cache_enable(DISABLE);
	disable_all_window();
	disable_L1_cache();
	post_log("#################### DDR Secure XIP - L1 CACHE Disabled ####################\n");
	function = (uint32_t(*)(void))DDR_PLAIN_TEXT_XIP;
	post_log("Running Plain text XIP DDR\n");
	if(XIP_PASS != xip(function,&exec_t)){
		post_log("error - Plain text XIP DDR - Failed\n");
		 result	= TEST_FAIL;
	}
	post_log("Execution time w/o Cache = %lldmSec\n",exec_t);

	/* L1 Cache/MMU/ACP config */
    enable_L1_cache();
    post_log("L1 Chache enable -> done");

    init_pagetable(0x91000000);
    post_log("Init page table  -> done");

    //Configure the read/write counter for ACP
    wr(IHOST_AXITRACE_ACP_ATM_CONFIG,0x51);
    wr(IHOST_AXITRACE_ACP_ATM_CMD,0x2);//start counter

    enable_mmu(0x91000000);
    enable_acp_aligner(1 /*bypass_acpal*/, 1 /*dis_break_burst*/ , 1 /*dis_early_resp*/);
    enable_dmac_acp(0x1 /*drive_idm_val*/ , 0x2 /*awcache*/, 0x1 /*awuser*/, 0x2 /*arcache*/, 0x1 /*aruser*/);
    wr(CRMU_IOMUX_CTRL7,0x00000220); //Select RGMII pad as input
    wr(CRMU_CHIP_IO_PAD_CONTROL,0x0);

	post_log("#################### DDR Secure XIP - L1 CACHE Enabled ####################\n");
	function =(uint32_t(*)(void)) DDR_PLAIN_TEXT_XIP;
	post_log("Running Plain text XIP DDR\n");
	if(XIP_PASS != xip(function,&exec_t)){
		post_log("error - Plain text XIP DDR - Failed\n");
		 result	= TEST_FAIL;
	}
	post_log("Execution time w Cache = %lldmSec\n",exec_t);
	disable_L1_cache();

	post_log("\n-- Test %s --\n",result == TEST_PASS ? "PASSED" : "FAILED");
	return result;
}


static int xip_ddr(void)
{
	int result = TEST_PASS;
	smau_cache_invalidate();
	smau_cache_enable(DISABLE);
	post_log("#################### DDR Secure XIP - CACHE Disabled ####################\n");
	result += SMAU_XIP_DDR(0);

	smau_cache_enable(ENABLE);
	post_log("#################### DDR Secure XIP - CACHE Enabled - Prefetch Length - 0 ####################\n");
	smau_cache_invalidate();
	smau_prefetch_length(0x00);
	result += SMAU_XIP_DDR(1);

	post_log("#################### DDR Secure XIP - CACHE Enabled - Prefetch Length - 1 ####################\n");
	smau_cache_invalidate();
	smau_prefetch_length(0x01);
	result += SMAU_XIP_DDR(2);

	post_log("#################### DDR Secure XIP - CACHE Enabled - Prefetch Length - 2 ####################\n");
	smau_cache_invalidate();
	smau_prefetch_length(0x02);
	result += SMAU_XIP_DDR(3);

	post_log("#################### DDR Secure XIP - CACHE Enabled - Prefetch Length - 3 ####################\n");
	smau_cache_invalidate();
	smau_prefetch_length(0x03);
	result += SMAU_XIP_DDR(4);

	post_log("#################### DDR Secure XIP - CACHE Enabled - Prefetch Length - 4 ####################\n");
	smau_cache_invalidate();
	smau_prefetch_length(0x04);
	result += SMAU_XIP_DDR(5);

	post_log("#################### DDR Secure XIP - CACHE Enabled - Prefetch Length - 5 ####################\n");
	smau_cache_invalidate();
	smau_prefetch_length(0x05);
	result += SMAU_XIP_DDR(6);

	post_log("#################### DDR Secure XIP - CACHE Enabled - Prefetch Length - 6 ####################\n");
	smau_cache_invalidate();
	smau_prefetch_length(0x06);
	result += SMAU_XIP_DDR(7);

	post_log("#################### DDR Secure XIP - CACHE Enabled - Prefetch Length - 7 ####################\n");
	smau_cache_invalidate();
	smau_prefetch_length(0x07);
	result += SMAU_XIP_DDR(8);

	post_log("\n-- Test %s --\n",result == TEST_PASS ? "PASSED" : "FAILED");
	return result;
}

static int xip_nand(void)
{
	int result = TEST_PASS;
	smau_cache_invalidate();
	smau_cache_enable(DISABLE);
	post_log("#################### NAND Secure XIP - CACHE Disabled ####################\n");
	result += SMAU_XIP_NAND(0);

	smau_cache_enable(ENABLE);

	post_log("#################### NAND Secure XIP - CACHE Enabled - Prefetch Length - 0 ####################\n");
	smau_cache_invalidate();
	smau_prefetch_length(0x00);
	result += SMAU_XIP_NAND(1);

	post_log("#################### NAND Secure XIP - CACHE Enabled - Prefetch Length - 1 ####################\n");
	smau_cache_invalidate();
	smau_prefetch_length(0x01);
	result += SMAU_XIP_NAND(2);

	post_log("#################### NAND Secure XIP - CACHE Enabled - Prefetch Length - 2 ####################\n");
	smau_cache_invalidate();
	smau_prefetch_length(0x02);
	result += SMAU_XIP_NAND(3);

	post_log("#################### NAND Secure XIP - CACHE Enabled - Prefetch Length - 3 ####################\n");
	smau_cache_invalidate();
	smau_prefetch_length(0x03);
	result += SMAU_XIP_NAND(4);

	post_log("#################### NAND Secure XIP - CACHE Enabled - Prefetch Length - 4 ####################\n");
	smau_cache_invalidate();
	smau_prefetch_length(0x04);
	result += SMAU_XIP_NAND(5);

	post_log("#################### NAND Secure XIP - CACHE Enabled - Prefetch Length - 5 ####################\n");
	smau_cache_invalidate();
	smau_prefetch_length(0x05);
	result += SMAU_XIP_NAND(6);

	post_log("#################### NAND Secure XIP - CACHE Enabled - Prefetch Length - 6 ####################\n");
	smau_cache_invalidate();
	smau_prefetch_length(0x06);
	result += SMAU_XIP_NAND(7);

	post_log("#################### NAND Secure XIP - CACHE Enabled - Prefetch Length - 7 ####################\n");
	smau_cache_invalidate();
	smau_prefetch_length(0x07);
	result += SMAU_XIP_NAND(8);

	post_log("\n-- Test %s --\n",result == TEST_PASS ? "PASSED" : "FAILED");
	return result;
}

static int xip_qspi(void)
{
	int result = TEST_PASS;
	smau_cache_invalidate();
	smau_cache_enable(DISABLE);
	post_log("#################### QSPI Secure XIP - CACHE Disabled ####################\n");
	result += SMAU_XIP_QSPI(0);

	smau_cache_enable(ENABLE);

	post_log("#################### QSPI Secure XIP - CACHE Enabled - Prefetch Length - 0 ####################\n");
	smau_cache_invalidate();
	smau_prefetch_length(0x00);
	result += SMAU_XIP_QSPI(1);

	post_log("#################### QSPI Secure XIP - CACHE Enabled - Prefetch Length - 1 ####################\n");
	smau_cache_invalidate();
	smau_prefetch_length(0x01);
	result += SMAU_XIP_QSPI(2);

	post_log("#################### QSPI Secure XIP - CACHE Enabled - Prefetch Length - 2 ####################\n");
	smau_cache_invalidate();
	smau_prefetch_length(0x02);
	result += SMAU_XIP_QSPI(3);

	post_log("#################### QSPI Secure XIP - CACHE Enabled - Prefetch Length - 3 ####################\n");
	smau_cache_invalidate();
	smau_prefetch_length(0x03);
	result += SMAU_XIP_QSPI(4);

	post_log("#################### QSPI Secure XIP - CACHE Enabled - Prefetch Length - 4 ####################\n");
	smau_cache_invalidate();
	smau_prefetch_length(0x04);
	result += SMAU_XIP_QSPI(5);

	post_log("#################### QSPI Secure XIP - CACHE Enabled - Prefetch Length - 5 ####################\n");
	smau_cache_invalidate();
	smau_prefetch_length(0x05);
	result += SMAU_XIP_QSPI(6);

	post_log("#################### QSPI Secure XIP - CACHE Enabled - Prefetch Length - 6 ####################\n");
	smau_cache_invalidate();
	smau_prefetch_length(0x06);
	result += SMAU_XIP_QSPI(7);

	post_log("#################### QSPI Secure XIP - CACHE Enabled - Prefetch Length - 7 ####################\n");
	smau_cache_invalidate();
	smau_prefetch_length(0x07);
	result += SMAU_XIP_QSPI(8);

	post_log("\n-- Test %s --\n",result == TEST_PASS ? "PASSED" : "FAILED");
	return result;
}

static int smau_performance_test(void)
{
	int result = TEST_PASS;
	int loop_counter = 0x00;

	result += xip_ddr();
	result += xip_qspi();
	result += xip_nand();
	post_log("------------------------------------------------------------------------------------------------------------------------------\n");
	post_log("|--- xip type ---|-cache dis-|--prefl=0--|--prefl=1--|--prefl=2--|--prefl=3--|--prefl=4--|--prefl=5--|--prefl=6--|--prefl=7--|\n");
	post_log("-------------------------------------------------- DDR-XIP -------------------------------------------------------------------\n");
	post_log("| %-14s |","PLAIN TEXT");
	for(loop_counter = 0x00 ; loop_counter < 9 ; loop_counter++)
	post_log(" %-9llu |",exec_time[(loop_counter*ARRAY_INDEX_CALCULATION)+DDR_PLAIN_TEXT_INDEX]);

	post_log("\n| %-14s |","CW AES ECB-128");
	for(loop_counter = 0x00 ; loop_counter < 9 ; loop_counter++)
	post_log(" %-9llu |",exec_time[(loop_counter*ARRAY_INDEX_CALCULATION)+DDR_XIP_ECB128_INDEX]);

	post_log("\n| %-14s |","CW PLAIN TEXT");
	for(loop_counter = 0x00 ; loop_counter < 9 ; loop_counter++)
	post_log(" %-9llu |",exec_time[(loop_counter*ARRAY_INDEX_CALCULATION)+DDR_CW_PLAIN_TEXT_INDEX]);
#if 1
	post_log("\n-------------------------------------------------- QSPI-XIP ----------------------------------------------------------------\n");
	//post_log("|--- xip type ---|-cache dis-|--prefl=0--|--prefl=1--|--prefl=2--|--prefl=3--|--prefl=4--|--prefl=5--|--prefl=6--|--prefl=7--|\n");
	//post_log("------------------------------------------------------------------------------------------------------------------\n");
	post_log("| %-14s |","PLAIN TEXT");
	for(loop_counter = 0x00 ; loop_counter < 9 ; loop_counter++)
	post_log(" %-9llu |",exec_time[(loop_counter*ARRAY_INDEX_CALCULATION)+QSPI_PLAIN_TEXT_INDEX]);

	post_log("\n| %-14s |","CW AES ECB-128");
	for(loop_counter = 0x00 ; loop_counter < 9 ; loop_counter++)
	post_log(" %-9llu |",exec_time[(loop_counter*ARRAY_INDEX_CALCULATION)+QSPI_XIP_ECB128_INDEX]);

	post_log("\n| %-14s |","CW PLAIN TEXT");
	for(loop_counter = 0x00 ; loop_counter < 9 ; loop_counter++)
	post_log(" %-9llu |",exec_time[(loop_counter*ARRAY_INDEX_CALCULATION)+QSPI_CW_PLAIN_TEXT_INDEX]);

	post_log("\n-------------------------------------------------- NAND-XIP ----------------------------------------------------------------\n");
	//post_log("|--- xip type ---|-cache dis-|--prefl=1--|--prefl=2--|--prefl=3--|--prefl=4--|--prefl=5--|--prefl=6--|--prefl=7--|\n");
	//post_log("------------------------------------------------------------------------------------------------------------------\n");
	post_log("| %-14s |","PLAIN TEXT");
	for(loop_counter = 0x00 ; loop_counter < 9 ; loop_counter++)
	post_log(" %-9llu |",exec_time[(loop_counter*ARRAY_INDEX_CALCULATION)+NAND_PLAIN_TEXT_INDEX]);

	post_log("\n| %-14s |","CW AES ECB-128");
	for(loop_counter = 0x00 ; loop_counter < 9 ; loop_counter++)
	post_log(" %-9llu |",exec_time[(loop_counter*ARRAY_INDEX_CALCULATION)+NAND_XIP_ECB128_INDEX]);

	post_log("\n| %-14s |","CW PLAIN TEXT");
	for(loop_counter = 0x00 ; loop_counter < 9 ; loop_counter++)
	post_log(" %-9llu |",exec_time[(loop_counter*ARRAY_INDEX_CALCULATION)+NAND_CW_PLAIN_TEXT_INDEX]);
	post_log("\n----------------------------------------------------------------------------------------------------------------------------\n");
#endif
	post_log("\n-- Test %s --\n",result == TEST_PASS ? "PASSED" : "FAILED");
	return result;
}


#define SPUM_PLAIN_TEXT_DDR  0x60050000
#define SPUM_AES_ECB_128_DDR 0x60070000

#define SMAU_PLAIN_TEXT_DDR  0x60090000
#define SMAU_AES_ECB_128_DDR 0x600b0000

#define TOTAL_DATA_SIZE 160 //64 KB Data
extern void *malloc(unsigned int num_bytes);
void free(void*);
static int spum_aes_ecb_128_dec(uint8_t *input, uint8_t *output , uint8_t *encr_key_ecb , uint32_t crypto_len )
{

	uint32_t auth_key_len	= 0;
	uint32_t crypto_offset  = 0;
	uint32_t auth_len  		= 0;
	uint32_t auth_offset	= 0;

	uint8_t *auth_key = NULL;
	BCM_SCAPI_STATUS status;
	bcm_bulk_cipher_context ctx;
	BCM_BULK_CIPHER_CMD *cmd;
	crypto_lib_handle *pHandle;


	cmd = (BCM_BULK_CIPHER_CMD *)malloc(sizeof(BCM_BULK_CIPHER_CMD));
	pHandle  = (crypto_lib_handle *)malloc(BCM_SCAPI_CRYPTO_LIB_HANDLE_SIZE);
    bcm_crypto_init (pHandle);

	cmd->cipher_mode  = BCM_SCAPI_CIPHER_MODE_DECRYPT;    /* encrypt or decrypt */
	cmd->encr_alg     = BCM_SCAPI_ENCR_ALG_AES_128;   /* encryption algorithm */
	cmd->encr_mode    = BCM_SCAPI_ENCR_MODE_ECB;
	cmd->auth_alg     = BCM_SCAPI_AUTH_ALG_NULL;    /* authentication algorithm */
	cmd->auth_mode    = BCM_SCAPI_AUTH_MODE_HASH;
	cmd->auth_optype    = BCM_SCAPI_AUTH_OPTYPE_INIT;
	cmd->cipher_order = BCM_SCAPI_CIPHER_ORDER_NULL;    /* encrypt first or auth first */

	status = bcm_bulk_cipher_init (
				pHandle,
				cmd,
				encr_key_ecb,
				auth_key,
				auth_key_len,
				&ctx);

	if( status != BCM_SCAPI_STATUS_OK){
			post_log("Cipher Init Failed\n");
			return status;
	}
	status = bcm_bulk_cipher_start (
				pHandle,
			    input,
			    crypto_len,
				NULL,
				crypto_len,
				crypto_offset,
				auth_len,
				auth_offset,
				output,
				&ctx);
	/*
    if (status != BCM_SCAPI_STATUS_OK_NEED_DMA) return status;

    post_log("start DMA \n");
	ctx.dmac = &dmac1;
    status = bcm_bulk_cipher_dma_start(pHandle, &ctx, NULL,NULL);//, OPCODE_RD, OPCODE_WR);
	*/
	if(status != BCM_SCAPI_STATUS_OK){
			post_log("Cipher DMA Start Failed\n");
			return status;
	}

	bcm_bulk_cipher_deInit (pHandle, &ctx);
	free(cmd);
	free(pHandle);
	return BCM_SCAPI_STATUS_OK;
}

static int spum_aes_ecb_256_dec(uint8_t *input, uint8_t *output , uint8_t *encr_key_ecb , uint32_t crypto_len )
{
	uint32_t auth_key_len	= 0;
	uint32_t crypto_offset  = 0;
	uint32_t auth_len  		= 0;
	uint32_t auth_offset	= 0;
	uint8_t *auth_key = NULL;
	BCM_SCAPI_STATUS status;
	bcm_bulk_cipher_context ctx;
	BCM_BULK_CIPHER_CMD *cmd;
	crypto_lib_handle *pHandle;


	cmd = (BCM_BULK_CIPHER_CMD *)malloc(sizeof(BCM_BULK_CIPHER_CMD));
	pHandle  = (crypto_lib_handle *)malloc(BCM_SCAPI_CRYPTO_LIB_HANDLE_SIZE);
    bcm_crypto_init (pHandle);

    cmd->cipher_mode  = BCM_SCAPI_CIPHER_MODE_DECRYPT;    /* encrypt or decrypt */
    cmd->encr_alg     = BCM_SCAPI_ENCR_ALG_AES_256;   /* encryption algorithm */
    cmd->encr_mode    = BCM_SCAPI_ENCR_MODE_ECB;
    cmd->auth_alg     = BCM_SCAPI_AUTH_ALG_NULL;    /* authentication algorithm */
    cmd->auth_mode    = BCM_SCAPI_AUTH_MODE_HASH;
    cmd->auth_optype  = BCM_SCAPI_AUTH_OPTYPE_INIT;
    cmd->cipher_order = BCM_SCAPI_CIPHER_ORDER_NULL;    /* encrypt first or auth first */
	status = bcm_bulk_cipher_init (
				pHandle,
				cmd,
				encr_key_ecb,
				auth_key,
				auth_key_len,
				&ctx);

	if( status != BCM_SCAPI_STATUS_OK){
			post_log("Cipher Init Failed\n");
			return status;
	}
	status = bcm_bulk_cipher_start (
				pHandle,
			    input,
			    crypto_len,
				NULL,
				crypto_len,
				crypto_offset,
				auth_len,
				auth_offset,
				output,
				&ctx);

	/*
    if (status != BCM_SCAPI_STATUS_OK_NEED_DMA) return status;
    post_log("start DMA \n");
	ctx.dmac = &dmac1;
    status = bcm_bulk_cipher_dma_start(pHandle, &ctx, NULL,NULL);//, OPCODE_RD, OPCODE_WR);
	*/
	if(status != BCM_SCAPI_STATUS_OK){
			post_log("Cipher DMA Start Failed\n");
			return status;
	}

	bcm_bulk_cipher_deInit (pHandle, &ctx);
	free(cmd);
	free(pHandle);
	return BCM_SCAPI_STATUS_OK;
}

static int spum_aes_ecb_256_enc(uint8_t *input, uint8_t *output , uint8_t *encr_key_ecb , uint32_t crypto_len )
{
	uint32_t auth_key_len 	= 0;
	uint32_t crypto_offset	= 0;
	uint32_t auth_len		= 0;
	uint32_t auth_offset	= 0;
	uint8_t *auth_key = NULL;
	BCM_SCAPI_STATUS status;
	bcm_bulk_cipher_context ctx;
	BCM_BULK_CIPHER_CMD *cmd;
	crypto_lib_handle *pHandle;

	cmd = (BCM_BULK_CIPHER_CMD *)malloc(sizeof(BCM_BULK_CIPHER_CMD));
	pHandle  = (crypto_lib_handle *)malloc(BCM_SCAPI_CRYPTO_LIB_HANDLE_SIZE);
	bcm_crypto_init (pHandle);

	cmd->cipher_mode  = BCM_SCAPI_CIPHER_MODE_ENCRYPT;    /* encrypt or encrypt */
	cmd->encr_alg     = BCM_SCAPI_ENCR_ALG_AES_256;   /* encryption algorithm */
	cmd->encr_mode    = BCM_SCAPI_ENCR_MODE_ECB;
	cmd->auth_alg     = BCM_SCAPI_AUTH_ALG_NULL;    /* authentication algorithm */
	cmd->auth_mode    = BCM_SCAPI_AUTH_MODE_HASH;
	cmd->auth_optype    = BCM_SCAPI_AUTH_OPTYPE_INIT;
	cmd->cipher_order = BCM_SCAPI_CIPHER_ORDER_NULL;    /* encrypt first or auth first */

	status = bcm_bulk_cipher_init (
				pHandle,
				cmd,
				encr_key_ecb,
				auth_key,
				auth_key_len,
				&ctx);

	if( status != BCM_SCAPI_STATUS_OK){
			post_log("Cipher Init Failed\n");
			return status;
	}

	status = bcm_bulk_cipher_start (
				pHandle,
				input,
			    crypto_len,
				NULL,
				crypto_len,
				crypto_offset,
				auth_len,
				auth_offset,
				output,
				&ctx);
/*
    if (status != BCM_SCAPI_STATUS_OK_NEED_DMA) return status;
    post_log("start DMA \n");
	ctx.dmac = &dmac1;
	status = bcm_bulk_cipher_dma_start(pHandle, &ctx, NULL,NULL);//, OPCODE_RD, OPCODE_WR);
*/
	if(status != BCM_SCAPI_STATUS_OK){
			post_log("Cipher DMA Start Failed\n");
			return status;
	}

	bcm_bulk_cipher_deInit (pHandle, &ctx);
	free(cmd);
	free(pHandle);
	return BCM_SCAPI_STATUS_OK;
}


static int spum_aes_ecb_128_enc(uint8_t *input, uint8_t *output , uint8_t *encr_key_ecb , uint32_t crypto_len )
{
	uint32_t auth_key_len;
	uint32_t crypto_offset;
	uint32_t auth_len;
	uint32_t auth_offset;
	uint8_t *auth_key = NULL;
	BCM_SCAPI_STATUS status;
	bcm_bulk_cipher_context ctx;
	BCM_BULK_CIPHER_CMD *cmd;
	crypto_lib_handle *pHandle;


	cmd 		= (BCM_BULK_CIPHER_CMD *)malloc(sizeof(BCM_BULK_CIPHER_CMD));
	pHandle  	= (crypto_lib_handle *)malloc(BCM_SCAPI_CRYPTO_LIB_HANDLE_SIZE);
	bcm_crypto_init(pHandle);

    cmd->cipher_mode  = BCM_SCAPI_CIPHER_MODE_ENCRYPT;    /* encrypt or encrypt */
    cmd->encr_alg     = BCM_SCAPI_ENCR_ALG_AES_128;   /* encryption algorithm */
    cmd->encr_mode    = BCM_SCAPI_ENCR_MODE_ECB;
	cmd->auth_alg     = BCM_SCAPI_AUTH_ALG_NULL;    /* authentication algorithm */
	cmd->auth_mode    = BCM_SCAPI_AUTH_MODE_HASH;
	cmd->auth_optype  = BCM_SCAPI_AUTH_OPTYPE_INIT;
	cmd->cipher_order = BCM_SCAPI_CIPHER_ORDER_NULL;    /* encrypt first or auth first */

	crypto_offset = 0;
	auth_len      = 0;
	auth_offset   = 0;
	auth_key_len  = 0;

	status = bcm_bulk_cipher_init (pHandle,cmd,encr_key_ecb,auth_key,auth_key_len,&ctx);
	if( status != BCM_SCAPI_STATUS_OK)
	{
			post_log("Cipher Init Failed\n");
			return status;
	}

	status = bcm_bulk_cipher_start(pHandle,input,crypto_len,NULL,crypto_len,crypto_offset,auth_len,auth_offset,output,&ctx);

/*
	if (status != BCM_SCAPI_STATUS_OK_NEED_DMA) return status;
        post_log("start DMA \n");
	ctx.dmac = &dmac1;

	status = bcm_bulk_cipher_dma_start(pHandle, &ctx, NULL,NULL);//, OPCODE_RD, OPCODE_WR);
*/
    if(status != BCM_SCAPI_STATUS_OK){
    	post_log("Cipher DMA Start Failed\n");
        return status;
    }

	bcm_bulk_cipher_deInit (pHandle, &ctx);
	free(cmd);
	free(pHandle);
	return BCM_SCAPI_STATUS_OK;
}

static uint32_t init_memory(uint8_t *spum_input , uint8_t *smau_input , uint32_t size)
{
uint32_t loop_count = 0x00;
uint32_t reg_index0 = 0, reg_index1 = 0;
uint32_t DATA1 = 0;
uint32_t dont_use[16];
uint32_t random_number;

bcm_rng_raw_generate (pHandle, (uint8_t  *)dont_use, 16 , NULL, NULL);
bcm_rng_raw_generate (pHandle, (uint8_t  *)dont_use, 16 , NULL, NULL);

bcm_rng_raw_generate (pHandle, (uint8_t *)&random_number, 1 , NULL, NULL);
DATA1 = random_number;

	post_log("Initializing input memory\n");
	for(loop_count = 0 ; loop_count < size ; loop_count += 16)
	{
		for(reg_index0 = 0,reg_index1 =  15 ; reg_index0 < 16 ; reg_index0 += 4, reg_index1 -= 4)
		{
			spum_input[(loop_count) + reg_index0] 	   = (uint8_t) (DATA1 & (0xFF));
			spum_input[(loop_count) + reg_index0 + 1] = (uint8_t) ((DATA1 >> 8)  & (0xFF));
			spum_input[(loop_count) + reg_index0 + 2] = (uint8_t) ((DATA1 >> 16) & (0xFF));
			spum_input[(loop_count) + reg_index0 + 3] = (uint8_t) ((DATA1 >> 24) & (0xFF));

			smau_input[(loop_count) + reg_index1] 	   = (uint8_t) (DATA1 & (0xFF));
			smau_input[(loop_count) + reg_index1 - 1] = (uint8_t) ((DATA1 >> 8)  & (0xFF));
			smau_input[(loop_count) + reg_index1 - 2] = (uint8_t) ((DATA1 >> 16) & (0xFF));
			smau_input[(loop_count) + reg_index1 - 3] = (uint8_t) ((DATA1 >> 24) & (0xFF));
			DATA1 += 1;
		}
		post_log(".");
	}
	post_log("\n");
	return random_number;
}

static void init_spum_memory(uint8_t *spum_input , uint32_t size , uint8_t data)
{
	uint32_t reg_index0 = 0;

	post_log("Initializing input memory\n");
	for(reg_index0 = 0 ; reg_index0 < size ; reg_index0 += 1)
	{
		spum_input[reg_index0] = (uint8_t) data;
		data += 1;
		if(!(reg_index0%16))
			post_log(".");
	}
	post_log("\n");
}

static void copy_bw_spum_and_smau(uint8_t *spum, uint8_t *smau,uint32_t size)
{
	uint32_t loop_count = 0x00;
	int32_t reg_index0 = 0;

	post_log("copying data...\n");
	for(loop_count = 0 ; loop_count < size ; loop_count += 16)
	{
		for(reg_index0 =  15 ; reg_index0 >= 0 ; reg_index0 -= 1)
		{
			smau[(loop_count) + reg_index0] =  spum[(loop_count) + (15 - reg_index0)];
		}
		post_log(".");
	}
	post_log("\n");
}

static void init_smau_memory(uint8_t *smau_input , uint32_t size , uint8_t data)
{
	int32_t reg_index0 = 0;
#if 0
	uint32_t loop_count = 0x00;
#endif


	post_log("Initializing input memory\n");
	for(reg_index0 = 0 ; reg_index0 < size ; reg_index0 += 1)
	{
		smau_input[reg_index0] = (uint8_t) data;
		data += 1;
		if(!(reg_index0%16))
			post_log(".");
	}
	post_log("\n");

#if 0
	for(loop_count = 0 ; loop_count < size ; loop_count += 16)
	{
		for(reg_index0 =  15 ; reg_index0 >= 0 ; reg_index0 -= 1)
		{
			smau_input[(loop_count) + reg_index0] = (uint8_t) data;
			data += 1;
		}
		post_log(".");
	}
	post_log("\n");
#endif
}


static uint32_t data_integrity(uint8_t *spum_input , uint8_t *smau_input , uint32_t size)
{
	uint32_t loop_count = 0x00;
	uint32_t reg_index0 = 0, reg_index1 = 0;
	int result = TEST_PASS;

	post_log("comparing memory\n");
	for(loop_count = 0 ; loop_count < size ; loop_count += 16)
	{
		for(reg_index0 = 0,reg_index1 =  15 ; reg_index0 < 16 ; reg_index0 += 4, reg_index1 -= 4)
		{
			if((spum_input[(loop_count) + reg_index0]) != smau_input[(loop_count) + reg_index1])
			{
				post_log("error : data @ 0x%X = 0x%X , data @ 0x%X = 0x%X\n",(&spum_input[0] + (loop_count) + reg_index0),spum_input[(loop_count) + reg_index0],
				                                                             (&smau_input[0] + (loop_count) + reg_index1),smau_input[(loop_count) + reg_index1]);
				result = TEST_FAIL;
				break;
			}

			if(spum_input[(loop_count) + reg_index0 + 1] != smau_input[(loop_count) + reg_index1 - 1])
			{
				post_log("error : data @ 0x%X = 0x%X , data @ 0x%X = 0x%X\n",(&spum_input[0] + (loop_count) + reg_index0 + 1),spum_input[(loop_count) + reg_index0 + 1],
				                                                             (&smau_input[0] + (loop_count) + reg_index1 - 1),smau_input[(loop_count) + reg_index1 - 1]);
				result = TEST_FAIL;
				break;
			}

			if(spum_input[(loop_count) + reg_index0 + 2] != smau_input[(loop_count) + reg_index1 - 2])
			{
				post_log("error : data @ 0x%X = 0x%X , data @ 0x%X = 0x%X\n",(&spum_input[0] + (loop_count) + reg_index0 + 2),spum_input[(loop_count) + reg_index0 + 2],
				                                                             (&smau_input[0] + (loop_count) + reg_index1 - 2),smau_input[(loop_count) + reg_index1 - 2]);
				result = TEST_FAIL;
				break;
			}

			if(spum_input[(loop_count) + reg_index0 + 3] != smau_input[(loop_count) + reg_index1 - 3])
			{
				post_log("error : data @ 0x%X = 0x%X , data @ 0x%X = 0x%X\n",(&spum_input[0] + (loop_count) + reg_index0 + 3),spum_input[(loop_count) + reg_index0 + 3],
				                                                             (&smau_input[0] + (loop_count) + reg_index1 - 3),smau_input[(loop_count) + reg_index1 - 3]);
				result = TEST_FAIL;
				break;
			}
		}
		post_log(".");
	}
	post_log("\n");
	return result;
}
static int smau_spum_combo_aes_ecb_128(void)
{
	int result = TEST_PASS;
	uint8_t *spum_input  = (uint8_t *)SPUM_PLAIN_TEXT_DDR;;
	uint8_t *smau_input  = (uint8_t *)SMAU_PLAIN_TEXT_DDR;;
	uint8_t *spum_output = (uint8_t *)SPUM_AES_ECB_128_DDR;;
	uint8_t *smau_output = (uint8_t *)SMAU_AES_ECB_128_DDR;;
	uint8_t *key = (uint8_t *)"broadcom spum-sm";
	uint8_t *key_r =(uint8_t *) "ms-mups mocdaorb";
	uint8_t loop_count = 0x00;	
	initDMA(&dmac1);
	init_memory(spum_input , smau_input , TOTAL_DATA_SIZE);

    //AES-ECB 128
	/* Encrypt using SPUM and SMAU then compare*/
	post_log("\n\nTest case1 : Encrypt using SPUM and SMAU and compare the data\n");
	post_log("SPUM Encrypting data... \n");
	spum_aes_ecb_128_enc(spum_input, spum_output , key , TOTAL_DATA_SIZE );

	SMAU_configure_aes_ecb_window(SMAU_DDR_CW2, (SMAU_AES_ECB_128_DDR), (SMAU_AES_ECB_128_DDR + 0x10000), AES_128, (uint32_t*)key_r, (uint32_t*)key_r, DISABLE,  SECURE );
	post_log("SMAU Encrypting data... \n");
	for(loop_count = 0 ; loop_count < TOTAL_DATA_SIZE ; loop_count++)
		smau_output[loop_count] = smau_input[loop_count];
	SMAU_disable_window(SMAU_DDR_CW2);
	result = data_integrity(spum_output , smau_output , TOTAL_DATA_SIZE);

	/* Encrypt using SPUM and decrypt using SMAU */
	post_log("\n\nTest case2 : Encrypt using SPUM and Decrypt using SMAU\n");
	init_spum_memory(spum_input , TOTAL_DATA_SIZE , 0x05);
	spum_aes_ecb_128_enc(spum_input, spum_output , key , TOTAL_DATA_SIZE );
	copy_bw_spum_and_smau(spum_output,smau_output,TOTAL_DATA_SIZE);
	SMAU_configure_aes_ecb_window(SMAU_DDR_CW2, (SMAU_AES_ECB_128_DDR), (SMAU_AES_ECB_128_DDR), AES_128, (uint32_t*)key_r, (uint32_t*)key_r, DISABLE,  SECURE );
	result = data_integrity(spum_input , smau_output , TOTAL_DATA_SIZE);
	SMAU_disable_window(SMAU_DDR_CW2);

	/* Encrypt using SMAU and decrypt using SPUM */
#if 0
	init_spum_memory(spum_input , TOTAL_DATA_SIZE , 0x05);
	for(loop_count = 0 ; loop_count < TOTAL_DATA_SIZE ; loop_count++)
		post_log("spum_input[%d] = 0x%X\n",loop_count,spum_input[loop_count]);

	spum_aes_ecb_128_enc(spum_input, spum_output , key , TOTAL_DATA_SIZE );

	for(loop_count = 0 ; loop_count < TOTAL_DATA_SIZE ; loop_count++)
		post_log("spum_output[%d] = 0x%X\n",loop_count,spum_output[loop_count]);

	spum_aes_ecb_128_dec(spum_output, smau_output , key , TOTAL_DATA_SIZE );

	for(loop_count = 0 ; loop_count < TOTAL_DATA_SIZE ; loop_count++)
		post_log("smau_output[%d] = 0x%X\n",loop_count,smau_output[loop_count]);
#endif

#if 1
	post_log("\n\nTest case3 : Encrypt using SMAU and Decrypt using SPUM\n");
	init_smau_memory(smau_input , TOTAL_DATA_SIZE , 0x0A);
	SMAU_configure_aes_ecb_window(SMAU_DDR_CW2, (SMAU_AES_ECB_128_DDR), (SMAU_AES_ECB_128_DDR + 0x1000), AES_128, (uint32_t*)key_r, (uint32_t*)key_r, DISABLE,  SECURE );
	for(loop_count = 0 ; loop_count < TOTAL_DATA_SIZE ; loop_count++)
		smau_output[loop_count] = smau_input[loop_count];
	SMAU_disable_window(SMAU_DDR_CW2);
	copy_bw_spum_and_smau(smau_output,spum_input,TOTAL_DATA_SIZE);
	spum_aes_ecb_128_dec(spum_input, spum_output , key , TOTAL_DATA_SIZE );
	result = data_integrity(spum_output , smau_input , TOTAL_DATA_SIZE);
#endif
	post_log("\n-- Test %s --\n",result == TEST_PASS ? "PASSED" : "FAILED");
	return result;
}

static int smau_spum_combo_aes_ecb_256(void)
{
	int result = TEST_PASS;
	uint8_t *spum_input  = (uint8_t *)SPUM_PLAIN_TEXT_DDR;;
	uint8_t *smau_input  = (uint8_t *)SMAU_PLAIN_TEXT_DDR;;
	uint8_t *spum_output = (uint8_t *)SPUM_AES_ECB_128_DDR;;
	uint8_t *smau_output = (uint8_t *)SMAU_AES_ECB_128_DDR;;
	uint8_t *key = (uint8_t *)"broadcom spum-smau combo testing";
	uint8_t *key_r = (uint8_t *)"gnitset obmoc uams-mups mocdaorb";
	uint8_t loop_count = 0x00;

	initDMA(&dmac1);
	init_memory(spum_input , smau_input , TOTAL_DATA_SIZE);

    //AES-ECB 128
	/* Encrypt using SPUM and SMAU then compare*/
	post_log("\n\nTest case1 : Encrypt using SPUM and SMAU and compare the data\n");
	post_log("SPUM Encrypting data... \n");
	spum_aes_ecb_256_enc(spum_input, spum_output , key , TOTAL_DATA_SIZE );

	SMAU_configure_aes_ecb_window(SMAU_DDR_CW2, (SMAU_AES_ECB_128_DDR), (SMAU_AES_ECB_128_DDR + 0x10000), AES_256, (uint32_t*)key_r, (uint32_t*)key_r, DISABLE,  SECURE );
	post_log("SMAU Encrypting data... \n");
	for(loop_count = 0 ; loop_count < TOTAL_DATA_SIZE ; loop_count++)
		smau_output[loop_count] = smau_input[loop_count];
	SMAU_disable_window(SMAU_DDR_CW2);
	result = data_integrity(spum_output , smau_output , TOTAL_DATA_SIZE);

	/* Encrypt using SPUM and decrypt using SMAU */
	post_log("\n\nTest case2 : Encrypt using SPUM and Decrypt using SMAU\n");
	init_spum_memory(spum_input , TOTAL_DATA_SIZE , 0x05);
	spum_aes_ecb_256_enc(spum_input, spum_output , key , TOTAL_DATA_SIZE );
	copy_bw_spum_and_smau(spum_output,smau_output,TOTAL_DATA_SIZE);
	SMAU_configure_aes_ecb_window(SMAU_DDR_CW2, (SMAU_AES_ECB_128_DDR), (SMAU_AES_ECB_128_DDR), AES_256, (uint32_t*)key_r, (uint32_t*)key_r, DISABLE,  SECURE );
	result = data_integrity(spum_input , smau_output , TOTAL_DATA_SIZE);
	SMAU_disable_window(SMAU_DDR_CW2);

#if 1
	post_log("\n\nTest case3 : Encrypt using SMAU and Decrypt using SPUM\n");
	init_smau_memory(smau_input , TOTAL_DATA_SIZE , 0x0A);
	SMAU_configure_aes_ecb_window(SMAU_DDR_CW2, (SMAU_AES_ECB_128_DDR), (SMAU_AES_ECB_128_DDR + 0x1000), AES_256, (uint32_t*)key_r, (uint32_t*)key_r, DISABLE,  SECURE );
	for(loop_count = 0 ; loop_count < TOTAL_DATA_SIZE ; loop_count++)
		smau_output[loop_count] = smau_input[loop_count];
	SMAU_disable_window(SMAU_DDR_CW2);
	copy_bw_spum_and_smau(smau_output,spum_input,TOTAL_DATA_SIZE);
	spum_aes_ecb_256_dec(spum_input, spum_output , key , TOTAL_DATA_SIZE );
	result = data_integrity(spum_output , smau_input , TOTAL_DATA_SIZE);
#endif
	post_log("\n-- Test %s --\n",result == TEST_PASS ? "PASSED" : "FAILED");
	return result;
}


#if 0
	uint32_t reg_index0 = 0x00;
	uint32_t reg_index1 = 0x00;
	uint32_t DATA1 = 0x00;
static int check_spum()
{
	int result = TEST_PASS;
	uint8_t *spum_input  = (uint8_t *)SPUM_PLAIN_TEXT_DDR;;
	uint8_t *smau_input  = (uint8_t *)SMAU_PLAIN_TEXT_DDR;;
	uint8_t *spum_output = (uint8_t *)SPUM_AES_ECB_128_DDR;;
	uint8_t *smau_output = (uint8_t *)SMAU_AES_ECB_128_DDR;;
	uint8_t *key = "broadcom spum-sm";
	uint8_t loop_count = 0x00;
	uint32_t reg_index0 = 0x00;
	uint32_t reg_index1 = 0x00;
	uint8_t DATA1 = 0;

	initDMA(&dmac1);

		post_log("Initializing input memory\n");
			for(reg_index0 = 0 ; reg_index0 < 16 ; reg_index0 += 1)
			{
				spum_input[reg_index0] 	  	= (uint8_t)(DATA1);
				smau_input[reg_index0] 		= (uint8_t)(DATA1);
				DATA1 += 1;
			}
			post_log("Display Actual data\n");
			for(reg_index0 = 0 ; reg_index0 < 16 ; reg_index0 += 1)
			{
				post_log("SPUM[%d] = 0x%X\n",reg_index0,spum_input[reg_index0]);
			}

			post_log("\n");

			for(reg_index0 = 0 ; reg_index0 < 16 ; reg_index0 += 1)
			{
				post_log("SMAU[%d] = 0x%X\n",reg_index0,smau_input[reg_index0]);
			}
    //AES-ECB 128
	/* Encrypt using SPUM and SMAU then compare*/
	post_log("SPUM Encrypting data... \n");
	spum_aes_ecb_128(spum_input, spum_output , key , 16 );

	SMAU_configure_aes_ecb_window(SMAU_DDR_CW2, (SMAU_AES_ECB_128_DDR), (SMAU_AES_ECB_128_DDR + 0x10000), AES_128, (uint32_t*)key, (uint32_t*)key, DISABLE,  SECURE );
	post_log("SMAU Encrypting data... \n");
	for(loop_count = 0 ; loop_count < 16 ; loop_count++)
		smau_output[loop_count] = smau_input[loop_count];
	SMAU_disable_window(SMAU_DDR_CW2);

	post_log("Display encrypted data\n");
	for(reg_index0 = 0 ; reg_index0 < 16 ; reg_index0 += 1)
	{
		post_log("SPUM[%d] = 0x%X\n",reg_index0,spum_output[reg_index0]);
	}

	post_log("\n");

	for(reg_index0 = 0 ; reg_index0 < 16 ; reg_index0 += 1)
	{
		post_log("SMAU[%d] = 0x%X\n",reg_index0,smau_output[reg_index0]);
	}

	wr(SPUM_APB_REGS_CTRL,((0x01 << SPUM_APB_REGS_CTRL__in_endian) | (0x01 << SPUM_APB_REGS_CTRL__out_endian)));
	post_log("SPUM_APB_REGS_CTRL = 0x%X\n",rd(SPUM_APB_REGS_CTRL));
	post_log("SPUM Encrypting data... \n");
	spum_aes_ecb_128(spum_input, spum_output , key , 16 );

	post_log("Display encrypted data\n");
	for(reg_index0 = 0 ; reg_index0 < 16 ; reg_index0 += 1)
	{
		post_log("SPUM[%d] = 0x%X\n",reg_index0,spum_output[reg_index0]);
	}

	post_log("\n");

	for(reg_index0 = 0 ; reg_index0 < 16 ; reg_index0 += 1)
	{
		post_log("SMAU[%d] = 0x%X\n",reg_index0,smau_output[reg_index0]);
	}

	post_log("\n-- Test %s --\n",result == TEST_PASS ? "PASSED" : "FAILED");
	return result;
	/* Encrypt using SMAU and decrypt using SPUM */
}
#endif
/*==================================================================================================
*Function    : SMAU_post_test(int flags, post_result *t_result)
*Parameter   : int flags, post_result *t_result
*Description : run all registered test for normal execution and regression 
*Return      : TEST_PASS/TEST_FAIL
====================================================================================================*/
//int SMAU_post_test(int flags, post_result *t_result)
extern BCM_SCAPI_STATUS bcm_rng_fips_init (
                    crypto_lib_handle *pHandle, fips_rng_context *rngctx, uint32_t bSkipGen);

int SMAU_post_test(int flags)
{
    uint32_t test_no;
    int loop_count = 0, total_tests = 0;
    int status = 0,autorun_status = 0,pass = 0,fail = 0,skip = 0;

    printf("Enter SMAU post test\n");

//    asm("swi 1"); /*to do : This is to enter in to Supervisor mode for smc */
	struct DmuBlockEnable dbe;
	dbe.crypto = 1;
	bcm_dmu_block_enable(dbe);

    printf("after DMU enable\n");

	FUNC_INFO function[]=	{
#ifdef CONFIG_HAVE_DDR
					ADD_TEST(  SMAU_ddr_rw_plaintext    		  ),
					ADD_TEST(  SMAU_ddr_rw_aes_ecb_128  		  ),
					ADD_TEST(  SMAU_ddr_rw_aes_ecb_192  		  ),
					ADD_TEST(  SMAU_ddr_rw_aes_ecb_256  		  ),
					ADD_TEST(  SMAU_ddr_rw_aes_ctr_128  		  ),
					ADD_TEST(  SMAU_ddr_rw_aes_ctr_192  		  ),
					ADD_TEST(  SMAU_ddr_rw_aes_ctr_256  		  ),
					ADD_TEST(  SMAU_ddr_rw_aes_ctr_128_rand  	  ),
					ADD_TEST(  SMAU_ddr_rw_aes_ctr_192_rand  	  ),
					ADD_TEST(  SMAU_ddr_rw_aes_ctr_256_rand  	  ),
					ADD_TEST(  SMAU_ddr_rw_aes_ecb_128_sha  	  ),
					ADD_TEST(  SMAU_ddr_rw_aes_ecb_192_sha  	  ),
					ADD_TEST(  SMAU_ddr_rw_aes_ecb_256_sha  	  ),
					ADD_TEST(  SMAU_ddr_rw_aes_ecb_128_cache_ctrl ),
	                ADD_TEST(  SMAU_ddr_rw_trust_zone 	    	  ),
					ADD_TEST(  SMAU_ddr_config_aes_ecb_128 		  ),
					ADD_TEST(  SMAU_ddr_disable_aes_ecb_window	  ),
#endif
					ADD_TEST(  SMAU_qspi_config_aes_ecb_128 	  ),
					ADD_TEST(  SMAU_nand_config_aes_ecb_128 	  ),
					ADD_TEST(  SMAU_qspi_disable_aes_ecb_window	  ),
					ADD_TEST(  SMAU_nand_disable_aes_ecb_window	  ),
#ifdef CONFIG_HAVE_DDR
					ADD_TEST(  SMAU_ddr_peep_window			      ),
					ADD_TEST(  SMAU_multiple_non_overlaping_window_access ),
					ADD_TEST(  SMAU_multiple_overlaping_window_access     ),
					ADD_TEST(  SMAU_peep_window_secure_nonsecure_access   ),
					ADD_TEST(  SMAU_cipher_window_secure_nonsecure_access ),
					ADD_TEST(  SMAU_write_invalidate_cache ),
					ADD_TEST(  SMAU_stress_cpu_path	 ),
					ADD_TEST(  SMAU_all_stress_test	 ),
					ADD_TEST(  SMAU_stress_DMA_path  ),
					ADD_TEST(  SMAU_stress_test	 ),
					ADD_TEST(  SMAU_stress_DMA_ACP_path ),
					ADD_TEST(  SMAU_stress_cache        ),
					ADD_TEST(  xip_ddr		),
#endif
					ADD_TEST(  xip_nand ),
					ADD_TEST(  xip_qspi  ),
#ifdef CONFIG_HAVE_DDR
					ADD_TEST(  smau_performance_test   ),
					ADD_TEST(  smau_spum_combo_aes_ecb_128 ),
					ADD_TEST(  xip_ddr_with_l1		),
					ADD_TEST(  smau_spum_combo_aes_ecb_256	 ),
#endif
					ADD_TEST(  SMAU_reg_read          )
					//ADD_TEST(  check_spum)

				} ;   

    pHandle  = (crypto_lib_handle *)malloc(BCM_SCAPI_CRYPTO_LIB_HANDLE_SIZE);
    bcm_crypto_init (pHandle);
    bcm_rng_fips_init (pHandle, &rngctx, 1);
    total_tests = (sizeof(function) / sizeof(function[0]));

    post_log("Number of test : %d",total_tests);

    do{
         for(loop_count = 0 ; loop_count  < total_tests ; loop_count++ )
         post_log("\n%2d %s",(loop_count + 1), function[loop_count].func_name);
         post_log("\n%2d run all tests",(total_tests + 1));
         post_log("\n%2d exit\n",(total_tests + 2));
         test_no = post_getUserInput ("\nEnter Test Number : ");

         if(test_no > (total_tests + 2) )
             post_log("ERROR - invalid entry\n");  
         else{
                  if(test_no == (total_tests + 1)){
                      autorun_status = 0;
                      pass = 0;
                      fail = 0;
                      skip = 0;
                      for(loop_count = 0 ; loop_count  < total_tests ; loop_count++ ){
                    	  post_log("\nExecuting test no : %2d. %s\n",(loop_count + 1),function[loop_count].func_name);
                    	  autorun_status = (*function[loop_count].func)();
                    	  switch(autorun_status)
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
                      }
                      post_log("Report TOTAL Test => %d - PASS => %d - FAIL => %d - SKIP => %d\n",(pass + fail + skip), pass,fail,skip);
                      if(fail)
                        status = -1;
                  } 
     
                  else if(test_no < (total_tests + 1)){
          	      if(TEST_FAIL == (*function[test_no - 1].func)())
	       	      status = -1;
                  }
             }

    }while(test_no != (total_tests + 2));

    return status;
}

#endif
#endif

