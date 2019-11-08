#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>

// MATRIX
#define MATRIX_M 10000
#define MATRIX_N 10000
long int matrix[MATRIX_M][MATRIX_N];

// MACROBLOCO
#define MACROBLOCO_M 500
#define MACROBLOCO_N 500
#define QTD_ELEM MACROBLOCO_M * MACROBLOCO_N

// CONTROLE SECAO CRITICA
int lr, lc = 0;
int primos = 0;

// THREADS
#define QTD_THREADS 4
pthread_t tid[QTD_THREADS];
pthread_mutex_t mutex;
int thstatus;

// RANDOM
#define RANDOM_SEED 1


void percorreMatrizSerial();
void percorreMatrizParalelo();
void* rotinaMacrobloco(void* arg);
int verificaPrimo(int num);

int verificaPrimo(int num) {

	int d, EhPrimo;

	if (num <= 1 || (num != 2 && num % 2 == 0))
		EhPrimo = 0;
	else
		EhPrimo = 1;

	d = 3;
	while (EhPrimo && d <= sqrt(num)) {
		if (num % d == 0)
			EhPrimo = 0;
		d = d + 2;
	}

	return EhPrimo;
}


void percorreMatrizSerial() {

	for (int i = 0; i < MATRIX_M; i++) {
		for (int j = 0; j < MATRIX_N; j++) {
			if (verificaPrimo(matrix[i][j]) == 1) {
				primos++;
			}
		}
	}
}

void percorreMatrizParalelo() {
	int i;

	pthread_mutex_init(&mutex, NULL);

	for (i = 0; i < QTD_THREADS; i++) {
		thstatus = pthread_create(&(tid[i]), NULL, rotinaMacrobloco, (void*)NULL);
		if (thstatus != 0) {
			printf("Erro na criação da Thread %d\n", i);
		}
	}
	for (i = 0; i < QTD_THREADS; i++) {
		thstatus = pthread_join(tid[i], NULL);
		if (thstatus != 0) {
			printf("Erro ao chamar a thread %d\n", i);
		}
	}
}

void* rotinaMacrobloco(void* arg) {
	int local_primos = 0;
	int local_lr, local_lc;

	pthread_mutex_lock(&mutex);
	local_lr = lr;
	local_lc = lc;
	if (lc == (MATRIX_N - MACROBLOCO_N)) {
		lr += MACROBLOCO_M;
		lc = 0;
	}
	else {
		lc += MACROBLOCO_N;
	}
	pthread_mutex_unlock(&mutex);

	while (local_lr < MATRIX_M && local_lc < MATRIX_N) {
		for (int i = local_lr; i < local_lr + MACROBLOCO_M; i++) {
			for (int j = local_lc; j < local_lc + MACROBLOCO_N; j++) {
				if (verificaPrimo(matrix[i][j]) == 1) {
					local_primos++;
				}
			}
		}

		pthread_mutex_lock(&mutex);
		primos = primos + local_primos;
		local_lr = lr;
		local_lc = lc;
		if (lc == (MATRIX_N - MACROBLOCO_N)) {
			lr += MACROBLOCO_M;
			lc = 0;
		}
		else {
			lc += MACROBLOCO_N;
		}
		pthread_mutex_unlock(&mutex);

		local_primos = 0;
	}

	pthread_exit((void*)NULL);
}


int main() {
	clock_t tempo_inicial, tempo_final;
	srand(RANDOM_SEED);

	// Inicializacao da Matriz
	for (int i = 0; i < MATRIX_M; i++) {
		for (int j = 0; j < MATRIX_N; j++) {
			matrix[i][j] = rand() % 29999;
		}
	}

	// Execução serial
	tempo_inicial = clock();
	percorreMatrizSerial();
	tempo_final = clock();

	printf("Tempo gasto em modo serial: %g ms.\n", (tempo_final - tempo_inicial) * 1000.0 / CLOCKS_PER_SEC);
	printf("Numeros primos encontrados: %d\n", primos);
	primos = 0;

	// Busca paralela
	tempo_inicial = clock();
	percorreMatrizParalelo();
	tempo_final = clock();

	printf("Tempo gasto em modo paralelo: %g ms.\n", (tempo_final - tempo_inicial) * 1000.0 / CLOCKS_PER_SEC);
	printf("Numeros primos encontrados: %d\n", primos);

	pthread_mutex_destroy(&mutex);

}
