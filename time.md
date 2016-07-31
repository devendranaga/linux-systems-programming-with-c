# time

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
printf("the current
system time in sec %ld\n", now);
```

The header file to include when using the `time` system call is `time.h`.

The system call to get the current date and time in human readable format is the `gmtime` and friends \(`asctime` and `localtime`\)

The `gmtime` takes a variable of type `time_t` as an address and returns a data structure of type `struct tm`.

The `struct tm` contains the below fields

```
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

the below example gives an idea on how to use the gmtime function in a more basic way.

```
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

```
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

The `settimeofday` API is used to set the system time. The prototype is as follows..

```c
int settimeofday(const struct timeval *tv, const struct timezone *tz);
```

The settimeofday API can fail in the following cases:

`EPERM`:

if the user is not privileged user and tries to call this API.

`ENOSYS`:

If the tz pointer in the call is not null and the os does not supports it.

The above calls would get the current 'wallclock' time. Meaning they are affected by the changes in the time due to clock drift and adjustments. The most important factors include the GPS setting the time into the system, NTP changing the system time syncing with the NTP servers. This would affect programs depending on these API. For example: the timers using the above API would either expire quickly \(due to time moving forward\) or wait forever \(due to time moving backwards to a larger value\).


The following code snippet describes the usage of the `settimeofday` system call.

```c
struct timeval tv;
int ret;

ret = gettimeofday(&tv, 0);
if (ret < 0) {
    perror("failed to gettimeofday");
    return -1;
}

tv.tv_sec += 1;
ret = settimeofday(&tv, 0);
if (ret < 0) {
    perror("failed to settimeofday");
    return -1;
}
```

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


When we are programming timers, we should avoid any calls to the above API as they are not monotonic or steadily moving forward in the future.

### Timer APIs

Linux supports the following timer API.

```c
1. setitimer
2. timer_create
3. timerfd_create
```

The command `hwclock` is very useful to get or set time to the system.

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

