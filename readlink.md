## readlink

readlink resolves the symbolic links. The readlink API prototype is as follows.

```c
size_t readlink(const char *pathname, char *buf, size_t bufsiz);
```

Also, include the `<unistd.h>` header file.

the `pathname` is the symbolic link and the readlink API dereferences the symbolic link and places the real name into the `buf` argument. The `readlink` API does not terminate the `buf` pointer with a `\0`. Thus we should be doing the `buf[readlink_ret + 1] = '\0'` to make sure the buffer is null terminated.

The below example demonstrates the `readlink`

```c
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    char buf[100];
    size_t len;

    if (argc != 2) {
        fprintf(stderr, "%s <linkfile name>\n", argv[0]);
        return -1;
    }

    memset(buf, 0, sizeof(buf));

    len = readlink(argv[1], buf, sizeof(buf));
    if (len == -1) {
        fprintf(stderr, "failed to resolve the link for %s\n", argv[1]);
        return -1;
    }

    buf[len + 1] = '\0';

    fprintf(stderr, "resolved %s\n", buf);

    return 0;
}
```

we compile and run the program on to one of the files under /proc. For ex: when run with the `/proc/1/fd/1` the program gives us:

`resolved /dev/null`

The real name of the `/proc/1/fd/1` is actually `/dev/null`.

Here is another example of `readdir` that reads a directory and describes the links with in the directory.

```c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    int ret;
    DIR *dirp;
    struct dirent *entry;

    if (argc != 2) {
        fprintf(stderr, "%s <directory>\n", argv[0]);
        return -1;
    }

    dirp = opendir(argv[1]);
    if (!dirp) {
        fprintf(stderr, "failed to open %s\n", argv[1]);
        return -1;
    }

    while (entry = readdir(dirp)) {
        char path[400];
        size_t len;
        char realname[400];
        struct stat s;

        memset(path, 0, sizeof(path));
        strcpy(path, argv[1]);
        strcat(path, "/");
        strcat(path, entry->d_name);

        ret = stat(path, &s);
        if (ret < 0) {
            fprintf(stderr, "failed to stat %s\n", path);
            continue;
        }

        if (S_ISDIR(s.st_mode)) {
            continue;
        }

        printf("filename %s\t", path);

        memset(realname, 0, sizeof(realname));

        len = readlink(path, realname, sizeof(realname));
        if (len < 0) {
            fprintf(stderr, "failed to readlink\n");
            return -1;
        }

        if (len > sizeof(realname)) {
            fprintf(stderr, "too large realname\n");
            continue;
        }

        realname[len + 1] = '\0';

        printf("realname %s\n", realname);
    }

    closedir(dirp);

    return 0;
}
```

The program opens a directory with the `opendir` system call and reads it using `readdir` till the end of the file is reached. At each entry read, we check if the file is a directory and drop dereferencing it. Otherwise, we reference the link using the `readlink` and print the realname of the link.

The program is careful at avoiding the buffer overflow when the length of the name exceeds the length of the buffer. In such cases we continue to the next name.
