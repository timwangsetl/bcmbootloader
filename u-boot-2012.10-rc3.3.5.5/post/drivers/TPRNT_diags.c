/*
 * $Copyright Open Broadcom Corporation$
 */

#include <common.h>
#include <post.h>

#if defined(CONFIG_CYGNUS)
#if CONFIG_POST & CONFIG_SYS_POST_TPRNT

#include "asm/arch/socregs.h"
#include <ssp.h>

#define SSPCR0_SCR 49
#define SSPCR0_CPSDVSR 2
uint8_t phase = 0;
int TPRNT_post_test(int flags);

/*Following array is hex equivalent of dot lines for printing CYGNUS*/
    /*Each character is printed in 24x16 dot matrix */
    /*Each line can have 48 bytes of data. Cygnus requires 12 bytes per dot line in a 24x16 dot matrix*/
    /* Printing is centred such that there are 18 bytes of data before and after Cygnus in each dot line*/

#if 0
    uint8_t line[24][12]  =
    			{{0x1F, 0xF8, 0xE0, 0x07, 0x07, 0xF8, 0x60, 0x0E, 0x70, 0x0E, 0x3F, 0xFC},
			     {0x3F, 0xFC, 0xE0, 0x07, 0x0F, 0xFC, 0x60, 0x0E, 0x70, 0x0E, 0x7F, 0xFE},
		         {0x7F, 0xFE, 0xE0, 0x07, 0x1F, 0xFE, 0x70, 0x0E, 0x70, 0x0E, 0xFF, 0xFE},
			     {0xE0, 0x07, 0xE0, 0x07, 0x38, 0x07, 0x78, 0x0E, 0x70, 0x0E, 0xE0, 0x0E},
		     	 {0xE0, 0x07, 0xE0, 0x07, 0x70, 0x07, 0x7C, 0x0E, 0x70, 0x0E, 0xE0, 0x0E},
			     {0xE0, 0x07, 0xE0, 0x0E, 0x70, 0x00, 0x7E, 0x0E, 0x70, 0x0E, 0xE0, 0x0E},
			     {0xE0, 0x00, 0x70, 0x1C, 0x70, 0x00, 0x77, 0x0E, 0x70, 0x0E, 0xE0, 0x00},
			     {0xE0, 0x00, 0x38, 0x38, 0x70, 0x00, 0x77, 0x0E, 0x70, 0x0E, 0xE0, 0x00},
			     {0xE0, 0x00, 0x1C, 0x70, 0x70, 0x00, 0x73, 0x8E, 0x70, 0x0E, 0xE0, 0x00},
			     {0xE0, 0x00, 0x0E, 0x70, 0x71, 0xFF, 0x71, 0xCE, 0x70, 0x0E, 0xFF, 0xFE},
			     {0xE0, 0x00, 0x07, 0xE0, 0x71, 0xFF, 0x71, 0xCE, 0x70, 0x0E, 0xFF, 0xFE},
			     {0xE0, 0x00, 0x03, 0xC0, 0x71, 0xFF, 0x71, 0xEE, 0x70, 0x0E, 0xFF, 0xFE},
			     {0xE0, 0x00, 0x01, 0xC0, 0x70, 0x07, 0x70, 0xEE, 0x70, 0x0E, 0x00, 0x0E},
			     {0xE0, 0x00, 0x01, 0xC0, 0x70, 0x07, 0x70, 0xEE, 0x70, 0x0E, 0x00, 0x0E},
			     {0xE0, 0x00, 0x01, 0xC0, 0x70, 0x07, 0x70, 0x7E, 0x70, 0x0E, 0x00, 0x0E},
			     {0xE0, 0x00, 0x01, 0xC0, 0x70, 0x07, 0x70, 0x7E, 0x70, 0x0E, 0x00, 0x0E},
			     {0xE0, 0x00, 0x01, 0xC0, 0x70, 0x07, 0x70, 0x3E, 0x70, 0x0E, 0x00, 0x0E},
			     {0xE0, 0x00, 0x01, 0xC0, 0x70, 0x07, 0x70, 0x3E, 0x70, 0x0E, 0x70, 0x0E},
			     {0xE0, 0x07, 0x01, 0xC0, 0x38, 0x0E, 0x70, 0x3E, 0x70, 0x0E, 0x70, 0x0E},
			     {0xE0, 0x07, 0x01, 0xC0, 0x1C, 0x1C, 0x70, 0x1E, 0x70, 0x0E, 0x70, 0x0E},
			     {0xE0, 0x07, 0x01, 0xC0, 0x1E, 0x0C, 0x70, 0x1E, 0x70, 0x0E, 0x70, 0x0E},
			     {0x7F, 0xFE, 0x01, 0xC0, 0x0F, 0xFC, 0x70, 0x1E, 0x7F, 0xFE, 0x7F, 0xFE},
			     {0x3F, 0xFC, 0x01, 0xC0, 0x07, 0xF8, 0x70, 0x0E, 0x3F, 0xFC, 0x3F, 0xFC},
			     {0x1F, 0xF8, 0x01, 0xC0, 0x03, 0xF0, 0x70, 0x0E, 0x1F, 0xF8, 0x1F, 0xF8}};
