#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "fft.h"

#define N 16384
#define T 0.001

struct data_set {
    double time;
    double data;
};

int read_csv(char*, struct data_set*);
void fast_fourier_transform(double*, double*, struct data_set*);
void power_spectral_density(double*, double*, double*);
void write_csv(char*, double*);

int main(void)
{
    char* fname;
    struct data_set* x_area_prime;
    double* real;
    double* imag;
    double* psd;

    fname = (char*)malloc(sizeof(char) * 32);

    x_area_prime = (struct data_set*)malloc(sizeof(struct data_set) * N);

    real = (double*)malloc(sizeof(double) * N);

    imag = (double*)malloc(sizeof(double) * N);

    psd = (double*)malloc(sizeof(double) * N);

    for (int no = 0; no < 9; no++) {
        sprintf(fname, "14-1_result-%d.csv", no + 1);

        read_csv(fname, x_area_prime);

        fast_fourier_transform(real, imag, x_area_prime);

        power_spectral_density(psd, real, imag);

        sprintf(fname, "14-2_result-%d.csv", no + 1);

        write_csv(fname, psd);
    }

    free(fname);

    free(x_area_prime);

    free(real);

    free(imag);

    free(psd);

    return 0;
}

int read_csv(char* fname, struct data_set* x_area_prime)
{
    FILE* fp;

    fp = fopen(fname, "r");

    for (int i = 0; i < N; i++) {
        fscanf(fp, "%lf,%lf\n", &x_area_prime[i].time, &x_area_prime[i].data);
    }

    fclose(fp);

    return 0;
}

void fast_fourier_transform(double* real, double* imag, struct data_set* x_area_prime)
{
    for (int i = 0; i < N; i++) {
        real[i] = x_area_prime[i].data;
        imag[i] = 0.0;
    }

    FFT_Window(N, real);

    fft(N, real, imag);

    return;
}

void power_spectral_density(double* psd_area, double* real, double* imag)
{
    double x_area_prime_abs[N], p[N];
    double f = 1.0 / (N * T);

    for (int j = 0; j < N; j++) {
        x_area_prime_abs[j] = sqrt(real[j] * real[j] + imag[j] * imag[j]) / N;
        p[j] = pow(x_area_prime_abs[j], 2.0);
        psd_area[j] = p[j] / f;
    }

    for (int j = 0; j < N / 2; j++) {
        x_area_prime_abs[j] = x_area_prime_abs[j] + x_area_prime_abs[N - j];
        p[j] = p[j] + p[N - j];
        psd_area[j] = psd_area[j] + psd_area[N - j];
    }

    return;
}

void write_csv(char* fname, double* psd_area)
{
    FILE* fp;
    double f = 1.0 / (N * T);

    fp = fopen(fname, "w");

    for (int j = 0; j < N / 2; j++) {
        fprintf(fp, "%lf,%lf\n", j * f, psd_area[j]);
    }

    fclose(fp);

    return;
}