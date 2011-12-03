#******************************************************************************
#
# Makefile - Rules for building the Ethernet with uIP.
#
# Copyright (c) 2009-2011 Texas Instruments Incorporated.  All rights reserved.
# Software License Agreement
# 
# Texas Instruments (TI) is supplying this software for use solely and
# exclusively on TI's microcontroller products. The software is owned by
# TI and/or its suppliers, and is protected under applicable copyright
# laws. You may not combine this software with "viral" open-source
# software in order to form a larger program.
# 
# THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
# NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
# NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
# CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
# DAMAGES, FOR ANY REASON WHATSOEVER.
# 
# This is part of revision 7611 of the EK-LM3S9B90 Firmware Package.
#
#******************************************************************************

#
# Defines the part type that this project uses.
#
PART=LM3S9B90

#Program name
NAME = enet_uip

#include the toolchain settings 
include $.../toolchainsettings

#For readablity we repeat the contents of toolchainsettings here:

##start toolchain settings
##COMPORT should be COMx or /dev/ttyUSBx or similar for windows and linux respectivly
#COMPORT = /dev/ttyS3
#SERIALPROG = 0
#WINDOWS = 0

##Use forward slashes for the paths
##DIR_STELLARISWARE is typically C:/Users/USERNAME/Development/StellarisWare or /home/USERNAME/Development/StellarisWare
#DIR_STELLARISWARE = /home/USERNAME/Development/StellarisWare
#DIR_DRIVERLIB = $(DIR_STELLARISWARE)/driverlib
#DIR_USBLIB = $(DIR_STELLARISWARE)/usblib
#DIR_UTILS = $(DIR_STELLARISWARE)/utils

##end toolchain settings


#Toolchain Command Config

os:=${shell uname -s}

CYGWIN = 0
ifneq ($(findstring CYGWIN, ${os}), )
	CYGWIN = 1
endif

ifeq ($(SERIALPROG),1)
	LINKER = bootlink.ld
	ifeq ($(WINDOWS),1)
		FLASHCMD = lmflash -q manual -i serial -p $(COMPORT) -b 115200 -r --offset=0x1000 --xfer-size=32 $(NAME).bin
	else
		FLASHCMD = sflash -p 0x1000 -c $(COMPORT) -b 115200 -s 32 $(NAME).bin
	endif
else
	LINKER = link.ld

	ifeq ($(WINDOWS),1)
		FLASHCMD = perl ./do_flash.pl $(NAME).bin
	else 
		FLASHCMD = ./do_flash.pl $(NAME).bin
	endif

endif


ifeq ($(WINDOWS),1)	
	ifeq ($(CYGWIN),1)
		CLEANCMD = rm -rf ${COMPILER} ${wildcard *~}
		COPYCMD = cp ./${COMPILER}/$(NAME).bin ./
	else
		CLEANCMD = del /s /q ${COMPILER} ${wildcard *~}
		COPYCMD = copy ./${COMPILER}/$(NAME).bin ./
	endif	
else 
	CLEANCMD = rm -rf ${COMPILER} ${wildcard *~}
	COPYCMD = cp ./${COMPILER}/$(NAME).bin ./
endif

#
# The base directory for StellarisWare.
#
ROOT=$(DIR_STELLARISWARE)

#
# Include the common make definitions.
#
include ${ROOT}/makedefs


#
# Where to find source files that do not live in this directory.
#
VPATH=$(DIR_STELLARISWARE)/third_party/uip-1.0/apps/dhcpc
VPATH+=$(DIR_STELLARISWARE)/third_party/uip-1.0/apps/httpd
VPATH+=$(DIR_STELLARISWARE)/third_party/uip-1.0/uip
VPATH+=$(DIR_STELLARISWARE)/utils

#
# Where to find header files that do not live in the source directory.
#
IPATH=.
IPATH+=$(DIR_STELLARISWARE)
IPATH+=$(DIR_STELLARISWARE)/third_party/uip-1.0/apps
IPATH+=$(DIR_STELLARISWARE)/third_party/uip-1.0/uip
IPATH+=$(DIR_STELLARISWARE)/third_party/uip-1.0

# The default rule, which causes the Ethernet with uIP to be built.
#
all: ${COMPILER}
all: ${COMPILER}/enet_uip.axf

#
# The rule to clean out all the build products.
#
clean:
	@rm -rf ${COMPILER} ${wildcard *~}

#
# Build and flash
#
flash: all 
	$(COPYCMD)
	$(FLASHCMD)

#
# The rule to create the target directory.
#
${COMPILER}:
	@mkdir -p ${COMPILER}

#
# Rules for building the Ethernet with uIP.
#
${COMPILER}/enet_uip.axf: ${COMPILER}/dhcpc.o
${COMPILER}/enet_uip.axf: ${COMPILER}/enet_uip.o
${COMPILER}/enet_uip.axf: ${COMPILER}/httpd.o
${COMPILER}/enet_uip.axf: ${COMPILER}/startup_${COMPILER}.o
${COMPILER}/enet_uip.axf: ${COMPILER}/uartstdio.o
${COMPILER}/enet_uip.axf: ${COMPILER}/uip.o
${COMPILER}/enet_uip.axf: ${COMPILER}/uip_arp.o
${COMPILER}/enet_uip.axf: ${COMPILER}/uip_timer.o
${COMPILER}/enet_uip.axf: ${COMPILER}/ustdlib.o
${COMPILER}/enet_uip.axf: ${ROOT}/driverlib/${COMPILER}/libdriver.a
${COMPILER}/enet_uip.axf: $(LINKER)
SCATTERgcc_enet_uip=$(LINKER)
ENTRY_enet_uip=ResetISR
CFLAGSgcc=-DTARGET_IS_TEMPEST_RC5

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
