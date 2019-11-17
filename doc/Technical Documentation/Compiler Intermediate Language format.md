# Compiler Intermediate Language Format

The intermediate language is a form of three-operand instructions.   It is similar to a high-level assembly language program,
however, the instructions as shown in output documents resemble a higher-level language syntax rather than a traditional
assembly language syntax.   However, each instruction is still only a small piece of the puzzle; for example there are
arithmetic instructions which take two operands and return a result, conditional instructions which compare two operands and
perform a branch, GOSUB instructions which branch to a subroutine, etc.   Like other intermediate languages it is more similar
to assembly languages than high level languages.

## 1.0.0 Example program

As example, consider this extended hello world program:

> \#include <stdio.h>
> 
> int main(int argc, char \*\*argv)
> {
>   for (int i=0; i < 10; i++)
>       printf("hi %d\n", i);
> }

when the /Y switch is used to compile it, the compiler dumps out the initial intermediate code representation as
generated from parsing, and the optimizer dumps out the final intermediate code representation after all optimizations
have been applied and registers have been assigned

The initial representation looks something like this:

> segment end exitseg
> 
> segment codeseg
> 
> global _main
> 
> 
> global _main
> _main:
> ; Line 3: int main(int argc, char \*\*argv) 
> ; Line 4: { 
> 
> 
>   BLOCK 1
>   PROLOGUE    unset
>   BLOCK 2
> 
> L_2:
>   DBG BLOCK START
>   DBG BLOCK START
>   DBG BLOCK START
>   _i:LINK(0).I =   #0.I
>   T0.I =   _i:LINK(0).I
>   CONDGO  L_7:PC ; T0.I S>= #A.I
>   BLOCK 3
> 
> L_5:
> ; Line 6:         printf("hi %d\n", i); 
> 
>   T0.I =   _i:LINK(0).I
>   PARM    T0.I
>   PARM    #L_1:PC.A
>   TAG
>   GOSUB   #_printf:PC.UI
>   PARMADJ #8.N
>   DBG BLOCK END
>   BLOCK 4
> 
> L_8:
>   T0.I =   _i:LINK(0).I
>   T2.I = T0.I + #1.I
>   _i:LINK(0).I =   T2.I
> ; Line 5:     for (int i=0; i < 10; i++) 
> 
>   BLOCK 5
> 
L_6:
>   T0.I =   _i:LINK(0).I
>   CONDGO  L_5:PC ; T0.I S< #A.I
>   BLOCK 6
> 
> L_7:
>   DBG BLOCK END
> ; Line 7: } 
> 
>   DBG BLOCK END
>   RV.T3.I =   #0.I
>   GOTO    L_3:PC
>   BLOCK 7
> 
> L_3:
>   EPILOGUE    unset
>   RET #0.UI
> 
> 
> segment end codeseg
>
> segment stringseg
> 
> L_1:
> 
>   DC.B "hi %d"
>   DC.B 0xa    
>   DC.B 0x0
>   extern _printf
>

Here the Txxx variables are compiler-generated temporaries; they haven't been assigned to either registers or the stack at this point.

Lines such as:

> T0.I  = _i:LINK(0).I

assign a temporary variable from a program-defined variable.   At this point the variable is assumed to be stacked at offset 0 on the stack.
Offsets are relative to where the stack pointer is when entering the function.  The offset will be 0 at this point because register allocation and
stack allocation are performed in the optimizer rather than in the parser.

Here the .I means it is an integer value.  In general operands will be qualified with the value type in these types of outputs.

Lines such as:

>   T2.I = T0.I + #1.I

perform arithmetic functions; in this case it is adding 1 to T0 (T0 being a load of _i).

Lines such as :

>   CONDGO  L_5:PC ; T0.I S< #A.I

perform a comparison then a branch.   An x86 assembly language correlation to this instruction would be:

>  cmp ecx, 10
>  jl L_5

assuming T0 was later stored in ECX.

