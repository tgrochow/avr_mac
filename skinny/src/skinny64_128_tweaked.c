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

// systrem library
#include <avr/pgmspace.h>
#include <string.h>


#include <skinny64_128_tweaked.h>

// Force the sboxes to be aligned on a 256-byte boundary.
// This makes sbox lookups more efficient.
#define ALIGN256 __attribute__((aligned(256)))

// S-box tables for Skinny-64.  We only use this for AVR platforms,
// as there will be issues with constant cache behaviour on ARM.
// It would be nice to avoid this for AVR as well, but the S-box
// operations are simply too slow using bit operations on AVR.
// Technically the S-boxes for Skinny-64 are 4-bit but we expand
// them to 8-bit to make the lookups easier.
static uint8_t const sbox[256] PROGMEM ALIGN256 = {
  0xcc, 0xc6, 0xc9, 0xc0, 0xc1, 0xca, 0xc2, 0xcb, 0xc3, 0xc8, 0xc5, 0xcd,
  0xc4, 0xce, 0xc7, 0xcf, 0x6c, 0x66, 0x69, 0x60, 0x61, 0x6a, 0x62, 0x6b,
  0x63, 0x68, 0x65, 0x6d, 0x64, 0x6e, 0x67, 0x6f, 0x9c, 0x96, 0x99, 0x90,
  0x91, 0x9a, 0x92, 0x9b, 0x93, 0x98, 0x95, 0x9d, 0x94, 0x9e, 0x97, 0x9f,
  0x0c, 0x06, 0x09, 0x00, 0x01, 0x0a, 0x02, 0x0b, 0x03, 0x08, 0x05, 0x0d,
  0x04, 0x0e, 0x07, 0x0f, 0x1c, 0x16, 0x19, 0x10, 0x11, 0x1a, 0x12, 0x1b,
  0x13, 0x18, 0x15, 0x1d, 0x14, 0x1e, 0x17, 0x1f, 0xac, 0xa6, 0xa9, 0xa0,
  0xa1, 0xaa, 0xa2, 0xab, 0xa3, 0xa8, 0xa5, 0xad, 0xa4, 0xae, 0xa7, 0xaf,
  0x2c, 0x26, 0x29, 0x20, 0x21, 0x2a, 0x22, 0x2b, 0x23, 0x28, 0x25, 0x2d,
  0x24, 0x2e, 0x27, 0x2f, 0xbc, 0xb6, 0xb9, 0xb0, 0xb1, 0xba, 0xb2, 0xbb,
  0xb3, 0xb8, 0xb5, 0xbd, 0xb4, 0xbe, 0xb7, 0xbf, 0x3c, 0x36, 0x39, 0x30,
  0x31, 0x3a, 0x32, 0x3b, 0x33, 0x38, 0x35, 0x3d, 0x34, 0x3e, 0x37, 0x3f,
  0x8c, 0x86, 0x89, 0x80, 0x81, 0x8a, 0x82, 0x8b, 0x83, 0x88, 0x85, 0x8d,
  0x84, 0x8e, 0x87, 0x8f, 0x5c, 0x56, 0x59, 0x50, 0x51, 0x5a, 0x52, 0x5b,
  0x53, 0x58, 0x55, 0x5d, 0x54, 0x5e, 0x57, 0x5f, 0xdc, 0xd6, 0xd9, 0xd0,
  0xd1, 0xda, 0xd2, 0xdb, 0xd3, 0xd8, 0xd5, 0xdd, 0xd4, 0xde, 0xd7, 0xdf,
  0x4c, 0x46, 0x49, 0x40, 0x41, 0x4a, 0x42, 0x4b, 0x43, 0x48, 0x45, 0x4d,
  0x44, 0x4e, 0x47, 0x4f, 0xec, 0xe6, 0xe9, 0xe0, 0xe1, 0xea, 0xe2, 0xeb,
  0xe3, 0xe8, 0xe5, 0xed, 0xe4, 0xee, 0xe7, 0xef, 0x7c, 0x76, 0x79, 0x70,
  0x71, 0x7a, 0x72, 0x7b, 0x73, 0x78, 0x75, 0x7d, 0x74, 0x7e, 0x77, 0x7f,
  0xfc, 0xf6, 0xf9, 0xf0, 0xf1, 0xfa, 0xf2, 0xfb, 0xf3, 0xf8, 0xf5, 0xfd,
  0xf4, 0xfe, 0xf7, 0xff
};
static uint8_t const sbox_inv[256] PROGMEM ALIGN256 = {
  0x33, 0x34, 0x36, 0x38, 0x3c, 0x3a, 0x31, 0x3e, 0x39, 0x32, 0x35, 0x37,
  0x30, 0x3b, 0x3d, 0x3f, 0x43, 0x44, 0x46, 0x48, 0x4c, 0x4a, 0x41, 0x4e,
  0x49, 0x42, 0x45, 0x47, 0x40, 0x4b, 0x4d, 0x4f, 0x63, 0x64, 0x66, 0x68,
  0x6c, 0x6a, 0x61, 0x6e, 0x69, 0x62, 0x65, 0x67, 0x60, 0x6b, 0x6d, 0x6f,
  0x83, 0x84, 0x86, 0x88, 0x8c, 0x8a, 0x81, 0x8e, 0x89, 0x82, 0x85, 0x87,
  0x80, 0x8b, 0x8d, 0x8f, 0xc3, 0xc4, 0xc6, 0xc8, 0xcc, 0xca, 0xc1, 0xce,
  0xc9, 0xc2, 0xc5, 0xc7, 0xc0, 0xcb, 0xcd, 0xcf, 0xa3, 0xa4, 0xa6, 0xa8,
  0xac, 0xaa, 0xa1, 0xae, 0xa9, 0xa2, 0xa5, 0xa7, 0xa0, 0xab, 0xad, 0xaf,
  0x13, 0x14, 0x16, 0x18, 0x1c, 0x1a, 0x11, 0x1e, 0x19, 0x12, 0x15, 0x17,
  0x10, 0x1b, 0x1d, 0x1f, 0xe3, 0xe4, 0xe6, 0xe8, 0xec, 0xea, 0xe1, 0xee,
  0xe9, 0xe2, 0xe5, 0xe7, 0xe0, 0xeb, 0xed, 0xef, 0x93, 0x94, 0x96, 0x98,
  0x9c, 0x9a, 0x91, 0x9e, 0x99, 0x92, 0x95, 0x97, 0x90, 0x9b, 0x9d, 0x9f,
  0x23, 0x24, 0x26, 0x28, 0x2c, 0x2a, 0x21, 0x2e, 0x29, 0x22, 0x25, 0x27,
  0x20, 0x2b, 0x2d, 0x2f, 0x53, 0x54, 0x56, 0x58, 0x5c, 0x5a, 0x51, 0x5e,
  0x59, 0x52, 0x55, 0x57, 0x50, 0x5b, 0x5d, 0x5f, 0x73, 0x74, 0x76, 0x78,
  0x7c, 0x7a, 0x71, 0x7e, 0x79, 0x72, 0x75, 0x77, 0x70, 0x7b, 0x7d, 0x7f,
  0x03, 0x04, 0x06, 0x08, 0x0c, 0x0a, 0x01, 0x0e, 0x09, 0x02, 0x05, 0x07,
  0x00, 0x0b, 0x0d, 0x0f, 0xb3, 0xb4, 0xb6, 0xb8, 0xbc, 0xba, 0xb1, 0xbe,
  0xb9, 0xb2, 0xb5, 0xb7, 0xb0, 0xbb, 0xbd, 0xbf, 0xd3, 0xd4, 0xd6, 0xd8,
  0xdc, 0xda, 0xd1, 0xde, 0xd9, 0xd2, 0xd5, 0xd7, 0xd0, 0xdb, 0xdd, 0xdf,
  0xf3, 0xf4, 0xf6, 0xf8, 0xfc, 0xfa, 0xf1, 0xfe, 0xf9, 0xf2, 0xf5, 0xf7,
  0xf0, 0xfb, 0xfd, 0xff
};

