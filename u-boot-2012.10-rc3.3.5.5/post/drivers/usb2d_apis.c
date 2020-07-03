#include <common.h>
#include <post.h>

#if defined(CONFIG_CYGNUS)
#if CONFIG_POST & CONFIG_SYS_POST_USB20

#include "asm/arch/socregs.h"
#include "asm/arch/reg_utils.h"
#include "usb2d_apis.h"

#define error_log printf
#define print_log printf
#define rd(x) reg32_read((volatile uint32_t *)x)
#define wr(x, v) reg32_write((volatile uint32_t *)x, v)


uint32_t *descAddr, *dataAddr;
usbSetupCmd_t *setupBuffer;
uint32_t *testDescAddr;
uint32_t *testDataAddr;

dEpConfig_s EpOutConfig[DEV_IMPLEMENTED_OUT_EPS];
dEpConfig_s EpInConfig[DEV_IMPLEMENTED_IN_EPS];

usb_device_info_t usbDevInfo;
testInfo_s devTestInfo;

uint32_t getEpBufOrMpktSizeRegAddr(uint32_t epNum, uint32_t epDir){
  if(epDir == 0) { //OUT EP
    if(epNum == 0)  { return USB2D_ENDPNT_OUT_BUFSIZE_0; }
    if(epNum == 1)  { return USB2D_ENDPNT_OUT_BUFSIZE_1; }
    if(epNum == 2)  { return USB2D_ENDPNT_OUT_BUFSIZE_2; }
    if(epNum == 3)  { return USB2D_ENDPNT_OUT_BUFSIZE_3; }
    if(epNum == 4)  { return USB2D_ENDPNT_OUT_BUFSIZE_4; }
  } else {
    if(epNum == 0)  { return USB2D_ENDPNT_IN_MAXPACKSIZE_0; }
    if(epNum == 1)  { return USB2D_ENDPNT_IN_MAXPACKSIZE_1; }
    if(epNum == 2)  { return USB2D_ENDPNT_IN_MAXPACKSIZE_2; }
    if(epNum == 3)  { return USB2D_ENDPNT_IN_MAXPACKSIZE_3; }
    if(epNum == 4)  { return USB2D_ENDPNT_IN_MAXPACKSIZE_4; }
  }
	return 0;
}

uint32_t getEpControlRegAddr(uint32_t epNum, uint32_t epDir) {
  if(epDir == 0) { //OUT EP
    if(epNum == 0)  { return USB2D_ENDPNT_OUT_CTRL_0; }
    if(epNum == 1)  { return USB2D_ENDPNT_OUT_CTRL_1; }
    if(epNum == 2)  { return USB2D_ENDPNT_OUT_CTRL_2; }
    if(epNum == 3)  { return USB2D_ENDPNT_OUT_CTRL_3; }
    if(epNum == 4)  { return USB2D_ENDPNT_OUT_CTRL_4; }
  } else {
    if(epNum == 0)  { return USB2D_ENDPNT_IN_CTRL_0; }
    if(epNum == 1)  { return USB2D_ENDPNT_IN_CTRL_1; }
    if(epNum == 2)  { return USB2D_ENDPNT_IN_CTRL_2; }
    if(epNum == 3)  { return USB2D_ENDPNT_IN_CTRL_3; }
    if(epNum == 4)  { return USB2D_ENDPNT_IN_CTRL_4; }
  }
	return 0;
}

uint32_t setEpDetails(uint32_t epNum, uint32_t epDir, uint32_t epType, uint32_t epMpkt){
  //Set the EP details based on EP-NUMBER, EP-DIRECTION,EP-TYPE and EP-MAXPKTSIZE
  uint32_t epCtrlRegAddr;
  uint32_t epBufOrMpktRegAddr;
  uint32_t reg_data;

  epCtrlRegAddr = getEpControlRegAddr(epNum,epDir);
  epBufOrMpktRegAddr = getEpBufOrMpktSizeRegAddr(epNum,epDir);

//  print_log("Writing for EpNum = %d :::: DIR = %d :::: epType = %d :::: epMpkt = %x\n",epNum,epDir,epType,epMpkt);
//  print_log("epCtrlRegAddr = %x epBufOrMpktRegAddr = %x\n",epCtrlRegAddr,epBufOrMpktRegAddr);

  reg_data = rd(epCtrlRegAddr);
  reg_data &= ~( 0x3 << USB2D_ENDPNT_IN_CTRL_1__ET_R);
  reg_data |=  ( (epType & 0x3) << USB2D_ENDPNT_IN_CTRL_1__ET_R);
  wr(epCtrlRegAddr,reg_data);

  wr(epBufOrMpktRegAddr,epMpkt);
  return 0;
}

