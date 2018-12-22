## file operations

### 1. C file handling

The function call set `fopen`, `fclose`, `fgets`, `fputs`, `fread` and `fwrite` can be used to perform basic to intermediate file handling.

The FILE is the handle returned by the `fopen` call.

the `fopen` takes the following form:

```c
FILE *fopen(const char *file, char *mode);
```

the fopen returns the pointer of type FILE. This also is called as file handle. The mode argument is one of "r", "w", "a" etc. The "r" argument is used to perform "read" on the file, the "w" argument is used to perform "write" on the file and the "a" argument is used to perform "append" on the file.

```c
FILE *fp;

fp = fopen("file", "w");
```

the `fopen` returns `NULL` if the file can't be opened.

The opened file can be used to perform operations on the file. The `fgets` is used to perform the read operation and `fputs` is used to perform the write operation on the file respectively.

Contents of the file can be read character by character using the `fgetc` and can be written character by character using the `fputc` call.

The `stdin`, `stdout` and `stderr` are the standard file descriptors for the standard input stream, output stream and error stream.

A call to `fgets` will read the contents of the string into the buffer of given length terminated by the `'\n\0'`.

So when performing `fgets`, the buffer should be stripped with `\n`. such as the following

```
buf[strlen(buf) - 1] = '\0'; // terminate the new line with '\0' character.
```

The below example reads the file given as argument from the command line and prints the contents on to the screen.

```c
#include <stdio.h>

#define LINE_LEN 512

int main(int argc, char **argv)
{
    char buf[LINE_LEN];
    FILE *fp;

    fp = fopen(argv[1], "r");
    if (!fp) {
        fprintf(stderr, "failed to open %s for reading\n", argv[1]);
        return -1;
    }

    while (fgets(buf, sizeof(buf), fp)) {
        fprintf(stderr, "%s", buf);
    }

    fclose(fp);
}

```


**Example: file system reading**

```c
#include <stdio.h>

int main(int argc, char **argv)
{
    FILE *fp;

    fp = fopen(argv[1], "r");
    if (!fp) {
        fprintf(stderr, "cannot open file %s for reading\n", argv[1]);
        return -1;
    }

    do {
        int ch;

        ch = fgetc(fp);
        if (ch == EOF)
            break;

        printf("%c", ch);
    } while (1);

    fclose(fp);

    return 0;
}

```

**Example: file read character by character**

```c
#include <stdio.h>

#define LINE_LEN 512

int main(int argc, char **argv)
{
    FILE *fp;
    int ret;
    char buf[LINE_LEN];

    fp = fopen(argv[1], "w");
    if (!fp) {
        fprintf(stderr, "failed to open %s for writing\n", argv[1]);
        return -1;
    }

    do {
        fgets(buf, sizeof(buf), stdin);
        if (buf[0] == '\n')
            break;
        fputs(fp, buf);
    } while (1);

    fclose(fp);

    return 0;
}

```

**Example: basic file writing**


###2. OS file handling

The system calls `open`, `close`, `read` and `write` perform the file system handling in the Linux.

Underlying kernel buffers the bytes that are written to or read from the file. The buffering is of kernel page size (8k or 4k) and only if the buffer size is over written, the kernel writes the contents of the file to the disk. When reading, the kernel reads the file contents before hand, and buffers them so that future reads will only happen from the buffer but not from the file (this is to save extra operations of the file to disk).

the `open` system call returns a file descriptor. The file descriptor is used as a handle in the kernel, to map to a specific portions such as inode and then to the file etc. The open system call prototype is as follows.

```c
int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);
```

| file mode permissions | description | 
|-----------------------|-------------|
| `O_CREAT` | create file |
| `O_RDWR` | open in read write mode |
| `O_RDONLY` | read only |
| `O_WRONLY` | write only |
| `O_APPEND` | append only | 
| `O_EXCL` | exclusive operation on the file when used with O_CREAT |

The two prototypes of the `open` system tells that its a variable argument function.

The first prototype is used when opening a file in read\/write mode. The second prototype is used when opening a new file and that's where the mode comes into picture.

Opening a file in read\/write mode would look as below.

```c
int file_desc;

file_desc = open(filename, O_RDWR);
```

The `O_RDWR` tells the kernel that the file must be opened in read and write mode.

Opening a new file would look as below.

```c
int file_desc;

file_desc = open(filename, O_RDWR | O_CREAT, S_IRWXU);
```

