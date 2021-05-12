/*
 * 001.c
 *
 * Created: 11.04.2019 10:57:39
 * Author : Tomek
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h> 
/*
h5 h6 kierunek mostka 
b4 pwm
b5 bridge diagnozis

d3 d2 enkoder
prad analog f1

*/

int main(void)
{
    /* Replace with your application code */
	        //76543210
	DDRH =  0b01100000;  // config H3 as output
	DDRB =  0b00110000;
	PORTH = 0b00100000;  // set h3 high
	PORTB = 0b00110000;
	         //76543210	
	
	//transmisja asynchroniczna tylko do odbierania
	UCSR0A = 0b00000010;
	UCSR0B = 0b10010000; 
	UCSR0C = 0b00000110;
	UBRR0  = 16;
	//
	
	//TCCR2A = 0b00000000;
	
	//bezposrednio przed while w³aczam przerwania
	sei();
	
    while (1) 
    {
    }
}

ISR(USART0_RX_vect)
{
	switch (UDR0)
	{
		case 'w':
		break;
		default:
		break;
	}
}

