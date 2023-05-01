#include "lists.h"

/**
 * Create list
 *
 * @return list_t*
 */
list_t *list_construct(void)
{
    return calloc(1, sizeof(list_t));
}

/**
 * Init existed list
 *
 * @param list
 */
void list_init(list_t *list)
{
    if (list)
    {
        list->head = NULL;
        list->last = NULL;
    }
}

/**
 * Destroy list
 *
 * @param list
 */
void list_clear(list_t *list, void (*destructor)(void *))
{
    if (list)
    {
        while (list->head)
        {
            void *value = list_remove(list);
            if (destructor && value)
            {
                destructor(value);
            }
        }
    }
}

/**
 * Destroy and free list
 *
 * @param list
 * @param destructor
 * @return list_t*
 */
list_t *list_destruct(list_t *list, void (*destructor)(void *))
{
    if (list)
    {
        list_clear(list, destructor);
        free(list);
    }
    return NULL;
}

/**
 * Add to end of queue
 *
 * @param list
 * @param value
 */
void list_add(list_t *list, void *value)
{
    if (list)
    {
        record_t *new_node = malloc(sizeof(record_t));
        new_node->next = NULL;
        new_node->value = value;

        if (list->last)
        {
            list->last->next = new_node;
        }
        else
        {
            list->head = new_node;
        }
        list->last = new_node;
    }
}

/**
 * Push to head of stack
 *
 * @param list
 * @param value
 */
void list_push(list_t *list, void *value)
{
    if (list)
    {
        record_t *new_node = malloc(sizeof(record_t));
        new_node->next = list->head;
        new_node->value = value;

        list->head = new_node;
        if (!list->last)
        {
            list->last = new_node;
        }
    }
}

/**
 * Remove from queue head
 *
 * @param list
 * @return void*
 */
void *list_remove(list_t *list)
{
    if (list)
    {

        record_t *node = list->head;
        if (node)
        {
            void *value = node->value;
            list->head = node->next;
            free(node);
            if (!list->head)
            {
                list->last = NULL;
            }
            return value;
        }
    }
    return NULL;
}

/**
 * Get head
 * 
 * @param list
 */
void *list_get(list_t *list)
{
    return list ? list->head : NULL;
}
