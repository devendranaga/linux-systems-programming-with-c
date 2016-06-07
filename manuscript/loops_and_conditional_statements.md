# loops and conditional statements


The 'C' programming language provides the conditional statements `if`, `else` and `else if` to perform various functions. For ex: run the program until this condition satisfy and exit if it does not. This would be the case for the algorithms. They always check for conditions that satisfy and execute and proceed to the next stages.


The if conditional checks for a truth statement. Any number other than 0 is true in C (Yes it means even the negative numbers).

The if condition looks like the below.

     if (condition) {
        // execute staement(s)
     }
     
if the `condition` is evaluated to true, then the statements inside the `if` are executed. Otherwise the statements enclosed in the `if` are not going to be run.

The **otherwise** part we call it the `else` conditional. The else conditional executed if the conditional in the `if` statement is not executed. Very simple.

The `else` condition looks like the below.

     if (condition) {
         // execute code path 1
     } else {
         // execute code path 2
     }
     
     
 Ex:
 
      int a = -1;
      
      if (a) {
          printf("a value is %d\n", a);
      } else {
          printf("a value is zero\n");
      }

Remember that the `else` conditional always follow the `if`.

The compiler always give an error if the `else` conditional does not follow the `if`.

If there are more than one conditions to be executed based on some truth conditionals, we use `else if`. 

The `else if` conditional looks like the below.

    if (condition1) {
        // execute conde path 1
    } else if (condition2) {
        // execute code path 2
    } else {
        // execute code path 3
    }
    
    
 Ex: 
 
      int a = 2;
     
      if (a == 1) {
          printf("a value is %d\n", a);
      } else if (a == 2) {
          printf("a value is %d\n", a);
      }
      
An else will always be at the last of the `if` `else if` conditional.

A series of `if` and `else if` conditional statements is also called an `else if ladder`.


The conditionals statements are very important in the coding.

There comes a situation where you need to loop until some condition is evaluated to true. You can do with the `while` and `for` statements. There is also `do .. while` statement. We will see about these below.

The `while` statement loops till the condition evaluates to false. It looks like the following..

    while (condition) {
        // series of statements    
    }
    
The series of statements are evaulated again and again till the condition becomes invalid.

For ex: let us consider a case where we print 10 number from 1.

    int i = 1;
    
    while (i <= 10) {
         printf("number %d\n", i);
         i ++;
    }
    
The i value before the `while` is 1 and the loop starts printing the numbers from 1 till 10 and when i becomes 11 the condition evaulates to false and the loop stops.

The `for` is similar but it has the following advantages.

`for` statement has initializer, condition evaluation and incrementer.

     for (init; cond; increment) {
     }
     
For ex: the `while` loop is rewritten as 

     int i;
     
     for (i = 1; i <= 10; i ++) {
         printf("i value %d\n", i);
     }
     
There can be cases where one needs to run the program forever. This forever programs we call infinite loops.

The infinite loops can be written as the following:

     while (1) {
        // statements;
     }
     
     for (;;) {
        // statements;
     }
     
An infinite program, when ran, can be stopped via signalling it by the `ctrl + c` combination from the Keyboard. We look at the signals more in the upcoming topics.