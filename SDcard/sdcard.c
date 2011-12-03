/*
Procyon Port of CHAN's FatFS for SD card
This port is incomplete, but basically functional
Further example releases will update this example
2011 teho Labs/B. A. Bryce
*/

#include <stdint.h>
#include <string.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"
#include "inc/hw_ssi.h"
#include "inc/hw_gpio.h"
#include "driverlib/systick.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/ssi.h"
#include "driverlib/interrupt.h"

#include "monitor.h"
#include "integer.h"

#include "ffconf.h"
#include "diskio.h"
#include "ff.h"
#include "fatrtc.h"


#define bufferSize 2048

#define STDIO_BASE UART0_BASE
#define sendSPI(a) ROM_SSIDataPut(SSI0_BASE, a)

#define TICKS_PER_SECOND 100
#define MS_PER_SYSTICK (1000 / TICKS_PER_SECOND)

// Prototypes
void configureHW(void);
void myDelay(unsigned long delay);

//STDIO-Monitor related
void std_putchar(uint8_t c);
uint8_t std_getchar(void);


//SD card related
void SD_SPI_slow(void);
void SD_SPI_fast(void);
uint8_t xmit_spi1(uint8_t cData);

void disk_timerproc (void);
DWORD get_fattime(void);
static void put_rc (FRESULT rc);



DWORD acc_size;			//Work register for fs command
WORD acc_files, acc_dirs;
FILINFO Finfo;

FATFS Fatfs[_VOLUMES];		// File system object for each logical drive
FATFS FsBackUp;
BYTE Buff[bufferSize];		// Working Buffer

char myPath[128];		//Scatch Path


static FRESULT scan_files (
	char* path		/* Pointer to the working buffer with start path */
)
{
	DIR dirs;
	FRESULT res;
	int i;
	char *fn;

	res = f_opendir(&dirs, path);
	put_rc(res);
	if (res == FR_OK) {
		i = strlen(path);
		while (((res = f_readdir(&dirs, &Finfo)) == FR_OK) && Finfo.fname[0]) {
			if (_FS_RPATH && Finfo.fname[0] == '.') continue;
#if _USE_LFN
			fn = *Finfo.lfname ? Finfo.lfname : Finfo.fname;
#else
			fn = Finfo.fname;
#endif
			if (Finfo.fattrib & AM_DIR) {
				acc_dirs++;
				*(path+i) = '/'; strcpy(path+i+1, fn);
				res = scan_files(path);
				*(path+i) = '\0';
				if (res != FR_OK) break;
			} else {
//				xprintf(PSTR("%s/%s\n"), path, fn);
				xprintf("%s/%s\n", path, fn);
				acc_files++;
				acc_size += Finfo.fsize;
			}
		}
	}

	return res;
}



int main(void)
{
	configureHW();



	BYTE res;
	UINT s1, s2;
	FIL file1, file2;		/* File object */


	put_rc(f_mount(0, &Fatfs[0]));

	put_rc(f_open(&file1, "0:test.txt", FA_OPEN_EXISTING | FA_READ));
	
	// read a whole file until done
	do
	{
		res = f_read(&file1, Buff, sizeof(Buff), &s1);    // Read a chunk of src file 
		if(s1)xputs(Buff);
	} while(res || s1 != 0);
	put_rc(f_close(&file1));
	f_mount(0, NULL);

	//For some reason I have to unmount/remount it
	put_rc(f_mount(0, &Fatfs[0]));
	myPath[0] = '\0';
	scan_files(myPath);
	f_mount(0, NULL);

	while(1)
	{
		myDelay(1000000);
		ROM_GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, 0xFF);	//LED toggle		
		
		myDelay(1000000);
		ROM_GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, 0x00);	//LED toggle

	}
}

void configureHW(void)
{

	//16 MHz Crystal with PLL at 50 MHz
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_4  | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	//*********** SETUP USART0 for Chan monitor functions
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);		//Turn on GPIOA
	ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);	//Assign USART pins
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);		//Enable USART 0
	// Setup USART0 for 115K, n-8-1
	ROM_UARTConfigSetExpClk(UART0_BASE, ROM_SysCtlClockGet(), 115200, (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE | UART_CONFIG_WLEN_8)); 
	ROM_UARTEnable(UART0_BASE);

	//Functions for Monitor.c
	xfunc_out = std_putchar;
	xfunc_in = std_getchar;
	
	//Setup SPI 0 (SSI 0)
	//GPIOA is already on
	ROM_GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);	//Assign SPI0 pins
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);						//Enable SPI0
	ROM_SSIConfigSetExpClk(SSI0_BASE, ROM_SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 4000000, 8);	
	ROM_SSIEnable(SSI0_BASE);

	//Setup SPI 1 (SSI 1)
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);		//Turn on GPIOH
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);		//Turn on GPIOH
	GPIOPinConfigure(GPIO_PF2_SSI1CLK);
	//GPIOPinConfigure(GPIO_PF3_SSI1FSS); // using software instead
	GPIOPinConfigure(GPIO_PH6_SSI1RX);
	GPIOPinConfigure(GPIO_PH7_SSI1TX);
	ROM_GPIOPinTypeSSI(GPIO_PORTH_BASE, GPIO_PIN_6 | GPIO_PIN_7);
	ROM_GPIOPinTypeSSI(GPIO_PORTF_BASE, GPIO_PIN_2 );//| GPIO_PIN_3
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);	
	ROM_SSIConfigSetExpClk(SSI1_BASE, ROM_SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 200000, 8);	
	ROM_SSIEnable(SSI1_BASE);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);	//software CS pin

	//For LED blinking
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_5);

	// Configure SysTick for a 100Hz interrupt.
	SysTickPeriodSet(SysCtlClockGet() / TICKS_PER_SECOND);
	SysTickEnable();
	SysTickIntEnable();

}


