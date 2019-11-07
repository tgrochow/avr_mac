/*
 * Copyright (C) 2017 Southern Storm Software, Pty Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef SKINNY64_128_TWEAKED_H
#define SKINNY64_128_TWEAKED_H

#define SKINNY_ROUND_NUMBER 36
#define SKINNY_BLOCK_SIZE 8

#include <inttypes.h>

struct skinny64_128_tweaked_state
{
  uint32_t schedule[SKINNY_ROUND_NUMBER];
  uint32_t rt;
  uint8_t rc;
  uint8_t tk1[SKINNY_BLOCK_SIZE];
  uint8_t tk2[SKINNY_BLOCK_SIZE];
  const uint8_t *key;
  const uint8_t *tweak;
};

void skinny_encrypt_block(uint8_t *output, const uint8_t *input);
void skinny_decrypt_block(uint8_t *output, const uint8_t *input);
void skinny_set_key(const uint8_t *key);
void skinny_set_tweak(const uint8_t *tweak);
void skinny_set_tk1();
void skinny_set_tk2();

#endif