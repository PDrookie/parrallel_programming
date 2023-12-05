#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>

static int *At_matrix = NULL;
static int *K_matrix = NULL;
static int *At_matrix_2 = NULL;
static int t, n, m, D_1, D_2;
static int num_threads = 0;

void *matrix_multiply(void *args) {
    uint64_t thread_id = (uint64_t)args;

    for (int k = thread_id; k < n * m; k += num_threads) {
        int row = k / m;
        int col = k % m;

        int cal = 0;

        for (int i = -(D_1 - 1) / 2; i <= (D_1 - 1) / 2; i++) {
            for (int j = -(D_2 - 1) / 2; j <= (D_2 - 1) / 2; j++) {

                int At_row = row + i;
                int At_col = col + j;
                At_row = At_row >= 0 ? At_row : At_row + n;
                At_col = At_col >= 0 ? At_col : At_col + m;
                At_row %= n;
                At_col %= m;

                int K_row = ((D_1 - 1) / 2) + i;
                int K_col = ((D_2 - 1) / 2) + j;

                int K = K_matrix[K_row * D_2 + K_col];
                int At = At_matrix[At_row * m + At_col];

                cal += At * K;
            }
        }

        At_matrix_2[k] = cal / (D_1 * D_2);
    }
}

int main(int argc, char *argv[]) {

    if(argc != 2) exit(1);
    num_threads = atoi(argv[1]);

    //scan file
    char file_name[128];
    scanf("%s", file_name);
    FILE *file = fopen(file_name, "r");
    fscanf(file, "%d", &t);
    fscanf(file, "%d %d", &n, &m);

    At_matrix = (int *)malloc(sizeof(int) * n * m);
    At_matrix_2 = (int *)malloc(sizeof(int) * n * m);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            fscanf(file, "%d", &At_matrix[i * m + j]);
        }
    }

    fscanf(file, "%d %d", &D_1, &D_2);
    K_matrix = (int *)malloc(sizeof(int) * D_1 * D_2);
    for (int i = 0; i < D_1; i++) {
        for(int j = 0; j < D_2; j ++)
        fscanf(file, "%d", &K_matrix[i * D_2 + j]);
    }
    fclose(file);

    pthread_t *threads;
    threads = (pthread_t *)malloc(sizeof(pthread_t) * num_threads);

    while (t --)
    {    
        for (uint64_t i = 0; i < num_threads; i++) {
            pthread_create(&threads[i], NULL, matrix_multiply, (void *)i);
        }

        for (int i = 0; i < num_threads; i++) {
            pthread_join(threads[i], NULL);
        }
        int *temp = At_matrix;
        At_matrix = At_matrix_2;
        At_matrix_2 = temp;
    }
     

    for (int i = 0; i < n * m; i++) {
        printf("%d ", At_matrix[i]);
    }

    free(At_matrix);
    free(At_matrix_2);
    free(K_matrix);

    return 0;
}
