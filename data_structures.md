## Data structures

The data structures are key to any software program. They define the layout of the program.
In this section we are going to understand about some of the most commonly used data structures.

We are also going to take a dig at the searching and sorting functions too.. These however are important in the data analysis part of the programs.

### 1. Linked List

1. Think of this as set of objects in series.
2. Objects can be of any type.
3. Dynamically modified \(created, deleted, altered\)
4. Number of elements in the list is the length of the list
5. Often more time consuming when looking for an element in the list if there are lots of elements.

Linked lists can be created with the simple data structure below.

```c
    struct linked_list {
        void *data;
        struct linked_list *next;
    };

    struct linked_list *head, *tail;
```

We will describe the following API.


| linked list API | description |
|-----------------|-------------|
| `list_add` | add an element to the list |
| `list_add_head` | add an element before the head of the list |
| `list_add_tail` | add an element after the tail of the list |
| `list_for_each` | iterate over every list node |
| `list_delete` | delete an element in the list |
| `list_free` | free the linked lists |


The `data` part is a pointer that pointing to a data structure. A user can point their data structure to this pointer.

The `tail` pointer is used to point to always the end of the list. The end pointer is used to chain the newly created element in the list at the end of the list. The end pointer is then made point to the newly created element.

The following code performs the `head` and `tail` pointer assignments

```c
    // if there's no head element add a new head element and point the
    // tail to the same so that the next element could be added as
    // the next element if the tail such as that in the else part
    //
    // if the head element is already there, then we have a node in
    // the linked list. so let's extend this by linking new element
    // as the next element to the tail such as 
    //
    // tail->next = new_node;
    if (!head) {
        head = new_node;
        tail = new_node;
    } else {
        tail->next = new_node;
        tail = new_node;
    }
```




![LinkedLists](images/linked_lists.jpg)

The `if` conditional says if there is no `head` element, then the new node becomes the head element and also the `tail` node points to the new node. Otherwise, the `new_node` gets added as the next element of the `tail` node and further the `tail` node gets pointed to the `new_node`. Thus if a new element is again getting added, it will always gets added at the end.

The part of the code is keep into a function called `list_add`.

```c
/**
 * @brief - add new element into the linked list at the tail
 */
int list_add(void *elem)
{
    struct linked_list *new_node;

    // allocate new_node
    new_node = calloc(1, sizeof(struct linked_list));
    if (!new_node) {
        return -1;
    }

    new_node->data = elem;

    // follow the same logic as in the above code snippet..
    //
    // if no head, add head and make tail points to it
    //
    // else, add the new node as next element of tail and 
    // make tail point to the new node
    if (!head) {
        head = new_node;
        tail = new_node;
    } else {
        tail->next = new_node;
        tail = new_node;
    }

    return 0;
}
```

the function creates a new linked list node and puts the `elem` to the `data` pointer of it.

Then it adds the element at the end of the list or at the beginning of the list \(no list head\).

The below function adds the elements to the head and so be called `list_add_head`.

```c
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
```

The below function adds the elements to the tail or at the end of linked list. The operation is O(1).

```c
int list_add_tail(void *elem)
{
    struct linked_list *new_node;

    new_node = calloc(1, sizeof(struct linked_list));
    if (!new_node)
        return -1;

    new_node->data = elem;

    if (!tail) {
        free(new_node);
        return -1;
    }

    tail->next = new_node;
    tail = new_node;

    return 0;
}
```

Before we make the new\_node the head, we must assign the next pointer of the new\_node to head so that we preserve the entire chain. Next is to make the head pointing to the new\_node. Thus keeping the new\_node the head.

To get the data pointer of each element, we need to `traverse` each element in the list and retrieve the data pointer.

The following code perform the traversal of the list:

```c
    void list_for_each(void (*app_func)(void *data))
    {
        struct linked_list *node;

        for (node = head; node; node = node->next) {
            if (app_func)
                app_func(node->data);
        }
    }
```

The `list_for_each` function goes through each element by advancing into the `next` pointer in the list. At each node, it dereferences the list element and calls the application callback function pointer `app_func`. This is the place where the function pointers come in handy in hiding the details of the linked list layer with the caller.

With the linked list we can delete a particular element in the list. A delete involve the following:
1. find the element in the list.
2. if the element is in the head, make head pointing to the next element after the head. free up the pointer that is at the previous head.
3. else, traverse through each element and if the element is found, make the previous node pointing to the next node. free up the list.

```c
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

```

The `free_func` is called when an element is found while we are deleting the node. Before we delete the node, we should make sure whatever the data application is allocated gets freed. Thus we call the `free_func` so that application would get chance to free up the memory that is has allocated.

The free function frees up all the nodes and removes the links. The `free_func` gets called on each linked list node thus notifying the user of the list API to safely free up its context structures.

The below function performs the free job.

