    #include<stdio.h>
    #include<stdlib.h>
    #include<string.h>
    #include<mpi.h>

    typedef struct{
        int x;
        int y;
        int id;
    } Construct;

    int cross(Construct *o, Construct *a, Construct *b){
        double ax = (a->x - o->x);
        double ay = (a->y - o->y);
        double bx = (b->x - o->x);
        double by = (b->y - o->y);
        return ax * by - ay * bx;
    }
    int square_length(Construct *a, Construct *b){
        double ax = (a->x - b->x);
        double ay = (a->y - b->y);
        return ax * ax + ay * ay;
    }

    int main(int argc, char **argv){
        int n;
        int process_rank, global_sz;
        Construct *points = NULL;
        Construct **record = NULL;
        double *costs = NULL;

        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &global_sz);
        MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

        if(process_rank == 0){
            char file_name[128];
            scanf("%s", file_name);
            FILE *file = fopen(file_name, "r");
            fscanf(file, "%d", &n);
            points = (Construct *)malloc(sizeof(Construct) * n);
            record = (Construct **)malloc(n * sizeof(Construct *) * n);
            costs = (double *)malloc(n * sizeof(double *) * n);

            for(int i = 0; i < n; i ++){
                fscanf(file, "%d %d", &points[i].x, &points[i].y);
                points[i].id = i + 1;
                if(record[0] == NULL){
                    record[0] = &points[0];
                }
                else if((points[i].x < record[0]->x) || (points[i].x == record[0]->x && points[i].y < record[0]->y)){
                    record[0] = &points[i];
                }
            }
            fclose(file);
        }


        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
        points = points ? points : (Construct *)malloc(sizeof(Construct) * n);
        MPI_Bcast(points, n * sizeof(Construct), MPI_BYTE, 0, MPI_COMM_WORLD);

        int k = 1;
        int l = 1;

        if(process_rank == 0){
            Construct *current = record[0];

            while(1){
                Construct *next = current;
                for(int j = 0; j < n; j ++){
                    double outer_product = cross(current, &points[j], next);
                    if(outer_product < 0 || (outer_product == 0 && square_length(current, &points[j]) > square_length(current, next))){
                        next = &points[j]; 
                    }
                }
                if(next->id != record[0]->id){
                    current = next;
                    record[k ++] = current;
                }
                else{ 
                    break;
                }
            }
        }

        MPI_Finalize();

        if(process_rank == 0){
            double min = 99999.99;
            for(int i = 0; i < l; i++){
                if(costs[i] < min){
                    min = costs[i];
                }
            }
            printf("%0.4f", min);

            free(points);
            free(record);
        }


        return 0;
    }