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
    * 
    * $Id::                                                       $:
    * $Rev:: : Global SVN Revision = 1950                         $:
    * 
 */
#include "common.h"
#include <mde_common_headers.h>

unsigned host_local_init (void)
{
  printf ("Northstar local init\n");
  return 0; // no error
}


unsigned host_get_elapsed_msec(void)
{
	get_timer(0);
}
/*
  host_block_read_32bit_data - read a block of 32bit data

  inputs:
    from_addr:  The address of the memory block to read
	size_ints:  The number of 32-bit data to read
	pread_data: A pointer to store the read data

  return:  The number of 32-bit data read

 */
unsigned host_block_read_32bit_data (unsigned from_addr, unsigned size_ints, unsigned *pread_data)
{
  unsigned *pfrom = (unsigned*)from_addr;
  int inx;
  
  for (inx = 0; inx < size_ints; inx ++ ) {
	*pread_data++ = *pfrom++;
  }
  return inx; 
}

/*
  host_block_write_32bit_data - write a block of 32bit data

  inputs:
    to_addr:    The address of the memory block to write to
	size_ints:  The number of 32-bit data to write
	pread_data: A pointer to the data to write

  return:  The number of 32-bit data written

 */
unsigned host_block_write_32bit_data (unsigned to_addr, unsigned size_ints, unsigned *pwrite_data)
{
  unsigned *pto = (unsigned*)to_addr;
  int inx;
  
  for (inx = 0; inx < size_ints; inx ++ ) {
	*pto++ = *pwrite_data++;
  }
  return inx; 
}

#define CHIP_NAME "NORTHSTAR ARMv7"
#define CHIP_ID   5301

sMde_t *setupNewMdeWithLocalSpecificInfo ( sMde_t *newMde )
{  
  int inx;

  newMde -> chip_id = CHIP_ID;
  strcpy ( &newMde->chip_name [0], CHIP_NAME );

  newMde -> bread.size_ints = MEMBUF_SIZE_INTS;

  newMde -> bwrit.size_ints = MEMBUF_SIZE_INTS;

  newMde -> local.cmds = host_setup_local_cmds (&newMde -> local.cmd_count);

  // setup methods for this chip

  // setup the memory blocks
  for (inx = 0; inx < LOCAL_MEM_BLOCK_COUNT; inx ++ ) {
	newMde -> mem_blks[inx].id    = inx;
	newMde -> mem_blks[inx].bsize = HOST_MEM_SIZE_BYTES;
  }
  newMde -> mem_blks_count  = inx; // 
  newMde -> mem_total_bytes = HOST_MEM_SIZE_BYTES * inx;

  newMde -> mem_blks[0].start = 0;
  newMde -> mem_blks[1].start = 0;

  // set up memory tester
  
  newMde -> memtester_count = LOCAL_MEM_TESTER_COUNT;
  newMde -> memtester [0].id      = 0;
  newMde -> memtester [0].enabled = 1;
  newMde -> memtester [0].start_addr_cached   = 0x02000000;
  newMde -> memtester [0].start_addr_uncached = 0x02000000;
  newMde -> memtester [0].test_bsize = 0x06000000;
  newMde -> memtester [0].test_wsize = 0x01800000;
  newMde -> memtester [0].stressor_present = 0;

  strcpy ( (char*)&newMde->memtester [0].name, "Northstar DDR PHY0");

  newMde -> phy_data_width = PHY_WIDTH; //
  return newMde;
}

void host_status_report (void)
{
}

//#define host_reg_read( offset)        *(unsigned*)(offset + NORTHSTAR_REG_BASE)

unsigned host_reg_read ( unsigned offset) 
{
  offset = offset & ~(0x00006000);
  //  host_printf ("host_reg_read address (%08x:%08x)->%08x\n", NORTHSTAR_REG_BASE, offset, (NORTHSTAR_REG_BASE+offset));
  return *(unsigned*)(offset + NORTHSTAR_REG_BASE);
}


//#define host_reg_write( offset, data) *(unsigned*)(offset + NORTHSTAR_REG_BASE)=data

void host_reg_write( unsigned offset, unsigned data)
{
  offset = offset & ~(0x00006000);

  //  host_printf ("host_reg_write address (%08x:%08x)->%08x with data %08x\n", NORTHSTAR_REG_BASE, offset, (NORTHSTAR_REG_BASE+offset), data );
  *(unsigned*)(offset + NORTHSTAR_REG_BASE) = data;
}

/*
  flush_data_cache is required by mde_rdi.c
  for NS - use a place holder until this is defined

 */
void flush_data_cache (unsigned addr, unsigned bytes, unsigned verbose)
{
  //  if (verboase) host_printf ("FDC: addr %08x bytes %x\n", addr, bytes);
  SHMOO_FLUSH_DATA_TO_DRAM (addr, bytes);
  SHMOO_INVALIDATE_DATA_FROM_DRAM( addr,  bytes );
}

#if 0

// shmoo_platform_s2.c
//

#include <common.h>
#include <asm/iproc/platform.h>
//#include <stdio.h>
#include <stdarg.h>

#define DEBUG_PRINTF //

/*added*/
#define CACHE_LINE_SIZE 32
void flush_dcache_range( unsigned long start , unsigned long stop )
{
    unsigned long i;

    for( i = start ; i < stop ; i += CACHE_LINE_SIZE) {
        asm volatile ("MCR  p15, 0, %0, c7, c10, 1"::"r"(i));
    }

    asm volatile ("MCR  p15, 0, r0, c7, c10, 4":::); /*DSB*/

    return;
}

void invalidate_dcache_range( unsigned long start , unsigned long stop )
{
    unsigned long i;

    for ( i = start ; i < stop ; i += CACHE_LINE_SIZE ) {
        asm volatile ("MCR  p15, 0, %0, c7, c6, 1"::"r"(i));
    }

    asm volatile ("MCR  p15, 0, r0, c7, c10, 4":::); /*DSB*/

    return;
} 

void  SHMOO_FLUSH_DATA_TO_DRAM( ddr40_addr_t Address , unsigned int bytes ) 
{
    unsigned long const EndAddress = Address + bytes;

    // printf("Flushing %08X+%d bytes\n", Address, bytes);
    flush_dcache_range( Address , EndAddress );

    return;
}

void SHMOO_INVALIDATE_DATA_FROM_DRAM( ddr40_addr_t Address , unsigned int bytes )
{
    unsigned long const EndAddress = Address + bytes;
  
    // printf("Invalidating %08X+%d bytes\n", Address, bytes);
    invalidate_dcache_range( Address , EndAddress );

    return;
}
#endif

/*
  For NORTHSTAR - provide an entry in the U-BOOT command section
  The user can then invoke MDE from the U-BOOT prompt
  Once in MDE - the user can type 'q' to return to U-BOOT
 */

U_BOOT_CMD(
	mde, 1,	0,	mde_entry,
	"DDR memory Diagnostics Environment shell",
	" - Enter Memory Diagnostics shell. Type 'q' to return to the u-boot shell"
);

