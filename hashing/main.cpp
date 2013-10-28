
#include <iostream>
#include <fstream>
#include <ctime>
#include "dshash.hpp"

/**
 * Main function for our tiny hashing program. If there are no arguments it will
 * attempt to read its data from stdin, and produce a hash from that. Otherwise
 * the first argument is assumed to be the file that we wish to hash.
 */

struct number a = {0, ((uint32_t)248421 << 16) | ((uint32_t)17703) , ((uint32_t)367827 << 16) | ((uint32_t)516523) };
struct number b = {0, ((uint32_t)531135 << 16) | ((uint32_t)651195) , ((uint32_t)20294 << 16) | ((uint32_t)499642) };

static void print_bits_32(uint32_t n) {
	uint32_t mask = (uint32_t)1 << 31; //0x8000000000000000;
	for (; mask; mask >>= 1)
		std::cout << (n&mask ? 1 : 0);
}

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

	char c;
	struct number n = {0, 0, 0};
	struct number ai = {0, 0, 1};
	struct number r = {0, 0, 1};
	struct number prod = {0, 0, 0};

	int i = 0;
  	std::clock_t start = std::clock();
	for (in >> c; in.good(); in >> c) {
		n.low = (uint32_t)c;

		multp(&ai, &a, &ai);
		multp(&ai, &n, &prod);
		add_to(&r, &prod);

		if (!((++i & (1 << 6)) - 1))
			modp(&r);
	}
  	std::clock_t stop = std::clock();
  	double running_time = double(stop - start) / double(CLOCKS_PER_SEC);
	std::cout << "Running time: " << running_time << std::endl;


	in.close();
	std::cout << "Number of iterations: " << i << std::endl;
	print_bits_32(r.low);
	std::cout << std::endl;
	std::cout << r.low << std::endl;
}
