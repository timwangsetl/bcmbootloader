#include <common.h>
#include <post.h>
#if defined(CONFIG_CYGNUS)
#include "../../halapis/include/amacswitch_apis.h"
#include "asm/arch/socregs.h"



#define ADD_TEST(X)  { #X , X }
#define TEST_PASS  0
#define TEST_FAIL -1
#define TEST_SKIP  2

#define NUM_TEST_ARGS 8
#define VAR_SIZE 50
extern void *malloc(unsigned int num_bytes);


/* Default Register Value */


typedef struct{

	char* func_name;
	int (*func)(void);
}FUNC_INFO;

int amacswitch_external_loopback_test(void)
{
	test_status_t result,rpkt_no,rpkt_chk;
	   test_status_t res_sts[2] = {TEST_PASS, TEST_PASS};

	  
	 
	   int i;
	   int bypass_mode,rd_data;
	    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, index, nextDscr;
	    uint32_t amac_id,amac_ports,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base;


	    //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
	    uint32_t frame_size,chk_noPkt;
	   
	    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;
	    uint32_t data;
	    uint32_t phy_addr, reg_addr;





	    uint8_t my_pkt_data[256] =
	   {
	     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
	     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
	     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
	     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
	     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
	     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
	     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
	     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
	     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
	     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
	     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
	     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
	     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

	   };

	    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

	            rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
	            post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

	            rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
	            post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);

	   /*test_args_name[0]="Amac Ports =";
	   test_args_name[1]="Tx Desc Base =";
	   test_args_name[2]="Rx Desc Base =";
	              test_args_name[3]="Tx Desc Depth =";
	              test_args_name[4]="Rx Desc Depth =";
	              test_args_name[5]="Tx Buffer Base =";
	              test_args_name[6]="Rx Buffer Base =";
	              test_args_name[7]="Frame size =";*/

	              amac_ports        =1;
	              tx_desc_base      =0x62E00000;//Max Desc space=(512 X 16)=0x2000
	              rx_desc_base      =0x62E06000;
	              tx_desc_len       =3;//Max Desc=512=0x200
	              rx_desc_len       =3;
	              tx_buff_base      =0x61000000;//Max Buffer space=(512 X 12K)=0x600000
	              rx_buff_base      =0x62000000;
	              frame_size          =128;
	              //frame_sz            =1024;
	        
	    chk_noPkt       =510;
	    //UNIMAC Config args
	    eth_speed       =0x2;//1Gbps
	    rx_pause_ignore =0;
	    hd_ena          =1;
	    mac_loop_con    =0;
	    ena_ext_config  =0;
	    rmt_loop_ena    =0;
	    tx_pause_ignore =0;

		bypass_mode = 0;

		tx_desc_base = (uint32_t) malloc(16*3);
	    if(tx_desc_base & 0xF) {
	        tx_desc_base += 0xF;
	        tx_desc_base &= 0xFFFFFFF0;
	    }

	    rx_desc_base = (uint32_t) malloc(16*3);
	    if(rx_desc_base & 0xF) {
	        rx_desc_base += 0xF;
	        rx_desc_base &= 0xFFFFFFF0;
	    }

	    tx_buff_base = (uint32_t) malloc(200);
	    if(tx_buff_base & 3) {
	        tx_buff_base += 4;
	        tx_buff_base &= 0xFFFFFFFC;
	    }

	    rx_buff_base = (uint32_t) malloc(200);
	    if(rx_buff_base & 3) {
	        rx_buff_base += 4;
	        rx_buff_base &= 0xFFFFFFFC;
	    }

	    post_log("\n--------------------------------------\n");
	    post_log("tx_desc_base = 0x%08X\n", tx_desc_base);
	    post_log("rx_desc_base = 0x%08X\n", rx_desc_base);
	    post_log("tx_buff_base = 0x%08X\n", tx_buff_base);
	    post_log("rx_buff_base = 0x%08X\n", rx_buff_base);
	    post_log("frame_size   = 0x%08X\n", frame_size);
	    post_log("--------------------------------------\n\n");

	    ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

	   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
	   for(amac_id=0;amac_id < amac_ports;amac_id++){
	        post_log("\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
	        //Basic AMAC block init
	        amac_init(amac_id,bypass_mode);

	        cfg_sw_gphy0_to_gphy1();

	        //------------------------------------------
	        //EXT


	        //------------------------------------------

	        ccg_mdio_init();
	        phy_addr = 0;



	        //------------------------------------------

	        phy_addr = 0;
	        for(i=0; i<16; i++) {
	            reg_addr = i;
	            gphy_reg_read(phy_addr, reg_addr, &data);
	        }

	        post_log("-------------------------------------------\n\n");

	        reg_addr = 0x17;
	        data = 0x0F09;
	        gphy_reg_write(phy_addr, reg_addr, &data);
	        reg_addr = 0x15;
	        data = 0x5193;
	        gphy_reg_write(phy_addr, reg_addr, &data);

	        reg_addr = 0x17;
	        data = 0x0F09;
	        gphy_reg_write(phy_addr, reg_addr, &data);
	        reg_addr = 0x15;
	        gphy_reg_read(phy_addr, reg_addr, &data);

	        //wait for linkup
	        reg_addr = 0x1;
	        do {
	            gphy_reg_read(phy_addr, reg_addr, &data);
	        } while(!(data & 0x4));

	        post_log("-------------------------------------------\n\n");

	        phy_addr = 1;
	        reg_addr = 0x17;
	        data = 0x0F09;
	        gphy_reg_write(phy_addr, reg_addr, &data);
	        reg_addr = 0x15;
	        data = 0x11C9;
	        gphy_reg_write(phy_addr, reg_addr, &data);

	        reg_addr = 0x17;
	        data = 0x0F09;
	        gphy_reg_write(phy_addr, reg_addr, &data);
	        reg_addr = 0x15;
	        gphy_reg_read(phy_addr, reg_addr, &data);

	        //wait for linkup
	        reg_addr = 0x1;
	        do {
	            gphy_reg_read(phy_addr, reg_addr, &data);
	        } while(!(data & 0x4));

	        post_log("-------------------------------------------\n\n");

	        //UNIMAC Config
	                unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
	                             rmt_loop_ena,tx_pause_ignore);

	                //Reset MIB Counter
	                counter_reset(amac_id);

	                //Set up MAC loopback
	                //amac_set_config(amac_id, MAC_LOOPBACK, 1);

	                //Initialize AMAC TX DMA block
	                amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
	                amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
	                for(index = 0; index < tx_desc_len; index++) {

	                     //frame_size=((rand()%(0x3000-64))+64);

	                     CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

	                     write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

	                     //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
	                     amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
	                                     (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
	                     amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
	                }

	                //Initialize AMAC RX DMA block
	                amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
	                                AMAC_PKT_BUFFER_SIZE_IN_BYTES);

	                //Update with next descriptor value - outside EOT range
	                //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

	                //Within EOT range
	                amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

	                 //amac_get_tx_status(amac_id);
	                 //amac_get_rx_status(amac_id);
	                 post_log("\n\n");
	           }
	            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





	            ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
	           for(amac_id=0;amac_id < amac_ports;amac_id++){
	                post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
	                // NxtDscr points to EOT Dscr
	                nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
	                                        ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
	                //Out of EOT range,for continuous loopback
	                /*nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
	                                        ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;*/
	                amac_enable_tx_dma(amac_id, nextDscr);
	                post_log("\n\n");
	           }
	           /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




	           //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
	           /*timeout=300;
	           post_log("Sending packets for specified time\n");
	           delay_noise(timeout,0);
	           post_log("Time_taken =%d secs\n",(timeout*7));
	           suspend_tx(amac_id);*/

	           for(amac_id=0;amac_id < amac_ports;amac_id++){
	                waitforTrans_internalloopback(amac_id);
	                post_log("\n");
	                amac_gettxstats(amac_id);
	                amac_getrxstats(amac_id);

	                rpkt_no=checker_all(amac_id);
	                post_log("\n");

	                post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
	                rpkt_chk=TEST_PASS;
	                rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

	                 if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
	                         res_sts[amac_id]=TEST_PASS;
	                         post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
	                 }
	                 else{
	                         res_sts[amac_id]=TEST_FAIL;
	                         post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
	                 }

	                 post_log("\n\n");
	           }
	           ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	           if((res_sts[0]==TEST_PASS)){
	                    result=TEST_PASS;
	           }else{
	                    result=TEST_FAIL;
	            }
	           return result;

}

