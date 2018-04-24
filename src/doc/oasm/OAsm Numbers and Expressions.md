##Numbers and Expressions


###Integers
 

 
 Integers may be specified in base 8, base 10, or base 16.  The rules for specifying integers follow the C language standard; e.g. if a number starts with 0 it is assumed to be base 8; if it starts with 0x it is assumed to be base 16; otherwise it is base 10.  For compatibility with other assemblers OAsm also supports the trailing 'h' to indicate hexadecimal values (base 16) but such numbers must start with a digit to prevent them from being interpreted as labels.
 
 For example:
 
>     012   ; octal for the decimal value 10
>     12   ; the decimal value 12
>     0x12 ; hexadecimal for the decimal value 18
>     012h ; hexadecimal for the decimal value 18


###Floating point values

 Floating point values are specified similarly as to a C compiler.  For example:
 
>     1.03
>     17.93e27
>     10000.4e-27
 
 Note that floating point values must start with a digit in **OAsm**.  .003 is not a valid floating point value because character sequences starting with a dot are interpreted as local labels.


###Characters and Strings

 **OAsm** makes no real distinction between single characters and sequences of characters.  Single quotes (') and double quotes (")  may be used interchangeably.  But the interpretation of characters and strings depends on context.
 
 When used in an instruction:
 
> mov    ax,"TO"
 
 The operand will be constructed in such a way that storing it to memory will result in the characters being stored in the same order they were typed.  In other words, the sequence:
 
> mov    ax,"TO"
> mov    \[label\],ax
 
 will result in the value at label being the same as if the assembler directive db were used to initialize the value:
 
> label    db "TO"
 
 Characters at the end of a string that cannot be encoded in the instruction will be lost, thus:
 
> mov    ax,"hi roger"
 
 is the same as:
 
> mov    ax,"hi"
 
 because the register ax only holds the equivalent of two characters. 
 
 On the other hand, data areas may be initialized with strings with various directives.  There are three types of values that can be initialized this way; bytes (1byte), words(2 bytes), and double-words(4 bytes).  For ASCII characters, the encoding is just the character, with enough leading zeros to pad to the appropriate size.


###Program Counter

 The symbol '$', by itself, means the current program counter.  This is an absolute program counter, and if passed through to the linker will result in an absolute offset being compiled into the program.  But sometimes it doesn't need to be used as an absolute value, for example it can be used to find the length of a section of data:
 
> mylabel   db   "hello world",10,13
> hellosize EQU $-mylabel
 
 where the EQU statement assigns the value of the expression '$-mylabel' to the label hellosize.


###Beginning of section

 
 The symbol '$$' means the beginning of the current section.  For example the expression $-$$ gives the offset into the current section.


###Expressions

 
 Usually wherever a number may be specified, a more complex expression consisting perhaps of numbers, labels, special symbols, and operators may be substituted.  **OAsm** uses operators similar to the ones found in a C compiler, with precedence similar to the C compiler, and introduces some new operators as well.  See table 1 for a listing of operators, and table 2 for a listing of operator precedences.
 
    

|colheader |colheader |
|--- |--- |
|( )|specify evaluation order of sub-expressions|
|SEG|refers to segment of a variable (16-bit only)|
|-|unary minus|
|+|unary plus|
|~|bitwise complement|
|!|logical not|
|\*|multiply|
|/|divide, unsigned|
|/-|divide, signed|
|%|modulous, unsigned|
|%-|modulous, signed|
|+|addition|
|-|subtraction|
|WRT|offset of a variable, from a specific segment|
|>>|unsigned shift right|
|<<|unsigned shift left|
|>|greater than|
|>=|greater than or equal|
|<|less than|
|<=|less than or equal|
|==|equals|
|!=|not equal to|
|&|binary and|
|^|binary exclusive or|
|||binary or|
|&&|logical and|
||||logical or|   

  
  Table 1, Operator meanings
  
     

|colheader |colheader |
|--- |--- |
|( )|parenthesis|
|SEG, -, +, ~, !|unary operators|
|\*, /, /-, %, %-|multiplicative operators|
|+, -, WRT|additive operators|
|<<, >>|shift operators|
|>, >=, <, <=|inequality operators|
|==, !=|equality operators|
|&|bitwise and|
|^|bitwise exclusive or|
|||bitwise or|
|&&|logical and|
||||logical or|   

  
  
 Table 2, Operator precedence from highest to lowest
 
 Expressions involving labels or segments will often be pushed off to the linker for evaluation, however, the linker only knows simple math such as +-\*/ and SEG.  Sometimes however, an expression such as '$-mylab' can be directly evaluated by the assembler, provided mylab is defined earlier in the current segment.  Such evaluations would result in a constant being passed to the linker.
 
 Note that **OAsm** mimics a multipass assembler, and will attempt to optimize branches to the smallest available form.  This is normally not a problem as after each optimization pass OAsm will reevaluate expressions found in the code or data.  However, some assembler directives such as EQU and TIMES evaluate their operands immediately, when the instruction is encountered.  And all branches start out at the largest possible size.  That means that a sequence like:
 
> section code USE32
> label:
>     cmp eax,1
>     jz  forward
>     inc eax
> forward:
> size EQU forward - label
 
 will result in 'size' being evaluated with the jz being a 6-byte instruction, but the final code will have the jz being a two-byte instruction.  This disparity between the calculated value and the actual value can introduce subtle bugs in a program.  To get around this explicitly clarify any jumps in a region that is going to be sized with 'short' or 'near':
 
> label:
>     cmp eax,1
>     jz  short forward
>     inc eax
> forward:
 
 
 Data directives aren't subject to size optimizations, so in the usual case of taking the size of a data region this isn't an issue.
 
    