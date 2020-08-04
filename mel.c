/* August 3, 2020

This program runs Mel Kaye's blackjack program by simulating an LGP-30.

Mel's program file bkjck.txt needs to be in the current directory.

The first column of output is player's hand, second column is dealer's hand.

Enter 'y' for a card, 'n' to hold.

% gcc mel.c
% ./a.out



	J - c	8 - c
	J - h
Card? n
		J - s
	total - 20	total - l8	score = $l.00


	4 - c	9 - h
	2 - d
Card? y
	6 - s
Card? 


BSD 2-Clause License

Copyright (c) 2020, George Weigt
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

uint32_t mem[4096];

char op[16] = {
	'z','b','y','r','i','d','n','m',
	'p','e','u','t','h','c','a','s',
};

void run_program(int track);
void print_char(int c);
uint64_t mul(uint32_t x, uint32_t y);
void load_program(void);
char *load_track(char *s);
char *load_word(char *s, uint32_t *p);
void dump_track(int n);
void stop(int line);

int
main(int argc, char **argv)
{
	load_program();
	run_program(45);
}

void
run_program(int track)
{
	int addr, counter, order;
	uint32_t acc, w;
	char buf[8];

	counter = 64 * track;

	for (;;) {

		w = mem[counter];
		order = w >> 16 & 0xf;
		addr = w >> 2 & 0xfff;

		// printf("%02d%02d %08x: %c %02d%02d\n", counter >> 6, counter & 0x3f, w, op[order], addr >> 6, addr & 0x3f);

		counter = (counter + 1) & 0xfff;

		switch (order) {

		case 0: // z stop
			break;

		case 1: // b bring from memory
			acc = mem[addr];
			break;

		case 2: // y store address
			mem[addr] = (mem[addr] & ~0x3ffc) | (acc & 0x3ffc);
			break;

		case 3: // r return address
			mem[addr] = (mem[addr] & ~0x3ffc) | (((counter + 1) & 0xfff) << 2);
			break;

		case 4: // i input
			fgets(buf, sizeof buf, stdin);
			if (buf[0] == 'y')
				acc = 25;
			else
				acc = 0;
			break;

		case 5: // d divide (mel's program does not use division)
			break;

		case 6: // n multiply
			acc = (uint32_t) mul(acc, mem[addr]);
			break;

		case 7: // m multiply
			acc = (uint32_t) (mul(acc, mem[addr]) >> 31);
			break;

		case 8: // p print
			print_char(addr >> 6);
			break;

		case 9: // e extract
			acc &= mem[addr];
			break;

		case 10: // u unconditional transfer
			counter = addr;
			break;

		case 11: // t test
			if (acc & 0x80000000)
				counter = addr;
			break;

		case 12: // h hold and store
			mem[addr] = acc & 0xfffffffe;
			break;

		case 13: // c clear and store
			mem[addr] = acc & 0xfffffffe;
			acc = 0;
			break;

		case 14: // a add
			acc = acc + mem[addr];
			break;

		case 15: // s subtract
			acc = acc + ((mem[addr] ^ 0xfffffffe) + 2);
			break;
		}
	}
}

uint64_t
mul(uint32_t x, uint32_t y)
{
	return (uint64_t) x * (uint64_t) y;
}

void
print_char(int k)
{
	static int shift = 1;

	switch (k) {

	// control codes

	case 0: // start read
		break;

	case 3: // space
		printf(" ");
		break;

	case 4: // lower case
		shift = 0;
		break;

	case 8: // upper case
		shift = 1;
		break;

	case 12: // color shift
		break;

	case 16: // carriage return
		printf("\n");
		break;

	case 20: // back space
		printf("\b");
		break;

	case 24:
		printf("\t");
		break;

	case 32: // cond stop
		break;

	case 63: // delete
		break;

	// signs

	case 7:
		if (shift)
			printf("_");
		else
			printf("-");
		break;

	case 11:
		if (shift)
			printf("=");
		else
			printf("+");
		break;

	// numerical

	case 2:
		if (shift)
			printf(")");
		else
			printf("0");
		break;

	case 6:
		if (shift)
			printf("L");
		else
			printf("l");
		break;

	case 10:
		if (shift)
			printf("*");
		else
			printf("2");
		break;

	case 14:
		if (shift)
			printf("\"");
		else
			printf("3");
		break;

	case 18:
		if (shift)
			printf("#"); // Delta
		else
			printf("4");
		break;

	case 22:
		if (shift)
			printf("%%");
		else
			printf("5");
		break;

	case 26:
		if (shift)
			printf("$");
		else
			printf("6");
		break;

	case 30:
		if (shift)
			printf("#"); // pi
		else
			printf("7");
		break;

	case 34:
		if (shift)
			printf("#"); // Sigma
		else
			printf("8");
		break;

	case 38:
		if (shift)
			printf("(");
		else
			printf("9");
		break;

	case 42:
		if (shift)
			printf("F");
		else
			printf("f");
		break;

	case 46:
		if (shift)
			printf("G");
		else
			printf("g");
		break;

	case 50:
		if (shift)
			printf("J");
		else
			printf("j");
		break;

	case 54:
		if (shift)
			printf("K");
		else
			printf("k");
		break;

	case 58:
		if (shift)
			printf("Q");
		else
			printf("q");
		break;

	case 62:
		if (shift)
			printf("W");
		else
			printf("w");
		break;

	// letters

	case 1:
		if (shift)
			printf("Z");
		else
			printf("z");
		break;

	case 5:
		if (shift)
			printf("B");
		else
			printf("b");
		break;

	case 9:
		if (shift)
			printf("Y");
		else
			printf("y");
		break;

	case 13:
		if (shift)
			printf("R");
		else
			printf("r");
		break;

	case 17:
		if (shift)
			printf("I");
		else
			printf("i");
		break;

	case 21:
		if (shift)
			printf("D");
		else
			printf("d");
		break;

	case 25:
		if (shift)
			printf("N");
		else
			printf("n");
		break;

	case 29:
		if (shift)
			printf("M");
		else
			printf("m");
		break;

	case 33:
		if (shift)
			printf("P");
		else
			printf("p");
		break;

	case 37:
		if (shift)
			printf("E");
		else
			printf("e");
		break;

	case 41:
		if (shift)
			printf("U");
		else
			printf("u");
		break;

	case 45:
		if (shift)
			printf("T");
		else
			printf("t");
		break;

	case 49:
		if (shift)
			printf("H");
		else
			printf("h");
		break;

	case 53:
		if (shift)
			printf("C");
		else
			printf("c");
		break;

	case 57:
		if (shift)
			printf("A");
		else
			printf("a");
		break;

	case 61:
		if (shift)
			printf("S");
		else
			printf("s");
		break;

	// remaining

	case 15:
		if (shift)
			printf(":");
		else
			printf(";");
		break;

	case 19:
		if (shift)
			printf("?");
		else
			printf("/");
		break;

	case 23:
		if (shift)
			printf("]");
		else
			printf(".");
		break;

	case 27:
		if (shift)
			printf("[");
		else
			printf(",");
		break;

	case 31:
		if (shift)
			printf("V");
		else
			printf("v");
		break;

	case 35:
		if (shift)
			printf("O");
		else
			printf("o");
		break;

	case 39:
		if (shift)
			printf("X");
		else
			printf("x");
		break;
	}
}

void
load_program(void)
{
	int fd, n;
	char *buf, *s;

	fd = open("bkjck.txt", O_RDONLY, 0);

	if (fd == -1) {
		printf("cannot open bkjck.txt\n");
		exit(1);
	}

	n = lseek(fd, 0, SEEK_END);

	lseek(fd, 0, SEEK_SET);

	buf = malloc(n + 1);

	if (buf == NULL)
		stop(__LINE__);

	if (read(fd, buf, n) != n)
		stop(__LINE__);

	close(fd);

	buf[n] = '\0';

	s = buf;

	while (*s)
		s = load_track(s);

	free(buf);
}

char *
load_track(char *s)
{
	int i, k, n;
	uint32_t w;

	while (*s == '\n' || *s == ' ')
		s++;

	if (*s++ != 'v')
		stop(__LINE__);

	s = load_word(s, &w);

	n = w >> 8 & 0x3f; // track number

	// printf("writing track %02d\n", n);

	k = 64 * n;

	for (i = 0; i < 64; i++) {
		s = load_word(s, &w);
		mem[k++] = w & 0xfffffffe;
	}

	s = load_word(s, &w);

	// dump_track(n);

	while (*s == '\n' || *s == ' ')
		s++;

	return s;
}

char *
load_word(char *s, uint32_t *p)
{
	int c;
	uint32_t w = 0;

	while (*s == '\n' || *s == ' ')
		s++;

	for (;;) {

		c = *s++;

		if (c == '\'')
			break;

		w <<= 4;

		switch (c) {
		case 'l': // l, not 1
			w |= 1;
			break;
		case '0':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			w |= c - '0';
			break;
		case 'f':
		case 'g':
			w |= c - 'f' + 10;
			break;
		case 'j':
		case 'k':
			w |= c - 'j' + 12;
			break;
		case 'q':
			w |= 14;
			break;
		case 'w':
			w |= 15;
			break;
		default:
			stop(__LINE__);
		}
	}

	*p = w;

	return s;
}

void
dump_track(int n)
{
	int i, k, o, s, t;
	uint32_t w;
	for (i = 0; i < 64; i++) {
		k = 64 * n + i;
		w = mem[k];
		o = w >> 16 & 0xf;
		t = w >> 8 & 0x3f;
		s = w >> 2 & 0x3f;
		printf("%02d%02d %08x: %c %02d%02d\n", k >> 6, k & 0x3f, w, op[o], t, s);
	}
}

void
stop(line)
{
	printf("see mel.c line number %d\n", line);
	exit(1);
}
