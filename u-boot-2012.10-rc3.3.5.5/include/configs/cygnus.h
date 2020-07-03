/*
 * cygnus.h
 *
 *  Created on: 05-Jul-2013
 *      Author: chandra
 */

#ifndef CYGNUS_CONFIG_H_
#define CYGNUS_CONFIG_H_

#include <asm/sizes.h>
#include <config.h>
#include "iproc_common_configs.h"
/* Architecture, CPU, etc */
#define CONFIG_ARMV7
#define CONFIG_IPROC (1)
//#define CONFIG_IPROC_P7 1
#define CONFIG_CYGNUS (1)
#define CONFIG_BOARD_EARLY_INIT_F (1)
#define CONFIG_BOARD_LATE_INIT    (1)
#define CONFIG_HAS_HALAPI

#if defined(CONFIG_CYGNUS)
    #define CONFIG_TZ
#endif

/*
Fix me

When DEBUG is enabled, need to disable both CACHE to make u-boot running
#define CONFIG_SYS_NO_ICACHE
#define CONFIG_SYS_NO_DCACHE
#define DEBUG
*/

#if 0
//<chandra:enabled debug info
//When DEBUG is enabled, need to disable both CACHE to make u-boot running
#define CONFIG_SYS_NO_ICACHE
#define CONFIG_SYS_NO_DCACHE
#define DEBUG
//>chandra:enabled debug info
#endif

#define CONFIG_IPROC_MMU                      (1)
#define CONFIG_L2_OFF               /* Disable L2 cache */
#define CONFIG_SYS_ARM_CACHE_WRITETHROUGH     (1)

#define CONFIG_MISC_INIT_R          /* Call board's misc_init_r function */

#if !defined(CONFIG_NO_CODE_RELOC)
    /* Interrupt configuration */
    #define CONFIG_USE_IRQ                     1   /* we need IRQ stuff for timer  */
#endif

/* Memory Info */
#ifdef CONFIG_HAVE_DDR
    #define CONFIG_SYS_MALLOC_LEN              SZ_256K//SZ_1M      /* see armv7/start.S. */
#else
    #define CONFIG_SYS_MALLOC_LEN              SZ_32K      /* see armv7/start.S. */
#endif

#define CONFIG_STACKSIZE_IRQ                   (4096)
#define CONFIG_STACKSIZE_FIQ                   (4096)

#define CONFIG_PHYS_SDRAM_0                     0x2000000
#define CONFIG_L2_CACHE_SIZE                    0x40000
#define CONFIG_PHYS_SDRAM_1                 0x60000000 //DDR
#define CONFIG_PHYS_SDRAM_1_SIZE            0x40000000//1GB

#define CONFIG_LOADADDR                         0x70000000 /* default destination location for tftp file (tftpboot cmd) */

#define CONFIG_PHYS_SDRAM_RSVD_SIZE         0


#define CONFIG_SYS_REF_CLK                      (25000000) /*Reference clock = 25MHz */
#define CONFIG_SYS_REF2_CLK                     (200000000) /*Reference clock = 200MHz */
#define IPROC_ARM_CLK                           (1000000000) /* ARM CPU clock - default to 1GHz */
#define IPROC_AXI_CLK                           (400000000)  /* 400 MHz - clk21 */
#define IPROC_APB_CLK                           (100000000)  /* 100 MHz - clk41 */


#undef  CONFIG_SYS_NS16550
#undef  CONFIG_SYS_NS16550_SERIAL
//#define CONFIG_SYS_NS16550_COM1                 (0x18000300)
//#define CONFIG_SYS_NS16550_REG_SIZE             (1)     /* no padding */
//#define CONFIG_SYS_NS16550_CLK                  (24008) /* Set it to right clock, i.e APB_clk/2 */
//#define CONFIG_SYS_NS16550_CLK_DIV              (10) /* Set it to right divider, i.e APB_clk/2/16/baudrate */
//#define CONFIG_BAUDRATE                         300

/* Where kernel is loaded to in memory */
#define CONFIG_SYS_LOAD_ADDR                    0x61008000
#define LINUX_BOOT_PARAM_ADDR                   0x60200000 /* default mapped location to store the atags pointer */

#define CONFIG_SYS_MEMTEST_START                CONFIG_PHYS_SDRAM_1
#define CONFIG_SYS_MEMTEST_END                  (CONFIG_PHYS_SDRAM_1+CONFIG_PHYS_SDRAM_1_SIZE)
#define CONFIG_NR_DRAM_BANKS                    1

