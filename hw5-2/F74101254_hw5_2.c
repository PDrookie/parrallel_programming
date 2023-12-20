#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<limits.h>
#include<omp.h>

int n;

typedef struct{
    int x;
    int y;
    int id;
    int inner;
    int midPoint;
} Construct;

double cross(Construct *o, Construct *a, Construct *b){
    double ax = (a->x - o->x);
    double ay = (a->y - o->y);
    double bx = (b->x - o->x);
    double by = (b->y - o->y);
    return ax * by - ay * bx;
}

double square_length(Construct *a, Construct *b){
    double x = (a->x - b->x);
    double y = (a->y - b->y);
    return sqrt(x * x + y * y);
}

//extracted-key
int minKey(double *key, int *mstSet, int V) {
    double min = 999;
    int min_index = 0;

    for (int v = 0; v < V; v ++) {
        if (mstSet[v] == 0 && key[v] < min) {
            min = key[v];
            min_index = v;
        }
    }
    return min_index;
}

//Prim MST
double Prim(double **graph, int V, int *a, int f) {
    int *parent = (int *)malloc(sizeof(int) * V);  
    double *key = (double *)malloc(sizeof(double) * V); ;     
    int *mstSet = (int *)malloc(sizeof(int) * V);
    double totalWeight = 0;

    for (int i = 0; i < V; i ++) {
        key[i] = INT_MAX;
        mstSet[i] = 0;
        parent[i] = -1;
    }

    if(a[0]){
        key[0] = 0;
        parent[0] = -1;
    } 
    else{
        key[f] = 0;
        parent[f] = -1;
    }

    for (int count = 0; count < V - 1; count ++) {
        int u = minKey(key, mstSet, V);
        mstSet[u] = 1;
        for (int v = 0; v < V; v ++) {
            if (graph[u][v] && mstSet[v] == 0 && graph[u][v] < key[v]) {
                parent[v] = u;
                key[v] = graph[u][v];
            }
        }
    }
    
    for (int i = 0; i < V; i++) {
      //  printf("%d ", i);
        if(parent[i] == -1)
            totalWeight += 0;
        else
            totalWeight += graph[i][parent[i]];
    }
    free(parent);
    free(key);
    free(mstSet);

    return totalWeight;
}

int main(int argc, char **argv){
    int innersize = 0;
    int hullsize = 1;
    int start = 0;
    double result = 999;
    int *able = NULL;
    double minweight;
    Construct *points = NULL;
    Construct **record = NULL;
    Construct *inner = NULL;

    //readfile
    char file_name[128];
    scanf("%s", file_name);
    FILE *file = fopen(file_name, "r");
    fscanf(file, "%d", &n);
    points = (Construct *)malloc(sizeof(Construct) * n);
    record = (Construct **)malloc(n * sizeof(Construct *) * n);
    able = (int *)malloc(sizeof(int) * n);
    for(int i = 0; i < n; i ++){
        fscanf(file, "%d %d", &points[i].x, &points[i].y);
        points[i].id = i + 1;
        points[i].inner = 1;
        if(record[0] == NULL){
            record[0] = &points[0];
        }
        else if((points[i].x < record[0]->x) || (points[i].x == record[0]->x && points[i].y < record[0]->y)){
            record[0] = &points[i];
            start = i;
        }
    }
    fclose(file);
    
    //convexhull
    points[start].inner = 0;
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
            record[hullsize ++] = current;
            current->inner = 0;
        }
        else{ 
            break;
        }
    }
    free(record);

    // record inner
    inner = (Construct *)malloc(sizeof(Construct) * (n - hullsize + 1));
    for(int i = 0; i < n; i ++){
        if(points[i].inner){
            inner[innersize] = points[i];
            inner[innersize ++].id = i;
        }
    }
    int possibility = 1;
    for(int i = 0; i < innersize; i ++){
        possibility *= 2;
    }

    //find the minweight 
    #pragma omp for
    for(int i = 0; i < possibility; i ++){
        int first_point = INT_MAX;
        int num = i;
        int bits = 0;
        //initialize
        for(int i = 0; i < innersize; i ++){
            inner[i].midPoint = 0;
        }
        //get bitmap
        while(num > 0){
            inner[bits ++].midPoint = num % 2;
            num /= 2;
        }
        //check the enable
        for(int i = 0; i < n; i ++){
            able[i] = 0;
            if(!points[i].inner){
                if(first_point > i){
                    first_point = i;
                }
                able[i] = 1;
            }
        }
        for(int i = 0; i < innersize; i ++){
            if(inner[i].midPoint){
                able[inner[i].id] = 1;
            }
        }
        //create Adj matrix
        double **graph = (double **)malloc(sizeof(double *) * n);
        for (int i = 0; i < n; i++) {
            graph[i] = (double *)malloc(sizeof(double) * n);
            for (int j = 0; j < n; j++) {
                if(able[i] == 0 || able[j] == 0 || i == j){
                    graph[i][j] = 0;
                }
                else{
                    double length = floor(square_length(&points[i], &points[j]) * 10000) / 10000;
                    graph[i][j] = length;
                }
            }
        }
        //get the weight
        minweight = Prim(graph, n, able, first_point); 

        if(result > minweight) result = minweight;

        for (int i = 0; i < n; i++) {
            free(graph[i]);
        }
        free(graph);
    }

    printf("%.4f", result);

    return 0;
}