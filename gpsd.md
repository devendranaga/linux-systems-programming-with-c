# GPSd

## Introduction

The [GPSd](http://www.catb.org/gpsd/) is a service that communicates with the GPS hardware and gets us the GPS fix and position.

Many GPS devices provide the data over a serial connection. The GPS device reports the data over the serial interface once per second. The once per second can also be configured once per some millseconds for say 100 milliseconds. The configuration may involve setting the serial port baud rate and using special firmware commands asking the device to perform a speed change of the data.

The GPSd software then reads and parses the information coming over the serial connection and finds out if the satellites gives us a fix with current time, latitude, longitude, elevation, speed and heading. There are some more parameters that GPSd gives us such as Dilution of precision and noise statistics. However, most of the GPSes won't give us everything. The GPSes may provide the information to the userspace via their own protocol or via the NMEA protocol. NMEA protocol is a standard protocol implemented by almost all GPS receivers and understood by almost all software modules. The most prominent messages ni the NMEA protocol are GPGGA, GPGSA, GPGST, GPRMC and GPGSV. These are also called as NMEA strings. More about the NMEA is [here](http://aprs.gids.nl/nmea/)


Most of the GPSes lose the fix intermittently when the device comes across the trees and building complexes. At this point of time a special algorithm such as position estimation may be employed (in rare GPSes it is implemented) and gives out the estimated GPS value.

## GPSd interfacing

The GPSd is a service daemon that listens on the TCP socket for clients and opens a serial connection with (almost all) GPS devices.

The GPSd provides the data to the clients in a JSON formatted strings. The strings are then decoded at the client apps using the GPSd client library API.

API list:

The following are the most important API that are used to interface with the GPSd and get the GPS information

Here is the [link](http://www.catb.org/gpsd/libgps.html) to the GPSd API. And part of the API description is taken from the link.

The include file for the client library is `gps.h` and the linker flags are `lgps` and `lm`.

| API | description |
| --| -- |
| gps_open | create a socket to the GPS daemon |
| gps_read | read from the GPS daemon |
| gps_stream | stream the report to the user |
| gps_unpack | parse JSON from a specified buffer into a session structure|
| gps_close | close the socket connection with the GPS daemon |


The `gps_open` prototype is as follows.

```c
int gps_open(char *server, int port, struct gps_data_t *gps_data);
```

For most of the time, the `server` and `port` arguments are set to 0. The `gps_data` structure is initialised and returned 0 on success. -1 is returned on failure.

The GPSd client library provides an API to automatically stream the fix back to the application. This is done using the `gps_stream` API.

The `gps_stream` prototype is as follows.

```c
int gps_stream(struct gps_data *gps_data, unsigned int flags, void *data);
```

the `flags` arguments allows the method or / and type of streaming.

One of the most commonly used `flags` are `WATCH_ENABLE`.

The `gps_read` prototype is as follows.

```c
int gps_read(struct gps_data_t *gps_data);
```

The `gps_read` API polls the GPS daemon for the data. Thus the polling frequency also does matter on your GPS fix quality or difference between two GPS fixes. The good quality of the GPS fix is when your poll rate becomes same as the update rate on the serial device (GPS hardware communication channel).

The GPSd provides another API to selectively wait for the application to wait for GPS data.

```c
bool gps_waiting(struct gps_data_t *gps_data, int timeout);
```

The `gps_waiting` waits for the GPS data from the GPS socket for `timeout` milliseconds. If the data comes with in the `timeout` the function returns true and otherwise on a timeout, it returns false. Upon the `gps_waiting` returning `true` the `gps_read` API is called to get the GPS data.

The `gps_close` prototype is as follows.

```c
void gps_close(struct gps_data_t *gps_data);
```

The `gps_close` closes or ends the session.


One example of the GPS API is follows.

```c
int main(int argc, char **argv)
{
    struct gps_data_t gps_data;
    int ret;

    ret = gps_open(NULL, 0, &gps_data);
    if (ret < 0) {
        fprintf(stderr, "failed to open connection to GPSd\n");
        return -1;
    }

    gps_stream(&gps_data, WATCH_ENABLE, NULL);

    while (1) {
        if (gps_waiting(&gps_data, 500)) {
            if (gps_read(&gps_data) == -1) {
                fprintf(stderr, "failed to read GPS data\n");
                return -1;
            }
            if ((gps_data.status == STATUS_FIX) && (gps_data.mode >= 2)) {
                fprintf(stderr, "GPS mode : %d\n", gps_data.mode);
                if (!isnan(gps_data.latitude)) {
                    fprintf(stderr, "GPS latitude : %f\n", gps_data.latitude);
                }

                if (!isnan(gps_data.longitude)) {
                   fprintf(stderr, "GPS longitude : %f\n", gps_data.longitude);
                }
            }
        }
    }

    gps_stream(&gps_data, WATCH_DISABLE, NULL);
    gps_close(&gps_data);
}
```

(Remember to use our `select` interface's timeout option is also an alternative to the `gps_waiting` in here that when the timer expire periodically and calling the `gps_read`)