uint32_t setEpConfigDetails(uint32_t config){
  //Default Ep details assumed
  //EP0 - OUT Control MPKT - 64
  //EP1 - OUT INTR    MPKT - 64
  //EP2 - OUT ISO     MPKT - 64
  //EP3 - OUT BULK    MPKT - 64
  //EP0 - IN  CONTROL MPKT - 64
  //EP1 - IN  INTR    MPKT - 64
  //EP2 - IN  ISO     MPKT - 64
  //EP3 - IN  BULK    MPKT - 64
  
  
  uint32_t i;
  //uint32_t reg_data;
  //SETTING EP TYPE AS BELOW

  if(config == 0){
    //DEFAULT CONFIGURATION
    for(i = 0 ; i < DEV_IMPLEMENTED_OUT_EPS; i++) {
      if(i == 0) { EpOutConfig[i].epType = 0; EpOutConfig[i].epMaxPktSize = 64; } //CONTROL
      if(i == 1) { EpOutConfig[i].epType = 3; EpOutConfig[i].epMaxPktSize = 64; } //INTR
      if(i == 2) { EpOutConfig[i].epType = 1; EpOutConfig[i].epMaxPktSize = 64; } //ISO
      if(i == 3) { EpOutConfig[i].epType = 2; EpOutConfig[i].epMaxPktSize = 64; } //BULK
    }
    for(i = 0 ; i < DEV_IMPLEMENTED_IN_EPS; i++) {
      if(i == 0) { EpInConfig[i].epType = 0; EpInConfig[i].epMaxPktSize = 64; } //CONTROL 
      if(i == 1) { EpInConfig[i].epType = 3; EpInConfig[i].epMaxPktSize = 64; } //INTR
      if(i == 2) { EpInConfig[i].epType = 1; EpInConfig[i].epMaxPktSize = 64; } //ISO
      if(i == 3) { EpInConfig[i].epType = 2; EpInConfig[i].epMaxPktSize = 64; } //BULK
    }
  } else if(config == 1) {
    //CONFIGURATION - 1 : BULK I/O MPKT 512 
//    print_log("Setting COnfig -1 parameters\n");
    for(i = 0 ; i < DEV_IMPLEMENTED_OUT_EPS; i++) {
      if(i == 0) { EpOutConfig[i].epType = 0; EpOutConfig[i].epMaxPktSize = 64; } //CONTROL
      if(i == 1) { EpOutConfig[i].epType = 3; EpOutConfig[i].epMaxPktSize = 64; } //INTR
      if(i == 2) { EpOutConfig[i].epType = 1; EpOutConfig[i].epMaxPktSize = 64; } //ISO
      if(i == 3) { EpOutConfig[i].epType = 2; EpOutConfig[i].epMaxPktSize = 512;}//64; } //BULK
    }
    for(i = 0 ; i < DEV_IMPLEMENTED_IN_EPS; i++) {
      if(i == 0) { EpInConfig[i].epType = 0; EpInConfig[i].epMaxPktSize = 64; } //CONTROL 
      if(i == 1) { EpInConfig[i].epType = 3; EpInConfig[i].epMaxPktSize = 64; } //INTR
      if(i == 2) { EpInConfig[i].epType = 1; EpInConfig[i].epMaxPktSize = 64; } //ISO
      if(i == 3) { EpInConfig[i].epType = 2; EpInConfig[i].epMaxPktSize = 512;} //64; } //BULK
    }
  } else if(config == 2)  {
    //DEFAULT CONFIGURATION
    for(i = 0 ; i < DEV_IMPLEMENTED_OUT_EPS; i++) {
      if(i == 0) { EpOutConfig[i].epType = 0; EpOutConfig[i].epMaxPktSize = 64; } //CONTROL
      if(i == 1) { EpOutConfig[i].epType = 3; EpOutConfig[i].epMaxPktSize = 512; } //INTR
      if(i == 2) { EpOutConfig[i].epType = 1; EpOutConfig[i].epMaxPktSize = 512; } //ISO
      if(i == 3) { EpOutConfig[i].epType = 2; EpOutConfig[i].epMaxPktSize = 512; } //BULK
    }
    for(i = 0 ; i < DEV_IMPLEMENTED_IN_EPS; i++) {
      if(i == 0) { EpInConfig[i].epType = 0; EpInConfig[i].epMaxPktSize = 64; } //CONTROL 
      if(i == 1) { EpInConfig[i].epType = 3; EpInConfig[i].epMaxPktSize = 512; } //INTR
      if(i == 2) { EpInConfig[i].epType = 1; EpInConfig[i].epMaxPktSize = 512; } //ISO
      if(i == 3) { EpInConfig[i].epType = 2; EpInConfig[i].epMaxPktSize = 512; } //BULK
    }
  } else if(config == 3)  {
    //DEFAULT CONFIGURATION
    for(i = 0 ; i < DEV_IMPLEMENTED_OUT_EPS; i++) {
      if(i == 0) { EpOutConfig[i].epType = 0; EpOutConfig[i].epMaxPktSize = 64; } //CONTROL
      if(i == 1) { EpOutConfig[i].epType = 3; EpOutConfig[i].epMaxPktSize = 1024; } //INTR
      if(i == 2) { EpOutConfig[i].epType = 1; EpOutConfig[i].epMaxPktSize = 1024; } //ISO
      if(i == 3) { EpOutConfig[i].epType = 2; EpOutConfig[i].epMaxPktSize = 512; } //BULK
    }
    for(i = 0 ; i < DEV_IMPLEMENTED_IN_EPS; i++) {
      if(i == 0) { EpInConfig[i].epType = 0; EpInConfig[i].epMaxPktSize = 64; } //CONTROL 
      if(i == 1) { EpInConfig[i].epType = 3; EpInConfig[i].epMaxPktSize = 1024; } //INTR
      if(i == 2) { EpInConfig[i].epType = 1; EpInConfig[i].epMaxPktSize = 1024; } //ISO
      if(i == 3) { EpInConfig[i].epType = 2; EpInConfig[i].epMaxPktSize = 512; } //BULK
    }
  } else if(config == 4)  {
    //DEFAULT CONFIGURATION
    for(i = 0 ; i < DEV_IMPLEMENTED_OUT_EPS; i++) {
      if(i == 0) { EpOutConfig[i].epType = 0; EpOutConfig[i].epMaxPktSize = 64; } //CONTROL
      if(i == 1) { EpOutConfig[i].epType = 3; EpOutConfig[i].epMaxPktSize = 128; } //INTR
      if(i == 2) { EpOutConfig[i].epType = 1; EpOutConfig[i].epMaxPktSize = 128; } //ISO
      if(i == 3) { EpOutConfig[i].epType = 2; EpOutConfig[i].epMaxPktSize = 128; } //BULK
    }
    for(i = 0 ; i < DEV_IMPLEMENTED_IN_EPS; i++) {
      if(i == 0) { EpInConfig[i].epType = 0; EpInConfig[i].epMaxPktSize = 64; } //CONTROL 
      if(i == 1) { EpInConfig[i].epType = 3; EpInConfig[i].epMaxPktSize = 128; } //INTR
      if(i == 2) { EpInConfig[i].epType = 1; EpInConfig[i].epMaxPktSize = 128; } //ISO
      if(i == 3) { EpInConfig[i].epType = 2; EpInConfig[i].epMaxPktSize = 128; } //BULK
    }
  } else if(config == 5)  {
    //DEFAULT CONFIGURATION
    for(i = 0 ; i < DEV_IMPLEMENTED_OUT_EPS; i++) {
      if(i == 0) { EpOutConfig[i].epType = 0; EpOutConfig[i].epMaxPktSize = 64; } //CONTROL
      if(i == 1) { EpOutConfig[i].epType = 3; EpOutConfig[i].epMaxPktSize = 256; } //INTR
      if(i == 2) { EpOutConfig[i].epType = 1; EpOutConfig[i].epMaxPktSize = 256; } //ISO
      if(i == 3) { EpOutConfig[i].epType = 2; EpOutConfig[i].epMaxPktSize = 256; } //BULK
    }
    for(i = 0 ; i < DEV_IMPLEMENTED_IN_EPS; i++) {
      if(i == 0) { EpInConfig[i].epType = 0; EpInConfig[i].epMaxPktSize = 64; } //CONTROL 
      if(i == 1) { EpInConfig[i].epType = 3; EpInConfig[i].epMaxPktSize = 256; } //INTR
      if(i == 2) { EpInConfig[i].epType = 1; EpInConfig[i].epMaxPktSize = 256; } //ISO
      if(i == 3) { EpInConfig[i].epType = 2; EpInConfig[i].epMaxPktSize = 256; } //BULK
    }
  }

//  print_log("SETTING EP BUFFER/MPKT DETAILS\n");
  //Fill the respective registers
  for(i = 1 ; i < DEV_IMPLEMENTED_OUT_EPS; i++) {
    setEpDetails(i,0,EpOutConfig[i].epType,EpOutConfig[i].epMaxPktSize);
  }

  for(i = 1 ; i < DEV_IMPLEMENTED_IN_EPS; i++) {
    setEpDetails(i,1,EpInConfig[i].epType ,EpInConfig[i].epMaxPktSize);
  }

  wr(USB2D_ENDPNT_IN_BUFFER_0,16);
  //wr(USB2D_ENDPNT_IN_BUFFER_3,128);
  wr(USB2D_ENDPNT_IN_BUFFER_3,256);

//  print_log("EP  IN CONFIG - MPS = 0x%08x\n", EpInConfig[3].epMaxPktSize);
//  print_log("EP OUT CONFIG - MPS = 0x%08x\n",EpOutConfig[3].epMaxPktSize);


  //MAP UDC registers with these values

//  for(i=0; i<10; i++) {
 //     reg_data = rd(CDRU_USBPHY_P2_STATUS);
//      print_log("CDRU_USBPHY_P2_STATUS = 0x%08X\n", reg_data);
 //     reg_data = rd(CDRU_USBPHY_P2_CTRL_0);
//      print_log("CDRU_USBPHY_P2_CTRL_0 = 0x%08X\n", reg_data);
 // }

//  print_log("-------- 1 ---------------\n");
  wr(USB2D_UDC20_ENDPOINT0, (64 << USB2D_UDC20_ENDPOINT0__MPS_R));
  wr(USB2D_UDC20_ENDPOINT1, ((EpOutConfig[1].epMaxPktSize << USB2D_UDC20_ENDPOINT0__MPS_R) | (1 << USB2D_UDC20_ENDPOINT0__CNW_R) | ( EpOutConfig[1].epType << USB2D_UDC20_ENDPOINT0__ET_R) | ( 0 << USB2D_UDC20_ENDPOINT0__ED) | ( 0x1 <<  USB2D_UDC20_ENDPOINT0__LEN_R)));
  wr(USB2D_UDC20_ENDPOINT2, ((EpInConfig[1].epMaxPktSize  << USB2D_UDC20_ENDPOINT0__MPS_R) | (1 << USB2D_UDC20_ENDPOINT0__CNW_R) | ( EpInConfig[1].epType  << USB2D_UDC20_ENDPOINT0__ET_R) | ( 1 << USB2D_UDC20_ENDPOINT0__ED) | ( 0x1 <<  USB2D_UDC20_ENDPOINT0__LEN_R)));
  wr(USB2D_UDC20_ENDPOINT3, ((EpOutConfig[2].epMaxPktSize << USB2D_UDC20_ENDPOINT0__MPS_R) | (1 << USB2D_UDC20_ENDPOINT0__CNW_R) | ( EpOutConfig[2].epType << USB2D_UDC20_ENDPOINT0__ET_R) | ( 0 << USB2D_UDC20_ENDPOINT0__ED) | ( 0x2 <<  USB2D_UDC20_ENDPOINT0__LEN_R)));
  wr(USB2D_UDC20_ENDPOINT4, ((EpInConfig[2].epMaxPktSize  << USB2D_UDC20_ENDPOINT0__MPS_R) | (1 << USB2D_UDC20_ENDPOINT0__CNW_R) | ( EpInConfig[3].epType  << USB2D_UDC20_ENDPOINT0__ET_R) | ( 1 << USB2D_UDC20_ENDPOINT0__ED) | ( 0x2 <<  USB2D_UDC20_ENDPOINT0__LEN_R)));
  wr(USB2D_UDC20_ENDPOINT5, ((EpOutConfig[3].epMaxPktSize << USB2D_UDC20_ENDPOINT0__MPS_R) | (1 << USB2D_UDC20_ENDPOINT0__CNW_R) | ( EpOutConfig[3].epType << USB2D_UDC20_ENDPOINT0__ET_R) | ( 0 << USB2D_UDC20_ENDPOINT0__ED) | ( 0x3 <<  USB2D_UDC20_ENDPOINT0__LEN_R)));
  wr(USB2D_UDC20_ENDPOINT6, ((EpInConfig[3].epMaxPktSize  << USB2D_UDC20_ENDPOINT0__MPS_R) | (1 << USB2D_UDC20_ENDPOINT0__CNW_R) | ( EpInConfig[3].epType  << USB2D_UDC20_ENDPOINT0__ET_R) | ( 1 << USB2D_UDC20_ENDPOINT0__ED) | ( 0x3 <<  USB2D_UDC20_ENDPOINT0__LEN_R)));
//  print_log("-------- 2 ---------------\n");
  return 0;
}

