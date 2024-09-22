#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>

void print_n( int16_t a ){
    printf("関数0-3 = %d\n", a);
}

int main(void){
    int16_t pattern[4][5] = {
        {1, 2, 3, 4, 5},       //パターン0
        {6, 7, 8, 9, 10},      //パターン1
        {11, 12, 13, 14, 15},  //パターン2
        {16, 17, 18, 19, 20}   //パターン3
        };

    uint16_t pattern_no = 0;

    printf("pattern = %d\n", pattern_no);

    printf("0-0 = %d\n", pattern[0][0]);
    printf("1-1 = %d\n", pattern[1][1]);
    printf("pattern_no-0 = %d\n", pattern[pattern_no][0]);
    printf("pattern_no-1 = %d\n", pattern[pattern_no][1]);
    //printf("|positive| = %d, |negative| = %d\n", abs(positive), abs(negative));    

    print_n(pattern[3][pattern_no]);

    return 0;
}