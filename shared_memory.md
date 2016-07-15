# Shared memory

## system V shared memory

The shared memory is one of the quickest forms of IPC that can be used between the processes. Since the memory is common between the two programs \(or more than two\) it is a must to protect it from being accessed parallely at the same time causing the corruption. Thus, we need to use some form of locking \(such as the semaphores or events\). The method of creating and communicating via the shared memory is as follows.

* A process creates a shared memory segment with a unique key value
* The process then attaches to it.
* Another process, knowing the unique key value, attaches to the shared memory segment.
* Now the two processes can communicate \(transfer the data between each other\) using the shared memory.

To create a shared memory, the Linux OS provides shared memory API as the following.

| shm API | description |
| :--- | :--- |
| shmget | allocate shared memory segment |
| shmat | attach to the shared memory with the given shared memory identifier |
| shmctl | perform control operations on the shared memory segment |
| shmdt | detaches from the shared memory |

To use the above API we must include `<sys/ipc.h>` and `<sys/shm.h>` header files.

`shmget` is used to create shared memory segments.

The `shmget` prototype is as follows.

```c
int shmget(key_t key, size_t size, int shmflg);
```

`shmget` returns the shared memory ID on success.

* The first argument `key` must be unique. This `key` can be generated using the `ftok()` call.
* The `size` argument is the size of the shared memory segment \(it is rounded to the multiples of PAGE\_SIZE. Usually PAGE\_SIZE is 4k\).
* The `shmflg` is usually set with the IPC\_CREAT flag. 
* If the `key` already exist, the `errno` is set to EEXIST and returns -1.

The below is an example to create the shared memory segment. The key is taken to be static number for the example.

```c
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main()
{
    int shmid;
    key_t key = 0x01020304;

    // create 4096 bytes of shared memory
    shmid = shmget(key, 4096, IPC_CREAT);
    if (shmid < 0) {
        fprintf(stderr, "failed to create shm segment\n");
        perror("shmget");
        return -1;
    }

    printf("created %d\n", shmid);
    return 0;
}
```

We compile and execute the program and on success it prints the last print statement i.e. created 993131 \(some number that is the shm id\).

an ipcs -m command on the created shared memory shows me this.

```bash
dev@hanzo:~$ ipcs -m

------ Shared Memory Segments --------
key        shmid      owner      perms      bytes      nattch     status
0x00000000 65536      lightdm    600        524288     2          dest
0x00000000 163841     lightdm    600        524288     2          dest
0x00000000 196610     lightdm    600        33554432   2          dest
0x01020304 229379     dev        0          4096       0
```

The API `shmat` performs the attachement to the shared memory segment. Its prototype is as following.

```c
void *shmat(int shmid, const void *shmaddr, int shmflg);
```

* the first argument `shmid` is the id returned from shmget.
* the second argument is the attach address, and is usually kept to NULL.
* the shmflg is also kept to 0 when doing read and write operations on the shared memory.

On success `shmat` returns the address of the segment and on failure it returns a value -1 and the value to be type casted to an integer to check for the failures.

Let us write two programs, one is the program that creates the shared memory, attaches to it and writes "Hello" to the memory. The another program attaches to the memory based on the key and reads from the memory and prints the contents on to the console.

**Server code**

```c
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main()
{
    int shmid;
    key_t key = 0x01020304;
    void *addr;

    shmid = shmget(key, 4096, IPC_CREAT);
    if (shmid < 0) {
        fprintf(stderr, "failed to create shm segment\n");
        perror("shmget");
        return -1;
    }

    printf("created %d\n", shmid);

    addr = shmat(shmid, NULL, 0);
    if ((int)addr == -1) {
        fprintf(stderr, "failed to attach\n");
        perror("shmat");
        return -1;
    }

    printf("got %p\n", addr);

    char *data = addr;

    strcpy(data, "Hello");

    while(1);
    return 0;
}
```

**Client code**

```c
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main()
{
    int shmid;
    key_t key = 0x01020304;
    void *addr;

    shmid = shmget(key, 4096, 0);
    if (shmid < 0) {
        fprintf(stderr, "failed to create shm segment\n");
        perror("shmget");
        return -1;
    }

    printf("found %d\n", shmid);

    addr = shmat(shmid, NULL, 0);
    if ((int)addr == -1) {
        fprintf(stderr, "failed to attach\n");
        perror("shmat");
        return -1;
    }

    printf("got %p\n", addr);

    char *data = addr;

    printf("Data %s\n", data);

    return 0;
}
```

We compile the two programs and create the binaries as `shmsrv` and `shmcli`. We run the `shmsrv` first and then `shmcli` next. The `shmsrv` program performs the write to the shared memory segment and runs into infinite loop while the `shmcli` program performs a read on the shared memory segment and prints the data `Hello` on to the screen.

Let us run the `ipcs -m -i 229379` \(where the 229379 is my shm id\).

```bash
dev@hanzo:~$ ipcs -m -i 229379

Shared memory Segment shmid=229379
uid=1000    gid=1000    cuid=1000    cgid=1000
mode=0    access_perms=0
bytes=4096    lpid=3617    cpid=3430    nattch=0
att_time=Sat Mar 26 17:21:50 2016
det_time=Sat Mar 26 17:21:50 2016
change_time=Sat Mar 26 17:08:59 2016

```

statistics about the shared memory be found using the `shmctl` API.

Let us add the following code to the shmcli.c file.

```c
    struct shmid_ds buf;

    ret = shmctl(shmid, IPC_STAT, &buf);
    if (ret < 0) {
        fprintf(stderr, "failed to shmctl\n");
        perror("shmctl");
        return -1;
    }

    printf("size %d\n", buf.shm_segsz);
    printf("attach time %d\n", buf.shm_atime);
    printf("detach time %d\n", buf.shm_dtime);
    printf("change time %d\n", buf.shm_ctime);
    printf("creator pid %d\n", buf.shm_cpid);
    printf("n attach %d\n", buf.shm_nattch);
```

## mmap

`mmap` maps the files or device into memory. `mmap` creates a new mapping in the virtual memory of the process.

The prototype is as follows.

```c
void *mmap(void *addr, size_t length, int prot, int flags,
            int fd, off_t offset);
```

If `addr` is `NULL`, the kernel initialises and chooses a memory and returns as the `mmap` return value.

file size is specified in the `length` argument.

the `prot` is is the protection bits for the memory. It is defined as



| prot | description |
| :--- | :--- |
| PROT\_EXEC | pages may be executable |
| PROT\_READ | pages may be read |
| PROT\_WRITE | pages may be written |
| PROT\_NONE | pages may not be accessible |

usual `prot` arguments for a file descriptor are `PROT_READ` and `PROT_WRITE`.

The `flags` argument determines whether the updates to the memory are visible to the other processes mapping to the same region. One of the most commonly used flags are `MAP_SHARED` and `MAP_PRIVATE`.

`MAP_SHARED` makes the other processes get the updates on the pages.

`MAP_PRIVATE` creates a private copy on write mapping and the updates are not visible to other processes that are mapping to the same file.



The `munmap` unmaps the files from the memory.



The prototype is as follows.

```c
int munmap(void *addr, 
```

