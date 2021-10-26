/*
 * partb2.c
 *
 * Created: 20-Oct-21 8:46:50 PM
 *  Author: varun
 */ 
/*
 * Lab3.c
 *
 * Created: 17-Oct-21 6:03:25 AM
 * Author : varun
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

int high_time=70;
int low_time=70;
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
	
	//Set Timer 0 to normal
	TCCR0A&=~(1<<WGM00);
	TCCR0A&=~(1<<WGM01);
	TCCR0B&=~(1<<WGM02);
	
	//prescale by 256
	
	TCCR0B&=~(1<<CS00);
	TCCR0B&=~(1<<CS01);
	TCCR0B|=(1<<CS02);
	
	//clear COMPA interrupt flag
	TIFR0|=(1<<OCF0A);
	
	OCR0A=20;
	
	//enable COMPA interrupt
	TIMSK0 |= (1<<OCIE0A);
	
	sei(); //Enable global interrupts
}

ISR(TIMER0_COMPA_vect)
{
	PORTD^=(1<<PORTD6);
	TCNT0=0;
}