static struct skinny64_128_tweaked_state skinny_state;

// Figure out how to do lookups from a pgmspace sbox table on this platform.
#if defined(RAMPZ)
#define SBOX(reg)   \
  "mov r30," reg "\n" \
  "elpm " reg ",Z\n"
#elif defined(__AVR_HAVE_LPMX__)
#define SBOX(reg)   \
  "mov r30," reg "\n" \
  "lpm " reg ",Z\n"
#elif defined(__AVR_TINY__)
#define SBOX(reg)   \
  "mov r30," reg "\n" \
  "ld " reg ",Z\n"
#else
#define SBOX(reg)   \
  "mov r30," reg "\n" \
  "lpm\n" \
  "mov " reg ",r0\n"
#endif

// Mix the columns during an encryption round.
#define MIX_COLUMNS(row0, row1, row2, row3) \
  "eor " row1 "," row2 "\n" \
  "eor " row2 "," row0 "\n" \
  "mov __tmp_reg__," row3 "\n" \
  "eor __tmp_reg__," row2 "\n" \
  "mov " row3 "," row2 "\n" \
  "mov " row2 "," row1 "\n" \
  "mov " row1 "," row0 "\n" \
  "mov " row0 ",__tmp_reg__\n"

// Inverse mix of the columns during a decryption round.
#define MIX_COLUMNS_INV(row0, row1, row2, row3) \
  "mov __tmp_reg__," row3 "\n" \
  "mov " row3 "," row0 "\n" \
  "mov " row0 "," row1 "\n" \
  "mov " row1 "," row2 "\n" \
  "eor " row3 ",__tmp_reg__\n" \
  "eor __tmp_reg__," row0 "\n" \
  "mov " row2 ",__tmp_reg__\n" \
  "eor " row1 "," row2 "\n"

