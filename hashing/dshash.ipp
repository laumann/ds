#include <ctype.h>
#include <iostream>
#include <cstdlib>
#include <sstream>

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

#define low8(c) ((uint32_t)c & ((1<<8)-1))



namespace dshash {

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
	 * Random 64-bit numbers
	 */
	static struct number a = { 0, ((uint32_t) 248421 << 16) | ((uint32_t) 17703), ((uint32_t) 367827 << 16) | ((uint32_t) 516523) };

	static struct number b = { 0, ((uint32_t) 531135 << 16) | ((uint32_t) 651195), ((uint32_t) 20294 << 16) | ((uint32_t) 499642) };

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
		a->high = rh & (((uint32_t) 1 << 25) - 1);

		struct number x = { 0, 0, rh >> 25 };
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
		uint64_t n64;	// tmp storage for 64-bit numbers

		/**
		 * Multiply all the terms
		 */
		mults[8] = m64(a->low, b->low);	// 2^0
		mults[7] = m64(a->low, b->mid);	// 2^32
		mults[6] = m64(a->mid, b->low);	// 2^32
		mults[5] = m64(a->low, b->high);	// 2^64
		mults[4] = m64(a->mid, b->mid);	// 2^64
		mults[3] = m64(a->high, b->low);	// 2^64
		mults[2] = m64(a->mid, b->high);	// 2^96
		mults[1] = m64(a->high, b->mid);	// 2^96
		mults[0] = m64(a->high, b->high);	// 2^128

		for (int i = 0; i < 15; i++)	// Zero
			numbers[i].low = numbers[i].mid = numbers[i].high = 0;

		numbers[0].low = low(mults[8]);
		numbers[0].mid = high(mults[8]);

		numbers[1].low = (uint32_t) (mults[7] >> 57);
		numbers[2].low = (uint32_t) (mults[6] >> 57);

		n64 = mults[7] & (((uint64_t) 1 << 57) - 1);
		numbers[3].mid = low(n64);
		numbers[3].high = high(n64);

		n64 = mults[6] & (((uint64_t) 1 << 57) - 1);
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

		n64 = mults[5] & (((uint64_t) 1 << 25) - 1);
		numbers[8].high = low(n64);

		n64 = mults[4] & (((uint64_t) 1 << 25) - 1);
		numbers[9].high = low(n64);

		n64 = mults[3] & (((uint64_t) 1 << 25) - 1);
		numbers[10].high = low(n64);

		numbers[11].high = low(mults[2] >> 57);
		numbers[11].mid = low(mults[2] >> 25);
		numbers[11].low = low(mults[2] << 7);

		numbers[12].high = low(mults[1] >> 57);
		numbers[12].mid = low(mults[1] >> 25);
		numbers[12].low = low(mults[1] << 7);

		n64 = mults[0] & (((uint64_t) 1 << 50) - 1);
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

	/**
	 * A hasher. This is magic obtained from various sources including:
	 *
	 *  - http://mikecvet.wordpress.com/2011/01/28/customizing-tr1-unordered_map-hashing-and-equality-functions/
	 *  - http://stackoverflow.com/questions/15809087/unordered-map-constructor-error-equal-to-templated-function
	 *  - http://stackoverflow.com/questions/7222143/unordered-map-hash-function-c
	 */
	struct hasher {
		size_t operator () (const std::string& key) const {
			std::istringstream s(key);

			s >> std::noskipws;
			uint64_t c;
			struct number x = { 0, 0, 0 };

			s >> c;
			struct number r = { 0, high(c), low(c) };

			for (s >> c; s.good(); s >> c) {
				x.low = low(c);
				x.mid = high(c);
		
				add_to(&r, &x);
				multp(&r, &a, &r);
		
			}
			multp(&r, &b, &r);
			return r.low;

			/*
			char c;
			struct number n = { 0, 0, 0 };
			struct number ai = { 0, 0, 1 };
			struct number r = { 0, 0, 1 };
			struct number prod = { 0, 0, 0 };

			int i = 0;
			int nchar = 0;
			for (s >> c; s.good(); s >> c) {
				if (nchar < 4)
					n.low |= (low8(c)) << (nchar << 3);
				else
					n.mid |= (low8(c)) << ((nchar - 4) << 3);
			
				if (nchar == 8) {
					multp(&ai, &a, &ai);
					multp(&ai, &n, &prod);
					add_to(&r, &prod);

					if (!((++i & (1 << 6)) - 1))
						modp(&r);
					nchar = n.high = n.mid = n.low = 0;
				}
			}
			if (nchar > 0) {
				multp(&ai, &a, &ai);
				multp(&ai, &n, &prod);
				add_to(&r, &prod);
				modp(&r);

			}
			multp(&r, &b, &r);
			return r.low;
			*/
		}
	};
}

using Key = std::string;
using Hash = dshash::hasher;
