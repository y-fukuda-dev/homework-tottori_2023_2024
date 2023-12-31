#include <stdio.h>
#include <stdlib.h>

#define N 1024

struct time_data_set {
    double time;
    double data;
};

struct freq_data_set {
    double freq;
    double data;
};

void read_csv_t(char*, struct time_data_set*);
void read_csv_f(char*, struct freq_data_set*);
int get_max_value_index(struct freq_data_set*);
void write_csv(struct time_data_set*);

int main(void)
{
    char* fname;
    struct time_data_set* result;
    struct time_data_set* x_area;
    struct freq_data_set* psd_area;
    int max_value_index;

    fname = (char*)malloc(sizeof(char) * 32);

    result = (struct time_data_set*)malloc(sizeof(struct time_data_set) * 15);

    x_area = (struct time_data_set*)malloc(sizeof(struct time_data_set) * N);

    psd_area = (struct freq_data_set*)malloc(sizeof(struct freq_data_set) * (N / 2));

    for (int no = 0; no < 15; no++) {
        sprintf(fname, "13-1_result-%d.csv", no + 1);

        read_csv_t(fname, x_area);

        result[no].time = x_area[0].time;

        sprintf(fname, "13-2_result-%d.csv", no + 1);

        read_csv_f(fname, psd_area);

        max_value_index = get_max_value_index(psd_area);

        result[no].data = psd_area[max_value_index].freq * 60;
    }

    write_csv(result);

    free(fname);

    free(result);

    free(x_area);

    free(psd_area);

    return 0;
}

void read_csv_t(char* fname, struct time_data_set* x_area)
{
    FILE* fp;

    fp = fopen(fname, "r");

    for (int k = 0; k < N; k++) {
        fscanf(fp, "%lf,%lf\n", &x_area[k].time, &x_area[k].data);
    }

    fclose(fp);

    return;
}


void read_csv_f(char* fname, struct freq_data_set* psd_area)
{
    FILE* fp;

    fp = fopen(fname, "r");

    for (int j = 0; j < N / 2; j++) {
        fscanf(fp, "%lf,%lf\n", &psd_area[j].freq, &psd_area[j].data);
    }

    fclose(fp);

    return;
}

int get_max_value_index(struct freq_data_set* psd_area)
{
    int index = 0;
    double value = psd_area[index].data;

    for (int j = 1; j < N / 2; j++) {
        if (value < psd_area[j].data) {
            value = psd_area[j].data;
            index = j;
        }
    }

    return index;
}

void write_csv(struct time_data_set* result)
{
    FILE* fp;

    fp = fopen("13-3_result.csv", "w");

    for (int j = 0; j < 15; j++) {
        fprintf(fp, "%lf,%lf\n", result[j].time, result[j].data);
    }

    fclose(fp);

    return;
}