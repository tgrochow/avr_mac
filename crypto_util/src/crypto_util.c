#include <crypto_util.h>

void xor_64_compound(uint8_t *x1, const uint8_t *x2)
{
  *((uint64_t*) x1) ^= *((const uint64_t*) x2);
}

void xor_64(uint8_t *x, const uint8_t *x1, const uint8_t *x2)
{
  *((uint64_t*) x) = *((const uint64_t*) x1) ^ *((const uint64_t*) x2);
}

void mult2_64(uint8_t *a)
{
  // calculate the most significant bit (msb)
  int8_t msb = a[7] >> 7;

  // shift 1 bit to the right to perform multiplication by two
  *((uint64_t*) a) <<= 1;

  // if msb was 1 xor value with 0x1b
  a[0] ^= select(0x00, 0x1b, msb);
}

// return a or b dependent on bit
// if bit equals 0 return a else if bit equals 1 return b
// for this function to work as expected bit must be exactly 0x00 or 0x01
uint8_t select(uint8_t a, uint8_t b, int8_t bit)
{
  uint8_t mask = -bit;
  uint8_t result = mask & (a ^ b);

  return result ^ a;
}

// Creates a padded message block with the last bytes of the message
void append_bit_padding(uint8_t *padded_message_block,
    uint16_t complete_block_byte_number, uint8_t message_block_remainder,
    uint8_t message_block_size, const uint8_t *message)
{
  // address of the first byte of the padded message block
  const uint8_t *first_byte_address = message + complete_block_byte_number;

  // byte index of padded message block
  uint8_t byte_index;

  // copy the bytes of the incomplete message block
  for(byte_index = 0; byte_index < message_block_remainder; ++byte_index)
  {
    padded_message_block[byte_index] = *(first_byte_address + byte_index);
  }

  // append bit padding
  padded_message_block[byte_index] = 0x80;
  for(byte_index += 1; byte_index < message_block_size; ++byte_index)
  {
    padded_message_block[byte_index] = 0;
  }
}