#ifndef __DS_HASH
#define __DS_HASH

#include <stdint.h>

/**
 * Representation of a 96-bit number. We work in 89 bits, so the top 7 bits are
 * always expected to be zero
 */
struct number {
	uint32_t high;
	uint32_t mid;
	uint32_t low;
};

/**
 * Useful macros
 */
#define m64(x, y) (int64_t)x * (int64_t)y

/**
 * Mask operations
 */
#define LOW_MASK (((uint64_t)1 << 32) - 1)

/**
 * Extract the 32 highest bits of 64-bit number
 */
#define high64(x) (x >> 32)
#define high(x) ((uint32_t)high64(x))

/**
 * Extract the 32 lowest bits of 64-bit number
 */
#define low64(x) (x & LOW_MASK)
#define low(x) ((uint32_t)low64(x))

/**
 * Function declarations
 */
//void print_number(struct number *);

/**
 * Add the second number to the first. That is, the first number will be
 * modified. Does not take overflow into consideration (as we expect to work in
 * 89 bit numbers, we can simply check the 90'th bit.
 */
void add_to(struct number *, struct number*);

/**
 * Multiply two numbers modulo p (2^89 - 1)
 */
void multp(struct number *, struct number *, struct number *);

#endif
