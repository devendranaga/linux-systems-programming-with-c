## posix threads programming

Thread is light weight process. Here are some of the features of threads.

*  Has the same address as the main thread (the thread that has created it).
*  Do not have to assign any stack. It will be allocated automatically.
*  Main thread can either wait or can create threads that will be wait by the OS. The OS will directly perform the cleanup
*  Thread immediately starts executing after it has been created. The execution depends on the scheduler. The thread will exit as soon as the main thread exits.
*  Threads can be created and stopped at run time.
*  Creation might involve some overhead. So the threads can be created at the initial stage, and can be woken up in-between or when needed. These are also called worker threads.
*  The `mutex`, `conditional variable` are some of the synchronisation functions to avoid corruption and parallel access to the same address space.
*  Standard GNU library provides POSIX thread implementation library. In linux there is no big difference between threads and processes as the overhead at creation and maintenance is mostly the same.
*  The threads will not appear in the `ps` command output. but `ps -eLf` would give all threads and processes running in the system.

In the below sections, the topic of discussion is on posix threads, specially on pthreads programming and handling.

For compilation , include `pthread.h` and pass `-pthread` to the linker flags.

### creating threads

similar to processes, threads have IDs. Thread have the id of the form `pthread_t`. Threads are created using the `pthread_create` API... Its prototype is desscribed below,


```c
int pthread_create(pthread_t *tid, pthread_attr_t *attr, void *(*thread_func)(void *), void *data);
```

the above API accepts a thread id of the form `pthread_t` and attribute of the form `pthread_attr_t` and a thread function that is executed when the thread is created as well the data that thread function going to receive.

usually, most implementations call the `pthread_create` the following way,


```c
int ret;
pthread_t tid;
int data;

ret = pthread_create(&tid, NULL, thread_func, &data);
if (ret < 0) {
    fprintf(stderr, "failed to create thread\n");
    return -1;
}
```

Thread creation might fail, because maximum number of processes on the system is exceeded the limit set. (see `sysconf` for max processes allowed)

the thread created will be automatically started up and starts running the thread function. So care must be taken to protect the common memory areas the thread and the main thread or others are accessing. This means the use of locks. pthread library provides mutexes and conditional variables for locking and job scheduling or waking up of threads. More about the locks and condition variables in below sections.

Below code explains a bit more about the pthread creation and running.

Download  [here](https://github.com/DevNaga/gists/blob/master/pthread_create.c)

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *thread_func(void *thread_ctx)
{
    while (1) {
        sleep(1);
        printf("thread func..\n");
    }
}

int main()
{
    int ret;
    pthread_t tid;

    ret = pthread_create(&tid, NULL, thread_func, NULL);
    if (ret < 0) {
        fprintf(stderr, "failed to create thread\n");
        return -1;
    }

    pthread_join(tid, NULL);

    return 0;
}

```

above example creates a thread called `thread_func` and starts executing it. The `pthread_join` is called to wait for the thread to complete its execution. More about the `pthread_join` in below sections. The created thread starts running the function and the function has an infinite loop and at every second it wakes up and prints a text "thread func..." on the screen.

Most threads that are created are not meant to die for short periods, but are used for executing a set of work items that main thread cannot handle. So main thread provides the work to the threads and they execute the work for the main thread and notify the main thread of the work completion, so that the main thread can push another job for the thread to execute.

Linux threads have the same overhead in creation as that of the linux processes.


### joining threads and thread attributes

Threads are created by default in attached mode, meaning they are joinable and must be joined by using `pthread_join`.

`pthread_join` prototype is as below.

```c
int pthread_join(pthread_t tid, void **retval)
```

the `pthread_join` accepts the thread id and then the `retval` is the value output when the thread returns and stops execution. This is catched in the `retval` argument of the `pthread_join` call.

Threads must be joined when they are not created in detach state. The main thread has to wait for the threads to complete their execution. The default thread created is joinable. This can be found via the `pthread_attr_getdetachstate` API.

The prototype is as follows,

```c
int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detach_state);
```

The thread can be created in detach state with the use of `pthread_attr_setdetachstate`.

The prototype is as follows.

```c
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detach_state)
```

where the `detach_state` is one of `PTHREAD_CREATE_DETACHED`, `PTHREAD_CREATE_JOINABLE`.

Before calling the APIs, one must initialise the thread attributes before creating a thread by using `pthread_create`. 

This can be done using the `pthread_attr_init` API.

The prototype is as follows.

```c
int pthread_attr_init(pthread_attr_t *attr);
```

Below is one of the example of the `pthread_attr_setdetachstate` and `pthread_attr_getdetachstate` functions.. Download [here](https://github.com/DevNaga/gists/blob/master/pthread_det.cpp)


```c
#include <iostream>
#include <chrono>
#include <unistd.h>
#include <pthread.h>

