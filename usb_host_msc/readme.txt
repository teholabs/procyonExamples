This example code was modified to work on Eridani an LM3S3651 based board
The author imposes no change in the licence from TI below
The software remains AS IS with no Warranty

USB Mass Storage Class Host

This example application demonstrates reading a file system from a USB mass
storage class device.  It makes use of FatFs, a FAT file system driver.  It
provides a simple command console via the UART for issuing commands to view
and navigate the file system on the mass storage device.

The first UART, which is connected to the FTDI virtual serial port on the
evaluation board, is configured for 115,200 bits per second, and 8-N-1
mode.  When the program is started a message will be printed to the
terminal.  Type ``help'' for command help.

For additional details about FatFs, see the following site:
http://elm-chan.org/fsw/ff/00index_e.html

-------------------------------------------------------------------------------

Copyright (c) 2009-2010 Texas Instruments Incorporated.  All rights reserved.
Software License Agreement

Texas Instruments (TI) is supplying this software for use solely and
exclusively on TI's microcontroller products. The software is owned by
TI and/or its suppliers, and is protected under applicable copyright
laws. You may not combine this software with "viral" open-source
software in order to form a larger program.

THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
DAMAGES, FOR ANY REASON WHATSOEVER.

This is part of revision 6594 of the EK-LM3S9B92 Firmware Package.
