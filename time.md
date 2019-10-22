## time

Linux reads time from the RTC hardware clock if its available. The clock runs indefinitely as long as the battery is giving the power \(even though the system is powered down\). Otherwise it starts the system from JAN 1 2000 00:00 hrs. \(As i saw it from the 2.6.23 kernel\) \(Needs updating\)

The resolution is in seconds from the RTC hardware.

The linux kernel then keeps the read time from the RTC into the software and keeps it ticking till it gets a reboot or shutdown signal or the power down interrupt.

The kernel's time management system provides a clock resolution of nano seconds.

Kernel maintains a software timer called jiffies that is measured in ticks. The jiffies are the number of ticks that have occurred since the system booted.

The system call to get the current time in seconds is `time`. The data type `time_t` is available to store the time.

The below code gets the current time in seconds since 1970 UTC JAN 1.

```c
time_t now;

now = time(0);
```

the `now` variable holds the current time in seconds. The `time_t` is typecasted from `long` type. Thus it is printable as the long type.

```c
printf("the current system time in sec %ld\n", now);
```

The header file to include when using the `time` system call is `time.h`.

The system call to get the current date and time in human readable format is the `gmtime` and friends \(`asctime` and `localtime`\)

The `gmtime` takes a variable of type `time_t` as an address and returns a data structure of type `struct tm`. The time value returned in the form of UTC from 1970 Jan 1.

The `struct tm` contains the below fields

```c
struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};
```

A call to the gmtime involve getting the time from the `time` API.

the field `tm_sec` will have a range of 0 to 59.
the field `tm_min` will have a range of 0 to 59.
the field `tm_hour` will have a range of 0 to 23.
the field `tm_mday` is day of the month will have a range of 0 to 31. and depending on the month (for Feb or for leap year).
the field `tm_mon` will have a range of 0 to 11. 0 being january and 11 being december.
the field `tm_year` will be subtracted by 1900. usually if its 2018, it will be shown as 118.
the field `tm_wday` is day of the week.

the field `tm_isdst` represents if the current time has a DST (Day light savings) in use.

the below example gives an idea on how to use the gmtime function in a more basic way.

```c
struct tm *t;
time_t cur;

cur = time(0); // current time

t = gmtime(&cur);
if (!t) {
    perror("failed to gmtime");
    return -1;
}

printf("t->year %d\n"
       "t->mon %d\n",
       "t->date %d\n"
       "t->hour %d\n"
       "t->min %d\n"
       "t->sec %d\n",

       t->tm_year + 1900, // the year is addded with 1900 to get to current year
       t->tm_mon + 1, // month starts from 0
       t->tm_mday, // day of the month, the Calendar
       t->tm_hour,
       t->tm_min,
       t->tm_sec);
```