uint32_t setEpControlReg(uint32_t regIndex, uint32_t regData){
  //FILL EP REGISTERS WITH GIVEN DESC regDataESS AND INDEX
  //INDEX  0-15  IN BUFFERS/DESCRIPTORS
  //INDEX 16-31 OUT BUFFERS/DESCRIPTORS
  //NOTE THAT WE USE MAX 8 EP's mearly 4 IN and 4 OUT
  uint32_t inEp;
  uint32_t outEp;

   inEp = 0;
  outEp = 0;

  if( regIndex < 16) inEp = 1;
  if( regIndex >= 16 ) outEp = 1;

  if(inEp){
    if( regIndex ==  DEV_EP_IN_OFFSET + 0) wr(USB2D_ENDPNT_IN_CTRL_0,regData);
    if( regIndex ==  DEV_EP_IN_OFFSET + 1) wr(USB2D_ENDPNT_IN_CTRL_1,regData);
    if( regIndex ==  DEV_EP_IN_OFFSET + 2) wr(USB2D_ENDPNT_IN_CTRL_2,regData);
    if( regIndex ==  DEV_EP_IN_OFFSET + 3) wr(USB2D_ENDPNT_IN_CTRL_3,regData);
    if( regIndex ==  DEV_EP_IN_OFFSET + 4) wr(USB2D_ENDPNT_IN_CTRL_4,regData);
    if( regIndex ==  DEV_EP_IN_OFFSET + 5) wr(USB2D_ENDPNT_IN_CTRL_5,regData);
    if( regIndex ==  DEV_EP_IN_OFFSET + 6) wr(USB2D_ENDPNT_IN_CTRL_6,regData);
    if( regIndex ==  DEV_EP_IN_OFFSET + 7) wr(USB2D_ENDPNT_IN_CTRL_7,regData);
    if( regIndex ==  DEV_EP_IN_OFFSET + 8) wr(USB2D_ENDPNT_IN_CTRL_8,regData);
    if( regIndex ==  DEV_EP_IN_OFFSET + 9) wr(USB2D_ENDPNT_IN_CTRL_9,regData);
    //if( regIndex == 10) wr(USB2D_ENDPNT_IN_DATADESCR_0,regData);
    //if( regIndex == 11) wr(USB2D_ENDPNT_IN_DATADESCR_0,regData);
    //if( regIndex == 12) wr(USB2D_ENDPNT_IN_DATADESCR_0,regData);
    //if( regIndex == 13) wr(USB2D_ENDPNT_IN_DATADESCR_0,regData);
    //if( regIndex == 14) wr(USB2D_ENDPNT_IN_DATADESCR_0,regData);
    //if( regIndex == 15) wr(USB2D_ENDPNT_IN_DATADESCR_0,regData);
  }

  if(outEp){
    if( regIndex ==  DEV_EP_OUT_OFFSET + 0) wr(USB2D_ENDPNT_OUT_CTRL_0,regData);
    if( regIndex ==  DEV_EP_OUT_OFFSET + 1) wr(USB2D_ENDPNT_OUT_CTRL_1,regData);
    if( regIndex ==  DEV_EP_OUT_OFFSET + 2) wr(USB2D_ENDPNT_OUT_CTRL_2,regData);
    if( regIndex ==  DEV_EP_OUT_OFFSET + 3) wr(USB2D_ENDPNT_OUT_CTRL_3,regData);
    if( regIndex ==  DEV_EP_OUT_OFFSET + 4) wr(USB2D_ENDPNT_OUT_CTRL_4,regData);
    if( regIndex ==  DEV_EP_OUT_OFFSET + 5) wr(USB2D_ENDPNT_OUT_CTRL_5,regData);
    if( regIndex ==  DEV_EP_OUT_OFFSET + 6) wr(USB2D_ENDPNT_OUT_CTRL_6,regData);
    if( regIndex ==  DEV_EP_OUT_OFFSET + 7) wr(USB2D_ENDPNT_OUT_CTRL_7,regData);
    if( regIndex ==  DEV_EP_OUT_OFFSET + 8) wr(USB2D_ENDPNT_OUT_CTRL_8,regData);
    if( regIndex ==  DEV_EP_OUT_OFFSET + 9) wr(USB2D_ENDPNT_OUT_CTRL_9,regData);
    //if( regIndex == 10) wr(USB2D_ENDPNT_IN_DATADESCR_0,regData);
    //if( regIndex == 11) wr(USB2D_ENDPNT_IN_DATADESCR_0,regData);
    //if( regIndex == 12) wr(USB2D_ENDPNT_IN_DATADESCR_0,regData);
    //if( regIndex == 13) wr(USB2D_ENDPNT_IN_DATADESCR_0,regData);
    //if( regIndex == 14) wr(USB2D_ENDPNT_IN_DATADESCR_0,regData);
    //if( regIndex == 15) wr(USB2D_ENDPNT_IN_DATADESCR_0,regData);
  }

  return 1;
}

