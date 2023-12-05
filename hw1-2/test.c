#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

typedef struct Points
{
    int x, y;
} Construct;

bool compare(Construct *a, Construct *b){
    return (a->x < b->x) || (a->x == b->x && a->y < b->y);
}
long long cross(Construct *o, Construct *a, Construct *b){
    return (a->x - o->x) * (b->y - o->y) - (a->y - o->y) * (b->x - o->x);
}
int square_length(Construct *a, Construct *b){
    return (a->x - b->x) * (a->x - b->x) + (a->y - b->y) * (a->y - b->y);
}
bool choose_further(Construct *o, Construct *a, Construct *b){
    return square_length(o, a) > square_length(o, b);
}

int main(int argc, char *argv[]){
    int n;
    int start = 0;
    Construct a;
    a.x = -29312; 
    a.y = -23323;
    Construct b;
    b.x = -29153;
    b.y = 16842;
    Construct c;
    c.x = 25089;
    c.y = 7178;
    

    printf("%lld", cross(&a, &b, &c));

    return 0;
}
