# Sixth chapter

##File handling

###1. C file handling

The function call set `fopen`, `fclose`, `fgets`, `fputs`, `fread` and `fwrite` can be used to perform basic to intermediate file handling.

The FILE is the handle returned by the `fopen` call.

the `fopen` takes the following form:

    fopen(const char *file, char *mode);
    
the fopen returns the pointer of type FILE. This also is called as file handle. The mode argument is one of "r", "w", "a" etc. The "r" argument is used to perform "read" on the file, the "w" argument is used to perform "write" on the file and the "a" argument is used to perform "append" on the file.

    FILE *fp;
    
    fp = fopen("file", "w");

the `fopen` returns `NULL` if the file can't be opened.

The opened file can be used to perform operations on the file. The `fgets` is used to perform the read operation and `fputs` is used to perform the write operation on the file respectively.

Contents of the file can be read character by character using the `fgetc` and can be written character by character using the `fputc` call.

The `stdin`, `stdout` and `stderr` are the standard file descriptors for the standard input stream, output stream and error stream.

A call to `fgets` will read the contents of the string into the buffer of given length terminated by the '\n\0'.

So when performing `fgets`, the buffer should be stripped with `\n`. such as the following

    buf[strlen(buf) - 1] = '\0'; // terminate the new line with '\0' character.
    
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
}```


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
}```

**Example: basic file writing**


###2. OS file handling

The system calls `open`, `close`, `read` and `write` perform the file system handling in the Linux.

Underlying kernel buffers the bytes that are written to or read from the file. The buffering is of kernel page size (8k or 4k) and only if the buffer size is over written, the kernel writes the contents of the file to the disk. When reading, the kernel reads the file contents before hand, and buffers them so that future reads will only happen from the buffer but not from the file (this is to save extra operations of the file to disk).

the `open` system call returns a file descriptor. The file descriptor is used as a handle in the kernel, to map to a specific portions such as inode and then to the file etc. The open system call prototype is as follows.

```c
int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);
```

The two prototypes of the `open` system tells that its a variable argument function.

The first prototype is used when opening a file in read/write mode. The second prototype is used when opening a new file and that's where the mode comes into picture.

Opening a file in read/write mode would look as below.

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

The `O_CREAT` tells the kernel that the file must be created and the `S_IRWXU` means that the file must have read (R), write (W) and executable (X) permissions.

The open system call on success returns a file descriptor to the file. Using this we can perform the read or write on the file. On failure, the open returns -1 and sets the error number, indicating the cause of failure. The most possible failure cases can be that the permissions to open a file in the directory (EACCESS), too large filename (ENAMETOOLONG), or invlaid filename pointer.

The `read` and `write` operations on the file are performed using the file descriptor returned by the `open`.

The `close` system call closes the file descriptor associated with the file. This is to be called when we are finished all the operations on the file.

The numbers 0, 1 and 2 are for the standard input (stdin), standard output (stdout) and standard error (stderr).

The below examples give a basic idea about the file system calls under the Linux OS.


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
    
    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "failed to open %s for reading\n", argv[1]);
        return -1;
    }
    
    do {
        ret = read(fd, buf, sizeof(buf));
        if (ret <= 0)
            break;
        fprintf(stderr, "%s\n", buf);
    } while (1);
    
    close(fd);
    
    return 0;
}```

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
```c

### Random number generator

The linux kernel provides a device interface to the psuedo randomnumber generator. The device is called **/dev/random**.

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

    rand_fd = open("/dev/random", O_RDONLY);
    if (rand_fd < 0) {
        fprintf(stderr, "cannot open /dev/random for reading\n");
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

Programs:

1. Get an integer number from **/dev/random**
2. Find a problem with the above code. Provide your fix.

