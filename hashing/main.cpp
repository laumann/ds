
#include <iostream>
#include <fstream>
#include "dshash.hpp"

/**
 * Main function for our tiny hashing program. If there are no arguments it will
 * attempt to read its data from stdin, and produce a hash from that. Otherwise
 * the first argument is assumed to be the file that we wish to hash.
 */
int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::exit(0);
	}
	std::ifstream in(argv[1], std::ifstream::binary);

	if (!in.is_open()) {
		std::cout << "Error: file not open" << std::endl;
		std::exit(1);
	}
	in >> std::noskipws;

	char c;
	for (in >> c; in.good(); in >> c) {
		std::cout << c;
	}

	in.close();
}
