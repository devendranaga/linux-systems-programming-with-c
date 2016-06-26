# Directory manipulation

## Reading / Writing Directories programmatically under linux:

Under the linux, directory is read by using the `ls` command. The `ls` command performs the listing of files and directories. The `mkdir` command performs the creation of directories. The `touch` command creates a file. The `ls`, `mkdir` and `touch` run in a shell (such as `bash` or `ash` or `sh`).


The `ls` command performs directory reading to list out the contents. The contents can be files or directories. There are many types of files with in the linux.

The `opendir` system call opens up a directory that is given as its first argument. It returns a directory tree node using which we can effectively traverse the directory using the `readdir` system call. The `readdir` call is repeatedly called over the directory tree node until the return value of the `readdir` becomes `NULL`.

The manual page of the `opendir` gives us the following prototype.

    DIR * opendir(const char *name);

The call returns the directory tree node of type `DIR` as a pointer.

The manual page of the `readdir` gives us the following prototype.

    struct dirent * readdir(DIR *dir);

The `readdir` takes the directory tree node pointer of type `DIR` that is returned from the `opendir` call. The `readdir` call is repeatedly called until it returns `NULL`. Each call to the `readdir` gives us a directory entry pointer of type `struct dirent`. From the man pages this structure is as follows:

```c
struct dirent {
    ino_t          d_ino;       /* inode number */
    off_t          d_off;       /* not an offset; see NOTES */
    unsigned short d_reclen;    /* length of this record */
    unsigned char  d_type;      /* type of file; not supported
                                              by all filesystem types */
    char           d_name[256]; /* filename */
};
```

The `d_name` and `d_type` elements in the structure are the most important things to us. The `d_name` variable gives us the file / directory that is present under the parent directory. The `d_type` tells us the type of the `d_name`. If the `d_type` is `DT_DIR`, then the `d_name` contains a directory, and if the `d_type` is `DT_REG`, then the `d_name` is a regular file.

An `opendir` call must follow a call to `closedir` if the `opendir` call is successful. From the man pages, the `closedir` call looks below:

```c
int closedir(DIR *dirp);
```

The `closedir` will close the directory referenced by `dirp` pointer and frees up any memory that is allocated by the `opendir` call.

The below example is a basic `ls` command that perform the listing of the directory contents. It does not perform the listing of symbolic links, permission bits, other types of files.

(You can download the example from [here](https://github.com/DevNaga/gists/blob/master/basic_listdir.c))

```c
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

int main(int argc, char **argv)
{
    DIR *dir;
    struct dirent *entry;
    char *dirname = NULL;

    if (argc == 1)
        dirname = ".";
    else
        dirname = argv[1];

    dir = opendir(dirname);
    if (!dir) {
        fprintf(stderr, "failed to open %s\n", dirname);
        return -1;
    }

    while (entry = readdir(dir)) {
        switch (entry->d_type) {
            case DT_DIR:
                fprintf(stderr, "dir         ");
            break;
            case DT_REG:
                fprintf(stderr, "reg         ");
            break;
            default:
                fprintf(stderr, "unknown     ");
            break;
        }

        fprintf(stderr, "%s\n", entry->d_name);
    }

    closedir(dir);

    return 0;
}
```

**
Example: Basic `ls` command example**

The above example simply lists down the files and directories. By taking this as an example, we can solve the below programming problems.

1. Sort the contents of the directory and print them.
2. Recursively perform reading of the directories with in the parent directories until there exist no more directories. The directories "." and ".." can be ignored.

Some file systems does not set the `entry->d_type` variable. Thus it is advised to perform the `stat` system call.

The following example shows how a `stat` system call is used to find out the filetype.

```c
struct stat s;
char buf[1000];

while (entry = readdir(dirp)) {
    memset(buf, 0, sizeof(buf));
    strcpy(buf, directory);
    strcat(buf, "/");
    strcpy(buf, dir->d_name);

    if (stat(buf, &s)) {
        printf("failed to stat %s\n", buf);
        continue;
    }

    if (S_ISREG(s.st_mode)) {
        printf("regular file %s\n", buf);
    } else if (S_ISDIR(s.st_mode)) {
        printf("directory %s\n", buf);
    } else if (S_ISLNK(s.st_mode)) {
        printf("link %s\n", buf);
    }
}
```
### Creating directories with ```mkdir```

The `mkdir` also a system call that creates a directory. The command `mkdir` with option `-p` would recursively create the directories. However, the `mkdir` system call would only create one directory.

The below program demonstrates a series of calls that manipulate or get the information from the directories. You can also view / download it [here](https://github.com/DevNaga/gists/blob/master/mkdir.c)


```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv)
{
    char dirname[40];
    int ret;

    ret = mkdir(argv[1], 0755);
    if (ret < 0) {
        perror("failed to mkdir: ");
        return -1;
    }

    printf("successfully created %s\n", argv[1]);

    printf("going into %s\n", argv[1]);

    ret = chdir(argv[1]);
    if (ret < 0) {
        perror("failed to chdir: ");
        return -1;
    }

    printf("inside %s\n", getcwd(dirname, sizeof(dirname)));

    return 0;
}

```

The `chdir` system call changes the current working directory of the program. The `getcwd` system call gets the current directory the program is under.

However, when we compile and run the above program with good inputs as the following:

        ./mkdir_program test/
        successfully created test/
        going into test/
        inside test/

and when the program exits, we are still in the directory where the program is compiled and run.

This is because the program does not affect the current directory of the shell (Shell is however a program too). The directory change is only affected to the program but not to anything else in the userspace if they are not related.

## scandir

The `scandir` scans the directory and calls the `filter` and `compar` functions and returns a list of `struct dirent` datastructures and returns the length of them. The prototype of the `scandir` looks as below..

```c
int scandir(const char *dir, struct dirent ***list,
            int (*filter)(const struct dirent *),
            int (*compar)(const struct dirent **, const struct dirent **));
```

The compare function can be a sorting function that arranges the files in an order. The `Glibc` has `alphasort` API to call as `compar` function. On success it returns the number of directory entries selected. On failure, it returns -1. Below is one of the example..

```c
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

int main(int argc, char **argv)
{
    struct dirent **list;
    int n, i;

    if (argc != 2) {
        printf("%s [directory] \n", argv[0]);
        return -1;
    }

    n = scandir(argv[1], &list, NULL, alphasort);
    if (n < 0) {
        printf("failed to scandir %s\n", argv[1]);
        return -1;
    }

    for (i = 0; i < n; i ++) {
        printf("name %s\n", list[i]->d_name);
        free(list[i]);
    }
    free(list);

    return 0;
}
```

