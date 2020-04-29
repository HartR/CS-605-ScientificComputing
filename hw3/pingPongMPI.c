#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

main(int argc, char** argv) 
{
    //initialize vars
    int process_rank;
    int mpi_tag_unused = 0;
    int loop_iter;
    double start_time, elapsed_time;
    //first input is number of iterations
    int num_iterations = atoi(argv[1]);  
    //because each int is 4 bytes, divide second input (bytes) by 4
    int msg_size = atoi(argv[2])/4; 
    int msg_array[msg_size];
    //since an int is 4 bytes, and the messages are sent twice (e.g. 8 bytes for every message) 
        //we calculate the total bandwidth with this formula
    double bandwidth = (double)msg_size * 8.0 * (double) num_iterations;

    //init MPI
    MPI_Status status;
    MPI_Comm World = MPI_COMM_WORLD;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

    //master process gets start time, sends the messages, receives them, then calculates time taken
    if (process_rank == 0)
    {   
        start_time = MPI_Wtime();
	    for (loop_iter = 0; loop_iter < num_iterations; ++loop_iter)
	    {
	        MPI_Send(msg_array, msg_size, MPI_INT, 1, mpi_tag_unused, World);
	        MPI_Recv(msg_array, msg_size, MPI_INT, 1, mpi_tag_unused, World, &status);
	    }
	    elapsed_time = MPI_Wtime() - start_time;
        //model the same output as the ivb_rc_pingpong message, for easy comparison
        //convert byte bandwidth to Mbit by multiplying with the constant below
        printf("%.0lf bytes in %.4f seconds = %.4f Mbit/sec\n", bandwidth, elapsed_time, (bandwidth*0.000008)/elapsed_time); 
        //convert seconds to microseconds by multiplying by a million
        printf("%d iters in %.4f seconds = %.4f usec/iter\n", num_iterations, elapsed_time, (elapsed_time*1000000.0)/(double)num_iterations);  
    }

    //slave process receives messages, then sends them right back
    if (process_rank == 1) 
    {
        for (loop_iter = 0; loop_iter < num_iterations; ++loop_iter)
        {   
            MPI_Recv(msg_array, msg_size, MPI_INT, 0, mpi_tag_unused, World, &status);
            MPI_Send(msg_array, msg_size, MPI_INT, 0, mpi_tag_unused, World);
        }
    }

    MPI_Finalize();
}