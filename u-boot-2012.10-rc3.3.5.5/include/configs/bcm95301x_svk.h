#ifndef __BCM95301X_SVK_H
#define __BCM95301X_SVK_H

#include "iproc_board.h" 

#define CONFIG_BOARD_EARLY_INIT_F (1)
#define CONFIG_BOARD_LATE_INIT
#define CONFIG_PHYS_SDRAM_1_SIZE			0x01000000 /* 16 MB */
#define IPROC_ETH_MALLOC_BASE 0xD00000

#undef CONFIG_ENV_IS_NOWHERE

/* Enable generic u-boot SPI flash drivers and commands */
#define CONFIG_CMD_SF
#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_STMICRO_NS
#define CONFIG_SPI_FLASH_MACRONIX_NS
#define CONFIG_SPI_FLASH_SPANSION
#define CONFIG_SPI_FLASH_SST
#define CONFIG_SPI_FLASH_WINBOND
#define CONFIG_SPI_FLASH_ATMEL

/* SPI flash configurations for Northstar */
#define CONFIG_IPROC_QSPI
#define CONFIG_IPROC_QSPI_BUS                   0
#define CONFIG_IPROC_QSPI_CS                    0

/* SPI flash configuration - flash specific */
#define CONFIG_IPROC_BSPI_DATA_LANES            1
#define CONFIG_IPROC_BSPI_ADDR_LANES            1
#define CONFIG_IPROC_BSPI_READ_CMD              0x0b
#define CONFIG_IPROC_BSPI_READ_DUMMY_CYCLES     8
#define CONFIG_SF_DEFAULT_SPEED                 50000000
#define CONFIG_SF_DEFAULT_MODE                  SPI_MODE_3

/* Environment variables */
#ifdef CONFIG_NAND_IPROC_BOOT
#define CONFIG_ENV_IS_IN_NAND                   1
#define CONFIG_ENV_OFFSET                       0x200000
#define CONFIG_ENV_RANGE                        0x200000
#else
#ifdef CONFIG_SPI_FLASH
#define CONFIG_ENV_IS_IN_SPI_FLASH              1
#define CONFIG_ENV_OFFSET                       0xa0000
#define CONFIG_ENV_SPI_MAX_HZ                   10000000
#define CONFIG_ENV_SPI_MODE                     SPI_MODE_3
#define CONFIG_ENV_SPI_BUS                      CONFIG_IPROC_QSPI_BUS
#define CONFIG_ENV_SPI_CS                       CONFIG_IPROC_QSPI_CS
#define CONFIG_ENV_SECT_SIZE                    0x10000     /* 64KB */
#endif /* CONFIG_SPI_FLASH */
#endif /* CONFIG_NAND_BOOT */

/* Environment variables for NAND flash */
#define CONFIG_CMD_NAND 
#define CONFIG_IPROC_NAND 
#define CONFIG_SYS_MAX_NAND_DEVICE			1
#define CONFIG_SYS_NAND_BASE		        0xdeadbeef
#define CONFIG_SYS_NAND_ONFI_DETECTION

#include "iproc_common_configs.h"

#undef CONFIG_STANDALONE_LOAD_ADDR
#define CONFIG_STANDALONE_LOAD_ADDR 0x81000000


#define CONFIG_BAUDRATE 115200
/* #define CONFIG_BOOTARGS     	"console=ttyS0,115200n8 maxcpus=2 mem=512M" */

/* #define CONFIG_BOOTDELAY		5*/ 	/* User can hit a key to abort kernel boot and stay in uboot cmdline */
/* #define CONFIG_BOOTCOMMAND 		"dhcp; run nfsargs; bootm;"	*/ /* UBoot command issued on power up */

/* #define CONFIG_DEFAULT_L2_CACHE_LATENCY (1) */ /* To use default value */ /* this has not been proven to be working for all boards */

