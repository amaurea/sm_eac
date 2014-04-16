#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define NCONT 4
#define NPER  2
char * bits[] = {"BYsSudlr","AXLR0123"};

// Pretty weird ordering. Each word is flipped, and
// the controllers are 0213.

void lprint(uint8_t * data)
{
	int i,i1,i2,j,b;
	uint8_t v;
	printf("..");
	for(i1=0;i1<2;i1++)
		for(i2=0;i2<NCONT/2;i2++)
		{
			i = i2*2+i1;
			printf("|");
			for(j=NPER-1;j>=0;j--)
			{
				v = data[NPER*i+j];
				for(b=0;b<8;b++,v<<=1)
					printf("%c",v&0x80?bits[NPER-j-1][b]:'.');
			}
		}
	printf("\n");
}

int main(int argc, char ** argv)
{
	char * ifile = argc > 1 ? argv[1] : "-";
	FILE * f = strcmp(ifile, "-") ? fopen(ifile,"r") : stdin;
	uint8_t data[NCONT*NPER];
	int i, n;
	while((n=fread(data, 1, sizeof(data), f)) > 0)
	{
		lprint(data);
		memset(data,0,sizeof(data));
	}
}
