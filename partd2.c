/*
 * partd2.c
 *
 * Created: 25-Oct-21 1:22:17 AM
 *  Author: varun
 */ 

/*
 * partd1.c
 *
 * Created: 25-Oct-21 12:22:38 AM
 *  Author: varun
 */ 

/*
 * partc.c
 *
 * Created: 24-Oct-21 7:26:30 PM
 *  Author: varun
 */ 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>

#define F_CPU 16000000
#include <util/delay.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include "parta.h"

#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)
#include <avr/sfr_defs.h>
#define HIGH 1
#define LOW 0
volatile unsigned long overflowCount=0;
volatile unsigned long startTime=0;
volatile unsigned long stopTime=0;
volatile unsigned long period=0;
int c=0;
char String[25];
volatile int high_time=70;
int low_time=70;
volatile int high_or_low=1;

void initialize();

int main(void)
{
	/* Replace with your application code */
	int cm=0;
	int ocr=0;
	initialize();
	//sprintf(String,"Initialized\n\r");
	//UART_putstring("hello!");
	while (1)
	{
		if(c==60)
		{//60ms have passed. Send Trigger pulse.
			PORTD|=(1<<PORTD7);
			_delay_us(10);
			PORTD&=~(1<<PORTD7);
			c=0;
			//UART_putstring("yo\n\r");
		}
		if(period!=0)
		{
			
			period=period*4/58;
			cm=(int)period;
			//ocr=(int)cm*0.62+56;
			if (cm>0 && cm<15)
			OCR0A=59;
			else if (cm>=15 && cm<25)
			OCR0A=62;
			else if (cm>=25 && cm<35)
			OCR0A=70;
			else if (cm>=35 && cm<45)
			OCR0A=79;
			else if (cm>=45 && cm<55)
			OCR0A=88;
			else if (cm>=55 && cm<70)
			OCR0A=94;
			else if (cm>=70 && cm<85)
			OCR0A=105;
			else if (cm>85)
			OCR0A=118;
			
			//TIFR0|=(1<<OCF0A);
			sprintf(String,"%d cm\n\r", ocr);
			UART_putstring(String);
			period=0;
		}
		_delay_ms(1);
		c++;
	}
}


void initialize()
{
	//Disable global interrupts
	cli();
	
	UART_init(BAUD_PRESCALER);
	
	//input output setup
	DDRB &=~(1<<DDB0);//INPUT echo pin PB0 or 8
	
	DDRD|=(1<<DDD7);//OUTPUT trig pin PD7 or 7
	
	//prescale by 64
	TCCR1B|=(1<<CS10);
	TCCR1B|=(1<<CS11);
	TCCR1B&=~(1<<CS12);
		
	//Set Timer 1 to normal
	TCCR1A&=~(1<<WGM10);
	TCCR1A&=~(1<<WGM11);
	TCCR1B&=~(1<<WGM12);
	TCCR1B&=~(1<<WGM13);
	
	//for rising edge of echo pin
	TCCR1B |=(1<<ICES1);
	
	//clear interrupt flag
	TIFR1|=(1<<ICF1);
	
	//enable overflow interrupt
	TIMSK1 |= (1<<TOIE1);
	
	//enable input capture interrupt
	TIMSK1 |= (1<<ICIE1);
	
	
		//input output setup
		DDRD |=(1<<DDD6);//OUTPUT PD6(OC0A)
		//PORTD&=~(1<<PORTD6);//PD6 LOW
		
		//Set Timer 0 to Phase correct PWM
		TCCR0A|=(1<<WGM00);
		TCCR0A&=~(1<<WGM01);
		TCCR0B|=(1<<WGM02);
		
		//prescale by 64
		
		TCCR0B|=(1<<CS00);
		TCCR0B|=(1<<CS01);
		TCCR0B&=~(1<<CS02);
		
		TCCR0A&=~(1<<COM0A1);//Toggle OC0A on compare match
		TCCR0A|=(1<<COM0A0);
		
		//clear COMPA interrupt flag
		TIFR0|=(1<<OCF0A);
		
		OCR0A=high_time;//70
		
		//enable COMPA interrupt
		//TIMSK0 |= (1<<OCIE0A);
	
	sei(); //Enable global interrupts
}



ISR(TIMER1_CAPT_vect)
{
	unsigned int timer1CounterValue;
	unsigned long overflowCopy = overflowCount;
	timer1CounterValue = ICR1;
	if (bit_is_set(TCCR1B,ICES1))
	{ 	// run this if ICES1 is clear, ISR was triggered by rising edge
		startTime = (overflowCopy << 16) + timer1CounterValue;
		TIFR1|=(1<<ICF1);
		TCCR1B&=~(1<<ICES1);			//set the ICES bit, so next INT is on falling edge
		//sprintf(String,"rising\n\r");
		//UART_putstring(String);
	}
	else if(bit_is_clear(TCCR1B,ICES1))
	{
		stopTime = (overflowCopy << 16) + timer1CounterValue;
		TIFR1|=(1<<ICF1);
		TCCR1B |= (1<<ICES1);//set for rising edge
		//sprintf(String,"falling\n\r");
		//UART_putstring(String);
		period=stopTime-startTime;
	}
}


ISR(TIMER1_OVF_vect)
{
	overflowCount++;
}