Below example provide the `gmtime` API usage. Download [here](https://github.com/DevNaga/gists/blob/master/gmtime.c)

```c
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>

int main()
{
    time_t now;
    struct tm *t;

    now = time(0);

    t = gmtime(&now);
    if (!t) {
        printf("failure to gmtime %s\n", strerror(errno));
        return -1;
    }

    printf("%04d:%02d:%02d-%02d:%02d:%02d\n",
                    t->tm_year + 1900,
                    t->tm_mon + 1,
                    t->tm_mday,
                    t->tm_hour,
                    t->tm_min,
                    t->tm_sec);

    return 0;
}

```

`gmtime` has the structure `struct tm` as static in its code. It is usually not safe to call `gmtime` with threads. A more safer approach is to call the reentrant version of `gmtime`, i.e. `gmtime_r`.

Below example provide the `gmtime_r` API. Download [here](https://github.com/DevNaga/gists/blob/master/gmtime1.c)

```c
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>

int main()
{
    time_t now;
    struct tm t1;
    struct tm *t;

    now = time(0);

    t = gmtime_r(&now, &t1);
    if (!t) {
        printf("failure to gmtime %s\n", strerror(errno));
        return -1;
    }

    printf("%04d:%02d:%02d-%02d:%02d:%02d\n",
                    t->tm_year + 1900,
                    t->tm_mon + 1,
                    t->tm_mday,
                    t->tm_hour,
                    t->tm_min,
                    t->tm_sec);

    return 0;
}

```

`ctime`  is another API that can return time in calendar time printable format (string) according to the time size.

`ctime` prototype is as follows.

```c
char *ctime(const time_t *t);
```

Below is an example of `ctime` API. Download [here](https://github.com/DevNaga/gists/blob/master/ctime.c)

```c
#include <stdio.h>
#include <time.h>

int main()
{
    time_t now = time(NULL);

    printf("current cal time %s\n", ctime(&now));
    return 0;
}

```


`mktime` is an API that converts the time in `struct tm` format into `time_t`. The prototype is as follows.

```c
time_t mktime(struct tm *t);
```


```c
#include <stdio.h>
#include <time.h>

int main(int argc, char **argv)
{
    int year, month, date, hr, min, sec;
    struct tm t;
    time_t result;

    if (argc != 7) {
        printf("%s [year] [month] [date] [hour] [minute] [second]\n", argv[0]);
        return -1;
    }

    year = atoi(argv[1]);
    month = atoi(argv[2]);
    date = atoi(argv[3]);
    hr = atoi(argv[4]);
    min = atoi(argv[5]);
    sec = atoi(argv[6]);

    if (month < 0 || month > 12) {
        printf("out of range month %d\n", month);
        return -1;
    }

    if (date < 0 || date > 31) {
        printf("out of range date %d\n", date);
        return -1;
    }

    if (hr < 0 || hr > 24) {
        printf("out of range hour %d\n", hr);
        return -1;
    }

    if (min < 0 || min > 60) {
        printf("out of range minute %d\n", min);
        return -1;
    }

    if (sec < 0 || sec > 60) {
        printf("out of range second %d\n", sec);
        return -1;
    }

    t.tm_year = year - 1900;
    t.tm_mon = month - 1;
    t.tm_mday = date;
    t.tm_hour = hr;
    t.tm_min = min;
    t.tm_sec = sec;
    t.tm_isdst = -1;

    result = mktime(&t);
    if (result == -1) {
        printf("Failed to get mktime\n");
        return -1;
    }

    printf("res %ld\n", result);

    return 0;
}
```

**Example: mktime**

The `t.tm_isdst` is set to -1 as we do not know the timezone.

A more resolution timeout can be obtained from the `gettimeofday` API. The API looks like below:

```c
int gettimeofday(struct timeval *tv, struct timezone *tz);
```

The second argument is usually passed NULL for getting the time since UTC.

The `gettimeofday` is used to get the microsecond resolution time as well as the timezone. The `gettimeofday` returns 0 on success and -1 on failure.

The value is returned into `struct timeval`. The `struct timeval` is as follows.

```c
struct timeval {
    time_t  tv_sec;
    suseconds_t tv_usec;
};
```

where the `tv_sec` represents the seconds part of the time and `tv_usec` represents the microseconds part of the time.

We simply use the below code to get the current time in seconds and micro seconds resolution.

```c
struct timeval cur_time;
int ret;

ret = gettimeofday(&cur_time, NULL);
if (ret < 0) { // most unlikely the call will fail
    perror("failed to gettimeofday");
    return -1;
}

printf("cur time sec: %ld, usec: %ld\n",
                    cur.tv_sec, cur.tv_usec);
```

The most common use of `gettimeofday` is to put the call above and below a function cal, analyze how much time the function call would take to execute.

An example would look as follows.

```c
void function()
{
    ...
}

void analysis()
{
    long diff;
    struct timeval before, after;

    gettimeofday(&before, 0);
    function();
    gettimeofday(&after, 0);

    diff = (((after.tv_sec * 1000) - (before.tv_sec * 1000)) +
               (after.tv_usec / 1000) - (before.tv_sec / 1000))

    printf("delta %ld\n", diff); 
}

```


The above calls would get the current 'wallclock' time. Meaning they are affected by the changes in the time due to clock drift and adjustments. The most important factors include the GPS setting the time into the system, NTP changing the system time syncing with the NTP servers. This would affect programs depending on these API. For example: the timers using the above API would either expire quickly \(due to time moving forward\) or wait forever \(due to time moving backwards to a larger value\).

The header file `sys/time.h` also provides a macro called `timersub`. The `timersub` accepts two `timeval` structures and produces the delta in the third varible that is also of type `timeval`.

Below is the `timersub` prototype looks like,

```c
timersub(struct timeval *stop, struct timeval *start, struct timeval *delta);

```

with the `timersub` the above diff calculation can be done simply as below, (diff can be replaced with `timersub`)


```c

struct timeval before, after;
struct timeval delta;

...

timersub(&after, &before, &delta);

printf("latency %ld sec %ld usec\n", delta.tv_sec, delta.tv_usec);

```




The `settimeofday` API is used to set the system time. The prototype is as follows..

```c
int settimeofday(const struct timeval *tv, const struct timezone *tz);
```

just like `gettimeofday`, the `settimeofday` `tz` argument can be set to `NULL` by default.

The settimeofday API can fail in the following cases:

`EPERM`:

if the user is not privileged user and tries to call this API.

`ENOSYS`:

If the tz pointer in the call is not null and the os does not supports it.

The following code snippet describes the usage of the `settimeofday` system call.

```c
struct timeval tv;
int ret;

ret = gettimeofday(&tv, 0);
if (ret < 0) {
    perror("failed to gettimeofday");
    return -1;
}

// set one sec in future
tv.tv_sec += 1;
ret = settimeofday(&tv, 0);
if (ret < 0) {
    perror("failed to settimeofday");
    return -1;
}

```

The problem with the `settimeofday` is that the time can go abruptly forward or abruptly backwards. This might affect some programs as we have discussed above that programs using wallclock time might misbehave with the abrupt change of time. To avoid this process we need to use the `adjtime` API which is described as follows.

The `adjtime` looks as follows

```c
int adjtime(const struct timeval *delta, struct timeval *olddelta);
```

The `adjtime` API speeds up or slows down the time in monotonically. If the `delta` argument is positive, then the system time is speeded up till the `delta` value and if the `delta` argument is negative, then the system time is slowed down till the `delta` value.

The below code sample shows the usage of `adjtime`.

```c
int ret;
struct timeval delta;

delta.tv_sec = 1;
delta.tv_usec = 0;

ret = adjtime(&delta, NULL);
if (ret < 0) {
    perror("failed to adjtime");
    return -1;
}

```

When we are programming timers, we should avoid any calls to the above API as they are not monotonic or steadily moving forward in the future.

The `time.h` provides a macro called `difftime` that is used to find the difference of time between two variables of type `time_t` (Although one can subtract the two from each other on linux system).

The `difftime` looks as follows.

```c
double difftime(time_t time0, time_t time1);
```

`clock` is another API that measures the CPU time perfectly. As we looked at one of the usage of `gettimeofday` call, we provided an example of using the `gettimeofday` to measure the time it takes to execute the function. However, this incurs the scheduling and other jobs with in the system and is not the effective way to find out only the CPU time. `clock` function provides us to do just this.

The `clock` looks as follows.

```c
clock_t clock();
```

The `clock` returns the number of ticks. To convert it, divide it by `CLOCKS_PER_SEC`. If the processor time used is not represented, it returns -1. The `clock` return value can wrap around every 72 minutes. On a 32 bit system the `CLOCKS_PER_SEC` is 1,000,000. 

The sample code is as below.

```c
clock_t start;
clock_t end;

start = clock();
func_call();
end = clock();

printf("ticks %d\n", end - start);

```

There is another API that is used to get the CPU times, called `times`.

The prototype is as follows

```c
clock_t times(struct tms *buf);
```

The `times` API stores the information into the `struct tms`. The structure looks as below.

```c
struct tms {
    clock_t tms_utime; // user time
    clock_t tms_stime; // system time
    clock_t tms_cutime; // user time of children
    clock_t tms_cstime; // system time of children
};
```

`tms_utime` is the amount of time spent in executing the instructions in user space.
`tms_stime` is the amount of time spent in executing the instructions in system.
`tms_cutime` is the amount of time spent by the children executing the instructions in user space.
`tms_cstime` is the amount of time spent by the children executing the instructions in system.

All the above times are units of clock ticks.


### Timer APIs

Linux supports the following timer API.

```c
1. setitimer
2. timer_create
3. timerfd_create
```

The command `hwclock` is very useful to get or set time to the system RTC hardware clock.

`hwclock` is also used to correct time drifts with the UTC. A periodic \(deterministic timeout\) set would allow the system to be in sync with the UTC time.

`hwclock` command has the following functions:

```
-r, --show

Read the hardware clock and print the time on standard output. 

--hctosys

set the system time from the hardware clock.

--systohc

Set the system time to the hardware clock.
```

