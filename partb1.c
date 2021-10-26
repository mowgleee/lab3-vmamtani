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

#include <avr/sfr_defs.h>
#define F_CPU 16000000
#define HIGH 1
#define LOW 0
bool flag=false;

void initialize();

int main(void)
{
    /* Replace with your application code */
	initialize();
    while (1) 
    {
		/*
		if (flag==true)
		{
			PORTD|=(1<<PORTD6);
		}
		if (flag==false)
		{
			PORTD&=~(1<<PORTD6);
		}
		*/
    }
}


void initialize()
{
	//Disable global interrupts
	cli();
	
	//UART_init(BAUD_PRESCALER);
	
	//input output setup
	DDRD |=(1<<DDD6);//OUTPUT PD6
	PORTD|=(1<<PORTD6);//PD6 LOW
	
	//prescale by 256
	
	TCCR0B&=~(1<<CS00);
	TCCR0B&=~(1<<CS01);
	TCCR0B|=(1<<CS02);
	
	//Set Timer 0 to normal
	TCCR0A&=~(1<<WGM00);
	TCCR0A&=~(1<<WGM01);
	TCCR0B&=~(1<<WGM02);
	//clear overflow interrupt flag
	TIFR0|=(1<<TOV0);
	
	//enable overflow interrupt
	TIMSK0 |= (1<<TOIE0);
	
	sei(); //Enable global interrupts
}

ISR(TIMER0_OVF_vect)
{
	//flag=~flag;
	PORTD^=(1<<PORTD6);
}