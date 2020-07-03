/*
 * $Copyright Broadcom Corporation Dual License $ 
 */
/***************************************************************************
 *     Copyright (c) 1999-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: local_pri_arb_msa_regs.h $
 * $brcm_Revision: cfe_bdvd_andover/1 $
 * $brcm_Date: 8/2/11 11:31a $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Tue Jan 18 17:14:06 2011
 *                 MD5 Checksum         1fb6ed1392ca34b0950e37060abe0b3b
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/bsp/Shmoo/ddr40phy/include/local_pri_arb_msa_regs.h $
 * 
 * cfe_bdvd_andover/1   8/2/11 11:31a ckder
 * SWBLURAY-26732:[ see Broadcom Issue Tracking JIRA for more info ].
 * Initial checkin to Clearcase - pre-release files for the record only.
 * 
 * dev_cfe_bdvd_andover_SWBLURAY-26732/1   7/29/11 4:01p ckder
 * Generic SHMOO release 0
 * 
 * Hydra_Software_Devel/2   1/20/11 6:36p etrudeau
 * SWBLURAY-24294: remove -no_bits and no_align from second combo_headers
 * pass and add bchp_ddr to files to take from second pass
 *
 ***************************************************************************/

#ifndef BCHP_PRI_ARB_MSA_REGS_H__
#define BCHP_PRI_ARB_MSA_REGS_H__

/***************************************************************************
 *PRI_ARB_MSA_REGS - PRIMARY_ARB memory soft access client registers
 ***************************************************************************/
#define BCHP_PRI_ARB_MSA_REGS_STATUS             (MSA_0_REG_BASE+0x00) /* Memory Controller MSA Status Register */
#define BCHP_PRI_ARB_MSA_REGS_CMD_TYPE           (MSA_0_REG_BASE+0x04) /* Memory Controller SCB Command Type Register */
#define BCHP_PRI_ARB_MSA_REGS_CMD_ADDR           (MSA_0_REG_BASE+0x08) /* Memory Controller SCB Address Register */

/***************************************************************************
 *STATUS - Memory Controller MSA Status Register
 ***************************************************************************/
/* PRI_ARB_MSA_REGS :: STATUS :: reserved0 [31:04] */
#define BCHP_PRI_ARB_MSA_REGS_STATUS_reserved0_MASK                0xfffffff0
#define BCHP_PRI_ARB_MSA_REGS_STATUS_reserved0_ALIGN               0
#define BCHP_PRI_ARB_MSA_REGS_STATUS_reserved0_BITS                28
#define BCHP_PRI_ARB_MSA_REGS_STATUS_reserved0_SHIFT               4

/* PRI_ARB_MSA_REGS :: STATUS :: FIFO_FULL [03:03] */
#define BCHP_PRI_ARB_MSA_REGS_STATUS_FIFO_FULL_MASK                0x00000008
#define BCHP_PRI_ARB_MSA_REGS_STATUS_FIFO_FULL_ALIGN               0
#define BCHP_PRI_ARB_MSA_REGS_STATUS_FIFO_FULL_BITS                1
#define BCHP_PRI_ARB_MSA_REGS_STATUS_FIFO_FULL_SHIFT               3

/* PRI_ARB_MSA_REGS :: STATUS :: FIFO_EMPTY [02:02] */
#define BCHP_PRI_ARB_MSA_REGS_STATUS_FIFO_EMPTY_MASK               0x00000004
#define BCHP_PRI_ARB_MSA_REGS_STATUS_FIFO_EMPTY_ALIGN              0
#define BCHP_PRI_ARB_MSA_REGS_STATUS_FIFO_EMPTY_BITS               1
#define BCHP_PRI_ARB_MSA_REGS_STATUS_FIFO_EMPTY_SHIFT              2

/* PRI_ARB_MSA_REGS :: STATUS :: T_LOCK [01:01] */
#define BCHP_PRI_ARB_MSA_REGS_STATUS_T_LOCK_MASK                   0x00000002
#define BCHP_PRI_ARB_MSA_REGS_STATUS_T_LOCK_ALIGN                  0
#define BCHP_PRI_ARB_MSA_REGS_STATUS_T_LOCK_BITS                   1
#define BCHP_PRI_ARB_MSA_REGS_STATUS_T_LOCK_SHIFT                  1

/* PRI_ARB_MSA_REGS :: STATUS :: BUSY [00:00] */
#define BCHP_PRI_ARB_MSA_REGS_STATUS_BUSY_MASK                     0x00000001
#define BCHP_PRI_ARB_MSA_REGS_STATUS_BUSY_ALIGN                    0
#define BCHP_PRI_ARB_MSA_REGS_STATUS_BUSY_BITS                     1
#define BCHP_PRI_ARB_MSA_REGS_STATUS_BUSY_SHIFT                    0

/***************************************************************************
 *CMD_TYPE - Memory Controller SCB Command Type Register
 ***************************************************************************/
/* PRI_ARB_MSA_REGS :: CMD_TYPE :: reserved0 [31:09] */
#define BCHP_PRI_ARB_MSA_REGS_CMD_TYPE_reserved0_MASK              0xfffffe00
#define BCHP_PRI_ARB_MSA_REGS_CMD_TYPE_reserved0_ALIGN             0
#define BCHP_PRI_ARB_MSA_REGS_CMD_TYPE_reserved0_BITS              23
#define BCHP_PRI_ARB_MSA_REGS_CMD_TYPE_reserved0_SHIFT             9

