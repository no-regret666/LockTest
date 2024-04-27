#include "hash_lock.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void hashInit(hash_lock_t* bucket) {
  for (int i = 0; i < HASHNUM; i++) {
    if (pthread_mutex_init(&(bucket->table[i].mutex), NULL) != 0) {
      perror("pthread_mutex_init");
      exit(EXIT_FAILURE);
    }
  }
}

int getValue(hash_lock_t* bucket, int key) {
  struct HashTable target = bucket->table[HASH(key)];
  if (pthread_mutex_lock(&(target.mutex)) != 0) {
    perror("pthread_mutex_lock");
    exit(EXIT_FAILURE);
  }
  Hlist cur = target.head;
  while (cur != NULL) {
    if (cur->key == key) {
      return cur->value;
    }
    cur = cur->next;
  }
  if (pthread_mutex_unlock(&(target.mutex)) != 0) {
    perror("pthread_mutex_unlock");
    exit(EXIT_FAILURE);
  }
  return -1;
}

void insert(hash_lock_t* bucket, int key, int value) {
  struct HashTable target = bucket->table[HASH(key)];
  if (pthread_mutex_lock(&(target.mutex)) != 0) {
    perror("pthread_mutex_lock");
    exit(EXIT_FAILURE);
  }
  Hlist cur = target.head;
  if (cur != NULL) {
    while (cur->next != NULL) {
      if (cur->key == key) {
        cur->value = value;
        return;
      }
      cur = cur->next;
    }
    if (cur->key == key) {
      cur->value = value;
      return;
    }
  }
  Hlist newNode = (Hlist)malloc(sizeof(Hnode));
  newNode->value = value;
  newNode->key = key;
  if (cur == NULL) {
    target.head = newNode;
  } else {
    cur->next = newNode;
  }
  if (pthread_mutex_unlock(&(target.mutex)) != 0) {
    perror("pthread_mutex_unlock");
    exit(EXIT_FAILURE);
  }
}

int setKey(hash_lock_t* bucket, int key, int new_key) {
  struct HashTable target = bucket->table[HASH(key)];
  if (pthread_mutex_lock(&(target.mutex)) != 0) {
    perror("pthread_mutex_lock");
    exit(EXIT_FAILURE);
  }
  Hlist cur = target.head;
  Hlist targetNode = (Hlist)malloc(sizeof(Hnode));
  while (cur != NULL) {
    if (key == cur->key) {
      targetNode = cur;
      cur->key = new_key;
      cur->next = cur->next->next;
    }
    cur = cur->next;
  }
  if (pthread_mutex_unlock(&(target.mutex)) != 0) {
    perror("pthread_mutex_unlock");
    exit(EXIT_FAILURE);
  }
  if (cur == NULL) {
    return -1;
  }

  target = bucket->table[HASH(targetNode->key)];
  if (pthread_mutex_lock(&(target.mutex)) != 0) {
    perror("pthread_mutex_lock");
    exit(EXIT_FAILURE);
  }
  cur = target.head;
  while (cur->next != NULL) {
    cur = cur->next;
  }
  cur->next = targetNode;
  if (pthread_mutex_unlock(&(target.mutex)) != 0) {
    perror("pthread_mutex_unlock");
    exit(EXIT_FAILURE);
  }
  return -1;
}