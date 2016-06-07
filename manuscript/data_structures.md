# Data structures and searching-sorting algorithms

The data structures are key to any software program. They define the layout of the program.
In this section we are going to understand about some of the most commonly used data structures.

We are also going to take a dig at the searching and sorting functions too.. These however are important in the data analysis part of the programs.

##Linked List

1. Think of this as set of objects in series.
2. Objects can be of any type.
3. Dynamically modified (created, deleted, altered)
4. Number of elements in the list is the length of the list

Linked lists can be created with the simple data structure below.

    struct linked_list {
        void *data;
        struct linked_list *next;
    };
    
    struct linked_list *head, *tail;
    
The ```data``` part is a pointer that pointing towards another data structure.

The ```tail``` pointer is used to point to always the end of the list. The end pointer is used to chain the newly created element in the list at the end of the list. The end pointer is then made point to the newly created element. The end pointer is always maintained to point to the end of the list.

The following code performs the ```head``` and ```tail`` pointer assignments

```c
    if (!head) {
        head = new_node;
        tail = new_node;
    } else {
        tail->next = new_node;
        tail = new_node;
    }
```

The `if` conditional says if there is no `head` element, then the new node becomes the head element and also the `tail` node points to the new node. Otherwise, the `new_node` gets added as the next element of the `tail` node and further the `tail` node gets pointed to the `new_node`. Thus if a new element is again getting added, it will always gets added at the end.

The part of the code is keep into a function called `list_add`.

```c
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
```

the function creates a new linked list node and puts the `elem` to the `data` pointer of it.

Then it adds the element at the end of the list or at the beginning of the list (no list head).

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

Before we make the new_node the head, we must assign the next pointer of the new_node to head so that we preserve the entire chain. Next is to make the head pointing to the new_node. Thus keeping the new_node the head.

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


*****
Problem:
1. program to count the number of elements in the list.
*****

##Doubly linked list

1. Similar to the linked list, but the each element contain its own `prev` pointer along with the `next` pointer.
2. Going back into the list is possible.

the doubly linked list looks as the following.

    struct doubly_linked_list {
        void *data;
        struct doubly_linked_list *prev;
        struct doubly_linked_list *next;
    };

    struct doubly_linked_list *head, *tail;
    
similar to the linked list, we define the API such as the following.

| Doubly linked list API| description |
| -- | -- |
| dllist_add_tail | add a node at the tail |
| dllist_add_head | add a node at the head |
| dllist_for_each_forward | move towards the end of the list from the head |
| dllist_for_each_reverse | move towards the beginning of the list from the tail |
| dllist_delte_elem | delete the element from the doubly linked list |
| dllist_destroy | destroy the created doubly linked list |


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

This API finds if a node is NULL, adds the node to the head. Otherwise, it points the previous element of the new node to the tail node and makes the tail's next pointer point to the new_node. Then finally makes tail points to the new_node.

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

the `dllist_add_head` always adds the node at the beginning of the head. Make the previous pointer of head point to the new_node and new_node next points to the head element and finally making the head node point to the new_node.

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

-----

##Circular lists

### Circular linked lists

1. The linked list's last element points to the head instead of NULL. Making it the circular linked list.

the circular linked list looks as the following.

    struct circular_linked_list {
        void *data;
        struct circular_linked_list *next;
    };
    
    struct circular_linked_list *head, *tail;
    
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

### Circular doubly lists

1. The doubly linked list's last element points to the head instead of NULL. Making it the circular doubly linked list.

the circular doubly linked list looks as the following.

    struct ciruclar_doubly_linked_list {
        void *data;
        struct circular_doubly_linked_list *next;
        struct circular_doubly_linked_list *prev;
    };
    
    struct circular_doubly_linked_list *head, *tail;
    

##Queues
1. Same as linked list.
2. new entries are added at the last. Also called enqueue.
3. the elements are retrieved at the front. Also called dequeue.
4. The queue is also called FIFO (first in first out).

```c
    struct queue {
        void *data;
        struct queue *next;
    };
    
    struct queue *front, *rear;
```

The queue front and rear are initialised when a new element is added. The addition of new element is as the following.

    int enqueue(void *data)
    {
        struct queue *node;
        
        node = calloc(1, sizeof(struct queue));
        if (!node) {
            return -1;
        }
        
        node->data = data;
        if (!rear) {
            rear = node;
            front = node;
        } else {
            rear->next = node;
            rear = node;
        }
        
        return 0;
    }

The dequeue is performed by taking one element out of the front end.

    int dequeue(void (*dequeue_func)(void *data))
    {
        struct queue *node;
        
        node = front;
        
        if (!node)
            return -1;
        
        front = node->next;
        dequeue_func(node->data);
        
        return 0;
    }
    
The queues above are dynamic lists. Meaning, there is no limit on the number of elements that be added. Only the memory limit would be the limitting factor.


##Stacks

A stack is a container of objects that are inserted and removed in Last in First out (LIFO) manner. Putting the elements into the stack is called `push` and taking out the elements out of the stack is called `pop`.

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

##Hash tables

##Tree
###Binary tree
###Red black trees

##Searching Algorithms


##Binary search

##Linear search

##Queue sort

##Shell sort

##Insertion sort

##Merge sort
