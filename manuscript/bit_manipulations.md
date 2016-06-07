# bit manipulations

The AND (&), OR (|), XOR (^), Negate (~), Not (!) and bit shifts (<< and >>) operations are possible in C.


The AND operation is used to test if the bit is set or unset. A set is called a 1 and unset is called a 0.

For example consider the following number 128 (in hex 0x80).

lets,
    
        a = 0x80
        
        MSBit_set = !!(a & 0x80)
        
        if (MSBit_set)
            printf("MSBit is set\n");
        else
            printf("MSBit is not set\n");

The above program tests if the Most significant bit (MSbit) of the variable a is set. The test is performed with a mask 0x80. The two Not (!) operators have the following purpose. The Not operator first inverts the value. If the value is not zero, it inverts it into 0 and if the value is 0, it inverts into 1. A Not operation that is performed twice would only give either 1 or 0 only.

Ex: if the Not operation performed on 12, !12 would give 0. and a Not operation again on 0 would give us 1. that is the Not operation results in a binary value. Thus if a value that is non-zero, a twice Not operator would make it 1. If the value that is zero, a twice Not operator would make it 0.

