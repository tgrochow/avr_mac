// system library
#include <string.h>

// project library
#include <mac_util.h>
#include <skinny64_128_tweaked.h>


#include <zmac.h>

void zmac_calc_mac(uint8_t *tag, const uint8_t *key_1, const uint8_t *key_2,
    const uint8_t *message, size_t message_size)
{
  uint8_t message_block_remainder = message_size & 0x0F;
  uint16_t complete_block_byte_number =
      message_size - message_block_remainder;
  uint16_t complete_message_block_number = complete_block_byte_number >> 4;

  uint8_t init_input[SKINNY_BLOCK_SIZE];
  uint8_t init_tweak[SKINNY_BLOCK_SIZE];
  uint8_t ll[SKINNY_BLOCK_SIZE];
  uint8_t lr[SKINNY_BLOCK_SIZE];
  uint8_t sl[SKINNY_BLOCK_SIZE];
  uint8_t sr[SKINNY_BLOCK_SIZE];
  uint8_t cl[SKINNY_BLOCK_SIZE];
  uint8_t cr[SKINNY_BLOCK_SIZE];
  uint8_t u[SKINNY_BLOCK_SIZE];
  uint8_t v[SKINNY_BLOCK_SIZE];
  const uint8_t *xr;

  // init intermediate results
  memset(sl, 0x00, SKINNY_BLOCK_SIZE);
  memset(sr, 0x00, SKINNY_BLOCK_SIZE);
  memset(cl, 0x00, SKINNY_BLOCK_SIZE);
  memset(cr, 0x00, SKINNY_BLOCK_SIZE);
  memset(u, 0x00, SKINNY_BLOCK_SIZE);
  memset(v, 0x00, SKINNY_BLOCK_SIZE);

  skinny_set_key(key_1);

  // init ll, lr
  memset(init_input, 0x00, SKINNY_BLOCK_SIZE);
  memset(init_tweak, 0x00, SKINNY_BLOCK_SIZE);
  skinny_set_tweak(init_tweak);
  skinny_encrypt_block(ll, init_input);
  init_tweak[0] = 0x01;
  skinny_set_tweak(init_tweak);
  skinny_encrypt_block(lr, init_input);

  // hashing
  for(uint16_t block_index = 0;
      block_index < complete_message_block_number; ++block_index)
  {
    // calculate start address of current message block
    xr = message + (block_index << 4);
    zmac_process_message_block(xr, ll, lr, sl, sr, cl, cr, u, v);
  }
  // process padded message block
  if(message_block_remainder)
  {
    uint8_t padded_message_block[MESSAGE_BLOCK_SIZE];
    append_bit_padding(padded_message_block, complete_block_byte_number,
        message_block_remainder, MESSAGE_BLOCK_SIZE, message);
    zmac_process_message_block(
        padded_message_block, ll, lr, sl, sr, cl, cr, u,v);
  }

  // finalizing
  skinny_set_key(key_2);
  skinny_set_tweak(u);
  skinny_encrypt_block(tag, v);
}

void zmac_process_message_block(const uint8_t *xr, uint8_t *ll, uint8_t *lr,
    uint8_t *sl, uint8_t *sr, uint8_t *cl, uint8_t *cr, uint8_t *u, uint8_t *v)
{
  /* calculate start address of the second message block half */
  const uint8_t *xl = xr + SKINNY_BLOCK_SIZE;

  xor_64(sl, xl, ll);
  xor_64(sr, xr, lr);
  skinny_set_tweak(sr);
  skinny_encrypt_block(cl, sl);
  xor_64(cr, cl, xr);

  xor_64_compound(u, cl);
  mult2_64(u);
  xor_64_compound(v, cr);

  mult2_64(ll);
  mult2_64(lr);
}