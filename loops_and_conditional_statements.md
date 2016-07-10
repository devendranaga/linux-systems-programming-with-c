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

```c
    if (condition1) {
        // execute conde path 1
    } else if (condition2) {
        // execute code path 2
    } else {
        // execute code path 3
    }
```    
    
 Ex: 
 
 ```c
      int a = 2;
     
      if (a == 1) {
          printf("a value is %d\n", a);
      } else if (a == 2) {
          printf("a value is %d\n", a);
      }
 ```
     
An else will always be at the last of the `if` `else if` conditional.

A series of `if` and `else if` conditional statements is also called an `else if ladder`.

For ex: an `else if ladder` would look as below.

```c
if (fruit_name == FRUIT_APPLE) {
    printf("Apple\n");
} else if (fruit_name == FRUIT_ORANGE) {
    printf("Orange\n");
} else if (fruit_name == FRUIT_PINEAPPLE) {
    printf("Pine Apple\n");
} else if (fruit_name == FRUIT_GRAPE) {
    printf("Grape\n");
} else {
    printf("Noooo .. i dont have any Fruits\n");
}
```

Always make sure that an `else` statement is always present in an `if.. else if` conditional. This allows not to miss any case that does not satisfy either `if` or `else if`. Most of the software bugs lie around the `if`, `else if` and `else` parts.

Sometimes, the `else if ladder` is replaced with a `switch` statement. The `switch` statement is often and mostly used for data elements of type `int` or `char`. On the otherhand, `switch` can't be used with `float`, `double` or strings. The compiler cries if any of them are being used in a switch.

For ex: the `switch` statement converted fruit example looks as below.

```c
switch (fruit_name) {
	case FRUIT_APPLE:
       printf("Apple\n");
   break;
   case FRUIT_ORANGE:
       printf("Orange\n");
   break;
   case FRUIT_PINEAPPLE:
       printf("Pine Apple\n");
   break;
   case FRUIT_GRAPE:
       printf("Grape\n");
   break;
   default:
       printf("Noooo .. i dont have any Fruits\n");
}
```
With in the `switch` statement, a series of `case` blocks with the `default` block can be used. Consider the `case` blocks as the `if` conditionals followed by the `break` statement causing the next `case` statement NOT to execute. If there is no `break` statement, the next conditions may execute and the result will be entirely different. We will see about this in the following paragraphs. The `default` statement is like an `else` conditional. If any of the `case` statements does not satisfy the condition given in the `switch` the `default` statement gets executed.

The conditionals statements are very important in the coding.

There are cases where the `switch` statement is not always `if .. else if`. Consider the following example from the stackoverflow website [here](http://programmers.stackexchange.com/questions/162574/why-do-we-have-to-use-break-in-switch)

There comes a situation where you need to loop until some condition is evaluated to true. You can do with the `while` and `for` statements. There is also `do .. while` statement. We will see about these below.

The `while` statement loops till the condition evaluates to false. It looks like the following..

    while (condition) {
        // series of statements    
    }

When the condition fails, the `while` conditional will not get executed.

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
     
The initialiser sets the value of the variable to 1 and it gets executed only once. The condition then executes and if condition becomes satisfied the statements with in the for loop are executed. After the last statement is executed, the increment part of the for loop is executed and then the condition with in the for loop. The initializer will only be executed in the beginning.

There can be cases where one needs to run the program forever. This forever programs we call infinite loops.

The infinite loops can be written as the following:

     while (1) {
        // statements;
     }
     
     for (;;) {
        // statements;
     }
     
An infinite program, when ran, can be stopped via signalling it by the `ctrl + c` combination from the Keyboard. We look at the signals more in the upcoming topics.

Infinite loops cause the program to consume all of the CPU causing the system load go high and the temperature of the system. Often, the daemons use the infinite loops in a controlled way to make the program run forever. The controlled way avoids the heavy CPU loads.