uint32_t getEpControlReg(uint32_t regIndex){
  //FILL EP REGISTERS WITH GIVEN DESC regDataESS AND INDEX
  //INDEX  0-15  IN BUFFERS/DESCRIPTORS
  //INDEX 16-31 OUT BUFFERS/DESCRIPTORS
  //NOTE THAT WE USE MAX 8 EP's mearly 4 IN and 4 OUT
  uint32_t inEp;
  uint32_t outEp;

   inEp = 0;
  outEp = 0;

  if( regIndex < 16) inEp = 1;
  if( regIndex >= 16 ) outEp = 1;

  if(inEp){
    if( regIndex ==  DEV_EP_IN_OFFSET + 0) return rd(USB2D_ENDPNT_IN_CTRL_0);
    if( regIndex ==  DEV_EP_IN_OFFSET + 1) return rd(USB2D_ENDPNT_IN_CTRL_1);
    if( regIndex ==  DEV_EP_IN_OFFSET + 2) return rd(USB2D_ENDPNT_IN_CTRL_2);
    if( regIndex ==  DEV_EP_IN_OFFSET + 3) return rd(USB2D_ENDPNT_IN_CTRL_3);
    if( regIndex ==  DEV_EP_IN_OFFSET + 4) return rd(USB2D_ENDPNT_IN_CTRL_4);
    if( regIndex ==  DEV_EP_IN_OFFSET + 5) return rd(USB2D_ENDPNT_IN_CTRL_5);
    if( regIndex ==  DEV_EP_IN_OFFSET + 6) return rd(USB2D_ENDPNT_IN_CTRL_6);
    if( regIndex ==  DEV_EP_IN_OFFSET + 7) return rd(USB2D_ENDPNT_IN_CTRL_7);
    if( regIndex ==  DEV_EP_IN_OFFSET + 8) return rd(USB2D_ENDPNT_IN_CTRL_8);
    if( regIndex ==  DEV_EP_IN_OFFSET + 9) return rd(USB2D_ENDPNT_IN_CTRL_9);
    //if( regIndex == 10) rd(USB2D_ENDPNT_IN_DATADESCR_0);
    //if( regIndex == 11) rd(USB2D_ENDPNT_IN_DATADESCR_0);
    //if( regIndex == 12) rd(USB2D_ENDPNT_IN_DATADESCR_0);
    //if( regIndex == 13) rd(USB2D_ENDPNT_IN_DATADESCR_0);
    //if( regIndex == 14) rd(USB2D_ENDPNT_IN_DATADESCR_0);
    //if( regIndex == 15) rd(USB2D_ENDPNT_IN_DATADESCR_0);
  }

  if(outEp){
    if( regIndex ==  DEV_EP_OUT_OFFSET + 0) return rd(USB2D_ENDPNT_OUT_CTRL_0);
    if( regIndex ==  DEV_EP_OUT_OFFSET + 1) return rd(USB2D_ENDPNT_OUT_CTRL_1);
    if( regIndex ==  DEV_EP_OUT_OFFSET + 2) return rd(USB2D_ENDPNT_OUT_CTRL_2);
    if( regIndex ==  DEV_EP_OUT_OFFSET + 3) return rd(USB2D_ENDPNT_OUT_CTRL_3);
    if( regIndex ==  DEV_EP_OUT_OFFSET + 4) return rd(USB2D_ENDPNT_OUT_CTRL_4);
    if( regIndex ==  DEV_EP_OUT_OFFSET + 5) return rd(USB2D_ENDPNT_OUT_CTRL_5);
    if( regIndex ==  DEV_EP_OUT_OFFSET + 6) return rd(USB2D_ENDPNT_OUT_CTRL_6);
    if( regIndex ==  DEV_EP_OUT_OFFSET + 7) return rd(USB2D_ENDPNT_OUT_CTRL_7);
    if( regIndex ==  DEV_EP_OUT_OFFSET + 8) return rd(USB2D_ENDPNT_OUT_CTRL_8);
    if( regIndex ==  DEV_EP_OUT_OFFSET + 9) return rd(USB2D_ENDPNT_OUT_CTRL_9);
    //if( regIndex == 10) rd(USB2D_ENDPNT_IN_DATADESCR_0);
    //if( regIndex == 11) rd(USB2D_ENDPNT_IN_DATADESCR_0);
    //if( regIndex == 12) rd(USB2D_ENDPNT_IN_DATADESCR_0);
    //if( regIndex == 13) rd(USB2D_ENDPNT_IN_DATADESCR_0);
    //if( regIndex == 14) rd(USB2D_ENDPNT_IN_DATADESCR_0);
    //if( regIndex == 15) rd(USB2D_ENDPNT_IN_DATADESCR_0);
  }

  return 0;
}


