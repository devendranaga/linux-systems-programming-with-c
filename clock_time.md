

## more precise timing

The `clock_` API provide futher timing information in the preciwsion of nanoesconds.

Below are some of the APIs.

1. `clock_gettime`
2. `clock_settime`

The `clock_gettime` API prototype is as follows.

```c

int clock_gettime(clockid_t clk_id, struct timespec *tp);

```

The `clock_settime` API prototype is as follows.

```c

int clock_settime(clockid_t clk_id, const struct timespec *tp);

```

Both the APIs follow the similar prototype but the second argument of `clock_settime` is a const so the `tp` argument cannot be modified.

The clockid_t is one of the following mentioned in the table.


| clock type | description |
|------------|-------------|
| `CLOCK_MONOTONIC` | monotonic clock |
| `CLOCK_REALTIME` | realtime clock |
| `CLOCK_BOOTIME` | time since boot |
| `CLOCK_PROCESS_CPUTIME_ID` | process cpu time |
| `CLOCK_THREAD_CPUTIME_ID` | thread cpu time |

The `timespec` structure is defined as below,


```c

struct timespec {
    time_t tv_sec;
    long tv_nsec;
};

```

Below program describe more details on the use of `clock_gettime` API:

```c

#include <stdio.h>
#include <time.h>

int main()
{
    struct timespec tp;
    int ret;

    ret = clock_gettime(CLOCK_MONOTONIC, &tp);
    if (ret != 0) {
        return -1;
    }

    printf("CLOCK_MONOTONIC: %ld.%ld\n", tp.tv_sec, tp.tv_nsec);

    ret = clock_gettime(CLOCK_REALTIME, &tp);
    if (ret != 0) {
        return -1;
    }

    printf("CLOCK_REALTIME: %ld.%ld\n", tp.tv_sec, tp.tv_nsec);

    ret = clock_gettime(CLOCK_BOOTTIME, &tp);
    if (ret != 0) {
        return -1;
    }

    printf("CLOCK_BOOTTIME: %ld.%ld\n", tp.tv_sec, tp.tv_nsec);

    ret = clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp);
    if (ret != 0) {
        return -1;
    }

    printf("CLOCK_PROCESS_CPUTIME_ID: %ld.%ld\n", tp.tv_sec, tp.tv_nsec);

    ret = clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tp);
    if (ret != 0) {
        return -1;
    }

    printf("CLOCK_THREAD_CPUTIME_ID: %ld.%ld\n", tp.tv_sec, tp.tv_nsec);
}

```

Below is the output of the program.

```bash

CLOCK_MONOTONIC: 17647.376058488
CLOCK_REALTIME: 1586512178.911238652
CLOCK_BOOTTIME: 17924.278885330
CLOCK_PROCESS_CPUTIME_ID: 0.3407998
CLOCK_THREAD_CPUTIME_ID: 0.3441832

```

Sometimes the `clock_gettime` is used as a base for the performance analyses process. The `clock_gettime` is used in combination with `CLOCK_MONOTONIC` to find the deltas. For example below code show the typical example.

```c

struct timespec start, stop;

struct timespec delta;

// start the measurement
clock_gettime(CLOCK_MONOTONIC, &start);

code_that_is_measurement();

// stop measuring
clock_gettime(CLOCK_MONOTONIC, &stop);

// perform delta calculation
delta_timespec(&start, &stop, &delta);
...

void delta_timespec(struct timespec *start,
                    struct timespec *stop,
                    struct timespec *delta)
{
    if ((stop->tv_nsec - start->tv_nsec) < 0) {
        delta->tv_sec = stop->tv_sec - start->tv_sec - 1;
        delta->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000ULL;
    } else {
        delta->tv_sec = stop->tv_sec - start->tv_sec;
        delta->tv_nsec = stop->tv_nsec - start->tv_nsec;
    }
}

```


In general the `clock_settime` is used to set the time to the system from a time source for example,

```c

struct timespec ts;

// set time in ts

ret = clock_settime(CLOCK_REALTIME, &ts);
if (ret < 0) {
    return -1;
}

```

To get the resolution `clock_getres` API is used.

The `clock_getres` prototype is as follows.


```c

int clock_getres(clockid_t id, struct timespec *ts);

```

However, only three of the clockids are being used. `CLOCK_MONOTONIC`, `CLOCK_REALTIME` and `CLOCK_PROCESS_CPUTIME_ID`.

an example use is as follows,

```c

#include <stdio.h>
#include <time.h>

int main()
{
    struct timespec ts;
    int ret;

    ret = clock_getres(CLOCK_MONOTONIC, &ts);
    if (ret < 0) {
        return -1;
    }

    printf("resolution: %ld.%ld\n", ts.tv_sec, ts.tv_nsec);
}

```


For more precise sleep the `clock_nanosleep` is used. The `clock_nanosleep` prototype is as follows.

```c

int clock_nanosleep(clockid_t clock_id, int flags,
                const struct timespec *req,
                struct timespec *rem);
                
```

the `clock_id` is one of the three arguments `CLOCK_MONOTONIC`, `CLOCK_REALTIME` and `CLOCK_PROCESS_CPUTIME_ID`.

the `flags` represent either `0` or `TIME_ABSTIME`. In general pass it `0`.

The `req` contain the time in seconds and nanoseconds to sleep.

If there is an interrupt, the `clock_nanosleep` will return nonzero err num.

Below is an example usecase of `clock_nanosleep`.

```c

#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

void signal_handle()
{
}

int main()
{
    struct timespec tv = {
        .tv_sec = 4,
        .tv_nsec = 0,
    };
    struct timespec other = {
        0, 0
    };
    int ret;

    signal(SIGINT, signal_handle);

    while (1) {
        ret = clock_nanosleep(CLOCK_REALTIME, 0, &tv, &other);
        printf("Ret %d err %d\n", ret, errno);
        if (ret == EINTR) {
            printf("others %ld.%ld\n", other.tv_sec, other.tv_nsec);
            if ((other.tv_sec != 0) || (other.tv_nsec != 0)) {
                tv.tv_sec = other.tv_sec;
                tv.tv_nsec = other.tv_nsec;
            } else {
                break;
            }
        } else {
            break;
        }
    }
}

```


