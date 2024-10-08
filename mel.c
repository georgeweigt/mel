/* August 13, 2020

This program runs Mel Kaye's blackjack program bkjck.tx by simulating an LGP-30 computer.

bkjck.tx is read from the working directory.

The first column of output is player's hand, second column is dealer's hand.

gcc mel.c
./a.out



               A - c          6 - d
               2 - c
Card?

Enter one of the following to be dealt a card:

     yes
     ok
     si
     ja
     oui

Anything else means stand.

Press the return key 18 times to get a blackjack.

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
#include <ctype.h>

uint32_t mem[4096]; // main memory
int lowercase, col; // printer state

void run_program(int track);
uint64_t umul(uint32_t x, uint32_t y);
uint64_t udiv(uint32_t x, uint32_t y);
void print_char(int c);
uint32_t read_word(void);
void trace(int k);
void load_program(char *filename);
char *load_track(char *s);
char *load_word(char *s, uint32_t *p);
char *read_file(char *filename);

int
main(int argc, char *argv[])
{
	load_program("bkjck.tx");
	run_program(45);
}

void
run_program(int track)
{
	int addr, counter, order;
	uint32_t acc, w;

	counter = 64 * track;

	for (;;) {

		// trace(counter);

		w = mem[counter];

		order = w >> 16 & 0xf;
		addr = w >> 2 & 0xfff;

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
			acc = read_word();
			break;

		case 5: // d divide
			acc = (uint32_t) udiv(acc, mem[addr]);
			break;

		case 6: // n multiply
			acc = (uint32_t) umul(acc, mem[addr]);
			break;

		case 7: // m multiply
			acc = (uint32_t) (umul(acc, mem[addr]) >> 31);
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
			acc += mem[addr];
			break;

		case 15: // s subtract
			acc += (mem[addr] ^ 0xfffffffe) + 2;
			break;
		}
	}
}

uint64_t
umul(uint32_t x, uint32_t y)
{
	return (uint64_t) x * y;
}

uint64_t
udiv(uint32_t x, uint32_t y)
{
	return ((uint64_t) x << 31) / y;
}

int ptab[128] = {
	0,	0,	'Z',	'z',	')',	'0',	' ',	' ',
	0,	0,	'B',	'b',	'L',	'1',	'_',	'-',
	0,	0,	'Y',	'y',	'*',	'2',	'=',	'+',
	0,	0,	'R',	'r',	'"',	'3',	':',	';',
	'\n',	'\n',	'I',	'i',	0,	'4',	'?',	'/',
	'\b',	'\b',	'D',	'd',	'%',	'5',	']',	'.',
	'\t',	'\t',	'N',	'n',	'$',	'6',	'[',	',',
	0,	0,	'M',	'm',	0,	'7',	'V',	'v',
	0,	0,	'P',	'p',	0,	'8',	'O',	'o',
	0,	0,	'E',	'e',	'(',	'9',	'X',	'x',
	0,	0,	'U',	'u',	'F',	'f',	0,	0,
	0,	0,	'T',	't',	'G',	'g',	0,	0,
	0,	0,	'H',	'h',	'J',	'j',	0,	0,
	0,	0,	'C',	'c',	'K',	'k',	0,	0,
	0,	0,	'A',	'a',	'Q',	'q',	0,	0,
	0,	0,	'S',	's',	'W',	'w',	0,	0,
};

void
print_char(int c)
{
	if (c == 4) {
		lowercase = 1;
		return;
	}

	if (c == 8) {
		lowercase = 0;
		return;
	}

	c = ptab[2 * c + lowercase];

	switch (c) {
	case 0:
		break;
	case '\b':
		putchar('\b');
		if (col)
			col--;
		break;
	case '\t':
		do
			putchar(' ');
		while (++col % 15); // tab stops were set mechanically
		break;
	case '\n':
		putchar('\n');
		col = 0;
		break;
	default:
		putchar(c);
		col++;
		break;
	}
}

int rtab[128] = {
	0,	0,	0,	0,	0,	0,	0,	0,
	20,	24,	16,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	3,	0,	14,	0,	26,	22,	0,	0,
	38,	2,	10,	11,	27,	7,	23,	19,
	2,	6,	10,	14,	18,	22,	26,	30,
	34,	38,	15,	15,	0,	11,	0,	19,
	0,	57,	5,	53,	21,	37,	42,	46,
	49,	17,	50,	54,	6,	29,	25,	35,
	33,	58,	13,	61,	45,	41,	31,	62,
	39,	9,	1,	27,	0,	23,	0,	7,
	0,	57,	5,	53,	21,	37,	42,	46,
	49,	17,	50,	54,	6,	29,	25,	35,
	33,	58,	13,	61,	45,	41,	31,	62,
	39,	9,	1,	0,	0,	0,	0,	0,
};

uint32_t
read_word(void)
{
	uint32_t w;
	char buf[6], *s;

	fgets(buf, sizeof buf, stdin);

	col = 0;

	s = buf;
	w = 0;

	while (*s && *s != '\n')
		w = w << 6 | rtab[*s++ & 0x7f];

	return w;
}

char otab[16] = {
	'z','b','y','r','i','d','n','m','p','e','u','t','h','c','a','s',
};

void
trace(int k)
{
	int o, s, t;
	uint32_t w = mem[k];
	o = w >> 16 & 0xf;
	t = w >> 8 & 0x3f;
	s = w >> 2 & 0x3f;
	printf("%02d%02d %08x: %c %02d%02d\n", k >> 6, k & 0x3f, w, otab[o], t, s);
}

void
load_program(char *filename)
{
	char *buf, *s;
	buf = read_file(filename);
	if (buf == NULL)
		exit(1);
	s = buf;
	while (*s)
		s = load_track(s);
	free(buf);
}

char *
load_track(char *s)
{
	int i, k;
	uint32_t c = 0, w;

	while (isspace(*s))
		s++;

	if (*s == 0)
		return s;

	if (*s++ != 'v')
		exit(1);

	s = load_word(s, &w);

	k = 64 * (w >> 8 & 0x3f);

	for (i = 0; i < 64; i++) {

		s = load_word(s, &w);
		mem[k++] = w;

		// checksum was 29 bits instead of 30 to avoid arithmetic overflow

		c += w & 0x7ffffffe; // sign bit not protected
		c += c >> 29;
		c &= 0x3ffffffe; // 29 bits
	}

	s = load_word(s, &w);

	if (c ^ w)
		printf("checksum error\n");

	return s;
}

char *
load_word(char *s, uint32_t *p)
{
	int c;
	uint32_t w = 0;

	while (isspace(*s))
		s++;

	while (*s != '\'') {

		c = *s++;

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
			w |= 0xa;
			break;
		case 'g':
			w |= 0xb;
			break;
		case 'j':
			w |= 0xc;
			break;
		case 'k':
			w |= 0xd;
			break;
		case 'q':
			w |= 0xe;
			break;
		case 'w':
			w |= 0xf;
			break;
		default:
			exit(1);
		}
	}

	*p = w;

	return s + 1;
}

char *
read_file(char *filename)
{
	int fd, n;
	char *buf;
	off_t t;

	fd = open(filename, O_RDONLY, 0);

	if (fd < 0)
		return NULL;

	t = lseek(fd, 0, SEEK_END);

	if (t < 0 || t > 0x1000000) { // 16 MB max
		close(fd);
		return NULL;
	}

	if (lseek(fd, 0, SEEK_SET)) {
		close(fd);
		return NULL;
	}

	n = (int) t;

	buf = malloc(n + 1);

	if (buf == NULL) {
		close(fd);
		return NULL;
	}

	if (read(fd, buf, n) != n) {
		close(fd);
		free(buf);
		return NULL;
	}

	close(fd);

	buf[n] = '\0';

	return buf;
}
