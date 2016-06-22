# stat system call

The stat system call is very useful in knowing the information about the file. The information such as file last access time (read and write), created time, file mode, permissions, file type, file size etc are provided by the stat system call. These are all stored in the `struct stat` data structure. This we need to pass when calling the stat().

The `stat` looks like below:

`int stat(const char *path, struct stat *s);`

stat returns 0 on success. So we should only access the structure returned in the stat call if it returns 0.

The most common usage of `stat` is to know if the given file is a regular file or directory.

```c
char *path = "/home/dev/work/test.c"
struct stat s;

if (stat(path, s) < 0) {
    fprintf(stderr, "failed to stat %s\n", s);
    perror("stat:");
    return -1;
}

if (s.st_mode & S_IFREG) {
    fprintf(stderr, "regular file\n");
} else {
    fprintf(stderr, "unknown or un-tested file type\n");
}
```

**Example: basic stat example**


The `stat` system call is often used in conjunction with the `readdir` system call, to find if the path contains a file or a directory.
