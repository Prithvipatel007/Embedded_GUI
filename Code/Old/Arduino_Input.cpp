#include <stdio.h>
#include <string.h>

char serialPortFilename[] = "/dev/ttyUSB0";

int main()
{
    char readBuffer[64];
    int numBytesRead;

    FILE *serPort = fopen(serialPortFilename, "rwb");

	if (serPort == NULL)
	{
		printf("ERROR");	
		return 0;
	}

	printf(serialPortFilename);
	printf(":\n");
	while(1)
	{
//	    char readBuffer[1024];
		memset(readBuffer, 0, 64);
		fread(readBuffer, sizeof(char),64,serPort);
		if(sizeof(readBuffer) != 0)
		{
			printf(readBuffer);
		}
	}
	return 0;
}