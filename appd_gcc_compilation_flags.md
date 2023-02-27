# Appendix. D Gcc compilation flags

The following options are useful while using the `gcc`.


| option | brief description |
|--------|-------------------|
| `-Wall` | enable all warnings |
| `-Werror` | enable all errors |
| `-Os` | enable optimisation |
| `-Wformat` | enable format scanning on `printf`, `scanf` etc.. |
| `-pipe` | write to pipe instaed of file for faster compilation |
| `-Wimplicit-function-declaration` | warn if function is not declared |
| `-Wshadow` | enable detection of shadow variables |
| `_FORTIFY_SOURCE | enable security options and insert memory fences on final binary |
| `-I` | include files path |
| `-g` | enable debug symbols |
| `-ggdb` | enable gdb specific debug symbols |
| `-L` | library files path |
| `-l` | link with the library |
| `-mtune=` | specify the target cpu architecture |
| `-fstack-usage` | generate stack usage for each function that is in the source file |
| `-Wstack-usage` | generate a warning if the stack usage exceeds some given bytes as argument to `-Wstack-usage` |
| `-fPIC` | enable position independent object file compilation |
| `-fPIE` | enable position independent exeutable file compilation |

some of the above options are detailed below..

1. `Wall` to get all sorts of warnings related to the C code. This will also perform a code review (basic style) and catches bugs that occur at beginning or intermediate stages of the testing process. the code review is done mostly the static code review. This catches undefined varibles, unused variables and places where variables are used without initialisation or assignment. When compiled with `-Werror` it forces the programmer to fix all the warnings. However some or more of the options may not be really needed fixing. some things such as `unused fuction arguments` can be disabled with `-Wno-unused-parameter` option.

2. `Werror` forces the programmer to fix the warnings generated.

3. the option `-mtune=` is used to specify the target CPU architecture. In some cases, it is simply used as `-mtune=native`.

4. `-On` is an optimisation flag that is useful in the code optimisation at the compilation stage. it goes with 1, 2, 3 or s. where each number specify the optimisation level. if given s, it means that the compiler can choose what optimisation can be applied. This is better than programmer choosing what optimisation better for the program. Sometimes, the optimisation level higher does not mean the generated code be high performance. So leaving this option to the compiler is better.

At each optimisation levels, gcc tries to optimize the program for more efficiency. for example, if the loops are present in the program, gcc figures if the value of the final count in a `for` loop can be predicted. Such as the following.

```c

uint8_t mac[] = {0x00, 0x01, 0x01, 0x02, 0x02, 0x02};
int i;

for (i = 0; i < sizeof(mac); i ++) {
	printf("%x:", mac[i]);
}
printf("\n");

```

gcc sees that the loop above can be optimised by simply replacing the `for` loop with 6 `printf` statements than extra jump statements that may stall pipeline in the CPU or add up more instructions.


4. `pipe` is used to faster the process of compilation. The `pipe` option makes the compiler to write to a pipe instead of writing to temporary files. The `pipe` is then used as a means of communication to the other programs in the subsequent sections of the compilation.
5. `Wimplicit-function-declaration` is used to find places where the function is not declared . This is the very important as in which the `C` language takes the function declaration as `int function(void)` if its not declared. In cases where `double` is returned by a function and the function is not declared anywhere, the returned value becomes `int`. This becomes a serious bug when the function tries to estimate the distance between two co-ordinates.
6. `I` is used to provide the compiler a path to look for the include files that the program uses.
7. `g` is the option that enables the debugging in the code. It adds needed symbols for the `gdb` for the debugging purposes and to properly record coredumps and also for the `valgrind` program to correctly locate the faulty line in the code for memory leaks / invalid memory accesses.

The `-fstack-usage` option generates a stack usage file with `.su` extension. the source  file when given `-fstack-usage` as option to `gcc` or `g++`. Knowing the stack usage is really important when programming very small embedded hardware with limited resources.

The `-Wstack-usage` warns the user when the stack usage exceeds a particular given limit as option to `-Wstack-usage`. The `-Wstack-usage=2048` mean that the warning is produced if a function uses the stack over the size 2k bytes.

The `_FORTIFY_SOURCE` is a macro option to be used with `-D` just like for an `#ifdef` to be defined at the compile time.

The `_FORTIFY_SOURCE` has some of the very nice features when it comes to security. It is to be used with the arguments `1` or `2`. When given this option, the optimisation flag `-O` with levels 2 or 3 must be enabled otherwise the `_FORTIFY_` will not compile and dumps errors.

This option adds string safety mechanism when used against the libc string library. If the size of the string is deterministic at the compile time, then it can predict if any buffer overrun and underrun that can cause process access undefined memory.

If such a possibility cannot be predicted, it adds the fences at each buffer boundary to the final binary. When a buffer overrun or underrun is being detected, the program simply crashes and dumps the debug on the console. This lets the program not run when there is such issue. without the `FORTIFY` option, the program may silently execute (or may possible crash) the code beyond the given memory location allowing for the exploit to execute arbitrary code.


