#include <algorithm>
#include <iostream>
#include <fstream>
#include <ctype.h>
#include <unordered_map>

/**
 * Test case: multiply 2^31 - 1 and 2^61 - 1
 */

/**
 * Representation of a 96-bit number. We need only up to 89 bits, but this will
 * suffice for our needs.
 */
struct number {
	uint32_t high;
	uint32_t mid;
	uint32_t low;
};

struct big_number {
	uint64_t high;
	uint64_t mid;
	uint64_t low;
};

void usage() {
	std::cout << "Usage: ./stl_hash <file>" << std::endl;
	std::exit(0);
}


/*
void print_base(struct number *n) {
	//std::vector<int> 
	while (n->high | n->mid | n->low) {
		// Compute X and R
		
		// output R
	}
}
*/

void print_number(struct number *n) {
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
 * Add two number structs. It is assumed that the result will fit in 96 bits.
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
 * 
 */
struct number *multp(struct number *a, struct number *b) {
	
	uint64_t mults[9];
	uint64_t n64; // tmp storage for 64-bit numbers
	uint32_t term;
	bool overflow;

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

	struct number numbers[15];
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


	struct number *r = (struct number*)malloc(sizeof(struct number));
	r->low = r->mid = r->high = 0;

	/**
	 * Add them up
	 */
	//std::cout << "Adding numbers" << std::endl;
	for (int i = 0; i < 15; i++) {
		//print_number(&numbers[i]); std::cout << std::endl;
		add_to(r, &numbers[i]);
	}
	//std::cout << "End of Adding numbers" << std::endl;

	/**
	 * One more modulo
	 */
	uint32_t rh = r->high;
	r->high = rh & (((uint32_t)1 << 25) - 1);

	struct number x = {0, 0, rh >> 25};
	add_to(r, &x);

	return r;
}

/**
 * Multiply to 'struct numbers' modulo p
 *
 * The idea is to multiply two big numbers
 *
 */
struct big_number *mult_mod_p(struct number *a, struct number *b) {

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


	for (int i = 0; i < 9; i++)
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

	struct big_number *bn = (struct big_number*)malloc(sizeof(struct big_number));
	bn->high = h;
	bn->mid = m;
	bn->low = l;
	return bn;

	/*
	print_bits(h);
	print_bits(m);
	print_bits(l);

	return NULL;
	*/
}
void test2() {
	struct number n1 = {0, 0, ((uint32_t)1 << 5) -1};
	struct number n2 = {0, 0, ((uint32_t)1 << 7) -1};

	struct big_number *result = mult_mod_p(&n1, &n2);
	print_bits(result->high);
	std::cout << "|";
	print_bits(result->mid);
	std::cout << "|";
	print_bits(result->low);
	std::cout << std::endl;
	std::cout << std::endl;
}

void test1() {
	struct number n1 = {0, 0, ((uint32_t)1 << 31) -1};
	struct number n2 = {0, 0, ((uint32_t)1 << 19) -1};

	struct big_number *result = mult_mod_p(&n1, &n2);
	print_bits(result->high);
	std::cout << "|";
	print_bits(result->mid);
	std::cout << "|";
	print_bits(result->low);
	std::cout << std::endl;
	std::cout << std::endl;
}

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
	
	struct number *result = multp(&n1, &n2);
	
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
	
	struct number *result = multp(&n1, &n2);
	
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
	
	struct number *result = multp(&n1, &n2);
	
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
	
	struct number *result = multp(&n1, &n2);
	
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
	
	struct number *result = multp(&n1, &n2);
	
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
	
	struct number *result = multp(&n1, &n2);
	print_number(result);
	std::cout << std::endl;
	/*
	std::cout << "Adding p" << std::endl;

	struct number p = { ((uint32_t)1<<25)-1 ,((uint32_t)1<<32)-1, ((uint32_t)1<<32)-1};
	print_number(&p);
	std::cout << std::endl;

	for (int i=0; i < 32; i++) {
		add_to(result, &p);
		print_number(result);
		std::cout << std::endl;
	}
	*/
}

int main(int argc, char* argv[]) {
	// Make hash table
	std::unordered_map<std::string, int> mapa;

	//test1();
	//test2();

	test_add();
	test_add_larger();
	std::cout << std::endl;
	/*
	test_new00();
	std::cout << std::endl;
	test_new11();
	std::cout << std::endl;
	test_new1();
	std::cout << std::endl;
	test_new_first_overflow();
	std::cout << std::endl;
	*/
	test_new_overflow();
	std::cout << std::endl;
	
	test_first_modulo();
	std::cout << std::endl;

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