void *thread_func(void *thread_contx)
{
    while (1) {
        sleep(1);
        std::cout <<" thread " << pthread_self() << std::endl;
    }
}

int main()
{
    pthread_attr_t attr;
    pthread_t thread_id;
    int ret;

    ret = pthread_attr_init(&attr);
    if (ret < 0) {
        return -1;
    }

    ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (ret < 0) {
        return -1;
    }

    ret = pthread_create(&thread_id, &attr, thread_func, &thread_id);
    if (ret < 0) {
        return -1;
    }

    int detach_state = 0;

    ret = pthread_attr_getdetachstate(&attr, &detach_state);
    if (ret < 0) {
        return -1;
    }

    if (detach_state == PTHREAD_CREATE_DETACHED) {
        std::cout  << "thread created as detached..\n" ;
    } else if (detach_state == PTHREAD_CREATE_JOINABLE) {
        std::cout << "thread created as joniable\n";
    }

    if (detach_state == PTHREAD_CREATE_JOINABLE) {
        pthread_join(thread_id, NULL);
    } else {
        while (1) {
            sleep(1);
        }
    }

    return 0;
}

```

The `pthread_attr_init` is called before creating any thread with the `pthread_create`. This is further called with the `pthread_attr_setdetachstate` with thread in detachmode, aka using the `PTHREAD_CREATE_DETACHED`. since the thread is in detached state, it cannot be joined with `pthread_join`. Once the thread is created, the attribute is again checked using the `pthread_attr_getdetachstate` and the detach state is checked if its in `JOINABLE` condition, a `pthread_join` is then called upon, otherwise, the program spins in a sleep loop forever.

Try replacing the `PTHREAD_CREATE_DETACHED` with `PTHREAD_CREATE_JOINABLE` in the call to `pthread_attr_setdetachstate` and see what happens to the joinable section of the code below.

### locking

**mutexes**

mutexes are varibles that is used for locking more than one thread of execution. Pthreads provide the mutexes for locking purposes.

A mutex needs to be initialised before it is being used in the program. pthread library defines the mutex as `pthread_mutex_t`.

To declare a mutex variable,


```c
pthread_mutex_t mutex;
```

mutex needs to be initialised before it is used to lock any particular section of the data. The `pthread_mutex_init` is used to initialise the mutex. Its prototype is as follows,

```c
int pthread_mutex_init(pthread_mutex_t *mutex, pthread_mutex_attr_t *attr);
```

usually, the following code is generally used to initilaise a mutex.

```c
pthread_mutex_t mutex;
int ret;

ret = pthread_mutex_init(&mutex, NULL);
if (ret < 0) {
    return -1;
}
```

Once the mutex is initialised, it can then be used to lock and unlock portion of the program. The `pthread_mutex_lock` and `pthread_mutex_unlock` are used to lock and unlock particular section of the program respectively.

The `pthread_mutex_lock` prototype is as follows.

```c
int pthread_mutex_lock(pthread_mutex_t *lock);
```

The `pthread_mutex_unlock` prototype is as follows.

```c
int pthread_mutex_unlock(pthread_mutex_t *lock);
```

Below example provide a locking demo. Download [here](https://github.com/DevNaga/gists/blob/master/pthread_mutex.c)

```c
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t lock;

void *thread_f(void *d)
{
    int *ptr = d;

    while (1) {
        sleep(1);

        pthread_mutex_lock(&lock);

        (*ptr) ++;

        pthread_mutex_unlock(&lock);
    }
}


int main()
{
    int t = 4;
    pthread_t tid;
    pthread_attr_t attr;
    int ret;

    ret = pthread_attr_init(&attr);
    if (ret < 0) {
        return -1;
    }

    ret = pthread_mutex_init(&lock, NULL);
    if (ret < 0) {
        return -1;
    }

    ret = pthread_create(&tid, &attr, thread_f, &t);
    if (ret < 0) {
        return -1;
    }

    while (1) {

        pthread_mutex_lock(&lock);

        printf("t value %d\n", t);

        pthread_mutex_unlock(&lock);
        sleep(1);
    }

    return 0;
};

```

**condition variables**

condition variables are synchronisation primitives that wait till a particular condition occurs. condition variables are mostly used in threads to wait for a particular event and sleep. No load occurs when the condition variable sleeps. So that the other thread can signal the condition variable to wake up the thread that is waiting on this condition variable. The pthread provides some of the below condition varilables.

1. `pthread_cond_init`.
2. `pthread_cond_wait`.
3. `pthread_cond_signal`.
4. `pthread_cond_broadcast`.
5. `pthread_cond_destroy`.

the `pthread_cond_t` is used to declare a condition variable. It is declared as the following,

```c
pthread_cond_t cond;

