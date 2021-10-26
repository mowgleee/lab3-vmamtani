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
#include <util/delay.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include "parta.h"

#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)
#include <avr/sfr_defs.h>
#define F_CPU 16000000
#define HIGH 1
#define LOW 0
volatile unsigned long overflowCount=0;
volatile unsigned long startTime=0;
volatile unsigned long stopTime=0;
volatile unsigned long period=0;
int c=0;
char String[25];

void initialize();

int main(void)
{
	/* Replace with your application code */
	int cm=0;
	initialize();
	while (1)
	{
		if(c==60)
		{//60ms have passed. Send Trigger pulse.
			PORTD|=(1<<PORTD7);
			_delay_us(10);
			PORTD&=~(1<<PORTD7);
			c=0;
		}
		if(period!=0)
		{
			
			period=period*4/58;
			cm=(int)period;
			sprintf(String,"%d cm\n\r", cm);
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
		TIFR1&=~(1<<ICF1);
		TCCR1B&=~(1<<ICES1);			//set the ICES bit, so next INT is on rising edge
	}
	else if(bit_is_clear(TCCR1B,ICES1))
	{
		stopTime = (overflowCopy << 16) + timer1CounterValue;
		TIFR1&=~(1<<ICF1);
		TCCR1B |= (1<<ICES1);//set for rising edge
		period=stopTime-startTime;
	}
}


ISR(TIMER1_OVF_vect)
{
	overflowCount++;
}