The `O_CREAT` tells the kernel that the file must be created and the `S_IRWXU` means that the file must have read \(R\), write \(W\) and executable \(X\) permissions.

The open system call on success returns a file descriptor to the file. Using this, the read or write operations can be performed on the file. On failure, the open returns -1 and sets the error number, indicating the cause of failure. The most possible failure cases can be that the permissions to open a file in the directory \(EACCESS\), too large filename \(ENAMETOOLONG\), or invlaid filename pointer.


More than one process can open a file and each time a new file descriptor is returned for the same file. Concurrent accesses to the same file must be controlled via some form of the synchronisation.

A file is referenced by an inode with in the kernel. Each inode has a unique value called inode number. An inode stores the metadata of each file, such as modification timestamp, access, creation time, owner, permissions etc.

Below example demonstrates the `open` system call with the `O_EXCL` feature. Download [here](https://github.com/DevNaga/gists/blob/master/open_excl.c)

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    int fd;

    if (argc != 2) {
        fprintf(stderr, "<%s> filename\n", argv[0]);
        return -1;
    }

    fd = open(argv[1], O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        fprintf(stderr, "failed to open file error: %s\n", strerror(errno));
        return -1;
    }

    fprintf(stderr, "opened file %s fd %d\n", argv[1], fd);

    close(fd);

    return 0;
}

```

The above example, the `O_CREAT` is used to create a file, the `O_EXCL` is used to check if the file exist, if it does, then the file open will fail. if the file does not exist, the file open succeeds and a file descriptor `fd` is returned.

when the program is run the following way:

```shell
./a.out open_excl
opened file open_excl fd 3

./a.out open_excl
failed to open file error: File exists

```

the first execution, `./a.out open_excl` returns a file descriptor because the file `open_excl` is not available. The same command when executed one more time, the failure happens with an error of `File exists`.

The `O_EXCL` is used to validate the existence of the file and only creates if it does not exist.

The flags `S_IRUSR` and `S_IWUSR` are permission bits on the file. They are described more in the below sections.

the below example describe the `O_APPEND` flag of the `open` system call. Download [here](https://github.com/DevNaga/gists/blob/master/open_excl.c)

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    int fd;

    if (argc != 2) {
        fprintf(stderr, "<%s> filename\n", argv[0]);
        return -1;
    }

    fd = open(argv[1], O_WRONLY | O_APPEND);
    if (fd < 0) {
        perror("");
        return -1;
    }

    while (1) {
        char bytes[1000];
        int ret;

        fprintf(stderr, "enter something to send to the file %s\n", argv[1]);

        ret = read(1, bytes, sizeof(bytes));
        if (ret <= 0) {
            break;
        }

        if (bytes[0] == '\n') {
            break;
        }

        write(fd, bytes, ret);
    }

    close(fd);

    return 0;
}

```

the above example opens a file in append mode for writing, taking the input from the `fd` 1 that is `stdin` and writing the output to the file descriptor opened. The program keep writing to the file unless the read returns -1 or 0 or the user pressed a new line `\n`, this checked in the first byte and the loop breaks.

Always the file is opened in append mode, no matter how many times the program has run, so the contents will be appended to the same file. Observe that the file open is performed without creating it. So the program expects that some file is already there.

The `read` and `write` operations on the file are performed using the file descriptor returned by the `open`.

The `read` system call prototype is defined as follows.


```c
size_t read(int fd, void *buf, size_t count);
```

the `read` system call returns the numbers of bytes read from the file descriptor `fd`. The `buf` is an input buffer passed to `read` and must be a valid pointer. the call fills in the buffer with the specified `count` bytes.

return `errno` values:

1. EBADF - bad fd
2. EFAULT - invalid buf pointer
3. EINTR - signal occured while reading

some of the common errors include:

1. If the file has read shorter than count bytes but more than 0 bytes, then it may be because the end of the file might have been reached.
2. The `read` system call returns 0 if the end of the file is reached and might return -1 if the calling process does not have permissions to read from the file descriptor `fd` or that the `fd` is not a valid `fd`.

A close of the file descriptor can also have the same effect of `read` returning 0, this is described later in the chapters.

the `write` system call prototype is defined as follows.


```c
size_t write(int fd, const void *buf, size_t count);
```

the `write` system call returns the number of bytes written to the file pointed by `fd` from the `buf` of `count` bytes. Writes normally does not happen directly to the underlying disk, but happens at delayed intervals when the i/o is ready. The kernel caches the writes and at suitable times (when there is an i/o with definitive cache size) writes the buffers to the disk.


