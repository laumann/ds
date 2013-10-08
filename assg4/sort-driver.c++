#include <algorithm>  // std::random_shuffle
#include "algorithm.i++" // K defined here
#undef NDEBUG
#include <cassert>
#include <ctime>
#include <functional> // std::less
#include <iostream>
#include <vector> // std::vector

template <typename position>
void generate(position p, position r) {
  using element = typename std::iterator_traits<position>::value_type;

  for (position q = p; q != r; ++q) {
    *q = element(q - p);
  }
  std::random_shuffle(p, r);
}

void usage(int argc, char **argv) {
  std::cerr << "Usage: " << argv[0] << " <N>" << std::endl;
  exit(1);
}

int main(int argc, char** argv) {
  using V = std::vector<K>;
  using C = std::less_equal<K>;

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
    std::cout << "[";
    V::iterator iter = many[k].begin();
    for ( ; iter != many[k].end(); ++iter) {
    	std::cout << *iter << ", ";
    }
    std::cout << "]" << std::endl;
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
}
