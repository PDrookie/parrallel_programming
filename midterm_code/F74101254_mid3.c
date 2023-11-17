#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int cmpfunc(const void * a, const void * b){
    return ( *(int*)a - *(int*)b );
}

int main(int argc, char *argv[]){
    int n = 0;
    int password_card[70002];
    int my_size;
    int my_rank;


    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &my_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if(my_rank == 0){
        char file_name[128];
        scanf("%s", file_name);
        FILE *file = fopen(file_name, "r");
        fscanf(file, "%d", &n);
        for(int i = 0; i < n; i ++){
            fscanf(file, "%d", &password_card[i]);
        }
    }


    qsort(password_card, n, sizeof(int), cmpfunc);


    if(my_rank == 0){
        for(int i = 0; i < n; i ++){
            printf("%d ", password_card[i]);
        }
    }
    MPI_Finalize();

    return 0;
}