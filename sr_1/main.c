#include <pthread.h>
#include <assert.h>


struct thread_data
{
    unsigned thread_num;
    unsigned n_threads;
    int* arr;
    int start;
    int end;
    int result;
};


void* job(void* arg) {
    struct thread_data* p = (struct thread_data*) arg;
    int sum = 0;
    int* arr = p->arr;

    int start = p->start;
    int end = p->end;

    for(int i = start;i < end; i++)
    {
        sum += arr[i];
    }

    p->result = sum;

    return NULL;
}


int vector_sum(int *v, int size, int nthreads)
{
    pthread_t thrds[nthreads];
    struct thread_data data[nthreads];

    double sum = 0;
    int step = size / nthreads;
    int residue = size % nthreads;
    int is_residue = residue > 0;
    int start = 0, end;

    for(int i = 0; i < nthreads; i++)
    {
        data[i].n_threads = nthreads;
        data[i].thread_num = i;
        data[i].arr = v;
        data[i].result = 0;
        data[i].start = start;

        is_residue = (residue--) > 0;
        end = start + step + is_residue;
        
        data[i].end = end;
        start = end;

        assert(pthread_create(thrds + i, NULL, &job, (void*)(data + i)) == 0);
    }
  
  for (int i = 0; i < nthreads; i++) {
    pthread_join(thrds[i], NULL);
    sum += data[i].result;
  }

  return sum;
}