GOSUB, PARM, and PARMADJ go together to form a call site.   Note that when using FASTCALL, the parms are just eplaced by assignments
to temporary variables.

the following give the backend hints about what to generate at the beginning and ending of functions.   For example is a stack
frame required, how much space to allocate to variables, what registers to push, etc...   These are mostly target-specific although
a little will be said about them.

These values are assigned by the optimizer; coming out of the parser they are set to 'unset'.

>   PROLOGUE    unset
>   EPILOGUE    unset

instructions such as this:

>   DC.B "string"

define data, in this case a sequence of bytes.   To define a integer one might do:

>   DC.I  4

to define data which is a reference the address of main

>   DC.A  _main

and so forth.

The BLOCK and BLOCK END statements indicate the beginning and ending of basic blocks; basic blocks will often be terminated by 
a branch instruction.

Finally, instructions like this:

>   RV.T3.I =   #0.I

instruct the compiler to generate the sequence required for a return value, e.g. on the x86 most integers would be moved into eax.
Not shown here you can also have something similar after a GOSUB statement:

>   GOSUB  #_compute_value:PC.UI
>   PARMADJ #0.I
>   T4.I = RV.T3.In

In this case the RV refers to the value returned by the function.

Not shown in this example is the indirect mode of temporary access:

> T0.I  = #_i:LINK(0).I
> T1.I = (T0.A).I

being the same thing as:

>T1.I = _i:LINK(0).I

This becomes more useful when explicitly calculating pointer values

A second representation is dumped after the optimization is complete.   In the case of the sample program it looks something like this:


> segment codeseg
> 
> global _main
> 
> 
> global _main
> _main:
> ; Line 3: int main(int argc, char \*\*argv) 
> ; Line 4: { 
> 
> 
>   BLOCK 1
>   PROLOGUE    #8000000000000008.UI,#0.UI
>   BLOCK END
>   BLOCK 2
> 
> L_2:
>   DBG BLOCK START
>   DBG BLOCK START
>   DBG BLOCK START
>   T3(EBX).I =   #0.I
>   BLOCK END
>   BLOCK 3
> 
> L_5:
> ; Line 6:         printf("hi %d\n", i); 
> -
>   T2(EAX).I =   T3(EBX).I
>   PARM    T2(EAX).I
>   PARM    #L_1:PC.A
>   TAG
>   GOSUB   #_printf:PC.UI
>   PARMADJ #8.N
>   DBG BLOCK END
>   BLOCK END
>   BLOCK 4
> 
> L_8:
>   T3(EBX).I = T3(EBX).I + #1.I
> ; Line 5:     for (int i=0; i < 10; i++) 
> 
>   BLOCK END
>   BLOCK 5
> 
> L_6:
>   T5(EAX).I =   T3(EBX).I
>   CONDGO  L_5:PC ; T5(EAX).I S< #A.I
>   BLOCK END
>   BLOCK 6
> 
> L_7:
>   DBG BLOCK END
> ; Line 7: } 
> 
>   DBG BLOCK END
>   RV.T0(EAX).I =   #0.I
>   BLOCK END
>   BLOCK 7
> 
> L_3:
>   EPILOGUE    #8000000000000008.UI
>   RET #0.UI
> 
>   BLOCK END
> 
> segment end codeseg
> 
> segment stringseg
> 
> L_1:
> 
>   DC.B "hi %d"
>   DC.B 0xa
>   DC.B 0x0
>   extern _printf

The salient differences after optimization are that it figured out that it could put _i in register EBX and
elided various temporaries initially defined for the increment.   It also figured out what values to use for
the prologue and epilogue; from these values one can gather one register is pushed (EBX?) and there is a stack frame
used for referencing either variables or arguments or both.

## 2.0.0 Detailed list of instructions

An overview was given showing example output files; next will be a list of all the possible intermediate language
instructions.

## 2.1.0 Operands


## 2.1.1 Operand types

