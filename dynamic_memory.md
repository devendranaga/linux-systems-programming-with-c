# Dynamic memory allocation

Dynamic memory is used at places where the need of memory is scarce, and places where the size of the memory is not known at compile time.

The C library implements some of the dynamic memory allocation functions. Some of the most used functions are as follows.

## malloc

prototype:
`void *malloc(size_t size);`


The `malloc` function allocates size bytes and returns a pointer to the allocated memory. The memory is not initialized.

an example of the usage is as follows...

```c
int *data;

data = malloc(sizeof(int));
if (!data) {
    printf("failed to allocate memory from malloc\n");
    return -1;
}

printf("data pointer %p\n", data);
```

The `malloc` API might fail and the memory pointed to may be `NULL` thus it is always recommended to check for the `NULL` before using the memory.

## calloc

prototype:
`void *calloc(int n_memb, size_t size);`

## realloc

prototype:
`void *realloc(void *ptr, size_t size);`

## free

prototype:
`void free(void *ptr);`

# Advanced memory operations

## debugging malloc

malloc_opt and friends ..

## stack based allocator (alloca)