return `errno` values:

1. EBADF - bad fd
2. EFAULT - invalid buf
3. EPERM - insufficient privileges

some of the most common errors include:

1. disk is full
2. bad fd  (someone closed it / programmer error)
3. no permissions to write to the fd


the write returns number of bytes written on success and -1 on error.

The `close` system call closes the file descriptor associated with the file. This is to be called when we are finished all the operations on the file.

the prototype of `close` system call is as follows

```c
int close(int fd);
```

return `errno` values:

1. EBADF - bad fd

`close` system call returns 0 on success -1 on failure. 

The numbers 0, 1 and 2 are for the standard input \(stdin\), standard output \(stdout\) and standard error \(stderr\).

The below examples give a basic idea about the file system calls under the Linux OS. Download [here](https://github.com/DevNaga/gists/blob/master/open.c)


```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define LINE_LEN 512

int main(int argc, char **argv)
{
    int fd;
    int ret;
    char buf[LINE_LEN];

    if (argc != 2) {
        fprintf(stderr, "<%s> filename\n", argv[0]);
        return -1;
    }

    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "failed to open %s for reading\n", argv[1]);
        return -1;
    }

    do {
        ret = read(fd, buf, sizeof(buf));
        if (ret <= 0)
            break;
        write(2, buf, ret);
    } while (1);

    close(fd);

    return 0;
}

```

The above example calls `open` in read only (The `O_RDONLY` flag) mode. If the file is not found, then the open fails and returns -1 and thus printing the failed to open file error on the console.

If the file is opened successfully, the `read` operation is called upon the file descriptor and the `read` calls returns number of bytes read from the file. At each read, the file contents on printed on the screen's `stderr` file descriptor that  is 2, using `write` system call.

Once the read completes, it either returns -1 or 0. This condition is checked upon always, The loop breaks and the file is closed.

**Example: basic file read via the OS calls**

```c

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define LINE_LEN 512

int main(int argc, char **argv)
{
    int fd;
    int ret;
    char buf[LINE_LEN];

    fd = open(argv[1], O_CREAT | O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "failed to open %s for writing\n",
                                argv[1]);
        return -1;
    }

    do {
        ret = read(0, buf, sizeof(buf));
        if (ret <= 0) {
            break;
        }

        if (buf[0] == '\n') {
            break;
        }

        write(fd, buf, ret);
    } while (1);


    close(fd);

    return 0;
}


```


just like the `fwrite` and `fread`, a series of data structures or binary data can be written to the file directly using the `read` and `write` system calls. Below is one example of how to do. Download [here](https://github.com/DevNaga/gists/blob/master/write_bins.c)

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

struct bin_data {
    int a;
    double b;
} __attribute__ ((__packed__));

int main(int argc, char **argv)
{
    int fd;

    if (argc != 2) {
        fprintf(stderr, "<%s> filename\n", argv[0]);
        return -1;
    }

    fd = open(argv[1], O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        fprintf(stderr, "failed to open %s\n", strerror(errno));
        return -1;
    }

    struct bin_data b[10];
    int i;

    for (i = 0; i < 10; i ++) {
        b[i].a  = i;
        b[i].b = i + 4.4;
    };

    write(fd, b, sizeof(b));

    close(fd);

    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "failed to open file %s\n", strerror(errno));
        return -1;
    }

    struct bin_data d[10];

    read(fd, d, sizeof(d));

    for (i = 0; i < 10; i ++) {
        fprintf(stderr, "a %d b %f\n", d[i].a, d[i].b);
    }

    close(fd);

    return 0;
}

```

in the above example, the file is created and a data structure is then setup and writes are made to the file. The file is then re-opened in read only and the same datastructures read back with the same size. The contents are then printed for a proof validity that the data structures infact are valid.


**Example: Basic file write via OS calls**

#### Practical example: File copy program

Here we are going to write a file copy program using the `open`, `read` and `write` system calls.

A file copy program takes a source filename on the command line and a destination file to which the source file contents to be copied.

```c
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    int fd1, fd2;
    int ret;

    if (argc != 3) {
        fprintf(stderr, "%s <source file> <destination file>\n", argv[0]);
        return -1;
    }

    fd1 = open(argv[1], O_RDONLY);
    if (fd1 < 0) {
        fprintf(stderr, "failed to open %s for reading\n", argv[1]);
        return -1;
    }

    fd2 = open(argv[2], O_CREAT | O_RDWR, S_IRWXU);
    if (fd2 < 0) {
        fprintf(stderr, "failed to create %s for writing\n", argv[2]);
        return -1;
    }

    while (1) {
        char c;

        ret = read(fd1, &c, sizeof(c));
        if (ret > 0) {
            write(fd2, &c, sizeof(c));
        } else if (ret <= 0) {
            close(fd1);
            close(fd2);
            break;
        }
    }

    return 0;
}

