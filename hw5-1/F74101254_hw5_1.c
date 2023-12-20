#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <omp.h>

typedef struct {
    int P;      // 計算需求
    int R;      // 釋放時間
    int W;      // 權重
} Task;


void swap(Task *a, Task *b) {
    Task temp = *a;
    *a = *b;
    *b = temp;
}

int main(int argc, char *argv[]) {
    int n;
    int minweight = 0;
    int time = 0;
    int working = 0;
    Task *taskList = NULL;

    //read file
    char file_name[128];
    scanf("%s", file_name);
    FILE *file = fopen(file_name, "r");
    fscanf(file, "%d", &n);
    taskList = (Task *)malloc(sizeof(Task) * n);
    for (int i = 0; i < n; i ++) {
        fscanf(file, "%d %d %d", &taskList[i].P, &taskList[i].R, &taskList[i].W);
    }
    fclose(file);

    while(n){
        for(int i = 0; i < n; i ++){ 
            if(time >= taskList[i].R){ 
                working = i;
                #pragma omp for   
                for(int j = 0; j < n; j ++){
                    if(i != j){
                        if(taskList[i].P * taskList[j].W > taskList[j].P * taskList[i].W && time >= taskList[j].R){
                            working = j;
                        }
                    }
                }
                if(i == working){
                    working = i;
                    break;
                }
            }
        }
        taskList[working].P --;
        time ++;

        if(!taskList[working].P){
            minweight += taskList[working].W * time;
            swap(&taskList[working], &taskList[n - 1]);
            n --; 
        } 
    }

    printf("%d", minweight);

    return 0;
}
