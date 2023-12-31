#include <stdio.h>
#include <stdlib.h>

#define N 1024

struct data_set {
    double time;
    double data;
};

int get_data_num();
void read_csv(struct data_set*, int);
int get_area_num(int);
void get_new_signal(struct data_set*, struct data_set*, int);
double calc_mean(struct data_set*);
void write_csv(char*, struct data_set*);

int main(void)
{
    char* fname;
    struct data_set* x;
    struct data_set x_area_prime[N];
    int data_num;
    int area_num;
    int k = 0;

    fname = (char*)malloc(sizeof(char) * 32);

    data_num = get_data_num();

    x = (struct data_set*)malloc(sizeof(struct data_set) * data_num);

    read_csv(x, data_num);

    area_num = get_area_num(data_num);

    for (int no = 0; no < area_num; no++) {
        get_new_signal(x, x_area_prime, k);

        sprintf(fname, "13-1_result-%d.csv", no + 1);

        write_csv(fname, x_area_prime);

        k += (N / 2);
    }

    free(fname);

    free(x);

    return 0;
}

int get_data_num()
{
    FILE* fp;
    char buf[4096];
    int count = 0;

    fp = fopen("data13x.csv", "r");

    while (fgets(buf, 4096, fp) != NULL) {
        count++;
    }

    fclose(fp);

    return count;
}

void read_csv(struct data_set* x, int data_num)
{
    FILE* fp;

    fp = fopen("data13x.csv", "r");

    for (int k = 0; k < data_num; k++) {
        fscanf(fp, "%lf,%lf\n", &x[k].time, &x[k].data);
    }

    fclose(fp);

    return;
}

int get_area_num(int data_num) {
    int count = 0;

    for (int k = 0; k <= data_num - N; k += N / 2) {
        for (int i = k; i < k + N; i++) {
            count++;
        }
    }

    return count / N;
}

void get_new_signal(struct data_set* x, struct data_set* x_area_prime, int k)
{
    struct data_set x_area[N];
    double x_area_mean;

    for (int i = 0; i < N; i++) {
        x_area[i].time = x[k].time;
        x_area[i].data = x[k].data;
        k++;
    }

    x_area_mean = calc_mean(x_area);

    for (int i = 0; i < N; i++) {
        x_area_prime[i].time = x_area[i].time;
        x_area_prime[i].data = x_area[i].data - x_area_mean;
    }

    return;
}

double calc_mean(struct data_set* x_area)
{
    double mean = 0.0;

    for (int i = 0; i < N; i++) {
        mean += x_area[i].data / N;
    }

    return mean;
}

void write_csv(char* fname, struct data_set* x_area_prime)
{
    FILE* fp;
    
    fp = fopen(fname, "w");

    for (int i = 0; i < N; i++) {
        fprintf(fp, "%lf,%lf\n", x_area_prime[i].time, x_area_prime[i].data);
    }

    fclose(fp);

    return;
}