// HAIFENG CHANGE
#define CONFIG_SYS_SDRAM_BASE                   (CONFIG_PHYS_SDRAM_1 + CONFIG_PHYS_SDRAM_RSVD_SIZE)

/* CONFIG_SYS_TEXT_BASE is where u-boot is loaded by boot1 */
#define CONFIG_SYS_STACK_SIZE                   (SZ_64K) /* 64K */

//We always put SP before DDR to SDRAM
#define CONFIG_SYS_INIT_SP_ADDR                 (CONFIG_PHYS_SDRAM_0 + CONFIG_SYS_STACK_SIZE - 16)

/* Serial Info */
#define CONFIG_SERIAL_MULTI
#define CONFIG_SYS_UART_CLK                     (IPROC_APB_CLK)
#define CONFIG_SYS_SYNOPSYS
#define CONFIG_SYS_SYNOPSYS_CLK_DIV             54  /* IPROC_APB_CLK/(16*115200) */

#ifdef CONFIG_CONS_INDEX
#undef CONFIG_CONS_INDEX
#endif
#define CONFIG_CONS_INDEX                       3
#define CONFIG_SYS_SYNOPSYS_COM0	            ChipcommonG_UART0_UART_RBR_THR_DLL
#define CONFIG_SYS_SYNOPSYS_COM1	            ChipcommonG_UART1_UART_RBR_THR_DLL
#define CONFIG_SYS_SYNOPSYS_COM2	            ChipcommonG_UART2_UART_RBR_THR_DLL
#define CONFIG_SYS_SYNOPSYS_COM3	            ChipcommonG_UART3_UART_RBR_THR_DLL
#define CONFIG_SYS_SYNOPSYS_COM4	            ChipcommonG_UART4_UART_RBR_THR_DLL

/* we support 4 ddr clock speed for cygnus
 *   400
 *   533
 *   667
 *   800
 */
#define CONFIG_DRAM_FREQ                       800 
/* SHMOO and28 needs about 64K storage for calibration process 
 * The buffter will be released after trace calibration is done
 * and final config saved into flash 
 * For Cygnus, we use the second half of SRAM as this buffer
 */
#define CONFIG_SHMOO_SRAM_BUF                   0x2020000
#
#define CONFIG_ENV_OVERWRITE    /* Allow serial# and ethernet mac address to be overwritten in nv storage */

#ifdef CONFIG_HAVE_DDR
    #define CONFIG_ENV_SIZE                     SZ_64K /* 64K */
#else
    #define CONFIG_ENV_SIZE                     SZ_4K /*4K */
#endif

/* NO flash */
#define CONFIG_SYS_NO_FLASH     /* Not using NAND/NOR unmanaged flash */

//#define BOARD_LATE_INIT

/* Ethernet configuration */
#if 1
#ifndef CONFIG_NO_CODE_RELOC
    #define CONFIG_BCMIPROC_ETH
    //#define CONFIG_CMD_MII
    #define IPROC_ETH_MALLOC_BASE               CONFIG_PHYS_SDRAM_1
#endif
#endif

//#define CONFIG_BCMHANA_ETH
//#define CONFIG_NET_MULTI

/* DMA configuration */
//#define CONFIG_BCM5301X_DMA 

/* General U-Boot configuration */
#define CONFIG_SYS_CBSIZE                       1024    /* Console buffer size */
#define CONFIG_SYS_PBSIZE                       (CONFIG_SYS_CBSIZE +    sizeof(CONFIG_SYS_PROMPT) + 16) /* Printbuffer size */
#define CONFIG_SYS_MAXARGS                      64
#define CONFIG_SYS_BARGSIZE                     CONFIG_SYS_CBSIZE

//#define CONFIG_VERSION_VARIABLE   /* Enabled UBOOT build date/time id string */
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2               "> "
#define CONFIG_CMDLINE_EDITING
#define CONFIG_SYS_LONGHELP

#define CONFIG_CRC32_VERIFY         /* Add crc32 to memory verify commands */
#define CONFIG_MX_CYCLIC            /* Memory display cyclic */

#define CONFIG_CMDLINE_TAG              /* ATAG_CMDLINE setup */
#define CONFIG_SETUP_MEMORY_TAGS        /* ATAG_MEM setup */

#define CONFIG_SYS_PROMPT                   "u-boot> "

