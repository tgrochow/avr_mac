#ifndef CIPHER_UTIL_H
#define CIPHER_UTIL_H

#include <inttypes.h>

void permute_tkn(uint8_t *tkn);
void inv_permute_tkn(uint8_t *tkn);
void calc_next_rc(uint8_t *rc);
void calc_prev_rc(uint8_t *rc);

#endif