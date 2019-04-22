// mm_wave.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//
#pragma comment(lib, "libfftw3-3.lib")
#pragma comment(lib, "winmm.lib")
#include "stdafx.h"
#include <iostream>
#include <conio.h>
#include <math.h>
#include <stdlib.h>
#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include "fftw3.h"
#include "Myfilter.h"

using namespace std;
#define Default_Samplerate 44100
#define Default_LEN_BUF	44100
#define nSec  5

#define	LEFT	0
#define	RIGHT	1
#define	STEREO	2

#define	HEAD	10	// 머리의 반지름 in cm
#define	DIS		1	// 음원과의 거리 in m
#define	Vs		339

unsigned long long result, Samplerate, LEN_BUF;
short	*in, *out;
char	filter_on = -1;
float	fc, *fir_coef, *iir_coefA, *iir_coefB;
int		MA_tap, fir_tap, w_type, iir_order, dir, max_delay;

double  sound_loc, phs;
short	*key_wav;
short	*aheadL, *aheadR;

enum { NUM_BUF = 8 };

HANDLE hEvent_BufferReady;
HANDLE hEvent_FinishedPlaying;
int iBuf;
int iplaying;
short *spt;
HWAVEIN hWaveIn;
HWAVEOUT hWaveOut;
WAVEFORMATEX pFormat;
//enum { NUM_BUF = 8 };
//WAVEHDR header[NUM_BUF];
WAVEHDR *header;

DWORD WINAPI RecordingWaitingThread(LPVOID ivalue)
{
	while (1) {
		WaitForSingleObject(hEvent_BufferReady, INFINITE);
		result = waveInUnprepareHeader(hWaveIn, &header[iBuf], sizeof(WAVEHDR));
		iplaying = iBuf;
		memcpy(spt, header[iBuf].lpData, LEN_BUF * sizeof(short));
		//if (filter_on == MOV_AVG) {
		//	MovingAverageFilter(spt, out, LEN_BUF, tap);
		//	//WeightedMovingAverageFilter(spt, out, LEN_BUF, tap);
		//	//memcpy(spt, out, LEN_BUF * sizeof(short));
		//}
		//if (filter_on == FIR) {
		//	FIR_filter(spt, out, fir_coef, LEN_BUF, fir_tap);
		//	//memcpy(spt, out, LEN_BUF * sizeof(short));
		//}
		if (filter_on == KEY) {
			Build_Sinewaves(cur_key, out, LEN_BUF);
			cur_corl = Correlation(spt, out, LEN_BUF, cur_key);
			printf("현재 음정[%s]와 상관값 : %f\n", key_name[cur_key], cur_corl);
		}

		memcpy(header[iBuf].lpData, out, LEN_BUF * sizeof(short));

		result = waveOutPrepareHeader(hWaveOut, &header[iplaying], sizeof(WAVEHDR));
		result = waveOutWrite(hWaveOut, &header[iplaying], sizeof(WAVEHDR)); // play audio
		++iBuf;
		if (iBuf == NUM_BUF) iBuf = 0;
		result = waveInPrepareHeader(hWaveIn, &header[iBuf], sizeof(WAVEHDR));
		result = waveInAddBuffer(hWaveIn, &header[iBuf], sizeof(WAVEHDR));
	}
	return 0;
}
DWORD WINAPI PlayingWaitingThread(LPVOID ivalue)
{
	while (1) {
		WaitForSingleObject(hEvent_FinishedPlaying, INFINITE);
		waveOutUnprepareHeader(hWaveOut, &header[iplaying], sizeof(WAVEHDR));
	}
}
static void CALLBACK waveOutProc(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD
	dwParam2)
{
	if (uMsg != WOM_DONE)
		return;
	SetEvent(hEvent_FinishedPlaying);
}
void CALLBACK myWaveInProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	if (uMsg != WIM_DATA)
		return;
	SetEvent(hEvent_BufferReady);
}

