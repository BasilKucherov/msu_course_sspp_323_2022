#include <cassert>
#include <fstream>
#include <papi.h>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <iomanip>

#define NUM_EVENTS 2

void
handle_error(int retval, const char *str)
{
    if (retval != PAPI_OK) {
        std::cerr << str <<  ": PAPI error " << retval << ":" << PAPI_strerror(retval) << std::endl;
        exit(1);
    }
}

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
        for (unsigned j = 0; j < columns_number; j++) {
          int32_t tmp = 0;

          for (unsigned k = 0; k < columns_number; k++)
            tmp += a[i * columns_number + k] * b[k * columns_number + j];

          c[i * columns_number + j] = tmp;
        }
      break;
    case 1:
      for (unsigned i = 0; i < columns_number; i++)
        for (unsigned k = 0; k < columns_number; k++) {
          int32_t tmp = a[i * columns_number + k];

          for (unsigned j = 0; j < columns_number; j++)
            c[i * columns_number + j] += tmp * b[k * columns_number + j];
        }
      break;
    case 2:
      for (unsigned k = 0; k < columns_number; k++)
        for (unsigned i = 0; i < columns_number; i++) {
          int32_t tmp = a[i * columns_number + k];

          for (unsigned j = 0; j < columns_number; j++)
            c[i * columns_number + j] += tmp * b[k * columns_number + j];
        }
      break;
    case 3:
      for (unsigned j = 0; j < columns_number; j++)
        for (unsigned i = 0; i < columns_number; i++) {
          int32_t tmp = c[i * columns_number + j];

          for (unsigned k = 0; k < columns_number; k++)
            tmp += a[i * columns_number + k] * b[k * columns_number + j];

          c[i * columns_number + j] = tmp;
        }
      break;
    case 4:
      for (unsigned j = 0; j < columns_number; j++)
        for (unsigned k = 0; k < columns_number; k++) {
          int32_t tmp = b[k * columns_number + j];

          for (unsigned i = 0; i < columns_number; i++)
            c[i * columns_number + j] += a[i * columns_number + k] * tmp;
        }
      break;
    case 5:
      for (unsigned k = 0; k < columns_number; k++)
        for (unsigned j = 0; j < columns_number; j++) {
          int32_t tmp = b[k * columns_number + j];

          for (unsigned i = 0; i < columns_number; i++)
            c[i * columns_number + j] += a[i * columns_number + k] * tmp;
        }
      break;
  }
}

int main(int argc, char** argv) {
  int event_set = PAPI_NULL, retval;
  long long values[NUM_EVENTS];

  assert(
      argc >= 5 &&
      "Usage: papi_matrix_mul <a_matrix> <b_matrix> <c_matrix> <mode>");

  int32_t *matrix_a, *matrix_b, *matrix_c;
  int32_t a_columns_number, b_columns_number, c_columns_number;
  int mode = std::atoi(argv[4]);

  read_matrix_from_file(&matrix_a, &a_columns_number, argv[1]);
  read_matrix_from_file(&matrix_b, &b_columns_number, argv[2]);

  assert(a_columns_number == b_columns_number &&
         "Matrices A and B have incompatible sizes");

  c_columns_number = a_columns_number;
  size_t n_elems = c_columns_number * c_columns_number;
  matrix_c = new (std::nothrow) int32_t[n_elems];

  assert(matrix_c != NULL && "ERROR: can't allocate memory for matrix C\n");

  std::memset(matrix_c, 0, n_elems * sizeof(int32_t));

  retval = PAPI_library_init(PAPI_VER_CURRENT);

  if (retval != PAPI_VER_CURRENT) {
      std::cerr << "PAPI library init error!" << std::endl;
      exit(1);
  }
  handle_error(PAPI_create_eventset(&event_set), "create eventset");

  handle_error(PAPI_add_event(event_set, PAPI_L1_DCM), "add PAPI_L1_DCM");
  handle_error(PAPI_add_event(event_set, PAPI_L2_DCM), "add PAPI_L2_DCM");
  
  handle_error(PAPI_start(event_set), "papi start");

  multiply_matrices(matrix_a, matrix_b, matrix_c, c_columns_number, mode);

  handle_error(PAPI_stop(event_set, values), "papi stop");

  std::cout << "PAPI_L1_DCM = " << values[0] << std::endl;
  std::cout << "PAPI_L2_DCM = " << values[1] << std::endl;

  write_matrix_to_file(matrix_c, c_columns_number, argv[3]);

  delete [] matrix_a;
  delete [] matrix_b;
  delete [] matrix_c;

  return 0;
}
