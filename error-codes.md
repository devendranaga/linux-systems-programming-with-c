# Error codes

Linux system calls returns errors in the form of codes stored in errno variable. This variable is referenced using the errno.h.



Here are some of the descriptions of the error codes. As usual, we use the `perror` or `strerror` to describe the error in a string format.

| error code | description |
| --- | --- |
| EPERM | Permission denied. Special privileges are needed. |
| ENOENT | The file does not exist. |
| EIO | Input \/ Output error occured. |
| EINTR | Interrupted system call. Signal occured before the system call was allowed to finish. |
| ENOEXEC | Invalid executable file format. This is detected by the exec family of functions. |
| ENOMEM | No memory is available. Out of memory situation can get this error. |
| EBUSY | System resource that can't be shared and already in use. |
| ENOTDIR | Not a directory. File is given as an argument instead of the directory. |
| ENODEV | Given device is not found under \/dev\/ etc. |
| EEXIST | File exists. A file tried to open in write mode and is already there. |
| EFAULT | Segmentation fault. Invalid pointer access is detected. |
| EACCESS | Permission denied. The file permissions on the file does not allow the user to access. |
| EMFILE | Too many files are opened and can't open anymore. |
| EISDIR | File is a directory. |
| EFBIG | File too big. |
| EADDRINUSE | socket address is already in use. |
| ENETDOWN | a socket operation failed because the network was down. |
| ENETUNREACH | a socket operation failed because the network is not reachable. |
| EADDRNOTAVAIL | socket address that is requested is not available. |
| EOPNOTSUPP | operation not supported. mostly happens with server and client socket type and family mismatches. |
| EINPROGRESS | Connect syscall is in in progress to connect with a server. |
| EAGAIN | Resource temporarily blocked. Try again the call may succeed. |
| EPIPE | Broken pipe. to ignore it, signal\(SIGPIPE, SIG\_IGN\). Usually a broken pipe means that the other end of the connnection is closed. |
| EMLINK | too many links |
| ESPIPE | Invalid seek operation. |
| ENOTTY | Inappropriate ioctl operation for the tty. |
| EBADF | File descriptor is bad. The file descriptor is used on the read, write, select, epoll calls is invalid. |
|  |  |