uint32_t fillEpDataDescReg(uint32_t addr, uint32_t regIndex){
  //FILL EP REGISTERS WITH GIVEN DESC ADDRESS AND INDEX
  //INDEX  0-15  IN BUFFERS/DESCRIPTORS
  //INDEX 16-31 OUT BUFFERS/DESCRIPTORS
  //NOTE THAT WE USE MAX 8 EP's mearly 4 IN and 4 OUT
  uint32_t inEp;
  uint32_t outEp;

   inEp = 0;
  outEp = 0;

  if( regIndex < 16) inEp = 1;
  if( regIndex >= 16 ) outEp = 1;

  if(inEp){
    if( regIndex ==  DEV_EP_IN_OFFSET + 0) wr(USB2D_ENDPNT_IN_DATADESCR_0,addr);
    if( regIndex ==  DEV_EP_IN_OFFSET + 1) wr(USB2D_ENDPNT_IN_DATADESCR_1,addr);
    if( regIndex ==  DEV_EP_IN_OFFSET + 2) wr(USB2D_ENDPNT_IN_DATADESCR_2,addr);
    if( regIndex ==  DEV_EP_IN_OFFSET + 3) wr(USB2D_ENDPNT_IN_DATADESCR_3,addr);
    if( regIndex ==  DEV_EP_IN_OFFSET + 4) wr(USB2D_ENDPNT_IN_DATADESCR_4,addr);
    if( regIndex ==  DEV_EP_IN_OFFSET + 5) wr(USB2D_ENDPNT_IN_DATADESCR_5,addr);
    if( regIndex ==  DEV_EP_IN_OFFSET + 6) wr(USB2D_ENDPNT_IN_DATADESCR_6,addr);
    if( regIndex ==  DEV_EP_IN_OFFSET + 7) wr(USB2D_ENDPNT_IN_DATADESCR_7,addr);
    if( regIndex ==  DEV_EP_IN_OFFSET + 8) wr(USB2D_ENDPNT_IN_DATADESCR_8,addr);
    if( regIndex ==  DEV_EP_IN_OFFSET + 9) wr(USB2D_ENDPNT_IN_DATADESCR_9,addr);
    //if( regIndex == 10) wr(USB2D_ENDPNT_IN_DATADESCR_0,addr);
    //if( regIndex == 11) wr(USB2D_ENDPNT_IN_DATADESCR_0,addr);
    //if( regIndex == 12) wr(USB2D_ENDPNT_IN_DATADESCR_0,addr);
    //if( regIndex == 13) wr(USB2D_ENDPNT_IN_DATADESCR_0,addr);
    //if( regIndex == 14) wr(USB2D_ENDPNT_IN_DATADESCR_0,addr);
    //if( regIndex == 15) wr(USB2D_ENDPNT_IN_DATADESCR_0,addr);
  }

  if(outEp){
    if( regIndex ==  DEV_EP_OUT_OFFSET + 0) wr(USB2D_ENDPNT_OUT_DATADESCR_0,addr);
    if( regIndex ==  DEV_EP_OUT_OFFSET + 1) wr(USB2D_ENDPNT_OUT_DATADESCR_1,addr);
    if( regIndex ==  DEV_EP_OUT_OFFSET + 2) wr(USB2D_ENDPNT_OUT_DATADESCR_2,addr);
    if( regIndex ==  DEV_EP_OUT_OFFSET + 3) wr(USB2D_ENDPNT_OUT_DATADESCR_3,addr);
    if( regIndex ==  DEV_EP_OUT_OFFSET + 4) wr(USB2D_ENDPNT_OUT_DATADESCR_4,addr);
    if( regIndex ==  DEV_EP_OUT_OFFSET + 5) wr(USB2D_ENDPNT_OUT_DATADESCR_5,addr);
    if( regIndex ==  DEV_EP_OUT_OFFSET + 6) wr(USB2D_ENDPNT_OUT_DATADESCR_6,addr);
    if( regIndex ==  DEV_EP_OUT_OFFSET + 7) wr(USB2D_ENDPNT_OUT_DATADESCR_7,addr);
    if( regIndex ==  DEV_EP_OUT_OFFSET + 8) wr(USB2D_ENDPNT_OUT_DATADESCR_8,addr);
    if( regIndex ==  DEV_EP_OUT_OFFSET + 9) wr(USB2D_ENDPNT_OUT_DATADESCR_9,addr);
    //if( regIndex == 10) wr(USB2D_ENDPNT_IN_DATADESCR_0,addr);
    //if( regIndex == 11) wr(USB2D_ENDPNT_IN_DATADESCR_0,addr);
    //if( regIndex == 12) wr(USB2D_ENDPNT_IN_DATADESCR_0,addr);
    //if( regIndex == 13) wr(USB2D_ENDPNT_IN_DATADESCR_0,addr);
    //if( regIndex == 14) wr(USB2D_ENDPNT_IN_DATADESCR_0,addr);
    //if( regIndex == 15) wr(USB2D_ENDPNT_IN_DATADESCR_0,addr);
  }

  return 1;
}



