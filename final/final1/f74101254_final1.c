#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Driver code
int main(int argc, char *argv[]) {
    int n = 0;
    int max = 0;
    int *array = NULL;
    int *count = NULL;
    int *output = NULL;

    char file_name[128];
    scanf("%s", file_name);
    FILE *file = fopen(file_name, "r");
    fscanf(file, "%d %d", &n, &max);
    array = (int *)malloc(sizeof(int) * n);
    count = (int *)malloc(sizeof(int) * max);
    output = (int *)malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++) {
        fscanf(file, "%d", &array[i]);
    }

    // counting sort
    for (int i = 0; i < max; i ++) count[i] = 0;

    for (int i = 0; i < n; i ++) count[array[i]] ++;

    for (int i = 1; i <= max; i ++) {
        count[i] += count[i - 1];
    }

    #pragma omp for
    for (int i = n - 1; i >= 0; i --) {
        output[count[array[i]] - 1] = array[i];
        count[array[i]]--;
    }

    // print result
    for (int i = 0; i < n; i++) {
        printf("%d ", output[i]);
    }

    return 0;
}


