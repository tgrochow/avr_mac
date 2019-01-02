#ifndef SERIAL_UTIL_H
#define SERIAL_UTIL_H

#ifndef F_CPU
#define F_CPU 16000000
#endif
#ifndef BAUD
#define BAUD 9600UL
#endif
#define UBRR_BAUD ((F_CPU/(16UL*BAUD))-1)

#include <avr/io.h>
#include <util/setbaud.h>
#include <inttypes.h>
#include <stddef.h>

void serial_init(void)
{
  // configure baudrate
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;

  // activate uart transmitter and receiver
  UCSR0B = (1 << RXEN0) | (1 << TXEN0);

  // configure uart data format 8bit 1 stop bit
  UCSR0C = (1 << UCSZ01) | ( 1 << UCSZ00);
}

void serial_print_char(const char c)
{
  //wait until transmission buffer is ready
  while(!( UCSR0A & (1 << UDRE0)))
  {}
  UDR0 = c;
}

void serial_print(const char* string)
{
  while(*string)
  {
    serial_print_char(*string);
    ++string;
  }

}

void serial_println(const char* string)
{
  serial_print(string);
  serial_print_char('\n');
}


#endif