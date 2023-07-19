#include "ponte.h"

sem_t macaco_sem; //controlar capacidade
sem_t gorila_sem; //binário, afinal só pode ter 1 gorila por vez atravessando
sem_t ponte_sem; //

int macacos_na_ponte = 0;
int gorila_na_ponte = 0;
int direcao_da_ponte = 1;

int testa_macacos(){
    pthread_t macacos[MAX_MACACOS]; // 0 a 9 são macacos, 10 e 11 são gorilas (adicionar depois)
    pthread_t gorilas[NUM_GORILAS]; // 0 a 9 são macacos, 10 e 11 são gorilas (adicionar depois)

    sem_init(&ponte_sem, 0, 1);
    sem_init(&macaco_sem, 0, 1);

    for(int i = 0; i < MAX_MACACOS; i++){
        int* id = malloc(sizeof(int));
        *id = 1 + i; //para ficar com id de 1 a 10
        if(pthread_create(&macacos[i], NULL, &thread_macaco, id) != 0){
            fprintf(stderr, "Erro ao criar thread %d\n", i);
            return 1;
        }
    }

    //Esperar todas as threads acabarem
    for(int i = 0; i < MAX_MACACOS; i++){
        if(pthread_join(macacos[i], NULL) != 0){
            fprintf(stderr, "Error ao fazer join na thread %d\n", i);
            return 3;
        }
    }

    sem_destroy(&macaco_sem);
    sem_destroy(&ponte_sem);
}

int testa_gorilas(){
    pthread_t macacos[MAX_MACACOS];
    pthread_t gorilas[NUM_GORILAS];

    sem_init(&ponte_sem, 0, 1);
    sem_init(&macaco_sem, 0, 1);
    sem_init(&gorila_sem, 0, 1);

    for(int i = 0; i < MAX_MACACOS; i++){
        int* id = malloc(sizeof(int));
        *id = 1 + i; //para ficar com id de 1 a 10
        if(pthread_create(&macacos[i], NULL, &thread_macaco, id) != 0){
            fprintf(stderr, "Erro ao criar thread %d\n", i);
            return 1;
        }
    }

    for(int i = 0; i < NUM_GORILAS; i++){
        int* id = malloc(sizeof(int));
        *id = 1 + i; //para ficar com id de 1 a 2
        if(pthread_create(&gorilas[i], NULL, &thread_gorila, id) != 0){
            fprintf(stderr, "Erro ao criar thread %d\n", i);
            return 1;
        }
    }

    //Esperar todas as threads acabarem
    for(int i = 0; i < MAX_MACACOS; i++){
        if(pthread_join(macacos[i], NULL) != 0){
            fprintf(stderr, "Error ao fazer join na thread %d\n", i);
            return 3;
        }
    }

    for(int i = 0; i < NUM_GORILAS; i++){
        if(pthread_join(gorilas[i], NULL) != 0){
            fprintf(stderr, "Error ao fazer join na thread %d\n", i);
            return 3;
        }
    }

    sem_destroy(&macaco_sem);
    sem_destroy(&ponte_sem);
    sem_destroy(&gorila_sem);
}

void* thread_macaco(void* arg){
    int* id = (int*)arg;

    int minha_direcao = *id % 2;

    while (1)
    {
        sem_wait(&macaco_sem);  // espera os macacos da outra direcao da ponte
        sem_wait(&ponte_sem);   // verifica se tiver espaco disponivel na ponte

        if((macacos_na_ponte == 0 || minha_direcao == direcao_da_ponte) && gorila_na_ponte == 0){ // se for possivel o macaco subir na ponte
            macacos_na_ponte++;     // ele sobe na ponte
            direcao_da_ponte = minha_direcao; // atualiza direcao da ponte
            sem_post(&macaco_sem);  // libera pro proximo macaco subir
            sem_post(&ponte_sem);   // libera pro proximo macaco subir
            break;
        }
        else{   // quando nao for possivel subir na ponte
            sem_post(&macaco_sem);  // libera pro proximo macaco tentar subir
            sem_post(&ponte_sem);   // libera pro proximo macaco tentar subir
        }
    }
    
    if(minha_direcao == 0){
        printf("Eu sou macaco-%d: ... Estou atravessando a ponte para direita!\n",*id);
    }
    else{
        printf("Eu sou macaco-%d: ... Estou atravessando a ponte para esquerda!\n",*id);
    }
    sleep(3);   // tempo que o macaco leva para atravessar

    printf("Eu sou macaco-%d: ... Atravessei a ponte!\n",*id);
    
    sem_wait(&ponte_sem);   // macaco descendo da ponte
    macacos_na_ponte--;     // macaco desce da ponte
    sem_post(&ponte_sem);   // libera o lugar que o macaco estava

    free(arg);
}

void* thread_gorila(void* arg){
    int* id = (int*)arg;
    int minha_direcao = *id % 2;

    sem_wait(&gorila_sem);
    while (1)
    {
        if(macacos_na_ponte == 0 && gorila_na_ponte == 0){  // se nao tiver macaco na ponte
            gorila_na_ponte++;     // ele sobe na ponte
            break;
        }
    }
    
    if(minha_direcao == 0){
        printf("Eu sou GORILA-%d: ... Estou atravessando a ponte para direita!\n",*id);
    }
    else{
        printf("Eu sou GORILA-%d: ... Estou atravessando a ponte para esquerda!\n",*id);
    }
    sleep(5);   // tempo que o gorila leva para atravessar

    printf("Eu sou GORILA-%d: ... Atravessei a ponte!\n",*id);

    sem_post(&ponte_sem);
    gorila_na_ponte--;     // gorila desce da ponte
    sem_post(&ponte_sem);   // libera pro proximo macaco tentar subir
    sem_post(&gorila_sem);
    free(arg);
}