//#include "stdafx.h"
//#include <Windows.h>
//#pragma comment(lib, "winmm.lib")
//#include <iostream>
//#include <conio.h>
//#include <cstdio>
//#include "Myfilter.h"
//
//using std::cin;
//
//namespace KIS{
//	HANDLE hEvent_BufferReady;
//	HANDLE hEvent_FinishedPlaying;
//#define LEN_BUF 2000
//#define nSec 5
//	int iBuf;
//	int iplaying;
//	unsigned long result, Samplerate;
//	short spt[LEN_BUF];
//	HWAVEIN hWaveIn;
//	HWAVEOUT hWaveOut;
//	WAVEFORMATEX pFormat;
//	enum { NUM_BUF = 8 };
//	WAVEHDR header[NUM_BUF];
//
//	short in[LEN_BUF], out[LEN_BUF];
//	char filter_on = 0;
//	float fc, *fir_coef;
//	int fir_tap, w_type;
//	int tap = 5;
//
//	FILE *fp = NULL;
//	char fname[100] = { 0 };
//	bool save_flag = false;
//	void	MovingAverageFilter(short *in, short *out, int N, int tap)
//	{
//		int		i, j, M;
//		float	t;
//	
//		for (i = 0; i < N; i++) {
//			for (j = 0, t = 0, M = 0; j < tap; j++) {
//				if (i - j >= 0) {
//					t += in[i - j];
//					M++;
//				}
//			}
//			out[i] = (short)(t / M);
//		}
//	}
//	
//	void	Make_Win(float *w, int len, int w_type)
//	{
//		int		i;
//	
//		for (i = 0; i < len; i++) {
//			if (w_type == RECT)
//				w[i] = 1;
//			if (w_type == HANN)
//				w[i] = (float)(0.5*(1 - cos(2 * 3.141592*i / (len - 1.))));
//			if (w_type == HAMM)
//				w[i] = 0.54 - 0.46*cos(3.141592*2.*i / (len - 1));
//			if (w_type == TRIA)
//				w[i] = (i < len / 2) ? ((float)i * 2) / len : 2 * (1 - (float)i / len);
//			if (w_type == BLACK)
//				w[i] = 0.42 - 0.5*cos(2 * M_PI*i / (len - 1)) + 0.08*cos(2 * M_PI*i / (len - 1));
//		}
//	}
//	
//	void	GetFIRcoefs(float *coef, float fc, int flen, int w_type)
//	{
//		int		n, t;
//		float	*w, f_norm;
//	
//		w = (float *)malloc(sizeof(float)*flen);
//		Make_Win(w, flen, w_type);
//	
//		f_norm = (float)fc / Samplerate;
//	
//		for (n = 0; n < flen; n++) {
//			t = (n - flen / 2);
//			if (t == 0) {
//				coef[n] = w[n] * 2 * f_norm;
//			}
//			else {
//				coef[n] = sin(2 * 3.141592*f_norm*t) / (3.141592*t);
//				coef[n] *= w[n];
//			}
//		}
//		//	for (n = 0; n < flen; n++) printf(" %f", coef[n]); printf("\n");
//		free(w);
//	}
//	
//	void	GetHPFcoefs(float *coef, int tap)
//	{
//		for (int i = 0; i < tap; i++) {
//			if (i % 2 == 1) coef[i] = -coef[i];
//		}
//	
//	}
//	void	FIR_filter(short *in, short *out, float *coef, int N, int tap)
//	{
//		int		i, j, k;
//		float	v;
//	
//		for (i = 0; i < N; i++) {
//			for (j = 0, v = 0; j < tap; j++) {
//				k = i - tap / 2 + j;
//				if (0 <= k && k < N)
//					v += in[k] * coef[j];
//			}
//			out[i] = (short)v;
//		}
//	}
//	
//	void	IIR_filfter(short *in, short *out, float *A, float *B, int N, int order)
//	{
//		int		i, j;
//		float	v;
//	
//		for (i = 0; i < N; i++) {
//			v = 0;
//			for (j = 0; j < order; j++) {
//				if (i - j >= 0) {
//					v += B[j] * in[i - j];
//					v += A[j] * out[i - j];
//				}
//			}
//			out[i] = (short)v;
//		}
//	}
//
//
//	DWORD WINAPI RecordingWaitingThread(LPVOID ivalue)
//	{
//		while (1) {
//			WaitForSingleObject(hEvent_BufferReady, INFINITE);
//			result = waveInUnprepareHeader(hWaveIn, &header[iBuf], sizeof(WAVEHDR));
//			iplaying = iBuf;
//			memcpy(spt, header[iBuf].lpData, LEN_BUF * sizeof(short));
//			if (filter_on == MOV_AVG) {
//				MovingAverageFilter(spt, out, LEN_BUF, tap);
//				//WeightedMovingAverageFilter(spt, out, LEN_BUF, tap);
//				//memcpy(spt, out, LEN_BUF * sizeof(short));
//			}
//			if (filter_on == FIR) {
//				FIR_filter(spt, out, fir_coef, LEN_BUF, fir_tap);
//				//memcpy(spt, out, LEN_BUF * sizeof(short));
//			}
//			if (filter_on == KEY) {
//				Build_Sinewaves(cur_key, out, LEN_BUF);
//				cur_corl = Correlation(spt, out, LEN_BUF, cur_key);
//				printf("현재 음정[%s]와 상관값 : %f\n", key_name[cur_key], cur_corl);
//			}
//
//			memcpy(header[iBuf].lpData, out, LEN_BUF * sizeof(short));
//
//			result = waveOutPrepareHeader(hWaveOut, &header[iplaying], sizeof(WAVEHDR));
//			result = waveOutWrite(hWaveOut, &header[iplaying], sizeof(WAVEHDR)); // play audio
//			++iBuf;
//			if (iBuf == NUM_BUF) iBuf = 0;
//			result = waveInPrepareHeader(hWaveIn, &header[iBuf], sizeof(WAVEHDR));
//			result = waveInAddBuffer(hWaveIn, &header[iBuf], sizeof(WAVEHDR));
//		}
//		return 0;
//	}
//	DWORD WINAPI PlayingWaitingThread(LPVOID ivalue)
//	{
//		while (1) {
//			WaitForSingleObject(hEvent_FinishedPlaying, INFINITE);
//			waveOutUnprepareHeader(hWaveOut, &header[iplaying], sizeof(WAVEHDR));
//		}
//	}
//	static void CALLBACK waveOutProc(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD
//		dwParam2)
//	{
//		if (uMsg != WOM_DONE)
//			return;
//		SetEvent(hEvent_FinishedPlaying);
//	}
//	void CALLBACK myWaveInProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
//	{
//		if (uMsg != WIM_DATA)
//			return;
//		SetEvent(hEvent_BufferReady);
//	}
//	void	Build_Sinewaves(int key_id, short *d, int N)
//	{
//		static double phs = 0.;
//		double	div_f;
//		int		div_i;
//		int amplelify = 3000;
//		for (int i = 0; i < N; i++) {
//			d[i] = (short)(amplelify *sin(2 * M_PI * (key_freq[key_id] / Samplerate) * i + phs));
//			d[i] += (short)(amplelify *sin(2 * M_PI * (key_freq[key_id + 2] / Samplerate) * i + phs));
//		}
//
//		phs = 2 * M_PI*(key_freq[key_id] / Samplerate)*N + phs;
//		div_f = phs / (M_PI * 2);
//		div_i = (int)div_f;
//		phs = phs - div_i * M_PI * 2;
//	}
//
//	double	Correlation(short *x, short *y, int N, int key_id)
//	{
//		int		max_lag, i, j;
//		double	mx, my, sx, sy, corl, max_corl;
//
//		max_lag = Samplerate / key_freq[key_id];
//
//		for (i = 0, max_corl = 0; i < max_lag; i++) { //max_lag : 두 신호의 phase가 어긋나있는 경우를 보상하기위해
//			mx = my = sx = sy = corl = 0.;
//			for (j = 0; j < N - i; j++) {
//				mx += x[j];
//				my += y[j + i];
//				sx += x[j] * x[j];
//				sy += y[j + i] * y[j + i];
//				corl += x[j] * y[j + i];
//			}
//			mx = mx / (N - i);
//			my = my / (N - i);
//			sx = sqrt(sx / (N - i) - mx * mx);
//			sy = sqrt(sy / (N - i) - my * my);
//			corl = (corl / (N - i) - mx * my) / (sx*sy);
//			corl = fabs(corl);
//			max_corl = (corl > max_corl) ? corl : max_corl;
//		}
//		return max_corl;
//	}
//	int key_check(unsigned long Sam)
//	{
//		Samplerate = Sam;
//		hEvent_BufferReady = CreateEvent(NULL, FALSE, FALSE, NULL);
//		hEvent_FinishedPlaying = CreateEvent(NULL, FALSE, FALSE, NULL);
//		pFormat.wFormatTag = WAVE_FORMAT_PCM; // simple, uncompressed format
//		pFormat.nChannels = 1; // 1=mono, 2=stereo
//		pFormat.nSamplesPerSec = Samplerate;
//		pFormat.wBitsPerSample = 16; // 16 for high quality, 8 for telephone-grade
//		pFormat.nBlockAlign = pFormat.nChannels*pFormat.wBitsPerSample / 8;
//		pFormat.nAvgBytesPerSec = (pFormat.nSamplesPerSec)*(pFormat.nChannels)*(pFormat.wBitsPerSample) / 8;
//		pFormat.cbSize = 0;
//		short int *pBuf;
//		size_t bpbuff = LEN_BUF;
//		pBuf = new short int[bpbuff * NUM_BUF];
//		result = waveInOpen(&hWaveIn, WAVE_MAPPER, &pFormat, (DWORD_PTR)myWaveInProc, 0L, CALLBACK_FUNCTION);
//		result = waveOutOpen(&hWaveOut, WAVE_MAPPER, &pFormat, (DWORD_PTR)waveOutProc, 0, CALLBACK_FUNCTION);
//		//initialize all headers in the queue
//		for (int i = 0; i < NUM_BUF; i++)
//		{
//			header[i].lpData = (LPSTR)&pBuf[i * bpbuff];
//			header[i].dwBufferLength = bpbuff * sizeof(*pBuf);
//			header[i].dwFlags = 0L;
//			header[i].dwLoops = 0L;
//		}
//		DWORD myThreadID;
//		DWORD myThreadIDPlay;
//		HANDLE hThread;
//		HANDLE hThreadPlay;
//		hThread = CreateThread(NULL, 0, RecordingWaitingThread, NULL, 0, &myThreadID);
//		hThreadPlay = CreateThread(NULL, 0, PlayingWaitingThread, NULL, 0, &myThreadIDPlay);
//		iBuf = 0;
//		waveInPrepareHeader(hWaveIn, &header[iBuf], sizeof(WAVEHDR));
//		waveInAddBuffer(hWaveIn, &header[iBuf], sizeof(WAVEHDR));
//		waveInStart(hWaveIn);
//		filter_on = KEY;
//		char key = 'k';
//		do {
//			key = _getch();
//
//		} while (key != 'e' && key != 'q');
//
//		waveInClose(hWaveIn);
//		waveOutClose(hWaveOut);
//		CloseHandle(hThread);
//		CloseHandle(hThreadPlay);
//		CloseHandle(hEvent_BufferReady);
//		CloseHandle(hEvent_FinishedPlaying);
//		return 0;
//	}
//
//}
//int main(int argc, _TCHAR* argv[])
//{
//	KIS::key_check(16000);
//}