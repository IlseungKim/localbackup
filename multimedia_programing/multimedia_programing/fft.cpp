#pragma comment(lib,"libfftw3-3.lib")
#include "fftw3.h"
#include "dft.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ITER 50

int main(int argc, char **argv)
{
	fftw_complex *data, *fft_result, *ifft_result;
	fftw_plan plan_forward, plan_backward;
	int i, F_SIZE;
	COMPLEX *ft_in, *ft_out;
	clock_t start, end;
	printf("Enter FFT SIZE : ");
	scanf("%d", &F_SIZE);
	ft_in = (COMPLEX *)malloc(sizeof(COMPLEX)*F_SIZE);
	ft_out = (COMPLEX *)malloc(sizeof(COMPLEX)*F_SIZE);
	for (i = 0; i<F_SIZE; i++) {
		ft_in[i].real = i;
		ft_in[i].imag = i * 0.5;
	}
	start = clock();
	for (i = 0; i < ITER; i++) {
		dft(ft_in, ft_out, F_SIZE);
		idft(ft_out, ft_in, F_SIZE);
	}
	end = clock();
	printf("[DFT]Eclipse time = %6.3f sec. \n", (end - start)*0.001);
	start = clock();
	for (i = 0; i < ITER; i++) {
		fft(ft_in, log2((unsigned)F_SIZE));
		ifft(ft_in, log2((unsigned)F_SIZE));
	}
	end = clock();
	printf("[FFT]Eclipse time = %6.3f sec. \n", (end - start)*0.001);
	data = (fftw_complex *)fftw_malloc(sizeof(fftw_complex)*F_SIZE);
	fft_result = (fftw_complex *)fftw_malloc(sizeof(fftw_complex)*F_SIZE);
	ifft_result = (fftw_complex *)fftw_malloc(sizeof(fftw_complex)*F_SIZE);
	plan_forward = fftw_plan_dft_1d(F_SIZE, data, fft_result, FFTW_FORWARD, FFTW_ESTIMATE);
	plan_backward = fftw_plan_dft_1d(F_SIZE, fft_result, ifft_result, FFTW_BACKWARD, FFTW_ESTIMATE);
	for (i = 0; i<F_SIZE; i++) {
		data[i][0] = i;
		data[i][1] = i * 0.5;
	}
	start = clock();
	for (i = 0; i < ITER; i++) {
		fftw_execute(plan_forward);
		fftw_execute(plan_backward);
	}
	end = clock();
	printf("FFTW]Eclipse time = %6.3f sec. \n", (end - start)*0.001);
	fftw_destroy_plan(plan_forward);
	fftw_destroy_plan(plan_backward);
	fftw_free(data);
	fftw_free(fft_result);
	fftw_free(ifft_result);
	return 0;
}