// Load a 64-bit input block into r16..r23.  Note that the even cells
// are in the high nibbles of each byte rather than the low nibble.
#define LOAD_BLOCK() \
  "ld r16,Z\n" \
  "ldd r17,Z+1\n" \
  "ldd r18,Z+2\n" \
  "ldd r19,Z+3\n" \
  "ldd r20,Z+4\n" \
  "ldd r21,Z+5\n" \
  "ldd r22,Z+6\n" \
  "ldd r23,Z+7\n" \

// Store r16..r23 to a 64-bit output block.
#define STORE_BLOCK() \
  "st Z,r16\n" \
  "std Z+1,r17\n" \
  "std Z+2,r18\n" \
  "std Z+3,r19\n" \
  "std Z+4,r20\n" \
  "std Z+5,r21\n" \
  "std Z+6,r22\n" \
  "std Z+7,r23\n"

void skinny_encrypt_block(uint8_t *output, const uint8_t *input)
{
  #if defined(RAMPZ)
    uint32_t sbox_addr = (uint32_t)sbox;
  #else
    uint16_t sbox_addr = (uint16_t)sbox;
  #endif

  __asm__ __volatile__
  (
    // Load the input block from Z[0..15] into r16..r23.
    LOAD_BLOCK()

    // Set up Z to point to the start of the sbox table.
    "ldd r30,%A3\n"
    "ldd r31,%B3\n"
    #if defined(RAMPZ)
      "in __tmp_reg__,%5\n"
      "push __tmp_reg__\n"
      "ldd __tmp_reg__,%C3\n"
      "out %5,__tmp_reg__\n"
    #endif

    // Top of the loop.
    "1:\n"

    // Transform the state using the sbox.
    SBOX("r16")
    SBOX("r17")
    SBOX("r18")
    SBOX("r19")
    SBOX("r20")
    SBOX("r21")
    SBOX("r22")
    SBOX("r23")

    // XOR the state with the key schedule.
    "ld __tmp_reg__,X+\n"
    "eor r16,__tmp_reg__\n"
    "ld __tmp_reg__,X+\n"
    "eor r17,__tmp_reg__\n"
    "ld __tmp_reg__,X+\n"
    "eor r18,__tmp_reg__\n"
    "ld __tmp_reg__,X+\n"
    "eor r19,__tmp_reg__\n"
    "ldi r24,0x20\n"
    "eor r20,r24\n"

    // Shift the rows.
    "swap r18\n"                // r18:r19 = shift_right_4(r18:r19)
    "swap r19\n"
    "mov r24,r18\n"
    "mov r25,r19\n"
    "andi r24,0xF0\n"
    "andi r25,0xF0\n"
    "andi r18,0x0F\n"
    "andi r19,0x0F\n"
    "or r18,r25\n"
    "or r19,r24\n"
    "mov __tmp_reg__,r20\n"     // r20:r21 = shift_right_8(r20:r21)
    "mov r20,r21\n"
    "mov r21,__tmp_reg__\n"
    "swap r22\n"                // r22:r23 = shift_right_12(r22:r23)
    "swap r23\n"
    "mov r24,r22\n"
    "mov r25,r23\n"
    "andi r24,0x0F\n"
    "andi r25,0x0F\n"
    "andi r22,0xF0\n"
    "andi r23,0xF0\n"
    "or r22,r25\n"
    "or r23,r24\n"

    // Mix the columns.
    MIX_COLUMNS("r16", "r18", "r20", "r22")
    MIX_COLUMNS("r17", "r19", "r21", "r23")

    // Bottom of the loop.
    "dec %4\n"
    "breq 2f\n"
    "rjmp 1b\n"
    "2:\n"

    // Restore the original RAMPZ value.
    #if defined(RAMPZ)
      "pop __tmp_reg__\n"
      "out %5,__tmp_reg__\n"
    #endif

    // Store the final state into the output buffer.
    "ldd r30,%A2\n"
    "ldd r31,%B2\n"
    STORE_BLOCK()

    : : "x"(skinny_state.schedule), "z"(input), "Q"(output), "Q"(sbox_addr),
        "r"((uint8_t)SKINNY_ROUND_NUMBER)
    #if defined(RAMPZ)
      , "I" (_SFR_IO_ADDR(RAMPZ))
    #endif
    :  "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23",
       "r24", "r25", "memory"
  );
}

