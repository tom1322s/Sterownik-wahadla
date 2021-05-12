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
#include <string.h>
#include <stdlib.h>

#define MAX_RECEIVE_SIZE 5
#define MAX_TRANSMISION_SIZE 50


volatile char Str[MAX_RECEIVE_SIZE+1];   //ta zmienna zostanie powo³ana pod konkretnym adresem w pamieci
volatile int iStr;      //idex do str
volatile unsigned char strValid;  //odebrany komunikat zmienna prawda fa³sz 

volatile char StrT[MAX_TRANSMISION_SIZE+1];
volatile int iStrT; 

void prepareToSend(char* text);
void send();

int main(void)
{
    /* Replace with your application code */
	        //76543210
	DDRB =  0b10000000; //nawet jesli potem to jest pwm i tak trzeba ustawic
	//PORTB = 0b10000000;
	        //76543210	
	
	//transmisja asynchroniczna tylko do odbierania
	UCSR0A = 0b00000010;
	UCSR0B = 0b10011000; 
	UCSR0C = 0b00000110;
	UBRR0  = 16;
	//
	//USTAWIENIA PWM TIMER3
	         //76543210
	TCCR1A = 0b00001011;
	TCCR1B = 0b00001100; //ustawi³em 10 bit rozdzielczosc oraz prescalej /64
	//tu potem wpisuje - OCR2A
	
	//bezposrednio przed while w³aczam przerwania
	iStr=0;
	sei();
	int refference;			//wartosc odczytana z transmicji
	char actionFlag=0;		//flaga akcji
	
    while (1) 
    {
		if(strValid)		//flaga zakonczenia transmisji
		{
			if(iStr<MAX_RECEIVE_SIZE)		//sprawdzenie poprawnosci d³ugosci transmisji
			{
				//dekodowanie
				refference=0;
				int multiplayer=1;
				for(int i=iStr-1;i>=0;i--)
				{
					if(Str[i]=='-') refference*=-1;
					else{
					refference+=(Str[i]-'0')*multiplayer;
					multiplayer*=10;
					}
				}
				//
				actionFlag=1;
			}
			
			iStr=0;
			strValid=0;
		}
		
		if(actionFlag)
		{
		
			if(refference==100)
			{
				prepareToSend("rowny");
				send();
			}
			else if(refference>100)
			UDR0='>';
			else if(refference<100)
			UDR0='<';
			actionFlag=0;
			
			OCR1C=refference;
		}

    }
}

ISR(USART0_RX_vect)  //przerwanie od odbioru
{
	char c=UDR0;
	if (c) 
	{
		Str[iStr]=c;
		iStr++;
		if(iStr>=MAX_RECEIVE_SIZE) iStr=MAX_RECEIVE_SIZE;
	}
	else strValid=1;
	

}

ISR(USART0_UDRE_vect)		//przerwanie po wyslaniu 1 UDR0
{
	char c=StrT[iStrT];
	UDR0=c;
	iStrT++;
	if(!c) {
		UCSR0B&=0b11011111;		//wy³aczenie przerwania po wyslaniu 1 UDR0
		UCSR0B|=0b01000000;		//w³aczenie przerwania od zakonczonej transmisji
	}
}

ISR(USART0_TX_vect)				//przerwanie od zakonczonej transmisji
{
	iStrT=0;
	UCSR0B&=0b10111111;			//wy³aczenie przerwania od zakonczonej transmisji
}

void prepareToSend(char* text) //nadpisanie do globalnego buffora
{
	int maxL=strlen(text);
	int i;
	for(i=0;i<maxL;i++)
	{
		StrT[i]=text[i];
	}
	StrT[i]=0;
}
void send()
{
	iStrT=0;
	UCSR0B|=0b00100000;		//w³aczenie przerwania po wyslaniu 1 UDR0
}

