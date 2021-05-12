/*
 * 001.c
 *
 * Created: 11.04.2019 10:57:39
 * Author : Tomek
 */ 
#define F_CPU 16000000UL  // 16 MHz
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h> 
#include <util/delay.h>


int main(void)
{
    /* Replace with your application code */
	        //76543210
	DDRB =  0b10000000;
	PORTB = 0b00000000;
	        //76543210	
			PORTB = 0b10000000;
			_delay_ms(1000);
			PORTB = 0b00000000;
			_delay_ms(1000);
			PORTB = 0b10000000;
			_delay_ms(1000);
			PORTB = 0b00000000;
			_delay_ms(1000);
	
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
		/*PORTB = 0b10000000;
		_delay_ms(1000);
		PORTB = 0b00000000;
		_delay_ms(1000);
		*/
    }
}

ISR(USART0_RX_vect)
{
	switch ((char) UDR0)
	{
		case 'w':
		{
			//PORTB = 0b10000000 ^ PORTB;
			PORTB = 0b10000000;
		}
		break;
		case 'q':
		{
			//PORTB = 0b10000000 ^ PORTB;
			PORTB = 0b00000000;
		}
		break;
		case 's':
		{
			_delay_ms(1000);
		}
		break;
		default: 
		{
			PORTB = 0b10000000 ^ PORTB;
		}
		break;
	}
}

