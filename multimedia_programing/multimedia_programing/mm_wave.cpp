// mm_wave.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//
#include "stdafx.h"
#include <iostream>
#include <conio.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;
#define Default_Samplerate 44100
#define Default_LEN_BUF	44100
#define nSec  5

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

#define	LEFT	0
#define	RIGHT	1
#define	STEREO	2

#define	HEAD	10	// 머리의 반지름 in cm
#define	DIS		1	// 음원과의 거리 in m
#define	Vs		339

#define	NUM_KEYS	3
#define	PI			3.141592

int iBuf;
int iplaying;
unsigned long result;
short	*in, *out;
char	filter_on = 0;
float	fc, *fir_coef, *iir_coefA, *iir_coefB;
int		MA_tap, fir_tap, w_type, iir_order, cur_key, dir, max_delay, Samplerate, LEN_BUF;

double	key_freq[] = { 262, 330, 392 };
double	cur_corl, sound_loc;
char	key_name[][5] = { "도","미","솔" };
short	*key_wav;
short	*aheadL, *aheadR;

enum { NUM_BUF = 8 };

void	MovingAverageFilter(short *in, short *out, int N, int tap)
{
	int		i, j, M;
	float	t;

	for (i = 0; i < N; i++) {
		for (j = 0, t = 0, M = 0; j < tap; j++) {
			if (i - j >= 0) {
				t += in[i - j];
				M++;
			}
		}
		out[i] = (short)(t / M);
	}
}

void	Make_Win(float *w, int len, int w_type)
{
	int		i;

	for (i = 0; i<len; i++) {
		if (w_type == RECT)
			w[i] = 1;
		if (w_type == HANN)
			w[i] = (float)(0.5*(1 - cos(2 * 3.141592*i / (len - 1.))));
		if (w_type == HAMM)
			w[i] = 0.54 - 0.46*cos(3.141592*2.*i / (len - 1));
		if (w_type == TRIA)
			w[i] = (i<len / 2) ? ((float)i * 2) / len : 2 * (1 - (float)i / len);
	}
}

void	GetFIRcoefs(float *coef, float fc, int flen, int w_type)
{
	int		n, t;
	float	*w, f_norm;

	w = (float *)malloc(sizeof(float)*flen);
	Make_Win(w, flen, w_type);

	f_norm = (float)fc / Samplerate;

	for (n = 0; n < flen; n++) {
		t = (n - flen / 2);
		if (t == 0) {
			coef[n] = w[n] * 2 * f_norm;
		}
		else {
			coef[n] = sin(2 * 3.141592*f_norm*t) / (3.141592*t);
			coef[n] *= w[n];
		}
	}
	//	for (n = 0; n < flen; n++) printf(" %f", coef[n]); printf("\n");
	free(w);
}

void	GetHPFcoefs(float *coef, int tap)
{
	for (int i = 0; i < tap; i++) {
		if (i % 2 == 1) coef[i] = -coef[i];
	}

}
void	FIR_filter(short *in, short *out, float *coef, int N, int tap)
{
	int		i, j, k;
	float	v;

	for (i = 0; i < N; i++) {
		for (j = 0, v = 0; j < tap; j++) {
			k = i - tap / 2 + j;
			if (0 <= k && k < N)
				v += in[k] * coef[j];
		}
		out[i] = (short)v;
	}
}

void	IIR_filfter(short *in, short *out, float *A, float *B, int N, int order)
{
	int		i, j;
	float	v;

	for (i = 0; i < N; i++) {
		v = 0;
		for (j = 0; j < order; j++) {
			if (i - j >= 0) {
				v += B[j] * in[i - j];
				v += A[j] * out[i - j];
			}
		}
		out[i] = (short)v;
	}
}

void	Build_Sinewaves(int key_id, short *d, int N)
{
	static double phs = 0.;
	double	div_f;
	int		div_i;

	for (int i = 0; i < N; i++)
		d[i] = (short)(3000 * sin(2 * PI * (key_freq[key_id] / Samplerate) * i + phs));

	phs = 2 * PI*(key_freq[key_id] / Samplerate)*N + phs;
	div_f = phs / (PI * 2);
	div_i = (int)div_f;
	phs = phs - div_i*PI * 2;
}

double	Correlation(short *x, short *y, int N, int key_id)
{
	int		max_lag, i, j;
	double	mx, my, sx, sy, corl, max_corl;

	max_lag = Samplerate / key_freq[key_id];

	for (i = 0, max_corl = 0; i < max_lag; i++) {
		mx = my = sx = sy = corl = 0.;
		for (j = 0; j < N - i; j++) {
			mx += x[j];
			my += y[j + i];
			sx += x[j] * x[j];
			sy += y[j + i] * y[j + i];
			corl += x[j] * y[j + i];
		}
		mx = mx / (N - i);
		my = my / (N - i);
		sx = sqrt(sx / (N - i) - mx*mx);
		sy = sqrt(sy / (N - i) - my*my);
		corl = (corl / (N - i) - mx*my) / (sx*sy);
		corl = fabs(corl);
		max_corl = (corl > max_corl) ? corl : max_corl;
	}
	return max_corl;
}

void	MakeStereo(short *in, short *out, int N, int dir)
{
	for (int i = 0; i < N; i++) {
		if (dir == LEFT) {
			out[i * 2] = in[i * 2];
			out[i * 2 + 1] = 0;
		}
		else if (dir == RIGHT) {
			out[i * 2] = 0;
			out[i * 2 + 1] = in[i * 2 + 1];
		}
		else {
			out[i * 2] = in[i * 2];
			out[i * 2 + 1] = in[i * 2 + 1];
		}

	}
}

