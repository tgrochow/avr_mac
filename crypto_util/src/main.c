// system library
#include <avr/io.h>
#include <util/delay.h>

// project library
#include <crypto_util.h>
#include <uart.h>

int main(void)
{
  uint8_t tkn[8] = {0x9e, 0xb9, 0x36, 0x40, 0xd0, 0x88, 0xda, 0x63};

  _delay_ms(1000);
  serial_init();
  _delay_ms(1000);

  serial_print_byte_array(tkn, 8);
  permute_tkn(tkn);
  serial_print_byte_array(tkn, 8);
  inv_permute_tkn(tkn);
  serial_print_byte_array(tkn, 8);

  DDRB |= (1 << PB7);

  while(1)
  {
    PORTB |= (1 << PB7);
    _delay_ms(3000);

    PORTB &= ~(1 << PB7);
    _delay_ms(3000);
  }

  return 0;
}