Operand types are used to qualify operands.   In the intermediate language, operands are simple data values; 
a structure or array would be viewed as a sequence of simple data values each of which is usually independently
accessed.   But there are exceptions, for example the block copy will copy the entire contents of a structure
from one place to another.

For example:

> T3.A
> _counter:RAM.I

here T3 is an address, and _counter is an integer.   For indirections through a temporary there are two types:

> (T5.A).I

Here the first type will usually be .A (in a future version of the compiler it could also be .FA) and the
second type will be the type of the data being referenced through the pointer.

The list of types is as follows:

**.N**         None
**.BOOL**      Boolean
**.STRING**    MSIL string
**.OBJECT**    MSIL object
**.A**         Address
**.FA**        FAR pointer (not used)
**.SA**        Segment value (not used)
**.UC**        Unsigned character
**.C**         Signed character
**.U16**       16-bit value
**.U32**       32-bit value
**.US**        unsigned short
**.S**         signed short
**.UI**        unsigned int
**.I**         signed int
**.UNATIVE**   unsigned native int (MSIL)
**.INATIVE**   signed native int (MSIL)
**.UL**        unsigned long
**.L**         signed long
**.ULL**       unsigned long long
**.LL**        signed long long
**.F**         float
**.D**         double
**.LD**        long double
**.IF**        imaginary float
**.ID**        imaginary double
**.ILD**       imaginary long double
**.CF**        complex float
**.CD**        complex double
**.CLD**       complex long double
**.BIT**       bit

## 2.1.2 Memory qualifiers

non temporaries usually have a memory qualifier, indicating what type of memory is being used.   For example is it an
AUTO variable?   In RAM?   In the CODE segment?

> _i:LINK(-4).I
> _bufferPointer:RAM.A

Valid are:

**:LINK(#)**             a stacked (auto) variable or parameter
    By convention, variables have a negative offset, parameters have a positive offset
**:STRUCTELEM(#)**       a reference to a structure element (MSIL)
**:PC**                  a reference to the code segment
**:TLS**                 a reference to thread local storage
**:RAM**                 a reference to RAM (variables, the data segment)
**:ABS**                 an absolute value used as an address

## 2.1.3 Operand expressions

Expressions in operans are a subset of the expressions generated by the front end.   They do a few basic things like
adding or subtracting constants from addresses, or negating an expression.


## 2.1.4 Operands

As discussed operands have types and qualifiers.   There are five types of operands.  These include constant values,
variable values, pointer indirections, and temporary values.

> T4.I
  this is a reference to the value of a temporary variable
> (T4.A).I
  this is a reference to the value a temporary variable points to
> _counter:RAM.I
  this is a reference to a variable
> \#_counter:RAM.I
  this is a reference to the address of a variable
> \#5.I
  this is a reference to a constant value (constants are displayed as hexadecimal numbers)
  
which as per section 2.1.3 the constants can be added to other constants:

> \#_buffer:RAM.B + 5


## 2.2.0  Intermediate instructions

Most simple operations are trivially described by an instruction, however, this does not imply that the
target processor code generated will be simple.   For example shifting, multiplying, or dividing long longs
would be the same as anything is in the intermediate language, but might result in a call to some library
helper function on an architecture with 32 bits.

## 2.2.1 Passthrough

> PASSTHROUGH

The passthrough instruction is a placeholder for a programp-specified assembly language instruction.   These are 
target-specific, and no attempt is made to display the actual instruction in these types of output files.


## 2.2.2 Label

> L_5:

Label definitions such as the one for L_5 are compiler-generated labels used for the targets of implicitly declared 
branches such as the ones in IF-ELSE or WHILE statements.   They are also used in other situations, e.g. for switch-case
statements or for C++ exception table information.


## 2.2.3 GOTO

> GOTO L_6:PC

an unconditional branch to another location

## 2.2.4 GOSUB

>GOSUB _compute_somthings:PC

a branch to a subroutine.

This will usually be accompanied by PARM and PARMADJ instructions.   For example the
C language function call:

AddTwoNumbers(5, 10)

might be generated as follows:

> PARM #10.I
> PARM #5.I
> GOSUB #_AddTwoNumbers:PC
> PARMADJ #8

where the PARMADJ cleans the stack up after the function call.


there is also a PARMBLOCK which pushes a structured value on the stack:

> PARMBLOCK _structuredValue.A(#20)

The compiler might sometimes use STACKALLOC to create space for a temporary class or structure being constructed then passed 
to another function.   The PARMADJ would take care of cleaning up such space.


## 2.2.5  RET

> RET 
> RET #8

This returns from a subroutine.  Here the optional number would be used for example in WIN32 STDCALL functions (or in PASCAL functions)
to pop the arguments off the stack after returning.

## 2.2.6 Add

> T4.I = T3.I + T2.I

Add two values and store the result in T4

## 2.2.7 Subtract

> T4.I = T3.I - T2.I

Subtract two values and store the result in T4

## 2.2.8 Unsigned divide

> T4.UI = T3.UI U/ T2.UI

Divide two values and store the result in T4

## 2.2.9 Unsigned modulus

> T4.UI = T3.UI U% T2.UI

Divide two values and store the modulus of the result in T4

## 2.2.10 Signed divide

> T4.I = T3.I S/ T2.I

Divide two values and store the result in T4

## 2.2.11 Signed modulus

> T4.I = T3.I S% T2.I

Divide two values and store the modulus of the result in T4

## 2.2.12 multiply unsigned and return high word

> T4.UI = T3.UI U*H T2.UI

Multiply two unsigned values to an intermediate type of unsigned long long, and return the high 32 bits of the result

This is used to optimize DIVIDE by constant instructions.

## 2.2.13 multiply signed and return high word

> T4.I = T3.I S*H T2.I

Multiply two signed values to an intermediate type of signed long long, and return the high 32 bits of the result

This is used to optimize DIVIDE by constant instructions.

## 2.2.14 multiply

> T4.UI = T3.UI * T2.UI
> T4.S = T3.S * T2.S

Multiply two values and store the result in T4

## 2.2.15 shift left

> T4.I = T3.I << T2.I

Shift a value left and store the result int T4

## 2.2.16 Unsigned shift right

> T4.I = T3.I UI >> T2.I

Shift a value right and store the result int T4

## 2.2.17 Signed shift right

> T4.I = T3.I S>> T2.I

Shift a value right and store the result int T4

## 2.2.18 Bitwise and

> T4.I = T3.I & T2.I

Take the bitwise and of two operands and store the result int T4

## 2.2.19 Bitwise or

> T4.I = T3.I | T2.I

Take the bitwise or of two operands and store the result int T4

## 2.2.19 Bitwise exclusive or

> T4.I = T3.I ^ T2.I

Take the bitwise exclusive or of two operands and store the result int T4

## 2.2.20 Negation

> T4.I = -T3.I

Negate a value and store the result.   This will be two's complement on many target processors.

## 2.2.21 Complement

> T4.i = ~T3.I

Take the ones complement of a value and store the result.

## 2.2.22 assignments

> T4.I = T3.I

assign one operand to be equal to another

## 2.2.23 Switch statements

Switch statements come in two parts.   The first part specifies the switch selector and default clause, and the second 
part is a list of constant/Label pairs (for the cases).

This is the entire processing of a switch in the parser and optimizer, however, the backend may be smarter and try to do something
like generate a binary tree of conditional branches for a large list of sparse cases.   Ot s lookup table for a non-sparse list.


> COSWITCH(#3.UI, T3.I, #10.UI, L3)
> SWBRANCH(#5.I, L_5)
> SWBRANCH(#A.I, L_6)
> SWBRANCH(#14.I, L_7)


here we define a switch based on the value of T3.i.   There are three case values and the spread between the lowest (5) and the highest +1 (21) is sixteen.
The cases go labels 5,6,7 and the default label is 3.

## 2.2.24 block assign

> T3.A BLOCK= #_structaddress:RAM.A(#20.I)

move a structured value with 32 bytes from RAM to a temporary area

## 2.2.25 block clear

> T3.A BLKCLR(#20.I)

clear a block with 32 bytes

## 2.2.26 block compare

> BLKCOMPARE:L_10:PC, T3.A, _buffer:RAM.A

compare two blocks

## 2.2.27 Conditional branches

> CONDGO:L_10:PC ; T3.I == #5

branch to label 10 if T3 is equal to 5.   The == may be replaced by a variety of other conditional operators.  These include:


**U<**     unsigned less than
**U>**     unsigned greater than
**U>=**    unsigned greater than or equal
**U<=**    unsigned lesl than or equal
**==**     equality
**!=**     inequality
**S<**     signed less than
**S>**     signed greater than
**S>=**    signed greater than or equal
**S<=**    signed less than or equal

## 2.2.28 Computation of equality or inequality


> T5.I = T3.I == #5

compute a boolean indicating whether the two values are equal.   The == may be replaced by a variety of other conditional operators.
These include:

**U<**     unsigned less than
**U>**     unsigned greater than
**U>=**    unsigned greater than or equal
**U<=**    unsigned lesl than or equal
**==**     equality
**!=**     inequality
**S<**     signed less than
**S>**     signed greater than
**S>=**    signed greater than or equal
**S<=**    signed less than or equal

## 2.2.29 Debug blocks

The beginning and end of source code blocks are delimited by the DBG_BLOCK_START and DBG_BLOCK_END tags, for correlation of
variable declarations with positions in the code segment.   This is used by the debugger.


## 2.2.30 Basic blocks

Basic blocks as generated by the code generator are delimited by BLOCK START and BLOCK END statements.   These blocks are used by
the optimizer and don't bear an obvious relationship to the blocks specified in the source code.  Instead of optimizing on a 
per-instruction basis most optimizations are done on a per-block basis.   Blocks are guaranteed to end when a branch has 
been indicated, so they are also often used to graph the branch tree of a function.

## 2.2.31 Prologue and epilogue


> PROLOGUE #8000000000000008, #14
> EPILOGUE #8000000000000008, #14

these give indications to the backend about the needs of the generated stack frame for a function.

The first number has the high bit set if a stack frame is required; the lower bits indicate which registers that
need to be pushed are being used.  

The second number gives the amount of space to allocate for local variables.


## 2.2.32 Stack allocations

> T3.A = STACKALLOC #C.I

Allocate 12 bytes on the stack and return the new stack pointer

> LOADSTACK T3.A

Load the stack pointer into T3

> SAVESTACK T3.A

Save T3.A into the stack pointer

## 2.2.33 tags

> EXPR TAG 7

expression tags are used by the MSIL backend for making sure that certain types of nested expressions get
appropriate 'pops' generated.

> TAG

Tags are used by the MSIL backend to indicate a call site.

## 2.2.34 Windows SEH

SEH tags are used to bracket windows exception handling sequences.   At the beginning of the sequence
one of the SEH modes will be signalled, orred with #0x80.  At the end of the sequence the SEH tag will appear
with the mode not orred with #0x80.

>  SEH 1

the body of the SEH sequence is a __try block statements

> SEH 2

the body of the SEH sequence is a __catch block statements

> SEH 3

the body of the SEH sequence is a __fault block statements

> SEH 4

the body of the SEH sequence is the __finally block statemnts

## 2.2.35 Atomics

> T3.I = ATOMIC FLAG FENCE #memoryOrder, _i:RAM.I
> T3.I = ATOMIC TEST AND SET #memoryOrder, _i:RAM.I
> ATOMIC CLEAR _i:RAM.I
> CMPSWP address, value, third
> XCHG first, second

Various atomic operations.

## 2.3.0 Data and administrative instructions


## 2.3.1 Segment statements

> segment code
> segment end code

brackets a sequence of code or data into a logical section.   The following predefined sections exist:

**exitseg**			not in a segment
**codeseg**			code/instructions
**dataseg**			data/variables
**bssxseg**			uninitialized data/variables
**stringseg**		strings
**constseg**		constant values (deprecated
**tlsseg**			tls values
**startupxseg**		sequence of functions to run at program start
**rundownxseg**		sequence of functiosn to run at program end
**tlssuseg**		sequence of functions to run at thread start
**tlsrdseg**		sequence of functions to run at thread end
**fixcseg**			code segement fixups (currently unused)
**fixdseg**			data segment fixups (currently unused)

## 2.3.2 Variable definitions

> global _main
> _main:
> export _exportablefunc
> _exportablefunc:

declare a named label, and give it linkage attributes

## 2.3.3 Label definitions

> L_5:

declare a compiler-generated label

## 2.3.4 reserve memory

> reserve 55

reserves 55 bytes of uninitialized memory

## 2.3.5 Symbol reference

> DC.A _counterVariable

declares data which is the offset of _counterVariable

## 2.3.6 startup reference

> DC.A _startupFunc, 32

Defines data which is a startup function to put in a startup or rundown table, and give it a priority

## 2.3.7 label reference

> DC.A L_5

declares data which is the offset of a compiler-generated label

> DC.I L_77 - L_3

define data which is the difference between two label offsets.  Used by exception handling data generation

## 2.3.8 Define data

> DC.B #77

Defines data which is a constant.   The .B may be replaced by any of the basic types:

**.BOOL**      Boolean
**.STRING**    MSIL string
**.OBJECT**    MSIL object
**.A**         Address
**.FA**        FAR pointer (not used)
**.SA**        Segment value (not used)
**.UC**        Unsigned character
**.C**         Signed character
**.U16**       16-bit value
**.U32**       32-bit value
**.US**        unsigned short
**.S**         signed short
**.UI**        unsigned int
**.I**         signed int
**.UNATIVE**   unsigned native int (MSIL)
**.INATIVE**   signed native int (MSIL)
**.UL**        unsigned long
**.L**         signed long
**.ULL**       unsigned long long
**.LL**        signed long long
**.F**         float
**.D**         double
**.LD**        long double
**.IF**        imaginary float
**.ID**        imaginary double
**.ILD**       imaginary long double
**.CF**        complex float
**.CD**        complex double
**.CLD**       complex long double
**.BIT**       bit

Note that in the special case of "DC.B" a string constant may appear in place of the numeric constant.

> DC.B "hello"

## 2.3.9 Virtual sections

> Virtual sectionName
> Virtual end sectionName

section name is a compiler generated name, which is off the mangled value of a C++ function name or data.

## 2.3.10 Alignment

> align 4

aligns current section on 4-byte boundary

## 2.3.11 Virtual table thunk

> [this] = [this] - 16
> goto @myfunc$qv

when overriding base class virtual functions with derived class virtual functions, it is sometimes necessary
to add a thunk to adjust for the new offset of the THIS pointer within the derived class.

## 2.3.12 Import thunk

> section data
> printf_pointer DC.A L_5
> section end data
> section code
> L_5:
>      goto [printf]

It is sometimes necessary to generate a thunk through the windows import table, for example for defining data
which is supposedto be a pointer to a function in the DLL.


## 2.3.13 member pointers which are pointers to virtual members

> GOTO [[this] + %d]

used as a thunk to branch to a virtual function, when referneced from a member pointer variable.

## 2.13.14 Auto variable reference

> DC.I OFFSETOF _i.Link(-4) + 0

defines data which is the offset from the top of the stack frame of an auto variable in a function.
Used in exception processing to locate variables to destroy during stack unwinding.

