#ifndef DOVE_MAC_PP_H
#define DOVE_MAC_PP_H

#include <inttypes.h>
#include <stddef.h>

#define MESSAGE_BLOCK_SIZE 16

void dmac_calc_theta(uint8_t *theta, const uint8_t *message,
    size_t message_length);

void dmac_calc_mac(uint8_t *tag, const uint8_t *key_1, const uint8_t *key_2,
    const uint8_t *theta, const uint8_t *message, size_t message_length);

void process_message_block(const uint8_t *t, uint8_t *s, uint8_t *u,
    uint8_t *x, uint8_t *y);

#endif