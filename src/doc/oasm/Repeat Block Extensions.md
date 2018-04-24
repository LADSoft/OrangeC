###Repeat Block Extensions

 
 The Repeat Block Extensions allow a method for replicating lines of code.  In the simplest case, a sequence of instructions or data can be literally repeated a fixed number of times:
 
> %rep 10
>     nop
> %endrep
 
 causes the preprocessor to present 10 nop instructions to the assembler.  In a more complex case, **%assign** can be used to define a function that varies with each loop iteration, allowing easy development of lookup tables:
 
> %assign i 20
> %rep 10
>     db   i
>     %assign i i - 1
> %endrep
 
 puts the numbers from 20 to 11 in a table, in decreasing order.  This type of functionality could be used with more complex functions, for example **%rep** would be one way a CRC lookup table could be developed with OAsm.
 
 In another case the loop count could be made to vary based on previous declarations:
 
> hello   db   "Hello World"
> %assign count 64 - ($-Hello)
> %rep count
>     db   ' '
> %endrep
 
 While the latter example is not too exciting and could be done other ways, e.g. with the resb or times directives, more complex functions could be integrated into this type of loop to generate different kinds of data.
 
 Repeat blocks may be nested.  For example:
 
> %assign i 10
> %rep 3
> %rep 3
>     db i
>     %assign i i + 1
> %endrep
> %assign i i - 6
> %endrep
 
 generates enough db statements to define the following sequence:
 
 10, 11, 12,  7, 8, 9, 4, 5, 6
 
 Repeat blocks can be exited prematurely.  If a **%exitrep** directive is parsed while a repeat block is being processed, the innermost repeat block exits immediately.  Generally, one would use preprocessor conditionals to prevent the **%exitrep** directive from being processed, until some condition occurs.  For example to pop all contexts named "MY\_CONTEXT" from the top of the context stack:
 
> %repeat 1000                      // 1000 is an arbitrary value
> %ifnctx MY\_CONTEXT
> %exitrep
> %endif
> %endrep


###%rep

 **%rep** is used to start a repeat block.  It takes one argument:  the number of repetitions to go through.


###%endrep

 
 **%endrep** is used to end a repeat block.  It takes no arguments


###%exitrep

 **%exitrep** is used to exit a repeat block prematurely.
  
  
 
 