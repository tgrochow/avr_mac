// system library
#include <crypto_util.h>
#include <string.h>
#include <skinny64_128_tweaked.h>


#include <dovemac.h>

void dmac_calc_mac(uint8_t *tag, const uint8_t *key_1, const uint8_t *key_2,
    const uint8_t *message, size_t message_size)
{
  uint8_t message_block_remainder = message_size & 0x0F;
  uint16_t complete_block_byte_number =
      message_size - message_block_remainder;
  uint16_t complete_message_block_number = complete_block_byte_number >> 4;

  uint8_t u[SKINNY_BLOCK_SIZE];
  uint8_t s[SKINNY_BLOCK_SIZE];
  uint8_t x[SKINNY_BLOCK_SIZE];
  uint8_t y[SKINNY_BLOCK_SIZE];
  uint8_t theta[SKINNY_BLOCK_SIZE];
  const uint8_t *t;

  // init intermediate results
  memset(u, 0, SKINNY_BLOCK_SIZE);
  memset(s, 0, SKINNY_BLOCK_SIZE);
  memset(x, 0, SKINNY_BLOCK_SIZE);
  memset(y, 0, SKINNY_BLOCK_SIZE);
  memset(theta, 0, SKINNY_BLOCK_SIZE);

  skinny_set_key(key_1);

  // hashing
  for(uint16_t block_index = 0;
      block_index < complete_message_block_number; ++block_index)
  {
    // calculate start address of current message block
    t = message + (block_index << 4);
    process_message_block(t, s, u, x, y, theta);
  }
  // process padded message block
  if(message_block_remainder)
  {
    uint8_t padded_message_block[MESSAGE_BLOCK_SIZE];
    append_bit_padding(padded_message_block, complete_block_byte_number,
        message_block_remainder, MESSAGE_BLOCK_SIZE, message);
    process_message_block(padded_message_block, s, u, x, y, theta);
  }
  xor_64_compound(x, theta);

  // finalizing
  skinny_set_key(key_2);
  skinny_set_tweak(x);
  skinny_encrypt_block(tag, y);
}

void process_message_block(const uint8_t *t, uint8_t *s, uint8_t *u, uint8_t *x,
    uint8_t *y, uint8_t* theta)
{
  /* calculate start address of the second message block half */
  const uint8_t* i = t + SKINNY_BLOCK_SIZE;
  xor_64_compound(theta, t);
  xor_64(s, y, i);
  xor_64(u, x, t);
  skinny_set_tweak(u);
  skinny_encrypt_block(x, s);
  xor_64_compound(y, x);
}