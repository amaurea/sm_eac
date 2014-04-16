#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* Produces assembly code for writing the input
 * machine code to the buffer starting at the indicated address.
 * An even number of bytes will be written, rounding up.
 * The nlag argument indicates the number of lag frames to insert
 * after each normal frame */

void lagframe(FILE * f) {
	int i;
	fprintf(f, ";Lag frame\n");
	for(i=0;i<8;i++) fprintf(f, "brk\n");
	fprintf(f, "\n");
}

void putword(FILE * f, uint16_t addr, uint16_t val)
{
	fprintf(f, "lda #$%04x\nsta $%04x\nrts\nbrk\n\n", val, addr);
}

int main(int argc, char ** argv)
{
	uint16_t tbuffer = strtol(argv[1], NULL, 0);
	uint16_t nlag    = strtol(argv[2], NULL, 0);
	char * iname = argc > 3 ? argv[3] : "-";
	FILE * ifile = strcmp(iname,"-") ? fopen(iname,"r") : stdin;
	char * oname = argc > 4 ? argv[4] : "-";
	FILE * ofile = strcmp(oname,"-") ? fopen(oname,"w") : stdout;
	int i, j, n;
	uint16_t data;
	for(i=0;fread(&data, 2, 1, ifile)>0;i+=2)
	{
		fprintf(ofile, ";Byte %x-%x\n", i, i+1);
		putword(ofile, tbuffer+i, data);
		for(j=0;j<nlag;j++) lagframe(ofile);
	}
}
