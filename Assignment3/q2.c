#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = 100; // Size of the array
    int *array = NULL;
    int segment_size = n / size;
    int *segment = (int *)malloc(segment_size * sizeof(int));
    int partial_sum = 0, total_sum = 0;

    if (rank == 0)
    {
        array = (int *)malloc(n * sizeof(int));
        for (int i = 0; i < n; i++)
        {
            array[i] = i + 1; // Initialize array with values 1 to n
        }
    }

    MPI_Scatter(array, segment_size, MPI_INT, segment, segment_size, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < segment_size; i++)
    {
        partial_sum += segment[i];
    }

    MPI_Reduce(&partial_sum, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("Total sum: %d\n", total_sum);
        free(array);
    }

    free(segment);
    MPI_Finalize();
    return 0;
}