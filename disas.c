#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

typedef struct AMode { char * fmt; int n[2]; } AMode;
typedef struct OP { int name, amode, reg; } OP;

AMode amodes[] = {
	{"$%0*x",       {2,2}}, //00 absolute
	{"($%0*x,x)",   {2,2}}, //01 absolute indexed indirect
	{"$%0*x,x",     {2,2}}, //02 absolute indexed,x
	{"$%0*x,y",     {2,2}}, //03 absolute indexed,y
	{"($%0*x)",     {2,2}}, //04 absolute indirect
	{"[$%0*x]",     {3,3}}, //05 absolute indirect long
	{"$%0*x",       {3,3}}, //06 absolute long
	{"$%0*x,x",     {3,3}}, //07 absolute long indexed,x
	{"a",           {0,0}}, //08 accumulator
	{NULL,          {0,0}}, //09 accumulator (ina)
	{"$%0*x",       {2,2}}, //10 block move
	{"$%0*x",       {1,1}}, //11 direct page
	{"$%0*x,x",     {1,1}}, //12 direct page indexed,x
	{"$%0*x,y",     {1,1}}, //13 direct page indexed,y
	{"($%0*x)",     {1,1}}, //14 direct page indirect
	{"[$%0*x]",     {1,1}}, //15 direct page indirect long
	{"(%0*x,x)",    {1,1}}, //16 dp indexed indirect,x
	{"$%0*x,y",     {1,1}}, //17 dp indexed,y
	{"(%0*x),y",    {1,1}}, //18 dp indirect indexed,y
	{"[%0*x],y",    {1,1}}, //19 dp indirect long indexed,y
	{"#$%0*x",      {1,2}}, //20 immediate
	{NULL,          {0,0}}, //21 implied
	{"$%0*x",       {1,1}}, //22 program counter relative
	{"$%0*x",       {2,2}}, //23 program counter relative long
	{"($%0*x,s),y", {1,1}}, //24 sr indirect indexed y
	{"$%0*x",       {2,2}}, //25 stack (absolute)
	{"($%0*x)",     {1,1}}, //26 stack (direct page indirect)
	{"$%0*x,pc",    {2,2}}, //27 stack (pc relative long)
	{NULL,          {0,0}}, //28 stack (pull)
	{NULL,          {0,0}}, //29 stack (push)
	{NULL,          {0,0}}, //30 stack (rtl)
	{NULL,          {0,0}}, //31 stack (rts)
	{"$%0*x,s",     {1,1}}, //32 stack relative
	{NULL,          {0,0}}, //33 stack/interrupt
	{NULL,          {0,0}}, //34 stack/rti
	{NULL,          {0,0}}, //35 wdm
};

char * names[] = {
	"adc", "and", "asl", "bcc", "bcs", "beq", "bit", "bmi",
	"bne", "bpl", "bra", "brk", "brl", "bvc", "bvs", "clc",
	"cld", "cli", "clv", "cmp", "cop", "cpx", "cpy", "dec",
	"dex", "dey", "eor", "inc", "inx", "iny", "jmp", "jsr",
	"lda", "ldx", "ldy", "lsr", "mvn", "mvp", "nop", "ora",
	"pea", "pei", "per", "pha", "phb", "phd", "phk", "php",
	"phx", "phy", "pla", "plb", "pld", "plp", "plx", "ply",
	"rep", "rol", "ror", "rti", "rtl", "rts", "sbc", "sec",
	"sed", "sei", "sep", "sta", "stp", "stx", "sty", "stz",
	"tax", "tay", "tcd", "tcs", "tdc", "trb", "tsb", "tsc",
	"tsx", "txa", "txs", "txy", "tya", "tyx", "wai", "wdm",
	"xba", "xce"
};

