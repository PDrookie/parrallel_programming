#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <omp.h>

#define Max 2001

int main(int argc, char **argv) {
    int n;
    int min_index;
    short Adj_matrix[Max][Max];
    int *visited = NULL; 
    int *dist = NULL;       

    // initialize graph
    for (int i = 0; i < Max; i++) {
        for (int j = 0; j < Max; j++) {
            Adj_matrix[i][j] = 0; 
        }
    }

    // read file
    char file_name[128];
    scanf("%s", file_name);
    FILE *file = fopen(file_name, "r");
    fscanf(file, "%d", &n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            fscanf(file, "%hd", &Adj_matrix[i][j]);
        }
    }
    fclose(file);

    dist = (int *)malloc(n * sizeof(int));
    visited = (int *)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        dist[i] = INT_MAX; 
        visited[i] = 0;
    }

    dist[0] = 0;

    // Dijkstra
    #pragma omp for
    for (int count = 0; count < n - 1; count ++) {
        int near_node = 0;
        int min_dist = INT_MAX;
        
        for (int j = 0; j < n; j ++) {
            if(!visited[j] && dist[j] <= min_dist){
                min_dist = dist[j];
                min_index = j;
            }
        }

        visited[min_index] = 1;

        for(int k = 0; k < n; k ++){
            if (!visited[k] && Adj_matrix[min_index][k] && dist[min_index] != INT_MAX 
                && dist[min_index] + Adj_matrix[min_index][k] < dist[k]) {
                dist[k] = dist[min_index] + Adj_matrix[min_index][k];
            }
        }
    }

    // print result
    for (int i = 0; i < n; i++) {
        printf("%d ", dist[i]);
    }

    // free memory
    free(dist);
    free(visited);  

    return 0;
}
