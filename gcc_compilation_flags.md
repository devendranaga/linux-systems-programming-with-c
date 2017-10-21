# Gcc compilation flags

The following Warning options are useful while using the `gcc`.

1. `Wall` to get all sorts of warnings related to the C code. This will also perform a code review (basic style) and catches bugs that occur at beginning or intermediate stages of the testing process.
2. `Wextra` would turn on more warnings that are not enabled by `Wall`.
3. `Werror` forces the programmer to fix the warnings generated.
4. `On` is an optimisation flag that is useful in the code optimisation at the compilation stage.
5. `pipe` is used to faster the process of compilation. The `pipe` option makes the compiler to write to a pipe instead of writing to temporary files. The `pipe` is then used as a means of communication to the other programs in the subsequent sections of the compilation.
6. `Wimplicit-function-declaration` is used to find places where the function is not declared . This is the very important as in which the `C` language takes the function declaration as `int function(void)` if its not declared. In cases where `double` is returned by a function and the function is not declared anywhere, the returned value becomes `int`. This becomes a serious bug when the function tries to estimate the distance between two co-ordinates.

The following debug options are useful while using the `gcc`.

1. `-g` enables the symbols in the generated binary so that it could be debuggable in a crash.


## Notes

The most common gcc flags used are:

    `-Wall -Wextra -Wshadow -Werror`
    
Most of the 90 % bugs that found while coding gets fixed if the flags are honored well. To best use of the gcc compiler, always turn on the -Werror option.
