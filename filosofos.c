#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUMERO 5
#define VEZES 5

pthread_mutex_t hashi[NUMERO];
pthread_mutex_t controle;
int comidas[NUMERO] = {0};

void *filosofo(void *arg) {
    int id = *(int*)arg;
    int esq = id;
    int dir = (id + 1) % NUMERO;

    while (1) {
        pthread_mutex_lock(&controle);
        if (comidas[id] >= VEZES) {
            pthread_mutex_unlock(&controle);
            break;
        }
        pthread_mutex_unlock(&controle);

        printf("Filosofo %d pensando...\n", id);
        sleep(1);

        int pode_comer = 0;
        while (!pode_comer) {
            pthread_mutex_lock(&controle);
            int min = comidas[0];
            for (int i = 1; i < NUMERO; i++) {
                if (comidas[i] < min) {
                    min = comidas[i];
                }
            }
            if (comidas[id] == min) {
                pode_comer = 1;
            }
            pthread_mutex_unlock(&controle);
            if (!pode_comer) {
                usleep(100000);
            }
        }

        if (id % 2 == 0) {
            pthread_mutex_lock(&hashi[esq]);
            pthread_mutex_lock(&hashi[dir]);
        } else {
            pthread_mutex_lock(&hashi[dir]);
            pthread_mutex_lock(&hashi[esq]);
        }

        printf("Filosofo %d comendo (%d/%d)...\n", id, comidas[id] + 1, VEZES);
        sleep(1);

        pthread_mutex_lock(&controle);
        comidas[id]++;
        pthread_mutex_unlock(&controle);

        pthread_mutex_unlock(&hashi[esq]);
        pthread_mutex_unlock(&hashi[dir]);
    }

    printf("Filosofo %d terminou.\n", id);
    return NULL;
}

int main() {
    pthread_t t[NUMERO];
    int id[NUMERO];
    clock_t inicio, fim;
    double tempo;

    pthread_mutex_init(&controle, NULL);
    for (int i = 0; i < NUMERO; i++) {
        pthread_mutex_init(&hashi[i], NULL);
    }

    inicio = clock();

    for (int i = 0; i < NUMERO; i++) {
        id[i] = i;
        pthread_create(&t[i], NULL, filosofo, &id[i]);
    }

    for (int i = 0; i < NUMERO; i++) {
        pthread_join(t[i], NULL);
    }

    fim = clock();
    tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;

    printf("Tempo de execucao: %.2f segundos\n", tempo);

    return 0;
}