#define CONFIG_EXTRA_ENV_SETTINGS \
	"brcmtag=0\0"               \
	"console=ttyS0\0"           \
    "loglevel=7\0"              \
    "tftpblocksize=512\0"       \
    "hostname=northstar\0"      \
    "vlan1ports=0 1 2 3 8u\0"   \
    "nfsroot=/dev/nfs rw nfsroot=/nfs/rootfs\0"        \
	"nfsargs=setenv bootargs "  \
        "loglevel=${loglevel} " \
        "console=${console},${baudrate}n8 "   \
        "ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}:northstar:: " \
        "maxcpus=2 "            \
		"mem=512M "             \
		"root=${nfsroot} "      \
		"\0"                    \
    "reflash="                  \
        "dhcp ${loadaddr} u-boot.bin; " \
        "sf probe 0; "          \
        "sf erase 0 +${filesize}; " \
        "sf write ${fileaddr} 0 ${filesize};" \
        "\0"                    \
    "clr_env="                  \
        "sf probe 0;"           \
        "sf erase 0xa0000 +1;"  \
        "\0"        
/*
#define CONFIG_BOOTCOMMAND \
	"if mmc rescan ${mmcdev}; then " \
		"if run loadbootscript; then " \
			"run bootscript; " \
		"else " \
			"if run loaduimage; then " \
				"run mmcboot; " \
			"fi; " \
		"fi; " \
	"fi"
*/

#ifdef CONFIG_IPROC_SPL
/*define CONFIG_IPROC_NAND_SPL_MAX_SIZE 4096 */
#define CONFIG_IPROC_NAND_SPL_MAX_SIZE 0x9000

#define CONFIG_SPL
#define CONFIG_SPL_FRAMEWORK
#define CONFIG_SPL_TEXT_BASE            0xC0000000
#define CONFIG_SPL_MAX_SIZE             0x37000  /*1 Sector Size - NAND_SPL Max Size = (128KB - 32KB) */
#define CONFIG_SPL_IMAGE_MAX_SIZE        0x17000  /*1 Sector Size - NAND_SPL Max Size = (128KB - 32KB) */
#define CONFIG_SPL_STACK                (0xC0020000 + CONFIG_SYS_STACK_SIZE - 16)

/*
 * SPL resides in sram
 */
#define CONFIG_SPL_BSS_START_ADDR       0xC0030000
#define CONFIG_SPL_BSS_MAX_SIZE         0x10000       /* 64 KB */
#define CONFIG_SYS_SPL_MALLOC_START     0x84100000
#define CONFIG_SYS_SPL_MALLOC_SIZE      0x100000


#define CONFIG_SPL_LIBCOMMON_SUPPORT

#ifdef CONFIG_NAND_IPROC_BOOT
#define CONFIG_IPROC_NAND_SPL_SYS_TEXT_BASE 0x1C000000
#define CONFIG_SPL_NAND_SUPPORT
#define CONFIG_SPL_NAND_SIMPLE
#define CONFIG_SYS_NAND_U_BOOT_OFFS     0x40000
#define CONFIG_SYS_NAND_PAGE_SIZE       4096
#define CONFIG_SPL_NAND_SIMPLE_BBT
#define CONFIG_SPL_UBOOT_START          (0x1C000000 + CONFIG_IPROC_NAND_SPL_MAX_SIZE) /* First 4K has NAND_SPL */
#else
#define CONFIG_IPROC_NAND_SPL_SYS_TEXT_BASE 0x1E000000
#define CONFIG_SPL_SPI_SUPPORT
#define CONFIG_SPL_SPI_FLASH_SUPPORT
#define CONFIG_SPL_SPI_LOAD
#define CONFIG_SPL_SPI_BUS 0
#define CONFIG_SPL_SPI_CS 0
#define CONFIG_SYS_SPI_U_BOOT_OFFS       0x20000
#define CONFIG_SYS_SPI_U_BOOT_SIZE       0xD0000
#define CONFIG_SPL_UBOOT_START           (0x1E000000 + CONFIG_IPROC_NAND_SPL_MAX_SIZE)
#endif /* CONFIG_NAND_IPROC_BOOT */
#define CONFIG_SPL_UBOOT_END            (CONFIG_SPL_UBOOT_START + CONFIG_SPL_IMAGE_MAX_SIZE)

#define CONFIG_SPL_LIBGENERIC_SUPPORT
#define CONFIG_SPL_SERIAL_SUPPORT
#define CONFIG_SPL_LDSCRIPT "$(CPUDIR)/iproc/u-boot-spl.lds"
#endif /* CONFIG_IPROC_SPL */



#endif /* __BCM95301X_SVK_H */



	
