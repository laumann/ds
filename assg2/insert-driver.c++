#include <ctime>
#include "data-structure.i++" // S defined here
#include <iostream>

void usage(int argc, char **argv) {
  std::cerr << "Usage: " << argv[0] << " <N>" << std::endl;
  exit(1);
}

int main(int argc, char** argv) {
  using K = typename S::key_type;

  int number_of_elements;
  if (argc == 1) {
    number_of_elements = 1000000;
  } 
  else if (argc == 2) {
    number_of_elements = atoi(argv[1]);
  }
  else { 
    usage(argc, argv);
  }
  int repetitions = 10000000 / number_of_elements;
  if (repetitions < 3) {
    repetitions = 3;
  }

  S* many = new S[repetitions];
  std::clock_t start = std::clock();
  for (int k = 0; k != repetitions; ++k) {
    for (int i = 0; i != number_of_elements; ++i) {
      (void) many[k].insert(K(i));
    }
  }
  std::clock_t stop = std::clock();

  for (int k = 0; k != repetitions; ++k) {
    many[k].clear();
  }
  delete[] many;
  
  double running_time = double(stop - start) / double(CLOCKS_PER_SEC);
  double time_per_run = 1000000000.0 * running_time / double(repetitions);
  double time_per_operation = time_per_run / double(number_of_elements);
  std::cout.precision(1);
  std::cout << std::fixed << number_of_elements << "\t" << time_per_operation;
  std::cout << "\n";

  return 0;
}
