#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define NPER  2
#define NCONT 4

int main(int argc, char ** argv)
{
	char * ifile = argc > 1 ? argv[1] : "-";
	char * ofile = argc > 2 ? argv[2] : "-";
	char * line;
	FILE * f  = strcmp(ifile, "-") ? fopen(ifile,"r") : stdin;
	FILE * of = strcmp(ofile, "-") ? fopen(ofile,"w") : stdout;
	int i, i1, i2, i3,j;
	size_t len = 0;
	ssize_t nread;
	uint8_t data[NCONT/2][2][NPER];
	while((nread=getline(&line, &len, f)) != -1)
	{
		// Read a full record
		memset(data,0,sizeof(data));
		for(i1=0;i1<NCONT/2;i1++)
			for(i2=0;i2<2;i2++)
				for(i3=0;i3<NPER;i3++)
					for(i=0;i<8;i++)
					{
						j=3+i1*2+i2+((i1*2+i2)*NPER+i3)*8+i;
						data[i1][i2][i3] <<= 1;
						//printf("%d %d %d %d %2d %c %02x",i1,i2,i3,i,j,line[j],data[i1][i2][i3]);
						if(line[j] != '.')
							data[i1][i2][i3] |= 1;
						//printf(" %02x\n", data[i1][i2][i3]);
					}
		// Output after taking into account the different byte ordering
		for(i2=0;i2<2;i2++)
			for(i1=0;i1<NCONT/2;i1++)
				for(i3=NPER-1;i3>=0;i3--)
					fwrite(&data[i1][i2][i3], 1, 1, of);
	}
}
