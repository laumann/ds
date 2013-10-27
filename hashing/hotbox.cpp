#include <algorithm>  // std::random_shuffle
//#undef NDEBUG
#include <cassert>
#include <ctime>
#include <cerrno>
#include <functional> // std::less
#include <iostream>
#include <vector> // std::vector
#include <fstream>
#include <cstdio>
#include <iomanip>

#include <unordered_set>

#include "hash.ipp" // Hash and Key should be defined here

#include <string>
#include <iterator>
#include <algorithm>
#include <streambuf>
#include <sstream>

/**
 * Usage function
 */
void usage(int argc, char *argv[]) {
	std::cout << "Usage: " << argv[0] << " <file> [N]" << std::endl;
	exit(0);
}

/**
 * Reads a file, and for each line, treats the text as the (relative) path to a
 * text file to hash.
 */
void get_file_contents(const char *filename, std::string *dest) {
	std::ifstream in(filename, std::ios::in);
	if (in) {
		//std::string contents;
		in.seekg(0, std::ios::end);
		dest->resize(in.tellg());
		//contents.resize(in.tellg());
		in.read(&(*dest)[0], dest->size());
		//in.read(&contents[0], contents.size());
		std::cout << *dest << std::endl;
		in.close();
		return;
		//return contents;
	}
	throw errno;
}

int main(int argc, char** argv) {

	if (argc < 2)
		usage(argc, argv);

	int repetitions = 1;
	if (argc > 2)
		repetitions = std::atoi(argv[2]);

	std::vector<std::string> input;

	/**
	 * Read in indicated file
	 */
	std::ifstream inf(argv[1]);
	if (!inf.is_open()) {
		std::cout << "Error: file '" << argv[1] << "' could not be opened." << std::endl;
		exit(1);
	}

	/**
	 * Read in contents of indicated files
	 */
	std::string line;
	std::ifstream in;
	while (std::getline(inf, line)) {
		in.open(line.c_str(), std::ios::in | std::ios::binary);
		if (in.is_open()) {
			std::ostringstream out;
			out << in.rdbuf();
			input.push_back(out.str());
			in.close();
		}
	}
	inf.close();

	std::clock_t start, stop;
	std::vector<double> time_sums(input.size(), 0);
	
	//C<T> *set;
	std::unordered_set<Key, Hash> *set;

	for (int i=0; i < repetitions; i++) {
		set = new std::unordered_set<Key, Hash>;

		for (uint32_t j = 0; j < input.size(); j++) {
			start = std::clock();

			set->insert(input[j]);
			//set->find(input[j]);

			stop = std::clock();
			
			// Add insertion time to correct index
			time_sums[j] += double(stop-start) / double(CLOCKS_PER_SEC);
		}

		delete set;
	}

	std::cout << std::setprecision(10) << std::fixed << std::endl << "Based on " << repetitions << " repetitions" << std::endl;
	std::cout << "Size (bytes)  Time (seconds)" << std::endl;
	//for (std::vector<double>::iterator it = time_sums.begin(); it != time_sums.end(); it++) {
	for (int i = 0; i < input.size(); i++) {
		std::cout << input[i].size() << time_sums[i]/(double)repetitions << std::endl;
	}

	return 0;
}
