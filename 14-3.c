#include <stdio.h>
#include <stdlib.h>

#define N 16384

struct time_data_set {
    double time;
    double data;
};

struct freq_data_set {
    double freq;
    double data;
};

int read_csv_t(char*, struct time_data_set*);
int read_csv_f(char*, struct freq_data_set*);
double center_frequency(struct freq_data_set*);
double calc_sum(struct freq_data_set*, int);
void write_csv(struct time_data_set*);

int main(void)
{
    char* fname;
    struct time_data_set* result;
    struct time_data_set* x_area;
    struct freq_data_set* psd_area;
    int max_value_index;

    fname = (char*)malloc(sizeof(char) * 32);

    result = (struct time_data_set*)malloc(sizeof(struct time_data_set) * 9);

    x_area = (struct time_data_set*)malloc(sizeof(struct time_data_set) * N);

    psd_area = (struct freq_data_set*)malloc(sizeof(struct freq_data_set) * (N / 2));

    for (int no = 0; no < 9; no++) {
        sprintf(fname, "14-1_result-%d.csv", no + 1);

        read_csv_t(fname, x_area);

        result[no].time = x_area[0].time;

        sprintf(fname, "14-2_result-%d.csv", no + 1);

        read_csv_f(fname, psd_area);

        result[no].data = center_frequency(psd_area);
    }

    write_csv(result);

    free(fname);

    free(result);

    free(x_area);

    free(psd_area);

    return 0;
}

int read_csv_t(char* fname, struct time_data_set* x_area)
{
    FILE* fp;

    fp = fopen(fname, "r");

    for (int k = 0; k < N; k++) {
        fscanf(fp, "%lf,%lf\n", &x_area[k].time, &x_area[k].data);
    }

    fclose(fp);

    return 0;
}


int read_csv_f(char* fname, struct freq_data_set* psd_area)
{
    FILE* fp;

    fp = fopen(fname, "r");

    for (int j = 0; j < N / 2; j++) {
        fscanf(fp, "%lf,%lf\n", &psd_area[j].freq, &psd_area[j].data);
    }

    fclose(fp);

    return 0;
}

double center_frequency(struct freq_data_set* psd_area)
{
    int j = 1;
    double criteria;

    criteria = calc_sum(psd_area, N / 2) / 2.0;

    while (1) {
        if (calc_sum(psd_area, j) > criteria) {
            return psd_area[j - 1].freq;
        }

        j++;
    }
}

double calc_sum(struct freq_data_set* psd_area, int n) {
    double sum = 0.0;

    for (int j = 0; j < n; j++) {
        sum += psd_area[j].data;
    }
    
    return sum; 
}

void write_csv(struct time_data_set* result)
{
    FILE* fp;

    fp = fopen("14-3_result.csv", "w");

    for (int j = 0; j < 9; j++) {
        fprintf(fp, "%lf,%lf\n", result[j].time, result[j].data);
    }

    fclose(fp);

    return;
}