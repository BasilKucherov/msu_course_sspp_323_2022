#include <iostream>
#include <iomanip>
#include <fstream>
#include <cassert>

int32_t* read_array_from_file(int32_t* elements_number,
                              std::string file_name) {
  std::ifstream file;

  file.open(file_name, std::ios::binary | std::ios::in);

  assert(file.is_open() && "ERROR: can't open file");

  file.read((char*)elements_number, sizeof(*elements_number));
  
  int32_t* array = new (std::nothrow) int32_t[*elements_number];

  assert(array != NULL && "ERROR: can't allocate memory for array\n");
  
  file.read((char*)array, (*elements_number) * sizeof(int32_t));

  file.close();

  return array;
}

bool is_sorted(int32_t* array, int32_t elements_number, bool* ascending) {
    if(elements_number >= 2) {
      *ascending = array[1] > array[0];
    }

    for(int32_t i = 1; i < elements_number; i++) {
        if (ascending ? (array[i] < array[i-1]) : (array[i] > array[i-1])) {
            return false;
        }
    }

    return true;
}

int main(int argc, char** argv) {
  assert(argc >= 2 &&
         "Usage: is_sorted <file_name>");

  std::string file_name = argv[1];

  int32_t elements_number;
  int32_t* array = read_array_from_file(&elements_number, file_name);
  
  bool ascending;
  if (is_sorted(array, elements_number, &ascending)) {
    std::cout << "Sorted: " << (ascending ? "ascending" : "descending") << std::endl;
  } else {
    std::cout << "Not sorted" << std::endl;
  }

  delete [] array;

  return 0;
}