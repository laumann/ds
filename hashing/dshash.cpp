
#include <ctype.h>
#include <iostream>
#include <cstdlib>

#include "dshash.hpp"

void usage() {
	std::cout << "Usage: ./stl_hash <file>" << std::endl;
	std::exit(0);
}


/**
 * Internal use (so far)
 */
static void print_number(struct number *n) {
	uint32_t mask = (uint32_t)1 << 31; //0x8000000000000000;
	for (; mask; mask >>= 1)
		std::cout << (n->high&mask ? 1 : 0);
	std::cout << "|";

	mask = (uint32_t)1 << 31;
	for (; mask; mask >>= 1)
		std::cout << (n->mid&mask ? 1 : 0);
	std::cout << "|";

	mask = (uint32_t)1 << 31;
	for (; mask; mask >>= 1)
		std::cout << (n->low&mask ? 1 : 0);
}

/**
 * Print the bit pattern
 */
void print_bits(uint64_t x) {
	uint64_t mask = (uint64_t)1 << 63; //0x8000000000000000;
	for (; mask; mask >>= 1)
		std::cout << (x&mask ? 1 : 0);
}

/**
 * Add b to a (modifying the struct)
 */
void add_to(struct number *a, struct number *b) {
	bool overflow;
	a->low += b->low;
	overflow = a->low < b->low;

	a->mid = (a->mid + b->mid) + overflow;
	overflow = a->mid < b->mid;

	a->high = (a->high + b->high) + overflow;
}

/**
 * Do mod p on number
 */
void modp(struct number *a) {
	uint32_t rh = a->high;
	a->high = rh & (((uint32_t)1 << 25) - 1);

	struct number x = {0, 0, rh >> 25};
	add_to(a, &x);
}

/**
 * Multiply two numbers modulo p, and put the result in the third number struct.
 * The output struct is expected to have been allocated, and whatever value was
 * in it will be overwritten.
 *
 * It is safe to use one of the input numbers as the output, as their usage
 * do not overlap. Therefore doing something like:
 *
 * 	multp(a, b, a);
 *
 * is perfectly alright and will put the result in a.
 */
void multp(struct number *a, struct number *b, struct number *result) {
	
	uint64_t mults[9];
	struct number numbers[15];
	uint64_t n64; // tmp storage for 64-bit numbers

	/**
	 * Multiply all the terms
	 */
	mults[8] = m64(a->low, b->low);		// 2^0
	mults[7] = m64(a->low, b->mid);		// 2^32
	mults[6] = m64(a->mid, b->low);		// 2^32
	mults[5] = m64(a->low, b->high);	// 2^64
	mults[4] = m64(a->mid, b->mid);		// 2^64
	mults[3] = m64(a->high, b->low);	// 2^64
	mults[2] = m64(a->mid, b->high);	// 2^96
	mults[1] = m64(a->high, b->mid);	// 2^96
	mults[0] = m64(a->high, b->high);	// 2^128

	for (int i = 0; i < 15; i++) // Zero
		numbers[i].low = numbers[i].mid = numbers[i].high = 0;

	numbers[0].low = low(mults[8]);
	numbers[0].mid = high(mults[8]);

	numbers[1].low = (uint32_t)(mults[7] >> 57);
	numbers[2].low = (uint32_t)(mults[6] >> 57);

	n64 = mults[7] & (((uint64_t)1 << 57) - 1);
	numbers[3].mid = low(n64);
	numbers[3].high = high(n64);

	n64 = mults[6] & (((uint64_t)1 << 57) - 1);
	numbers[4].mid = low(n64);
	numbers[4].high = high(n64);

	n64 = mults[5] >> 25;
	numbers[5].low = low(n64);
	numbers[5].mid = high(n64);

	n64 = mults[4] >> 25;
	numbers[6].low = low(n64);
	numbers[6].mid = high(n64);
	
	n64 = mults[3] >> 25;
	numbers[7].low = low(n64);
	numbers[7].mid = high(n64);

	n64 = mults[5] & (((uint64_t)1 << 25) - 1);
	numbers[8].high = low(n64);
	
	n64 = mults[4] & (((uint64_t)1 << 25) - 1);
	numbers[9].high = low(n64);

	n64 = mults[3] & (((uint64_t)1 << 25) - 1);
	numbers[10].high = low(n64);

	numbers[11].high = low(mults[2] >> 57);
	numbers[11].mid = low(mults[2] >> 25);
	numbers[11].low = low(mults[2] << 7);
	
	numbers[12].high = low(mults[1] >> 57);
	numbers[12].mid = low(mults[1] >> 25);
	numbers[12].low = low(mults[1] << 7);

	n64 = mults[0] & (((uint64_t)1 << 50) - 1);
	numbers[13].high = low(n64 >> 25);
	numbers[13].mid = low(n64 << 7);

	numbers[14].low = low(mults[0] >> 50);

	result->low = result->mid = result->high = 0;

	/**
	 * Add them up
	 */
	for (int i = 0; i < 15; i++)
		add_to(result, &numbers[i]);

	/**
	 * One more modulo
	 */
	modp(result);
}

