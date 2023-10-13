/* MPIdijk.c
* The program that does MPI Dijkstra’s Algorithm. */
#include <stdlib.h> 
#include <stdio.h> 
#include <string.h> 
#include <math.h> 
#include <sys/time.h>
#include <mpi.h>
#define N 2048
#define SOURCE 1
#define MAXINT 9999999

/*  single source Dijkstra's Algorithm  */ 
/*  @param n: number of vertices;
    @param source: rank of the root
    @param wgt: points to locally stored portion of the weight adjacency matrix of the graph; @param lengths: points to a vector that will store the distance of the shortest paths from the source to the locally stored vertices; 
*/
void SingleSource(int n, int source, int *wgt, int *lengths, MPI_Comm comm) { 
    int temp[N];
    int i, j;
    int nlocal;     /* The number of vertices stored locally */
    int *marker;    /* Used to mark the vertices belonging to Vo */
    int firstvtx;   /* The index number of the first vertex that is stored locally */ 
    int lastvtx;    /* The index number of the last vertex that is stored locally */ 
    int u, udist;
    int lminpair[2], gminpair[2];
    int npes, myrank;
    
    MPI_Status status;
    MPI_Comm_size(comm, &npes); 
    MPI_Comm_rank(comm, &myrank);

    nlocal = n / npes;
    firstvtx = myrank*nlocal;
    lastvtx = firstvtx + nlocal - 1;

    /* Set the initial distances from source to all the other vertices */ 
    for (j = 0; j<nlocal; j++) {
        lengths[j] = wgt[source*nlocal + j]; 
    }
    /* This array is used to indicate if the shortest part to a vertex has been found or not. */ 
    /* if marker [v] is one, then the shortest path to v has been found, otherwise it has not. */ 
    marker = (int *)malloc(nlocal*sizeof(int));
    for (j = 0; j<nlocal; j++) {
        marker[j] = 1; 
    }
    /* The process that stores the source vertex, marks it as being seen */ 
    if (source >= firstvtx && source <= lastvtx) {
        marker[source - firstvtx] = 0; 
    }
    /* The main loop of Dijkstra's algorithm */ 
    for (i = 1; i<n; i++) {
        /* Step 1: Find the local vertex that is at the smallest distance from source */
        lminpair[0] = MAXINT; /* set it to an architecture dependent large number */
        lminpair[1] = -1;
        for (j = 0; j<nlocal; j++) {
            if (marker[j] && lengths[j] < lminpair[0]) { 
                lminpair[0] = lengths[j];
                lminpair[1] = firstvtx + j; 
            }
        }

        /* Step 2: Compute the global minimum vertex, and insert it into Vc */ 
        MPI_Allreduce(lminpair, gminpair, 1, MPI_2INT, MPI_MINLOC, comm); 
        udist = gminpair[0];
        u = gminpair[1];

        /* The process that stores the minimum vertex, marks it as being seen */ 
        if (u == lminpair[1]) {
            marker[u - firstvtx] = 0; 
        }

        /* Step 3: Update the distances given that u got inserted */ 
        for (j = 0; j<nlocal; j++) {
            if (marker[j] && ((udist + wgt[u*nlocal + j]) < lengths[j])) { 
                lengths[j] = udist + wgt[u*nlocal + j];
            } 
        }
    }
    free(marker); 
}
int main(int argc, char *argv[]) {
    int npes, myrank, nlocal;
    int weight[N][N]; /*adjacency matrix*/
    int distance[N]; /*distance vector*/
    int *localWeight; /*local weight array*/
    int *localDistance; /*local distance vector*/
    int sendbuf[N*N]; /*local weight to distribute*/ int i, j, k;
    char fn[255];
    FILE *fp;
    double time_start, time_end;
    struct timeval tv;
    struct timezone tz;

    gettimeofday(&tv, &tz);
    time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.00;

    /* Initialize MPI and get system information */ 
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &npes); 
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    nlocal = N/npes; /* Compute the number of elements to be stored locally. */

    /*allocate local weight and local disatance arrays for each prosess*/ 
    localWeight = (int *)malloc(nlocal*N*sizeof(int));
    localDistance = (int *)malloc(nlocal*sizeof(int));

    /* Open input file, read adjacency matrix and prepare for sendbuf */ 
    if (myrank == SOURCE) {
        strcpy(fn,"input2048.txt");
        fp = fopen(fn,"r");
        if ((fp = fopen(fn,"r")) == NULL) {
            printf("Can't open the input file: %s\n\n", fn);
            exit(1); 
        }
        //printf("\nThe adjacency matrix: \n"); 
        for(i = 0; i < N; i++) {
            for(j = 0; j < N; j++) {
                fscanf(fp,"%d", &weight[i][j]);
                // if (weight[i][j] == 9999999) printf("%4s", "INT"); 
                // else printf("%4d", weight[i][j]);
            }
            // printf("\n"); 
        }

        /*prepare send data */ 
        for(k=0; k<npes; ++k) {
            for(i=0; i<N;++i) {
                for(j=0; j<nlocal;++j) {
                    sendbuf[k*N*nlocal+i*nlocal+j]=weight[i][k*nlocal+j]; 
                } 
            }
        }
        
    }

    /*distribute data*/
    MPI_Scatter(sendbuf, nlocal*N, MPI_INT, localWeight, nlocal*N, MPI_INT, SOURCE, MPI_COMM_WORLD);
    /*Implement the single source dijkstra's algorithm*/
    SingleSource(N, SOURCE, localWeight, localDistance, MPI_COMM_WORLD);
    /*collect local distance vector at the source process*/
    MPI_Gather(localDistance, nlocal, MPI_INT, distance, nlocal, MPI_INT, SOURCE, MPI_COMM_WORLD);

    if (myrank == SOURCE) { 
        printf("Nodes: %d\n", N);
        //printf("The distance vector is \n"); 
        //for (i = 0; i < N; ++i) {
        // printf("%d ", distance[i]); 
        // }
        // printf("\n");
        gettimeofday(&tv, &tz);
        time_end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.00; 
        printf("time cost is %1f\n", time_end - time_start);
    } 
    free(localWeight); 
    free(localDistance); 
    MPI_Finalize(); 
    
    return 0;
}