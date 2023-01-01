
#include <stdlib.h>
#include <string.h>

#include "dyn_list.h"
#include "qstring.h"

dyn_list* new_dyn_list(int mem_alloc, void (*free_object)(void*)) {
  dyn_list* list = malloc(sizeof(dyn_list));
  list->data = malloc(sizeof(void*) * 8);
  list->size = 0;
  list->capacity = 8;
  list->mem_alloc = mem_alloc;
  list->free_object = free_object;
  return list;
}

void dyn_list_destroy(dyn_list* list) {
  dyn_list_clear(list);
  free(list->data);
  free(list);
}

void dyn_list_add(dyn_list* list, void* data) {
  if (list->size == list->capacity) {
    list->capacity *= 2;
    list->data = realloc(list->data, sizeof(void*) * list->capacity);
  }
  list->data[list->size++] = data;
}

void dyn_list_insert(dyn_list* list, int index, void* data) {
  if (index < 0 || index >= list->capacity) {
    return;
  }

  if (list->size == list->capacity) {
    list->capacity *= 2;
    list->data = realloc(list->data, sizeof(void*) * list->capacity);
  }

  for (int i = list->size; i > index; i--) {
    list->data[i] = list->data[i - 1];
  }
  list->data[index] = data;
  list->size++;
}

void dyn_list_remove(dyn_list* list, int index) {
  if (index < 0 || index >= list->size) {
    return;
  }
  if (list->mem_alloc)
      (*list->free_object)(list->data[index]);
  for (int i = index; i < list->size - 1; i++) {
    list->data[i] = list->data[i + 1];
  }
  list->size--;
}

void dyn_list_clear(dyn_list* list) {
  if (list->mem_alloc) {
    for (size_t i = 0; i < list->size; i++) {
      (*list->free_object)(list->data[i]);
    }
  }
  list->size = 0;
}

void* dyn_list_get(dyn_list* list, int index) {
  if (index < 0 || index >= list->size) {
    return NULL;
  }
  return list->data[index];
}

void dyn_list_set(dyn_list* list, int index, void* data) {
  if (index < 0 || index >= list->size) {
    return;
  }

  if (list->mem_alloc && index < list->size)
      (*list->free_object)(list->data[index]);
  list->data[index] = data;
}