#endif


uint8_t line[24][12]  =
  {
	{0xFF, 0xFE, 0xFF, 0xFE, 0x60, 0x0E, 0xFF, 0xFE, 0xFF, 0xFE, 0xFE, 0xFE},
	{0xFF, 0xFE, 0xFF, 0xFE, 0x60, 0x0E, 0xFF, 0xFE, 0xFF, 0xFE, 0xFE, 0xFE},
	{0xE0, 0x00, 0xE0, 0x0E, 0x70, 0x0E, 0xE0, 0x0E, 0xE0, 0x00, 0x00, 0x0E},
	{0xE0, 0x00, 0xE0, 0x0E, 0x78, 0x0E, 0xE0, 0x0E, 0xE0, 0x00, 0x00, 0x0E},
	{0xE0, 0x00, 0xE0, 0x0E, 0x7C, 0x0E, 0xE0, 0x0E, 0xE0, 0x00, 0x00, 0x0E},
	{0xE0, 0x00, 0xE0, 0x0E, 0x7E, 0x0E, 0xE0, 0x0E, 0xE0, 0x00, 0x00, 0x0E},
	{0xE0, 0x00, 0xE0, 0x0E, 0x77, 0x0E, 0xE0, 0x0E, 0xE0, 0x00, 0x00, 0x0E},
	{0xE0, 0x00, 0xE0, 0x0E, 0x77, 0x0E, 0xE0, 0x0E, 0xE0, 0x00, 0x00, 0x0E},
	{0xE0, 0x00, 0xE0, 0x0E, 0x73, 0x83, 0xE0, 0x0E, 0xE0, 0x00, 0x00, 0x0E},
	{0xE0, 0x00, 0xE0, 0x0E, 0x71, 0xCE, 0xE0, 0x0E, 0xE0, 0x00, 0x00, 0x0E},
	{0xE0, 0x00, 0xE0, 0x0E, 0x71, 0xCE, 0xE0, 0x0E, 0xE0, 0x00, 0x00, 0x0E},
	{0xFF, 0xFE, 0xE0, 0x0E, 0x71, 0xEE, 0xFC, 0x0E, 0xFF, 0xFE, 0x00, 0x0E},


	{0xFF, 0xFE, 0xE0, 0x0E, 0x70, 0xEE, 0xFC, 0x0E, 0xFF, 0xFE, 0x00, 0x0E},
	{0x00, 0x0E, 0xE0, 0x0E, 0x70, 0xEE, 0x00, 0x0E, 0xE0, 0x0E, 0x00, 0x0E},
	{0x00, 0x0E, 0xE0, 0x0E, 0x70, 0x7E, 0x00, 0x0E, 0xE0, 0x0E, 0x00, 0x0E},
	{0x00, 0x0E, 0xE0, 0x0E, 0x70, 0x7E, 0x00, 0x0E, 0xE0, 0x0E, 0x00, 0x0E},
	{0x00, 0x0E, 0xE0, 0x0E, 0x70, 0x3E, 0x00, 0x0E, 0xE0, 0x0E, 0x00, 0x0E},
	{0x00, 0x0E, 0xE0, 0x0E, 0x70, 0x3E, 0x00, 0x0E, 0xE0, 0x0E, 0x00, 0x0E},
	{0x00, 0x0E, 0xE0, 0x0E, 0x70, 0x3E, 0x00, 0x0E, 0xE0, 0x0E, 0x00, 0x0E},
	{0x00, 0x0E, 0xE0, 0x0E, 0x70, 0x1E, 0x00, 0x0E, 0xE0, 0x0E, 0x00, 0x0E},
	{0x00, 0x0E, 0xE0, 0x0E, 0x70, 0x1E, 0x00, 0x0E, 0xE0, 0x0E, 0x00, 0x0E},
	{0x00, 0x0E, 0xE0, 0x0E, 0x70, 0x1E, 0x00, 0x0E, 0xE0, 0x0E, 0x00, 0x0E},
	{0xFF, 0xFE, 0xE0, 0x0E, 0x70, 0x0E, 0xFF, 0xFE, 0xE0, 0x0E, 0xFF, 0xFE},
	{0xFF, 0xFE, 0xE0, 0x0E, 0x70, 0x0E, 0xFF, 0xFE, 0xE0, 0x0E, 0xFF, 0xFE}};