```c
void list_free(int (*free_func)(void *data))
{
    struct linked_list *prev;
    struct linked_list *node;

    node = head;

    while (node) {
        free_func(node->data);
        prev = node;
        node = node->next;
        free(prev);
    }
}
```

---

Problem:
1. program to count the number of elements in the list.

---

## 2. Doubly linked list

1. Similar to the linked list, but the each element contain its own `prev` pointer along with the `next` pointer.
2. Going back into the list is possible.

the doubly linked list looks as the following.

```
struct doubly_linked_list {
    void *data;
    struct doubly_linked_list *prev;
    struct doubly_linked_list *next;
};

struct doubly_linked_list *head, *tail;
```

similar to the linked list, we define the API such as the following.

| Doubly linked list API | description |
| :--- | :--- |
| dllist\_add\_tail | add a node at the tail |
| dllist\_add\_head | add a node at the head |
| dllist\_for\_each\_forward | move towards the end of the list from the head |
| dllist\_for\_each\_reverse | move towards the beginning of the list from the tail |
| dllist\_delte\_elem | delete the element from the doubly linked list |
| dllist\_destroy | destroy the created doubly linked list |

The `dllist_add_tail` API adds the node to the tail of the list. The code looks as follows.

```c
int dllist_add_tail(void *elem)
{
    struct doubly_linked_list *new_node;

    new_node = calloc(1, sizeof(struct doubly_linked_list));
    if (!new_node)
        return -1;

    new_node->data = elem;

    if (!head) {
        head = new_node;
        tail = new_node;
    } else {
        new_node->prev = tail;
        tail->next = new_node;
        tail = new_node;
    }

    return 0;
}
```

This API finds if a node is NULL, adds the node to the head. Otherwise, it points the previous element of the new node to the tail node and makes the tail's next pointer point to the new\_node. Then finally makes tail points to the new\_node.

The `dllist_add_head` API adds the element to the head. The API looks as follows.

```c
int dllist_add_head(void *elem)
{
    struct doubly_linked_list *new_node;

    new_node = calloc(1, sizeof(struct doubly_linked_list));
    if (!new_node)
        return -1;

    new_node->data = elem;

    if (!head) {
        head = new_node;
        tail = new_node;
    } else {
        new_node->next = head;
        head->prev = new_node;
        head = new_node;
    }

    return 0;
}
```

the `dllist_add_head` always adds the node at the beginning of the head. Make the previous pointer of head point to the new\_node and new\_node next points to the head element and finally making the head node point to the new\_node.

the `dllist_for_each_forward` makes the iterator move from the head to the end. At each point, the callback is called and given the user data to the callback. So if the user gives out a callback, the API calls the callback for each node with the userdata.

```c
void dllist_for_each_forward(void (*callback)(void *data))
{
    struct dllist *iter;

    for (iter = head; iter; iter = iter->next) {
        callback(iter->data);
    }
}
```

the `dllist_for_each_reverse` makes the iterator move from the tail to the head. At each node we call the callback and pass the data to the user in the callback.

```c
void dllist_for_each_reverse(void (*callback)(void *data))
{
    struct dllist *iter;

    for (iter = tail; iter; iter = iter->prev) {
        callback(iter->data);
    }
}
```

the `dlist_delete_elem` deletes the element at the head, tail or in the middle anywhere. A callback is called if a match is performed. Otherwise, failure and -1 is returned.

```c
int dllist_delete_elem(void *data, void (*callback)(void *data))
{
    struct dllist *elem, *prev;

    if (head->data == data) {
        elem = head->next;
        elem->prev = NULL;
        callback(head->data);
        free(head);
        head = elem;
        return 0;
    }

    elem = head;
    prev = head;

    while (elem) {
        if (elem->data == data) {
            callback(elem->data);
            prev->next = elem->next;
            elem->next->prev = prev;
            free(elem);
            return 0;
        }
        prev = elem;
        elem = elem->next;
    }

    return -1;
}
```

the `dlist_destroy` deletes all the doubly linked list chain and calls a user specific callback for each doubly linked node.

```c
int dllist_destroy(void (*callback)(void *data))
{
    struct dllist *iter, *next;

    iter = head;

    while (iter) {
        callback(iter->data);
        next = iter->next;
        free(iter);
        iter = next;
    }

    return 0;
}
```

## 3. Circular lists

### 3.1 Circular linked lists

The linked list's last element points to the head instead of NULL, thus making it the circular linked list.

the circular linked list looks as the following.

```c
struct circular_linked_list {
    void *data;
    struct circular_linked_list *next;
};

struct circular_linked_list *head, *tail;
```

Below are some of the API described in the circular list.

| Circular linked list API | description |
|--------------------------|-------------|
| `circular_list_add` | add elements to the circular list |
| `circular_list_del` | delete elements from the circular list |
| `circular_list_foreach` | iterate over circular list |
| `circular_list_free` | free elements of circular list |


