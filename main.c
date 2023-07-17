#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <semaphore.h>

#define THREAD_NUM 10 
#define MAX_MACACOS 5

sem_t semaphore_esquerda;
sem_t semaphore_direita;
sem_t macacos_atravessando;

struct inf_macaco {
    int id;
    int lado;
};

typedef struct inf_macaco inf_macaco;

void print_macaco(inf_macaco b) {
    puts("");
    printf("Macaco id: %d\n", b.id);
    puts("");
}

void* atravesar(void* args) {
    inf_macaco b = *(inf_macaco*)args;

    if (b.lado == 0) {
        sem_wait(&semaphore_esquerda);
    } else {
        sem_wait(&semaphore_direita);
    }

    sem_wait(&macacos_atravessando);

    puts("===Iniciou=== travesia: ");
    print_macaco(b);

    sleep(1);

    puts("~~~Terminou~~~ travesia: ");
    print_macaco(b);

    sem_post(&macacos_atravessando);

    if (b.lado == 0) {
        sem_post(&semaphore_esquerda);
    } else {
        sem_post(&semaphore_direita);
    }

    free(args);
}

int main() {
    pthread_t th[THREAD_NUM];

    sem_init(&semaphore_esquerda, 0, MAX_MACACOS);
    sem_init(&semaphore_direita, 0, MAX_MACACOS);
    sem_init(&macacos_atravessando, 0, 1);

    for (int i = 0; i < THREAD_NUM; i++) {
        inf_macaco* a = malloc(sizeof(inf_macaco));
        a->id = i;
        if (i < 5) {
            a->lado = 0; // esquerda
        } else {
            a->lado = 1; // direita
        }

        if (pthread_create(&th[i], NULL, &atravesar, a) != 0) {
            perror("Failed to create thread ");
            printf("%d\n", i);
            return 1;
        }
    }

    for (int i = 0; i < THREAD_NUM; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join thread ");
            printf("%d\n", i);
            return 2;
        }
    }

    sem_destroy(&semaphore_esquerda);
    sem_destroy(&semaphore_direita);
    sem_destroy(&macacos_atravessando);
    return 0;
}