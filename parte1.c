/*
 * parte1.c
 *
 * Created: 25-Oct-21 2:35:12 AM
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

char String[25];
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)
#include <avr/sfr_defs.h>
#define HIGH 1
#define LOW 0

void Initialize();

int main(void)
{
	Initialize();
	int d=0;
	while(1)
	{
		volatile int ad=ADCW;
		//sprintf(String,"%d\n\r",ad);
		//UART_putstring(String);
		if (ad>=4&&ad<=100)
		{
			d=5;
			sprintf(String,"ADC: %d, Duty Cycle: 5%\n\r",ad);
		}
		else if(ad>100&&ad<201)
		{
			d=10;
			sprintf(String,"ADC: %d, Duty Cycle: 10%\n\r",ad);
		}
		else if(ad>200&&ad<301)
		{
			d=15;
			sprintf(String,"ADC: %d, Duty Cycle: 15%\n\r",ad);
		}
		else if(ad>300&&ad<401)
		{
			d=20;
			sprintf(String,"ADC: %d, Duty Cycle: 20%\n\r",ad);
		}
		else if(ad>400&&ad<501)
		{
			d=25;
			sprintf(String,"ADC: %d, Duty Cycle: 25%\n\r",ad);
		}
		else if(ad>500&&ad<601)
		{
			d=30;
			sprintf(String,"ADC: %d, Duty Cycle: 30%\n\r",ad);
		}
		else if(ad>600&&ad<701)
		{
			d=35;
			sprintf(String,"ADC: %d, Duty Cycle: 35%\n\r",ad);
		}
		else if(ad>700&&ad<801)
		{
			d=40;
			sprintf(String,"ADC: %d, Duty Cycle: 40%\n\r",ad);
		}
		else if(ad>800&&ad<901)
		{
			d=45;
			sprintf(String,"ADC: %d, Duty Cycle: 45%\n\r",ad);
		}
		else if(ad>900&&ad<=1024)
		{
			d=50;
			sprintf(String,"ADC: %d, Duty Cycle: 50%\n\r",ad);
		}
		UART_putstring(String);
	}
}

void Initialize()
{
	UART_init(BAUD_PRESCALER);
	//ADC setup
	//clear power reduction for ADC
	PRR&=~(1<<PRADC);
	//select Vref=AVcc
	ADMUX|=(1<<REFS0);
	ADMUX&=~(1<<REFS1);
	//set ADC clk to 128; i.e., 125K
	ADCSRA|=(1<<ADPS0);
	ADCSRA|=(1<<ADPS1);
	ADCSRA|=(1<<ADPS2);
	//set channel 0
	ADMUX&=~(1<<MUX0);
	ADMUX&=~(1<<MUX1);
	ADMUX&=~(1<<MUX2);
	ADMUX&=~(1<<MUX3);
	//set to free running
	ADCSRA|=(1<<ADATE);
	ADCSRB&=~(1<<ADTS0);
	ADCSRB&=~(1<<ADTS1);
	ADCSRB&=~(1<<ADTS2);
	//disable digital input
	DIDR0|=(1<<ADC0D);
	//enable adc
	ADCSRA|=(1<<ADEN);
	//start conv
	ADCSRA|=(1<<ADSC);
}