int key_check()
{
	Samplerate = 16000;
	LEN_BUF = 4000;
	spt = new short[LEN_BUF];
	in = new short[LEN_BUF];
	out = new short[LEN_BUF];
	header = new WAVEHDR[LEN_BUF];
	hEvent_BufferReady = CreateEvent(NULL, FALSE, FALSE, NULL);
	hEvent_FinishedPlaying = CreateEvent(NULL, FALSE, FALSE, NULL);
	pFormat.wFormatTag = WAVE_FORMAT_PCM; // simple, uncompressed format
	pFormat.nChannels = 1; // 1=mono, 2=stereo
	pFormat.nSamplesPerSec = Samplerate;
	pFormat.wBitsPerSample = 16; // 16 for high quality, 8 for telephone-grade
	pFormat.nBlockAlign = pFormat.nChannels*pFormat.wBitsPerSample / 8;
	pFormat.nAvgBytesPerSec = (pFormat.nSamplesPerSec)*(pFormat.nChannels)*(pFormat.wBitsPerSample) / 8;
	pFormat.cbSize = 0;
	short int *pBuf;
	size_t bpbuff = LEN_BUF;
	pBuf = new short int[bpbuff * NUM_BUF];
	result = waveInOpen(&hWaveIn, WAVE_MAPPER, &pFormat, (DWORD_PTR)myWaveInProc, 0L, CALLBACK_FUNCTION);
	result = waveOutOpen(&hWaveOut, WAVE_MAPPER, &pFormat, (DWORD_PTR)waveOutProc, 0, CALLBACK_FUNCTION);
	// initialize all headers in the queue
	for (int i = 0; i < NUM_BUF; i++)
	{
		header[i].lpData = (LPSTR)&pBuf[i * bpbuff];
		header[i].dwBufferLength = bpbuff * sizeof(*pBuf);
		header[i].dwFlags = 0L;
		header[i].dwLoops = 0L;
	}
	DWORD myThreadID;
	DWORD myThreadIDPlay;
	HANDLE hThread;
	HANDLE hThreadPlay;
	hThread = CreateThread(NULL, 0, RecordingWaitingThread, NULL, 0, &myThreadID);
	hThreadPlay = CreateThread(NULL, 0, PlayingWaitingThread, NULL, 0, &myThreadIDPlay);
	iBuf = 0;
	waveInPrepareHeader(hWaveIn, &header[iBuf], sizeof(WAVEHDR));
	waveInAddBuffer(hWaveIn, &header[iBuf], sizeof(WAVEHDR));
	waveInStart(hWaveIn);

	char key = 'k';
	do {
		key = _getch();
		cur_key = key - '0';

	} while (key != 'e' && key != 'q');

	waveInClose(hWaveIn);
	waveOutClose(hWaveOut);
	CloseHandle(hThread);
	CloseHandle(hThreadPlay);
	CloseHandle(hEvent_BufferReady);
	CloseHandle(hEvent_FinishedPlaying);
	return 0;
}
void	Build_Sinewaves(int key_id, short *d, int N)
{
	static double phs = 0.;
	double	div_f;
	int		div_i;
	int amplelify = 5000;
	for (int i = 0; i < N; i++) {
		d[i] = (short)(amplelify *sin(2 * M_PI * (key_freq[key_id] / Samplerate) * i + phs));
		//d[i] += (short)(amplelify *sin(2 * M_PI * (key_freq[key_id + 2] / Samplerate) * i + phs));
	}

	phs = 2 * M_PI*(key_freq[key_id] / Samplerate)*N + phs;
	div_f = phs / (M_PI * 2);
	div_i = (int)div_f;
	phs = phs - div_i * M_PI * 2;
}