#if 0
			   {

				{0xFF, 0xFE, 0xFF, 0xFE, 0x60, 0x0E, 0xFF, 0xFE, 0xFF, 0xFE, 0xFE, 0xFE},
				{0xFF, 0xFE, 0xFF, 0xFE, 0x60, 0x0E, 0xFF, 0xFE, 0xFF, 0xFE, 0xFE, 0xFE},
				{0xC0, 0x00, 0xC0, 0x06, 0x70, 0x0E, 0xC0, 0x06, 0xC0, 0x00, 0x00, 0x06},
				{0xC0, 0x00, 0xC0, 0x06, 0x78, 0x0E, 0xC0, 0x06, 0xC0, 0x00, 0x00, 0x06},
				{0xC0, 0x00, 0xC0, 0x06, 0x7C, 0x0E, 0xC0, 0x06, 0xC0, 0x00, 0x00, 0x06},
				{0xC0, 0x00, 0xC0, 0x06, 0x7E, 0x0E, 0xC0, 0x06, 0xC0, 0x00, 0x00, 0x06},
				{0xC0, 0x00, 0xC0, 0x06, 0x77, 0x0E, 0xC0, 0x06, 0xC0, 0x00, 0x00, 0x06},
				{0xC0, 0x00, 0xC0, 0x06, 0x77, 0x0E, 0xC0, 0x06, 0xC0, 0x00, 0x00, 0x06},
				{0xC0, 0x00, 0xC0, 0x06, 0x73, 0x83, 0xC0, 0x06, 0xC0, 0x00, 0x00, 0x06},
				{0xC0, 0x00, 0xC0, 0x06, 0x71, 0xCE, 0xC0, 0x06, 0xC0, 0x00, 0x00, 0x06},
				{0xC0, 0x00, 0xC0, 0x06, 0x71, 0xCE, 0xC0, 0x06, 0xC0, 0x00, 0x00, 0x06},
				{0xFF, 0xFE, 0xC0, 0x06, 0x71, 0xEE, 0xFC, 0x06, 0xFF, 0xFE, 0x00, 0x06},


				{0xFF, 0xFE, 0xC0, 0x06, 0x70, 0xEE, 0xFC, 0x06, 0xFF, 0xFE, 0x00, 0x06},
				{0x00, 0x06, 0xC0, 0x06, 0x70, 0xEE, 0x00, 0x06, 0xC0, 0x06, 0x00, 0x06},
				{0x00, 0x06, 0xC0, 0x06, 0x70, 0x7E, 0x00, 0x06, 0xC0, 0x06, 0x00, 0x06},
				{0x00, 0x06, 0xC0, 0x06, 0x70, 0x7E, 0x00, 0x06, 0xC0, 0x06, 0x00, 0x06},
				{0x00, 0x06, 0xC0, 0x06, 0x70, 0x3E, 0x00, 0x06, 0xC0, 0x06, 0x00, 0x06},
				{0x00, 0x06, 0xC0, 0x06, 0x70, 0x3E, 0x00, 0x06, 0xC0, 0x06, 0x00, 0x06},
				{0x00, 0x06, 0xC0, 0x06, 0x70, 0x3E, 0x00, 0x06, 0xC0, 0x06, 0x00, 0x06},
				{0x00, 0x06, 0xC0, 0x06, 0x70, 0x1E, 0x00, 0x06, 0xC0, 0x06, 0x00, 0x06},
				{0x00, 0x06, 0xC0, 0x06, 0x70, 0x1E, 0x00, 0x06, 0xC0, 0x06, 0x00, 0x06},
				{0x00, 0x06, 0xC0, 0x06, 0x70, 0x1E, 0x00, 0x06, 0xC0, 0x06, 0x00, 0x06},
				{0xFF, 0xFE, 0xC0, 0x06, 0x70, 0x0E, 0xFF, 0xFE, 0xC0, 0x06, 0xFF, 0xFE},
				{0xFF, 0xFE, 0xC0, 0x06, 0x70, 0x0E, 0xFF, 0xFE, 0xC0, 0x06, 0xFF, 0xFE}};
