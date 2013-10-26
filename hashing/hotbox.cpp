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
	std::cerr << "Usage: " << argv[0] << " <file> [N]" << std::endl;
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

void print_lots() {
	int j = 1;
	for (int i = 1; i < 10000; i++) {
		std::cout << '.';
		j *= i;
	}
	std::cout << j << std::endl;
}

int main(int argc, char** argv) {

	if (argc < 2)
		usage(argc, argv);

	int repetitions = 1;
	if (argc > 2)
		repetitions = std::atoi(argv[2]);

	std::vector<std::string> input;

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
	 * Read in texts
	 */
	std::string line;
	std::string contents;
	while (std::getline(inf, line))
		input.push_back(line);
	inf.close();
	
	std::cout << "Input size " << input.size() << std::endl;
	std::clock_t start, stop;
	std::vector<double> time_sums(input.size(), 0);
	
	for (std::vector<double>::iterator it = time_sums.begin(); it != time_sums.end(); it++) {
		std::cout << *it << std::endl;
	}

	for (int i=0; i < repetitions; i++) {
		set.clear();

		start = std::clock();
		for (int j = 0; j < input.size(); j++) {
			set.insert(input[j]);
			
			// Add insertion time to correct index
			//std::cout << "start: " << start/double(CLOCKS_PER_SEC) << std::endl;
			//std::cout << "end:   " << stop/double(CLOCKS_PER_SEC) << std::endl;
			time_sums[j] += double(stop-start) / double(CLOCKS_PER_SEC);
		}
		stop = std::clock();
	}

	for (std::vector<double>::iterator it = time_sums.begin(); it != time_sums.end(); it++) {
		std::cout << (*it)/(double)repetitions << std::endl;
	}

	//while (std::getline(inf, line)) {
	//	std::cout << "Reading file: " << line;
		/**
		 * Read in the indicated book somehow and pass it to the
		 * 'insert' function
		 */
	//	contents = get_file_contents(line.c_str());
	//	std::cout << " (size: " << contents.size() << ")" << std::endl;
		
	//	start = std::clock();
	//	set.insert(contents);
	//	end = std::clock();

	//}
	//inf.close();

	//std::cout << "Read " << set.size() << " lines" << std::endl;

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
