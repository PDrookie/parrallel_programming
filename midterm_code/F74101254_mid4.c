#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<mpi.h>

int matrix_element(int *matrix, int i , int j, int m, int n){
    i %= m;
    j %= n;
    i = (i < 0) ? (i + m) : i;
    j = (j < 0) ? (j + n) : j;
    return matrix[i * n + j];
}


int main(int argc, char *argv[]){
    int t;
    int n, m;
    int D_1, D_2;
    int32_t *At_matrix = NULL;
    int32_t *K_matrix = NULL;
    int32_t *At_matrix_2 = NULL;
    int process_rank, global_sz;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &global_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

    if(process_rank == 0){
        char file_name[128];
        scanf("%s", file_name);
        FILE *file = fopen(file_name, "r");
        fscanf(file, "%d", &t);
        fscanf(file, "%d %d", &n, &m);
        At_matrix = (int *)malloc(sizeof(int *) * n * m);
        At_matrix_2 = (int *)malloc(sizeof(int *) * n * m);
        for(int i = 0; i < n; i ++){
            for(int j = 0; j < m; j ++){
                fscanf(file, "%d", &At_matrix[i * m + j]);
            }
        }

        fscanf(file, "%d %d", &D_1, &D_2);
        K_matrix = (int *)malloc(sizeof(int *) * D_1 * D_2);
        for(int i = 0; i < D_1 * D_2; i ++){
                fscanf(file, "%d", &K_matrix[i]);
            }

        fclose(file);
    }

    MPI_Bcast(&t, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&D_1, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&D_2, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int local_sz = (m * n) % global_sz ? (m * n) + (global_sz - ((m * n) % global_sz)) : (m * n);

    K_matrix = (K_matrix == NULL) ? ((int *)malloc(sizeof(int *) * D_1 * D_2)) : K_matrix;
    MPI_Bcast(K_matrix, D_1 * D_2, MPI_INT, 0, MPI_COMM_WORLD);

    At_matrix = (At_matrix == NULL) ? ((int *)malloc(sizeof(int *) * local_sz)) : At_matrix;
    At_matrix_2 = (At_matrix_2 == NULL) ? ((int *)malloc(sizeof(int *) * local_sz)) : At_matrix_2;
    MPI_Bcast(At_matrix, m * n, MPI_INT, 0, MPI_COMM_WORLD);

    while(t --){
        int start = process_rank * (local_sz / global_sz);
        int end = (process_rank + 1) * (local_sz / global_sz);
        
        for(int k = start; k < end; k ++){
            int row = k / m;
            int col = k % m;

            int32_t cal = 0;

            for(int32_t i = -(D_1 - 1) / 2; i <= (D_1 - 1) / 2 ; i ++){
                for(int32_t j = -(D_2 - 1) / 2; j <= (D_2 - 1) / 2; j ++){
                    cal += matrix_element(K_matrix, ((D_1 - 1) / 2) + i, ((D_2 - 1) / 2) + j, D_1, D_2) * 
                          matrix_element(At_matrix, row + i, col + j, n, m);
                }
            }
            At_matrix_2[row * m + col] = cal / (D_1 * D_2);
        }

        MPI_Allgather(&At_matrix_2[start], end - start, MPI_INT, At_matrix, end - start, MPI_INT, MPI_COMM_WORLD);
    }

    if(process_rank == 0){
        for(size_t i = 0; i < n; i ++){
            for(size_t j = 0; j < n; j ++){
                printf("%d ", matrix_element(At_matrix, i, j, n, m));
            }
        }
    }

    MPI_Finalize();
    return 0;
}