#endif





void TPRNT_add_delay(uint32_t delay)
{
  volatile uint32_t i;
    	 for(i=0;i<delay;i++);
 }



static void ssp_spi0_config(uint16_t data_size, uint16_t frame_format, uint16_t scr, uint8_t cpsdvsr, uint8_t clk_phase, uint8_t clk_polarity, uint16_t master_slave);

/* Input:
 * data_size: SSPCR0_DSS_16 or SSPCR0_DSS_8 or SSPCR0_DSS_4
 * frame_format: SSPCR0_FRF_MOT or SSPCR0_FRF_TI or SSPCR0_FRF_NAT
 * scr: any value from 0 - 255
 * cpsdvsr: Must be an even number from 2-254. LSB always returns zero on reads.
 * clk_phase: a value of 0 or 1. Default is 0.
 * clk_polarity: a value of 0 or 1. Default is 0.
 * master_slave: 0 - Master or 1 - Slave 
*/ 


static void ssp_spi0_config(uint16_t data_size, uint16_t frame_format, uint16_t scr, uint8_t cpsdvsr,  uint8_t clk_phase, uint8_t clk_polarity, uint16_t master_slave)
{

	//Disable serial port operation
        CPU_WRITE_32(ChipcommonG_SPI0_SSPCR1, (master_slave << 2));

        // Set serial clock rate, Format, data size	
        CPU_WRITE_32(ChipcommonG_SPI0_SSPCR0, ((scr << 8) | (clk_phase << 7) | (clk_polarity << 6) | (frame_format << 4) | data_size));

	// Clock prescale register set to 2, with SCR =0 gives fastest possible clock
	// must be an even number (2-254)
	// bit rate = Fsspclk / (SSPCPSR * (1 + clock_rate))
	// Fsspclk = 100MHz for Cygnus
        CPU_WRITE_32(ChipcommonG_SPI0_SSPCPSR, cpsdvsr);

	// Enable serial port operation
	CPU_WRITE_32(ChipcommonG_SPI0_SSPCR1, ((master_slave << 2) | SSPCR1_SSE));
	    
}

static void ssp_spi0_byte_write(uint8_t data);

static void ssp_spi0_byte_write(uint8_t data) {

	uint32_t reg_data;
	int ssp_tnf = 0;


	while (ssp_tnf == 0 ) {
			  reg_data = CPU_READ_32(ChipcommonG_SPI0_SSPSR);
			  ssp_tnf = (reg_data  &   (1 << ChipcommonG_SPI0_SSPSR__TNF));
	  }


	//post_log("Entering SPI write : data Written : %08X \n",reg_data);


       
       CPU_WRITE_32(ChipcommonG_SPI0_SSPDR, data);
}

/*
static void ssp_spi0_dummy_byte_write();

static void ssp_spi0_dummy_byte_write() {
         uint32_t dummy = 0xD01EBA1E;
	 CPU_WRITE_32(ChipcommonG_SPI0_SSPDR, dummy);
}
*/
static void send_line_data (uint8_t line_no);

