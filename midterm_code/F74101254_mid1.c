#include<stdio.h>

int main(int argc, char *argv[]){
    int process = 16; //assume there are 16 cores;
    double parallelize = 0.87;
    double speedup = 0;
    double answer = 0;

    speedup = parallelize + process * (1 - parallelize);
    answer = process / speedup;

    printf("%.2f", answer); 

    return 0;
}