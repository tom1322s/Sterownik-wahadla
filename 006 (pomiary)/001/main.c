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
#include <time.h>

#define MAX_RECEIVE_SIZE 5
#define MAX_TRANSMISION_SIZE 50


volatile char Str[MAX_RECEIVE_SIZE+1];   //ta zmienna zostanie powo³ana pod konkretnym adresem w pamieci
volatile int iStr;      //idex do str
volatile unsigned char strValid;  //odebrany komunikat zmienna prawda fa³sz

volatile char StrT[MAX_TRANSMISION_SIZE+1];
volatile int iStrT;
volatile char EndTransmisionFlag;

volatile int counter=0;
//volatile char TempFlag=0;
volatile int speed, speedZad=0, counterOld=0;
volatile int licznik=0;
volatile int U;
volatile unsigned long long Time;

void prepareToSend(char* text);
void send();
void sendWithStop();
void motorMove (int pwm, char type);
void uchartostr (unsigned char liczba);
void inttostr (int liczba);
void uninttostr (unsigned int liczba);

volatile char Buff[7];


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
	DDRB = 0b10110000; //nawet jesli potem to jest pwm i tak trzeba ustawic
	DDRH = 0b01100000;
	PORTB = 0b00100000;
	//76543210
	
	//transmisja asynchroniczna tylko do odbierania
	UCSR0A = 0b00000010;
	UCSR0B = 0b10011000;
	UCSR0C = 0b00000110;
	UBRR0  = 16;
	//
	//USTAWIENIA PWM TIMER1
	//76543210
	TCCR1A = 0b00001001;
	TCCR1B = 0b00001100; //ustawi³em 10 bit rozdzielczosc oraz prescalej /64   i chyba 8 bit 256 chyba
	//tu potem wpisuje - OCR1C
	
	//USTAWIENIA PWM TIMER2A
	//76543210
	TCCR2A = 0b10000011;
	TCCR2B = 0b00000100; //ustawi³em 8 bit rozdzielczosc oraz prescalej /64
	//tu potem wpisuje - OCR2A
	
	//ustawienia countera timer0   czas jaki to odmierza to prescaler*256/16 000 000 sekund
	TCCR0A = 0b00000000;
	TCCR0B = 0b00000100;//prescaler 256
	TIMSK0 = 0b00000001;
	//
	
	//przerwania zewnetrzen na enkoder
	EIMSK = 0b00001100;
	EICRA = 0b01010000;
	
	//bezposrednio przed while w³aczam przerwania
	iStr=0;
	sei();
	int refference;			//wartosc odczytana z transmicji
	char actionFlag=0;		//flaga akcji
	char sendFlag=0;

	motorMove(-50,1);
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
			
			/*if(refference==100)
			{
				prepareToSend("rowny");
				sendWithStop();
				prepareToSend("rowny");
				sendWithStop();
			}*/
			//else if(refference>100)
			//UDR0='>';
			//else if(refference<100)
			//UDR0='<';

			//OCR1C=refference;
			//OCR2A = 100;
			
			speedZad=refference;
			//motorMove(refference,1);
			actionFlag=0;
			//prepareToSend("dostalem");
			//sendWithStop();
			while(counter>50 || counter < -50);
			Time=0;
			sendFlag=1;
		}
		
		if (licznik >= 10 && sendFlag)
		{
			uninttostr((unsigned int) (Time/1000));
			prepareToSend(Buff);
			sendWithStop();
			//prepareToSend(";");
			//sendWithStop();
			inttostr(counter);
			prepareToSend(Buff);
			sendWithStop();
			prepareToSend("\n");
			sendWithStop();
			licznik=0;
			if (Time > 64000000) sendFlag = 0;
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
	EndTransmisionFlag=0;
}

ISR(INT2_vect)
{
	//OCR1C=250;
	if(counter>8400 || counter<-8400) counter%=8400;

	char En;
	En = (PIND & 0b00001100);
	switch (En)
	{
		case 0b00001000:
		{
			counter++;
		}
		break;
		case 0b00000000:
		{
			counter--;
		}
		break;
		case 0b00001100:
		{
			counter--;
		}
		break;
		case 0b00000100:
		{
			counter++;
		}
		break;
	}
}