static void send_line_data (uint8_t line_no) {

	uint8_t zero = 0x00;
	uint8_t wrte_data;
    volatile    int i;
//post_log("line no : %d\n",line_no);
    /*Each character is printed in 24x16 dot matrix */
    /*Each line can have 48 bytes of data. Cygnus requires 12 bytes per dot line in a 24x16 dot matrix*/
    /* Printing is centred such that there are 18 bytes of data before and after Cygnus in each dot line*/

//for (i=0; i < 18; i++) {
		for (i=0; i < 14; i++) {

		ssp_spi0_byte_write(zero);
	}

	for (i=0; i<12; i++) {

		if ((i% 2)== 0)
				{
				ssp_spi0_byte_write(zero);
				}
			wrte_data = line[line_no][i];
			//post_log("Entering SPI write : data Written : %08X \n",wrte_data);
			ssp_spi0_byte_write(wrte_data);

		}


	//for (i=11; i<=0; i--) {
	//for (i=0; i<12; i++) {
	//post_log("Entering SPI write : data Written  \n");
	//for (i=11; i<=0; i--) {
	//for (i=0; i<12; i++) {
#if 0
		wrte_data = line[23-line_no][11];
		ssp_spi0_byte_write(wrte_data);
		wrte_data = line[23-line_no][10];
		ssp_spi0_byte_write(wrte_data);

		wrte_data = line[23-line_no][8];
		ssp_spi0_byte_write(wrte_data);
		wrte_data = line[23-line_no][9];
		ssp_spi0_byte_write(wrte_data);

		wrte_data = line[23-line_no][7];
		ssp_spi0_byte_write(wrte_data);
		wrte_data = line[23-line_no][6];
		ssp_spi0_byte_write(wrte_data);

		wrte_data = line[23-line_no][5];
		ssp_spi0_byte_write(wrte_data);
		wrte_data = line[23-line_no][4];
		ssp_spi0_byte_write(wrte_data);

		wrte_data = line[23-line_no][2];
		ssp_spi0_byte_write(wrte_data);
		wrte_data = line[23-line_no][3];
		ssp_spi0_byte_write(wrte_data);

		wrte_data = line[23-line_no][1];
		ssp_spi0_byte_write(wrte_data);
		wrte_data = line[23-line_no][0];
		ssp_spi0_byte_write(wrte_data);


	//}
	//post_log("\n line number : %x  \n",line_no);
#endif
  
	//for (i=0; i < 18; i++) {
	for (i=0; i < 14; i++) {
		ssp_spi0_byte_write(zero);
	}

}
/*
static void ssp_spi0_wait_for_rne (void);

static void ssp_spi0_wait_for_rne () {

    int32_t rd_data;
    int ssp_rne = 0;

     while(!ssp_rne) {
	     rd_data = CPU_READ_32(ChipcommonG_SPI0_SSPSR);
	     ssp_rne = (rd_data >> ChipcommonG_SPI0_SSPSR__RNE);
     }

}
*/
static void deassert_strobe_n(void);

static void deassert_strobe_n() {
	uint32_t read_val;
	uint32_t mask;

        read_val = CPU_READ_32(ChipcommonG_GP_DATA_OUT);
        mask = (1 << 21) | (1 << 20) | (1 << 13) | (1 << 12);
        read_val |= mask;
        //post_log("Within Deassert Strobe : %08X \n",read_val);

        CPU_WRITE_32(ChipcommonG_GP_DATA_OUT, read_val);
}

static void assert_strobe_n(void);

static void assert_strobe_n() {
	uint32_t read_val;
	//uint32_t mask;

        read_val = CPU_READ_32(ChipcommonG_GP_DATA_OUT);
        //mask = (1 << 21) | (1 << 20) | (1 << 13) | (1 << 12) ;
        read_val = read_val & 0xFFCFCFFF;

        //post_log("Within assert Strobe : %08X \n",read_val);

        CPU_WRITE_32(ChipcommonG_GP_DATA_OUT, read_val);
        //TPRNT_micro_second_delay(3);
        //TPRNT_micro_second_delay(3);
        //TPRNT_micro_second_delay(3);
        TPRNT_add_delay(3);
}

static void deassert_latch_n(void);

