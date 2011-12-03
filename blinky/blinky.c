/*
Blinky flash PB5
teho Labs 2011/B. A. Bryce
*/

#include "inc/lm3s9b90.h"

#define blinkTime 200000

// Function prototypes
void myDelay(unsigned long delay);

int main(void)
{

    // Enable GIPO Port B
    SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOB;
	
    myDelay(1);

    // Enable the GPIO pin for the LED (PD0).  Set the direction as output, and
    // enable the GPIO pin for digital function.

    GPIO_PORTB_DIR_R |= (1<<5);
    GPIO_PORTB_DEN_R |= (1<<5);


    while(1)
    {
	myDelay(blinkTime); //Wait ~ blinkTime cycles
		
        GPIO_PORTB_DATA_R |= (1<<5);// PB5 on
		
	myDelay(blinkTime);//Wait ~ blinkTime cycles
		
        GPIO_PORTB_DATA_R &= ~((1<<5));// PB5 off

    }
}

//Waste cycle delay function
void myDelay(unsigned long delay)
{ 
	while(delay)
	{ 
		delay--;
		__asm__ __volatile__("mov r0,r0");
	}
}