/*
void test_add() {
	struct number n1 = {0, 0, 21};
	struct number n2 = {0, 0, 21};

	print_number(&n1);
	std::cout << std::endl;
	print_number(&n2);
	std::cout << std::endl;
	
	add_to(&n1, &n2);
	
	print_number(&n1);
	std::cout << std::endl;
}
*/
void test_add_larger() {
	struct number n1 = {0, ((uint32_t)1<<29)-1, ((uint32_t)1<<32)-1};
	struct number n2 = {0, 0, ((uint32_t)1<<31)-1 };

	print_number(&n1);
	std::cout << std::endl;
	print_number(&n2);
	std::cout << std::endl;
	
	add_to(&n1, &n2);
	
	print_number(&n1);
	std::cout << std::endl;
}

void test_new00() {
	struct number n1 = {0, 0, 0};
	struct number n2 = {0, 0, 0};

	print_number(&n1);
	std::cout << std::endl;
	print_number(&n2);
	std::cout << std::endl;
	
	struct number *result = (struct number*)malloc(sizeof(struct number));
	multp(&n1, &n2, result);
	
	print_number(result);
	std::cout << std::endl;

}

void test_new11() {
	struct number n1 = {0, 0, 1};
	struct number n2 = {0, 0, 1};

	print_number(&n1);
	std::cout << std::endl;
	print_number(&n2);
	std::cout << std::endl;
	
	struct number *result = (struct number*)malloc(sizeof(struct number));
	multp(&n1, &n2, result);
	
	print_number(result);
	std::cout << std::endl;

}

void test_new1() {
	struct number n1 = {0, 0, 21};
	struct number n2 = {0, 0, 2};

	print_number(&n1);
	std::cout << std::endl;
	print_number(&n2);
	std::cout << std::endl;
	
	struct number *result = (struct number*)malloc(sizeof(struct number));
	multp(&n1, &n2, result);
	
	print_number(result);
	std::cout << std::endl;

}

void test_new_first_overflow() {
	struct number n1 = {0, 0, ((uint32_t)1 << 20)};
	struct number n2 = {0, 0, ((uint32_t)1 << 32) - 1};

	print_number(&n1);
	std::cout << std::endl;
	print_number(&n2);
	std::cout << std::endl;
	
	struct number *result = (struct number*)malloc(sizeof(struct number));
	multp(&n1, &n2, result);
	
	print_number(result);
	std::cout << std::endl;

}

void test_new_overflow() {
	struct number n1 = {0, ((uint32_t)1 << 8), 0};
	struct number n2 = {0, 0, ((uint32_t)1 << 32) - 1};

	print_number(&n1);
	std::cout << std::endl;
	print_number(&n2);
	std::cout << std::endl;
	
	struct number *result = (struct number*)malloc(sizeof(struct number));
	multp(&n1, &n2, result);
	
	print_number(result);
	std::cout << std::endl;
}

void test_first_modulo() {
	struct number n1 = {0, ((uint32_t)1 << 30), 0};
	struct number n2 = {0, 0, ((uint32_t)1 << 32) - 1};

	print_number(&n1);
	std::cout << std::endl;
	print_number(&n2);
	std::cout << std::endl;
	
	struct number *result = (struct number*)malloc(sizeof(struct number));
	multp(&n1, &n2, result);
	print_number(result);
	std::cout << std::endl;
}

#define array_size(x) (sizeof(x)/sizeof(x[0]))

uint32_t hash(struct number *a, struct number *b, struct number x[]) {
	
	struct number ai = {0, 0, 1};
	struct number r = {0, 0, 0};
	struct number prod = {0, 0, 0};
	
	for (int i = 0; i < array_size(x); i++) {
		multp(&ai, a, &ai);
		multp(&ai, &x[i], &prod);
		add_to(&r, &prod);
		
		if (!((i & (1 << 6)) - 1))
			modp(&r);
	}
	multp(&r, b, &r);
	return r.low;
}