//#include <config_cmd_default.h>
#define CONFIG_CMD_CONSOLE
//#define CONFIG_ENV_IS_NOWHERE
#undef  CONFIG_ENV_IS_NOWHERE
#define CONFIG_CMD_NFS
#undef  CONFIG_GENERIC_MMC
#define CONFIG_CMD_NET
#define CONFIG_CMD_PING
//#define CONFIG_CYGNUS_LOOPBACK_MODE
//#define CONFIG_CMD_LINK_LOCAL//chandra: added
#define CONFIG_CMD_MEMORY
//#define CONFIG_CMD_MISC


#define CONFIG_GMAC_NUM     0


/* Enable generic u-boot SPI flash drivers and commands */
#define CONFIG_CMD_SF
#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_STMICRO_NS
#define CONFIG_SPI_FLASH_MACRONIX_NS
#define CONFIG_SPI_FLASH_SPANSION
#define CONFIG_SPI_FLASH_SST
#define CONFIG_SPI_FLASH_WINBOND
#define CONFIG_SPI_FLASH_ATMEL

/* QSPI flash configurations*/
#define CONFIG_IPROC_QSPI
#define CONFIG_IPROC_QSPI_BUS                   0
#define CONFIG_IPROC_QSPI_CS                    0
#define IPROC_QSPI_MEM_BASE                     0xF0000000

#define CONFIG_CMD_SPI                          1
/* SPI flash configuration, 2 flashes connected to SPI2 and SPI3 */
#define CONFIG_PL022_SPI
#define CONFIG_PL022_SPI_BUS                   1
#define CONFIG_PL022_SPI_CS                    0
#define CONFIG_SYS_SPI_CLK                     100000000 /* 100MHz */

/* SPI flash configuration - flash specific */
#define CONFIG_IPROC_BSPI_DATA_LANES            1
#define CONFIG_IPROC_BSPI_ADDR_LANES            1
#define CONFIG_IPROC_BSPI_READ_CMD              0x0b
#define CONFIG_IPROC_BSPI_READ_DUMMY_CYCLES     8
#define CONFIG_SF_DEFAULT_SPEED                 500000
#define CONFIG_SF_DEFAULT_MODE                  SPI_MODE_3



#ifdef CONFIG_NAND_IPROC_BOOT
#define CONFIG_ENV_IS_IN_NAND                   1
#else
/* Environment variables in SPI flash */
#ifdef  CONFIG_SPI_FLASH
#define CONFIG_ENV_IS_IN_SPI_FLASH              1
#define CONFIG_ENV_SPI_MAX_HZ                   10000000
#define CONFIG_ENV_SPI_MODE                     SPI_MODE_3
#define CONFIG_ENV_SPI_BUS                      CONFIG_IPROC_QSPI_BUS
#define CONFIG_ENV_SPI_CS                       CONFIG_IPROC_QSPI_CS
#define CONFIG_ENV_SECT_SIZE                    0x10000     /* 64KB */
#endif /* CONFIG_SPI_FLASH */
#endif /* CONFIG_NAND_BOOT */

/* Environment variables for NAND flash */
#define CONFIG_NAND_FLASH
#define NAND_INFO//chandra:only for debug
#define CONFIG_CMD_NAND 
#define CONFIG_IPROC_NAND 
#define CONFIG_SYS_MAX_NAND_DEVICE			    1
#define CONFIG_SYS_NAND_BASE		            0xdeadbeef
#define CONFIG_SYS_NAND_ONFI_DETECTION
#define CONFIG_INITRD_TAG                       1       /*  send initrd params           */
#define CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_DEVICE

/* Backup uboot uses same environment area as primary uboot */
#ifdef CONFIG_NAND_IPROC_BOOT
#define CONFIG_ENV_OFFSET                      0xc0000
#elif defined(CONFIG_CYGNUS_BOARD_DIAGS)
#define CONFIG_ENV_OFFSET                      0xf0000
#else
#define CONFIG_ENV_OFFSET                      0xa0000
#endif

#define CONFIG_CMD_SAVEENV



#if defined(CONFIG_IPROC_BOARD_DIAGS) && !defined(CONFIG_NO_CODE_RELOC)
#define IPROC_BOARD_DIAGS   
#endif /* CONFIG_IPROC_BOARD_DIAGS */

#if defined(CONFIG_CYGNUS_BOARD_DIAGS)
#define CYGNUS_BOARD_DIAGS
#endif /* CONFIG_CYGNUS_BOARD_DIAGS */


