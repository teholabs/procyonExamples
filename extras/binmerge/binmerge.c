/*
binmerge teho Labs 2011
This program just splices two files with an offset of 0x1000
The empty space between the files is filled with 0
This allows the combination of the bootloader (bl.bin) with other code
Using a JTAG it is then possible to program the bootloader and application code in one flash
The application code must still be compiled with a linker approprate to the bootloader
*/

#include <stdio.h>
#include <stdlib.h>

#define offset 0x1000

int main(int argc, char **argv)
{

	if(argc != 4)
	{
		printf("Use: inputFile1 inputFile2 outputFile\n");
		return 0;
	}
	printf("Using offset 0x1000\n");
	*argv++;
	FILE *f1;
	FILE *f2;
	FILE *f3;
	f1 = fopen(*argv++,"r");
	f2 = fopen(*argv++,"r");
	f3 = fopen(*argv++,"w");
	int i = 0;
	int flag = 0;
	char buffer[] = "\0";

	while(fread(buffer,1,1,f1) != 0)
	{
		fwrite(buffer, 1, 1, f3);
		i++;
	}
	if(i>=offset)
	{
		printf("file 1 was too long aborting\n");
	}
	else printf("input file 1 was %d bytes long\n", i);

	buffer[0] = 0;

	while(i<offset)
	{
		fwrite(buffer, 1, 1, f3);
		i++;
	}
	i = 0;
	while(fread(buffer,1,1,f2) != 0)
	{
		fwrite(buffer, 1, 1, f3);
		i++;
	}
	printf("input file 2 was %d bytes long\n", i);
	return 0;
}
