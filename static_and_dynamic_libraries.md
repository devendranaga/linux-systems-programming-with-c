## Static and dynamic libraries


1. The static libraries are denoted with `.a` extension while the dynamic libraries are denoted with `.so` extension.
2. The static libraries, when linked they directly add the code into the resulting executable. Thus allowing the program to directly resolve the function references at the linker time. This also meaning that the program size is greatly increased.
3. The dynamic libraries, when linked they only add the symbol references and addresses where the symbol can be found. So that when the program is run on the target system, the symbols will be resolved at the target system (mostly by the ld loader). Thus, the dynamic library does not add any code to the resulting binary.
4. The dynamic library poses a problem with the un-resolved symbols when the program is run on the target system.
5. The Program binary versions can be changed or incremented irrespective with the dynamic library linkage as long as the dynamic library provides the same APIs to the user program. Thus introducing the modularity.

To create a shared library:

```bash
gcc -shared -o libshared.so -fPIC 1.c 2.c ..
```
    

when creating the shared library using the -fPIC is most important. The position independent operation allows the program to load the address at the different address.

To create a static library:

```bash
ar rcs libstatic.a 1.o 2.o
```

## The libdl.so (Dynamic loading of functions)

The dynamic loading allows program to load a library at run time into the memory, retrieve the address of functions and variables, can perform actions and unload the library. This adds an extended functionality to the program and allows methods to inject code into the program.

`dlopen`: open the dynamic object

`dlsym`: obtain the address of a symbol from a `dlopen` object

`dladdr`: find the shared object containing a given address

include the header file `<dlfcn.h>` to use the dynamic library. The `dladdr` function prototype is as follows:

```c
int dladdr(void *addr, Dl_info *dlip);
```

The `Dl_info` looks as the following:

```c
typedef struct {
    const char *dli_fname;
    void       *dli_fbase;
    const char *dli_sname;
    void       *dli_saddr;
} Dl_info;
```

`dladdr` shall query the dynamic linker for information about the shared object containing the address `addr`.  The information shall be returned in the user supplied data structure referenced by `dlip`.

The `dlopen` is defined as follows:

```c
void *dlopen(const char *filename, int flags);
```
    
The `dlopen` function loads the dynamic library file referenced by `filename` and returns a handle pointer for the libbrary. The `flags` arguments tell the `dlopen` on the way to load the library. Usually `flags` are set to `RTLD_LAZY`. The `RTLD_LAZY` performs the lazy binding. This means that it only resolves the symbols when the code calls them.

The opened library handle is then used to get the function addresses with referencing to their names. This is done with `dlsym` function call.

The `dlsym` is defined as follows:

```c
void *dlsym(void *handle, const char *symbol);
```
    
The `symbol` is the function name and `handle` is the return of `dlopen`. The `dlsym` returns the address of the function. It is then captured into a function pointer for further use.

Let us define a file named `lib.c` (You can also download it [here](https://github.com/DevNaga/gists/blob/master/lib.c)). It is defined as follows.

```c
lib.c:

#include <stdio.h>

int function_a()
{
    printf("function a is defined\n");
    return 0;
}
```

We then compile it as follows.

```bash
gcc -fPIC -o lib.c
```
    
The `lib.o` is generated with the above compiler command.

Let us define the code that perform the dynamic loading. (You can also download it [here](https://github.com/DevNaga/gists/blob/master/dlopen.c))

```c
#include <stdio.h>
#include <dlfcn.h>

int func(void);

int main()
{
    int (*func)(void);
    void *dl_handle;

    dl_handle = dlopen("./libtest.so", RTLD_LAZY);
    if (!dl_handle) {
        fprintf(stderr, "no handle found \n");
        return -1;
    }

    func = dlsym(dl_handle, "function_a");
    printf("function %p\n", func);

    func();

    return 0;
}
```

We then compile the above program as the following:

```c
gcc dlopen.c -ldl -rdynamic
```
    
The `ldl` and `rdynamic` are used to compile and link the above code.

When we execute the resultant `a.out` file it prints the following:

```bash
function 0x7f05b82d46e0
function a is defined
```
