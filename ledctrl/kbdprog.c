
#include <string.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "defs.h"
#include "ledprog.h"

/*
 * next neighbour relations
 *
 * each key has:
 *	- top right
 *	- right
 *	- bottom right
 *	- bottom left
 *	- left
 *	- top left
 *
 * except for keypad where it is 
 *	- top
 *	- right
 * 	- 
 *	- bottom
 *	- left
 *	-
 */

static const unsigned char keyrels[MAXLED][6] = {
	{  	-1,	8, 	16,	72,	65, 	-1	},	// 0 !
	{	-1,	9,	17,	24,	8,	-1	},	// 1 #
	{	-1,	10,	18,	25,	9,	-1	},	// 2 %
	{	-1,	11,	19,	26,	10,	-1	},	// 3 &
	{	-1,	12,	20,	27,	11,	-1	},	// 4 (
	{	-1,	73,	21,	28,	12,	-1	},	// 5 left arrow
	{	-1,	14,	-1,	22,	-1,	-1	},	// 6 home
	{	-1, 	15,	-1,	23,	14,	-1	},	// 7 crsr right
	{	-1,	1,	24,	16,	0,	-1	},	// 8 "
	{	-1,	2,	25,	17,	1,	-1	},	// 9 $
	{	-1,	3,	26,	18,	2,	-1	},	// 10 '
	{	-1,	4,	27,	19,	3,	-1	},	// 11 backslash
	{	-1,	5,	28,	20,	4,	-1	},	// 12 )
	{	56,	74,	-1,	-1,	-1,	48	},	// 13 ctrl
	{	-1,	7,	-1,	30,	6,	-1	},	// 14 crsr down
	{	-1,	-1,	-1,	31,	7,	-1	},	// 15 del
	{	8,	24,	32, -1/*67*/,	72,	0	},	// 16 q
	{	9,	25,	33,	40,	24,	1	},	// 17 e
	{	10,	26,	34,	41,	25,	2	},	// 18 t
	{	11,	27,	35,	42,	26,	3	},	// 19 u
	{	12,	28,	36,	43,	27,	4	},	// 20 o
	{	73,	75,	76,	44,	28,	5	},	// 21 up arrow
	{	6,	30,	-1,	38,	-1,	-1	},	// 22 7
	{	7,	31,	-1,	39,	30,	-1	},	// 23 9
	{	1,	17,	40,	32,	16,	8	},	// 24 w
	{	2,	18,	41,	33,	17,	9	},	// 25 r
	{	3,	19,	42,	34,	18,	10	},	// 26 y
	{	4,	20,	43,	35,	19,	11	},	// 27 i
	{	5,	21,	44,	36,	20,	12	},	// 28 p
	{	-1,	-1,	-1,	-1,	-1,	-1	},	// 29 
	{	14,	23,	-1,	46,	22,	-1	},	// 30 8
	{	15,	-1,	-1,	47,	39,	-1	},	// 31 /
	{	24,	40,	48,	64, -1/*67*/,	16	},	// 32 a
	{	25,	41,	49,	56,	40,	17	},	// 33 d
	{	26,	42,	50,	57,	41,	18	},	// 34 g
	{	27,	43,	51,	58,	42,	19	},	// 35 j
	{	28,	44,	52,	59,	43,	20	},	// 36 l
	{	-1,	-1,	-1,	-1,	-1,	-1	},	// 37
	{	22,	46,	-1,	54,	-1,	-1	},	// 38 4
	{	23,	47,	-1,	55,	46,	-1	},	// 39 6
	{	17,	33,	56,	48,	32,	24	},	// 40 s
	{	18,	34,	57,	49,	33,	25	},	// 41 f
	{	19,	35,	58,	50,	34,	26	},	// 42 h
	{	20,	36,	59,	51,	35,	27	},	// 43 k
	{	21,	76,	60,	52,	36,	28	},	// 44 :
	{	-1,	-1,	-1,	-1,	-1,	-1	},	// 45
	{	30,	39,	-1,	62,	38,	-1	},	// 46 5
	{	31,	-1,	-1,	63,	39,	-1	},	// 47 *
	{	40,	56,	13,	-1,	64,	32	},	// 48 z
	{	41,	57,	74,	-1,	56,	33	},	// 49 c
	{	42,	58,	-1,	-1,	57,	34	},	// 50 b
	{	43,	59,	-1,	-1,	58,	35	},	// 51 m
	{	44,	60,	77,	74,	59,	36	},	// 52 ;
	{	68,	-1,	-1,	69,	76,	75	},	// 53 return
	{	38,	62,	-1,	70,	-1,	-1	},	// 54 1
	{	39,	63,	-1,	71,	62,	-1	},	// 55 3
	{	33,	49,	-1,	13,	48,	40	},	// 56 x
	{	34,	50,	74,	-1,	49,	41	},	// 57 v
	{	54,	74,	-1,	-1,	50,	42	},	// 58 n
	{	36,	52,	-1,	74,	51,	43	},	// 59 ,
	{	76,	69,	-1,	77,	52,	44	},	// 60 ?
	{	-1,	-1,	-1,	-1,	-1,	-1	},	// 61
	{	46,	55,	-1,	78,	54,	-1	},	// 62 2
	{	47,	-1,	-1,	79,	55,	-1	},	// 63 +
	{	32,	48,	-1,	-1,	-1, -1/*67*/	},	// 64 left shift
	{	-1,	0,	72,	-1,	-1,	-1	},	// 65 @
	{	-1,	-1,	68,	75,	73,	-1	},	// 66 ]
	{	16,	32,	64,	-1,	-1,	-1	},	// 67 shift lock
	{	-1,	-1,	-1,	53,	75,	66	},	// 68 >
	{	53,	-1,	-1,	-1,	60,	76	},	// 69 right shift
	{	54,	78,	-1,	-1,	-1,	-1	},	// 70 0
	{	55,	79,	-1,	-1,	78,	-1	},	// 71 -
	{	0,	16, -1/*67*/,	-1,	-1,	65	},	// 72 rev
	{	-1,	66,	75,	21,	5,	-1	},	// 73 [
	{	-1,	-1,	-1,	-1,	-1,	-1	},	// 74 spc
	{	66,	68,	53,	76,	21,	73	},	// 75 <
	{	75,	53,	69,	60,	44,	21	},	// 76 stp
	{	60,	-1,	-1,	-1,	74,	52	},	// 77 srq
	{	62,	71,	-1,	-1,	70,	-1	},	// 78 .
	{	63,	-1,	-1,	-1,	71,	-1	}	// 79 =
};

