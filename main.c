#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>

// MATRIX
#define MATRIX_M 10000
#define MATRIX_N 10000
#define MATRIX_LEN MATRIX_M * MATRIX_N
int matrix[MATRIX_M][MATRIX_N];

// QUANTIDADE DE COISAS
#define QTD_ELEM 100
#define QTD_MB MATRIX_LEN / QTD_ELEM
#define RANDOM_SEED 0.0000005

// CONTROLE
int lr, lc = 0;
int primos = 0;

// Pthreads
#define QTD_THREADS 8
pthread_t tid[QTD_THREADS];
pthread_mutex_t mutex;

void *rotina(void *arg);
int verificaPrimo(int num);

int verificaPrimo(int num) {
    int resultado = 0;
    for(int i = 2; i <= num / 2; i++) {
        if (num % i == 0) {
            resultado++;
            break;
        }
    }
    return resultado;
}

void *rotina(void *arg) {
    int qtd_elem_mn = sqrt(QTD_ELEM);
    int local_primos = 0;

    pthread_mutex_lock(&mutex);
    int local_lr = lr;
    int local_lc = lc;
    if(lc == (MATRIX_N - qtd_elem_mn)) {
        lr += qtd_elem_mn;
        lc = 0;
    } else {
        lc += qtd_elem_mn;
    }
    pthread_mutex_unlock(&mutex);

    while(local_lr < MATRIX_M && local_lc < MATRIX_N) {
        for(int i = local_lr; i < local_lr + qtd_elem_mn; i++) {
            for(int j = local_lc; j < local_lc + qtd_elem_mn; j++) {
                if(verificaPrimo(matrix[i][j]) == 0) {
                    local_primos++;
                }
            }
        }

        pthread_mutex_lock(&mutex);
        local_lr = lr;
        local_lc = lc;
        if(lc == (MATRIX_N - qtd_elem_mn)) {
            lr += qtd_elem_mn;
            lc = 0;
        } else {
            lc += qtd_elem_mn;
        }
        pthread_mutex_unlock(&mutex);
    }

    printf("Local primos: %d\n", local_primos);
    pthread_exit((void*)NULL);

}

int main() {
    int i;
    srand(RANDOM_SEED);

    for(i = 0; i < MATRIX_M; i++) {
        for(int j = 0; j < MATRIX_N; j++) {
            matrix[i][j] = rand() % 29999;
        }
    }

    for(i = 0; i < QTD_THREADS; i++) {
        pthread_create(&(tid[i]), NULL, rotina, (void *)NULL);
    }

    for(i = 0; i < QTD_THREADS; i++) {
        pthread_join(tid[i], NULL);
    }

    printf("Numeros primos: %d\n", primos);

    pthread_mutex_destroy(&mutex);
}