void skinny_decrypt_block(uint8_t *output, const uint8_t *input)
{
  #if defined(RAMPZ)
    uint32_t sbox_addr = (uint32_t)sbox_inv;
  #else
    uint16_t sbox_addr = (uint16_t)sbox_inv;
  #endif

  __asm__ __volatile__
  (
    // Load the input block from Z[0..15] into r16..r23.
    LOAD_BLOCK()

    // Set up Z to point to the start of the sbox table.
    "ldd r30,%A3\n"
    "ldd r31,%B3\n"
    #if defined(RAMPZ)
      "in __tmp_reg__,%5\n"
      "push __tmp_reg__\n"
      "ldd __tmp_reg__,%C3\n"
      "out %5,__tmp_reg__\n"
    #endif

    // Top of the loop.
    "1:\n"

    // Inverse mix of the columns.
    MIX_COLUMNS_INV("r16", "r18", "r20", "r22")
    MIX_COLUMNS_INV("r17", "r19", "r21", "r23")

    // Inverse shift of the rows.
    "swap r18\n"                // r18:r19 = shift_right_12(r18:r19)
    "swap r19\n"
    "mov r24,r18\n"
    "mov r25,r19\n"
    "andi r24,0x0F\n"
    "andi r25,0x0F\n"
    "andi r18,0xF0\n"
    "andi r19,0xF0\n"
    "or r18,r25\n"
    "or r19,r24\n"
    "mov __tmp_reg__,r20\n"     // r20:r21 = shift_right_8(r20:r21)
    "mov r20,r21\n"
    "mov r21,__tmp_reg__\n"
    "swap r22\n"                // r22:r23 = shift_right_4(r22:r23)
    "swap r23\n"
    "mov r24,r22\n"
    "mov r25,r23\n"
    "andi r24,0xF0\n"
    "andi r25,0xF0\n"
    "andi r22,0x0F\n"
    "andi r23,0x0F\n"
    "or r22,r25\n"
    "or r23,r24\n"

    // XOR the state with the key schedule.
    "ld __tmp_reg__,-X\n"
    "eor r19,__tmp_reg__\n"
    "ld __tmp_reg__,-X\n"
    "eor r18,__tmp_reg__\n"
    "ld __tmp_reg__,-X\n"
    "eor r17,__tmp_reg__\n"
    "ld __tmp_reg__,-X\n"
    "eor r16,__tmp_reg__\n"
    "ldi r24,0x20\n"
    "eor r20,r24\n"

    // Transform the state using the inverse sbox.
    SBOX("r16")
    SBOX("r17")
    SBOX("r18")
    SBOX("r19")
    SBOX("r20")
    SBOX("r21")
    SBOX("r22")
    SBOX("r23")

    // Bottom of the loop.
    "dec %4\n"
    "breq 2f\n"
    "rjmp 1b\n"
    "2:\n"

    // Restore the original RAMPZ value.
    #if defined(RAMPZ)
      "pop __tmp_reg__\n"
      "out %5,__tmp_reg__\n"
    #endif

    // Store the final state into the output buffer.
    "ldd r30,%A2\n"
    "ldd r31,%B2\n"
    STORE_BLOCK()

    : : "x"(skinny_state.schedule + SKINNY_ROUND_NUMBER), "z"(input),
        "Q"(output), "Q"(sbox_addr), "r"((uint8_t)SKINNY_ROUND_NUMBER)
    #if defined(RAMPZ)
      , "I" (_SFR_IO_ADDR(RAMPZ))
    #endif
    :  "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23",
       "r24", "r25", "memory"
  );
}