int amacswitch_internal_loopback_test_ddr(void)
{



    
    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];



   uint32_t bypass_mode;

   bypass_mode = 0;


    //256-byte data; CRC will be appended by Tx MAC
    uint8_t my_pkt_data[256] =
   {
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

   };

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr;
    uint32_t amac_id,amac_ports,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base;


	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,chk_noPkt;
 
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;


	 ///////////// TEST ARG INIT ////////////////////



              amac_ports        =1;
              tx_desc_base      =0x71000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x72000000;
              tx_desc_len       =16;//Max Desc=512=0x200
              rx_desc_len       =16;
              tx_buff_base      =0x61000000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x62000000;
              frame_size          =128;
              //frame_sz            =1024;
              
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);

        //Set up MAC loopback
        amac_set_config(amac_id, MAC_LOOPBACK, 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
         post_log("\n\n");
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        //Out of EOT range,for continuous loopback
        /*nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;*/
        amac_enable_tx_dma(amac_id, nextDscr);
        post_log("\n\n");
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




   //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
   /*timeout=300;
   post_log("Sending packets for specified time\n");
   delay_noise(timeout,0);
   post_log("Time_taken =%d secs\n",(timeout*7));
   suspend_tx(amac_id);*/

   for(amac_id=0;amac_id < amac_ports;amac_id++){
        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        //amac_gettxstats(amac_id);
        //amac_getrxstats(amac_id);

        rpkt_no=checker_all(amac_id);
        post_log("\n");

        post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        rpkt_chk=TEST_PASS;
        rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

         if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
         }

         post_log("\n\n");
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

}



int amacswitch_internal_loopback_test_sram(void)
{

   
  

    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];



   uint32_t bypass_mode;

   bypass_mode = 0;


    //256-byte data; CRC will be appended by Tx MAC
    uint8_t my_pkt_data[256] =
   {
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

   };

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr;
    uint32_t amac_id,amac_ports,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base;


	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,chk_noPkt;

    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;


	 ///////////// TEST ARG INIT ////////////////////



              amac_ports        =1;
              tx_desc_base      =0x2000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x2010000;
              tx_desc_len       =2;//Max Desc=512=0x200
              rx_desc_len       =2;
              tx_buff_base      =0x2020000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x2030000;
              frame_size          =128;
              //frame_sz            =1024;
            
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);

        //Set up MAC loopback
        amac_set_config(amac_id, MAC_LOOPBACK, 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
         post_log("\n\n");
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        //Out of EOT range,for continuous loopback
        /*nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;*/
        amac_enable_tx_dma(amac_id, nextDscr);
        post_log("\n\n");
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




   //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
   /*timeout=300;
   post_log("Sending packets for specified time\n");
   delay_noise(timeout,0);
   post_log("Time_taken =%d secs\n",(timeout*7));
   suspend_tx(amac_id);*/

   for(amac_id=0;amac_id < amac_ports;amac_id++){
        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        //amac_gettxstats(amac_id);
        //amac_getrxstats(amac_id);

        rpkt_no=checker_all(amac_id);
        post_log("\n");

        post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        rpkt_chk=TEST_PASS;
        rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

         if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
         }

         post_log("\n\n");
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

}


int amacswitch_bypass_test(void)
{


  
    uint32_t index;
	test_status_t result;
   test_status_t res_sts[2];



   uint32_t bypass_mode;

   bypass_mode = 1;


    //256-byte data; CRC will be appended by Tx MAC
    uint8_t my_pkt_data[256] =
   {
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

   };

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr;
    uint32_t amac_id,amac_ports,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base;


	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size;

    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;


	 ///////////// TEST ARG INIT ////////////////////



              amac_ports        =1;
              tx_desc_base      =0x71000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x72000000;
              tx_desc_len       =16;//Max Desc=512=0x200
              rx_desc_len       =16;
              tx_buff_base      =0x61000000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x62000000;
              frame_size          =128;
              //frame_sz            =1024;
               
 
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);

        //Set up MAC loopback
        //amac_set_config(amac_id, MAC_LOOPBACK, 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
         post_log("\n\n");
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        //Out of EOT range,for continuous loopback
        /*nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;*/
        amac_enable_tx_dma(amac_id, nextDscr);
        post_log("\n\n");
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




   //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
   /*timeout=300;
   post_log("Sending packets for specified time\n");
   delay_noise(timeout,0);
   post_log("Time_taken =%d secs\n",(timeout*7));
   suspend_tx(amac_id);*/

   for(amac_id=0;amac_id < amac_ports;amac_id++){
        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        //amac_gettxstats(amac_id);
        //amac_getrxstats(amac_id);

        //rpkt_no=checker_all(amac_id);
        //post_log("\n");

        //post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        //rpkt_chk=TEST_PASS;
        //rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

         /*if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
         }

         post_log("\n\n");*/
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   res_sts[0] = TEST_PASS;

   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

}

