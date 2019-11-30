#include <cipher_util.h>

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
  "ldd r23,Z+7\n"

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

// Permutes the cells within a TKn value while expanding the key schedule.
// PT = [9, 15, 8, 13, 10, 14, 12, 11, 0, 1, 2, 3, 4, 5, 6, 7]
#define PERMUTE_TKN() \
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

#define INV_PERMUTE_TKN() \
  "mov r8,r20\n"    \
  "mov r9,r21\n"    \
  "mov r10,r22\n"   \
  "mov r11,r23\n"   \
  "mov r23,r16\n"   \
  "andi r23,0x0F\n" \
  "mov r20,r17\n"   \
  "andi r20,0xF0\n" \
  "swap r16\n"      \
  "andi r16,0x0F\n" \
  "or r20,r16\n"    \
  "mov r22,r19\n"   \
  "andi r22,0xF0\n" \
  "andi r17,0x0F\n" \
  "or r22,r17\n"    \
  "mov r21,r18\n"   \
  "andi r21,0xF0\n" \
  "andi r19,0x0F\n" \
  "swap r18\n"      \
  "andi r18,0xF0\n" \
  "or r23,r18\n"    \
  "andi r19,0x0F\n" \
  "or r21,r19\n"    \
  "mov r16,r8\n"    \
  "mov r17,r9\n"    \
  "mov r18,r10\n"   \
  "mov r19,r11\n"

  // Transform the contents of a register using LFSR2.
#define LFSR2(reg) \
  "mov r24, " reg "\n" \
  "lsl " reg "\n"      \
  "bst r24,7\n"        \
  "bld " reg ",4\n"    \
  "bst r24,3\n"        \
  "bld " reg ",0\n"    \
  "lsr r24\n"          \
  "lsr r24\n"          \
  "andi r24,0x11\n"    \
  "eor " reg ",r24\n"


void permute_tkn(uint8_t *tkn)
{
  __asm__ __volatile__
  (
    // load the tkn bytes into r16..r23.
    LOAD_BLOCK()
    // permute tkn
    PERMUTE_TKN()
    // store r16...r23 into tkn
    STORE_BLOCK()

    : : "z"(tkn)
    :   "r8",  "r9", "r10", "r11", "r16", "r17", "r18", "r19", "r20", "r21",
        "r22", "r23", "memory"
  );
}

void inv_permute_tkn(uint8_t *tkn)
{
  __asm__ __volatile__
  (
    // load the tkn bytes into r16..r23.
    LOAD_BLOCK()
    // inverse permute tkn
    INV_PERMUTE_TKN()
    // store r16...r23 into tkn
    STORE_BLOCK()

    : : "z"(tkn)
    :   "r8",  "r9", "r10", "r11", "r16", "r17", "r18", "r19", "r20", "r21",
        "r22", "r23", "memory"
  );
}

void calc_next_rc(uint8_t *rc)
{
  __asm__ __volatile__
  (
     // Generate the rc value for the next round.
    // rc = (rc << 1) ^ ((rc >> 5) & 0x01) ^ ((rc >> 4) & 0x01) ^ 0x01;
    "ld r25,Y\n"
    "clr r24\n"
    "lsl r25\n"
    "bst r25,6\n"
    "andi r25,0xbf\n"
    "bld r24,0\n"
    "eor r25,r24\n"
    "bst r25,5\n"
    "bld r24,0\n"
    "eor r25,r24\n"
    "ldi r24,1\n"
    "eor r25,r24\n"
    "st Y,r25\n"

    : : "y"(rc)
    :   "r24",  "r25", "memory"
  );
}

void calc_prev_rc(uint8_t *rc)
{
  __asm__ __volatile__
  (
    "ld r25,Y\n"
    "clr r24\n"
    "bst r25,5\n"
    "bld r24,0\n"
    "eor r25,r24\n"
    "ldi r24,1\n"
    "eor r25,r24\n"
    "bst r25,0\n"
    "lsr r25\n"
    "bld r25,5\n"
    "st Y,r25\n"

    : : "y"(rc)
    :   "r24",  "r25", "memory"
  );
}