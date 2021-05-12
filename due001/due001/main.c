/*
 * due001.c
 *
 * Created: 26.06.2019 11:54:44
 * Author : Tomek
 */ 


#include "sam.h"


int main(void)
{
    /* Initialize the SAM system */
    SystemInit();
	
					//10987654321098765432109876543210
	//PIOB->PIO_OER = 0b00001000000000000000000000000000;
	//PIOB->PIO_PER = 0b00001000000000000000000000000000;
	//PIOB->PIO_SODR= 0b00001000000000000000000000000000;
	//PIOC = 0b00000000000000000000000000000000;
    /* Replace with your application code */
	
	PIOB->PIO_OER = PIO_PB27;
	PIOB->PIO_PER = PIO_PB27;
	PIOB->PIO_PUDR = PIO_PB27;
	//PIOB->PIO_SODR = PIO_PB27;
	
    while (1) 
    {
		PIOB->PIO_CODR = PIO_PB27;
    }
}
