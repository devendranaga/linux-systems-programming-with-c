## Shared memory

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

`mmap` maps the files or device into memory, so that operations can be directly done on the memory. The memory afterwards, can be synced in or out based on its validity. `mmap` creates a new mapping in the virtual memory of the process.

The prototype is as follows.

```c
void *mmap(void *addr, size_t length, int prot, int flags,
            int fd, off_t offset);
```

If `addr` is `NULL`, the kernel initialises and chooses a memory and returns as the `mmap` return value.

on a successful `mmap` the pointer to the address of the shared memory is returned, otherwise `MAP_FAILED` is returned. So a check is made on the returned memory  for validity.

```c
void *maped;

maped = mmap(NULL, 1024 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
if (maped == MAP_FAILED) {
    return -1;
}

```

when allocating the memory, it is usually best practise to allocate on page boundary. A default page size usually 4kB and may vary from hardware to hardware. check with `sysconf(_SC_PAGE_SIZE)` for runtime page size for each platform.

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

To unmap the memory that is maped by `mmap` the `munmap` is used. The `munmap` unmaps the mapped memory.

The prototype is as follows.

```c
int munmap(void *addr, size_t length);
```

include `<sys/mman.h>` for the `mmap` API. Download [here](https://github.com/DevNaga/gists/blob/master/mmap.c)

sample code:

```c
#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

int main(int argc, char **argv)
{
    int ret;
    int fd;
    void *addr;
    struct stat s;

    if (argc != 2) {
        printf("%s [filename]\n", argv[0]);
        return -1;
    }

    fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        printf("failed to open %s\n", argv[1]);
        return -1;
    }

    ret = stat(argv[1], &s);
    if (ret < 0) {
        printf("failed to stat %s\n", argv[1]);
        return -1;
    }

    addr = mmap(NULL, s.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (!addr) {
        printf("Failed to mmap %s\n", strerror(errno));
        return -1;
    }

    printf("data at the address %p is %s\n", addr, addr);
    munmap(addr, s.st_size);
    close(fd);
    return 0;
}
```

before running the program, perform the following.

```shell
echo "mmap test" > test
gcc -Wall mmap.c
./a.out test
```


`MAP_SHARED` is required on regular files when writing. If doing `MAP_PRIVATE`, the syncs to the underlying file will not happen.

if there is no file that is available or the file is not a text file, the visualisation of the data is not possible.

There is also a way to write the data stored at the memory back to the file using the `msync` API. `msync` allows the memory written at the address to be flushed down to the file either synchronously or asynchronously.

The `msync` API prototype is as follows.

```c
int msync(void *addr, size_t length, int flags);
```

The `msync` will write the contents stored at the address `addr` of `length` bytes into the file that the `addr` points to. The `addr` is the return value of the `mmap` where in which the file descriptor is given to map the contents.

The `flags` argument has two values.

`MS_ASYNC`: schedule an update on this address to the file on the disk. The call returns immediately after setting the bit in the kernel for the update.

`MS_SYNC`: request an update and wait till the update finishes.

Here is an extension of the above example that performs the `msync` API. Download [here](https://github.com/DevNaga/gists/blob/master/mmap_sync.c)

```c
#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

int main(int argc, char **argv)
{
    int ret;
    int fd;
    void *addr;
    struct stat s;
    int file_size = 0;

    if (argc != 3) {
        printf("%s [filename] [filesize in MB]\n", argv[0]);
        return -1;
    }

    file_size = atoi(argv[2]);

    fd = open(argv[1], O_RDWR | O_CREAT, S_IRWXU);
    if (fd < 0) {
        printf("failed to open %s\n", argv[1]);
        return -1;
    }

    ret = ftruncate(fd, file_size * 1024 * 1024);
    if (ret < 0) {
        printf("failed to trucate file %s to %d MB\n", argv[1], file_size);
        return -1;
    }

    ret = stat(argv[1], &s);
    if (ret < 0) {
        printf("failed to stat %s\n", argv[1]);
        return -1;
    }

    addr = mmap(NULL, s.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (!addr) {
        printf("Failed to mmap %s\n", strerror(errno));
        return -1;
    }

    printf("data at the address %p is %s\n", addr, addr);

    memset(addr, 0, s.st_size);
    strcpy(addr, "Hello Mmap");

    // sync to the disk sychrnously
    msync(addr, s.st_size, MS_SYNC);
    perror("msync");

    munmap(addr, s.st_size);

    close(fd);
    return 0;
}

```

The `ftruncate` is used to first truncate the file before calling mmap with the size of the file. if the file is created newly, its size is default 0 bytes. Thus `mmap` fails on mapping the file to the memory. Instead truncate the file with a specific size and then performing a `stat` on it gives the size of the new truncated value. Thus the call on `mmap` will succeed and the mapping is performed. Subsequent writes on the files are basically copying the values to the address by `strcpy` if string is supposed to be written to the file or a `memcpy` if the data is other than string format.

The `mmap` is mostly used in optimising the file writes, such as in case of data bases. They map the file into the RAM and only write \(perform the `msync`\) optimally. This reduces the use of write system calls in the kernel and the kernel's paging daemon flushing the pages to the disk and using this saved CPU usage to the other tasks.


Linux provides another shared memory POSIX API called `shm_open` and `shm_unlink` which can be used along with the `mmap` and `munmap`.

The prototype of `shm_open` is as follows,


```c
int shm_open(const char *name, int oflag, mode_t mode);

```

the `name` should contain a `/` first and the name of the shared memory file.
the `oflag` is same as that of `O_CREAT | O_RDWR` for creation and `O_RDWR` for read-write operation.
the `mode` represents the creation mask `S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP` etc.

when given as `O_CREAT` to the `oflag` the shared memory will be created. When there are more than one process that is gonna use shared memory, one process would create the memory with `O_CREAT` along with the rest of the oflags and file mode creation flags. The other processes would use the `O_RDWR` `O_RDONLY` and / or `O_WRONLY` flags. When used with out `O_CREAT`, the mode is set to 0.


The `shm_open` returns a file descriptor of the path, and this can be used by the `mmap` as the following,

Prototype of `mmap` is shown below,

```c
void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);

```

code to perform `mmap` with the `shm_open` is as follows,

```c

int mapfd;

mapfd = shm_open("/shm_path", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
if (mapfd < 0) {
    return -1;
}

ftruncate(mapfd, 1024 * 1024);

void *mapaddr;

mapaddr = mmap(NULL, 1024 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, mapfd, 0);
if (mapaddr == MAP_FAILED) {
    return -1;
}

```

notice the use of `ftruncate` system call. This is required to let `mmap` work correctly with the given length bytes.

There is no need to `msync` because the operation is on a file descriptor returned by the call to `shm_open`.

the `shm_unlink` prototype is as follows.


```c
int shm_unlink(const char *name);

```

this is called right after the program has stopped using the fd returned by the `shm_open`.


Below is an example that demo the use of `mmap` with the `shm_open`. Download [here](https://github.com/DevNaga/gists/blob/master/mmap_comm.c)

```c
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

#define SHM_NAME "/test_shm"
#define SHM_BYTES_MAX (1024 * 1024)
#define FIFO_NAME "/test_fifo"

static int fifd;
static int fd;

int intr_reader()
{
    int intr = 1;

    return write(fifd, &intr, sizeof(int));
}

int wait_for_intr()
{
    int intr = 0;
    int ret;

    ret = read(fifd, &intr, sizeof(intr));
    if (ret <= 0) {
        return -1;
    }

    return intr == 1;
}

void* mmap_create_buf()
{
    fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if (fd < 0) {
        perror("shm_open");
        return NULL;
    }

    ftruncate(fd, SHM_BYTES_MAX);

    void *addr;

    addr = mmap(NULL, SHM_BYTES_MAX, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        return NULL;
    }

    return addr;
}

void* mmap_attach_buf()
{
    fd = shm_open(SHM_NAME, O_RDWR, 0);
    if (fd < 0) {
        perror("shm_open");
        return NULL;
    }

    void *addr;

    addr = mmap(NULL, SHM_BYTES_MAX, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        return NULL;
    }

    return addr;
}

int fifo_create()
{
    int ret;

    unlink(FIFO_NAME);

    ret = mkfifo(FIFO_NAME, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if (ret < 0) {
        perror("mkfifo");
        return -1;
    }

    fifd = open(FIFO_NAME, O_RDWR);
    if (fifd < 0) {
        perror("open");
        return -1;
    }

    return 0;
}

int fifo_attach()
{
    fifd = open(FIFO_NAME, O_RDWR);
    if (fifd < 0) {
        perror("open");
        return -1;
    }

    return 0;
}

int main(int argc, char **argv)
{
    int ret;
    void *addr;

    if (argc != 2) {
        fprintf(stderr, "<%s> producer/consumer\n", argv[0]);
        return -1;
    }

    if (!strcmp(argv[1], "producer")) {
        addr = mmap_create_buf();
        if (!addr) {
            return -1;
        }

        ret = fifo_create();
        if (ret < 0) {
            return -1;
        }

        while (1) {
            strcpy(addr, "Hello ");

            intr_reader();
            sleep(1);
        }
    } else if (!strcmp(argv[1], "consumer")) {
        addr = mmap_attach_buf();
        if (!addr) {
            return -1;
        }

        ret = fifo_attach();
        if (ret < 0) {
            return -1;
        }

        while (1) {
            wait_for_intr();
            printf("data from prod: %s\n", addr);

            memset(addr, 0, 10);
        }
    }
}

```

The above example demonstrates the use of `mmap` and the `shm_open` along with the `mkfifo` for synchronisation.

The creator is simply the producer and the reader is simply the consumer. The creator creates the shared memory fd, maps using mmap and then creates a fifo with `mkfifo` and calls `open`.

The consumer opens the shread memory with the `shm_open` and maps the memory, it also opens the fifo. The consumer never creates any of the shared fd, memory or the fifo. It opens and waits for the data.

The consumer waits on the read calling the `wait_for_intr` call. The producer sleeps every second and writes "Hello" to the shared memory and interrupts the reader via the `intr_reader`.

Compile the program and run it in two separate terminals and observe the communication between the two programs.



Linux defines another system call called `mprotect`, allowing to change the permissions of the existing region of memory.


The prototype of `mprotect` is as follows.

```c
int mprotect(const void *addr, size_t len, int prot);

```

where the `addr` is a page aligned memory. The `len` is the portion of the memory to be protected. the `prot` is a combination of `PROT_READ` and `PROT_WRITE`. The `mprotect` overrides the existing protection bits when the memory is created via `mmap`. this means that if the memory needs to be in read only then the `prot` field must be only `PROT_READ`. if its in read write then the combination of OR must be used such as `PROT_READ | PROT_WRITE`.


`mprotect` returns 0 if protection bits are changed success and -1 on failure.



Linux provides a system call `madvise` to provide the kernel an advise. The prototype is as follows,


```c
int madvise(void *addr, size_t length, int advise);

```

The `madvise` system call gives the advise to the kernel about the address. The advise is one of the following.


| type | meaning |
|------|---------|
| MADV_NORMAL | no special treatment for the address |
| MADV_RANDOM | expect page references in random |
| MADV_SEQUENTIAL | expect page references in sequential |
| MADV_WILLNEED | expect the access in near future |


An example call to the `madvise` is as follows.

```c
void *addr;

addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
if (addr == MAP_FAILED) {
    return -1;
}

ret = madvise(addr, size, MADV_RANDOM);
if (ret < 0) {
    return -1;
}

```

There is another group of syscalls called `mlock` and `mlockall` that explain the system to lock a particular region of shared memory and not allow it to be paged. This means, the physical page that has been allocated to the page table entry and any accesses to it will not create a page fault. This means to let the page in the RAM always.

`munlock` unlocks the portion of the memory that has been locked by the `mlock`.

`mlock` tends to improve performance when used in a time sensitive code thus reducing the paging.

`mlock` prototypes are as follows. include `<sys/mman.h>` before using `mlock` system calls.

```c
int mlock(const void *addr, size_t len);
int mlock2(const void *addr, size_t len, int flags);
int munlock(const void *addr, size_t len);

int mlockall(int flags);
int munlockall(void);

```

the syscalls, `mlockall` and `munlockall` locks the entire pages mapped to the address space of the calling process.

the parameter `flags` in `mlockall` represents the following:


| name | meaning |
|------|---------|
| `MCL_CURRENT` | lock all pages which are currently mapped into the address space of the process |
| `MCL_FUTURE` | lock all pages which become mapped into the address space of the process in future. |
| `MCL_ONFAULT` | when used together with `MCL_CURRENT` and `MCL_FUTURE`, mark all current or future pages to be locked into RAM when they are faulted |


The calling convention for `mlock` is usually the following.


```c
void *addr = malloc(sizeof(int));
if (!addr) {
    return -1;
}

ret = mlock(addr, sizeof(int));
if (ret < 0) {
    return -1;
}

```