static void deassert_latch_n() {
	uint32_t read_val;
	uint32_t mask;

        read_val = CPU_READ_32(ChipcommonG_GP_DATA_OUT);
        mask = (1 << 7);
        read_val |= mask;

        //post_log("Within Deassert Latch : %08X \n",read_val);

        CPU_WRITE_32(ChipcommonG_GP_DATA_OUT, read_val);
}

static void assert_latch_n(void);

static void assert_latch_n() {
	uint32_t read_val;
	//uint32_t mask;

        read_val = CPU_READ_32(ChipcommonG_GP_DATA_OUT);

        //mask = (1 << 7) ;
        read_val = read_val & 0xFFFFFF7F;
        //post_log("Within assert Latch : %08X \n",read_val);




        CPU_WRITE_32(ChipcommonG_GP_DATA_OUT, read_val);
        //TPRNT_micro_second_delay(1);
        //TPRNT_micro_second_delay(1);
        TPRNT_add_delay(1);
}

static void stepper_motor_control (uint8_t );

static void stepper_motor_control (uint8_t phase) {
	uint32_t read_val;

	//uint32_t mask;

       /* Driven as per the timing diagram in SS205-V4-LV technical manual */
	switch (phase) {
		case 0:
			read_val = CPU_READ_32(ASIU_GP_DATA_OUT_3);
			//mask = (1 << 23) | (1 << 22);
		      //  read_val &= ! mask;
				read_val = read_val & 0xFE1FFFFF;
		        read_val |= (1 <<24) | (1 <<21);
		        CPU_WRITE_32(ASIU_GP_DATA_OUT_3, read_val);
			break;
		case 1:
			read_val = CPU_READ_32(ASIU_GP_DATA_OUT_3);
			//mask = (1 << 24) | (1 << 22);
		      //  read_val &= ! mask;
				read_val = read_val & 0xFE1FFFFF;
				read_val |= (1 <<23) | (1 <<24);
		        CPU_WRITE_32(ASIU_GP_DATA_OUT_3, read_val);
			break;
		case 2:
			read_val = CPU_READ_32(ASIU_GP_DATA_OUT_3);
			//mask = (1 << 24) | (1 << 21);
		      //  read_val &= ! mask;
				read_val = read_val & 0xFE1FFFFF;
		        read_val |= (1 <<23) | (1 <<22);
		        CPU_WRITE_32(ASIU_GP_DATA_OUT_3, read_val);
			break;
		case 3:
			read_val = CPU_READ_32(ASIU_GP_DATA_OUT_3);
			//mask = (1 << 23) | (1 << 21);
		        //read_val &= ! mask;
				read_val = read_val & 0xFE1FFFFF;
		        read_val |= (1 <<21) | (1 <<22);
		        CPU_WRITE_32(ASIU_GP_DATA_OUT_3, read_val);
			break;
		default:
			read_val = CPU_READ_32(ASIU_GP_DATA_OUT_3);
			//mask = (1 << 23) | (1 << 22);
		        //read_val &= ! mask;
				read_val = read_val & 0xFE1FFFFF;
		        read_val |= (1 <<24) | (1 <<21);
		        CPU_WRITE_32(ASIU_GP_DATA_OUT_3, read_val);
	}
}

extern void cpu_wr_single(uint32_t addr, uint32_t data, int size);
int TPRNT_post_test(int flags)
{
    uint32_t read_val, mask;

    int i, step;

    cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0, 4);

    post_log("\n Configure GPIO_0_to_3 to be output pins. These 4 GPIOs are used to control the phase of stepper motor\n");
/*Motor Control Configuration *
1.	HMI_DC_GPIO0_3P3 in the main board connected to MOTOR_CTL_1A in the daughter card
2.	HMI_DC_GPIO1_3P3 in the main board connected to MOTOR_CTL_1B in the daughter card
3.	HMI_DC_GPIO2_3P3 in the main board connected to MOTOR_CTL_2A in the daughter card
4.	HMI_DC_GPIO3_3P3 in the main board connected to MOTOR_CTL_2B in the daughter card*/


