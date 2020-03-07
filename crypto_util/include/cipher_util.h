#ifndef CIPHER_UTIL_H
#define CIPHER_UTIL_H

#include <inttypes.h>

void permute_tkn(uint8_t *tkn);
void inv_permute_tkn(uint8_t *tkn);
void calc_next_rc(uint8_t *rc);
void calc_prev_rc(uint8_t *rc);
void lfsr2(uint8_t *t);
void inv_lfsr2(uint8_t *t);
void skinny_generate_next_key(uint8_t *round_key, uint8_t *round_constant,
    uint8_t *tk1, uint8_t *tk2);
void skinny_set_tk1(uint8_t *round_key, uint8_t *round_constant, uint8_t *tk1,
    uint8_t *tk2);
void skinny_set_tk2(uint8_t *round_key, uint8_t *tk2);

#endif
