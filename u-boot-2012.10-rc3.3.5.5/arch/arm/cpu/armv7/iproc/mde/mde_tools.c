/*
 * $Copyright Broadcom Corporation Dual License $ 
 */
/*  *********************************************************************
    *********************************************************************
    *  Broadcom Common Diagnostics Environment (CDE)
    *********************************************************************
    *  Filename: cde_main.c
    *
    *  Function:
    *
    *  Author:  Chung Kwong (C.K.) Der (ckder@broadcom.com)
    *
 */

#include <mde_common_headers.h>

unsigned mdeGetTime ( sMde_t *mde )
{
  unsigned msec;

  msec = host_get_elapsed_msec ();
  host_printf ("Since start: %d msec (or %d sec)\n", msec, (msec/1000));
  return DIAG_RET_OK;
}