void	Rendering(short *in, short *out, int N, double dir)
{
	int		i, dt;
	double	dL;

	dL = 2 * HEAD*0.01*fabs(sin(dir));
	dt = (int)(dL*Samplerate / Vs); printf("dt=%d\n", dt);
	if (dir >= 0) {
		for (i = 0; i < N; i++) {
			out[i * 2] = (i < dt) ? aheadL[max_delay - dt + i] : in[(i - dt) * 2];
			out[i * 2] = (short)(out[i * 2] * (DIS / (DIS + dL)));
			out[i * 2 + 1] = in[i * 2 + 1];
		}
	}
	else {
		for (i = 0; i < N; i++) {
			out[i * 2 + 1] = (i < dt) ? aheadR[max_delay - dt + i] : in[(i - dt) * 2 + 1];
			out[i * 2 + 1] = (short)(out[i * 2 + 1] / (DIS / (DIS + dL)));
			out[i * 2] = in[i * 2];
		}
	}
	for (i = 0; i < max_delay; i++) {
		aheadL[i] = in[(N - max_delay + i) * 2];
		aheadR[i] = in[(N - max_delay + i) * 2 + 1];
	}
}

void	Do_Proc(short *in, short *out, int N)
{
	if (filter_on == MOV_AVG) 
		MovingAverageFilter(in, out, N, MA_tap);
	else if (filter_on == FIR) 
		FIR_filter(in, out, fir_coef, N, fir_tap);
	else if (filter_on == IIR) 
		IIR_filfter(in, out, iir_coefA, iir_coefB, N, iir_order);
	else if (filter_on == SLOC) 
		Rendering(in, out, N / 2, sound_loc);
}

void Print_Usage(char *argv)
{
	printf("************************************++++\n");
	printf("* Multimedia programming example codes *\n");
	printf("************************************++++\n");
	printf("\n사용법: %s 샘플링주파수(Hz) 입력_파일이름 출력_파일이름 option-.\n", argv);
	printf("입/출력파일 포맷: 16비트, Little endian mono. \n");
	printf("options-\n");
	printf("m: Moving average filtering..\n");
	printf("f: FIR filtering..\n");
	printf("i: IIR filtering..\n");
	printf("l: Sound rendering..\n");
}

int main(int argc, char **argv)
{
	char	key;
	FILE	*fpr, *fpw;

	if (argc < 5) {
		Print_Usage(argv[0]);
		exit(-1);
	}

	Samplerate = atoi(argv[1]);

	if ((fpr = fopen(argv[2], "rb")) == NULL) {
		printf("File [%s] is not found.. \n", argv[2]);
		exit(-1);
	}
	fseek(fpr, 0L, SEEK_END);
	LEN_BUF = ftell(fpr) / sizeof(short);
	rewind(fpr);

	in = (short *)malloc(sizeof(short)*LEN_BUF);
	out = (short *)malloc(sizeof(short)*LEN_BUF);
	fread(in, sizeof(short), LEN_BUF, fpr);
	fclose(fpr);

	key = argv[4][0];

	if (key == 'l') {
		max_delay = (int)(Samplerate * 2 * HEAD*0.01 / Vs);
		aheadL = (short *)malloc(sizeof(short)*max_delay);
		aheadR = (short *)malloc(sizeof(short)*max_delay);
		printf("Max_head = %d\n", max_delay);
	}

	if (key == 'm') {
		printf("Moving average filter is selected..\n");
		printf("Enter moving average filter length: ");
		scanf("%d", &MA_tap);
		filter_on = MOV_AVG;
	}

	if (key == 'f') {
		printf("FIR filter is selected..\n");
		printf("Cutoff freqency (in Hz) : "); scanf("%f", &fc);
		printf("Filter length (in samples, shoud be odd number) : "); scanf("%d", &fir_tap);
		printf("Window type (0=Rect/1=Hann/2=Hamm/3=Tria) "); scanf("%d", &w_type);
		if (fir_coef != NULL) free(fir_coef);
		fir_coef = (float *)malloc(sizeof(float)*fir_tap);
		GetFIRcoefs(fir_coef, fc, fir_tap, w_type);
		printf("(L)PF or (H)PF ");
		key = getche();
		if (key == 'H' || key == 'h') {
			printf("\nHPF selected.\n"); GetHPFcoefs(fir_coef, fir_tap);
		}
		else
			printf("\nLPF selected.\n");

		filter_on = FIR;
	}
		
	if (key == 'i') {
		printf("IIR filter is selected..\n");
		printf("Enter order : "); scanf("%d", &iir_order);
		iir_coefA = (float *)malloc(sizeof(float)*iir_order);
		iir_coefB = (float *)malloc(sizeof(float)*iir_order);
		printf("Enter coef: \n");
		for (int i = 0; i < iir_order; i++) {
			printf("A[%d], B[%d] : ", i, i);
			scanf("%f %f", &iir_coefA[i], &iir_coefB[i]);
		}
		filter_on = IIR;
	}

	if (key == 'l') {
		filter_on = 0;
		printf("Sound localization is selected.\n");
		printf("Enter sound location (angle, in degree) ");
		scanf("%lf", &sound_loc);
		sound_loc = sound_loc * PI / 180;
		filter_on = SLOC;
	}

	Do_Proc(in, out, LEN_BUF);
	fpw = fopen(argv[3], "wb");
	fwrite(out, sizeof(short), LEN_BUF, fpw);
	fclose(fpw);

	return 0;
}