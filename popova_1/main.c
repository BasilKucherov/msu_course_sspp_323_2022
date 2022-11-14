#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <assert.h>

void
shift1(int rank, int np, int *x, int *y, int n)
{
    if (rank % 2 == 0) {
        MPI_Send(x, n, MPI_INT, (rank + 1) % np, 0,
                 MPI_COMM_WORLD);
        MPI_Recv(y, n, MPI_INT, (rank + np - 1) % np, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else {
        MPI_Recv(y, n, MPI_INT, (rank + np - 1) % np, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(x, n, MPI_INT, (rank + 1) % np, 0,
                 MPI_COMM_WORLD);
    }
}


void
shift2(int rank, int np, int *x, int *y, int n)
{
    MPI_Request req1, req2;
    MPI_Isend(x, n, MPI_INT, (rank + 1) % np, 0,
              MPI_COMM_WORLD, &req1);
    
    MPI_Irecv(y, n, MPI_INT, (rank + np - 1) % np, 0,
              MPI_COMM_WORLD, &req2);

    MPI_Wait(&req1, MPI_STATUS_IGNORE);
    MPI_Wait(&req2, MPI_STATUS_IGNORE);
}

void shift3(int rank, int np, int* x, int* y, int n) {
    int dim[1], period[1];
    dim[0] = np;
    period[0] = 1;
    MPI_Comm ring_comm;

    MPI_Cart_create(MPI_COMM_WORLD, 1, dim, period, 0, &ring_comm);

    int source, dest;
    MPI_Cart_shift(ring_comm, 0, 1, &source, &dest);

    MPI_Sendrecv(x, n, MPI_INT, dest, 0,
                 y, n, MPI_INT, source, 0,
                 ring_comm, NULL);
}


int
main(int argc, char **argv)
{
    assert(argc >= 3 && "Usage: ./run <arr_size> <ring_trans_type_[0-2]>");

    int np, rank;
    int *x = NULL, *y = NULL;
    int n, trans_type;

    sscanf(argv[1], "%d", &n);
    sscanf(argv[2], "%d", &trans_type);

    MPI_Init(&argc, &argv);

    x = calloc(n, sizeof(x[0]));
    y = calloc(n, sizeof(y[0]));

    assert (x != NULL && y != NULL);

    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    x[0] = rank;
    y[0] = 0;

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    switch (trans_type) {
        case 0:
            shift1(rank, np, x, y, n);
            break;
        case 1:
            shift2(rank, np, x, y, n);
            break;
        case 2:
            shift3(rank, np, x, y, n);
            break;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double end = MPI_Wtime();

    free(x);
    free(y);
    MPI_Finalize();

    if (rank == 0) {
        printf("Runtime = %f\n", end-start);
    }

    return 0;
}
