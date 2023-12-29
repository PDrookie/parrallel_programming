#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <mpi.h>

#define SOURCE 0
#define linked 2500
#define INF INT_MAX

typedef struct Node {
    int dest;
    int weight;
} Node;

int main(int argc, char** argv) {
    int n;
    int src, dest, distance;
    int process_rank, global_sz;
    int *neighbor = NULL;   /*記錄每個點與多少個點相連*/
    int *dist = NULL;       /*收納鄰近的點的距離*/
    int *visited = NULL;    /*記錄拜訪的點的值*/
    int *Index = NULL;      /*收納最鄰近的點*/
    Node *graph = NULL;     /*記錄所有資訊*/

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &global_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

    if (process_rank == SOURCE) {
        char file_name[128];
        scanf("%s", file_name);
        FILE *file = fopen(file_name, "r");
        fscanf(file, "%d", &n);
        graph = (Node *)malloc(n * linked * sizeof(Node));
        neighbor = (int *)malloc(n * sizeof(int));
        memset(neighbor, 0, n * sizeof(int));
        while (fscanf(file, "%d %d %d", &src, &dest, &distance) == 3) {
            graph[src * linked + neighbor[src]].weight = distance;
            graph[src * linked + neighbor[src]].dest = dest;
            neighbor[src] ++;
        }
        fclose(file);
    }

    MPI_Bcast(&n, 1, MPI_INT, SOURCE, MPI_COMM_WORLD);
    visited = (visited == NULL) ? ((int *)malloc(n *sizeof(int *))) : visited;
    MPI_Bcast(visited, n, MPI_INT, SOURCE, MPI_COMM_WORLD);
    graph = (graph == NULL) ? ((Node *)malloc(n * linked * sizeof(Node *))) : graph;
    MPI_Bcast(graph, n * linked * sizeof(Node *), MPI_BYTE, SOURCE, MPI_COMM_WORLD);
    neighbor = (neighbor == NULL) ? ((int *)malloc(n *sizeof(int *))) : neighbor;
    MPI_Bcast(neighbor, n, MPI_INT, SOURCE, MPI_COMM_WORLD);  

    int nlocal = (n + global_sz - 1)/global_sz;
    int start = process_rank * nlocal;
    int end = (process_rank + 1) * nlocal;

    dist = (int *)malloc(global_sz * sizeof(int));
    Index = (int *)malloc(global_sz * sizeof(int));
    visited = (int *)malloc(n * sizeof(int));

    // 計算local數據範圍
    for (int i = 0; i < n; i++) {
        visited[i] = INF; // 定義全部為尚未訪問(INF)
    }

    visited[0] = 0;

    // Dijkstra
    for (int i = 0; i < n-1; i++) {
        int near_node = 0;
        int min_dist = INF;
        
        for (int j = start; j < end && j < n; j++) {
            if(visited[j] != INF){
                for(int k = 0; k < neighbor[j]; k ++){
                    if (visited[graph[j * linked + k].dest] == INF && graph[j * linked + k].weight + visited[j] < min_dist) {
                        min_dist = graph[j * linked + k].weight + visited[j];
                        near_node = graph[j * linked + k].dest;
                    }
                }
            }
        }

        MPI_Gather(&min_dist, 1, MPI_INT, dist, 1, MPI_INT, SOURCE, MPI_COMM_WORLD);
        MPI_Gather(&near_node, 1, MPI_INT, Index, 1, MPI_INT, SOURCE, MPI_COMM_WORLD);

        min_dist = INF;
        near_node = INF;

        if(process_rank == SOURCE){
            for(int i = 0; i < global_sz; i ++){
                if(dist[i] < min_dist && dist[i] > 0){
                    min_dist = dist[i];
                    near_node = Index[i];
                }
            }
        }
        MPI_Bcast(&near_node, 1, MPI_INT, SOURCE, MPI_COMM_WORLD);
        MPI_Bcast(&min_dist, 1, MPI_INT, SOURCE, MPI_COMM_WORLD);
        visited[near_node] = min_dist;
    }


    if (process_rank == SOURCE) {
        for (int i = 0; i < n; i++) {
            printf("%d ", visited[i]);
        }
    }

    free(graph);
    free(dist);
    free(visited);
    free(Index);
    free(neighbor);

    MPI_Finalize();
    return 0;
}