OP ops[] = {
	{11,33,0},{39,16,0},{20,33,0},{39,32,0},{78,11,0},{39,11,0},{ 2,11,0},{39,15,0}, // 00
	{47,29,0},{39,20,1},{ 2, 8,0},{45,29,0},{78, 0,0},{39, 0,0},{ 2, 0,0},{39, 6,0},
	{ 9,22,0},{39,18,0},{39,14,0},{39,24,0},{77,11,0},{39,12,0},{ 2,12,0},{39,19,0}, // 10
	{15,21,0},{39, 3,0},{27, 9,0},{75,21,0},{77, 0,0},{39, 2,0},{ 2, 2,0},{39, 7,0},
	{31, 0,0},{ 1,16,0},{31, 6,0},{ 1,32,0},{ 6,11,0},{ 1,11,0},{57,11,0},{ 1,15,0}, // 20
	{53,28,0},{ 1,20,1},{57, 8,0},{52,28,0},{ 6, 0,0},{ 1, 0,0},{57, 0,0},{ 1, 6,0},
	{ 7,22,0},{ 1,18,0},{ 1,14,0},{ 1,24,0},{ 6,12,0},{ 1,12,0},{57,12,0},{ 1,19,0}, // 30
	{63,21,0},{ 1, 3,0},{23, 8,0},{79,21,0},{ 6, 2,0},{ 1, 2,0},{57, 2,0},{ 1, 7,0},
	{59,34,0},{26,16,0},{87,35,0},{26,32,0},{37,10,0},{26,11,0},{35,11,0},{26,15,0}, // 40
	{43,29,0},{26,20,1},{35, 8,0},{46,29,0},{30, 0,0},{26, 0,0},{35, 0,0},{26, 6,0},
	{13,22,0},{26,18,0},{26,14,0},{26,24,0},{36,10,0},{26,12,0},{35,12,0},{26,19,0}, // 50
	{17,21,0},{26, 3,0},{49,29,0},{74,21,0},{30, 6,0},{26, 2,0},{35, 2,0},{26, 7,0},
	{61,31,0},{ 0,16,0},{42,27,0},{ 0,32,0},{71,11,0},{ 0,11,0},{58,11,0},{ 0,15,0}, // 60
	{50,28,0},{ 0,20,1},{58, 8,0},{60,30,0},{30, 4,0},{ 0, 0,0},{58, 0,0},{ 0, 6,0},
	{14,22,0},{ 0,18,0},{ 0,14,0},{ 0,24,0},{71,12,0},{ 0,12,0},{58,12,0},{ 0,19,0}, // 70
	{65,21,0},{ 0, 3,0},{55,28,0},{76,21,0},{30, 1,0},{ 0, 2,0},{58, 2,0},{ 0, 7,0},
	{10,22,0},{67,16,0},{12,23,0},{67,32,0},{70,11,0},{67,11,0},{69,11,0},{67,15,0}, // 80
	{25,21,0},{ 6,20,1},{81,21,0},{44,29,0},{70, 0,0},{67, 0,0},{69, 0,0},{67, 6,0},
	{ 3,22,0},{67,18,0},{67,14,0},{67,24,0},{70,12,0},{67,12,0},{69,13,0},{67,19,0}, // 90
	{84,21,0},{67, 3,0},{82,21,0},{83,21,0},{71, 0,0},{67, 2,0},{71, 2,0},{67, 7,0},
	{34,20,2},{32,16,0},{33,20,2},{32,32,0},{34,11,0},{32,11,0},{33,11,0},{32,15,0}, // a0
	{73,21,0},{32,20,1},{72,21,0},{51,28,0},{34, 0,0},{32, 0,0},{33, 0,0},{32, 6,0},
	{ 4,22,0},{32,18,0},{32,14,0},{32,24,0},{34,12,0},{32,12,0},{33,17,0},{32,19,0}, // b0
	{18,21,0},{32, 3,0},{80,21,0},{85,21,0},{34, 2,0},{32, 2,0},{33, 3,0},{32, 7,0},
	{22,20,2},{19,16,0},{56,20,0},{19,32,0},{22,11,0},{19,11,0},{23,11,0},{19,15,0}, // c0
	{29,21,0},{19,20,1},{24,21,0},{86,21,0},{22, 0,0},{19, 0,0},{23, 0,0},{19, 6,0},
	{ 8,22,0},{19,18,0},{19,14,0},{19,24,0},{41,26,0},{19,12,0},{23,12,0},{19,19,0}, // d0
	{16,21,0},{19, 3,0},{48,29,0},{68,21,0},{30, 5,0},{19, 2,0},{23, 2,0},{19, 7,0},
	{21,20,2},{62,16,0},{66,20,0},{62,32,0},{21,11,0},{62,11,0},{27,11,0},{62,15,0}, // e0
	{28,21,0},{62,20,1},{38,21,0},{88,21,0},{21, 0,0},{62, 0,0},{27, 0,0},{62, 6,0},
	{ 5,22,0},{62,18,0},{62,14,0},{62,24,0},{40,25,0},{62,12,0},{27,12,0},{62,19,0}, // f0
	{64,21,0},{62, 3,0},{54,28,0},{89,21,0},{31, 1,0},{62, 2,0},{27, 2,0},{62, 7,0},
};

int main(int argc, char ** argv)
{
	int a = 1, x = 1, icount;
	uint8_t opcode;
	uint32_t args;
	int narg, opt;
	char * iname;
	FILE * ifile;
	OP * op;
	AMode * amode;
	while((opt=getopt(argc,argv,"xaXA")) != -1)
	{
		switch(opt) {
			case 'x': x=0; break;
			case 'a': a=0; break;
			case 'X': x=1; break;
			case 'A': a=1; break;
			default:
				fprintf(stderr, "disas [-xaXA] [file]\n");
				exit(1);
		}
	}
	iname = argc > optind ? argv[optind] : "-";
	ifile = strcmp(iname, "-") ? fopen(iname,"r") : stdin;
	for(icount=0;;icount++)
	{
		if(fread(&opcode, 1, 1, ifile) < 1) break;
		op = &ops[opcode];
		amode = &amodes[op->amode];
		narg = amode->n[!op->reg ? 0 : op->reg==1 ? a : x];
		memset(&args, 0, sizeof(args));
		if(fread(&args, 1, narg, ifile) < narg) break;
		printf("%s", names[op->name]);
		if(amode->fmt) {
			printf(" ");
			printf(amode->fmt, narg<<1, args);
		}
		if(opcode == 0xc2) { // rep
			if(args&0x10) x = 1;
			if(args&0x20) a = 1;
		}
		else if(opcode == 0xe2) { // sep
			if(args&0x10) x = 0;
			if(args&0x20) a = 0;
		}
		printf("\n");
	}
}
