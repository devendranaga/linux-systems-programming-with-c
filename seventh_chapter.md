# Seventh chapter

## POSIX Threads

Thread is light weight process.

*  Has the same address as the main thread (the thread that has created it).
*  Do not have to assign any stack. It will be allocated automatically.
*  Main thread can either wait or can create threads that will be wait by the OS. The OS will directly perform the cleanup
*  Thread immediately starts executing after it has been created. The execution depends on the scheduler. The thread will exit as soon as the main thread exits.
*  Threads can be created and stopped at run time.
*  Creation might involve some overhead. So the threads can be created at the initial stage, and can be woken up in-between or when needed. These are also called worker threads.
*  The `mutex`, `conditional variable` are some of the synchronisation functions to avoid corruption and parallel access to the same address space.
*  Standard GNU library provides POSIX thread implementation library. In linux there is no big difference between threads and processes as the overhead at creation and maintenance is mostly the same.
*  The threads will not appear in the `ps` command output. but `ps -eLf` would give all threads and processes running in the system.
