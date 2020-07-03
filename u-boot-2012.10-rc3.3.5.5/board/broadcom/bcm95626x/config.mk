ifdef CONFIG_IPROC_ROM_BUILD
CONFIG_SYS_TEXT_BASE = 0xFFF90000
else
ifdef CONFIG_NAND_IPROC_BOOT
CONFIG_SYS_TEXT_BASE = 0xE0000000
else 
CONFIG_SYS_TEXT_BASE = 0xF0000000
endif
endif

ifdef CONFIG_NO_CODE_RELOC
ifdef CONFIG_IPROC_ROM_BUILD
ifdef CONFIG_L2C_AS_RAM
LDSCRIPT := $(SRCTREE)/board/$(BOARDDIR)/u-boot-nr-rom.lds
else
LDSCRIPT := $(SRCTREE)/board/$(BOARDDIR)/u-boot-nr-rom-sram.lds
endif
else
ifdef CONFIG_L2C_AS_RAM
LDSCRIPT := $(SRCTREE)/board/$(BOARDDIR)/u-boot-nr.lds
else
LDSCRIPT := $(SRCTREE)/board/$(BOARDDIR)/u-boot-nr-sram.lds
endif
endif
endif

#PLATFORM_RELFLAGS += -DSVN_REVISION=' " $(SVN_REV)"'
