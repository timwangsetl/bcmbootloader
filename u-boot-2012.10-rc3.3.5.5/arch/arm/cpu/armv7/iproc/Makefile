#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#

include $(TOPDIR)/config.mk

LIB	= $(obj)libiproc.o
SOBJS	+= cache.o
COBJS	+= shmoo_sig.o timer.o reset.o gic.o l2cache.o benchscreen.o genpll.o pwrctl.o armpll.o 

ifdef CONFIG_CYGNUS
COBJS += shmoo_and28_reuse.o
ifndef CONFIG_CYGNUS_EMULATION
COBJS += ddr_init.o
endif
endif

ifndef CONFIG_CYGNUS

ifdef CONFIG_SPL_BUILD
COBJS   += iproc-spl.o
endif

ifndef CONFIG_IPROC_NO_DDR
ifdef CONFIG_SHMOO_AND28_REUSE
COBJS += shmoo_and28_reuse.o
endif
COBJS += ddr_init.o
endif

endif #end of CONFIG_CYGNUS

ifeq (${CONFIG_IPROC_NAND},y)
COBJS	+= iproc_nand.o
endif


SRCS	:= $(START:.o=.S) $(SOBJS:.o=.S) $(COBJS:.o=.c)

OBJS	:= $(addprefix $(obj),$(COBJS) $(SOBJS))
START	:= $(addprefix $(obj),$(START))

all:	$(obj).depend $(LIB)

$(LIB):	$(OBJS)
	$(call cmd_link_o_target, $(OBJS))

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
