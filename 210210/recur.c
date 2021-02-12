#include <stdio.h>
#include <stdlib.h>

void up_down(int n)
{
	printf("Level %d:n locatiopn %p\n", n, &n);
	if (n < 4) {
		up_down(n+1);
	}

	printf("LEVEL %d: n location %p\n ", n ,&n);

}

int main(int argc, char *argv[] )
{
	up_down(1);


}