// Permutes the cells within a TKn value while expanding the key schedule.
// PT = [9, 15, 8, 13, 10, 14, 12, 11, 0, 1, 2, 3, 4, 5, 6, 7]
#define PERMUTE_TKn() \
  "mov r8,r16\n"              /* r8..r11 = TK[0..7] */ \
  "mov r9,r17\n" \
  "mov r10,r18\n" \
  "mov r11,r19\n" \
  "mov r16,r23\n"             /* TK[1] = TK[15] */ \
  "andi r16,0x0F\n" \
  "mov r17,r20\n"             /* TK[2] = TK[8] */ \
  "andi r17,0xF0\n" \
  "swap r20\n"                /* TK[0] = TK[9] */ \
  "andi r20,0xF0\n" \
  "or r16,r20\n" \
  "mov r19,r22\n"             /* TK[6] = TK[12] */ \
  "andi r19,0xF0\n" \
  "andi r22,0x0F\n"           /* TK[3] = TK[13] */ \
  "or r17,r22\n" \
  "mov r18,r21\n"             /* TK[4] = TK[10] */ \
  "andi r18,0xF0\n" \
  "swap r23\n"                /* TK[5] = TK[14] */ \
  "andi r23,0x0F\n" \
  "or r18,r23\n" \
  "andi r21,0x0F\n"           /* TK[7] = TK[11] */ \
  "or r19,r21\n" \
  "mov r20,r8\n"              /* TK[8..15] = r8..r11 */ \
  "mov r21,r9\n" \
  "mov r22,r10\n" \
  "mov r23,r11\n"

/**
 * \brief Clears the key schedule and sets it to the schedule for TK1.
 *
 * \param key Points to the 16 bytes of TK1.
 * \param tweaked Set to true if the subclass uses tweaks.
 */
void skinny_set_tk1(const uint8_t *key)
{
  __asm__ __volatile__
  (
    // Load the TK1 cells into r16..r23.
    LOAD_BLOCK()

    // Set rc to zero (stored in r25).
    "clr r25\n"

    // Top of the loop.
    "1:\n"

    // Generate the rc value for the next round.
    // rc = (rc << 1) ^ ((rc >> 5) & 0x01) ^ ((rc >> 4) & 0x01) ^ 0x01;
    "clr r24\n"
    "lsl r25\n"
    "bst r25,6\n"
    "bld r24,0\n"
    "eor r25,r24\n"
    "bst r25,5\n"
    "bld r24,0\n"
    "eor r25,r24\n"
    "ldi r24,1\n"
    "eor r25,r24\n"

    // Store the first 8 cells of TK1 into the key schedule and XOR with rc.
    "mov r24,r25\n"
    "andi r24,0x0F\n"
    "swap r24\n"
    "eor r24,r16\n"
    "st X+,r24\n"
    "mov r24,%3\n"
    "eor r24,r17\n"
    "st X+,r24\n"
    "mov r24,r25\n"
    "andi r24,0x30\n"
    "eor r24,r18\n"
    "st X+,r24\n"
    "st X+,r19\n"

    // Permute TK1 for the next round.
    PERMUTE_TKn()

    // Bottom of the loop.
    "dec %2\n"
    "breq 2f\n"
    "rjmp 1b\n"
    "2:\n"

    : : "x"(skinny_state.schedule), "z"(key), "r"(SKINNY_ROUND_NUMBER),
        "r"((uint8_t)0x20)
    : "r8",  "r9", "r10", "r11", "r16", "r17", "r18", "r19",
      "r20", "r21", "r22", "r23", "r24", "r25", "memory"
  );
}

/**
 * \brief XOR's the key schedule with the schedule for TK1.
 *
 * \param key Points to the 16 bytes of TK1.
 *
 * This function is used to adjust the tweak for the tweakable versions
 * of the SKINNY block cipher.
 */
