#ifndef USBD_APIS_H
#define USBD_APIS_H

#define CYG   1

#define DEV_EP_IN_OFFSET 0
#define DEV_EP_OUT_OFFSET 16
#define DEV_IMPLEMENTED_OUT_EPS 4
#define DEV_IMPLEMENTED_IN_EPS 4

typedef enum
{
	TEST_PASS     = 0,
	TEST_FAIL     = 1,
	TEST_ERROR    = 2,
	TEST_NOT_INIT = 3
} test_status_t;

typedef enum usbdIntrType {
  DEVINTR,
  EPINTR
} usbdIntrType_t;

typedef enum usbdTestType {
  BULK,
  ISO,
  INTR
} usbdTestType_t;


test_status_t init_usbd(uint32_t argc, uint32_t *argv[]);
uint32_t get_intr_detail_and_clear(uint32_t epIntr);
uint32_t fillEpDataDescReg(uint32_t addr, uint32_t regIndex);
uint32_t setEpControlReg(uint32_t regIndex, uint32_t regData);
uint32_t getEpControlReg(uint32_t regIndex);
uint32_t setEpDetails(uint32_t epNum, uint32_t epdir, uint32_t epType, uint32_t epMpkt);
uint32_t getEpControlRegAddr(uint32_t epNum, uint32_t epDir);
uint32_t getEpBufOrMpktSizeRegAddr(uint32_t epNum, uint32_t epDir);


extern uint32_t *descAddr, *dataAddr;

typedef enum testType
{
  BASIC_BULK_OUT,
  BASIC_INTR_OUT,
  BASIC_ISO_OUT,
  BASIC_BULK_IN,
  BASIC_INTR_IN,
  BASIC_ISO_IN,
  SUSP_RESUME,
  HEAVY_BULK_OUT,       //random size ranging to 60 k
  HEAVY_BULK_IN,        //random size ranging to 60 k
  HEAVY_BULK_OUT_IN     //random size ranging to 60 k 
} testType_t;

typedef struct testInfo {
  testType_t TT;
  uint32_t trSize;
  uint32_t bufferFill;
  uint32_t bulkNumItr;
  uint32_t bulkCurItr;
  uint32_t done;
  uint32_t usbdDescMem; //expect 512K
  uint32_t usbdDescDataMem; //expect 512K
  uint32_t usbdXferMem; //expect 64M
  usbdTestType_t testType;
} testInfo_s;

extern testInfo_s devTestInfo;

typedef struct devTest {
  uint32_t config;
  uint32_t junk; 
} devTest_s;

typedef enum usb_dev_state {DEV_UNCONNECTED,DEV_DEFAULT ,DEV_SUSPENDED, DEV_ENABLED, DEV_ADDRESSED, DEV_CONFIGURED } usb_dev_state_t;

typedef enum bulkTest { BULK_INACTIVE, BULK_ACTIVE } bulkTest_t;
typedef enum intrTest { INTR_INACTIVE, INTR_ACTIVE } intrTest_t;
typedef enum  isoTest {  ISO_INACTIVE,  ISO_ACTIVE }  isoTest_t;

typedef enum transferDir { OUT, IN, BI } transferDir_t;

typedef struct bulkTestInfo {
   bulkTest_t bulkTest;
   transferDir_t dir;
   uint32_t size; //in bytes
   uint32_t done;
   uint32_t EpNum;
   uint32_t EpDt;
} bulkTestInfo_t;

typedef struct intrTestInfo {
   intrTest_t intrTest;
   transferDir_t dir;
   uint32_t size; //in bytes
   uint32_t done;
   uint32_t EpNum;
   uint32_t EpDt;
   uint32_t qH; //qH Address
   uint32_t FrmNum; //scheduled frame index
} intrTestInfo_t;

typedef struct  isoTestInfo {
   isoTest_t  isoTest;
   transferDir_t dir;
   uint32_t size; //in bytes
   uint32_t done;
   uint32_t EpNum;
   uint32_t EpDt;
   uint32_t iTD; //iTD Address
   uint32_t FrmNum; //scheduled frame index
}  isoTestInfo_t;

typedef struct device_info  {
   usb_dev_state_t devState;
   uint32_t devConfig;
   uint32_t devSpeed;

   uint32_t onlyEnumeration;
   uint32_t suspendResume;

   bulkTestInfo_t bulkTestInfo;
   intrTestInfo_t intrTestInfo;
    isoTestInfo_t  isoTestInfo;

} usb_device_info_t;   

extern usb_device_info_t usbDevInfo;

/////////////////////////////////////////////////////
//   DEVICE RELATED DATA or DESCRIPTOR STRUCTURES  //
/////////////////////////////////////////////////////
//DEVICE EP CONFIGURATION STRUCTURE
typedef struct epConfig {
  uint32_t epType; //0-Control, 1-iso, 2-bulk, 3-intr
  uint32_t epMaxPktSize;
} dEpConfig_s;


/////////////////////////////////////////////////////
//   DEVICE SETUP DATA and STATUS DESCRIPTOR       //
/////////////////////////////////////////////////////

typedef struct dSetupStatusQ {
	uint32_t BS       :  2;
	uint32_t RxSts    :  2;
	uint32_t CfgSts   : 12;
	uint32_t Reserved : 16;
} dSetupStatusQ_s; 

typedef union dSetupStatusQ_u {
  dSetupStatusQ_s dSS;
  uint32_t dSS_val;
}dSetupStatusQ_t; 


typedef struct usbSetupDataMem {
  dSetupStatusQ_t DSS;
	uint32_t res_0;
	uint32_t SetupDw0;
	uint32_t SetupDw1;
} usbSetupDataMem_t; 

#define __packed __attribute__((packed))
/////////////////////////////////////////////////////
// DEVICE IN or OUT EP DATA and STATUS DESCRIPTOR  //
/////////////////////////////////////////////////////

typedef  struct dPayload {   //Access this for ISO transfers
  uint16_t Size     : 13;
  uint16_t PID      :  2;
} __packed dPayload_s;

typedef  union dPayload_u {
  dPayload_s isoSize;
  uint16_t   Size;
}__packed dPayload_t;

typedef  struct dDataStatusQ {
	dPayload_t PayloadSize    ;  // Access 16 bit for non ISO
	uint32_t FrmNum   : 11;
	uint32_t L        :  1;
	uint32_t TsfrSts  :  2;
	uint32_t BS       :  2;
}__packed dDataStatusQ_s; 

typedef  union dDataStatusQ_u {
  dDataStatusQ_s dDS;
  uint32_t dDS_val;
}__packed dDataStatusQ_t; 


typedef struct usbDevDataMem {
  dDataStatusQ_t DDS;
	uint32_t res_0;
	uint32_t BufferP;
	uint32_t NextDs;
} usbDevDataMem_t; 

/////////////////////////////////////////////////////
//       USB DEVICE SETUP DATA STRUCTURE           //
/////////////////////////////////////////////////////

typedef struct usbSetupCmd {
  uint32_t bmRequestType : 8 ;
  uint32_t bRequest      : 8 ;
  uint32_t wValue        : 16 ;
  uint32_t wIndex        : 16 ;
  uint32_t wLength       : 16 ;

} usbSetupCmd_t;

extern usbSetupCmd_t *setupBuffer;
#endif //USBD_APIS_H
