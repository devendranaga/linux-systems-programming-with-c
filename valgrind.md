# valgrind


1. valgrind is a memory checker. It detects various memory errors and problems such as access errors, leaks, un-freeable memory etc.
2. valgrind can be installed in the following way:

     on Ubuntu:
     
            sudo apt-get install valgrind
            
     on Fedora:
     
            #  dnf install valgrind

3. valgrind simple example:

           valgrind -v --leak-check=full ./leak_program
          
