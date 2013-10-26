#include <algorithm>  // std::random_shuffle
//#include "algorithm.i++" // K defined here
//#undef NDEBUG
#include <cassert>
#include <ctime>
#include <cerrno>
#include <functional> // std::less
#include <iostream>
#include <vector> // std::vector
#include <fstream>
#include <cstdio>

#include <unordered_set>

/**
 * Reads a file, and for each line, treats the text as the (relative) path to a
 * text file to hash.
 *
 * Runs a preset number of iterations hashing the files indicated
 */

void usage(int argc, char **argv) {
	std::cerr << "Usage: " << argv[0] << " <file>" << std::endl;
	exit(1);
}

std::string get_file_contents(const char *filename) {
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (in) {
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.read(&contents[0], contents.size());
		in.close();
		return contents;
	}
	throw errno;
}

int main(int argc, char** argv) {

	if (argc < 2)
		usage(argc, argv);

	/**
	 * (1) Initialize
	 */
	std::unordered_set<std::string> set;

	/**
	 * Read in indicated file
	 */
	std::ifstream inf(argv[1]);
	if (!inf.is_open()) {
		std::cout << "Error: file not opened" << std::endl;
		exit(1);
	}

	/**
	 * Iterate
	 */
	std::string line;
	std::string contents;
	while (std::getline(inf, line)) {
		std::cout << "Reading file: " << line;
		/**
		 * Read in the indicated book somehow and pass it to the
		 * 'insert' function
		 */
		contents = get_file_contents(line.c_str());
		std::cout << " (size: " << contents.size() << ")" << std::endl;
		set.insert(contents);

	}
	inf.close();

	std::cout << "Read " << set.size() << " lines" << std::endl;

	return 0;



	/*
	using V = std::vector<K>;
	using C = std::less<K>;



  int n;
  if (argc == 1) {
    n = 1000000;
  } 
  else if (argc == 2) {
    n = atoi(argv[1]);
  }
  else { 
    usage(argc, argv);
  }

  int repetitions = 32 * 1024 * 1024 / n;
  if (repetitions < 3) {
    repetitions = 3;
  }

  V* many = new V[repetitions];
  for (int k = 0; k != repetitions; ++k) {
    many[k].resize(n);
    ::generate(many[k].begin(), many[k].end());
  }

  std::clock_t start = std::clock();
  for (volatile int k = 0; k != repetitions; ++k) {
    NAME::sort(many[k].begin(), many[k].end(), C());
  }
  std::clock_t stop = std::clock();

  for (int k = 0; k != repetitions; ++k) {
    assert(std::is_sorted(many[k].begin(), many[k].end(), C()));
    many[k].clear();
  }
  delete[] many;
  
  double running_time = double(stop - start) / double(CLOCKS_PER_SEC);
  double time_per_run = 1000000000.0 * running_time / double(repetitions);
  double scaling_factor = double(n) * ilogb(double(n)); 
  double scaled_time = time_per_run / scaling_factor;
  std::cout.precision(3);
  std::cout << n << "\t" << scaled_time;
  std::cout << "\n";

  return 0;
  */
}