uint32_t USB_DEV_INIT(void){

  usbDevDataMem_t   *dDDM;
  usbSetupDataMem_t *setupBufDes;

  uint32_t reg_data;
  uint32_t epControl;
  uint32_t i;


    ////////   USB2D_DEV_EP_INTMASK.in_ep_mask = 16'h0;
    ////////   USB2D_DEV_EP_INTMASK.out_ep_mask = 16'h0;
    ////////   tbc.sw_driver.usb2d.usb2d_reg_write ( USB2D_DEV_EP_INTMASK.adr, USB2D_DEV_EP_INTMASK.pack_b());


  //Program device config register
  reg_data  = rd(USB2D_DEVCONFIG);
  reg_data |=  ( 1 << USB2D_DEVCONFIG__SET_DESC); 
  reg_data &= ~( 1 << USB2D_DEVCONFIG__DIR); //0
  reg_data |=  ( 1 << USB2D_DEVCONFIG__PI);  //1
  reg_data |=  ( 1 << USB2D_DEVCONFIG__SP);  //1
  wr(USB2D_DEVCONFIG,reg_data);
//  print_log("Seting DEVICE CONFIG as %0x\n",reg_data);

  //Enable DMA Mode and other features 
  reg_data  = rd(USB2D_DEVCTRL);
  reg_data |= (1 << USB2D_DEVCTRL__MODE);
  //reg_data |= (1 << USB2D_DEVCTRL__TDE);
  //reg_data |= (1 << USB2D_DEVCTRL__RDE);
  reg_data |= (0xF << USB2D_DEVCTRL__BRLEN_R);
  reg_data |= (0xF << USB2D_DEVCTRL__THLEN_R);
  //reg_data |= (1 << USB2D_DEVCTRL__DU);
  if(devTestInfo.bufferFill) reg_data |= (1 << USB2D_DEVCTRL__BF);
  reg_data |= (1 << USB2D_DEVCTRL__BREN);
  wr(USB2D_DEVCTRL,reg_data);
//  print_log("Setting DEVCTRL = %0x\n",reg_data);


  //UN-MASKING DEVICE INTERRUPTS
  reg_data = rd(USB2D_DEVINTRMASK);
  reg_data &= 0xFFFFFF00;
  wr(USB2D_DEVINTRMASK,reg_data);

  //CLEARING ENDPOINT INTERRUPT MASKS
  wr(USB2D_ENDPNTINTRMASK,0x00000000);

  setEpConfigDetails(0);//Selecting Default EP Config

  //Allocate memory for EP descriptors and TX/RX data
  //descAddr = (uint32_t *)malloc(2 * 16 * (4 * 4)); //Allocating for total 16EP each of 4DW
  //dataAddr = (uint32_t *)malloc(2 * 16 * 512); //Allocating memory of 512 bytes for each EP
  descAddr = (uint32_t *)devTestInfo.usbdDescMem; //(uint32_t *)0x68000000;//malloc(2 * 16 * (4 * 4)); //Allocating for total 16EP each of 4DW
  dataAddr = (uint32_t *)devTestInfo.usbdDescDataMem;//(uint32_t *)0x69000000;//malloc(2 * 16 * 512); //Allocating memory of 512 bytes for each EP
  //setupBuffer = (uint32_t *) malloc(sizeof(usbSetupCmd_t));
  // setupBufDes = (usbSetupDataMem_t *) malloc(sizeof(usbSetupDataMem_t));
  setupBufDes = (usbSetupDataMem_t*)devTestInfo.usbdDescDataMem;//0x6a000000; // (usbSetupDataMem_t *) malloc(sizeof(usbSetupDataMem_t));
  
  memset((void*) descAddr, 0x0,(2 * 16 * (4 * 4)));
  memset((void*) dataAddr, 0x0,(16 * 512));
  //memset((void*) setupBuffer, 0x0,(sizeof(usbSetupCmd_t)));
  memset((void*) setupBufDes, 0x0,(sizeof(usbSetupDataMem_t)));

//  print_log("DESC_ALLOC_ADDR = %0x\n",descAddr);
//  print_log("DATA_ALLOC_ADDR = %0x\n",dataAddr);
//  print_log(" SET_ALLOC_ADDR = %0x\n",setupBufDes);

  //Create descriptors and fill them with data buffers accordingly
  dDDM = (usbDevDataMem_t *) descAddr;

  for(i = 0; i < 32; i++){ //Doing a basic fill of memory for 32 EP's which are like 16-IN and 16-OUT
    //Get the desc address to temp data structure array and fill the contents of memory with them
//    print_log("DescAddr[%0x] = %0x\n",i,(uint32_t)dDDM);

    if( i < 16 ) {
      //IN ENDPOINT
      dDDM->DDS.dDS_val = 0x08000000;
      dDDM->res_0       = 0x00000000;
      dDDM->BufferP     = (uint32_t) dataAddr + (512 * i);
      dDDM->NextDs      = 0x00000000;
    } else {
      //OUT ENDPOINT
      dDDM->DDS.dDS_val = 0x08000000;
      dDDM->res_0       = 0x00000000;
      dDDM->BufferP     = (uint32_t) dataAddr + (512 * (i + 16));
      dDDM->NextDs      = 0x00000000;
    }

    fillEpDataDescReg((uint32_t)dDDM,i); //Filling EP register values for BUFFER pointers with the Descriptor memory address

    epControl = getEpControlReg(i);
    epControl |= (1 << 7); //setting NAK
    setEpControlReg(i,epControl);

    dDDM++;
  }

  setupBufDes->DSS.dSS_val = 0x00000000;
  //Fill SETUPBUFFERDESC and SETUP CMD BUFFER at EP0 IN AREA
  wr(USB2D_ENDPNT_OUT_SETUPBUF_0,(uint32_t)setupBufDes);
  //Enable TDE and RDE   

  reg_data  = rd(USB2D_DEVCTRL);
  reg_data |= (1 << USB2D_DEVCTRL__TDE);
  reg_data |= (1 << USB2D_DEVCTRL__RDE);
  wr(USB2D_DEVCTRL,reg_data);
//  print_log("Setting USB2D_DEVCTRL = %0x\n",reg_data);
 
  return 1;
}

#endif
#endif


