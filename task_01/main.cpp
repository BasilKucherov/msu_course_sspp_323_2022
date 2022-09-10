#include <iostream>
#include <pthread.h>
#include <assert.h>
#include <sys/time.h>
#include <iomanip>

struct thread_data
{
    unsigned thread_num;
    unsigned num_threads;
    double num_partition_intervals;
    double x_start;
    double x_end;
    double result;
};

double calc_function(double x)
{
    return 4. / (1. + x*x);
}

void *
job(void *arg)
{
    struct thread_data *p = (struct thread_data*) arg;
    double x_step = (p->x_end - p->x_start) / (p->num_partition_intervals / p->num_threads);
    double x_current = p->x_start;
    double x_end = p->x_end;

    double sum = 0;

    while(x_current + x_step < x_end)
    {
        sum += calc_function(x_current + x_step / 2) * x_step;
        x_current += x_step;
    }

    sum += calc_function(x_current + x_end) / 2. * (x_end - x_current);
    p->result = sum;
    return 0;
}

int
main(int argc, char **argv)
{
    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    assert(argc >= 3 && "Insufficient number of parameters");
    
    unsigned num_partition_intervals = std::stoi(argv[1]);
    unsigned num_threads = std::stoi(argv[2]);

    assert(num_threads > 0 && "Threads number must be positive");
    assert(num_partition_intervals > 0 && "Partition intervals number must be positive");

    double x_start = 0;
    double x_step = 1. / num_threads;

    pthread_t thrds[num_threads];
    struct thread_data data[num_threads];

    for(int i = 0; i < num_threads; i++)
    {
        data[i].thread_num = i;
        data[i].num_threads = num_threads;
        data[i].num_partition_intervals = num_partition_intervals;
        data[i].result = 0;
        data[i].x_start = x_start;

        if(i == num_threads - 1) 
        {
            x_start = 1;
        }
        else
        {
            x_start += x_step;
        }

        data[i].x_end = x_start;

        assert(pthread_create(thrds + i, NULL,
                              &job,
                              (void *) (data + i)) == 0);
    }

    for(int i = 0; i < num_threads; i++)
    {
        pthread_join(thrds[i], NULL);
    }

    double my_pi = 0;
    for(int i = 0; i < num_threads; i++)
    {
        my_pi += data[i].result;
    }

    struct timeval end_time;
    gettimeofday(&end_time, NULL);

    double elapsed_time = end_time.tv_sec - start_time.tv_sec + (end_time.tv_usec - start_time.tv_usec) / 1e6;

    std::cout << my_pi << std::endl;
    std::cout << std::fixed << std::setprecision (6) << "Elapsed_time: " <<  elapsed_time << "s" << std::endl;

    return 0;
}