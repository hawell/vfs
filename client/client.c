#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void save_image(const char*, unsigned char*, int);

int main()
{
	int d = open("/dev/vfs301", O_RDONLY);
	if (d == -1)
	{
		printf("error opening file\n");
		return 1;
	}

	printf("enroll your finger:\n");
	int p=0,n;
	unsigned char buff[500000];
	do
	{
		n = read(d, &buff[p], 1000);
		p += n;
	}
	while (n);

	save_image("scan1.pgm", buff, p);

	printf("one more time:\n");
	p = 0;
	do
	{
		n = read(d, &buff[p], 1000);
		p += n;
	}
	while (n);

	save_image("scan2.pgm", buff, p);

	return 0;
}

void save_image(const char* filename, unsigned char* buff, int len)
{
	FILE* img = fopen(filename, "w");
	char line[1000];
	sprintf(line, "P5  200 %d 255\n", len/288);
	fwrite(line, 1, strlen(line), img);

	int pos=0;
	//while (!(buff[pos]==0x01 && buff[pos+1]==0xfe)) pos++;
	while (pos < len && (buff[pos]==0x01 && buff[pos+1]==0xfe))
	{
		fwrite(&buff[pos+8], 1, 200, img);
		pos += 288;
	}
	fclose(img);

	for (int i=0; i<len; i++)
	{
		printf("%02x ", buff[i]);
		if (i%16 == 15) printf("\n");
	}
	printf("\n");
}
