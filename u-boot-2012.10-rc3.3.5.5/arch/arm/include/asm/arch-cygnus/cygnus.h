/*
 * cygnus.h
 *
 *  Created on: 04-Jul-2013
 *      Author: chandra
 */

#ifndef CYGNUS_H_
#define CYGNUS_H_
/*-----------------------------------------------------------------------------------------*/

#include <asm/arch/powerManagement.h>
/*-----------------------------------------------------------------------------------------*/

#define mTriggerDump() mWrite32(IPROC_CRMU_MAIL_BOX0, 0x12345678);

/*-----------------------------------------------------------------------------------------*/

#define IPROC_MAILBOX_GIC_ID (IPROC_INTERRUPTS__cmicd_intr_BIT0 + CHIP_INTR1__CRMU_MAILBOX)
/*-----------------------------------------------------------------------------------------*/
#endif /* CYGNUS_H_ */
