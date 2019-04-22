#pragma once
#define _USE_MATH_DEFINES
#include <math.h>

#define NUM_KEYS 8

#define	MOV_AVG	0
#define	FIR		1
#define	IIR		2
#define	KEY		3
#define	SPC		4
#define	SLOC	5

#define	RECT	0
#define	HANN	1
#define	HAMM	2
#define	TRIA	3
#define BLACK   4

double	cur_corl;
int		cur_key;
double	key_freq[] = { 262,293,330,349,392,440,493,1046 };
char	key_name[][5] = { "도","레","미","파","솔","라","시","도3" };

int		fft_test(short *in, short *out, unsigned long long LEN_BUF);

double	Correlation(short *x, short *y, int N, int key_id);
void	Build_Sinewaves(int key_id, short *d, int N);

