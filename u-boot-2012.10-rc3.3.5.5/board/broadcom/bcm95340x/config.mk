ifdef CONFIG_IPROC_ROM_BUILD
CONFIG_SYS_TEXT_BASE = 0xFFF90000
else
ifdef CONFIG_NAND_IPROC_BOOT
CONFIG_SYS_TEXT_BASE = 0xE0000000
else 
ifdef CONFIG_NOR_IPROC_BOOT
CONFIG_SYS_TEXT_BASE = 0xE8000000
else 
CONFIG_SYS_TEXT_BASE = 0xF0000000
endif 
endif
endif

# Default: little endian
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
else # !CONFIG_NO_CODE_RELOC
ifdef CONFIG_L2C_AS_RAM
LDSCRIPT := $(SRCTREE)/board/$(BOARDDIR)/u-boot-l2c.lds
endif
endif

# Big endian
ifdef CONFIG_SYS_BIG_ENDIAN
ifdef CONFIG_NO_CODE_RELOC
ifdef CONFIG_IPROC_ROM_BUILD
ifdef CONFIG_L2C_AS_RAM
LDSCRIPT := $(SRCTREE)/board/$(BOARDDIR)/u-boot-nr-rom-be.lds
else
LDSCRIPT := $(SRCTREE)/board/$(BOARDDIR)/u-boot-nr-rom-sram-be.lds
endif
else
ifdef CONFIG_L2C_AS_RAM
LDSCRIPT := $(SRCTREE)/board/$(BOARDDIR)/u-boot-nr-be.lds
else
LDSCRIPT := $(SRCTREE)/board/$(BOARDDIR)/u-boot-nr-sram-be.lds
endif
endif
else # !CONFIG_NO_CODE_RELOC
ifdef CONFIG_L2C_AS_RAM
LDSCRIPT := $(SRCTREE)/board/$(BOARDDIR)/u-boot-l2c-be.lds
endif
endif
endif

#PLATFORM_RELFLAGS += -DSVN_REVISION=' " $(SVN_REV)"'
