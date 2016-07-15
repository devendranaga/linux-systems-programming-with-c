#include <stdio.h>
#include <stdlib.h>

struct linked_list {
    void *data;
    struct linked_list *next;
};

struct linked_list *head, *tail;

int list_add(void *elem)
{
    struct linked_list *new_node;

    new_node = calloc(1, sizeof(struct linked_list));
    if (!new_node) {
        return -1;
    }

    new_node->data = elem;

    if (!head) {
        head = new_node;
        tail = new_node;
    } else {
        tail->next = new_node;
        tail = new_node;
    }

    return 0;
}

int list_add_head(void *elem)
{
    struct linked_list *new_node;

    new_node = calloc(1, sizeof(struct linked_list));
    if (!new_node)
        return -1;

    new_node->data = elem;

    if (!head) {
        head = new_node;
        tail = new_node;
    } else {
        new_node->next = head;
        head = new_node;
    }
    return 0;
}

int list_add_tail(void *elem)
{
    struct linked_list *new_node;

    new_node = calloc(1, sizeof(struct linked_list));
    if (!new_node)
        return -1;

    new_node->data = elem;

    if (tail) {
        tail->next = new_node;
        tail = new_node;
    }

    return 0;
}

void list_for_each(void (*app_func)(void *data))
{
    struct linked_list *node;

    for (node = head; node; node = node->next) {
        if (app_func)
            app_func(node->data);
    }
}

int list_count()
{
    int count = 0;
    struct linked_list *node;

    for (node = head; node; node = node->next) {
        count ++;
    }

    return count;
}

int list_delete(void *elem, int (*free_func)(void *data))
{
    struct linked_list *node, *prev;

    // check if the element is at the head node
    if (head->data == elem) {
        node = head;
        head = head->next;
        free_func(node->data);
        free(node);
        return 0;
    }

    node = head;
    prev = head;
    while (node) {
        if (node->data == elem) {
            prev->next = node->next;
            free_func(node->data);
            free(node);
            return 0;
        } else {
            prev = node;
            node = node->next;
        }
    }

    return -1;
}

