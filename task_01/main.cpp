#include <iostream>
#include <pthread.h>
#include <assert.h>
#include <sys/time.h>
#include <iomanip>


int
main(int argc, char **argv)
{
    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    assert(argc >= 3 && "Insufficient number of parameters");

    // unsigned num_partition_intervals = std::stoi(argv[1]);
    // unsigned num_threads = std::stoi(argv[1]);
    long long int sum = 0;
    for(int i = 0; i < 1000000; i++)
    {
        sum += i;
    }
    std::cout << sum << std::endl;

    struct timeval end_time;
    gettimeofday(&end_time, NULL);

    double elapsed_time = double(end_time.tv_usec - start_time.tv_usec) / 1e6;
    std::cout << std::setprecision (6) << "Elapsed_time: " <<  elapsed_time << "s" << std::endl;

    return 0;
}