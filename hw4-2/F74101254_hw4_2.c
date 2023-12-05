#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

static int *P_matrix = NULL; 
static int **m = NULL;
static int n;
static int num_threads = 0;
static int L, I, J;


void *dp_worker(void *args) {	 
    long thread_id = (long)args;
    for(int k = thread_id + I; k <= J - 1; k += num_threads){
        int result = m[I][k] + m[k + 1][J] + P_matrix[I - 1] * P_matrix[k] * P_matrix[J];
        if (result < m[I][J]){
            m[I][J] = result;
	    }    
    }
}


int main(int argc, char *argv[]){   
    if(argc != 2) exit(1);
    num_threads = atoi(argv[1]);

    char file_name[128];
    scanf("%s", file_name);
    FILE *file = fopen(file_name, "r");
    fscanf(file, "%d", &n);
    n += 1;
    P_matrix = (int *)malloc(sizeof(int) * n);
    P_matrix[0] = 1;
    for (int i = 1; i <= n; i++) {
        fscanf(file, "%d", &P_matrix[i]);
    }
    fclose(file);

    m = (int **)malloc(sizeof(int *) * n);
    for(int i = 0; i < n; i ++){
        m[i] = (int *)malloc(sizeof(int *) * n);
    }

    for(int i = 0; i < n; i ++)
	for(int j = 0; j < n; j ++)
       	    m[i][j] = 0;

    pthread_t *threads;
    threads = (pthread_t *)malloc(sizeof(pthread_t) * num_threads);

    for (L = 2; L < n; L++) {
	for (I = 1; I < n - L + 1; I++) {
	    J = I + L - 1;
            m[I][J] = INT_MAX - 1;
            for(long i = 0; i < num_threads; i ++){
                pthread_create(&threads[i], NULL, dp_worker, (void *)i);
            }
            for(int i = 0; i < num_threads; i++){
                pthread_join(threads[i], NULL);
            }
	    }
    }
    printf("%d" , m[1][n - 1]);
    return 0;
}
