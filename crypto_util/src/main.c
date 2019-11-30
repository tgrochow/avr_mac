// system library
#include <avr/io.h>
#include <string.h>
#include <util/delay.h>

// project library
#include <cipher_util.h>
#include <uart.h>

#define TKN_SIZE 8
#define ROUND_NUMBER 36

void test_tkn_permutation()
{
  uint8_t tkn[TKN_SIZE] = {0x9e, 0xb9, 0x36, 0x40, 0xd0, 0x88, 0xda, 0x63};
  uint8_t tkn_copy[TKN_SIZE];
  uint8_t tkn_permutation_correct = 1;

  memcpy(tkn_copy, tkn, TKN_SIZE);
  permute_tkn(tkn);
  if (memcmp(tkn, tkn_copy, sizeof(tkn)) == 0)
  {
    tkn_permutation_correct = 0;
  }
  else
  {
    inv_permute_tkn(tkn);
    if (memcmp(tkn, tkn_copy, sizeof(tkn)) != 0)
    {
      tkn_permutation_correct = 0;
    }
  }

  const char * test_message = "tkn permutation";
  serial_print_test_result(test_message, tkn_permutation_correct);
}

void test_round_constant_calculation()
{
  uint8_t rc_tv[ROUND_NUMBER] =
  {
    0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3e, 0x3d, 0x3b, 0x37, 0x2f, 0x1e, 0x3c,
    0x39, 0x33, 0x27, 0x0e, 0x1d, 0x3a, 0x35, 0x2b, 0x16, 0x2c, 0x18, 0x30,
    0x21, 0x02, 0x05, 0x0b, 0x17, 0x2e, 0x1c, 0x38, 0x31, 0x23, 0x06, 0x0d
  };

  uint8_t rc = 0;
  uint8_t rc_correct = 1;
  uint8_t rc_index;

  for (rc_index = 0; rc_index < ROUND_NUMBER; rc_index += 1)
  {
    calc_next_rc(&rc);
    if (rc != rc_tv[rc_index])
    {
      rc_correct = 0;
      break;
    }
  }

  const char * test_message_1 = "calculate next round constant";
  serial_print_test_result(test_message_1, rc_correct);

  rc_correct = 1;
  for (rc_index = ROUND_NUMBER - 1; rc_index > 0; rc_index = rc_index - 1)
  {
    if (rc_index != ROUND_NUMBER - 1)
    {
      calc_prev_rc(&rc);
    }
    if (rc != rc_tv[rc_index])
    {
      rc_correct = 0;
      break;
    }
  }

  const char * test_message_2 = "calculate previous round constant";
  serial_print_test_result(test_message_2, rc_correct);
}

int main(void)
{
  _delay_ms(1000);
  serial_init();
  _delay_ms(1000);

  // run tests
  serial_print_line_break();
  serial_println("test results:");
  serial_print_line_break();
  test_tkn_permutation();
  test_round_constant_calculation();
  serial_print_line_break();

  // blink
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