void std_putchar(uint8_t c) 
{
	while(HWREG(STDIO_BASE + UART_O_FR) & UART_FR_TXFF);	// Wait until FIFO has space
	HWREG(STDIO_BASE + UART_O_DR) = c;			// Send the character
}


uint8_t std_getchar(void) 
{
	
	while(HWREG(STDIO_BASE + UART_O_FR) & UART_FR_RXFE);	//Wait for a character
	return ((uint8_t) (HWREG(STDIO_BASE + UART_O_DR)));	//Return the character

}


void myDelay(unsigned long delay)
{ 
	while(delay)
	{ 
		delay--;
		__asm__ __volatile__("mov r0,r0");
	}
}


//This is a more typical SPI function for SPI1 than the StellarisWare versions
uint8_t xmit_spi1(uint8_t cData)
{
    while(!(HWREG(SSI1_BASE + SSI_O_SR) & SSI_SR_TNF));	//wait for space
    HWREG(SSI1_BASE + SSI_O_DR) = cData;		//put the data in
    while(!(HWREG(SSI1_BASE + SSI_O_SR) & SSI_SR_RNE));	//wait for returned data
    return (uint8_t) (HWREG(SSI1_BASE + SSI_O_DR));	//return the byte
}


// CHAN related functions

static void put_rc (FRESULT rc)
{

	xputs("FR_");
	switch(rc)
	{
		case FR_OK:
		xputs("OK\n");
		break;

		case FR_DISK_ERR:
		xputs("DISK_ERR\n");
		break;

		case FR_NOT_READY:
		xputs("NOT_READY\n");
		break;

		case FR_NO_FILE:
		xputs("NO_FILE\n");
		break;

		case FR_NO_PATH:
		xputs("NO_PATH\n");
		break;

		case FR_INVALID_NAME:
		xputs("INVALID_NAME\n");
		break;

		case FR_NOT_ENABLED:
		xputs("NOT_ENABLED\n");
		break;

		case FR_NO_FILESYSTEM:
		xputs("NO_FILESYSTEM\n");
		break;

		case FR_DENIED:
		xputs("DENIED\n");
		break;

		case FR_EXIST:
		xputs("EXIST\n");
		break;

		case FR_INVALID_OBJECT:
		xputs("INVALID_OBJECT\n");
		break;


		case FR_WRITE_PROTECTED:
		xputs("WRITE_PROTECTED\n");
		break;


		case FR_INVALID_DRIVE:
		xputs("INVALID_DRIVE\n");
		break;


		case FR_MKFS_ABORTED:
		xputs("MKFS_ABORTEDE\n");
		break;

		case FR_TIMEOUT:
		xputs("TIMEOUT\n");
		break;

		case FR_LOCKED:
		xputs("LOCKED\n");
		break;

		case FR_NOT_ENOUGH_CORE:
		xputs("NOT_ENOUGH_CORE\n");
		break;

		case FR_TOO_MANY_OPEN_FILES:
		xputs("TOO_MANY_OPEN_FILES\n");
		break;
	}


}




/*---------------------------------------------------------*/
/* User Provided Timer Function for FatFs module           */
/*---------------------------------------------------------*/
/* This is a real time clock service to be called from     */
/* FatFs module. Any valid time must be returned even if   */
/* the system does not support a real time clock.          */
/* This is not required in read-only configuration.        */


DWORD get_fattime(void)
{
	fatRTC rtc; // totally fake
	rtc.year = 1;
	rtc.mday = 1;
	rtc.hour = 1;
	rtc.min = 1;
	rtc.sec = 1;



	// Pack date and time into a DWORD variable 
	return	  ((DWORD)(rtc.year - 1980) << 25)
			| ((DWORD)rtc.month << 21)
			| ((DWORD)rtc.mday << 16)
			| ((DWORD)rtc.hour << 11)
			| ((DWORD)rtc.min << 5)
			| ((DWORD)rtc.sec >> 1);

}




// 100 Hz timer
void SysTickIntHandler(void)
{

	disk_timerproc();

}


void SD_SPI_slow(void)
{
	ROM_SSIDisable(SSI1_BASE);	
	ROM_SSIConfigSetExpClk(SSI1_BASE, ROM_SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 300000, 8);
	ROM_SSIEnable(SSI1_BASE);
}
void SD_SPI_fast(void)
{
	ROM_SSIDisable(SSI1_BASE);
	ROM_SSIConfigSetExpClk(SSI1_BASE, ROM_SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 5000000, 8);
	ROM_SSIEnable(SSI1_BASE);
}