Adding an element to the circular list is same as adding an element to the linked list, but the last element points to the head back.

```c
int circular_list_add(void *elem)
{
    struct circular_linked_list *new;

    new = calloc(1, sizeof(struct circular_linked_list));
    if (!new)
        return -1;

    new->data = elem;
    if (!head) {
        head = new;
        tail = new;
    } else {
        tail->next = new;
        tail = new;
    }

    // point the end of the element to the head.
    new->next = head;
    return 0;
}
```

The traversal of the list is done as follows, however the check has to be made with the last element not equal to head as the list is circular.

```c
int circular_list_for_each(void (*func)(void *data))
{
    struct circular_linked_list *node = head;

    do {
        if (func)
            func(node->data);
        node = node->next;
    } while (node != head);

    return 0;
}
```

The callback function is called at each element traversal.

The API `circular_list_del` removes an element from the list. It could be a head node, tail node or any node in between. Below is the API implementation.

The logic follows the same as that of the linked list.

```c

int circular_list_del(void *content, void (*callback_ptr)(void *data))
{
    struct circular_linked_list *elem, *prev;

    if (!callback_ptr) {
        return -1;
    }

    elem = prev = head;

    if (elem->data == content) {
        head = elem->next;
        callback_ptr(elem->data);
        free(elem);
        tail->next = head;
        return 0;
    }

    elem = elem->next;

    while (elem != head) {
        if (elem->data == content) {
            prev->next = elem->next;
            callback_ptr(elem->data);
            free(elem);
            return 0;
        } else {
            prev = elem;
            elem = elem->next;
        }
    }

    return 0;
}

```

The API `circular_list_free` frees the circular list. Below is the implementation. The logic follow the similar implementation as that of Linked list.

```c

int circular_list_free()
{
   struct circular_linked_list *elem, *prev;

   elem = prev = head;

   do {
      prev = elem;
      elem = elem->next;
      free(prev);
   } while (eleme != head);
}

```


### 3.2 Circular doubly lists

1. The doubly linked list's last element points to the head instead of NULL. Making it the circular doubly linked list.

the circular doubly linked list looks as the following.

```c
struct ciruclar_doubly_linked_list {
    void *data;
    struct circular_doubly_linked_list *next;
    struct circular_doubly_linked_list *prev;
};

struct circular_doubly_linked_list *head, *tail;
```

## 4. Queues

1. Queues are Same as linked list.
2. new entries are added at the last. Also called enqueue.
3. the elements are retrieved at the front. Also called dequeue.
4. The queue is also called FIFO \(first in first out\).

```c
    struct queue {
        void *data;
        struct queue *next;
    };

    struct queue *head, *tail;
    static int count = 0;
```

Below are the following APIs described. The design and implementation is much consolidated.


| Queue function  | description |
|-----------------|-------------|
| `enqueue` | enqueue an item into the queue |
| `dequeue` | dequeue an item from the queue |
| `get_count` | get the count of elements in queue |


The queue front and rear are initialised when a new element is added. The addition of new element is as the following.


```c

int get_count()
{
    return count;
}

int enqueue(void *content)
{
    struct queue *item;

    item = calloc(1, sizeof(struct queue));
    if (!item) {
        return -1;
    }

    item->data = content;

    if (!head) {
        head = item;
        tail = item;
    } else {
        tail->next = item;
        tail = item;
    }

    count ++;

    return 0;
}

```


The dequeue is performed by taking one element out of the front end.

```c

int dequeue(void (*callback_ptr)(void *data))
{
    struct queue *item;

    if (!callback_ptr) {
        return -1;
    }

    if (head) {
        count --;
        item = head;
        callback_ptr(head->data);
        head = head->next;
        free(item);
    } else {
        return -1;
    }

    return 0;
}

```

The `callback_ptr` is called when a dequeue is performed and the data gets copied to the argument of `callback_ptr`.


The queues above are dynamic lists. Meaning, there is no limit on the number of elements that be added. Only the memory limit would be the limitting factor.

## 5. Stacks

A stack is a container of objects that are inserted and removed in Last in First out \(LIFO\) manner. Putting the elements into the stack is called `push` and taking out the elements out of the stack is called `pop`.

`push` adds an item into the top of the stack and `pop` removes the item from the top.

```c
struct stack_list {
    void *item;
    struct stack_list *next;
};

struct stack_list *head, *tail;
```

The data structure is similar to the linked list but the elements are keep on adding at the head so that the pop can take the element out of the head and give it to the user.

Here is the `push` API that adds the element at the head.

```c
int push(void *elem)
{
    struct stack_list *new_node;

    new_node = calloc(1, sizeof(struct stack_list));
    if (!new_node)
        return -1;

    new_node->item = elem;
    if (!head) {
        head = new_node;
        tail = new_node;
    } else {
        new_node->next = head;
        head = new_node;
    }

    return 0;
}
```