//-	For the above four signals,
//-	Both AON_GPIO1 and AON_GPIO0 should be driven .1.

	read_val = CPU_READ_32(GP_OUT_EN);
	read_val |= 0x3;
	CPU_WRITE_32(GP_OUT_EN , read_val);

	read_val = CPU_READ_32(GP_DATA_OUT);
        read_val |= 0x3;
        CPU_WRITE_32(GP_DATA_OUT, read_val);


	//Now enable ASIU GPIOs
	//Actual values to be driven on these lines can be achieved from the ASIU GPIO registers for driving the GPIO0_3P3, GPIO1_3P3, GPIO2_3P3 and GPIO3_3P3.
	//This should use ASIU GPIOs 117 - 120.
    read_val = CPU_READ_32(ASIU_GP_OUT_EN_3);
    mask = 0x1e00000;
    read_val |= mask;
    CPU_WRITE_32(ASIU_GP_OUT_EN_3, read_val);


    stepper_motor_control(phase);

//-	AON_GPIO_MODE should be high . .1. and AON_GPIO_EN_L should be low . .0.. (on board) - TBD */


    post_log("\n Configure GPIO_7 as an output pin. This drives the latch signal \n");

    read_val = CPU_READ_32(ChipcommonG_GP_OUT_EN);
    mask = (1 << 7);
    read_val |= mask;
    CPU_WRITE_32(ChipcommonG_GP_OUT_EN, read_val);

    post_log("\n Drive a hi on GPIO-7 to de-assert active low latch singal \n");

    deassert_latch_n();

    post_log("\n Configure GPIOs 21,20,13 and 12 as output pins. These drive the strobe signals \n");

    read_val = CPU_READ_32(ChipcommonG_GP_OUT_EN);
    mask = (1 << 21) | (1 << 20) | (1 << 13) | (1 << 12);
    read_val |= mask;
    CPU_WRITE_32(ChipcommonG_GP_OUT_EN, read_val);

    post_log("\n Drive a hi on GPIOs 21,20,13 and 12 to de-assert active low strobe singals \n");
    deassert_strobe_n();

    post_log("\n\n INFO: Thermal Printer supports 8 MHz max clock \n");

    post_log("\n\n Configuring SPI-0 for Motorola mode, 8-bit data, Master, SPO=0, SPH=0, and for 1Mbps baud \n");

      // ssp_spi0_config(7, 0, 49, 2, 0, 0, 0);
    ssp_spi0_config(SSPCR0_DSS_8, SSPCR0_FRF_MOT, SSPCR0_SCR, SSPCR0_CPSDVSR, SPI_CLOCK_PHASE_1, SPI_CLOCK_POLARITY_1, SSPCR1_MS);

    post_log("\n Spi-0 is Configured. \n");
    
    post_log("\n Thermal latch is driven from GPIO-7. SPI-0_SS should not be connected to Thermal latch.\n");

    //post_getConfirmation("\n Confirm that R12 resistor is not loaded. (Y/N)");

    post_log("\n Configure GPIO_5 to be an output pin and drive a '1'. This is to turn on printer head voltage circuitry on board\n");

    /*Test code for GPIO6 */
#if 0

        read_val = CPU_READ_32(ChipcommonG_GP_OUT_EN);
        mask     = (1 << 6);
        read_val |= mask;
        CPU_WRITE_32(ChipcommonG_GP_OUT_EN, read_val);

        read_val = CPU_READ_32(ChipcommonG_GP_DATA_OUT);
        mask = (1 << 6);
        read_val |= mask;
        CPU_WRITE_32(ChipcommonG_GP_DATA_OUT, read_val);

        TPRNT_add_delay(2000);
        //100 count for 1 micro.s
        //100000 count for 1 millisec.

        read_val = CPU_READ_32(ChipcommonG_GP_DATA_OUT);
        mask = (1 << 6);
        read_val = read_val & (~mask);
        CPU_WRITE_32(ChipcommonG_GP_DATA_OUT, read_val);

        TPRNT_add_delay(6000);

        read_val = CPU_READ_32(ChipcommonG_GP_DATA_OUT);
        mask = (1 << 6);
        read_val |= mask;
        CPU_WRITE_32(ChipcommonG_GP_DATA_OUT, read_val);
