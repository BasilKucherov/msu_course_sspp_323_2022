#include <time.h>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>

int32_t* generate_square_matrix(unsigned columns_number,
                                int rand_seed = -1,
                                int max = RAND_MAX,
                                int negative = true) {
  size_t n_elems = columns_number * columns_number;
  int32_t* matrix = new (std::nothrow) int32_t[n_elems];

  assert(matrix != NULL && "ERROR: can't allocate memory for matrix\n");

  if (rand_seed == -1) {
    std::srand(time(NULL));
  } else {
    std::srand(rand_seed);
  }

  int offset = (-max / 2) ? (negative) : (0);

  for (unsigned i = 0; i < columns_number; i++) {
    for (unsigned j = 0; j < columns_number; j++) {
      matrix[i * columns_number + j] = std::rand() % max + offset;
    }
  }

  return matrix;
}

void write_matrix_to_file(int32_t* matrix,
                          int32_t columns_number,
                          std::string file_name) {
  std::ofstream file;

  file.open(file_name, std::ios::binary | std::ios::out);

  assert(file.is_open() && "ERROR: can't open file");

  size_t matrix_size = columns_number * columns_number * sizeof(int32_t);

  file.write((char*)&columns_number, sizeof(columns_number));
  file.write((char*)matrix, matrix_size);

  file.close();
}

int main(int argc, char** argv) {
  assert(argc >= 3 &&
         "Usage: generate_square_matrix <columns_number> <output_file_name> "
         "<optional_rand_seed> <optional_max> <optional_negative>");

  unsigned columns_number = std::atoi(argv[1]);
  std::string file_name = argv[2];
  int rand_seed = (argc >= 4) ? (std::atoi(argv[3])) : (0);
  int max = (argc >= 5) ? (std::atoi(argv[4])) : (RAND_MAX);
  int negative = (argc >= 6) ? (std::atoi(argv[5])) : (1);

  int32_t* matrix =
      generate_square_matrix(columns_number, rand_seed, max, negative);
  write_matrix_to_file(matrix, columns_number, file_name);

  delete[] matrix;	

  return 0;
}