ISR(INT3_vect)
{
	//OCR1C=250;
	if(counter>8400 || counter<-8400) 	counter%=8400;

	char En;
	En = (PIND & 0b00001100);
	switch (En)
	{
		case 0b00001000:
		{
			counter--;
		}
		break;
		case 0b00000000:
		{
			counter++;
		}
		break;
		case 0b00001100:
		{
			counter++;
		}
		break;
		case 0b00000100:
		{
			counter--;
		}
		break;
	}
}

ISR(TIMER0_OVF_vect)  //przerwanie od przepe³nienia licznika timer0
{
	//if (speedZad)
	{
		//int U;
		if (counterOld > 0 && counter < counterOld) counterOld-=8400;
		if (counterOld < 0 && counter > counterOld) counterOld+=8400;
		speed=(counter-counterOld)/(0.004096);
		counterOld=counter;
		//U = 50 + (speedZad - speed) * 0.1;
		//OCR1C = U;
		//motorMove(U,1);
	}
	licznik++;
	Time+=4096;
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

void sendWithStop()
{
	EndTransmisionFlag=1;
	iStrT=0;
	UCSR0B|=0b00100000;		//w³aczenie przerwania po wyslaniu 1 UDR0
	while(EndTransmisionFlag);
}

void motorMove (int pwm, char type)
{
	char direction;
	unsigned char PWM_VAL;
	//prepareToSend("jesztem");
	//sendWithStop();
	//prepareToSend(uchartostr(pwm));
	//sendWithStop();
	if(pwm<256 && pwm>-256)
	{
		if(pwm>=0)
		{
			direction=0;
			PWM_VAL=pwm;
		}
		else
		{
			direction=1;
			register int temp=(-1 * pwm);
			PWM_VAL=temp;
		}
		//uchartostr(PWM_VAL);
		//prepareToSend(Buff);
		//sendWithStop();
		switch(type)
		{
			case 1:
			{
				if (direction)
				{
					//76543210
					PORTH&=0b10011111;
					PORTH|=0b01000000;
				}
				else
				{
					PORTH&=0b10011111;
					PORTH|=0b00100000;
				}
				OCR2A=PWM_VAL;
				//OCR1C=PWM_VAL;
				break;
			}
		}
	}
}

void uchartostr (unsigned char liczba)
{
	
	Buff[0] = liczba/100 + '0';
	liczba%=100;
	Buff[1] = liczba/10 + '0';
	liczba%=10;
	Buff[2] = liczba + '0';
	Buff[3] = 0;
	
}

void inttostr (int liczba)
{
	if (liczba>=0)
	{
		Buff[0] = liczba/10000 + '0';
		liczba%=10000;
		Buff[1] = liczba/1000 + '0';
		liczba%=1000;
		Buff[2] = liczba/100 + '0';
		liczba%=100;
		Buff[3] = liczba/10 + '0';
		liczba%=10;
		Buff[4] = liczba + '0';
		Buff[5] = 0;
	}
	else
	{
		liczba = -1 * liczba;
		Buff[0] = '-';
		Buff[1] = liczba/10000 + '0';
		liczba%=10000;
		Buff[2] = liczba/1000 + '0';
		liczba%=1000;
		Buff[3] = liczba/100 + '0';
		liczba%=100;
		Buff[4] = liczba/10 + '0';
		liczba%=10;
		Buff[5] = liczba + '0';
		Buff[6] = 0;
	}
	
}

void uninttostr (unsigned int liczba)
{
		Buff[0] = liczba/10000 + '0';
		liczba%=10000;
		Buff[1] = liczba/1000 + '0';
		liczba%=1000;
		Buff[2] = liczba/100 + '0';
		liczba%=100;
		Buff[3] = liczba/10 + '0';
		liczba%=10;
		Buff[4] = liczba + '0';
		Buff[5] = 0;
	
}