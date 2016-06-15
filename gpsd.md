# GPSd

## Introduction

The GPSd is a service that communicates with the GPS hardware and gets us the GPS fix and position.

Many GPS devices provide the data over a serial connection. The GPS device reports the data over the serial interface once per second. The once per second can also be configured once per some millseconds for say 100 milliseconds. The configuration may involve setting the serial port baud rate and using special firmware commands asking the device to perform a speed change of the data.

The GPSd software then reads and parses the information coming over the serial connection and finds out if the satellites gives us a fix with current time, latitude, longitude, elevation, speed and heading. There are some more parameters that GPSd gives us such as Dilution of precision and noise statistics. However, most of the GPSes won't give us everything.


Most of the GPSes lose the fix intermittently when the device comes across the trees and building complexes. At this point of time a special algorithm such as position estimation may be employed (in rare GPSes it is implemented) and gives out the estimated GPS value.

## GPSd interfacing

The GPSd provides the data to the clients in a JSON formatted strings. The strings are then decoded at the client apps using the GPSd client library API.

API list:


The following are the most important API that are used to interface with the GPSd and get the GPS information



| API | description |
| --| -- |
| gps_open | create a socket to the GPS daemon |
| gps_read | read from the GPS daemon |
| gps_unpack | parse JSON from a specified buffer into a session structure|
| gps_close | close the socket connection with the GPS daemon |


The `gps_read` API polls the GPS daemon for the data. Thus the polling frequency also does matter on your GPS fix quality or difference between two GPS fixes. The good quality of the GPS fix is when your poll rate becomes same as the update rate on the serial device (GPS hardware communication channel).

(Remember to use our `select` interface's timeout option in here that when the timer expire periodically and calling the `gps_read`)