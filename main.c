#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>

// MATRIX
#define MATRIX_M 10000
#define MATRIX_N 10000
#define MATRIX_LEN MATRIX_M * MATRIX_N
int matrix[MATRIX_M][MATRIX_N];

// MACROBLOCO
#define MACROBLOCO_M 5000
#define MACROBLOCO_N 5000
#define QTD_ELEM MACROBLOCO_M * MACROBLOCO_N
#define QTD_MB MATRIX_LEN / QTD_ELEM

// CONTROLE SECAO CRITICA
int lr, lc = 0;
int primos = 0;

// THREADS
#define QTD_THREADS 4
pthread_t tid[QTD_THREADS];
pthread_mutex_t mutex;

// RANDOM
#define RANDOM_SEED 0.0000005

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

int verificaPrimo2(int num) {
    int d, EhPrimo;

    if (num <= 1 || (num != 2 && num % 2 == 0))
        EhPrimo = 0;	/* numenhum numero inteiro <= 1 ou par > 2 e' primo */
    else
        EhPrimo = 1;		/* o numero e' primo ate que se prove o contrario */
        
    d = 3;
    while (EhPrimo  && d <= num / 2) {
        if (num % d == 0)
            EhPrimo = 0;
        d = d + 2;		/* testamos so' os  impares: 3, 5, 7... */
    }
}

void *rotina(void *arg) {
    int qtd_elem_mn = sqrt(QTD_ELEM);
    int local_primos = 0;

    pthread_mutex_lock(&mutex);
    int local_lr = lr;
    int local_lc = lc;
    if(lc == (MATRIX_N - MACROBLOCO_N)) {
        lr += MACROBLOCO_M;
        lc = 0;
    } else {
        lc += MACROBLOCO_N;
    }
    pthread_mutex_unlock(&mutex);

    while(local_lr < MATRIX_M && local_lc < MATRIX_N) {
        for(int i = local_lr; i < local_lr + MACROBLOCO_M; i++) {
            for(int j = local_lc; j < local_lc + MACROBLOCO_N; j++) {
                if(verificaPrimo2(matrix[i][j])) {
                    local_primos++;
                }
            }
        }

        pthread_mutex_lock(&mutex);
        local_lr = lr;
        local_lc = lc;
        if(lc == (MATRIX_N - MACROBLOCO_N)) {
            lr += MACROBLOCO_M;
            lc = 0;
        } else {
            lc += MACROBLOCO_N;
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
