#include "dshash.hpp"
#include <iostream>
#include <vector>

/**
 * Assert the i'th is a 
 *
 */
#define bit(n, i) (n & (1 << i))

class TestCase; // forward declaration

class Tests {
public:
	void register_test(TestCase*);
	int size();
	
	std::vector<TestCase*> tests;
};

static Tests t; // collection of tests


void Tests::register_test(TestCase *test) {
	this->tests.push_back(test);
}

int Tests::size() {
	return this->tests.size();
}

class TestCase {
public:
	TestCase(std::string, int (*run)());
	std::string name;
	int (*run)();
};
/**
 * When the test is declared it adds itself to the list
 */
TestCase::TestCase(std::string n, int (*f)()) {
	name = n;
	run = f;
	t.register_test(this);
}

static void print_bits_32(uint32_t n) {
	uint32_t mask = (uint32_t)1 << 31; //0x8000000000000000;
	for (; mask; mask >>= 1)
		std::cout << (n&mask ? 1 : 0);
}

static void print_number(struct number *n) {
	print_bits_32(n->high);
	std::cout << "|";
	print_bits_32(n->mid);
	std::cout << "|";
	print_bits_32(n->low);
	/*
	uint32_t mask = (uint32_t)1 << 31; //0x8000000000000000;
	for (; mask; mask >>= 1)
		std::cout << (n->high&mask ? 1 : 0);

	mask = (uint32_t)1 << 31;
	for (; mask; mask >>= 1)
		std::cout << (n->mid&mask ? 1 : 0);
	std::cout << "|";

	mask = (uint32_t)1 << 31;
	for (; mask; mask >>= 1)
		std::cout << (n->low&mask ? 1 : 0);
	*/
}


/**
 * What do I want to do? I want to restructure all of our test cases in such a
 * way that they support an easy addition of more test function.
 *
 * Declaring a test shold be as easy as declaring TEST(name) { .. }
 *
 * All tests return an integer: 0 if there were no errors (success) and non-zero
 * indicates a failure of some kind.
 *
 */

#define fail return 1;
#define ok return 0;

#define TEST_NAME(name) test_ ## name

#define TEST_STRUCT(name) test_case_ ## name 

#define TEST(name) \
int TEST_NAME(name) (); \
TestCase TEST_STRUCT(name)(#name, & TEST_NAME(name));\
int TEST_NAME(name) ()

/**
 * Fill an uint32_t with 1's from 'from' to 'to'.
 */
#define ones(from, to) ((((uint32_t)1<<to)-1) - (((uint32_t)1<<from)-1))

/**
 * Assert that the number has the provided bit patterns, or print an error
 * message.
 */
#define assert_bit_pattern(n, high, mid, low)\
	if (!(n.high ^ high | n.mid ^ mid | n.low ^ low)) {\
		std::cout << "\nError: Expected ";\
		print_bits_32(high);\
		std::cout << "|";\
		print_bits_32(mid);\
		std::cout << "|";\
		print_bits_32(low);\
		std::cout << "\nBut got:       ";\
		print_number(&n);\
		fail;\
	}\
	ok

TEST(t42) {
	struct number n1 = {0, 0, 21};
	struct number n2 = {0, 0, 21};
	
	add_to(&n1, &n2);
	/**
	 * This should equal zero
	 */
	return n1.high | n1.mid | (n1.low ^ 42);
}

TEST(another) {
	//std::cout << "Foo" << std::endl;
	ok;
}

TEST(add_larger) {
	struct number n1 = {0, ((uint32_t)1<<29)-1, ((uint32_t)1<<32)-1};
	struct number n2 = {0, 0, ((uint32_t)1<<31)-1 };

	add_to(&n1, &n2);
	/**
	 * The 30th bit in n1.mid should be set and all but the 32nd and first
	 * bit in n1.low should be set. n1.high should be zero
	 */
	//sert_bit_pattern(n1, 0, ((uint32_t)1<<29), ones(1,31));
	return n1.high | (n1.mid ^ ((uint32_t)1<<29)) | n1.low ^ ones(1,31);
}


TEST(add_high) {
	struct number n1 = {0, ((uint32_t)1<<32)-1, 0};
	struct number n2 = {0, ((uint32_t)1<<10)-1, 0 };
	/*
	std::cout << std::endl;
	print_number(&n1);
	std::cout << std::endl;
	print_number(&n2);
	std::cout << std::endl;
	*/

	add_to(&n1, &n2);
	/**
	 * The 30th bit in n1.mid should be set and all but the 32nd and first
	 * bit in n1.low should be set. n1.high should be zero
	 */
	return n1.high ^ 1 | n1.mid ^ ones(1,10);
}

/**
 * Test that the ones(...) macro does what it's supposed to
 */
TEST(on) {
	return (~0) ^ ones(0,32);
}


/**
 * Test that we can multiply zero and zero.
 */
TEST(multp_zeros) {
	struct number n1 = {0, 0, 0};
	struct number n2 = {0, 0, 0};
	
	struct number *result = (struct number*)malloc(sizeof(struct number));
	multp(&n1, &n2, result);
	
	return result->high | result->mid | result->low;
}

TEST(multp_one_one) {
	struct number n1 = {0, 0, 1};
	struct number n2 = {0, 0, 1};

	struct number *result = (struct number*)malloc(sizeof(struct number));
	multp(&n1, &n2, result);

	return result->high | result->mid | (result->low ^ 1);
}

/**
 * Test multiplying to small numbers yields a "random" number
 */
TEST(multp_42) {
	struct number n1 = {0, 0, 21};
	struct number n2 = {0, 0, 2};

	struct number *result = (struct number*)malloc(sizeof(struct number));
	multp(&n1, &n2, result);

	return result->high | result->mid | (result->low ^ 42);
}

TEST(first_overflow) {
	struct number n1 = {0, 0, ((uint32_t)1 << 20)};
	struct number n2 = {0, 0, ((uint32_t)1 << 32) - 1};

	struct number *result = (struct number*)malloc(sizeof(struct number));
	multp(&n1, &n2, result);
	
	return result->high | result->mid ^ ones(0, 20) | result->low ^ ones(20,32);
}

TEST(new_overflow) {
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
	return 0;
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


void test_42() {
	struct number n1 = {0, 0, 21};
	struct number n2 = {0, 0, 21};

	//print_number(&n1);
	//std::cout << std::endl;
	//print_number(&n2);
	//std::cout << std::endl;
	
	add_to(&n1, &n2);
	
	//print_number(&n1);
	//std::cout << std::endl;
}

/*
int main(int argc, char* argv[]) {
	//test_42();
	test_42_larger();
	std::cout << std::endl;
	
	test_new00();
	std::cout << std::endl;
	test_new11();
	std::cout << std::endl;
	test_new1();
	std::cout << std::endl;
	test_new_first_overflow();
	std::cout << std::endl;
	
	test_new_overflow();
	std::cout << std::endl;
	
	test_first_modulo();
	std::cout << std::endl;
}
*/


int main() {
	int success = 0;
	int failed = 0;
	int result;

	for (std::vector<TestCase*>::iterator test = t.tests.begin(); test != t.tests.end(); test++) {
		std::cout << "Test: " << (*test)->name;
		result = (*test)->run();
		if (result) {
			failed++;
			std::cout << " (failed)." << std::endl;
		} else {
			success++;
			std::cout << " (ok)." << std::endl;
		}
	}

	// Print summary
	std::cout << std::endl;
	std::cout << "Summary (" << t.size() << " tests):" << std::endl;
	std::cout << "Success: " << success << " Failed: " << failed << std::endl;
}
