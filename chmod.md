# permission system calls
# chmod system call
# chown system call
# access system call

The `access` system call prototype is as follows.

```c
int access(const char *pathname, int mode);
```

`access` system call checks whether the calling process can access the `pathname`. The mode is one of the following.

| mode | description|
|------|------------|
| `F_OK` | check if the file exists |
| `R_OK` | check if the file is read accessible |
| `W_OK` | check if the file is write accessible |
| `X_OK` | check if the file is executable accessible |

include the header file `<unistd.h>`.

The below program depicts the usage of the `access` API. You can also download it in [here](https://github.com/DevNaga/gists/blob/master/access.c).

```c
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "%s [filename] [mode]\n", argv[0]);
        return -1;
    }

    int ret;
    int mode;

    if (!strcmp(argv[2], "F_OK")) {
        mode = F_OK;
    } else if (!strcmp(argv[2], "R_OK")) {
        mode = R_OK;
    } else if (!strcmp(argv[2], "W_OK")) {
        mode = W_OK;
    } else if (!strcmp(argv[2], "X_OK")) {
        mode = X_OK;
    } else {
        fprintf(stderr, "invalid option %s entered\n", argv[2]);
        return -1;
    }

    ret = access(argv[1], mode);
    if (ret < 0) {
        fprintf(stderr, "failed to access %s error : %s\n",
                                argv[1], strerror(errno));
        return -1;
    }

    fprintf(stderr, "%s is ok\n", argv[2]);
    return 0;
}
```
