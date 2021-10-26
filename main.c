/*
 * partf.c
 *
 * Created: 25-Oct-21 3:43:36 AM
 *  Author: varun
 */ 

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

char String[30];
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)
#include <avr/sfr_defs.h>
#define HIGH 1
#define LOW 0

volatile int high_time=70;
volatile unsigned long overflowCount=0;
volatile unsigned long startTime=0;
volatile unsigned long stopTime=0;
volatile unsigned long period=0;
void Initialize();
bool flag=false;
int pinf=0;

bool debounce()
{
	if (~(PIND & (1<<PIND2)))
	{
		_delay_ms(30);
		if (~(PIND & (1<<PIND2)))
		{
			return 1;
		}
	}
	return 0;
}


int main(void)
{
	Initialize();
	int d=0;
	int ocb=0;
	unsigned int ad;
	int cm=0;
	int ocr=0;
	int c=0;
	//bool buttonState=HIGH;
	//bool bflag=LOW;
	int l=0;
	while(1)
	{
		if((l==2 && (c%2==0)) || (l!=2 && c==60))
		{//60ms have passed. Send Trigger pulse.
			PORTD|=(1<<PORTD7);
			_delay_us(10);
			PORTD&=~(1<<PORTD7);
			c=0;
			l=0;
		}
		_delay_ms(1);
		c++;
		ad=ADCW;
		//sprintf(String,"%d\n\r",ad);
		//UART_putstring(String);
		if (ad>=4&&ad<=100)
		{
			d=5;
		}
		else if(ad>100&&ad<201)
		{
			d=10;
		}
		else if(ad>200&&ad<301)
		{
			d=15;
		}
		else if(ad>300&&ad<401)
		{
			d=20;
		}
		else if(ad>400&&ad<501)
		{
			d=25;
		}
		else if(ad>500&&ad<601)
		{
			d=30;
		}
		else if(ad>600&&ad<701)
		{
			d=35;
		}
		else if(ad>700&&ad<801)
		{
			d=40;
		}
		else if(ad>800&&ad<901)
		{
			d=45;
		}
		else if(ad>900&&ad<=1024)
		{
			d=50;
		}
		
		if((PIND & (1<<PIND2)))
		{
			
			//flag=~flag;
			flag=false;
			UART_putstring("press not detected\n\r");
			
		}
		else{
			if (debounce())
			{
				UART_putstring("press detected\n\r");
				l=2;
				flag=true;
			}
		}
		if(period!=0 && flag==true)//continuous mode;
		{
			period=period*4/58;
			cm=(int)period;
			ocr=(int)cm*0.62+56;
			OCR0A=ocr;
			ocb=d*ocr/100;
			OCR0B=ocb;
			//TIFR0|=(1<<OCF0A);
			//sprintf(String,"%d cm\n\r", ocr);
			UART_putstring("continuous mode\n\r");
			period=0;
		}		
		if(period!=0 && flag==false)//discrete mode
		{			
			period=period*4/58;
			cm=(int)period;
			period=0;
			sprintf(String,"%d cm\n",cm);
			UART_putstring(String);
			if (cm>0 && cm<15)
			{
				ocr=59;
			}
			else if (cm>=15 && cm<25)
			{
				ocr=62;
			}
			else if (cm>=25 && cm<35)
			{
				ocr=70;
			}
			else if (cm>=35 && cm<45)
			{
				ocr=79;
			}
			else if (cm>=45 && cm<55)
			{
				ocr=88;
			}
			else if (cm>=55 && cm<70)
			{
				ocr=94;
			}
			else if (cm>=70 && cm<85)
			{
				ocr=105;
			}
			else if (cm>85)
			{
				ocr=118;
			}
			OCR0A=ocr;
			ocb=d*ocr/100;
			OCR0B=ocb;
			//sprintf(String,"%d cm\n\r", ocr);
			sprintf(String,"%d ocr value\n\r",ocr);
			UART_putstring(String);
			UART_putstring("Discrete mode\n\r");
			
		}
		
	}
}

void Initialize()
{
	UART_init(BAUD_PRESCALER);
	
	//button setup
	DDRD &= ~(1<<DDD2);//input button Digital pin 2
	//PCICR|=(1<<PCIE2);
	//PCMSK2|=(1<<PCINT18);
	
	PORTD|=(1<<PORTD2);//input pullup
	
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
	
	//Disable global interrupts
	cli();
	
	
	//timer1 setup for Ultrasonic sensor
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
	
	
	//timer0 setup for buzzer pwm
	//input output setup
	DDRD |=(1<<DDD5);//OUTPUT PD5(OC0A)
	//PORTD&=~(1<<PORTD5);//PD5 LOW
			
	//Set Timer 0 to Phase correct PWM
	TCCR0A|=(1<<WGM00);
	TCCR0A&=~(1<<WGM01);
	TCCR0B|=(1<<WGM02);
			
	//prescale by 64
		
	TCCR0B|=(1<<CS00);
	TCCR0B|=(1<<CS01);
	TCCR0B&=~(1<<CS02);
		
	TCCR0A|=(1<<COM0B1);//Toggle OC0B on compare match
	TCCR0A&=~(1<<COM0B0);
		
	//clear COMPB interrupt flag
	TIFR0|=(1<<OCF0B);
			
	OCR0A=high_time;
	OCR0B=0.5*OCR0A;
			
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
		period=stopTime-startTime;
	}
}


ISR(TIMER1_OVF_vect)
{
	overflowCount++;
}

/*
ISR(PCINT1_vect)
{
	pinf++;
	flag=~flag;
	sprintf(String,"%d\n\r", pinf);
	UART_putstring(String);
	
	if (pinf%2==0)
	{
		//change mode
		UART_putstring("button pressed");
		flag=~flag;
	}
	
}
*/