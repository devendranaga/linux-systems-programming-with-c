## Pipes and Fifos

### Pipe

A `pipe` is a method to connect an output of one process to the input of another process. The `pipe` provides a one way communication between the processes.

Example:

```bash
ls | sort
```

The above `|` is called a pipe that pipes the output of ls to the input of sort.

The `pipe` system call is used to create a pipe in C.

The following is the prototype of the function.

```c
int pipe(int pipefd[2]);
```

The `pipefd[0]` is the read end of the pipe and `pipefd[1]` is the write end of the pipe.

On success 0 is returned from the function and on failure -1.

include header file `unistd.h` for the `pipe` system call.

Here is the below example describing the usage of `pipe` system call. You can also download the program [here](https://github.com/DevNaga/gists/blob/master/pipes.c)

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	int pipefd[2];
	int ret;

	ret = pipe(pipefd);
	if (ret < 0) {
		fprintf(stderr, "failed to create a pipe\n");
		return -1;
	}

	pid_t child_pid;

	child_pid = fork();
	if (child_pid == 0) {
		char msg[100];

		close(pipefd[1]);
		read(pipefd[0], msg, sizeof(msg));
		printf("read %s\n", msg);
		exit(1);
	} else {
		char msg[] = "Hello ";

		close(pipefd[0]);
		write(pipefd[1], msg, sizeof(msg));
		exit(1);
	}

	return 0;
}
```

### Fifo

A fifo is a named pipe and works same as the `pipe`. 

One way to create a fifo is the following

```bash
mknod fifo_file p
mfifo a=rw fifo_file
```

`mknod` creates a block or special character files. The `p` option is used to create a fifo.

`mkfifo` creates a fifo with the given name. The `a` option is used to open the file in `rw` mode.

There are `mknod` and `mkfifo` system calls and are defined below.

```c
int mknod(const char *path, mode_t mode, dev_t dev);
int mkfifo(const char *path, mode_t mode);
```

Example of the calls are shown as below

```c
mknod("/home/devnaga/fifo_file", S_IFIFO | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, 0);
mkfifo("/home/devnaga/fifo_file", S_IFIFO | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

```

sometimes, `S_IFIFO` is omitted.

once a fifo is created an `open` call is performed on the fifo file at either ends. At the creator end, the call shall be made with `O_CREAT` option.

the code example that does it is as follows,


```c

int ret;
int fd;

ret = mkfifo ("/fifo_test", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
if (ret < 0) {
	return -1;
}

fd = open("/fifo_test", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
if (fd < 0) {
	return -1;
}

```

Below is an example of a program that use fifo for the communication. Download [here](https://github.com/DevNaga/gists/blob/master/fifo.c)

```c
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    int ret;

    if (argc != 2) {
        fprintf(stderr, "<%s> create/open\n", argv[0]);
        return -1;
    }

    int fd;

    if (!strcmp(argv[1], "create")) {
        unlink("/fifo_test");

        ret = mkfifo("/fifo_test", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
        if (ret < 0) {
            return -1;
        }

        fd = open("/fifo_test", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
        if (fd < 0) {
            return -1;
        }

        while (1) {
            char *sender_msg = "Hello from sender";
            write(fd, sender_msg, strlen(sender_msg) + 1);

            sleep(1);
        }
    } else if (!strcmp(argv[1], "open")) {
        fd = open("/fifo_test", O_RDWR);
        if (fd < 0) {
            return -1;
        }

        while (1) {
            char rxmsg[200];

            ret = read(fd, rxmsg, sizeof(rxmsg));
            if (ret <= 0) {
                break;
            }

            printf("msg fromsender: %s\n", rxmsg);
        }
    }

    return 0;
}

```

The above program creates a fifo in `create` portion of the code and opens it in read write, the arguments "create / open" are passed via the command line. In `open` mode, the fifo is simply opened for read and write.

run the above program in two separate terminals and observe the output.
