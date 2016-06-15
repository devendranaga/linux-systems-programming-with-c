# time

Linux reads time from the RTC hardware clock if its available. The clock runs indefinitely as long as the battery is giving the power (even though the system is powered down). Otherwise it starts the system from JAN 1 2000 00:00 hrs. (As i saw it from the 2.6.23 kernel) (Needs updating)

The resolution is in seconds from the RTC hardware.

The kernel's time management system provides a clock resolution of nano seconds.

Kernel maintains a software timer called jiffies that is measured in ticks. The jiffies are the number of ticks that have occurred since the system booted.

The system call to get the current time in seconds is `time`. The data type `time_t` is available to store the time.

The below code gets the current time in seconds since 1970 UTC JAN 1.

        time_t now;
        
        now = time(0);

the `now` variable holds the current time in seconds. The `time_t` is typecasted from `long` type. Thus it is printable as the long type.

        printf("the current system time in sec %ld\n", now);

The header file to include when using the `time` system call is `time.h`.

The system call to get the current date and time in human readable format is the `gmtime` and friends (`asctime` and `localtime`)

The `gmtime` takes a variable of type `time_t` as an address and returns a data structure of type `struct tm`.

The `struct tm` contains the below fields

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

A call to the gmtime involve getting the time from the `time` API.

the below example gives an idea on how to use the gmtime function in a more basic way.

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
           
A more resolution timeout can be obtained from the `gettimeofday` API. The API looks like below:

    int gettimeofday(struct timeval *tv, struct timezone *tz);
    
The second argument is usually passed NULL for getting the time since UTC.

We simply use the below code to get the curren time in seconds and micro seconds resolution.

    struct timeval cur_time;
    int ret;
    
    ret = gettimeofday(&cur_time, NULL);
    if (ret < 0) { // most unlikely the call will fail
        perror("failed to gettimeofday");
        return -1;
    }
    
    printf("cur time sec: %ld, usec: %ld\n",
                        cur.tv_sec, cur.tv_usec);


The most common use of `gettimeofday` is to perform microsecond sleep calls with the usleep, analyze how much time the function call would take to execute.

The above calls would get the current 'wallclock' tme. Meaning they are affected by the changes in the time due to clock drift and adjustments. The most important factors include the GPS setting the time into the system, NTP changing the system time syncing with the NTP servers. This would affect programs depending on these API. For example: the timers using the above API would either expire quickly (due to time moving forward) or wait forever (due to time moving backwards to a larger value).

When we are programming timers, we should avoid any calls to the above API as they are not monotonic or steadily moving forward in the future.

The command `hwclock` is very useful to get or set time to the system.

`hwclock` is also used to correct time drifts with the UTC. A periodic (deterministic timeout) set would allow the system to be in sync with the UTC time.

`hwclock` command has the following functions:

    -r, --show
    
    Read the hardware clock and print the time on standard output. 
    
    --hctosys
    
    set the system time from the hardware clock.
    
    --systohc
    
    Set the system time to the hardware clock.