```

the `pthread_cond_init` initialises the condition variable.

### thread pools

Creating threads at runtime is a bit costly job and a create-delete sequence is an overhead if done frequent. 

In general, the threads are created as a pool of workers waiting for the work to be executed.

Below is an example of the basic thread pooling. Download [here](https://github.com/DevNaga/gists/blob/master/pools.c)

```c
#include <stdio.h>
#include <pthread.h>

void *thread_func(void *data)
{
    int *i = data;

    while (1) {
        printf("----------------------------\n");
        sleep(1);
        printf("tid-> [%lu] i %d\n", pthread_self(), *i);

        (*i) ++;
        printf("----------------------------\n");
    }

}

int main()
{
    int array[8];
    int i;
    pthread_t tid[8];
    int ret;

    for (i = 0; i < 8; i ++) {
        array[i] = 0;
        ret = pthread_create(&tid[i], NULL, thread_func, &array[i]);
        if (ret < 0) {
            printf("failed to create thread\n");
            return -1;
        }
    }

    for (i = 0; i < 8; i ++) {
        pthread_join(tid[i], NULL);
    }

    return 0;
}

```

more detailed thread pool mechanism is shown in the example below. Download [here](https://github.com/DevNaga/gists/blob/master/pool_lib.c)

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

struct pthread_object {
    pthread_t tid;
    int work_count;
    void *thread_data;
    void (*work)(void *thread_data);
    pthread_mutex_t lock;
    pthread_cond_t cond;
};

struct pthread_pool_ctx {
    int n_threads;
    struct pthread_object *n_obj;
};

void *thread_worker(void *thread_data)
{
    struct pthread_object *ctx = thread_data;

    while (1) {
        pthread_mutex_lock(&ctx->lock);

        pthread_cond_wait(&ctx->cond, &ctx->lock);

        ctx->work(ctx->thread_data);

        //ctx->work_count --;

        pthread_mutex_unlock(&ctx->lock);
    }
}

void * pthread_pool_create(int n_workers)
{
    struct pthread_pool_ctx *ctx;
    int i;
    int ret;

    ctx = calloc(1, sizeof(struct pthread_pool_ctx));
    if (!ctx) {
        return NULL;
    }

    ctx->n_threads = n_workers;

    ctx->n_obj = calloc(n_workers, sizeof(struct pthread_object));
    if (!ctx->n_obj) {
        return NULL;
    }


    for (i = 0; i < n_workers; i ++) {
        ctx->n_obj[i].work_count = 0;
        ret = pthread_create(&ctx->n_obj[i].tid, NULL, thread_worker, &ctx->n_obj[i]);
        if (ret < 0) {
            return NULL;
        }
    }

    return ctx;
}

void pthread_schedule_work(void *priv, void (*work)(void *thread_data), void *work_data)
{
    struct pthread_pool_ctx *ctx = priv;
    static int min = -1;
    int idx = 0;
    int i;

    min = ctx->n_obj[0].work_count;
    for (i = 1; i < ctx->n_threads; i ++) {
        if (ctx->n_obj[i].work_count < min) {
            min = ctx->n_obj[i].work_count;
            idx = i;
        }
    }

    if (idx == -1) {
        return;
    }

    pthread_mutex_lock(&ctx->n_obj[idx].lock);

    ctx->n_obj[idx].work = work;
    ctx->n_obj[idx].thread_data = work_data;
    ctx->n_obj[idx].work_count ++;

    printf("out worker loads  ==== ");

    for (i = 0; i < ctx->n_threads; i ++) {
        printf("| %d ", ctx->n_obj[i].work_count);
    }
    printf(" |\n");

    pthread_cond_signal(&ctx->n_obj[idx].cond);

    pthread_mutex_unlock(&ctx->n_obj[idx].lock);
}

void work_func(void *priv)
{
    int *i = priv;

    printf("value at i %d\n", *i);

    (*i) ++;

}

int main()
{
    void *priv;

    priv = pthread_pool_create(8);
    if (!priv) {
        return -1;
    }

    int work_data = 0;

    while (1) {
        usleep(200 * 1000);

        pthread_schedule_work(priv, work_func, &work_data);
    }


    return 0;
}

```

The above example defines 2 API for the thread pool.

```c
1. pthread_pool_create
2. pthread_schedule_work
```

