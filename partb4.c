/*
 * partb4.c
 *
 * Created: 24-Oct-21 7:08:25 PM
 *  Author: varun
 */ 

/*
 * main.c
 *
 * Created: 20-Oct-21 10:38:22 PM
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
bool flag=false;

int high_time=35;
int low_time=35;
volatile int high_or_low=1;

void initialize();

int main(void)
{
	/* Replace with your application code */
	initialize();
	while (1)
	{
	}
}


void initialize()
{
	//Disable global interrupts
	cli();
	
	UART_init(BAUD_PRESCALER);
	
	//input output setup
	DDRD |=(1<<DDD6);//OUTPUT PD6(OC0A)
	//PORTD&=~(1<<PORTD6);//PD6 LOW
	
	//Set Timer 0 to Phase correct PWM
	TCCR0A|=(1<<WGM00);
	TCCR0A&=~(1<<WGM01);
	TCCR0B|=(1<<WGM02);
	
	//prescale by 256
	
	TCCR0B&=~(1<<CS00);
	TCCR0B&=~(1<<CS01);
	TCCR0B|=(1<<CS02);
	
	TCCR0A&=~(1<<COM0A1);//Toggle OC0A on compare match
	TCCR0A|=(1<<COM0A0);
	
	//clear COMPA interrupt flag
	TIFR0|=(1<<OCF0A);
	
	OCR0A=high_time;//70
	
	//enable COMPA interrupt
	//TIMSK0 |= (1<<OCIE0A);
	
	sei(); //Enable global interrupts
}