#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <omp.h>

#define SOURCE 0
#define linked 2500
#define INF INT_MAX

typedef struct Node {
    int dest;
    int weight;
} Node;

int main(int argc, char** argv) {
    int n;
    int distance;
    int *neighbor = NULL;   /*記錄每個點與多少個點相連*/
    int *dist = NULL;       /*收納鄰近的點的距離*/
    int *visited = NULL;    /*記錄拜訪的點的值*/
    int *Index = NULL;      /*收納最鄰近的點*/
    Node *graph = NULL;     /*記錄所有資訊*/

    char file_name[128];
    scanf("%s", file_name);
    FILE *file = fopen(file_name, "r");
    fscanf(file, "%d", &n);
    graph = (Node *)malloc(n * linked * sizeof(Node));
    neighbor = (int *)malloc(n * sizeof(int));
    memset(neighbor, 0, n * sizeof(int));
    for(int i = 0; i < n; i ++){
        for(int j = 0; j < n; j ++){
            fscanf(file, "%d", &distance);
            graph[i * linked + neighbor[i]].weight = distance;
            graph[i * linked + neighbor[i]].dest = j;
            neighbor[i] ++;
        }
    }
    fclose(file);


    dist = (int *)malloc(n * sizeof(int));
    Index = (int *)malloc(n * sizeof(int));
    visited = (int *)malloc(n * sizeof(int));

    // 計算local數據範圍
    for (int i = 0; i < n; i++) {
        visited[i] = INF; // 定義全部為尚未訪問(INF)
    }

    visited[0] = 0;

    // Dijkstra
    #pragma omp for
    for (int i = 0; i < n-1; i++) {
        int near_node = 0;
        int min_dist = INF;
        
        for (int j = 0; j < n; j ++) {
            if(visited[j] != INF){
                for(int k = 0; k < neighbor[j]; k ++){
                    if (visited[graph[j * linked + k].dest] == INF && graph[j * linked + k].weight + visited[j] < min_dist) {
                        min_dist = graph[j * linked + k].weight + visited[j];
                        near_node = graph[j * linked + k].dest;
                    }
                }
            }
        }


        for(int i = 0; i < n; i ++){
            if(dist[i] < min_dist && dist[i] > 0){
                min_dist = dist[i];
                near_node = Index[i];
            }
        }
        visited[near_node] = min_dist;
    }


    for (int i = 0; i < n; i++) {
        printf("%d ", visited[i]);
    }

    free(graph);
    free(dist);
    free(visited);
    free(Index);
    free(neighbor);

    return 0;
}
