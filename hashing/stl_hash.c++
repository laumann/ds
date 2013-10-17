
#include <algorithm>
#include <iostream>
#include <fstream>
#include <ctype.h>
#include <unordered_map>

/**
 * Representation of a 96-bit number. We need only up to 89 bits, but this will
 * suffice for our needs.
 */
struct number {
	uint32_t high;
	uint32_t mid;
	uint32_t low;
};

void usage() {
	std::cout << "Usage: ./stl_hash <file>" << std::endl;
	std::exit(0);
}



void print_base(struct number *n) {
	//std::vector<int> 
	while (n->high | n->mid | n->low) {
		// Compute X and R
		
		// output R
	}
}

/**
 * Print the bit pattern
 */
void print_bits(uint64_t x) {
	uint64_t mask = (uint64_t)1 << 63; //0x8000000000000000;
	std::cout << "mask:   " << mask <<std::endl;
	std::cout << "number: " << x << std::endl;
	while (mask) {
		std::cout << (x&mask ? 1 : 0);
		mask >>= 1;
	}
	std::cout << std::endl;
}

#define m64(x, y) (int64_t)x * (int64_t)y

/**
 * Mask operations
 */
#define LOW_MASK (((uint64_t)1 << 32) - 1)

/**
 * Extract the 32 highest bits of 64-bit number
 */
#define high(x) ((uint32_t)(x >> 32))
#define high64(x) ((uint64_t)(x >> 32))

/**
 * Extract the 32 lowest bits of 64-bit number
 */
#define low(x) ((uint32_t)(x & LOW_MASK))
#define low64(x) ((uint64_t)(x & LOW_MASK))

/**
 * Add to number structs. It is assumed that the result will fit in 96 bits.
 */
struct number *add(struct number *a, struct number *b) {
	
	struct number *r = (struct number *)malloc(sizeof(struct number));
	bool overflow;

	r->low = a->low + b->low;
	overflow = r->low < (a->low | b->low);

	r->mid = a->mid + b->mid + overflow;
	overflow = r->mid < (a->mid | b->mid);

	r->high = a->high + b->high + overflow;
	return r;
}

/**
 * Multiply to 'struct numbers' modulo p
 *
 * The idea is to multiply two big numbers
 *
 */
struct number *mult_mod_p(struct number *a, struct number *b) {

	struct number *r;
	uint64_t mults[9];

	/**
	 * Multiply all the bastards
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


	for (int i; i < 9; i++)
		std::cout << mults[i] << std::endl;

	/**
	 * 3 * 64-bit = 196 bit number
	 */
	uint64_t h, m, l;
	
	uint32_t part, overflow;
	uint64_t sum;

	// bits 0-64
	sum = high(mults[8]) + low(mults[7]) + low(mults[6]);
	l = ((uint64_t)low(sum) << 32) | (uint64_t)low(mults[8]);
	overflow = high(sum);

	// bits 64-96
	sum = high(mults[7]) + high(mults[6]) + low(mults[5]) + low(mults[4]) + low(mults[3]) + overflow;
	m = low(sum);
	overflow = high(sum);

	// bits 96-128
	sum = high(mults[5]) + high(mults[4]) + high(mults[3]) + low(mults[2]) + low(mults[1]) + overflow;
	m |= ((uint64_t)low(sum) << 32);
	overflow = high(sum);

	// The remaining bits
	h = high64(mults[2]) + high64(mults[1]) + mults[0] + (uint64_t)overflow;

	// Now we have the product (in bits). Next we need to do modulo p = 2^89 - 1
	// Take the 89 lowest bits + the number from shifting down 89 bits
	

	print_bits(l);
	print_bits(m);
	print_bits(h);

	return NULL;
}

int main(int argc, char* argv[]) {
	// Make hash table
	std::unordered_map<std::string, int> mapa;

	struct number x = { 0, 0, (uint32_t)1 << 22}; // should end up in 23+29 = 42
	struct number y = { 0, 0, (uint32_t)1 << 29};

	mult_mod_p(&x, &y);
	
	x = { 0, ((uint32_t)1 << 22) -1, 0}; // should end up in (22+32)+29 = 42+32 = 74
	y = { 0, 0, ((uint32_t)1 << 29) -1};
	mult_mod_p(&x, &y);


	std::cout << "======================" << std::endl;
	uint32_t big = (2 << 31) - 2;
	std::cout << big << std::endl;
	uint32_t m = 1;
	uint32_t sum = big + m;
	bool overflow = sum < (big  | m);
	std::cout << "Result: " << sum  << " | Overflow: " << overflow << std::endl;

	m = 2;
	sum = big + m;
	overflow = sum < (big | m);
	std::cout << "Result: " << sum  << " | Overflow: " << overflow << std::endl;

	if (argc < 2)
		usage();

	std::ifstream in(argv[1]);
	std::string word = "";
	int i = 0;
	while (in >> word) {
		std::cout << "[" << ++i << "] " << word << std::endl;
		mapa[word]++;
	}
	std::cout << "Occurences of 'the': " << mapa["the"] << std::endl;
	in.close();




/*
	std::ifstream in;
	in.open(argv[1]);

	wchar_t c[;
	std::wfilebuf win;
	win.open(argv[1]);

	if (win.is_open()) {
		while (!win.eof()) {
			win >> c;

		}
	}

	if (in.is_open()) {
		std::cout << "file is opened" << std::endl;

		char c[256];
		int i = 0;
		while (!in.eof()) {
			in.get(c, 256, ' ');
			std::cout << "[" << ++i << "]" << c << std::endl;
		}
		in.close();
	} else {
		std::cout << "error opening file" << std::endl;
	}
*/
}
/*
number hash (a, b) {
	random a;
	for (int i = 0; i < d; i++) {
		a *= a;
		res += x_i * a_i;
		res = res % p;
	}
	res *= b;
	res = res % p;
	res = res % m;
	return res;
}
*/
