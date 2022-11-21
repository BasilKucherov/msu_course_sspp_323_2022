#include <assert.h>
#include <pthread.h>
#include <sys/time.h>
#include <iomanip>
#include <iostream>

struct thread_data {
  unsigned num_iterations;
  double x_start;
  double x_end;
  double result;
};

double calc_function(double x) {
  return 4. / (1. + x * x);
}

void* job(void* arg) {
  struct thread_data* p = (struct thread_data*)arg;
  double sum = 0;

  if (p->num_iterations != 0) {
    double x_step = (p->x_end - p->x_start) / (p->num_iterations);
    double x_current = p->x_start;

    for (int i = 0; i < p->num_iterations - 1; i++) {
      sum += calc_function(x_current + x_step / 2) * x_step;
      x_current += x_step;
    }

    x_step = p->x_end - x_current;
    sum += calc_function(x_current + x_step / 2) * x_step;
  }

  p->result = sum;
  return 0;
}

int main(int argc, char** argv) {
  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  assert(argc >= 3 &&
         "Insufficient number of parameters. Usage: ./run <number of partition "
         "intervals> <number of threads>");

  unsigned num_partition_intervals = std::stoi(argv[1]);
  unsigned num_threads = std::stoi(argv[2]);

  assert(num_threads > 0 && "Threads number must be positive");
  assert(num_partition_intervals > 0 &&
         "Partition intervals number must be positive");

  double x_start = 0;
  double x_step = 1. / num_partition_intervals;

  unsigned num_iterations = num_partition_intervals / num_threads;
  unsigned num_iterations_residue = num_partition_intervals % num_threads;

  pthread_t thrds[num_threads];
  struct thread_data data[num_threads];

  for (int i = 0; i < num_threads; i++) {
    data[i].num_iterations = num_iterations;
    data[i].result = 0;
    data[i].x_start = x_start;

    if (num_iterations_residue > 0) {
      data[i].num_iterations++;
      num_iterations_residue--;
    }

    if (i == num_threads - 1) {
      x_start = 1;
    } else {
      x_start += x_step * data[i].num_iterations;
    }

    data[i].x_end = x_start;

    assert(pthread_create(thrds + i, NULL, &job, (void*)(data + i)) == 0);
  }

  for (int i = 0; i < num_threads; i++) {
    pthread_join(thrds[i], NULL);
  }

  double my_pi = 0;
  for (int i = 0; i < num_threads; i++) {
    my_pi += data[i].result;
  }

  struct timeval end_time;
  gettimeofday(&end_time, NULL);

  double elapsed_time = end_time.tv_sec - start_time.tv_sec +
                        (end_time.tv_usec - start_time.tv_usec) / 1e6;

  std::cout << my_pi << std::endl;
  std::cout << std::fixed << std::setprecision(6)
            << "Elapsed_time: " << elapsed_time << "s" << std::endl;

  return 0;
}