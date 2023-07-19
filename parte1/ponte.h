#ifndef PONTE
#define PONTE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <semaphore.h>

#define MAX_ANIMAIS 10

#define MAX_MACACOS 10
#define NUM_GORILAS 2

int testa_macacos(); //retorna 0, caso seja bem sucessida, ou um valor !=0 , um código de erro
int testa_gorilas();
void* thread_macaco(void* arg);
void* thread_gorila(void* arg);


#endif