#ifndef SERIAL_UTIL_H
#define SERIAL_UTIL_H

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

#endif