#include<stdio.h>
#include<stdlib.h>
//#include<mpi.h>

int main(int argc, char *argv[]){
    char file_name[50];
    scanf("%s", file_name);
    FILE *file = fopen(file_name, "r");
    if(file = NULL){
        pritnf("Could not open the file %s\n", file_name);
        return 1;
    }
    int my_rank, comm_sz;
    int source;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    int n, m, k;
    int total_combination, local_combination;
    int part[32][32];
    int cost[32];
    fscanf(file, "%d %d", &n, &m);
    for(int i = 0; i < m; i ++){
        fscanf(file, "%d %d\n", &k, &cost[i]);
        for(int j = 0; j < k; j ++){
            fscanf(file, "%d", &part[i][j]);
        }
    }

    int test = (int*)malloc(sizeof(int) * n);

    if(my_rank != 0){
        MPI_send();
    }
    else{
        total_combination = local_combination;
        for(int i = 1; i <  ;i ++){
            MPI_recv();
            total_combination += local_combination;
        }
    }
    MPI_Finalize();
    return 0;

}