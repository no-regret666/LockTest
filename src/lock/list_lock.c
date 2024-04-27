#include "list_lock.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void listInit(list_lock_t* list) {
  if (pthread_mutex_init(&(list->mutex), NULL) != 0) {
    perror("pthread_mutex_init");
    exit(EXIT_FAILURE);
  }
}

void producer(list_lock_t* list, DataType value) {
  if (pthread_mutex_lock(&(list->mutex)) != 0) {
    perror("pthread_mutex_lock");
    exit(EXIT_FAILURE);
  }
  LinkList node = (LinkList)malloc(sizeof(LNode));
  if (node == NULL) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }
  node->value = value;
  node->next = NULL;
  if (list->head == NULL) {
    list->head = node;
  } else {
    LinkList cur = list->head;
    while (cur->next) {
      cur = cur->next;
    }
    cur->next = node;
  }
  if (pthread_mutex_unlock(&(list->mutex)) != 0) {
    perror("pthread_mutex_unlock");
    exit(EXIT_FAILURE);
  }
  if (pthread_cond_signal(&(list->cond)) != 0) {
    perror("pthread_cond_signal");
    exit(EXIT_FAILURE);
  }
}

void consumer(list_lock_t* list) {
  if (pthread_mutex_lock(&(list->mutex)) != 0) {
    perror("pthread_mutex_lock");
    exit(EXIT_FAILURE);
  }
  while (list->head == NULL) {
    pthread_cond_wait(&(list->cond), &(list->mutex));
  }
  if (list->head->next == NULL) {
    free(list->head);
    list->head = NULL;
  } else {
    LinkList cur = list->head;
    while (cur->next->next) {
      cur = cur->next;
    }
    free(cur->next);
    cur->next = NULL;
  }
  if (pthread_mutex_unlock(&(list->mutex)) != 0) {
    perror("pthread_mutex_unlock");
    exit(EXIT_FAILURE);
  }
}

int getListSize(list_lock_t* list) {
  if (pthread_mutex_lock(&(list->mutex)) != 0) {
    perror("pthread_mutex_lock");
    exit(EXIT_FAILURE);
  }
  if (list->head == NULL) {
    if (pthread_mutex_unlock(&(list->mutex)) != 0) {
      perror("pthread_mutex_unlock");
      exit(EXIT_FAILURE);
    }
    return 0;
  }
  LinkList cur = list->head;
  int size = 1;
  while (cur->next) {
    size++;
    cur = cur->next;
  }
  if (pthread_mutex_unlock(&(list->mutex)) != 0) {
    perror("pthread_mutex_unlock");
    exit(EXIT_FAILURE);
  }
  return size;
}