// kprog state, one per key
unsigned char kprog[MAXLED];
unsigned char tmp[MAXLED][COLSANDPARS];

extern unsigned char rgb[MAXLED][COLSANDPARS];

int delcnt = 0;

/*
 * Program handling per for patterns
 *
 */
void kprog_setup() {
	memset(kprog, 0, MAXLED * sizeof(unsigned char));
	memset(tmp, 0, MAXLED * COLSANDPARS * sizeof(unsigned char));

	delcnt = 0;
}


static const unsigned char logo_white[] = {
	7,14,22,38,54,78,71,
	23,31
};

static const unsigned char logo_red[] = {
	55,63
};

void kp_logo() {

	unsigned char white[] = { 0,0,128, 0};
	unsigned char red[] = { 128,0,0, 0};
	
	for (int i = 0; i < sizeof(logo_white); i++) {
		prog_set(logo_white[i], LP_LINDECAY, white);
	}
	for (int i = 0; i < sizeof(logo_red); i++) {
		prog_set(logo_red[i], LP_LINDECAY, red);
	}
}


void kprog_set(int k, int prog, char pars[COLSANDPARS]) {
	

	switch(prog) {
	case KP_DILUTE:
		kprog[k] = prog;
		prog_set(k, LP_LINDECAY, pars);
		break;
	case KP_LOGO:	
		kp_logo();
		break;
	default:
		break;
	}
}

void kp_dilute(int k, unsigned char src[COLSANDPARS], 
			unsigned char trg[MAXLED][COLSANDPARS]) {

	unsigned char r, g, b;
	int i,j;

	r = src[0] >> 3;
	g = src[1] >> 3;
	b = src[2] >> 3;

	for (i = 0; i < 6; i++) {
		if ((j = keyrels[k][i]) >= 0) {
			trg[j][0] += r;
			trg[j][1] += g;
			trg[j][2] += b;
		}
	}
	src[0] >>= 1;
	src[1] >>= 1;
	src[2] >>= 1;
}

void kp_dilute_prog(int k) {

	int i,j;

	for (i = 0; i < 6; i++) {
		if ((j = keyrels[k][i]) >= 0) {
			kprog[j] = KP_DILUTE;
		}
	}
}

// advance state from st1 to st2
void kprog_advance() {

	if (delcnt) {
		delcnt --;
		return;
	}
	delcnt = 6;
	
	memset(tmp, 0, MAXLED * COLSANDPARS * sizeof(unsigned char));
	
	for (int k = 0; k < MAXLED; k++) {
		switch (kprog[k]) {
		case KP_DILUTE:
			if (rgb[k][0] | rgb[k][1] | rgb[k][2]) {
				// TODO switch on kprog
				kp_dilute(k, rgb[k], tmp);
			} else {
				kprog[k] = 0;
			}
			break;
		default:
			kprog[k] = 0;
			break;
		}
	}
	for (int k = 0; k < MAXLED; k++) {
		if (kprog[k]) {
			kp_dilute_prog(k);
		}
	}

	//memcpy(rgb, tmp, MAXLED * COLSANDPARS * sizeof(unsigned char));

	for (int k = 0; k < MAXLED; k++) {

		if (kprog[k]) {
			// any color set?
			prog_set(k, LP_CONST, tmp[k]);
		}
	}
}




