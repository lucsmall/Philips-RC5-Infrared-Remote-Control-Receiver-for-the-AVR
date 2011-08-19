#include "main.h"
#include "serial.h"


void serial_init(unsigned int baud) 
{
	// set baud rate
	UBRRH = (uint8_t)(UART_BAUD_CALC(baud,F_CPU)>>8);
	UBRRL = (uint8_t)UART_BAUD_CALC(baud,F_CPU);
	// Enable receiver and transmitter and receive interrupt; 
	UCSRB = (1 << RXEN) | (1 << TXEN) | (1 << RXCIE);
	//asynchronous 8N1
	UCSRC = 0x06; //(1 << USBS0) | (3 << UCSZ00);
}

void serial_putc(unsigned char c) 
{
   // wait until UDR ready
	while(!(UCSRA & (1 << UDRE)))
    ;
	UDR = c;    // send character
}

void serial_puts (char *s) 
{
  while (*s) {
		serial_putc(*s);
		s++;
	}
}
