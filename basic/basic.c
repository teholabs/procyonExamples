/*
Basic services
B. A. Bryce/teho Labs 2010
This design is slightly less efficent than using some of the Luminary codes
However the use Chan's libraries for things should make it more portable

Contains code for both HW and Software control of SS pins
*/

#include <stdint.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"
#include "inc/hw_ssi.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/ssi.h"
#include "driverlib/interrupt.h"

#include "monitor.h"
#include "integer.h"

#define STDIO_BASE UART0_BASE
#define sendSPI(a) ROM_SSIDataPut(SSI0_BASE, a)

// Prototypes
void configureHW(void);
void myDelay(unsigned long delay);

//STDIO-Monitor related
void std_putchar(uint8_t c);
uint8_t std_getchar(void);

// SPI related
void softSS_off(unsigned long ssiBase, unsigned long ssPort, unsigned char ssPin);
void softSS_on(unsigned long ssiBase, unsigned long ssPort, unsigned char ssPin);



int main(void)
{
	configureHW();

	xprintf("It's portable!\n");	//Generic crossplatform printf
	while(1)
	{
		myDelay(1000000);
		ROM_GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, 0xFF);	//LED toggle
		ROM_SSIDataPut(SSI0_BASE, 'U');				//HW SS SPI0
		
		
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


	//For LED blinking
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_5);

}

void softSS_off(unsigned long ssiBase, unsigned long ssPort, unsigned char ssPin)
{

	while((HWREG(ssiBase + SSI_O_SR) & SSI_SR_BSY));
	HWREG(ssPort + (GPIO_O_DATA + (ssPin << 2))) = 0xFF;

}


void softSS_on(unsigned long ssiBase, unsigned long ssPort, unsigned char ssPin)
{

	while((HWREG(ssiBase + SSI_O_SR) & SSI_SR_BSY));
	HWREG(ssPort + (GPIO_O_DATA + (ssPin << 2))) = 0x00;

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