`pthread_pool_create` creates a pool of threads and assigns the work objects for each of the threads and the corresponding locking. The objects are of type `pthread_object`. 

Each thread that has started executes the same worker function, the worker function must be async safe and re-entrant to be able to execute concurrently more than one thread. Each thread waits on the condition variable to see if there is any work pending for the thread.

Main thread signals the threads upon a work is available, and the threads wake up running the job. 

The wake up is done via `pthread_schedule_work`. This function checks weights for each thread, considering what amount of work the thread been doing and if its complete, using the `work_count` variable. The function then runs a round robin scheduler, selecting the next thread that has less work than all the threads. The `work_count` is incremented on this thread and the job is given.

Worker loads are continuously printed on the screen when the `pthread_schedule_work` is called.

the main program, initialises the pool by calling `pthread_pool_create` and then schedules the work with `pthread_schedule_work` as and when needed. In the demonstration above, the main thread simply calls the `pthread_schedule_work` at every 200 msec.


The tcp server and client programs under socket section can use threads to exploit the multithreaded client server communication. Below is an example that describe the multi thread server. Download [here]()

```c

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>

struct thread_data {
    int sock;
};

void *thread_handle(void *data)
{
    struct thread_data *thread_data = data;
    int ret;

    while (1) {
        char rxbuf[1024];

        ret = recv(thread_data->sock, rxbuf, sizeof(rxbuf), 0);
        if (ret <= 0) {
            fprintf(stderr, "failed to recv %s\n", strerror(errno));
            break;
        }

        printf("data %s from client %d\n", (char *)rxbuf, thread_data->sock);
    }
}

int main(int argc, char **argv)
{
    struct sockaddr_in serv;
    int sock;
    int csock;
    struct thread_data data;
    pthread_t tid;
    int ret;

    if (argc != 3) {
        fprintf(stderr, "<%s> <ip> <port>\n", argv[0]);
        return -1;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        fprintf(stderr, "failed to socket open %s\n", strerror(errno));
        return -1;
    }

    memset(&serv, 0, sizeof(serv));

    serv.sin_addr.s_addr = inet_addr(argv[1]);
    serv.sin_port = htons(atoi(argv[2]));
    serv.sin_family = AF_INET;

    ret = bind(sock, (struct sockaddr *)&serv, sizeof(serv));
    if (ret < 0) {
        fprintf(stderr, "failed to bind %s\n", strerror(errno));
        return -1;
    }

    ret = listen(sock, 4);
    if (ret < 0) {
        fprintf(stderr, "failed to listen %s\n", strerror(errno));
        return -1;
    }

    while (1) {
        csock = accept(sock, NULL, NULL);
        if (csock < 0) {
        fprintf(stderr, "failed to accept %s\n", strerror(errno));
            return -1;
        }

        struct thread_data *thr;

        thr = calloc(1, sizeof(struct thread_data));
        if (!thr) {
            fprintf(stderr, "failed to allocate %s\n", strerror(errno));
            return -1;
        }

        thr->sock = csock;

        ret = pthread_create(&tid, NULL, thread_handle, thr);
        if (ret < 0) {
            fprintf(stderr, "failed to pthrad_create %s\n", strerror(errno));
            return -1;
        }
    }

    return 0;
}

```

In the above program a server socket is created and waiting for the connections, while it waits for the connections in the infinite while loop, the connection may arrive if the client connects to the server at the given port and ip address.

As and when the connection is arrived, the server creates a thread context structure and sets the socket address to the client socket.

This is then provided as a data pointer to the `pthread_create` call. Once the thread function `thread_handle` is called, it is then waits on client descriptor forever and reads from the clients.

In the above program, the main thread acts as a controller and waiting for the client connections and initiates a thread as soon as it sees a new client. The started thread then serves the connection by waiting on the `recv` system call.

Another example below is the client. Download [here]()

```c

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
    int cli_sock;
    int ret;

    if (argc != 3) {
        fprintf(stderr, "<%s> <ip> <port>\n", argv[0]);
        return -1;
    }

    struct sockaddr_in serv_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr(argv[1]),
        .sin_port = htons(atoi(argv[2])),
    };

    cli_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (cli_sock < 0) {
        return -1;
    }

    ret = connect(cli_sock, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));
    if (ret < 0) {
        return -1;
    }

    while (1) {
        char msg[] = "sending data to the server";

        send(cli_sock, msg, strlen(msg) + 1, 0);

        sleep(1);
    }

    close(cli_sock);

    return 0;
}

```


Notes:

1. **C++ language does provide some abstraction in the STL via the posix or based on the native thread API employed by the operating system used. It is `std::thread`.**

