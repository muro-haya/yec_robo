#include<stdio.h>
#include<stdlib.h>

int main(void){
    int positive = 100;
    printf("positive = %d\n", positive);

    int negative = -100;
    printf("negative = %d\n", negative);
    printf("|positive| = %d, |negative| = %d\n", abs(positive), abs(negative));    

    return 0;
}