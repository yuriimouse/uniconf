#ifndef LISTS_H
#define LISTS_H

#define _GNU_SOURCE

#include <stdlib.h>

typedef struct record
{
    struct record *next;
    void *value;
} record_t;

typedef struct list
{
    record_t *head;
    record_t *last;
} list_t;

list_t *list_construct(void);
void list_init(list_t *list);
void list_clear(list_t *list, void (*destructor)(void *));
list_t *list_destruct(list_t *list, void (*destructor)(void *));

void list_add(list_t *list, void *value); // to end
void list_push(list_t *list, void *value); // to head 
void *list_remove(list_t *list);
#define list_pop(list) list_remove(list)
void *list_get(list_t *list);

#define list_foreach(list) for (record_t *node = list->head; node; node = node->next)

#endif // LISTS_H
