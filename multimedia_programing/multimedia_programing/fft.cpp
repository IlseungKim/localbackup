#pragma comment(lib,"libfftw3-3.lib")
#include "fftw3.h"
#include "dft.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

fftw_complex *data, *fft_result, *ifft_result;
fftw_plan plan_forward, plan_backward;
unsigned int i, F_SIZE,ITER;
COMPLEX *ft_in, *ft_out;

int fft_test(short *in, short *out, unsigned long long LEN_BUF)
{
	printf("type ITERATION : ");
	scanf("%d", &ITER);
	FILE *fp_fft;
	clock_t start, end;
	unsigned long rmse = 0;
	F_SIZE = 2;
	while (F_SIZE < LEN_BUF) {
		F_SIZE *= 2;
	}

	printf("ITER : %d, FFT SIZE : %llu\n",ITER ,F_SIZE);

	ft_in = (COMPLEX *)malloc(sizeof(COMPLEX)*F_SIZE);
	ft_out = (COMPLEX *)malloc(sizeof(COMPLEX)*F_SIZE);
	for (i = 0; i<LEN_BUF; i++) {
		ft_in[i].real = in[i];
		ft_in[i].imag = 0;
	}
	for (i = LEN_BUF; i<F_SIZE; i++) {
		ft_in[i].real = 0;
		ft_in[i].imag = 0;
	}

	start = clock();
	for (i = 0; i < ITER; i++) {
		dft(ft_in, ft_out, F_SIZE);
		idft(ft_out, ft_in, F_SIZE);
	}
	end = clock();

	//FILE write
	char fname[100] = "dft.snd";
	fp_fft = fopen(fname, "wb");
	rmse = 0;
	for (i = 0; i < LEN_BUF; ++i) {
		out[i] = ft_in[i].real;
		rmse += (out[i] - in[i])*(out[i] - in[i]);
	}
	fwrite(out, sizeof(short), LEN_BUF, fp_fft);
	fclose(fp_fft);
	
	printf(" [DFT]Eclipse time = %6.3f sec. RMSE = %6.3f\n", (end - start)*0.001,(float)rmse/LEN_BUF);
	start = clock();
	for (i = 0; i < ITER; i++) {
		fft(ft_in, log2((unsigned)F_SIZE));
		ifft(ft_in, log2((unsigned)F_SIZE));
	}
	end = clock();
	
	//FILE WRITE FOR FFT 
	char fname1[100] = "fft.snd";
	fp_fft = fopen(fname1, "wb");
	rmse = 0;
	for (i = 0; i < LEN_BUF; ++i) {
		out[i] = ft_in[i].real;
		rmse += (out[i] - in[i])*(out[i] - in[i]);
	}
	fwrite(out, sizeof(short), LEN_BUF, fp_fft);
	fclose(fp_fft);

	printf(" [FFT]Eclipse time = %6.3f sec. RMSE = %6.3f\n", (end - start)*0.001,(float)rmse/LEN_BUF);
	data = (fftw_complex *)fftw_malloc(sizeof(fftw_complex)*F_SIZE);
	fft_result = (fftw_complex *)fftw_malloc(sizeof(fftw_complex)*F_SIZE);
	ifft_result = (fftw_complex *)fftw_malloc(sizeof(fftw_complex)*F_SIZE);
	plan_forward = fftw_plan_dft_1d(F_SIZE, data, fft_result, FFTW_FORWARD, FFTW_ESTIMATE);
	plan_backward = fftw_plan_dft_1d(F_SIZE, fft_result, ifft_result, FFTW_BACKWARD, FFTW_ESTIMATE);
	for (i = 0; i<LEN_BUF; i++) {
		data[i][0] = in[i];
		data[i][1] = 0;
	}
	for (i = LEN_BUF; i<F_SIZE; i++) {
		data[i][0] = 0;
		data[i][1] = 0;
	}

	start = clock();
	for (i = 0; i < ITER; i++) {
		fftw_execute(plan_forward);
		fftw_execute(plan_backward);
	}
	end = clock();

	//FILE WRITE FOR FFT 
	char fname2[100] = "fftw.snd";
	fp_fft = fopen(fname2, "wb");
	rmse = 0;
	for (i = 0; i < LEN_BUF; ++i) {
		out[i] = ifft_result[i][0] / F_SIZE;
		rmse += (out[i] - in[i])*(out[i] - in[i]);
	}
	fwrite(out, sizeof(short), LEN_BUF, fp_fft);
	fclose(fp_fft);

	printf("[FFTW]Eclipse time = %6.3f sec. RMSE = %6.3f\n", (end - start)*0.001,(float)rmse/LEN_BUF);
	fftw_destroy_plan(plan_forward);
	fftw_destroy_plan(plan_backward);
	fftw_free(data);
	fftw_free(fft_result);
	fftw_free(ifft_result);
	return 0;
}