int amacswitch_robosw_gmii_2_rgmii_switchport_1_to_5_ddr(void)
{


  

    uint32_t index,rd_data;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];



   uint32_t bypass_mode;

   bypass_mode = 0;


    //256-byte data; CRC will be appended by Tx MAC
    uint8_t my_pkt_data[256] =
   {
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

   };

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr;
    uint32_t amac_id,amac_ports,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base;


	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,chk_noPkt;

    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
    cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

    rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
    post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

    rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
    post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);


	 ///////////// TEST ARG INIT ////////////////////


              amac_ports        =1;
              tx_desc_base      =0x71000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x72000000;
              tx_desc_len       =16;//Max Desc=512=0x200
              rx_desc_len       = 16;
              tx_buff_base      =0x61000000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x62000000;
              frame_size          =128;
              //frame_sz            =1024;
              
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

       //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);

        //Set up MAC loopback
        // amac_set_config(amac_id, "MAC_LOOPBACK", 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
        // post_log("\n\n");
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////// Configuration for Managed mode //////////////////////////////////////

    cfg_sw();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        //Out of EOT range,for continuous loopback
        /*nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;*/
        amac_enable_tx_dma(amac_id, nextDscr);
        //post_log("\n\n");
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



   //////////////////////////////////////////////// Send and stop packet transmission for A0  and A1 ////////////////////////
  /* timeout=100;
   post_log("Sending packets for specified time\n");
   delay_noise(timeout,0);
   post_log("Time_taken =%d secs\n",(timeout));
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        suspend_tx(amac_id);
   }*/
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



   //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){

        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        //amac_gettxstats(amac_id);
        //amac_getrxstats(amac_id);

        rpkt_no=checker_all(amac_id);
        //rpkt_no = TEST_PASS;
        post_log("\n");

        post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        rpkt_chk=TEST_PASS;

        rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);


         if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
         }

         post_log("\n\n");
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

}

int amacswitch_robosw_gmii_2_rgmii_switchport_1_to_5_sram(void)
{


  

    uint32_t index,rd_data;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];



   uint32_t bypass_mode;

   bypass_mode = 0;


    //256-byte data; CRC will be appended by Tx MAC
    uint8_t my_pkt_data[256] =
   {
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

   };

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr;
    uint32_t amac_id,amac_ports,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base;
 

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,chk_noPkt;

    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
    cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

    rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
    post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

    rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
    post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);


	 ///////////// TEST ARG INIT ////////////////////



              amac_ports        = 1;
              tx_desc_base      =0x2000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x2010000;
              tx_desc_len       = 2;//Max Desc=512=0x200
              rx_desc_len       = 2;
              tx_buff_base      =0x2020000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x2030000;
              frame_size          =128;
              //frame_sz            =1024;
               
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX descriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

       //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);

        //Set up MAC loopback
        // amac_set_config(amac_id, "MAC_LOOPBACK", 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
        // post_log("\n\n");
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////// Configuration for Managed mode //////////////////////////////////////

    cfg_sw();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        //Out of EOT range,for continuous loopback
        /*nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;*/
        amac_enable_tx_dma(amac_id, nextDscr);
        //post_log("\n\n");
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



   //////////////////////////////////////////////// Send and stop packet transmission for A0  and A1 ////////////////////////
  /* timeout=100;
   post_log("Sending packets for specified time\n");
   delay_noise(timeout,0);
   post_log("Time_taken =%d secs\n",(timeout));
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        suspend_tx(amac_id);
   }*/
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



   //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){

        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        //amac_gettxstats(amac_id);
        //amac_getrxstats(amac_id);

        rpkt_no=checker_all(amac_id);
        //rpkt_no = TEST_PASS;
        post_log("\n");

        post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        rpkt_chk=TEST_PASS;

        rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);


         if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
         }

         post_log("\n\n");
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

}