```


**lseek system call**

`lseek` system call allows to seek with in the file to a position specified as an argument. 

The `lseek` system call prototype is defined as follows.


```c
off_t lseek(int fd, off_t offset, int whence);
```

`lseek` returns an offset in the file pointed to by file descriptor `fd`. The `offset` and `whence` are related to each other.

the `offset` argument specifies the position in the file. the position is of the form `off_t` and whence is one of the following.

| type | definition |
|------|------------|
| `SEEK_SET` | set the offset to current given off_t bytes |
| `SEEK_CUR` | set the offset to current location + given off_t bytes |
| `SEEK_END` | set the offset to end of file + given off_t bytes |

the return error codes are :

1. EBADF - bad fd
2. EINVAL - whence argument is invalid
3. EOVERFLOW - the file size offset cannot be represented in off_t type

Below example describe the use of `lseek` `SEEK_SET` attribute. Download [here](https://github.com/DevNaga/gists/blob/master/lseek.c)

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    int fd;
    int ret;

    if (argc != 2) {
        fprintf(stderr, "<%s> filename\n", argv[0]);
        return -1;
    }

    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        return -1;
    }

    while (1) {
        char bytes[1000];

        ret = read(fd, bytes, sizeof(bytes));
        if (ret <= 0) {
            // reached eof .. set to beginning of the file
            lseek(fd, 0, SEEK_SET);

            fprintf(stderr, " reached EOF.. setting to the beginning of file\n");

            sleep(1);
        } else {
            write(2, bytes, ret);
        }
    }

    close(fd);

    return 0;
}

```

in the above example, the `read` returns a -1 or 0 when it reaches an end of file, and there, the position of the file is moved to the beginning by setting the offset bytes to 0 and using `SEEK_SET` as it sets the position to given bytes. The `sleep` call is added to let the display slowly and repeatedly print the file contents using the below `write` system call. The `write` call is performed on the `stderr` file descriptor that is 2.

Below is another example of `lseek` that is used to find the file size. Download [here](https://github.com/DevNaga/gists/blob/master/lseek_size.c)

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    int fd;
    off_t offset;
    struct stat s;
    int ret;

    if (argc != 2) {
        fprintf(stderr, "<%s> filename\n", argv[0]);
        return -1;
    }

    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        return -1;
    }

    offset = lseek(fd, 0, SEEK_END);

    close(fd);

    printf("file size offset %ld\n", offset);

    ret = stat(argv[1], &s);
    if (ret < 0) {
        return -1;
    }

    printf("file size from stat %ld\n", s.st_size);

    return 0;
}

```


The above example, the `open` system call gives out an fd for a given valid file, then the `lseek` is called upon the `fd` with the offset 0 bytes and the `SEEK_END`. The `lseek` returns the offset currently in the file, basically this is starting at the beginning of the file till the last point, effectively counting the bytes in the file.

The same is verified by the `stat` system call with the file name as argument, the `stat` returns the `struct stat` which then contain the `st_size` member variable that outputs the file size.

running the above example prints,

```shell
./a.out lseek_size.c 
file size offset 608
file size from stat 608

