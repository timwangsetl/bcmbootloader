/*
 * irq_handlers.S
 *
 *  Created on: 08-Jul-2013
 *      Author: Chandra Mohan C
 *       email: chandra@broadcom.com
 */
/*-----------------------------------------------------------------------------------------*/

.global SVC_Handler
SVC_Handler:
    PUSH       {R0-R12,lr}             @ Store registers.
    LDR        R0,[lr,#-4]             @ Calculate address of SVC instruction
                                       @ and load it into R0.
    BIC        R0,R0,#0xFF000000       @ Mask off top 8 bits of instruction
                                       @ to give SVC number.

    @ Use value in R0 to determine which SVC routine to execute.
    BL			C_SVC_handler

    LDM         sp!, {R0-R12,pc}^      @ Restore registers and return.
/*-----------------------------------------------------------------------------------------*/

.global NS_Data_abort_Handler
NS_Data_abort_Handler:

ldr	       sp, NS_DATA_ABORT_STACK_START
PUSH {R0-R12,lr}
BL C_NS_DataAbort_Handler
POP {R0-R12,lr}
subs pc,lr,#4

.global SMC_Handler
SMC_Handler:
ldr	       sp, SMC_STACK_START
PUSH       {R0-R12,lr}

LDR        R0,[lr,#-4]
LDR        r1, SMC_ID_MASK
BIC        R0,R0,r1
CMP        R0, #0x1
BEQ        SEC_WORLD

mrc p15, 0, r1, c1, c1, 0 @ read SCR
bic r1, r1, #0x07f
orr r1, r1, #0x31 @ enable NS, AW, FW
mcr p15, 0, r1, c1, c1, 0 @ write SCR, switch to non-sec
isb
LDM         sp!, {R0-R12,pc}^

SEC_WORLD:
mrc p15, 0, r1, c1, c1, 0 @ read SCR
bic r1, r1, #0x07f
orr r1, r1, #0x31 @ enable NS, AW, FW
and	r1, r1, #0xFE
mcr p15, 0, r1, c1, c1, 0 @ write SCR, switch to non-sec
isb
LDM         sp!, {R0-R12,pc}^


 SMC_ID_MASK:
 	.word 0xFFFFFFF0

 SMC_STACK_START:
 	.word 0x70000000

 NS_DATA_ABORT_STACK_START:
 	.word 0x70005000

.global nonsec_switch
nonsec_switch:
PUSH       {R0-R12,lr}
mrc p15, 0, ip, c12, c0, 0
    isb
	smc #0
	isb
	add 	ip, ip, #0x40
mcr	p15, 0, ip, c12, c0, 0
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x00002000	@ clear bits 13 (--V-)
	mcr	p15, 0, r0, c1, c0, 0
LDM         sp!, {R0-R12,pc}^

.global sec_switch
sec_switch:
PUSH       {R0-R12,lr}
	smc #1
	isb
LDM         sp!, {R0-R12,pc}^
