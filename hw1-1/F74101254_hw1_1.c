#include<stdio.h>
#include<mpi.h>

int main(int argc, char *argv[]){
    int n, m;
    unsigned int tests[32] = {0};
    int process_rank, global_sz;
    int total_combination = 0, local_combination = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &global_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

    if(process_rank == 0){
        char file_name[128];
        scanf("%s", file_name);
        FILE *file = fopen(file_name, "r");
        fscanf(file, "%d %d", &n, &m);
        for(int i = 0; i < m; i ++){
            int part_size, part, cost[32];
            fscanf(file, "%d %d", &part_size, &cost[i]);
            for(int j = 0; j < part_size; j ++){
                fscanf(file, "%d", &part);
                tests[i] |= 1 << (part - 1);
            }
        }
        fclose(file);
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&tests, m, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

    for(int i = process_rank; i < (1 << m); i += global_sz){
        unsigned int j = 32, covered = 0;
        while (j --){
            if(i & (1 << j)){
                covered |= tests[j];
            }
        }
        if(covered == (1 << n) - 1) local_combination ++;
    }

    MPI_Reduce(&local_combination, &total_combination, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Finalize();

    if(process_rank == 0){
        printf("%d", total_combination);
    }

    return 0;
}

