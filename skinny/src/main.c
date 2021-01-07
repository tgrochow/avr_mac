// system library
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

// project library
#include <skinny64_128_tweaked.h>
#include <uart.h>

struct test_vector
{
  uint8_t key[8];
  uint8_t tweak[8];
  uint8_t plaintext[8];
  uint8_t ciphertext[8];
};

static struct test_vector const tv =
{
  {0x9e, 0xb9, 0x36, 0x40, 0xd0, 0x88, 0xda, 0x63},
  {0x76, 0xa3, 0x9d, 0x1c, 0x8b, 0xea, 0x71, 0xe1},
  {0xcf, 0x16, 0xcf, 0xe8, 0xfd, 0x0f, 0x98, 0xaa},
  {0x15, 0xfa, 0x68, 0x61, 0x8f, 0xdf, 0x84, 0x58}
};

int main(void)
{
  _delay_ms(1000);
  serial_init();
  _delay_ms(1000);

  uint8_t buffer[8];
  skinny_set_key(tv.key);
  skinny_set_tweak(tv.tweak);
  skinny_encrypt_block(buffer, tv.plaintext);
  serial_print_byte_array(buffer, 8);

  if (memcmp(buffer, tv.ciphertext, sizeof(buffer)) == 0)
  {
    serial_println("skinny64_128_tweaked encryption: sucessfull");
  }
  else
  {
    serial_println("skinny64_128_tweaked encryption: failed");
  }

  skinny_decrypt_block(buffer, buffer);
  if (memcmp(buffer, tv.plaintext, sizeof(buffer)) == 0)
  {
    serial_println("skinny64_128_tweaked decryption: sucessfull");
  }
  else
  {
    serial_println("skinny64_128_tweaked decryption: failed");
  }

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