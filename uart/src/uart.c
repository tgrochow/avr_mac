#include <avr/io.h>
#include <util/setbaud.h>
#include <stdlib.h>

#include "uart.h"

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

void serial_print_uint8(uint8_t hex)
{
  char str_hex[3];
  itoa(hex, str_hex, 16);
  serial_print("0x");
  serial_print(str_hex);
  serial_print_char('\n');
}

void serial_print_uint32(uint32_t hex)
{
  char str_hex[9];
  itoa(hex, str_hex, 16);
  serial_print("0x");
  serial_print(str_hex);
  serial_print_char('\n');
}

// print byte array in hexadecimal encoding
void serial_print_byte_array(const uint8_t* array, size_t array_length)
{
  char str_hex[3];
  size_t array_index;
  serial_print_char('[');
  for (array_index = 0; array_index < array_length; ++array_index)
  {
    itoa(array[array_index], str_hex, 16);
    serial_print("0x");
    serial_print(str_hex);

    if (array_index + 1 < array_length)
    {
      serial_print(", ");
    }
  }
  serial_print_char(']');
  serial_print_char('\n');
}