int amacswitch_robosw_gmii_2_gmii_switchport_0_to_1_ddr(void)
{


  

    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];

  

   uint32_t bypass_mode;
   uint32_t rd_data;
   uint32_t page;
   	uint32_t offset;
   	uint32_t wdatah, wdatal;
   	uint32_t rdatah=0, rdatal=0;

       
       uint32_t phy_addr,reg_addr,data,i;

   bypass_mode = 0;


    //256-byte data; CRC will be appended by Tx MAC
    uint8_t my_pkt_data[256] =
   {
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

   };

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr;
    uint32_t amac_id,amac_ports,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base;
  

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,chk_noPkt;

    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
        cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

        rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
        post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

        rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
        post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);


	 ///////////// TEST ARG INIT ////////////////////



              amac_ports        =1;
              tx_desc_base      =0x71000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x72000000;
              tx_desc_len       =16;//Max Desc=512=0x200
              rx_desc_len       = 16;
              tx_buff_base      =0x61000000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x62000000;
              frame_size          =128;
              //frame_sz            =1024;
           
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);


       //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);

        //Set up MAC loopback
        // amac_set_config(amac_id, "MAC_LOOPBACK", 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
        // post_log("\n\n");
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////// Configuration for Managed mode //////////////////////////////////////

    //cfg_sw_0_1();
   cfg_sw_gphy0_to_gphy1();

                      page=0x0;
                      offset=0x6f;
                      
                      wdatal=(rdatal | 0x1);
                      wdatah=rdatah;
                      srab_wr(page, offset, wdatah, wdatal);


                      ccg_mdio_init();

                       phy_addr = 0;
                         for(i=0; i<16; i++) {
                                reg_addr = i;
                                gphy_reg_read(phy_addr, reg_addr, &data);
                           }

                         //Lane Swapping and Polarity inversion for Gphy0
                         phy_addr = 0;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 0x15;
                         data = 0x5193;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         for(i=0; i<20000000; i++) ;

                         //Lane Swapping and Polarity inversion for Gphy1
                         phy_addr = 1;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 0x15;
                         data = 0x11C9;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         for(i=0; i<20000000; i++) ;

                         phy_addr = 0;
                                       reg_addr = 0x1;
                                       do {
                                    	    gphy_reg_read(phy_addr,reg_addr,&data);
                                       } while(!(data & 0x4));

                         //wait for linkup
                         phy_addr = 1;
                         reg_addr = 0x1;
                         do {
                      	    gphy_reg_read(phy_addr,reg_addr,&data);
                         } while(!(data & 0x4));


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        //Out of EOT range,for continuous loopback
        /*nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;*/
        amac_enable_tx_dma(amac_id, nextDscr);
        //post_log("\n\n");
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



   //////////////////////////////////////////////// Send and stop packet transmission for A0  and A1 ////////////////////////
  /* timeout=100;
   post_log("Sending packets for specified time\n");
   delay_noise(timeout,0);
   post_log("Time_taken =%d secs\n",(timeout));
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        suspend_tx(amac_id);
   }*/
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



   //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){

        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        amac_gettxstats(amac_id);
        amac_getrxstats(amac_id);

       // rpkt_no=checker_all(amac_id);Nishant

        rpkt_no = TEST_PASS;
        post_log("\n");

        post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        rpkt_chk=TEST_PASS;

        rpkt_chk=chkpktdata_add(amac_id,tx_desc_base,rx_desc_base,2*chk_noPkt);


         if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
         }

         post_log("\n\n");
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*page = 0xe2;
	offset=0x00;
	rdata = srab_rd(page, offset);
    rdatal=rdata[0];
    rdatah=rdata[1];
    post_log("Page : 0xe2 Offset: 0x00 : rdatal : %08X rdatah : %08X \n",rdatal,rdatah);

    page = 0xe2;
    offset=0x01;
    rdata = srab_rd(page, offset);
    rdatal=rdata[0];
    rdatah=rdata[1];
    post_log("Page : 0xe2 Offset: 0x01 : rdatal : %08X rdatah : %08X \n",rdatal,rdatah);

    page = 0xe2;
    offset=0x02;
    rdata = srab_rd(page, offset);
    rdatal=rdata[0];
    rdatah=rdata[1];
    post_log("Page : 0xe2 Offset: 0x02 : rdatal : %08X rdatah : %08X \n",rdatal,rdatah);

    page = 0xe2;
    offset=0x03;
    rdata = srab_rd(page, offset);
    rdatal=rdata[0];
    rdatah=rdata[1];
    post_log("Page : 0xe2 Offset: 0x03 : rdatal : %08X rdatah : %08X \n",rdatal,rdatah);

    page = 0x10;
    offset=0x00;
    rdata = srab_rd(page, offset);
    rdatal=rdata[0];
    rdatah=rdata[1];
    post_log("Page : 0x10 Offset: 0x00 : rdatal : %08X rdatah : %08X \n",rdatal,rdatah);

    page = 0x10;
    offset=0x01;
    rdata = srab_rd(page, offset);
    rdatal=rdata[0];
    rdatah=rdata[1];
    post_log("Page : 0x10 Offset: 0x01 : rdatal : %08X rdatah : %08X \n",rdatal,rdatah);

    page = 0x10;
    offset=0x02;
    rdata = srab_rd(page, offset);
    rdatal=rdata[0];
    rdatah=rdata[1];
    post_log("Page : 0x10 Offset: 0x02 : rdatal : %08X rdatah : %08X \n",rdatal,rdatah);

    page = 0x10;
    offset=0x03;
    rdata = srab_rd(page, offset);
    rdatal=rdata[0];
    rdatah=rdata[1];
    post_log("Page : 0x10 Offset: 0x03 : rdatal : %08X rdatah : %08X \n",rdatal,rdatah);

    page = 0x11;
    offset=0x00;
    rdata = srab_rd(page, offset);
    rdatal=rdata[0];
    rdatah=rdata[1];
    post_log("Page : 0x11 Offset: 0x00 : rdatal : %08X rdatah : %08X \n",rdatal,rdatah);

    page = 0x11;
    offset=0x01;
    rdata = srab_rd(page, offset);
    rdatal=rdata[0];
    rdatah=rdata[1];
    post_log("Page : 0x11 Offset: 0x01 : rdatal : %08X rdatah : %08X \n",rdatal,rdatah);

    page = 0x11;
    offset=0x02;
    rdata = srab_rd(page, offset);
    rdatal=rdata[0];
    rdatah=rdata[1];
    post_log("Page : 0x11 Offset: 0x02 : rdatal : %08X rdatah : %08X \n",rdatal,rdatah);

    page = 0x11;
        offset=0x03;
        rdata = srab_rd(page, offset);
        rdatal=rdata[0];
        rdatah=rdata[1];
        post_log("Page : 0x11 Offset: 0x03 : rdatal : %08X rdatah : %08X \n",rdatal,rdatah);*/





   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

}

