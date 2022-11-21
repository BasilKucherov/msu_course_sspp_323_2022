void
my_bcast(void *buffer,
         int count,
         MPI_Datatype datatype,
         int root,
         MPI_Comm comm)
{

    int rank, np;
    MPI_Comm_size(comm, &np);
    MPI_Comm_rank(comm, &rank);

    // Посылаем в 0, с него начинается массовая рассылка
    if (rank == root && root != 0) {
        MPI_Send(buffer, count, datatype,
                    0, 0, comm);
        }
    

    if (rank == 0 && rank != root) {
        MPI_Recv(buffer, count, datatype,
                    root, 0, comm, MPI_STATUS_IGNORE);
    }

    int iterations = (int) log2(np) + 1;
    
    int senders = 1;
    int recievers = senders * 2;
	
    int step = 1;
    int dest = rank + 1;

    for (int i = 0; i < iterations; i++) {
        if (rank < senders && dest < np) {
            MPI_Send(buffer, count, datatype,
                        dest, 0, comm);
        } else if (rank >= senders && rank < recievers) {
            MPI_Recv(buffer, count, datatype,
                    rank - (1 << i), 0, comm, MPI_STATUS_IGNORE);
        }
        
        dest += step;
        step *= 2;
        senders *= 2;
        recievers *= 2;
    }
} 
