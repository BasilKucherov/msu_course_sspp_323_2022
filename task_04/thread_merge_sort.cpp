#include <pthread.h>
#include <sys/time.h>
#include <iomanip>
#include <iostream>
#include <string>
#include <cstring>
#include <cassert>
#include <fstream>
#include <algorithm>

struct chunk {
  int32_t* array;
  int elements_number;
};

struct thread_data {
  struct chunk* chunks;
  int chunks_number;
  struct chunk result;
};

static inline int
min(int a, int b)
{
    return a > b ? b : a;
}

void merge(int32_t* arr_1, int arr_1_len, int32_t* arr_2, int arr_2_len, int32_t* arr_3) {
  int i = 0;
  int j = 0;

  for(;i < arr_1_len && j < arr_2_len;) {
    if (arr_1[i] < arr_2[j]) {
      arr_3[i + j] = arr_1[i];
      i++;
    } else {
      arr_3[i + j] = arr_2[j];
      j++;
    }
  }

  for(;i < arr_1_len; i++) {
      arr_3[i + j] = arr_1[i];
  }

  for(;j < arr_2_len; j++) {
      arr_3[i + j] = arr_2[j];
  }
}

void* job(void* arg) {
  struct thread_data* p = (struct thread_data*)arg;
  
  int chunks_number = p->chunks_number;
  struct chunk* chunks = p->chunks;

  if (chunks_number == 1) {
    int elements_number = chunks[0].elements_number;

    (p->result).array = new (std::nothrow) int32_t[elements_number];
    memcpy((p->result).array, chunks[0].array, elements_number * sizeof(int32_t));

    std::sort((p->result).array, (p->result).array + elements_number);
    (p->result).elements_number = elements_number;

  } else {
    int num_threads = 2;

    pthread_t thrds[num_threads];
    struct thread_data data[num_threads];

    for (int i = 0; i < num_threads; i++) {
      data[i].chunks = chunks + i * chunks_number / num_threads + min(i, chunks_number % num_threads);
      data[i].chunks_number = chunks_number / num_threads + (i < chunks_number % num_threads);

      assert(pthread_create((thrds + i), NULL, &job, (void*)(data + i)) == 0);
    }

    for(int i = 0; i < num_threads; i++) {
      pthread_join(thrds[i], NULL);
    }

    int elements_number = data[0].result.elements_number + data[1].result.elements_number;
    (p->result).array = new (std::nothrow) int32_t[elements_number];
    (p->result).elements_number = elements_number;

    merge(data[0].result.array, data[0].result.elements_number, data[1].result.array, data[1].result.elements_number, (p->result).array);
  }


  return 0;
}


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
  assert(argc >= 4 &&
         "Insufficient number of parameters. Usage: ./thread_merge_sort <in_arr_file>  <out_arr_file> <number_of_threads>");

  std::string in_arr_file = argv[1];
  std::string out_arr_file = argv[2];
  int num_threads = std::stoi(argv[3]);

  assert(num_threads > 0 && "Threads number must be positive");

  int32_t elements_number;
  int32_t* array = read_array_from_file(&elements_number, in_arr_file);
  
  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  struct chunk* chunks = new (std::nothrow) struct chunk[num_threads];

  for (int i = 0; i < num_threads; i++) {
    chunks[i].array = array + elements_number / num_threads * i + min(i, elements_number % num_threads);
    chunks[i].elements_number = elements_number / num_threads + (i < elements_number % num_threads);
  }

  pthread_t thrd;
  struct thread_data data = {chunks, num_threads};

  assert(pthread_create(&thrd, NULL, &job, (void*)(&data)) == 0);

  pthread_join(thrd, NULL);

  struct timeval end_time;
  gettimeofday(&end_time, NULL);

  double elapsed_time = end_time.tv_sec - start_time.tv_sec +
                        (end_time.tv_usec - start_time.tv_usec) / 1e6;

  std::cout << std::fixed << std::setprecision(6)
            << "Elapsed_time: " << elapsed_time << "s" << std::endl;
  
  write_array_to_file(data.result.array, data.result.elements_number, out_arr_file);

  delete[] array;
  delete[] data.result.array;
  delete[] chunks;

  return 0;
}