#ifdef IPROC_BOARD_DIAGS
//POST related defines
#define CONFIG_HAS_POST
//define post to support diags available, this is a ORed list
//See bitmask definition in post.h

#ifdef CYGNUS_BOARD_DIAGS
#define CONFIG_POST (CONFIG_SYS_POST_UART | CONFIG_SYS_POST_QSPI  | CONFIG_SYS_POST_SRAM \
    | CONFIG_SYS_POST_BBL | CONFIG_SYS_POST_SCI | CONFIG_SYS_POST_LCD | CONFIG_SYS_POST_MSR \
    | CONFIG_SYS_POST_KEYPAD | CONFIG_SYS_POST_TPRNT | CONFIG_SYS_POST_USB20 | CONFIG_SYS_POST_RGMII \
    | CONFIG_SYS_POST_PCIE | CONFIG_SYS_POST_I2C | CONFIG_SYS_POST_DS1WM | CONFIG_SYS_POST_MEMORY \
    | CONFIG_SYS_POST_SPI | CONFIG_SYS_POST_GPIO | CONFIG_SYS_POST_TOUCHSCREEN | CONFIG_SYS_POST_NAND  \
    | CONFIG_SYS_POST_PWM | CONFIG_SYS_POST_MMC | CONFIG_SYS_POST_TAMPER | CONFIG_SYS_POST_SMAU \
    | CONFIG_SYS_POST_AUDIO | CONFIG_SYS_POST_TDM | CONFIG_SYS_POST_CAN |  CONFIG_SYS_POST_CAM)
#else
#define CONFIG_POST (CONFIG_SYS_POST_UART | CONFIG_SYS_POST_QSPI | CONFIG_SYS_POST_NAND | \
                     CONFIG_SYS_POST_RGMII | CONFIG_SYS_POST_MEMORY)
#endif

#define CONFIG_CMD_DIAG

//#define CONFIG_SWANG_DEBUG_BUILD
/*
#define CONFIG_DIAGS_MEMTEST_START                (CONFIG_PHYS_SDRAM_1 + 0x1000000 )//offset by 16 MB if not running from L2
#define CONFIG_DIAGS_MEMTEST_END                  0x8000000
*/
#if (defined(CONFIG_CYGNUS))
#define CONFIG_DIAGS_MEMTEST_START                0x60000000 
#define CONFIG_DIAGS_MEMTEST_END                  0xA0000000
#else
#define CONFIG_DIAGS_MEMTEST_START                0x81000000//offset by 16 MB if not running from L2
#define CONFIG_DIAGS_MEMTEST_END                  0xC0000000
#endif


//Chip Common A UART 0
//#define CONFIG_SYS_NS16550_COM1                 (0x18000300)
//Chip Common A UART 1
//#define CONFIG_SYS_NS16550_COM2                 (0x18000400)
//Chip Common B UART 0
//#define CONFIG_SYS_NS16550_COM3                 (0x18008000)

#undef CONFIG_CYGNUS_EMULATION

#ifndef CONFIG_CYGNUS_EMULATION
#define  CONFIG_UART0_INDEX              0
#define  CONFIG_UART1_INDEX              1
#define  CONFIG_UART2_INDEX              2
#define  CONFIG_UART3_INDEX              3
#define  CONFIG_UART4_INDEX              4
#else
#define  CONFIG_UART0_INDEX              0
#define  CONFIG_UART1_INDEX              1
#define  CONFIG_UART2_INDEX              2
#define  CONFIG_UART3_INDEX              3
#endif

/* GPIO */
#define CONFIG_HAS_PWM
#define CONFIG_IPROC_GPIO
//#define CONFIG_IPROC_GSIOSPI


/* PCIE */
#define CONFIG_CMD_PCI
#define CONFIG_CMD_PCI_ENUM
#define CONFIG_PCI
#define CONFIG_PCI_SCAN_SHOW
#define CONFIG_IPROC_PCIE

/* I2C */
#define CONFIG_CMD_I2C
#define CONFIG_IPROC_I2C
#define CONFIG_SYS_I2C_SPEED    100000       /* Default on 100KHz */
#define CONFIG_SYS_I2C_SLAVE    0xff    /* No slave address */
#define CONFIG_I2C_MULTI_BUS

/* SDIO */
#define CONFIG_CMD_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_IPROC_MMC
#define CONFIG_CYGNUS_EMMC