#endif


    /********test code ends **************/

    read_val = CPU_READ_32(ChipcommonG_GP_OUT_EN);
    mask     = (1 << 5);
    read_val |= mask;

    CPU_WRITE_32(ChipcommonG_GP_OUT_EN, read_val);

    read_val = CPU_READ_32(ChipcommonG_GP_DATA_OUT);
    mask = (1 << 5);
    read_val |= mask;

    CPU_WRITE_32(ChipcommonG_GP_DATA_OUT, read_val);

    read_val = CPU_READ_32(ChipcommonG_GP_OUT_EN);
	post_log("Printer power enabled ....\n");
    post_log("ChipcommonG_GP_OUT_EN WITH GPIO_5 : %08X \n",read_val);

    read_val = CPU_READ_32(ChipcommonG_GP_DATA_OUT);
    post_log("GP_DATA_OUT WITH GPIO_5 : %08X \n",read_val);

   // uint32_t user_ip = 0;
    //do{
    for (i=0; i < 24; i++) {
    	TPRNT_add_delay(30);
	    send_line_data(i);
	    deassert_strobe_n();
	    TPRNT_add_delay(12);
        assert_latch_n();
        TPRNT_add_delay(12);
        deassert_latch_n();
        TPRNT_add_delay(10);
        assert_strobe_n();
        TPRNT_add_delay(5);
        TPRNT_add_delay(4000);
	    for(step=0; step < 2; step++) {
	            if (phase == 3) {
		            phase = 0;
	            } else {
		            phase++;
	            }
                    stepper_motor_control(phase);
                    read_val = CPU_READ_32(ChipcommonG_GP_OUT_EN);
                        //post_log("ChipcommonG_GP_OUT_EN WITH GPIO_5 : %08X \n",read_val);

                        read_val = CPU_READ_32(ChipcommonG_GP_DATA_OUT);
                        //post_log("GP_DATA_OUT WITH GPIO_5 : %08X \n",read_val);
                        //TPRNT_milli_second_delay(1);
                        TPRNT_add_delay(8000);
	    }

    }
    deassert_strobe_n();
   // user_ip = post_getUserResponse("Do you want to continue (Y/N) : ");
   // }while(user_ip == 0);

    read_val = CPU_READ_32(ChipcommonG_GP_DATA_OUT);
    mask = (1 << 5);
	mask = ~mask;
    read_val = read_val & mask;

    CPU_WRITE_32(ChipcommonG_GP_DATA_OUT, read_val);
    read_val = CPU_READ_32(ChipcommonG_GP_OUT_EN);
    post_log("Printer power disabled ....\n");

    /*Keep the motor running running! Keep the motor running*/
    /* Inorder for the paper to roll enough to display the print turn the motor such that paper moves by 1 cm*/
    /*2 steps per dot line; 8 dot per mm and 10 mm per cm*/
    TPRNT_add_delay(4000);

    for (i=0; i< 80; i++) {
		//TPRNT_milli_second_delay(2);
    	TPRNT_add_delay(4000);
	    for(step=0; step < 2; step++) {
	            if (phase == 3) {
		            phase = 0;
	            } else {
		            phase++;
	            }
	                stepper_motor_control(phase);
                    read_val = CPU_READ_32(ChipcommonG_GP_OUT_EN);
                        //post_log("ChipcommonG_GP_OUT_EN WITH GPIO_5 : %08X \n",read_val);

                        read_val = CPU_READ_32(ChipcommonG_GP_DATA_OUT);
                        //post_log("GP_DATA_OUT WITH GPIO_5 : %08X \n",read_val);
                        //TPRNT_milli_second_delay(1);
                        TPRNT_add_delay(2000);
	    }

    }

	read_val = CPU_READ_32(ASIU_GP_DATA_OUT_3);
    mask = 0x1e00000;
	mask = ~mask;
    read_val = read_val & mask;
    CPU_WRITE_32(ASIU_GP_DATA_OUT_3, read_val);

	read_val = CPU_READ_32(ASIU_GP_DATA_OUT_3);

	post_log("Motor Drive GPIO set LOW ....%8x\n",read_val);


    post_getConfirmation("\n  Do you see CYGNUS being printed on the paper? (Y/N)");
    //post_log("\n\r Thermal Printer Diag Passed\n");
    return 0;
    
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_TPRNT */
#endif