/* PRI_ARB_MSA_REGS :: CMD_TYPE :: REQ_TYPE [08:00] */
#define BCHP_PRI_ARB_MSA_REGS_CMD_TYPE_REQ_TYPE_MASK               0x000001ff
#define BCHP_PRI_ARB_MSA_REGS_CMD_TYPE_REQ_TYPE_ALIGN              0
#define BCHP_PRI_ARB_MSA_REGS_CMD_TYPE_REQ_TYPE_BITS               9
#define BCHP_PRI_ARB_MSA_REGS_CMD_TYPE_REQ_TYPE_SHIFT              0

/***************************************************************************
 *CMD_ADDR - Memory Controller SCB Address Register
 ***************************************************************************/
/* PRI_ARB_MSA_REGS :: CMD_ADDR :: reserved_for_eco0 [31:29] */
#define BCHP_PRI_ARB_MSA_REGS_CMD_ADDR_reserved_for_eco0_MASK      0xe0000000
#define BCHP_PRI_ARB_MSA_REGS_CMD_ADDR_reserved_for_eco0_ALIGN     0
#define BCHP_PRI_ARB_MSA_REGS_CMD_ADDR_reserved_for_eco0_BITS      3
#define BCHP_PRI_ARB_MSA_REGS_CMD_ADDR_reserved_for_eco0_SHIFT     29

/* PRI_ARB_MSA_REGS :: CMD_ADDR :: ADDR [28:00] */
#define BCHP_PRI_ARB_MSA_REGS_CMD_ADDR_ADDR_MASK                   0x1fffffff
#define BCHP_PRI_ARB_MSA_REGS_CMD_ADDR_ADDR_ALIGN                  0
#define BCHP_PRI_ARB_MSA_REGS_CMD_ADDR_ADDR_BITS                   29
#define BCHP_PRI_ARB_MSA_REGS_CMD_ADDR_ADDR_SHIFT                  0

/***************************************************************************
 *MSA_DATA%i - Memory Controller MSA Data Register
 ***************************************************************************/
#define BCHP_PRI_ARB_MSA_REGS_MSA_DATAi_ARRAY_BASE                 0x0040c810
#define BCHP_PRI_ARB_MSA_REGS_MSA_DATAi_ARRAY_START                0
#define BCHP_PRI_ARB_MSA_REGS_MSA_DATAi_ARRAY_END                  63
#define BCHP_PRI_ARB_MSA_REGS_MSA_DATAi_ARRAY_ELEMENT_SIZE         32

/***************************************************************************
 *MSA_DATA%i - Memory Controller MSA Data Register
 ***************************************************************************/
/* PRI_ARB_MSA_REGS :: MSA_DATAi :: Data [31:00] */
#define BCHP_PRI_ARB_MSA_REGS_MSA_DATAi_Data_MASK                  0xffffffff
#define BCHP_PRI_ARB_MSA_REGS_MSA_DATAi_Data_ALIGN                 0
#define BCHP_PRI_ARB_MSA_REGS_MSA_DATAi_Data_BITS                  32
#define BCHP_PRI_ARB_MSA_REGS_MSA_DATAi_Data_SHIFT                 0


/***************************************************************************
 *MSA_MASK%i - Memory Controller MSA Mask Data Register
 ***************************************************************************/
#define BCHP_PRI_ARB_MSA_REGS_MSA_MASKi_ARRAY_BASE                 0x0040c910
#define BCHP_PRI_ARB_MSA_REGS_MSA_MASKi_ARRAY_START                0
#define BCHP_PRI_ARB_MSA_REGS_MSA_MASKi_ARRAY_END                  63
#define BCHP_PRI_ARB_MSA_REGS_MSA_MASKi_ARRAY_ELEMENT_SIZE         32

/***************************************************************************
 *MSA_MASK%i - Memory Controller MSA Mask Data Register
 ***************************************************************************/
/* PRI_ARB_MSA_REGS :: MSA_MASKi :: reserved0 [31:04] */
#define BCHP_PRI_ARB_MSA_REGS_MSA_MASKi_reserved0_MASK             0xfffffff0
#define BCHP_PRI_ARB_MSA_REGS_MSA_MASKi_reserved0_ALIGN            0
#define BCHP_PRI_ARB_MSA_REGS_MSA_MASKi_reserved0_BITS             28
#define BCHP_PRI_ARB_MSA_REGS_MSA_MASKi_reserved0_SHIFT            4

/* PRI_ARB_MSA_REGS :: MSA_MASKi :: Mask [03:00] */
#define BCHP_PRI_ARB_MSA_REGS_MSA_MASKi_Mask_MASK                  0x0000000f
#define BCHP_PRI_ARB_MSA_REGS_MSA_MASKi_Mask_ALIGN                 0
#define BCHP_PRI_ARB_MSA_REGS_MSA_MASKi_Mask_BITS                  4
#define BCHP_PRI_ARB_MSA_REGS_MSA_MASKi_Mask_SHIFT                 0


#endif /* #ifndef BCHP_PRI_ARB_MSA_REGS_H__ */

/* End of File */
