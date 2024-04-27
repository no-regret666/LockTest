#include "lock.h"

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void amountInit(lock_t* Account) {
  if(pthread_mutex_init(&(Account->mutex),NULL) != 0){
    perror("pthread_mutex_init");
    exit(EXIT_FAILURE);
  }
}

void Income(lock_t* Account, int amount) {
  if(pthread_mutex_lock(&(Account->mutex)) != 0){
    perror("pthread_mutex_lock");
    exit(EXIT_FAILURE);
  }
  Account->amount += amount;
  if(pthread_mutex_unlock(&(Account->mutex)) != 0){
    perror("pthread_mutex_unlock");
    exit(EXIT_FAILURE);
  }
}

void Expend(lock_t* Account, int amount) {
  if(pthread_mutex_lock(&(Account->mutex)) != 0){
    perror("pthread_mutex_lock");
    exit(EXIT_FAILURE);
  }
  Account->amount -= amount;
  if(pthread_mutex_unlock(&(Account->mutex)) != 0){
    perror("pthread_mutex_unlock");
    exit(EXIT_FAILURE);
  }
}