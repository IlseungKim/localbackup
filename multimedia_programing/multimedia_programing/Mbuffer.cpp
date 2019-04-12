#include "stdafx.h"
#include <Windows.h>
#pragma comment(lib, "winmm.lib")
#include <iostream>
#include <conio.h>
#include <cstdio>
#include "Myfilter.h"

using namespace std;
HANDLE hEvent_BufferReady;
HANDLE hEvent_FinishedPlaying;
#define Samplerate 44100 //16000 44100 48000 ...
#define LEN_BUF 8000
#define nSec 5
int iBuf;
int iplaying;
unsigned long result;
short spt[LEN_BUF];
HWAVEIN hWaveIn;
HWAVEOUT hWaveOut;
WAVEFORMATEX pFormat;
enum { NUM_BUF = 8 };
WAVEHDR header[NUM_BUF];

short in[LEN_BUF], out[LEN_BUF];
char filter_on = 0;
float fc, *fir_coef;
int fir_tap, w_type;
int tap = 5;

FILE *fp = NULL;
char fname[100] = {0};
bool save_flag = false;

DWORD WINAPI RecordingWaitingThread(LPVOID ivalue)
{
	while (1) {
		WaitForSingleObject(hEvent_BufferReady, INFINITE);
		result = waveInUnprepareHeader(hWaveIn, &header[iBuf], sizeof(WAVEHDR));
		iplaying = iBuf;
		memcpy(spt, header[iBuf].lpData, LEN_BUF * sizeof(short));
		if (filter_on == MOV_AVG) {
			MovingAverageFilter(spt, out, LEN_BUF, tap);
			//WeightedMovingAverageFilter(spt, out, LEN_BUF, tap);
			memcpy(spt, out, LEN_BUF * sizeof(short));
		}
		else if (filter_on == FIR) {
			FIR_filter(spt, out, fir_coef, LEN_BUF, fir_tap);
			memcpy(spt, out, LEN_BUF * sizeof(short)); 
		}
		memcpy(header[iBuf].lpData, spt, LEN_BUF * sizeof(short));
		if (save_flag) {
			fwrite(spt, sizeof(short), LEN_BUF, fp);
		}
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
int main(int argc, _TCHAR* argv[])
{
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
	
	char key = 'r';
	do {
		key = _getch();
		if (key == 'm') {
			printf("Enter taps of the Filter : ");
			scanf("%d", &tap);
			printf("Normal = 0, Weighted = 1 : ");
			scanf("%d", &filter_on);
			if (filter_on == 0) {
				printf("Moving Average Filter on\n\n");
			}
			else {
				printf("Weighted Moving Average Filter on\n\n");
			}
		}
		if (key == 'f') {
			printf("FIR filter on");
			printf("Cutoff freqency (in Hz) : "); scanf("%f", &fc);
			printf("Filter length (in samples, should be odd number) : "); scanf("%d", &fir_tap);
			printf("Window type(0 = Rect / 1 = Hann / 2 = Hamm / 3 = Tria / 4 = Blackman) "); scanf("%d", &w_type);
			if (fir_coef != NULL) free(fir_coef);
			fir_coef = (float *)malloc(sizeof(float)*fir_tap);
			GetFIRcoefs(fir_coef, fc, fir_tap, w_type, Samplerate);
			printf("(L)PF or (H)PF ");
			key = _getch();
			if (key == 'H' || key == 'h') {
				printf("HPF selected.\n"); GetHPFcoefs(fir_coef, fir_tap);
			}
			else
				printf("LPF selected.\n");
			filter_on = FIR;
		}//circular convolution 구현하기
		if (key == 'r') {
			printf("Filter off\n\n");
			filter_on = -1;
		}
		if (key == 's'){
			if (!save_flag) {
				printf("File save mode. Enter your .raw filename : ");
				cin >> fname;
				if (fp = fopen(fname, "w")) {
					printf("\nnow recording\n\n");
				}
				else{
					printf("\nFile open error\n\n");
				}
				save_flag = true;
			}
			else {
				if (!fclose(fp)) {
					printf("File saved\n\n");
				}
				else {
					printf("Can't save the file\n\n");
				}
				save_flag = false;
			}
		}
	} while (key != 'e' && key != 'q');

	waveInClose(hWaveIn);
	waveOutClose(hWaveOut);
	CloseHandle(hThread);
	CloseHandle(hThreadPlay);
	CloseHandle(hEvent_BufferReady);
	CloseHandle(hEvent_FinishedPlaying);
	return 0;
}