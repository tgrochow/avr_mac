#ifndef CIPHER_UTIL_H
#define CIPHER_UTIL_H

#include <inttypes.h>

#define SKINNY_BLOCK_SIZE 8

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

struct skinny64_128_tweaked_state
{
  uint8_t round_key[4];
  uint8_t rc;
  uint8_t tk1[SKINNY_BLOCK_SIZE];
  uint8_t tk2[SKINNY_BLOCK_SIZE];
  const uint8_t *key;
  const uint8_t *tweak;
};

#endif
