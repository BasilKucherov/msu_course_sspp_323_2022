#include <time.h>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cstdint>

int32_t* generate_array(int32_t elements_number, bool use_random_seed, int random_seed) {
  int32_t* array = new (std::nothrow) int32_t[elements_number];

  assert(array != NULL && "ERROR: can't allocate memory for array\n");

  if (use_random_seed) {
    std::srand(random_seed);
  } else {
    std::srand(time(NULL));
  }

  int offset = -(RAND_MAX / 2);

  for(int32_t i = 0; i < elements_number; i++) {
    array[i] = std::rand() + offset; 
  }

  return array;
}

void write_array_to_file(int32_t* array,
                          int32_t elements_number,
                          std::string file_name) {
  std::ofstream file;

  file.open(file_name, std::ios::binary | std::ios::out);

  assert(file.is_open() && "ERROR: can't open file");

  file.write((char*)&elements_number, sizeof(elements_number));
  file.write((char*)array, elements_number * sizeof(int32_t));

  file.close();
}

int main(int argc, char** argv) {
  assert(argc >= 3 &&
         "Usage: array_generator <elements_number> <output_file_name> <optional_random_seed>");

  int32_t elements_number = std::atoi(argv[1]);
  std::string file_name = argv[2];
  bool use_random_seed = (argc > 3);
  int random_seed = use_random_seed ? (std::atoi(argv[3])) : (0);

  int32_t* array = generate_array(elements_number, use_random_seed, random_seed);

  write_array_to_file(array, elements_number, file_name);

  delete [] array;

  return 0;
}