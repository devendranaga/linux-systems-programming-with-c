## Makefiles

Makefile is a different language, is a group of instructions that when executed using the command ```make``` generates the output binaries.

The binary names, how to generate them, the optimizations are specified in the Makefile.


Makefiles are useful to organize and compile a large set of C, C++ or any other source file into different sets in different ways like customization.

The command ```make``` is available in the ```automake``` package of any linux distribution.

To install ```make``` on Ubuntu:

```bash
sudo apt-get install make automake
```

on Fedora (as root):

```bash
dnf install make automake
```


To compile our gist example hash.c using the makefile, we put it in a file called, `Makefile`.

`cat Makefile`

```makefile
hash:
    gcc hash.c -o hash
```
  
  
a `make` command is issued in the same directory gives us the `hash` binary.
  
Now, we can add more things to the Makefile as the following.
  
```makefile
all:
    gcc hash.c -o hash
  
clean:
    rm -rf hash.o hash

```
  
We have two subcommands now for the make argument. They are `all` and `clean`.
  
Issuing `make clean` would remove the binary `hash` and the object file `hash.o`.
  
Issuing `make all` would compile the binary and generate the `hash`.
  
Let us define the variable `CC` which can either be assigned with in the makefile to the C compiler `gcc` or passed via a command line argument. The command line argument is mostly used in cases where the compiler can be an architectural dependent compiler (in case of cross compilation) or a proprietary compiler.
  
The makefile now looks as below.
  
```makefile
CC=gcc
  
all:
    $(CC) hash.c -o hash
clean:
    rm -rf hash.o hash
```
  
We can pass the `CC` variable via the command line as following `make CC=gcc`.