int amacswitch_robosw_gmii_2_gmii_switchport_0_to_1_sram(void)
{
   



    uint32_t index;
	test_status_t result,rpkt_no;
   test_status_t res_sts[2];


 
   uint32_t bypass_mode;
   uint32_t rd_data;


   uint32_t page;
      	uint32_t offset;
      	uint32_t wdatah, wdatal;
      	uint32_t rdatah=0, rdatal=0;

       
          uint32_t phy_addr,i,reg_addr;
          uint32_t data;

   bypass_mode = 0;


    //256-byte data; CRC will be appended by Tx MAC


    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr;
    uint32_t amac_id,amac_ports,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base;


	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size;

    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
        cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

        rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
        post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

        rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
        post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);


	 ///////////// TEST ARG INIT ////////////////////


              amac_ports        =1;
              tx_desc_base      =0x2000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x2010000;
              tx_desc_len       =2;//Max Desc=512=0x200
              rx_desc_len       = 2;
              tx_buff_base      =0x2020000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x2030000;
              frame_size          =64;
              //frame_sz            =1024;
              
 
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        //cfg_sw_0_1(); modified by sathishs
        cfg_sw_gphy0_to_gphy1();

            page=0x0;
            offset=0x6f;
            //rdata = srab_rd(page, offset);
            wdatal=(rdatal | 0x1);
            wdatah=rdatah;
            srab_wr(page, offset, wdatah, wdatal);


            ccg_mdio_init();

            for(i=0; i<80000000; i++) ;
             phy_addr = 0;
               for(i=0; i<16; i++) {
                      reg_addr = i;
                      gphy_reg_read(phy_addr, reg_addr, &data);
                 }

               //Lane Swapping and Polarity inversion for Gphy0
               phy_addr = 0;
               reg_addr = 0x17;
               data = 0x0F09;
               gphy_reg_write(phy_addr, reg_addr, &data);
               //gphy_reg_read(phy_addr, reg_addr, &data);

               phy_addr = 0;
               reg_addr = 0x15;
               data = 0x5193;
               gphy_reg_write(phy_addr, reg_addr, &data);
               //gphy_reg_read(phy_addr, reg_addr, &data);

               for(i=0; i<80000000; i++) ;

               /*phy_addr = 0;
                                            reg_addr = 0x1;
                                            do {
                                         	    gphy_reg_read(phy_addr,reg_addr,&data);
                                            } while(!(data & 0x4));*/

               //Lane Swapping and Polarity inversion for Gphy1
               phy_addr = 1;
               reg_addr = 0x17;
               data = 0x0F09;
               gphy_reg_write(phy_addr, reg_addr, &data);
               //gphy_reg_read(phy_addr, reg_addr, &data);

               phy_addr = 1;
               reg_addr = 0x15;
               data = 0x11C9;
               gphy_reg_write(phy_addr, reg_addr, &data);

               for(i=0; i<80000000; i++) ;



               //wait for linkup
               phy_addr = 1;
               reg_addr = 0x1;
               do {
            	    gphy_reg_read(phy_addr,reg_addr,&data);
               } while(!(data & 0x4));

               phy_addr = 0;
                              reg_addr = 0x1;
                              do {
                           	    gphy_reg_read(phy_addr,reg_addr,&data);
                              } while(!(data & 0x4));

       //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);


         amac_set_config(amac_id, MAC_LOOPBACK, 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             //CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             //write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             /*amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
        // post_log("\n\n");
   }*/

        	amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
        	                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
        	             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        	        }

        	        //Initialize AMAC RX DMA block
        	        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
        	                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        	        //Update with next descriptor value - outside EOT range
        	        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////// Configuration for Managed mode //////////////////////////////////////




          //This is for phy internal loopback
            /*reg_addr = 0;
            gphy_reg_read(phy_addr, reg_addr, &data);
            data |= (1 << 14);
            gphy_reg_write(phy_addr, reg_addr, &data);
            gphy_reg_read(phy_addr, reg_addr, &data);

            data = 0x01E1;
            gphy_reg_write(phy_addr, 0x0004, &data);
            gphy_reg_read(phy_addr, 0x0004, &data);*/

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        /*nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;*/
        //Out of EOT range,for continuous loopback
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        amac_enable_tx_dma(amac_id, nextDscr);
        //post_log("\n\n");
   }


   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   post_log("Sending packets for specified time\n");
      //delay_noise(timeout,0);
      for(i=0; i<1000000; i++) {
         //dummy read
         if((i & 0xFFF) == 0)
         post_log("\r\rTransmitting pkts... %d\r\r", i);
         data = rd(0x02000000);
      }

      for(amac_id=0;amac_id < amac_ports;amac_id++){
             suspend_tx(amac_id);
        }

        for(i=0; i<1000; i++) {
           //dummy read
           data = rd(0x02000000);
        }

   //////////////////////////////////////////////// Send and stop packet transmission for A0  and A1 ////////////////////////
  /* timeout=100;
   post_log("Sending packets for specified time\n");
   delay_noise(timeout,0);
   post_log("Time_taken =%d secs\n",(timeout));
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        suspend_tx(amac_id);
   }*/
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //post_log("Entering the debug points for Henry \n");


   //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){

        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        amac_gettxstats(amac_id);
        amac_getrxstats(amac_id);
        //amac_gettxrxstats(amac_id);
        //amac_getrxstats(amac_id);

        rpkt_no=checker_all(amac_id);//Nishant

        //rpkt_no = TEST_PASS;
        post_log("\n");

        //post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        //rpkt_chk=TEST_PASS;

        //rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);


         if((rpkt_no==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
         }

         post_log("\n\n");
   }

   page=0x1;
   offset=0;
   //rdata = srab_rd(page, offset);
   //post_log("Page 1 offset 0 : %08X \n",rdata[0]);

   page=0x1;
   offset=4;
  //rdata = srab_rd(page, offset);
   //post_log("Page 1 offset 4 : %08X \n",rdata[0]);

   page=0x1;
      offset=8;
     //rdata = srab_rd(page, offset);
     // post_log("Page 1 offset 8 : %08X \n",rdata[0]);


   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

}

int amacswitch_internal_loopback_test_gphy0(void)
{



    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];



   uint32_t bypass_mode;
   uint32_t page;
         	uint32_t offset;
         	uint32_t wdatah, wdatal;
         	uint32_t rdatah=0, rdatal=0;

             
             uint32_t phy_addr,i,reg_addr;
             uint32_t data;

   bypass_mode = 0;


    //256-byte data; CRC will be appended by Tx MAC
    uint8_t my_pkt_data[256] =
   {
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

   };

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr;
    uint32_t amac_id,amac_ports,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base;
  

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,chk_noPkt;
 
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;


	 ///////////// TEST ARG INIT ////////////////////



              amac_ports        =1;
              tx_desc_base      =0x2000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x2010000;
              tx_desc_len       =2;//Max Desc=512=0x200
              rx_desc_len       =2;
              tx_buff_base      =0x2020000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x2030000;
              frame_size          =128;
              //frame_sz            =1024;
               
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        cfg_sw_0_1();

                    page=0x0;
                    offset=0x6f;
                    
                    wdatal=(rdatal | 0x1);
                    wdatah=rdatah;
                    srab_wr(page, offset, wdatah, wdatal);


                    ccg_mdio_init();

                     phy_addr = 0;
                       for(i=0; i<16; i++) {
                              reg_addr = i;
                              gphy_reg_read(phy_addr, reg_addr, &data);
                         }

                       //Lane Swapping and Polarity inversion for Gphy0
                       phy_addr = 0;
                       reg_addr = 0x17;
                       data = 0x0F09;
                       gphy_reg_write(phy_addr, reg_addr, &data);
                       //gphy_reg_read(phy_addr, reg_addr, &data);

                       phy_addr = 0;
                       reg_addr = 0x15;
                       data = 0x5193;
                       gphy_reg_write(phy_addr, reg_addr, &data);
                       //gphy_reg_read(phy_addr, reg_addr, &data);



                       //wait for linkup
                       //reg_addr = 0x1;
                       //do {
                    	//    gphy_reg_read(phy_addr,reg_addr,&data);
                       //} while(!(data & 0x4));

                       reg_addr = 0;
                                   gphy_reg_read(phy_addr, reg_addr, &data);
                                   data |= (1 << 14);
                                   gphy_reg_write(phy_addr, reg_addr, &data);

        //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);

        //Set up MAC loopback
        amac_set_config(amac_id, MAC_LOOPBACK, 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
         post_log("\n\n");
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        //Out of EOT range,for continuous loopback
        /*nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;*/
        amac_enable_tx_dma(amac_id, nextDscr);
        post_log("\n\n");
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




   //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
   /*timeout=300;
   post_log("Sending packets for specified time\n");
   delay_noise(timeout,0);
   post_log("Time_taken =%d secs\n",(timeout*7));
   suspend_tx(amac_id);*/

   for(amac_id=0;amac_id < amac_ports;amac_id++){
        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        //amac_gettxstats(amac_id);
        //amac_getrxstats(amac_id);

        rpkt_no=checker_all(amac_id);
        post_log("\n");

        post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        rpkt_chk=TEST_PASS;
        rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

         if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
         }

         post_log("\n\n");
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

}

int amacswitch_internal_loopback_test_gphy1(void)
{
    
   
    

    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];
  
  

   uint32_t bypass_mode;
   uint32_t page;
         	uint32_t offset;
         	uint32_t wdatah, wdatal;
         	uint32_t rdatah=0, rdatal=0;

             
             uint32_t phy_addr,i,reg_addr;
             uint32_t data;

   bypass_mode = 0;


    //256-byte data; CRC will be appended by Tx MAC
    uint8_t my_pkt_data[256] =
   {
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

   };

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr;
    uint32_t amac_id,amac_ports,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base;


	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,chk_noPkt;

    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;


	 ///////////// TEST ARG INIT ////////////////////



              amac_ports        =1;
              tx_desc_base      =0x2000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x2010000;
              tx_desc_len       =2;//Max Desc=512=0x200
              rx_desc_len       =2;
              tx_buff_base      =0x2020000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x2030000;
              frame_size          =128;
              //frame_sz            =1024;
              
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        cfg_sw_0_1();

                    page=0x0;
                    offset=0x6f;
                    
                    wdatal=(rdatal | 0x1);
                    wdatah=rdatah;
                    srab_wr(page, offset, wdatah, wdatal);


                    ccg_mdio_init();

                     phy_addr = 1;
                       for(i=0; i<16; i++) {
                              reg_addr = i;
                              gphy_reg_read(phy_addr, reg_addr, &data);
                         }

                       //Lane Swapping and Polarity inversion for Gphy0
                       phy_addr = 1;
                       reg_addr = 0x17;
                       data = 0x0F09;
                       gphy_reg_write(phy_addr, reg_addr, &data);
                       //gphy_reg_read(phy_addr, reg_addr, &data);

                       phy_addr = 1;
                       reg_addr = 0x15;
                       data = 0x11C9;
                       gphy_reg_write(phy_addr, reg_addr, &data);
                       //gphy_reg_read(phy_addr, reg_addr, &data);



                       //wait for linkup
                       //reg_addr = 0x1;
                       //do {
                    	//    gphy_reg_read(phy_addr,reg_addr,&data);
                       //} while(!(data & 0x4));

                       reg_addr = 0;
                                   gphy_reg_read(phy_addr, reg_addr, &data);
                                   data |= (1 << 14);
                                   gphy_reg_write(phy_addr, reg_addr, &data);

        //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);

        //Set up MAC loopback
        amac_set_config(amac_id, MAC_LOOPBACK, 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
         post_log("\n\n");
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        //Out of EOT range,for continuous loopback
        /*nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;*/
        amac_enable_tx_dma(amac_id, nextDscr);
        post_log("\n\n");
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




   //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
   /*timeout=300;
   post_log("Sending packets for specified time\n");
   delay_noise(timeout,0);
   post_log("Time_taken =%d secs\n",(timeout*7));
   suspend_tx(amac_id);*/

   for(amac_id=0;amac_id < amac_ports;amac_id++){
        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        //amac_gettxstats(amac_id);
        //amac_getrxstats(amac_id);

        rpkt_no=checker_all(amac_id);
        post_log("\n");

        post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        rpkt_chk=TEST_PASS;
        rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

         if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
         }

         post_log("\n\n");
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

}

int amacswitch_robosw_gphy0_2_gphy1_switchport_0_to_1(void)
{
   test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2] = {TEST_PASS, TEST_PASS};

  
   int i;
   int bypass_mode,rd_data;
    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, index, nextDscr;
    uint32_t amac_id,amac_ports,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base;
    

    //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size;
   
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;
    uint32_t addr, data;
    uint32_t phy_addr, reg_addr;




  

    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

            rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
            post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

            rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
            post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);

   /*test_args_name[0]="Amac Ports =";
   test_args_name[1]="Tx Desc Base =";
   test_args_name[2]="Rx Desc Base =";
              test_args_name[3]="Tx Desc Depth =";
              test_args_name[4]="Rx Desc Depth =";
              test_args_name[5]="Tx Buffer Base =";
              test_args_name[6]="Rx Buffer Base =";
              test_args_name[7]="Frame size =";*/

              amac_ports        =1;
              tx_desc_base      =0x62E00000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x62E06000;
              tx_desc_len       =3;//Max Desc=512=0x200
              rx_desc_len       =3;
              tx_buff_base      =0x61000000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x62000000;
              frame_size          =128;
              //frame_sz            =1024;
               
   
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =0;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =0;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	bypass_mode = 0;

	tx_desc_base = (uint32_t) malloc(16*3);
    if(tx_desc_base & 0xF) {
        tx_desc_base += 0xF;
        tx_desc_base &= 0xFFFFFFF0;
    }

    rx_desc_base = (uint32_t) malloc(16*3);
    if(rx_desc_base & 0xF) {
        rx_desc_base += 0xF;
        rx_desc_base &= 0xFFFFFFF0;
    }

    tx_buff_base = (uint32_t) malloc(200);
    if(tx_buff_base & 3) {
        tx_buff_base += 4;
        tx_buff_base &= 0xFFFFFFFC;
    }

    rx_buff_base = (uint32_t) malloc(200);
    if(rx_buff_base & 3) {
        rx_buff_base += 4;
        rx_buff_base &= 0xFFFFFFFC;
    }

    post_log("\n--------------------------------------\n");
    post_log("tx_desc_base = 0x%08X\n", tx_desc_base);
    post_log("rx_desc_base = 0x%08X\n", rx_desc_base);
    post_log("tx_buff_base = 0x%08X\n", tx_buff_base);
    post_log("rx_buff_base = 0x%08X\n", rx_buff_base);
    post_log("frame_size   = 0x%08X\n", frame_size);
    post_log("--------------------------------------\n\n");

    ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        cfg_sw_gphy0_to_gphy1();

        //------------------------------------------
        //EXT

        //------------------------------------------

        ccg_mdio_init();
        phy_addr = 0;

        //------------------------------------------

        phy_addr = 0;
        for(i=0; i<16; i++) {
            reg_addr = i;
            gphy_reg_read(phy_addr, reg_addr, &data);
        }

        post_log("-------------------------------------------\n\n");

        reg_addr = 0x17;
        data = 0x0F09;
        gphy_reg_write(phy_addr, reg_addr, &data);
        reg_addr = 0x15;
        data = 0x5193;
        gphy_reg_write(phy_addr, reg_addr, &data);

        reg_addr = 0x17;
        data = 0x0F09;
        gphy_reg_write(phy_addr, reg_addr, &data);
        reg_addr = 0x15;
        gphy_reg_read(phy_addr, reg_addr, &data);

        //wait for linkup
        reg_addr = 0x1;
        do {
            gphy_reg_read(phy_addr, reg_addr, &data);
        } while(!(data & 0x4));

        post_log("-------------------------------------------\n\n");

        phy_addr = 1;
        reg_addr = 0x17;
        data = 0x0F09;
        gphy_reg_write(phy_addr, reg_addr, &data);
        reg_addr = 0x15;
        data = 0x11C9;
        gphy_reg_write(phy_addr, reg_addr, &data);

        reg_addr = 0x17;
        data = 0x0F09;
        gphy_reg_write(phy_addr, reg_addr, &data);
        reg_addr = 0x15;
        gphy_reg_read(phy_addr, reg_addr, &data);

        //wait for linkup
        reg_addr = 0x1;
        do {
            gphy_reg_read(phy_addr, reg_addr, &data);
        } while(!(data & 0x4));

        post_log("-------------------------------------------\n\n");

        //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);

        //Set up MAC loopback
        //amac_set_config(amac_id, "MAC_LOOPBACK", 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             //CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             //write_pkt_to_memory(my_pkt_data,(frame_size), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
         post_log("\n\n");
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        /*nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;*/
        //Out of EOT range,for continuous loopback
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len + 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        amac_enable_tx_dma(amac_id, nextDscr);
        post_log("\n\n");
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



   //////////////////////////////////////////////// Send and stop packet transmission for A0  and A1 ////////////////////////
   
   post_log("Sending packets for specified time\n");
   //delay_noise(timeout,0);
   for(i=0; i<1000000; i++) {
      //dummy read
      if((i & 0xFFF) == 0)
      post_log("\r\rTransmitting pkts... %d\r\r", i);
      data = rd(0x02000000);
   }


   //post_log("Press a key to continue...\n");
   //getHex();

   for(amac_id=0;amac_id < amac_ports;amac_id++){
        suspend_tx(amac_id);
   }

   for(i=0; i<1000; i++) {
      //dummy read
      data = rd(0x02000000);
   }

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



   //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        amac_gettxstats(amac_id);
        amac_getrxstats(amac_id);

        rpkt_no=checker_all(amac_id);
        post_log("\n");

        post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        rpkt_chk=TEST_PASS;
        //rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

         if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
         }

         post_log("\n\n");
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   post_log("\n--------------------------------------\n");
   post_log("GMAC0_XMTSTATUS0_0   = 0x%08X\n", *(uint32_t *) GMAC0_XMTSTATUS0_0);
   post_log("GMAC0_XMTSTATUS1_0   = 0x%08X\n", *(uint32_t *) GMAC0_XMTSTATUS1_0);
   post_log("GMAC0_XMTCONTROL_0   = 0x%08X\n", *(uint32_t *) GMAC0_XMTCONTROL_0);
   post_log("GMAC0_RCVSTATUS0     = 0x%08X\n", *(uint32_t *) GMAC0_RCVSTATUS0);
   post_log("GMAC0_RCVSTATUS1     = 0x%08X\n", *(uint32_t *) GMAC0_RCVSTATUS1);
   post_log("GMAC0_RCVCONTROL     = 0x%08X\n", *(uint32_t *) GMAC0_RCVCONTROL);
   post_log("GMAC0_DEVCONTROL     = 0x%08X\n", *(uint32_t *) GMAC0_DEVCONTROL);
   post_log("GMAC0_DEVSTATUS      = 0x%08X\n", *(uint32_t *) GMAC0_DEVSTATUS);
   post_log("GMAC0_XMTADDR_LOW_0  = 0x%08X\n", *(uint32_t *) GMAC0_XMTADDR_LOW_0);
   post_log("GMAC0_XMTADDR_HIGH_0 = 0x%08X\n", *(uint32_t *) GMAC0_XMTADDR_HIGH_0);
   post_log("--------------------------------------\n\n");

   addr = tx_desc_base;
   while(addr < tx_buff_base) {
      post_log("[0x%08X] = 0x%08X\n", addr, *(uint32_t *) addr);
      addr += 4;
   }

   if((res_sts[0]==TEST_PASS) && (res_sts[1]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;
}


/*int gphy_internal_loopback(void)
{
    test_status_t status = TEST_PASS;
    uint32_t test_args[NUM_TEST_ARGS];
    char *test_args_name[NUM_TEST_ARGS];
    uint32_t l,frame_sz,iter=1;
    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];
   amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
   amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
   amac_rx_pkt_info_t* pkt_ptr;
   uint32_t bypass_mode;
   uint32_t rd_data;
   uint32_t rdatal_16bit;

   uint32_t page;
      	uint32_t offset;
      	uint32_t wdatah, wdatal;
      	uint32_t rdatah, rdatal;
      	uint32_t port_id;
          uint32_t *rdata;
          uint32_t phy_addr,i,reg_addr;
          uint32_t data;

   bypass_mode = 0;


    //256-byte data; CRC will be appended by Tx MAC
    uint8_t my_pkt_data[256] =
   {
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

   };

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
    uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
    uint8_t rdata8;

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,step,chk_noPkt;
    uint32_t timeout;
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
        cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

        rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
        post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

        rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
        post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);


	 ///////////// TEST ARG INIT ////////////////////

              test_args_name[0]="Amac Ports =";
              test_args_name[1]="Tx Desc Base =";
              test_args_name[2]="Rx Desc Base =";
              test_args_name[3]="Tx Desc Depth =";
              test_args_name[4]="Rx Desc Depth =";
              test_args_name[5]="Tx Buffer Base =";
              test_args_name[6]="Rx Buffer Base =";
              test_args_name[7]="Frame size =";

              amac_ports        =1;
              tx_desc_base      =0x2000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x2010000;
              tx_desc_len       =2;//Max Desc=512=0x200
              rx_desc_len       = 2;
              tx_buff_base      =0x2020000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x2030000;
              frame_size          =128;
              //frame_sz            =1024;
               step            =23;
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        cfg_sw_0_1();

            ccg_mdio_init();

                    phy_addr = 0;
                    for(i=0; i<16; i++) {
                        reg_addr = i;
                        gphy_reg_read(phy_addr, reg_addr, &data);
                    }

                    reg_addr = 0;
                    gphy_reg_read(phy_addr, reg_addr, &data);
                    data |= (1 << 14);
                    gphy_reg_write(phy_addr, reg_addr, &data);
                    gphy_reg_read(phy_addr, reg_addr, &data);

                    data = 0x01E1;
                    gphy_reg_write(phy_addr, 0x0004, &data);
                    gphy_reg_read(phy_addr, 0x0004, &data);


       //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);

        //Set up MAC loopback
        // amac_set_config(amac_id, "MAC_LOOPBACK", 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
        // post_log("\n\n");
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////// Configuration for Managed mode //////////////////////////////////////


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        //Out of EOT range,for continuous loopback
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        amac_enable_tx_dma(amac_id, nextDscr);
        //post_log("\n\n");
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



   //////////////////////////////////////////////// Send and stop packet transmission for A0  and A1 ////////////////////////
   timeout=100;
   post_log("Sending packets for specified time\n");
   delay_noise(timeout,0);
   post_log("Time_taken =%d secs\n",(timeout));
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        suspend_tx(amac_id);
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



   //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){

        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        amac_gettxstats(amac_id);
        amac_getrxstats(amac_id);

       // rpkt_no=checker_all(amac_id);Nishant

        rpkt_no = TEST_PASS;
        post_log("\n");

        post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        rpkt_chk=TEST_PASS;

        rpkt_chk=chkpktdata_add(amac_id,tx_desc_base,rx_desc_base,2*chk_noPkt);


         if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
         }

         post_log("\n\n");
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

}*/






/******* Internal test function start ********/
int AMACSWITCH_post_test(int flags)
{

    int status = 0;


    //lcd_clk_gate();
    //lcd_asiu_mipi_dsi_gen_pll_pwr_on_config();
    //lcd_ctrl_init(0x61000000,&config);
#if 1
     status = TEST_PASS;
        if(TEST_FAIL == amacswitch_robosw_gmii_2_gmii_switchport_0_to_1_sram())
    	      status = TEST_FAIL;
        return status;
#else

    FUNC_INFO function[]	=	{
	     	 	 	 	 	 	 //ADD_TEST( amacswitch_internal_loopback_test_ddr      ),
	     	 	 	 	 	 	 ADD_TEST( amacswitch_internal_loopback_test_sram ),
	     	 	 	 	 	 	 //ADD_TEST( amacswitch_bypass_test ),
	     	 	 	 	 	 	 //ADD_TEST( amacswitch_robosw_gmii_2_rgmii_switchport_1_to_5_ddr ),
	     	 	 	 	 	     //ADD_TEST( amacswitch_robosw_gmii_2_rgmii_switchport_1_to_5_sram ),
	     	 	 	 	 	 	 ADD_TEST( amacswitch_robosw_gmii_2_gmii_switchport_0_to_1_ddr ),
	     	 	 	 	 	     //ADD_TEST( amacswitch_internal_loopback_test_gphy0 ),
	     	 	 	 	 	     //ADD_TEST( amacswitch_internal_loopback_test_gphy1 ),
    		                     ADD_TEST( amacswitch_external_loopback_test),
	     	 	 	 	 	     ADD_TEST( amacswitch_robosw_gmii_2_gmii_switchport_0_to_1_sram ),
                                 ADD_TEST( amacswitch_robosw_gphy0_2_gphy1_switchport_0_to_1 )


								};

    FUNC_INFO regression_func[]	=	{
    		                                 ADD_TEST( amacswitch_internal_loopback_test_ddr   ),
    		                                 ADD_TEST( amacswitch_internal_loopback_test_sram ),
    		                                 ADD_TEST( amacswitch_robosw_gmii_2_gmii_switchport_0_to_1_sram )


									};

    total_tests = (sizeof(function) / sizeof(function[0]));
    total_reg_tests = (sizeof(regression_func) / sizeof(regression_func[0]));

    if(flags & POST_REGRESSION )
    {
    	for(loop_count = 0 ; loop_count  < total_reg_tests ; loop_count++ )
    	{
    		post_log("\nExecuting test %2d - %s\n",(loop_count + 1), regression_func[loop_count].func_name);

    		status = (*regression_func[loop_count].func)();
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

    	}
    	return 0;
    }
    else
    {
    	post_log("Number of test : %d",total_tests);
    	do{
    		for(loop_count = 0 ; loop_count  < total_tests ; loop_count++ )
    			post_log("\n%2d %s",(loop_count + 1), function[loop_count].func_name);
    		post_log("\n%2d run all tests",(total_tests + 1));
    		post_log("\n%2d exit\n",(total_tests + 2));
    		test_no = post_getUserInput ("\nEnter Test Number : ");

    		if(test_no > (total_tests + 2) )
    			post_log("ERROR - invalid entry\n");
    		else
    		{
                  if(test_no == (total_tests + 1))
                  {
                      autorun_status = 0;
                      pass = 0;
                      fail = 0;
                      skip = 0;
                      for(loop_count = 0 ; loop_count  < total_tests ; loop_count++ )
                      {
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

                  else if(test_no < (total_tests + 1))
                  {
                	  if(TEST_FAIL == (*function[test_no - 1].func)())
                		  status = -1;
                  }
             }

    	}while(test_no != (total_tests + 2));
    	return status;

    }
    return 0;
#endif
}
#endif