void skinny_xor_tk1(const uint8_t *key)
{
  __asm__ __volatile__
  (
    // Load the TK1 bytes into r16..r23.
    LOAD_BLOCK()

    // Top of the loop.
    "1:\n"

    // XOR the first two rows of TK1 with the key schedule.
    "ld __tmp_reg__,X\n"
    "eor __tmp_reg__,r16\n"
    "st X+,__tmp_reg__\n"
    "ld __tmp_reg__,X\n"
    "eor __tmp_reg__,r17\n"
    "st X+,__tmp_reg__\n"
    "ld __tmp_reg__,X\n"
    "eor __tmp_reg__,r18\n"
    "st X+,__tmp_reg__\n"

    // Permute TK1 for the next round.
    PERMUTE_TKn()

    // Bottom of the loop.
    "dec %2\n"
    "breq 2f\n"
    "rjmp 1b\n"
    "2:\n"

    : : "x"(skinny_state.schedule), "z"(key), "r"(SKINNY_ROUND_NUMBER)
    :  "r8",  "r9", "r10", "r11", "r16", "r17", "r18", "r19",
      "r20", "r21", "r22", "r23", "r24", "memory"
  );
}

// Transform the contents of a register using LFSR2.
#define LFSR2(reg) \
  "mov r24, " reg "\n" \
  "lsl " reg "\n" \
  "bst r24,7\n" \
  "bld " reg ",4\n" \
  "bst r24,3\n" \
  "bld " reg ",0\n" \
  "lsr r24\n" \
  "lsr r24\n" \
  "andi r24,0x11\n" \
  "eor " reg ",r24\n"

/**
 * \brief XOR's the key schedule with the schedule for TK2.
 *
 * \param key Points to the 16 bytes of TK2.
 */
void skinny_set_tk2(const uint8_t *key)
{
  __asm__ __volatile__
  (
    // Load the TK2 bytes into r16..r23.
    LOAD_BLOCK()

    // Top of the loop.
    "1:\n"

    // XOR the first two rows of TK2 with the key schedule.
    "ld __tmp_reg__,X\n"
    "eor __tmp_reg__,r16\n"
    "st X+,__tmp_reg__\n"
    "ld __tmp_reg__,X\n"
    "eor __tmp_reg__,r17\n"
    "st X+,__tmp_reg__\n"
    "ld __tmp_reg__,X\n"
    "eor __tmp_reg__,r18\n"
    "st X+,__tmp_reg__\n"
    "ld __tmp_reg__,X\n"
    "eor __tmp_reg__,r19\n"
    "st X+,__tmp_reg__\n"

    // Permute TK2 for the next round.
    PERMUTE_TKn()

    // Apply LFSR2 to the first two rows of TK2.
    LFSR2("r16")
    LFSR2("r17")
    LFSR2("r18")
    LFSR2("r19")

    // Bottom of the loop.
    "dec %2\n"
    "breq 2f\n"
    "rjmp 1b\n"
    "2:\n"

    : : "x"(skinny_state.schedule), "z"(key), "r"(SKINNY_ROUND_NUMBER)
    :  "r8",  "r9", "r10", "r11", "r16", "r17", "r18", "r19",
      "r20", "r21", "r22", "r23", "r24", "memory"
  );
}


void skinny_set_key(const uint8_t *key)
{
  skinny_reset_tweak();
  skinny_set_tk2(key);
}

/**
 * \brief Sets the 64-bit tweak value for this block cipher.
 *
 * \param tweak Points to the tweak, and can be NULL if you want a
 * tweak of all-zeroes (the default).
 * \param len Length of \a tweak in bytes, which must be 8.
 *
 * This function must be called after setKey() as the setKey()
 * call will implicitly set the tweak back to all-zeroes.
 *
 * \sa setKey()
 */
void skinny_set_tweak(const uint8_t *tweak)
{
  skinny_xor_tk1(skinny_state.tweak);
  if (tweak)
  {
    memcpy(skinny_state.tweak, tweak, SKINNY_BLOCK_SIZE);
    skinny_xor_tk1(skinny_state.tweak);
  }
  else
  {
    memset(skinny_state.tweak, 0, sizeof(skinny_state.tweak));
  }
}

/**
 * \brief Resets the tweak to all-zeroes.
 *
 * This is used by subclass implementations of setKey().
 */
void skinny_reset_tweak()
{
  memset(skinny_state.tweak, 0, sizeof(skinny_state.tweak));
  skinny_set_tk1(skinny_state.tweak);
}