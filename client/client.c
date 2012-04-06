#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
	int d = open("/dev/vfs301", O_RDONLY);
	if (d == -1)
	{
		printf("error opening file\n");
		return 1;
	}

	int p=0,n;
	unsigned char buff[200000];
	do
	{
		n = read(d, &buff[p], 1000);
		p += n;
	}
	while (n == 1000);
	for (int i=0; i<p; i++)
	{
		printf("%x ", buff[i]);
		if (i%16 == 15) printf("\n");
	}
	printf("\n");
	return 0;
}
