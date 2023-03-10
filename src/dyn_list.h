
#ifndef QEDIT_DYN_LIST_H
#define QEDIT_DYN_LIST_H

#define INITIAL_CAPACITY 8
#define GROWTH_FACTOR 2

#define PRIMITIVE 0
#define QSTRING 1

typedef struct {
  void** data;
  int size;
  int capacity;
  int mem_alloc;
  void (*free_object)(void*);
} dyn_list;

dyn_list* new_dyn_list(int mem_alloc, void (*free_object)(void*));
void dyn_list_destroy(dyn_list* list);
void dyn_list_add(dyn_list* list, void* data);
void dyn_list_insert(dyn_list* list, int index, void* data);
void dyn_list_remove(dyn_list* list, int index);
void dyn_list_clear(dyn_list* list);
void* dyn_list_get(dyn_list* list, int index);
void dyn_list_set(dyn_list* list, int index, void* data);

#endif