ifdef CONFIG_NAND_IPROC_BOOT
	CONFIG_SYS_TEXT_BASE = 0xE0000000
else
	ifdef CONFIG_ROM_BOOT
		CONFIG_SYS_TEXT_BASE = 0xFFF90000
	else
		CONFIG_SYS_TEXT_BASE = 0xF0000000
	endif
endif

ifdef CONFIG_NO_CODE_RELOC
	ifdef CONFIG_HAVE_DDR
		LDSCRIPT := $(SRCTREE)/board/$(BOARDDIR)/u-boot-ddr.lds
	else
		LDSCRIPT := $(SRCTREE)/board/$(BOARDDIR)/u-boot-sram.lds
	endif
else
	LDSCRIPT := $(SRCTREE)/board/$(BOARDDIR)/u-boot-relocate.lds
endif

#PLATFORM_RELFLAGS += -DSVN_REVISION=' " $(SVN_REV)"'
