// system library
#include <crypto_util.h>
#include <skinny64_128_tweaked.h>
#include <string.h>


#include <dovemac_pp.h>

void dmac_calc_theta(uint8_t *theta, const uint8_t *message,
    size_t message_size)
{
  const uint8_t *t;
  uint16_t message_block_number = message_size >> 4;
  memset(theta, 0, SKINNY_BLOCK_SIZE);

  for(uint16_t block_index = 0;
      block_index < message_block_number; ++block_index)
  {
    // calculate start address of current message block
    t = message + (block_index << 4);
    xor_64_compound(theta, t);
  }
}

void dmac_calc_mac(uint8_t *tag, const uint8_t *key_1, const uint8_t *key_2,
    const uint8_t *theta, const uint8_t *message, size_t message_size)
{
  uint16_t complete_message_block_number = message_size >> 4;

  uint8_t u[SKINNY_BLOCK_SIZE];
  uint8_t s[SKINNY_BLOCK_SIZE];
  uint8_t x[SKINNY_BLOCK_SIZE];
  uint8_t y[SKINNY_BLOCK_SIZE];
  const uint8_t *t;

  // init intermediate results
  memset(u, 0, SKINNY_BLOCK_SIZE);
  memset(s, 0, SKINNY_BLOCK_SIZE);
  memset(x, 0, SKINNY_BLOCK_SIZE);
  memset(y, 0, SKINNY_BLOCK_SIZE);

  skinny_set_key(key_1);

  // hashing
  for(uint16_t block_index = 0;
      block_index < complete_message_block_number; ++block_index)
  {
    // calculate start address of current message block
    t = message + (block_index << 4);
    process_message_block(t, s, u, x, y);
  }
  xor_64_compound(x, theta);

  // finalizing
  skinny_set_key(key_2);
  skinny_set_tweak(x);
  skinny_encrypt_block(tag, y);
}

void process_message_block(const uint8_t *t, uint8_t *s, uint8_t *u, uint8_t *x,
    uint8_t *y)
{
  /* calculate start address of the second message block half */
  const uint8_t* i = t + SKINNY_BLOCK_SIZE;
  xor_64(s, y, i);
  xor_64(u, x, t);
  skinny_set_tweak(u);
  skinny_encrypt_block(x, s);
  xor_64_compound(y, x);
}