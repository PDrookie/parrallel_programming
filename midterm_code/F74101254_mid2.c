#include<stdio.h>

int main(int argc, char *argv[]){
    double serial_time = 0.00;
    double parallelize_time = 0.00;
    int process = 8;

    parallelize_time = 100 + 100 + 100 + 300 + 100 + 200 + 100 + 100;

    serial_time = parallelize_time * process;
    serial_time += 150;
    parallelize_time += 150;

    double answer = serial_time / parallelize_time;
    printf("%.2f", answer);

    return 0;
}