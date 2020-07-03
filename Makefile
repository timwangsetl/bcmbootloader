ifndef TOPDIR
export TOPDIR = $(shell (cd ./ && pwd -P))
endif

export PATH:=${PATH}:/opt/iproc/buildroot/host/usr/bin

ifndef BINPATH
export BINPATH=$(TOPDIR)/../binfile
endif

ifndef BOOTPATH
export BOOTPATH=$(TOPDIR)/u-boot-2012.10-rc3.3.5.5
endif

all: boot_build 

boot_build: #clean
	@echo making bootloader
	cd $(BOOTPATH) && make -j4 CROSS_COMPILE="arm-linux-"  ARCH=arm hurricane2_config
	cd $(BOOTPATH) && make 
	cp $(BOOTPATH)/u-boot.bin $(BINPATH) 


clean: 
	cd $(BOOTPATH)/ && make distclean
	cd $(BOOTPATH)/ && rm -fr hurricane2_config