double	Correlation(short *x, short *y, int N, int key_id)
{
	int		max_lag, i, j;
	double	mx, my, sx, sy, corl, max_corl;

	max_lag = Samplerate / key_freq[key_id];

	for (i = 0, max_corl = 0; i < max_lag; i++) { //max_lag : 두 신호의 phase가 어긋나있는 경우를 보상하기위해
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
		sx = sqrt(sx / (N - i) - mx * mx);
		sy = sqrt(sy / (N - i) - my * my);
		corl = (corl / (N - i) - mx * my) / (sx*sy);
		corl = fabs(corl);
		max_corl = (corl > max_corl) ? corl : max_corl;
	}
	return max_corl;
}
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

	for (i = 0; i < len; i++) {
		if (w_type == RECT)
			w[i] = 1;
		if (w_type == HANN)
			w[i] = (float)(0.5*(1 - cos(2 * 3.141592*i / (len - 1.))));
		if (w_type == HAMM)
			w[i] = 0.54 - 0.46*cos(3.141592*2.*i / (len - 1));
		if (w_type == TRIA)
			w[i] = (i < len / 2) ? ((float)i * 2) / len : 2 * (1 - (float)i / len);
		if (w_type == BLACK)
			w[i] = 0.42 - 0.5*cos(2 * M_PI*i / (len - 1)) + 0.08*cos(2 * M_PI*i / (len - 1));
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
	else if (filter_on == KEY) {
		key_check();
	}
	else return;
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

	int fft_flag = 0;
	if (key == 'b') {
		filter_on = -1;
		fft_test(in, out, LEN_BUF);
	}
	else if (key == 't' || fft_flag) {
		printf("FFT is selecte\n");
		fft_flag = 1;

		printf("Enter start freq, end freq : ");
		unsigned long long start_f = 0, end_f = 0;
		scanf("%llu %llu", &start_f, &end_f);

		int npoint = 1;
		while (npoint < LEN_BUF) {
			npoint *= 2;
		}

		end_f = end_f > Samplerate ? Samplerate : end_f;
		start_f = (double)start_f / Samplerate * npoint;
		end_f = (double)end_f / Samplerate * npoint;

		//warning message pop up when they over boundary
		fftw_complex *ft_in = (fftw_complex *)fftw_malloc(sizeof(fftw_complex)*npoint);
		fftw_complex *ft_out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex)*npoint);
		fftw_complex *ift_out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex)*npoint);
		fftw_plan plan_forward = fftw_plan_dft_1d(npoint, ft_in, ft_out, FFTW_FORWARD, FFTW_ESTIMATE);
		fftw_plan plan_backward = fftw_plan_dft_1d(npoint, ft_out, ift_out, FFTW_BACKWARD, FFTW_ESTIMATE);

		for (unsigned long long i = 0; i < LEN_BUF; ++i) {
			ft_in[i][0] = in[i];
			ft_in[i][1] = 0;
		}
		for (unsigned long long i = LEN_BUF; i < npoint; ++i) {
			ft_in[i][0] = 0;
			ft_in[i][1] = 0;
		}

		fftw_execute(plan_forward);

		for (unsigned long long i = 0; i < start_f; ++i) {
			ft_out[i][0] = 0;
			ft_out[i][1] = 0;
			ft_out[npoint - i - 1][0] = 0;
			ft_out[npoint - i - 1][1] = 0;
		}

		for (unsigned long long i = end_f; i < npoint / 2; ++i) {
			ft_out[i][0] = 0;
			ft_out[i][1] = 0;
			ft_out[npoint - i - 1][0] = 0;
			ft_out[npoint - i - 1][1] = 0;
		}

		/*FILE *t_debug = fopen("fft.csv", "w");
		for (unsigned long long i = 0; i < npoint; ++i) {
			fprintf(t_debug, "%f\n", sqrt(ft_out[i][0] * ft_out[i][0] + ft_out[i][1] * ft_out[i][1]));
		}
		fclose(t_debug);*/
		fftw_execute(plan_backward);

		for (unsigned long long i = 0; i < LEN_BUF; ++i) {
			out[i] = ift_out[i][0] / npoint;
		}

		fftw_destroy_plan(plan_forward);
		fftw_destroy_plan(plan_backward);
		fftw_free(ft_in);
		fftw_free(ft_out);
		fftw_free(ift_out);
	}

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
		key = _getche();
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
		sound_loc = sound_loc * M_PI / 180;
		filter_on = SLOC;
	}

	if (key == 'k') {
		filter_on = 0;
		printf("\n음정연습\n");
		printf("키를 입력하세요 :");
		for (int i = 0; i < NUM_KEYS; ++i)printf("%s(%d) ", key_name[i], i);
		printf("...\n");
		scanf("%d", &cur_key);
		filter_on = KEY;
	}

	Do_Proc(in, out, LEN_BUF);
	fpw = fopen(argv[3], "wb");
	fwrite(out, sizeof(short), LEN_BUF, fpw);
	fclose(fpw);

	return 0;
}