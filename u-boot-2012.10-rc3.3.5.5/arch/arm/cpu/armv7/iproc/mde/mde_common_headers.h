/*
 * $Copyright Broadcom Corporation Dual License $ 
 */
/*  *********************************************************************
    *********************************************************************
    *  Broadcom Memoroy Diagnostics Environment (MDE)
    *********************************************************************
    *  Filename: mde_common_headers.h
    *
    *  Function: declare all MDE headers
	*            This is included in all MDE C files
    *
    *  Author:  Chung Kwong (C.K.) Der (ckder@broadcom.com)
    *
 */
#ifndef MDE_COMMON_HEADERS_H
#define MDE_COMMON_HEADERS_H

//// Include local header files

#include "mde_local_headers.h"

#include "tinymt32.h"

#include "mde_types.h"
#include "mde.h"
#include "mde_def.h"
#include "mde_mt.h"
#include "mde_obj.h"

#include "mde_proto.h"
#include "mde_plat.h"

// access for PHY registers
// These two header files provide 0 based offset

#include "mde_phy_word_lane.h"
#include "mde_phy_ctrl_regs.h"
#include "mde_shim_addr_cntl.h"

#include "mde_rd_eye.h"

#endif // MDE_COMMON_HEADERS_H