```


### truncate and ftruncate

the `truncate` and `ftruncate` system calls are useful in truncating the file to a given size without adding anything into the file. These are very useful in coming sections where a memory map is used to write to files than the regular file io calls. See `mmap` section for more details on where `truncate` or `ftruncate` are being used.

The prototype of `truncate` is as follow.

```c
int truncate(const char *path, off_t bytes);
```

the prototype of `ftruncate` is as follow.

```c
int ftruncate(int fd, off_t bytes);
```

the `ftruncate` is different from `truncate` in the sense that it accepts only the `fd` than the filename.

Below example describe the use of `truncate` and `ftruncate`. Download [here](https://github.com/DevNaga/gists/blob/master/truncate.c)

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

long filesize(char *filename)
{
    struct stat st;
    int ret;

    ret = stat(filename, &st);
    if (ret < 0) {
        return -1;
    }

    return st.st_size;
}

int main(int argc, char **argv)
{
    long size;
    int fd;
    int ret;

    if (argc != 2) {
        fprintf(stderr, "<%s> filename\n", argv[0]);
        return -1;
    }

    ret = truncate(argv[1], 1024 * 1024);
    if (ret < 0) {
        fprintf(stderr, "failed to truncate file to 1M error :%s \n", strerror(errno));
        return -1;
    }

    size = filesize(argv[1]);

    fprintf(stderr, "truncated file %s file size %ld\n", argv[1], size);

    fprintf(stderr, "unlinking %s\n", argv[1]);

    ret = unlink(argv[1]);
    if (ret < 0) {
        fprintf(stderr, "failed to unlink %s\n", strerror(errno));
        return -1;
    }

    fd = open(argv[1], O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        fprintf(stderr, "failed to open file %s\n", argv[1]);
        return -1;
    }

    ret = ftruncate(fd, 1024 * 1024);
    if (ret < 0) {
        fprintf(stderr, "failed to ftruncate file to 1M error: %s\n", strerror(errno));
        return -1;
    }

    size = filesize(argv[1]);

    fprintf(stderr, "truncated file %s filesize %ld\n", argv[1], size);

    return 0;
}

```


### Random number generator

The linux kernel provides a device interface to the psuedo randomnumber generator. The device is called **/dev/urandom**.

This device is opened like any other file in the linux. A read call on the device with the given length would give that length data back in random.

This random number or array is then used as a seed to a random number generator.

The sample program is located [here](https://github.com/DevNaga/gists/blob/master/dev_random.c), and is also printed below for your reference.

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    int ret;
    int rand_fd;

    rand_fd = open("/dev/urandom", O_RDONLY);
    if (rand_fd < 0) {
        fprintf(stderr, "cannot open /dev/urandom for reading\n");
        return -1;
    }

    unsigned char randbytes[16];

    read(rand_fd, randbytes, sizeof(randbytes));

    int i;

    for (i = 0; i < sizeof(randbytes); i++) {
        printf("%02x", randbytes[i]);
    }

    printf("\n");

    return 0;
}

```

with the above example code, one can write a simple C++ class for various uses. Such as getting a byte, two bytes, an integer, a stream of random data etc. Below is an example of such class, Download [here](https://github.com/DevNaga/gists/blob/master/random.cpp)


```cpp
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

class RandomGen {
    private:
        int Fd_;
    
    public:
        RandomGen();
        int getRandBytes(char *bytes);
        int getRandBytes(short *bytes);
        int getRandBytes(int *bytes);
        int getRandBytes(uint8_t *bytes, int len);
        ~RandomGen();
};

RandomGen::RandomGen()
{
    Fd_ = open("/dev/urandom", O_RDONLY);
    if (Fd_ < 0) {
        return;
    }
}

RandomGen::~RandomGen()
{
    if (Fd_ > 0) {
        close(Fd_);
    }
}

int RandomGen::getRandBytes(char *bytes)
{
    return read(Fd_, bytes, sizeof(char));
}

int RandomGen::getRandBytes(short *bytes)
{
    return read(Fd_, bytes, sizeof(short));
}

int RandomGen::getRandBytes(int *bytes)
{
    return read(Fd_, bytes, sizeof(int));
}

int RandomGen::getRandBytes(uint8_t *bytes, int len)
{
    return read(Fd_, bytes, len);
}

```

In the above example, the `RandomGen` class has a constructor opening a file descriptor to the `/dev/urandom` in read only mode. The class then defines a overloaded function `getRandBytes` that can be called for one bytes, two, four or a series of bytes to receive for random data.

The class destructor is called upon, when the class goes out of scope , and it closes the file descriptor.

This class demonstrates an easy and alternate way of getting random numbers than using the highly unsecure `rand` or `srand` functions.


POSIX standard defines  `_LARGEFILE64_SOURCE` to support reading or writing of files with sizes more than 2GB.


For syncing to the disk the Linux supports `fsync`.

The `fsync` system call syncs the file to the disk flushing the buffers. The prototype is as follows,

```c
int fsync(int fd);
```

it accepts a file descriptor of the file and returns 0 on success and -1 on failure.

the `sync` system call syncs the entire system buffers to disk. The prototype is as follows,

```c
void sync(void);

```

the command `sync` does the smae job as well.

While shutting the system down, it is usually a good practise to `sync` the system buffers to the disk, if there is a sufficient power.

