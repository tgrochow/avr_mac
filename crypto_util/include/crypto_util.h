#ifndef CRYPTO_UTIL_H
#define CRYPTO_UTIL_H

#include <inttypes.h>

void xor_64_compound(uint8_t *x1, const uint8_t *x2);
void xor_64(uint8_t *x, const uint8_t *x1, const uint8_t *x2);
void mult2_64(uint8_t *a);
uint8_t select(uint8_t a, uint8_t b, int8_t bit);
void append_bit_padding(uint8_t *padded_message_block,
    uint16_t complete_block_byte_number, uint8_t message_block_remainder,
    uint8_t message_block_size, const uint8_t *message);

#endif