#ifndef UART_H
#define UART_H

#include <inttypes.h>
#include <stddef.h>

#ifndef F_CPU
#define F_CPU 16000000
#endif
#ifndef BAUD
#define BAUD 9600UL
#endif
#define UBRR_BAUD ((F_CPU/(16UL*BAUD))-1)

void serial_init(void);
void serial_print_char(const char c);
void serial_print(const char* string);
void serial_println(const char* string);
void serial_print_uint8(uint8_t hex);
void serial_print_uint32(uint32_t hex);
void serial_print_byte_array(const uint8_t* array, size_t array_length);

#endif