#pragma once
#define _USE_MATH_DEFINES
#include <math.h>

#define MOV_AVG 0
#define FIR 1
#define IIR 2
#define RECT 0
#define HANN 1
#define HAMM 2
#define TRIA 3
#define BLACK 4

void WeightedMovingAverageFilter(short *in, short *out, int N, int tap) {
	int i, j, M, k;
	float t;

	for (i = 0; i < N; i++) {
		for (j = i - (int)(tap / 2), t = 0, M = 0, k = 0; j <= (int)(i + tap / 2); j++) {
			if (j >= 0 && j < N) {
				if (j == i) {
					t += in[j] * tap / 2;
					M += tap / 2;
				}
				t += in[j];
				M++;
			}
		}
		
		out[i] = (short)(t / M);
	}
}
void MovingAverageFilter(short *in, short *out, int N, int tap) {
	int i, j, M, k;
	float t;
	for (i = 0; i < N; i++) {
		for (j = 0, t = 0, M = 0, k = 0; j < tap; j++) {
			if (i - j >= 0) {
				t += in[i - j];
				M++;
			}
		}

		out[i] = (short)(t / M);
	}
}
void Make_Win(float *w, int len, int w_type)
{
	int i;
	for (i = 0; i<len; i++) {
		if (w_type == RECT)
			w[i] = 1;
		if (w_type == HANN)
			w[i] = (float)(0.5*(1 - cos(2 * M_PI*i / (len - 1.))));
		if (w_type == HAMM)
			w[i] = 0.54 - 0.46*cos(M_PI*2.*i / (len - 1));
		if (w_type == TRIA)
			w[i] = (i<len / 2) ? ((float)i * 2) / len : 2 * (1 - (float)i / len);
		if (w_type == BLACK)
			w[i] = 0.42 - 0.5*cos(2 * M_PI*i / (len - 1)) + 0.08*cos(2 * M_PI*i / (len - 1));
	}
}

void GetFIRcoefs(float *coef, float fc, int flen, int w_type, int Samplerate)
{
	int n, t;
	float *w, f_norm;
	w = (float *)malloc(sizeof(float)*flen);
	Make_Win(w, flen, w_type);
	f_norm = (float)fc / Samplerate;
	for (n = 0; n < flen; n++) {
		t = (n - flen / 2);
		if (t == 0)
			coef[n] = w[n] * 2 * f_norm;
		else
			coef[n] = w[n] * sin(2 * M_PI*f_norm*t) / (M_PI*t);
	}
	free(w);
}


void GetHPFcoefs(float *coef, int tap)
{
	for (int i = 0; i < tap; i++) {
		if (i % 2 == 1) coef[i] = -coef[i];
	}
}

void FIR_filter(short *in, short *out, float *coef, int N, int tap)
{
	int i, j, k;
	float v;
	for (i = 0; i < N; i++) {
		for (j = 0, v = 0; j < tap; j++) {
			k = i - tap / 2 + j;
			if (k > 0)
				v += in[k%N] * coef[j];
			else
				v += in[(k + N) % N] * coef[j];
		}
		out[i] = (short)v;
	}
}
