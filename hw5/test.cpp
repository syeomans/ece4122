#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <stddef.h>

typedef struct ship {
    int rankId;         // 0: Buzzy, 1-7: Yellow Jackets
    int status = 1;     // 0: crashed, 1: active, 2: docked
    float position[3];  // [x, y, z] location vector
    float velocity[3];  // [x, y, z] speed vector
    float force[3];     // [x, y, z] mass * acceleration vector
} ship;

int main(int argc, char **argv) {

    const int tag = 13;
    int size, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        fprintf(stderr,"Requires at least two processes.\n");
        exit(-1);
    }

    /* create a type for struct ship */
    const int nitems=5;
    int          blocklengths[5] = {1, 1, 3, 3, 3};
    MPI_Datatype types[5] = {MPI_INT, MPI_INT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT};
    MPI_Datatype mpi_ship_type;
    MPI_Aint     offsets[5];

    offsets[0] = offsetof(ship, rankId);
    offsets[1] = offsetof(ship, status);
    offsets[2] = offsetof(ship, position);
    offsets[3] = offsetof(ship, velocity);
    offsets[4] = offsetof(ship, force);

    // MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_ship_type);
    MPI_Type_struct(nitems, blocklengths, offsets, types, &mpi_ship_type);
    MPI_Type_commit(&mpi_ship_type);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        ship send[8];
        for (int j=0; j<8; j++)
        {
            send[j].rankId = j;
            send[j].status = j;
            for (int i=0; i<3; i++)
            {
                send[j].position[i] = 5.0 * j;
                send[j].velocity[i] = 1.4;
                send[j].force[i] = 3.1;
            }
        }

        const int dest = 1;
        MPI_Send(&send, 8, mpi_ship_type, dest, tag, MPI_COMM_WORLD);


        printf("Rank %d: sent structure ship\n", rank);
    }
    if (rank == 1) {
        MPI_Status status;
        const int src=0;

        ship recv[8];

        MPI_Recv(&recv, 8, mpi_ship_type, src, tag, MPI_COMM_WORLD, &status);
        printf("Rank %d: Received: pos = %f status = %d\n", rank,
                 recv[2].position[1], recv[2].status);
    }

    MPI_Type_free(&mpi_ship_type);
    MPI_Finalize();

    return 0;
}
