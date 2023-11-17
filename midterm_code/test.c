#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int merge(int *ina, int lena, int *inb, int lenb, int *out){
    int i, j;
    int outcount = 0;
    
    for(i = 0, j = 0; i < lena; i ++){
        while ((inb[j] < ina[i]) && j < lenb){
            out[outcount ++] = inb[j ++];
        }
        out[outcount ++] = ina[i];
    }
    while(j < lenb){
        out[outcount ++] = inb[j ++];
    }
    return 0;
}

int domerge_sort(int *a, int start, int end, int *b){
    if((end - start) <= 1)
        return 0;
    
    int mid = (end + start) / 2;
    domerge_sort(a, start, mid, b);
    domerge_sort(a, mid, end, b);
    merge(&(a[start]), mid - start, &(a[mid]), end - mid, &(b[start]));
    for(int i = start; i < end; i ++){
        a[i] = b[i];
    }
    return 0;
}

int merge_sort(int n, int *a){
    int b[n];
    domerge_sort(a, 0, n, b);
    return 0;
}

void MPI_Pairwise_Exchange(int localn, int *locala, int sendrank, int recvrank, MPI_Comm comm){
    int rank;
    int remote[localn];
    int all[2 * localn];
    const int mergetag = 1;
    const int sortedtag = 2;


    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    if(rank == sendrank){
        MPI_Send(locala, localn, MPI_DOUBLE, recvrank, mergetag, MPI_COMM_WORLD);
        MPI_Recv(locala, localn, MPI_DOUBLE, recvrank, sortedtag, MPI_COMM_WORLD, MPI_SOURCE_IGNORE);
    }
    else{
        MPI_Recv(locala, localn, MPI_DOUBLE, recvrank, mergetag, MPI_COMM_WORLD, MPI_SOURCE_IGNORE);
        merge(locala, localn, localn, all);
    }


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