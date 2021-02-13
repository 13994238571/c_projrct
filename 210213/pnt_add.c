/*指针地址*/

#include <stdio.h>
#define SIZE 4
int main(int argc, char* argv[])
{
    short dates[SIZE];
    short* pti;
    short index;
    double bills[SIZE];
    double* ptf;
    pti = dates;
    ptf = bills;
    printf("%23s %15s\n","short", "double");
    for(index = 0; index < SIZE; index++) {
        printf("pointers + %d: %10p %10p\n", index, pti + index, ptf + index);



    }
    return 0;


}

/*
                  short          double
pointers + 0: 0x7ffd89c6a408 0x7ffd89c6a410
pointers + 1: 0x7ffd89c6a40a 0x7ffd89c6a418
pointers + 2: 0x7ffd89c6a40c 0x7ffd89c6a420
pointers + 3: 0x7ffd89c6a40e 0x7ffd89c6a428
指针+1指的是加一个存储单元。对数组而言，意味着加1后的地址是下一个元素的地址。
int sum(int* ar, int n)
int sum(int*, int)
int sum(int ar[], int n)
int sun(int [], int)
ptr2 - ptr1 = 2, 意思是这两个指针所指向的两个元素相隔两个int.  而不是两个字节
*/