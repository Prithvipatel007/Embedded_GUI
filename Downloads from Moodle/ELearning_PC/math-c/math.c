#include <stdio.h>
#include <stdlib.h>

#include <math.h>

int main(void)
{
	float x = 0.0;
	int n = 1;
	
	while (n < 10)
	{
		x = pow((-1.0), (n + 1.0))/n;
		
		printf("x(%2d) = % 1.2f\n", n, x);

		n++;
	}

    return EXIT_SUCCESS;
}