The `pop` API takes the element out of the head and calls the user callback.

```c
int pop(void (*callback)(void *data))
{
    struct stack_list *new_node;

    if (!head)
        return -1;

    new_node = head->next;

    callback(head->item);
    free(head);
    head = new_node;
    return 0;
}
```

## 6. Hash tables

A hash table is a data structure used to implement an associative array, a structure that can map keys to values. Hash tables are used to find the index based on the hash value.

To do this, they compute hash of the input and add the value into the array. If more than one input matches to the same hash, it is called collision.

To add an element, we compute the hash out of it using the hashing formula. We then derive the index out of the hash value and then add the element to the index.

The perfect hash function was implemented by `Dan Bernstein`. And it is the following snippet.

```c
static int hash = 5381;

hash = ((hash << 5) + hash) + number;
```

the index is then calcuated using the below snippet.

```c
index = hash % size;
```
Collisions are the problems in hash tables. When the input matches to the same hash, this is treated as collision. When a collision occur, the elements cannot be added at the right index.
When the collision occur, the element is added at the tail of the data. The best hashing algorithm prevents the number of collisions.

Example hash program: You can find it [here](https://github.com/DevNaga/gists/blob/master/hash_tables.c)

```c
/***
 * @simple hash table implementation in C
 *
 * Author: Dev Naga <devendra.aaru@gmail.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * List elements
 */
struct list_elem {
    void *spec_data;
    struct list_elem *next;
};

/**
 * Hash table
 */
struct hash_list {
    struct list_elem *item;
};

static int table_size;
static struct hash_list *hlist;

/**
 * Init the hash tables witha pre-defined table
 */
int hlist_init(int hlist_size)
{
    hlist = calloc(hlist_size, sizeof(struct hash_list));
    if (!hlist) {
        return -1;
    }

    table_size = hlist_size;
    return 0;
}

/**
 * Hash the data with a simple addition of characters
 */
static int hlist_hash_data(char *hash_str)
{
    int hash = 0;
    int i;

    for (i = 0; i < strlen(hash_str); i ++) {
        hash += hash_str[i];
    }

    return hash % table_size;
}

static int hlist_add_item_new(int index, void *item)
{
    hlist[index].item = calloc(1, sizeof(struct list_elem));
    if (!hlist[index].item) {
        return -1;
    }

    hlist[index].item->spec_data = item;
    hlist[index].item->next = NULL;

    return 0;
}

static int hlist_add_item_append(int index, void *item)
{
    struct list_elem *last, *iter;

    iter = hlist[index].item;
    last = iter;
    while (iter) {
       last = iter;
       iter = iter->next;
    }

    struct list_elem *new;

    new = calloc(1, sizeof(struct list_elem));
    if (!new) {
        return -1;
    }

    new->spec_data = item;
    last->next = new;

    return 0;
}

/**
 * Find item in the hash tables if exist
 */
int hlist_find_item(void *item, int (*compare)(void *a, void *b))
{
    int i;

    for (i = 0; i < table_size; i ++) {
        struct list_elem *list;
        int res;

        for (list = hlist[i].item; list; list = list->next) {
            res = compare(item, list->spec_data);
            if (res) {
                return 1;
            }
        }
    }

    return 0;
}

/**
 * Add an item in list.. append if already hashed to same data, add new if does not
 */
int hlist_add_item(void *item, char *hash_str)
{
    int index;

    index = hlist_hash_data(hash_str);

    if (hlist[index].item == NULL) {
        hlist_add_item_new(index, item);
    } else {
        hlist_add_item_append(index, item);
    }

    return 0;
}

/**
 * Print hash table elements
 */
void hlist_print(void (*data_callback)(void *data))
{
   int i;

   for (i = 0; i < table_size; i ++) {
       struct list_elem *list;

       printf("hash item: <%d>\n", i);
       for (list = hlist[i].item; list; list = list->next) {
           printf("\t list->spec_data <%p>\n", list->spec_data);
           data_callback(list->spec_data);
       }
   }
}

/***
 * Test program
 */
void print_items(void *data)
{
    //printf("item : <%s>\n", data);
}

int compare_item(void *a, void *b)
{
    char *a_1 = a;
    char *a_2 = b;

    return (strcmp(a_1, a_2) == 0);
}

int main(int argc, char **argv)
{
    int i;

    hlist_init(20);

    for (i = 1; i <= argc - 1; i ++) {
        if (!hlist_find_item(argv[i], compare_item)) {
            hlist_add_item(argv[i], argv[i]);
        }
    }

    hlist_print(print_items);

    return 0;
}
```

For more info on the data structures and on C/C++ languages refer this book [here](https://leanpub.com/c_cpp_refman)
