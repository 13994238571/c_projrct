/*打印每个月的天数*/
#include <stdio.h>
#define MONTHS 12
int main(int argc, char* argv[])
{
    int day[MONTHS] = {31,28,31,30,31,30,31,31,30,331,30,31,};
    int index;
    for(index = 0; index < MONTHS; index++) {
        printf("month %2d has %2d day\n", index + 1, day[index]);
<<<<<<< HEAD
        
=======
        //return 0;
>>>>>>> 16fe3beceb1e0452e1f1ec4aa5d213d3a78f601b

    }






}