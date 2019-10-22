## Inotify

The `inotify` are a set of system calls that are useful to monitor the files and directories. When a directory is modified, inotify will return events from the directory itself and also for the files inside the directory.

The watching on the files for any changes is done for the following purposes \/ needs.

1. configuration changes
2. automatic backup program triggering
3. directory monitoring for suspicious activity
4. automatic crash dump collection of a program \(by watching the core files directory and uploading them to the server when any new cores get added\)

The following are the useful inotify calls.

```C
int inotify_init(void);
int inotify_add_watch(int fd, const char *path, uint32_t mask);
int inotify_rm_watch(int fd, uint32_t mask);
```

Include `sys/inotify.h` to use the above API.

The `inotify_init` creates a file descriptor that can be used to receive the file or directory events. The file descriptor can also be monitored via the `select` calls.

The `inotify_add_watch` adds a new watch entry or modifies an existing watch entry to the list of monitored files or directories. The `fd` is the file descriptor returned from the `inotify_init`. The `path` is either a file or a directory. 

The `inotify_rm_watch` removes the inotify watch that was previously added with the `inotify_add_watch`.

The returned fd is monitored via the `select`, `poll`, `epoll` system calls. The `read` call is then called upon a return from the `select`, `poll` or `epoll` to read the events.


The mask argument in the `inotify_add_watch` has the following types.

| mask | description |
| :--- | :--- |
| `IN_ACCESS` | File was accessed via read |
| `IN_ATTRIB` | permissions, timestamps, such as file options have changed |
| `IN_CLOSE_WRITE` | file opened for writing was closed |
| `IN_CLOSE_NOWRITE` | file or directory not opened for writing was closed |
| `IN_CREATE` | File\/Directory created in the watched directory |
| `IN_DELETE` | File\/Directory deleted from watched directory |
| `IN_DELETE_SELF` | Watched file\/directory itself is deleted |
| `IN_MODIFY` | File was modified |
| `IN_MOVE_SELF` | Watched file\/directory itself moved |
| `IN_MOVED_FROM` | Generated for the directory containing the new filename when a file is renamed |
| `IN_MOVED_TO` | Generated for the directory containing the new filename when a file is renamed |
| `IN_OPEN` | File\/Directory was opened |

The `IN_ALL_EVENTS` flag is used to monitor all of the above events.

When there is an event, the `read` system call on the inotify fd might return a set of events instead of filling one event at a time.

The events are a set of objects of type `struct inotify_event` that looks as follows (for reference).

```c
struct inotify_event {
    int wd;   // watch descriptor
    uint32_t mask;  // mask describing the event
    uint32_t cookie; // session cookie or something
    uint32_t len;  // length of the below `name` field
    char name[];  // optional null-terminated string that contains the name of the directory / file being monitored
};
```

The reading of the events might look as the following ...

```c
int ret;
int processed = 0;

while (processed < read_bytes) {
    struct inotify_event *event;

    event = (struct inotify_event *)(buf + processed);
    // event processing
    processed += sizeof(struct inotify_event);
}
```

Below is the sample program that demonstrate the `inotify` system call usage.

```c
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/inotify.h>

int main(int argc, char **argv)
{
    int fd;
    int ret;
    char buf[4096] __attribute__((aligned(8)));
    int num_read;
    struct inotify_event *event;

    if (argc != 2) {
        printf("%s [file / directory]\n", argv[0]);
        return -1;
    }

    fd = inotify_init();
    if (fd < 0) {
        printf("failed to inotify_init\n");
        return -1;
    }

    ret = inotify_add_watch(fd, argv[1],
                                    IN_ACCESS |
                                        IN_CREATE |
                                        IN_DELETE |
                                        IN_OPEN);
    while (1) {
        int processed = 0;

        ret = read(fd, buf, sizeof(buf));
        if (ret < 0) {
            break;
        }

        while (processed < ret) {
            event = (struct inotify_event *)(buf + processed);

            if (event->mask & IN_ACCESS) {
                printf("Read event on file %s\n", event->name);
            }

            if (event->mask & IN_CREATE) {
                printf("File created %s\n", event->name);
            }

            if (event->mask & IN_DELETE) {
                printf("File deleted %s\n", event->name);
            }

            if (event->mask & IN_OPEN) {
                printf("File is in open %s\n", event->name);
            }
            processed += sizeof(struct inotify_event);
        }
    }
}
```

**Example: inotify example**

The `inotify` is mostly used by the build systems / continuous integration tools to monitor directories (such as the ones that contain the release images for a new software release etc) for some interesting events.
