#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <cstdlib>
#include "dshash.hpp"
#include <vector>

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
#define low8_64(c) ((uint64_t)c & ((1<<8)-1))

struct number a = {0, ((uint32_t)248421 << 16) | ((uint32_t)17703) , ((uint32_t)367827 << 16) | ((uint32_t)516523) };
struct number b = {0, ((uint32_t)531135 << 16) | ((uint32_t)651195) , ((uint32_t)20294 << 16) | ((uint32_t)499642) };

#include "hash.h2pp" // Must define a function 'uint32_t hashfoo(std::ifstream& s)' that takes an inputstream (ifstream) and processes it

/**
 * 
 */
int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cout << "Usage: ./dshash <file>" << std::endl;
		std::exit(0);
	}
	std::ifstream in(argv[1], std::ifstream::binary);

	if (!in.is_open()) {
		std::cout << "Error: file not open" << std::endl;
		std::exit(1);
	}
	in >> std::noskipws;

	std::clock_t start = std::clock();
	uint32_t h = hashfoo(in);
	std::clock_t stop = std::clock();

	in.close();

	double running_time = double(stop - start) / double(CLOCKS_PER_SEC);
	
	std::cout << std::setprecision(10) << std::fixed << running_time << std::endl;

	return 0;
}
