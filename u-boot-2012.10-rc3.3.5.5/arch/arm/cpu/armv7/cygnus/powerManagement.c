/*
 * powerManagement.c
 *
 *  Created on: 04-Jul-2013
 *      Author: Chandra Mohan C
 *       email: chandra@broadcom.com
 */

/*-----------------------------------------------------------------------------------------*/

#include <config.h>
#include <common.h>
#include <asm/arch/iproc.h>
#include <asm/arch/cygnus.h>
#include "asm/arch/powerManagement.h"
volatile uint32_t ns_data_abort_count = 0x00;

/*-----------------------------------------------------------------------------------------*/

static void pm_isr(void *data);
/*-----------------------------------------------------------------------------------------*/

typedef volatile unsigned char vu8;
typedef volatile unsigned short vu16;
typedef volatile unsigned long vu32;

#define mObject(reg) (*((vu32*)reg))

#define mRead32(reg) mObject(reg)
#define mWrite32(reg, data) (mObject(reg)= data)

#define mSetBit(reg, bitPosition) (mObject(reg)|= (1<< bitPosition))
#define mClearBit(reg, bitPosition) (mObject(reg)&= ~(1<< bitPosition))

#define mSetValue(reg, value, pattern, offset) (mWrite32(reg, (mObject(reg)& ~(pattern<< offset)) | (value<< offset))))))
/*-----------------------------------------------------------------------------------------*/

inline static void writeMail(u32 mail1, u32 mail2)
{
	mWrite32(IPROC_CRMU_MAIL_BOX0, mail1);
	mWrite32(IPROC_CRMU_MAIL_BOX1, mail2);
}

inline static void getMail(u32 mail1, u32 mail2)
{
	mWrite32(CRMU_IPROC_MAIL_BOX0, 0x12345678);
	mWrite32(CRMU_IPROC_MAIL_BOX1, 0x12345678);
}
/*-----------------------------------------------------------------------------------------*/

int pmInit()
{
	int status= -1;

#ifdef CONFIG_USE_IRQ
	gic_disable_interrupt(IPROC_MAILBOX_GIC_ID);
	irq_install_handler(IPROC_MAILBOX_GIC_ID, pm_isr, NULL);
	gic_config_interrupt(IPROC_MAILBOX_GIC_ID, 1, IPROC_INTR_LEVEL_SENSITIVE, 0, IPROC_GIC_DIST_IPTR_CPU0);
	gic_enable_interrupt(IPROC_MAILBOX_GIC_ID);
#else
	printf("WARN-Cygnus: CONFIG_USE_IRQ disabled, so powerManagement won't work\n");
#endif

	return status;
}
/*-----------------------------------------------------------------------------------------*/

static void pm_isr(void *data)
{
	serial_printf("ticks=%llu \n", get_ticks());
	serial_printf("mailbox0= 0x%lx mailbox1= 0x%lx \n", mRead32(CRMU_IPROC_MAIL_BOX0), mRead32(CRMU_IPROC_MAIL_BOX1));

	// acknowledge mailbox irq
	mSetBit(CRMU_IPROC_INTR_CLEAR, CRMU_IPROC_INTR_STATUS__IPROC_MAILBOX_INTR);
}
/*-----------------------------------------------------------------------------------------*/
extern unsigned int get_pc(void);
void C_SVC_handler (unsigned number)
{
    switch (number)
    {
        case 0:
        {
        	serial_printf("INFO-Cygnus:%d:%s() isr @ 0x%x handled\n", __LINE__, __func__, get_pc());
        }break;

        case 1:
        {

        }break;

        default:
        {

        }
    }
}

void C_NS_DataAbort_Handler (unsigned number)
{
	//serial_printf("Non secure Abort Handler\n");
	ns_data_abort_count += 1;
}
/*-----------------------------------------------------------------------------------------* /

void privateTimerInit()
{
	iproc_clk_enum();

	gic_disable_interrupt(29);
	gic_enable_distributor();
	irq_install_handler(29, systick_isr, NULL);
	gic_config_interrupt(29, 1, IPROC_INTR_EDGE_TRIGGERED, 0, IPROC_GIC_DIST_IPTR_CPU0);
	iproc_systick_init(2000);
	gic_enable_interrupt(29);
}
/ *-----------------------------------------------------------------------------------------*/
