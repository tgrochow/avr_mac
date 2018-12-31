#ifndef UART_UTIL_H
#define UART_UTIL_H

#define F_CPU 16000000
#define BAUD 2400UL

#include <avr/io.h>
#include <util/setbaud.h>
#include <inttypes.h>
#include <stddef.h>


#define UBRR_BAUD ((F_CPU/(16UL*BAUD))-1)

void uart_init(void)
{
  // configure baudrate
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;

  // activate uart transmitter and receiver
  UCSR0B = (1 << RXEN0) | (1 << TXEN0);

  // configure uart data format 8bit 1 stop bit
  UCSR0C = (1 << UCSZ01) | ( 1 << UCSZ00);
}

void print_char(const char c)
{
  //wait until transmission buffer is ready
  while(!( UCSR0A & (1 << UDRE0)))
  {}
  UDR0 = c;
}

void print(const char* string)
{
  while(*string)
  {
    print_char(*string);
    ++string;
  }

}

void println(const char* string)
{
  print(string);
  print_char('\n');
}


#endif