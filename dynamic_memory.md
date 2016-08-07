# Dynamic memory allocation

Dynamic memory is used at places where the need of memory is scarce, and places where the size of the memory is not known at compile time.

The C library implements some of the dynamic memory allocation functions. Some of the most used functions are as follows.

Include `<stdlib.h>` for the dynamic memory allocation functions.

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

When allocating space for a string, the argument of `malloc` must be one plus the length of
 the string. This is because the string is terminated with a `\0` character. The reason
 being is that the length does not count the terminating `\0` character but it needs to
 be stored in the array.
 
 # calloc

prototype:
`void *calloc(int n_memb, size_t size);`

The `calloc` function allocates size bytes * n_memb. The allocated memory is set to zero
 and returned. The `calloc` is same as `malloc` + `memset`.
 
Here is the following example of the `calloc` function usage.

```c
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int *array;
    
    array = calloc(10, sizeof(int));
    if (!array) {
        printf("failed to allocate memory at %s %u\n", __func__, __LINE__);
        return -1;
    }
    
    printf("memory %p\n", array);
    return 0;
}
```

## realloc

prototype:
`void *realloc(void *ptr, size_t size);`

The `realloc` API changes the size of the `ptr` variable to `size` bytes. The sample
 program is below.
 
```c
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int *memory = NULL;
    
    memory = realloc(memory, sizeof(int));
    if (!memory) {
        printf("failed to allocate at %s %u\n", __func__, __LINE__);
        return -1;
    }
    
    return 0;
}
```

The `realloc` API when passed `NULL` behaves same as `malloc(size)`. At each call to the
`realloc` API, the size needs to be the older size plus the new size that is to be
allocated.

## free

prototype:
`void free(void *ptr);`

The `free` API frees up the memory allocated by the memory allocation API.
If, after the allocation, when `free` is not called, then the program said to be leaking.

Please be sure to pass a valid pointer to the `free` call.

# Advanced memory operations

## posix_memalign

## mallinfo

The `mallinfo` function returns a copy of a structure containing the memory information
 about the memory allocations performed by `malloc` and related functions. The structure
 is as follows.
 
```c
struct mallinfo {
    int arena;     /* Non-mmapped space allocated (bytes) */
    int ordblks;   /* Number of free chunks */
    int smblks;    /* Number of free fastbin blocks */
    int hblks;     /* Number of mmapped regions */
    int hblkhd;    /* Space allocated in mmapped regions (bytes) */
    int usmblks;   /* Maximum total allocated space (bytes) */
    int fsmblks;   /* Space in freed fastbin blocks (bytes) */
    int uordblks;  /* Total allocated space (bytes) */
    int fordblks;  /* Total free space (bytes) */
    int keepcost;  /* Top-most, releasable space (bytes) */
};
```

The below example shows the usage of the `mallinfo`.

```c
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

int main()
{
	int *var;

	var = calloc(1, sizeof(int));
	if (!var) {
		printf("failed to allocate at %s %u\n", __func__, __LINE__);
		return -1;
	}

	struct mallinfo inf;

	inf = mallinfo();
	printf("total non mapped                       %d\n", inf.arena);
	printf("ordinary blocks                        %d\n", inf.ordblks);
	printf("fastbin blocks                         %d\n", inf.smblks);
	printf("no of mapped regions                   %d\n", inf.hblks);
	printf("bytes in mapped region                 %d\n", inf.hblkhd);
	printf("maximum total allocated space          %d\n", inf.usmblks);
	printf("free bytes held in fastbins            %d\n", inf.fsmblks);
	printf("total allocated space                  %d\n", inf.uordblks);
	printf("total free space                       %d\n", inf.fordblks);
	printf("topmost releasable blocks              %d\n", inf.keepcost);

	return 0;
}
```

Here is one of the example of the output.

```bash
root@89e0cc8e998a:~/books# ./a.out 
total non mapped                       135168
ordinary blocks                        1
fastbin blocks                         0
no of mapped regions                   0
bytes in mapped region                 0
maximum total allocated space          0
free bytes held in fastbins            0
total allocated space                  32
total free space                       135136
topmost releasable blocks              135136
```

## debugging malloc

malloc_opt and friends ..

## mtrace

The function `mtrace` is used to debug the malloc and friends allocations and frees.
This allows us to perform memory leak analysis.

The Glibc provides a `mtrace` program to perform leak analysis. The sample usage of the
 function is as below.

```c
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <mcheck.h>

int main(void)
{
	int i;
	char *ptr;

    mtrace();

	for (i = 0; i < 100; i ++) {
		ptr = malloc(sizeof(char));
		if (!ptr) {
			printf("failed to allocate at %s %u\n", __func__, __LINE__);
			return -1;
		}
//		free(ptr);
	}

	return 0;
}
```

The program is then run by exporting the MALLOC_TRACE to the corresponding file name
and keeping the exported file writeable.

```c
root@89e0cc8e998a:~/books# MALLOC_TRACE=/root/books/dev.trace ./a.out 
``` 

It produces the trace as the following into the `/root/books/dev.trace` file.

```bash
= Start
@ ./a.out:[0x4005d6] + 0x1c6f450 0x1
@ ./a.out:[0x4005d6] + 0x1c6f470 0x1
@ ./a.out:[0x4005d6] + 0x1c6f490 0x1
@ ./a.out:[0x4005d6] + 0x1c6f4b0 0x1
..........
@ ./a.out:[0x4005d6] + 0x1c70090 0x1
@ ./a.out:[0x4005d6] + 0x1c700b0 0x1
```

running the mtrace on the file below produces the following output.

```bash
root@89e0cc8e998a:~/books# mtrace ./a.out dev.trace 

Memory not freed:
-----------------
           Address     Size     Caller
0x0000000001c6f450      0x1  at /root/books/mtrace.c:14
0x0000000001c6f470      0x1  at /root/books/mtrace.c:14
0x0000000001c6f490      0x1  at /root/books/mtrace.c:14
.......
0x0000000001c6f4b0      0x1  at /root/books/mtrace.c:14
0x0000000001c6f4d0      0x1  at /root/books/mtrace.c:14
0x0000000001c6f4f0      0x1  at /root/books/mtrace.c:14
0x0000000001c6f510      0x1  at /root/books/mtrace.c:14

```

## stack based allocator (alloca)