### __attribute__

1. `aligned`
2. `packed`



## Gcov for code coverage:

to compile with test coverage (to use with `gcov`), the following options need to be also present with `gcc`.

`-fprofile-arcs -ftest-coverage -pg -lgcov`

The gcov then adds in the profile code in the object file generated, when executed, the binary generates the profiler output files `.gcno` and `.gcna` for further analysis with `gcov`.

## LDD:

`ldd` program print any shared object dependencies on the file exe.

example:

```shell
ldd loggerTest 
	linux-vdso.so.1 =>  (0x00007ffe71305000)
	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f9774539000)
	/lib64/ld-linux-x86-64.so.2 (0x00007f9774903000)
```

all the above `.so` files are shared objects from standard libc.

## nm:

`nm` command lists the symbols in the output object file or an executable. simply running `nm` command on the object or executable dumps out the symbol information.

For example: compile the program `pipes.c` in the repo as 

```bash
gcc pipes.c
```

will produce an `a.out` binary. running `nm` on `a.out` as,


```bash
nm a.out
```

will produce the following symbol list.

```bash
0000000000601078 B __bss_start
                 U close@@GLIBC_2.2.5
0000000000601088 b completed.7594
0000000000601068 D __data_start
0000000000601068 W data_start
0000000000400700 t deregister_tm_clones
0000000000400780 t __do_global_dtors_aux
0000000000600e18 t __do_global_dtors_aux_fini_array_entry
0000000000601070 D __dso_handle
0000000000600e28 d _DYNAMIC
0000000000601078 D _edata
0000000000601090 B _end
                 U exit@@GLIBC_2.2.5
0000000000400944 T _fini
                 U fork@@GLIBC_2.2.5
00000000004007a0 t frame_dummy
0000000000600e10 t __frame_dummy_init_array_entry
0000000000400aa0 r __FRAME_END__
                 U fwrite@@GLIBC_2.2.5
0000000000601000 d _GLOBAL_OFFSET_TABLE_
                 w __gmon_start__
0000000000400978 r __GNU_EH_FRAME_HDR
00000000004005e8 T _init
0000000000600e18 t __init_array_end
0000000000600e10 t __init_array_start
0000000000400950 R _IO_stdin_used
                 w _ITM_deregisterTMCloneTable
                 w _ITM_registerTMCloneTable
0000000000600e20 d __JCR_END__
0000000000600e20 d __JCR_LIST__
                 w _Jv_RegisterClasses
0000000000400940 T __libc_csu_fini
00000000004008d0 T __libc_csu_init
                 U __libc_start_main@@GLIBC_2.2.5
00000000004007c6 T main
                 U pipe@@GLIBC_2.2.5
                 U printf@@GLIBC_2.2.5
                 U read@@GLIBC_2.2.5
0000000000400740 t register_tm_clones
                 U __stack_chk_fail@@GLIBC_2.4
00000000004006d0 T _start
0000000000601080 B stderr@@GLIBC_2.2.5
0000000000601078 D __TMC_END__
                 U write@@GLIBC_2.2.5

```

alternatively `nm` can be run on the object files.

the `nm` gives some of the symbols with letters such as `u`  `U`  `B`  `b`  `T` or `t` etc..

| symbol | meaning |
|--------|---------|
| `u` | unique symbol |
| `U` | undefined symbol | 
| `T`  `t` | text section |
| `B`  `b` | uninitialised data section |
| `D`  `d` | initialised data section |
| `R`  `r` | read-only section |


to generate an object file, use

```bash
gcc -c pipes.c
```

this generates an object file `pipes.o`. running `nm` on it is same as that of the final binary file.

```bash
nm pipes.o
```

would produce less debug symbols because the object is not linked against the libraries. The `nm` output is as follows.

```bash
                 U close
                 U exit
                 U fork
                 U fwrite
0000000000000000 T main
                 U pipe
                 U printf
                 U read
                 U __stack_chk_fail
                 U stderr
                 U write

```

what `nm` actually does is that it reads the debug symbols to produce the references. To cause no debug symbols the final executable can be compiled with the strip option `-s`. This option is only valid for the final executable but not with the obj files.

running with the strip option produces no output from `nm`.

```bash
gcc -s pipes.c
nm a.out

nm: a.out: no symbols

```

with `-g` gcc option, `nm` would produce even more dbeug symbols.

some of the useful arguments of `nm` are the following.

| argument | meaning |
|----------|---------|
| `-a` | display all the symbols |


## ar:

static linking the object files is done with the `ar` command.


```shell
ar rcv libabc.a obj.o obj1.o  ..
```

above example is generally used most commonly.

## readelf:

displays information about the elf files.



## creating shared object:

shared object files are also called as `so files`. They have an extension of `.so`. Below command produces a shared object file,


```bash
gcc -shared -fPIC file.o file1.o libsharedobj.so

```

the option `-shared` is used to create a shared object file. The option `-fPIC` tells the compiler to create a position independent code.


## objdump:
