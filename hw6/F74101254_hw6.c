#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <math.h>

// MPI & OpenMP libraries
#include <mpi.h>
#include <omp.h>

// Parameters
#define ALPHA 1
#define BETA 1
#define BUF_MAX_SIZE 10000

typedef struct Ant {
    int length;
    int city;
    int* tour;
    bool* cities;
} Ant;

int main(int argc, char* argv[]) {
    int comm_sz = 0;
    int my_rank = 0;
    double startwtime = 0.0, endwtime = 0.0;

    // MPI init
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // setting random seed
    srand(time(NULL) + (25 * my_rank));

    // read input
    int idx = 0;
    int cities_num = 0, ants_num = 0, iteration = 0;
    int loc_best_length = INT_MAX;
    int buffer[BUF_MAX_SIZE] = {0};
    if (my_rank == 0) {
        FILE* file = fopen(argv[1], "r");
        if (!file) {
            perror("Failed to open");
            exit(EXIT_FAILURE);
        }
        fscanf(file, "%d %d %d", &cities_num, &ants_num, &iteration);
        while (fscanf(file, "%d", &buffer[idx ++]) != EOF);
        fclose(file);
    }
    MPI_Bcast(&idx, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&buffer, idx, MPI_INT, 0, MPI_COMM_WORLD);

    // Start time
    startwtime = MPI_Wtime();

    // initialization
    int* cities[cities_num];
    for(int i = 0; i < cities_num; i ++)
        cities[i] = &buffer[i * cities_num];
    
    double pheromone[cities_num][cities_num];
    for(int i = 0; i < cities_num; i ++) {
        for(int j = 0; j < cities_num; j ++)
            pheromone[i][j] = 1.0;
    }

    // get local best tour
    int loc_best_tour[cities_num + 1];
    for(int i = 0; i < cities_num + 1; i ++)
        loc_best_tour[i] = -1;

    for(int t = 0; t < iteration; t ++) {
        Ant ants[ants_num];

        #pragma omp for
        for(int i = 0; i < ants_num; i ++) {
            ants[i].length = 0;
            ants[i].city = rand() % cities_num;
            ants[i].tour = (int *) malloc(sizeof(int) * (cities_num + 1));
            ants[i].cities = (bool *) malloc(sizeof(bool) * (cities_num + 1));
            for (int j = 0; j < cities_num + 1; j ++) {
                ants[i].tour[j] = -1;
                ants[i].cities[j] = true;
            }
            ants[i].cities[ants[i].city] = false;
            ants[i].tour[0] = ants[i].city;
        }

        for(int i = 0; i < cities_num - 1; i ++) {
            #pragma omp for
            for (int k = 0; k < ants_num; k ++) {
                // choose next city
                Ant* ant = &ants[k];
                double *posibility = (double *) malloc(cities_num * sizeof(double));
                double *t_arr = (double *) malloc(cities_num * sizeof(double));
                double sum = 0.0;

                double *reciprocal_d = (double *) malloc(cities_num * sizeof(double));
                for(int j = 0; j < cities_num; j ++) {
                    if (ant->cities[j] && ant->city != j)
                        reciprocal_d[j] = 1.0 / cities[ant->city][j];
                    else
                        reciprocal_d[j] = 0;
                }

                for(int j = 0; j < cities_num; j ++) {
                    double a = 1.0, b = 1.0;
                    for(int k = 0; k < ALPHA; k ++)
                        a *= reciprocal_d[j];
                    for(int k = 0; k < BETA; k ++)
                        b *= pheromone[ant->city][j];
                    t_arr[j] = a * b;
                }

                for (int j = 0; j < cities_num; j++) {
                    if (ant->cities[j])
                        sum += t_arr[j];
                }

                // next city's probability
                for(int j = 0; j < cities_num; j ++) {
                    if (sum != 0 && ant->cities[j])
                        posibility[j] = t_arr[j] / sum;
                    else
                        posibility[j] = 0.0;
                }

                // roulette wheel selection
                int next = -1;
                double offset = 0;
                double rand_num = ((double)rand()) / (RAND_MAX + 1.0);
                for(int j = 0; j < cities_num; j ++) {
                    offset += posibility[j];
                    if(rand_num < offset) {
                        next = j;
                        break;
                    }
                }
                // no next city
                if(next != -1) {
                    ant->length += cities[ant->city][next];
                    ant->city = next;
                    ant->cities[next] = false;
                    ant->tour[i+1] = next;
                }

                free(posibility);
                free(t_arr);
                free(reciprocal_d);
            }
        }

        // back to the first city
        #pragma omp for
        for(int i = 0; i < ants_num; i ++) {
            ants[i].cities[cities_num] = false;
            ants[i].length += cities[ants[i].city][ants[i].tour[0]];
            ants[i].city = ants[i].tour[0];
            ants[i].tour[cities_num] = ants[i].tour[0];
        }

        #pragma omp for
        for(int i = 0; i < ants_num; i ++) {
            if(ants[i].length < loc_best_length) {
                #pragma omp critical
                {
                    if (ants[i].length < loc_best_length) {
                        loc_best_length = ants[i].length;
                        for (int j = 0; j < cities_num + 1; j++)
                            loc_best_tour[j] = ants[i].tour[j];
                    }
                }
            }
        }

        // update pheromone
        for(int i = 0; i < cities_num; i ++) {
            int rho = 0.25;
            for(int j = 0; j < cities_num; j ++)
                pheromone[i][j] = (1 - rho) * pheromone[i][j];
        }
        for(int i = 0; i < ants_num; i ++) {
            double q = 1000.0;
            for(int j = 0; j < cities_num; j ++) {
                if(ants[i].length != 0) {
                    int city1 = ants[i].tour[j];
                    int city2 = ants[i].tour[j+1];
                    pheromone[city1][city2] += q / ants[i].length;
                }
            }
        }
        for(int i = 0; i < ants_num; i++) {
            free(ants[i].tour);
            free(ants[i].cities);
        }
    }

    // code on ppt
    struct {
        int cost;
        int rank;
    } loc_data, global_data;

    loc_data.cost = loc_best_length;
    loc_data.rank = my_rank;

    MPI_Allreduce(&loc_data, &global_data, 1, MPI_2INT, MPI_MINLOC, MPI_COMM_WORLD);

    if(global_data.rank != 0) {
        // Receive best tour
        if(my_rank == 0)
            MPI_Recv(&loc_best_tour, cities_num + 1, MPI_INT, global_data.rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // Send best tour to process 0
        else if (my_rank == global_data.rank)
            MPI_Send(&loc_best_tour, cities_num + 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    if(my_rank == 0) {
        printf("========================================\n");
        printf("Best tour length : %d\n", loc_best_length);
        printf("The best tour : ");
        for (int i = 0; i < cities_num + 1; i++)
            printf("%d ", loc_best_tour[i]);
        printf("\n");

        // end time
        endwtime = MPI_Wtime();
        printf("The elapsed time = %lf sec\n", endwtime - startwtime);
        printf("========================================\n");
    }

    MPI_Finalize();

    return 0;
}