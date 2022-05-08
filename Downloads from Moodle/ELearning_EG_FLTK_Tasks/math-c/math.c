#include <stdio.h>
#include <stdlib.h>

#include <math.h>

int main(void)
{
	float x = 2.0;
	int n = 1;
	
	while (n < 10)
	{
		printf("x(%2d) = % 1.2f\n", n, x);

		x = x/2.0 + 1.0/x;
		
		n++;
	}

    return EXIT_SUCCESS;
}
