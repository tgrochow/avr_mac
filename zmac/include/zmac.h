#ifndef ZMAC_H
#define ZMAC_H

#include <inttypes.h>
#include <stddef.h>

#define MESSAGE_BLOCK_SIZE 16

void zmac_calc_mac(uint8_t *tag, const uint8_t *key_1, const uint8_t *key_2,
    const uint8_t *message, size_t message_size);

void zmac_process_message_block(const uint8_t *xr, uint8_t *ll, uint8_t *lr,
    uint8_t *sl, uint8_t *sr, uint8_t *cl, uint8_t *cr, uint8_t *u, uint8_t *v);

#endif