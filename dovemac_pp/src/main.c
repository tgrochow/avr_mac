// system library
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>

// project library
#include <dovemac_pp.h>
#include <stack_paint.h>
#include <uart.h>

// predefined constants
#include <predefined_messages/message_128.h>

/* calculated macs
64:   {0x30, 0xf5, 0xb7, 0x24, 0xab, 0x57, 0x9b, 0x67}
128:  {0xa0, 0x9d, 0x00, 0xbb, 0xaa, 0x06, 0xe9, 0xc6}
256:  {0x4f, 0x39, 0x21, 0x23, 0x87, 0x58, 0x3c, 0x82}
512:  {0xb2, 0xa6, 0x55, 0x47, 0x6c, 0xc2, 0xda, 0x6e}
1024: {0xfe, 0x92, 0xda, 0xa9, 0x3c, 0x76, 0x77, 0x34}
2048: {0x5a, 0x3d, 0x3a, 0x2e, 0x1f, 0x3b, 0xf3, 0xb1}
4096: {0x01, 0x19, 0x22, 0xe4, 0x52, 0xc1, 0xda, 0xa2} */

int main(void)
{
  const uint8_t key_1[8] = {0xf5, 0x26, 0x98, 0x26, 0xfc, 0x68, 0x12, 0x38};
  const uint8_t key_2[8] = {0x9e, 0xb9, 0xd6, 0x40, 0xd0, 0x88, 0xda, 0x63};
  uint8_t mac[8];
  uint8_t theta[8];

  _delay_ms(1000);
  serial_init();
  _delay_ms(1000);

  memset(mac, 0, sizeof(mac));
  dmac_calc_theta(theta, message, sizeof(message));
  dmac_calc_mac(mac, key_1, key_2, theta, message, sizeof(message));
  serial_print_byte_array(mac, sizeof(mac));

  // extract the unused stack bytes
  uint16_t stack_count_int = stack_count();
  char stack_count_string[6];
  utoa(stack_count_int, stack_count_string, 10);
  serial_println(stack_count_string);

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