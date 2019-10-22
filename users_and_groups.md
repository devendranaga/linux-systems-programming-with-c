## users and groups

The password file `/etc/passwd` contain details about each user with name, password (not in clear text) and UID, GID and current working directory.

The system call `getpwnam` is used to get the information for each user.

The `getpwnam` returns what is called `struct passwd`. It looks like the following,

```c
struct passwd {
    char *pw_name; // name of the user
    char *pw_passwd; // passwd
    uid_t pw_uid;
    gid_t pw_gid;
    char *pw_gecos; // user information
    char *pw_dir; // home directory
    char *pw_shell; // shell program
}

```

The prototype of `getpwnam` looks like below,

```c
struct passwd * getpwnam(const char *name);
```

Below is an example of `getpwnam`. Download [here](https://github.com/DevNaga/gists/blob/master/getpass.c)

```c
#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "<%s> username\n", argv[0]);
        return -1;
    }

    struct passwd *pw;

    pw = getpwnam(argv[1]);
    if (!pw) {
        return -1;
    }

    printf("name %s\n", pw->pw_name);
    printf("password %s\n", pw->pw_passwd);
    printf("uid %d\n", pw->pw_uid);
    printf("gid %d\n", pw->pw_gid);
    printf("user informatino %s\n", pw->pw_gecos);
    printf("home %s\n", pw->pw_dir);
    printf("shell %s\n", pw->pw_shell);

    return 0;
}

```
