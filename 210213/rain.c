/*
float rain[5][12];内含5个数组元素的数组，每个数组元素内含12个float类型的元素
rain是一个内含5个元素的数组，每个元素的类型都是float[12]，也就是说，rain的每个元素本身都含有12个float类型的数组。
rain[0][0]是首元素，第二个元素是rain[0][1].

*/

#include <stdio.h>
#define MONTHS 12
#define YEARS 5
int main(int argc, char* argv[])
{
    const float rain[YEARS][MONTHS] = {
        {4.3, 4.3, 4.3, 3.5, 5.7, 3.9, 3.8, 7.6, 8.4, 2.5, 3.7, 5.6},
        {5.6, 5.8, 4.8, 7.6, 2.8, 4.9, 5.9, 8.4, 4.7, 4.3, 89.5, 4.6},
        {5.3, 2.7, 6.4, 3.7, 5.98, 7.65, 8.5, 5.7, 87.6, 5.6, 7.7, 5.9},
        {5.6, 5.8, 4.8, 7.6, 2.8, 4.9, 5.9, 8.4, 4.7, 4.3, 89.5, 4.6},
        {5.3, 2.7, 6.4, 3.7, 5.98, 7.65, 8.5, 5.7, 87.6, 5.6, 7.7, 5.9},
    };
    int year, month;
    float subtot, total;
    printf("YEAR RAINFALL (inches)\n");
    for(year = 0, total = 0; year < YEARS; year++ ) {
        for(month = 0, subtot = 0; month < MONTHS; month++) {
            subtot += rain[year][month];
            //printf("%5d %15.3f\n", 2010 + year, subtot);
        }
        printf("%5d %15.3f\n", 2010 + year, subtot);
    }



}