/* USB */
#define CONFIG_CMD_USB
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_IPROC
#define CONFIG_USB_STORAGE
#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION
#define CONFIG_LEGACY_USB_INIT_SEQ

/*D1W */
#define CONFIG_IPROC_DS1WM

#endif /* ENF of DIAGS */

#ifdef IPROC_BOARD_DIAGS
#ifdef CONFIG_SYS_HZ
#undef CONFIG_SYS_HZ
#endif
#define CONFIG_SYS_HZ       1000*1000
#else
#define CONFIG_SYS_HZ       1000
#endif

#define CONFIG_ETHADDR      d4:ae:52:bc:a5:08
#define CONFIG_IPADDR       192.168.0.101
#define CONFIG_NETMASK      255.255.255.0
#define CONFIG_GATEWAYIP    192.168.0.1
#define CONFIG_SERVERIP     192.168.0.100

#define CONFIG_INITRD_TAG        1       /*  send initrd params           */
#define CONFIG_CMD_RUN
#define CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_DEVICE
#define MTDIDS_DEFAULT      "nand0=nand_iproc.0"
#define MTDPARTS_DEFAULT    "mtdparts=mtdparts=nand_iproc.0:1024k(nboot),1024k(nenv),8192k(nsystem),1038336k(ndata)"
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_BOOT

#define CONFIG_BAUDRATE                 115200
#ifdef CONFIG_SYS_BAUDRATE_TABLE
#undef CONFIG_SYS_BAUDRATE_TABLE
#endif
#define CONFIG_SYS_BAUDRATE_TABLE   {9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600}
#ifdef CONFIG_BOOTARGS
#undef CONFIG_BOOTARGS
#endif
#define CONFIG_BOOTARGS         "console=ttyS0,9600n8 maxcpus=1 mem=512M root=/dev/ram0 rw"
#define CONFIG_BOOTFILE         "uImage"

#ifdef CONFIG_BOOTCOMMAND
#undef CONFIG_BOOTCOMMAND
#endif
#define CONFIG_BOOTCOMMAND      "dhcp; run nfsargs; bootm;" /* UBoot command issued on power up */

#define CONFIG_EXTRA_ENV_SETTINGS \
    "brcmtag=0\0"               \
    "console=ttyS0\0"           \
    "loglevel=7\0"              \
    "machid=0x127f\0"            \
    "tftpblocksize=512\0"       \
    "hostname=cygnus\0"      \
    "vlan1ports=0 1 5 8u\0"   \
    "nfsroot=/dev/nfs rw nfsroot=/nfs/rootfs\0"        \
    "nfsargs=setenv bootargs "  \
        "loglevel=${loglevel} " \
        "console=${console},${baudrate}n8 "   \
        "ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}:cygnus:: " \
        "maxcpus=1 "            \
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
        "\0"                    \
    "wmail=mw.l 0x03024028 12345678 1\0"    \
    "gmail=mw.l 0x03024030 12345678 1\0"

/*#define CONFIG_BOOTCOMMAND \
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
#define SHMOO_MIN_BLOCK_SIZE    0x10000

/* NAND flash configuration*/
#define CONFIG_SHMOO_REUSE_NAND_OFFSET          0x00400000
#define CONFIG_SHMOO_REUSE_NAND_RANGE           0x00200000

#if defined(CYGNUS_BOARD_DIAGS)
#define CONFIG_SHMOO_REUSE_QSPI_OFFSET          0x00100000
/* Offset of NOR flash to save Shmoo values */
#define CONFIG_SHMOO_REUSE_NOR_OFFSET           0x00100000
#else
#define CONFIG_SHMOO_REUSE_QSPI_OFFSET          0x000E0000
/* Offset of NOR flash to save Shmoo values */
#define CONFIG_SHMOO_REUSE_NOR_OFFSET           0x000E0000
#endif
/* Offset of NAND flash to save Shmoo values */
#define CONFIG_SHMOO_REUSE_NAND_OFFSET          0x00400000
/* Memory test address to verify stored SHMOO values */
#define CONFIG_SHMOO_REUSE_MEMTEST_START        0x61000000
/* Memory test address to verify stored SHMOO values */
#define CONFIG_SHMOO_REUSE_MEMTEST_LENGTH       0x1000
#define CONFIG_SHMOO_AND28_REUSE 1

#define CONFIG_AES
#define CONFIG_SHMOO_REUSE_DELAY_MSECS          50

#endif /* CYGNUS_CONFIG_H_ */
