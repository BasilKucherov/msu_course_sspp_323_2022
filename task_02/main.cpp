#include <sys/time.h>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstring>


void read_matrix_from_file(int32_t** matrix,
                           int32_t* columns_number,
                           std::string file_name) {
  std::ifstream file;
  size_t n_elems;

  file.open(file_name, std::ios::binary | std::ios::in);

  assert(file.is_open() && "ERROR: can't open file");

  file.read((char*)columns_number, sizeof(*columns_number));

  n_elems = (*columns_number) * (*columns_number);
  *matrix = new (std::nothrow) int32_t[n_elems];

  assert(*matrix != NULL && "ERROR: can't allocate memory for matrix\n");

  file.read((char*)*matrix, n_elems * sizeof(int32_t));

  file.close();
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

void multiply_matrices(int32_t* a,
                       int32_t* b,
                       int32_t* c,
                       unsigned columns_number,
                       int mode) {
  switch (mode) {
    case 0:
      for (unsigned i = 0; i < columns_number; i++)
        for (unsigned j = 0; j < columns_number; j++)
          for (unsigned k = 0; k < columns_number; k++)
            c[i * columns_number + j] +=
                a[i * columns_number + k] * b[k * columns_number + j];
      break;
    case 1:
      for (unsigned i = 0; i < columns_number; i++)
        for (unsigned k = 0; k < columns_number; k++)
          for (unsigned j = 0; j < columns_number; j++)
            c[i * columns_number + j] +=
                a[i * columns_number + k] * b[k * columns_number + j];
      break;
    case 2:
      for (unsigned k = 0; k < columns_number; k++)
        for (unsigned i = 0; i < columns_number; i++)
          for (unsigned j = 0; j < columns_number; j++)
            c[i * columns_number + j] +=
                a[i * columns_number + k] * b[k * columns_number + j];
      break;
    case 3:
      for (unsigned j = 0; j < columns_number; j++)
        for (unsigned i = 0; i < columns_number; i++)
          for (unsigned k = 0; k < columns_number; k++)
            c[i * columns_number + j] +=
                a[i * columns_number + k] * b[k * columns_number + j];
      break;
    case 4:
      for (unsigned j = 0; j < columns_number; j++)
        for (unsigned k = 0; k < columns_number; k++)
          for (unsigned i = 0; i < columns_number; i++)
            c[i * columns_number + j] +=
                a[i * columns_number + k] * b[k * columns_number + j];
      break;
    case 5:
      for (unsigned k = 0; k < columns_number; k++)
        for (unsigned j = 0; j < columns_number; j++)
          for (unsigned i = 0; i < columns_number; i++)
            c[i * columns_number + j] +=
                a[i * columns_number + k] * b[k * columns_number + j];
      break;
  }
}

int main(int argc, char** argv) {
  assert(
      argc >= 5 &&
      "Usage: generate_square_matrix <a_matrix> <b_matrix> <c_matrix> <mode>");

  int32_t *matrix_a, *matrix_b, *matrix_c;
  int32_t a_columns_number, b_columns_number, c_columns_number;
  int mode = std::atoi(argv[4]);

  read_matrix_from_file(&matrix_a, &a_columns_number, argv[1]);
  read_matrix_from_file(&matrix_b, &b_columns_number, argv[2]);

  assert(a_columns_number == b_columns_number &&
         "Matricec A and B have incompatible sizes");

  c_columns_number = a_columns_number;
  size_t n_elems = c_columns_number * c_columns_number;
  matrix_c = new (std::nothrow) int32_t[n_elems];

  assert(matrix_c != NULL && "ERROR: can't allocate memory for matrix C\n");

  std::memset(matrix_c, 0, n_elems * sizeof(int32_t));

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  multiply_matrices(matrix_a, matrix_b, matrix_c, c_columns_number, mode);

  struct timeval end_time;
  gettimeofday(&end_time, NULL);

  double elapsed_time = end_time.tv_sec - start_time.tv_sec +
                        (end_time.tv_usec - start_time.tv_usec) / 1e6;

  std::cout << std::fixed << std::setprecision(6)
            << "Elapsed_time: " << elapsed_time << "s" << std::endl;

  write_matrix_to_file(matrix_c, c_columns_number, argv[3]);

  delete[] matrix_a;
  delete[] matrix_b;
  delete[] matrix_c;

  return 0;
}
