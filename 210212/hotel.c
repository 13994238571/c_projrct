/*酒店管理函数*/
#include <stdio.h>
#include <stdlib.h>
#include "hotel.h"
int menu(void)
{
	int code, status;
	printf("\n%s%s\n",STARS, STARS);
	printf("Enter the number of the desired hotel:\n");
	printf("1) Fairfield Arms   2)Hotel Olympic\n");
	printf("3) Chertworthy Plaza 4) The Stockton\n");
	printf(" %s%s\n", STARS,STARS);
    while((status = scanf("%d", &code)) != 1 || (code < 1 || code > 5))
    {
    	if (status != 1) {
    		scanf("%*s");

    	}
    	printf("Enter an integer from 1 to 5, please\n");

    }
    return code;

}

int getnights(void)
{
<<<<<<< HEAD
	int nights, ff;
	printf("How many nights are needed?\n");
	while(ff = scanf("%d", &nights) != 1) {
		scanf("%*s");
		printf("iiii");
		putchar(ff);
=======
	int nights;
	printf("How many nights are needed?\n");
	while(scanf("%d", &nights) != 1) {
		scanf("%*s");
>>>>>>> 9f40986e524c9de2b4b0103d2d92c7b0b05c87f1
		printf("Please enter an integer, such as 2\n");


	}
	return nights;

}


void showprice(double rate, int nights)
{
	int n;
	double total = 0.0;
	double factor = 1.0;
	for (n = 1; n <= nights; n++, factor *= DISCOUNT) {
		total += rate*factor;
	printf("The total cost will be $%0.2f.\n", total);


	}



}

 




