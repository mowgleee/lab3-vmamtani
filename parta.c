#include "parta.h"
#include <avr/io.h>

void UART_init(int BAUD_PRESCALER)
{
  UBRR0H = (unsigned char)(BAUD_PRESCALER>>8);
  UBRR0L = (unsigned char)BAUD_PRESCALER;
  
  UCSR0B = (1<<RXEN0)|(1<<TXEN0);
  // 2 stop bits, 8 data bits
  UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); //8 data bits
  UCSR0C |= (1<<USBS0); //2 stop bits
}

void UART_send(unsigned char data)
{
  while(!(UCSR0A & (1<<UDRE0)));//while transmit buffer not empty-wait
  UDR0 = data;// Put data into buffer and send it
}

void UART_putstring(char* StrPtr)
{
  while(*StrPtr != 0x00)
  {
    UART_send(*StrPtr);
	StrPtr++;
  }
}
