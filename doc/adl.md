# Architecture Description Language Version 1.0

The architecture description language (ADL) is intended to form a high-level specification of architecture-specific data such as instruction codings.   Later, it may be used to generate compiler backends.   ADL files are xml files with specific nodes and attributes that are processed by a parser, which then generates useable C++ code to translate source code to a byte code sequence.   This could be used for example an assembler.   With modifications to the ADL compiler, the codings could also be used for a disassembler and/or simulator at some future point.

The ADL specifies a way of parsing the input to find the correct sequence of byte codes, then streaming them out to a byte array.   The streaming is done with a resolution of as low as 1 bit, so arbitrary byte codes can be streamed out while still preserving individual entities such as a register or scale factor at the source level.

This documentation has two major parts; a description of the ADL and a description of how to interface the generated code e.g. to an assembler.

## ADL description

An ADL file is an xml file.   The outer data tag is ADL.

The ADL file has three major top-level entities.

The `Processor` entity gives the name of the architecture.   In the future it may give other architectually specific information.

The `Coding` entity gives information for assemblers/dissassemblers.   It allows a translation to be performed between a source-code representation  and the coding sequence required by a CPU supporting the architecture.

The `Compiling` entity is not currently supported, in the future it will be used to specify instruction sequences for the compiler to use as well as things like peephole optimizations and register allocation information.

An example file is as follows:

```xml
  <ADL>
    <Processor Name="x64">
        <Param Name="Case Sensitive" Value="false"/>
        <!-- more params -->
    </Processor>
    <Coding>
    <!-- coding data -->
    </Coding>
    <Compiling profile="bit32">
    <!-- compiler data -->
    </Compiling>
  </ADL>
```


### Processor entity

The `Processor` entity simply gives the name of the architecture, in text format.   This will be used in the generated code, and to select names for generated files.

For example:

```xml
    <Processor Name="x64">
        <Param Name="Case Sensitive" Value="false"/>
        <!-- more params -->
    </Processor>
```

There are three parameters at present.

```xml
    <Param Name="CaseSensitive" Value="false"/>
    <Param Name="Endian" Value="little"/>
    <Param Name="BitsPerMAU" Value="8"/>
```

The `Name` attribute is the name of the parameter, and the `Value` attribute is the value of the parameter.

The `CaseSensitive` parameter indicates whether the assembler source code is case sensitive.   It is used for parsing.

The `Endian` parameter indicates the endianness of the architecture.   E.G `big` or `little`.   It is used for streaming numeric constants defined with `number` into the coding sequence.  

The `BitsPerMAU` parameter is the minimum number of bits that may be retrieved by the hardware.   Right now only 8 (byte) is supported.


### Coding entity

The `Coding` entity gives the correlation between a source code representation, and the corresponding instruction byte codes.   It has 6 groups of definitions, which together form the basis for translation.

These groups are

* Number Formats             specifies number formats or numeric constants
* State Variables            specifies variables that can be used for example to choose between different CPU modes
* Coding States              specifies parts of a coding sequence that depend on the assembler state
* Register definitions       specifies registers in terms of ordinals and the address modes that can use them.
* Addressing Modes           specifies the relationship between addressing modes and coding sequences
* Opcodes                    specifies the relationship between opcodes and their allowed operands, and expands upon the coding sequences

Where coding sequences are streaming specifications for instructions.

This information is specified free form in the confines of the `Coding` entity, without further grouping in the xml file.

For example:

```xml
    <Coding>

        <Number Class="signed8">
            <Instance Value="-#:8"/>
        </Number>
        <!-- more number formats -->
        <StateVars>
            <Instance  Name="processorbits" Init="16"/>
        </StateVars>
        <!-- more state variables -->
        <State Name="addr32">
            <Value Cond="'processorbits'==32" Coding=""/>
            <Value Cond="'processorbits'!=32" Coding="0x67:8"/>
        </state>
        <!-- More coding states -->
        <Register Class="general">
            <Instance Name="al" Ord="0" B="0" X="0" R="0" W="0" Class="reg8,low"/>
            <Instance Name="ah" Ord="4" B="0" X="0" R="0" W="0" Class="reg8,high"/>
            <Instance Name="ax" Ord="0" B="0" X="0" R="0" W="0" Class="reg16"/>
            <Instance Name="eax" Ord="0" B="0" X="0" R="0" W="0" Class="base32, index32, reg32"/>
            <Instance Name="rax" Ord="0" B="0" X="0" R="0" W="8" Class="base64, index64, reg64"/>
        </Register>
        <!-- more register definitions -->
        <DoubleReg Name="edx:eax"/>
        <!-- more double register definitions -->
        <Address Name="['base:base32']" Class="rm,frm,rm8,rm16,rm32,rm64,mmrm,sserm,rm32bit,rm64bit" Coding="'addr32' *'mand' 0x40+'W'+'R'+'base.B':8 'op' 0:2 'mod':3 'base.Ord':3"/>
        <!-- more addressing modes -->
        <Opcode Name="fnstenv">
            <Operands Name="'rm:rm'" op="0xd9:8" mod="6" R="0" W="0"/>
            <!-- more operand definitions -->
        </Opcode>
        <!-- more opcode definitions -->
    </Coding>
    <Prefix Name="lock" Coding="0xf0:8"/>
    <!-- more prefix definitions -->
```

This is an incomplete example with some statements taken from the x64 ADL file.   There are a number of use cases not shown here.

#### Basic concepts

There are a number of basic concepts which are used in various places within the ADL.   These include:

* Conditionals       allows different uses of an entity depending on a specified condition
* Classes            allows linkage of one entity to another
* Variables          allows accessing a register, number, or address mode that is specified in the source code
* Coding sequences   allows specifying the bit stream that makes up the instruction bytes
* Address names      allows mapping a token sequence to an address mode

Some of these can be seen in previous examples.   The next sections will elaborate on them.

##### Conditionals

A condition is denoted by an XML attributed `Cond`.   It can be used in either a number or a coding state.

For example in the x64 ADL file the following `Number` node is only matched if the processor is in 16 bit mode:

```xml
    <Number Class="rel16" relOfs="2">
      <Instance Value="-$#:16" Cond="'processorbits'==16"/>
    </Number>
```

here `processorbits` was already defined as a state variable:

```xml
    <StateVars>
        <Instance  Name="processorbits" Init="16"/>
    </StateVars>
```

and the quotes around `processorbits` indicate it should be looked up in the state variables table.

The matching is only done because a Cond attribute exists and compares a processor bits variable with 16.   These conditional expression will be directly embedded in the C++ code so they can reference anything a C++ expression can reference.

The following `State` node encodes an x64 data size prefix into the coding sequence, if the processor is not in 16-bit mode.

```xml
    <State Name="data16">
      <Value Cond="'processorbits'==16" Coding=""/>
      <Value Cond="'processorbits'!=16" Coding="0x66:8"/>
    </State>
```

##### Classes

`Numbers`, `Registers`, `Address Modes`, and `Opcodes` may have classes specified as an attribute.

* `Number` classes are consumed by address modes or operands and are used to correlate a number in ASCII text to its coding sequence.

* `Register` classes are also consumed by address modes; they group various registers togther e.g. if their different ordinals are often used in the same contexts there might be a class linking them.   They are generally used to generate a token sequence that can parse one of several register names and come up with a unique ordinal.   The register classes can also be used to differentiate between different uses of the same ordinals.

* `Address mode` classes are consumed by opcodes and are used to group multiple address modes together.   Between a token sequence specified in the address mode and the class specification, a unique coding sequence can be derived.   Note that this means two Address specifications could differ only in class to allow different codings for the same input token sequence.

* `Opcode` classes are consumed by other opcodes.  They are used to group different combinations of address mode groupings together to find the coding sequence for the various address mode groupings that could be accessed by a single mnemonic.

for example in the number

```xml
    <Number Class="rel16" relOfs="2">
      <Instance Value="-$#:16" Cond="'processorbits'==16"/>
    </Number>
```

The class `rel16` can be referenced as part of the source code representation in a way that will be defined in a subsequent section.  e.g. to link a numeric type in the source code to an Addressing mode.

Registers have a similar specification:

```xml
    <Register Class="general">
      <Instance Name="al" Ord="0" B="0" X="0" R="0" W="0" Class="reg8,low"/>
      <!-- more aliases for this register -->
    </Register>
```

here the class on the `register` node is ignored at present; it is the class on the `instance` node that matters.  

We see that things can have more than one class.   For example, different addressing modes might reference different groups of registers.

For addressing modes we might have something like:

```xml
    <Address Name="['mem:address16']" Class="rmx,rm,frm,rm8,rm16,rm32,rm16it" Coding="'addr16' *'mand' 0x40:8 'op' 0:2 'mod':3 6:3 'mem':16"/>
```
    
Here we again have multiple classes associated with the addressing mode.   The reason for doing this is their may be variations on an addressing mode, for example on the x64 some instructions allow a full range of addressing modes, but other instructions allow only an abbreviated amount from the full set.   By assocating one class with the full range and another with a more restriced range, the abbreviation can be achieved without respecifying all the address modes in question.   That is, assuming the values that are there in the abbreviated set are encoded exactly the same as those in the full set.

The addressing mode example also shows how classes are consumed.   Here the `Name` field specifies the source-code token sequence that results in the encoding. For example: `address16` was defined as a class on a `Number` like this:

```xml
    <Number Class="address16">
      <Instance Value="$#:16" Cond="'processorbits'==16"/>
    </Number>
```

So the address mode in question is referencing a 16-bit address constant.

More will be said about this in the next section.


##### Variables

There are four types of variables.

* State variables allow examination of assembly state
* Coding State variables allow inserting some constant value in a coding sequence, usually based on some conditional value which is based on a state variable
* Attribute variables are constants that are selected based on the choice of opcode or register
* Address-related variables hold things like register values or numbers that are parsed out of the address mode portion of the input stream.

Variables are typically accessed by enclosing their name in quotes.    For example to access a state variable called `processorbits` use `'processorbits'`

```xml
    <StateVars>
        <Instance  Name="processorbits" Init="16"/>
    </StateVars>
    <Number Class="rel16" relOfs="2">
      <Instance Value="-$#:16" Cond="'processorbits'==16"/>
    </Number>
```

State variables are generally used with `Cond` attributes.   State variables will be detailed further in a future section.

Coding state variables are used in `Coding` attributes.   They specify a constant value that may depend on the value of a state variable, which is streamed into the opcode coding sequence. These will also be detailed further in a future section.

Attribute variables are XML attributes with arbitrary names, that can be referenced from within a contained coding sequence.   For example a common group of addressing modes on the x64 processor always generates roughly the same coding sequence for some interrelated instructions with the same operands.  The exception being that three bits in the middle of the sequence are related to the specific instruction being selected.   An attribute variable related to the mnemonic can specify those three bits.  Another use for an attribute variable is to specify some parameter based on a selected register name.

For example:

```xml
        <Opcode Name="fnstenv">
            <Operands Name="'rmval:rm'" op="0xd9:8" mod="6" R="0" W="0"/>
        </Opcode>
```

here `op` is used to specify the first byte of the coding sequence, and `mod` is the middle three bits of the address mode.   `R` and `W` are further attribute variables. A coding in an address mode associated with the `rm` class will utilize these values to customize the output coding sequence.   Again, these names are arbitrary, they could just as well be "`tree`" and "`moss`".

Address-related variables appear in the `Name` field of an `Operand` or `Address` entity.  This field relates directly to source code, and is populated by specifics of what is parsed.  Such variables are in two parts, the name of a variable to be used in coding sequences, and a class to associate with the address mode.   The class can indicate either another address mode, or a register.

In the above example `'rmval:rm'` references a variable `rmval` which has an address mode class of `rm`.

Variables defined at a higher scope bleed down into whichever coding sequence is selected.   For example an attribute variable can be selected in a coding attribute locally, in a coding attribute within either the opcode or operands of a class-based opcode, or a coding attribute within an address mode.

##### Coding sequencea

Coding sequences specify the sequence of bits that will be generated for a given address mode.   They often depend on coding state variables, address-related variables, and attribute variables to fill in parts of the coding sequence which change based on assembler state, opcode, or operands.

A coding sequence can be located in one of three places.   First, within the current operand node within the selected opcode node.   If there is no coding sequence there, a coding sequence might be found within a contained class-based opcode.   If there is no such opcode or there is no coding sequence within it, a coding sequence will be found in the associated address mode.

A coding sequence may exist in an opcode node, but then reuse the encoding of an address mode as a sub-part of the actual encoding.   This is for the case where normal address modes are augmented in some way for some specific instructions.

A coding attribute holds a coding sequence.   For example:

```xml
    <Address Name="['mem:address16' + 'reg:reg16']" Class="rmx,rm,frm,rm8,rm16,rm32,rm16it" Coding="'addr16' *'mand' 0x40:8 'op' ~3:2 'mod':3 'reg.Ord':3 'mem':16"/>
```

Individual parts of the sequence or codings are in two parts, the value, and the number of bits to be encoded.   These parts are separated by a : character.

The simplest of codings is a constant.   For example `0x40:8` specifies an encoded constant of 40 hex, encoded into 8 bits.   Constants can also be in decimal, simply use a number without the `0x` hex specifier to do so.

Another simple type of coding is a variable reference.   For example the coding `'mod':3`  uses the variable `mod`, encoded into three bits.

Codings may inherit a bit size from a variable definition.   For example for the coding `'op'`, the x64 ADL file usually specifies that op is an 8-bit constant, for example `'0xd9:8'`.

Preceding the name of variable codings by `*` means the coding is optional.   For example the `mand` variable is defined in a few opcodes but not in other opcodes; when it is not defined nothing will be emitted.

The `.` operand accesses an attribute variable associated with a register.   In the above example assuming reg16 is the class associated with a register, the coding `'reg.Ord':3` takes the selected register and encodes the value of its `Ord` attribute into 3 bits.

Codings may contain the following unary operands:

`- ~`

where `~` is a unary complement and `-` is a unary minus,

or the following binary operands:

`+ - & | ^ < > `.

where for example `~3:2` encodes `0` into two bits and `3+3:3` encodes `6` into three bits.   Here `<` is left shift and `>` is right shift.



Two keywords may be used within an encoding.

The `native` keyword indicates that a lower level coding should be applied.   For example:

```xml
    <Opcode Name="add">
        <Operands Name="rmval:rm" Coding = "0x22:8 native 0x22:8"/>
    </Opcode>   
```

encodes the native encoding for the rm addressing mode between two 0x22 sentinels.

The `illegal` keyword indicates that this coding is illegal, no opcode will be emitted and the assembler should display an error.   This couild be used to flag that an address mode that would be valid for one opcode, is not valid for another addressing mode depending on any amount of information.

For example in the state variable:

```xml
    <State Name="no64">
      <Value Cond="'processorbits'==64" Coding="illegal"/>
      <Value Cond="'processorbits'!=64" Coding=""/>
    </State>
``` 

any `Coding` that references this state variable when `processorbits == 64` becomes illegal because of the reference to `illegal`.

##### Address Names

The `Name` attribute of an `Address` node indicates the mapping from text as found in the source code.   As an example:

```xml
    <Address Name="[ds:bx+'index:index16' + 'signed:signed8offset']" Class="rmx,rm,frm,rm8,rm16,rm32,rm16it" Coding = "'addr16' *'mand' 0x40:8 'op' 1:2 'mod':3 0:2 'index.index':1 'signed':8"/>
```

Here the mapping is `[ds:bx+'index:index16' + 'signed:signed8offset']`.   In this example everything outside the single quote sequences is taken literally, and must exist in the source code.   In this case the values between the quotes are address-related variables, e.g. `index` is a variable name and `index16` is a register class.   `signed` is a variable name and `signed8offset` is a number class.   

Note that there is no requirement that a specific class type be used in a specific place, those just happen to be the class types specified in this example.

Name attributes can also have optional text.   For example we can modify the previous example slightly:

```xml
    <Operand Name="{dword} [ds:bx+'index:index16' + 'signed:signed8offset']" Coding = "'addr16' *'mand' 0x40:8 'op' 1:2 'mod':3 0:2 'index.index':1 'signed':8"/>
```

and now the braces indicate there may optionally be a word `dword` before the rest of the operand.   Note there is currently no way of escaping the brace...

#### Number formats

There are several formats for numbers.   The first is a literal value; the second is a group of enumerations, the next is a signed or unsigned number, and the last one is a relative quantity (relative to the current code position)

The number tag gives a class name, which may be used in addressing modes to select the number.   For example assuming `addr16` is the class of a 16 bit number:

```xml
    <Number Class="addr16">
      <Instance Value="$#:16" Cond="'processorbits'==16"/>
    </Number>
```

we might have an address field that references it as follows

```xml
    <Address Name="'val:addr16'" Class="rm" Coding="0x40:8 'val':16"/>
```

Here the address-related variable in the name field gives the class name of the number type to use.

Numeric formats map a textual value to a numeric value.   The `Number` node contains an `instance` node containing a `Value` attribute.   It is the `Value` attribute that holds the mapping for the number.

Here the textual value `0` is being mapped to `0`.   The value before the `;` is the textual value; the value after it is the numeric value that it will be mapped to.

```xml
    <Number Class="zero">
      <Instance Value="0;0"/>
    </Number>
```

Here we are mapping a scale factor into a compact form used in instructions.   The compact form happens to be the log to the base 2 of the textual form.

```xml
    <Number Class="scale">
      <Instance Value="1;0"/>
      <Instance Value="2;1"/>
      <Instance Value="4;2"/>
      <Instance Value="8;3"/>
    </Number>
```

here the number 8 present in the source line at the location 'scale' is accessed, is converted to a binary 3.

In the example given at the start of this section:

```xml
    <Number Class="addr16">
      <Instance Value="$#:16" Cond="'processorbits'==16"/>
    </Number>
```

We are mapping a number with class `addr16` to a mapping of `$#:16`.   Additionally, a `Cond` attribute specifies that the assembler must be in 16 bit mode for this kind of value to match the input stream.

The $#:16 tells several things about the number.   First, the `$` means it can match relocatable quantities (labels).   The `#` means parse out a number.  The `:16` indicates the number of bits to stream for the coding.

Note that the number matcher functions are not generated by the ADL compiler;   they are meant to be a hand-written addition to take into account the vagaries of numbers in whichever assembler is being developed.   The current number matcher in OASM only handles numbers up to 32 bits.

The final example is a relative offset:

```xml
    <Number Class="rel16" relOfs="2">
      <Instance Value="-$#:16" Cond="'processorbits'==16"/>
    </Number>
```

This calculates a relative offset between two points in the output stream.   This particular example is for 16-bit relative branches on the x64.   In this case the program counter is two bytes before the end of the instruction when the calculation is performed, and the processor takes the relative offset from the end of the instruction.

The relative offset is calculated as follows:   

> `relative offset valiue = targetvalue-(programcounter+relofs)`;

so that the `relOfs` value of two specified in the number means calculate the relative offset from the position in the code that is immediately after the entire instruction coding, including the offset, rather than from the middle of the instruction.

Here the `-` in the value attribute indicates this number is signed.   Leaving it out as in previous examples, means the number is unsigned.


#### State variables group

State variables are variables that are visible to external code.   The assembler may set them based on its internal state, for example to determine word size.

For example:

```xml
        <StateVars>
            <Instance  Name="processorbits" Init="16"/>
        </StateVars>
```

Here the `Instance` node defines a state variable.   The `Name` attribute gives the name of the variable (for purposes of the ADL file) and the `Init` attribute gives it an initial value.   State variables are always 32 bit integers.


#### Coding states group

Coding States specify individual codings that may vary based for example on a processor mode.   Typically, a `Cond` attribute will be present to indicate what the coding should be.

In the following example:


```xml
    <State Name="only64">
      <Value Cond="'processorbits'!=64" Coding="illegal"/>
      <Value Cond="'processorbits'==64" Coding=""/>
    </State>
```

here the `Name` field of the `State` node gives the name of the state variable, to be used in a coding.

The `Value` nodes give a coding which is based on a condition.   The name of the state would be used in a coding sequence to select this coding.

In this case if the state variable named  `'processorbits'` is equal to 64, then nothing is emitted.   Otherwise the `illegal` coding is emitted, which results in an error.

In the following example:

```xml
    <State Name="rex">
      <Value Cond="true" Coding="0x4:4"/>
    </State>
```

when the coding state variable `rex` appears in a coding a value of four will be emitted as a four-bit coding sequence.

The use would be similar to:

```xml
    <Address Name="eax" class="eax" Coding="'only64' 'rex' 'op':8"/>
```


where the coding state variables are as usual enclosed in single quotes.

#### Register definitions group

Register definitions map the relationship between a textual register name, and an ordinal value.   One or more class values may be used to distinguish between multiple register names with the same ordinal.   For example in the following:

```xml
    <Register Class="general">
      <Instance Name="al" Ord="0" B="0" X="0" R="0" W="0" Class="reg8,low"/>
      <Instance Name="ax" Ord="0" B="0" X="0" R="0" W="0" Class="reg16"/>
      <Instance Name="eax" Ord="0" B="0" X="0" R="0" W="0" Class="base32, index32, reg32"/>
      <Instance Name="rax" Ord="0" B="0" X="0" R="0" W="8" Class="base64, index64, reg64"/>
      <Instance Name="r8b" Ord="0" B="1" X="2" R="4" W="0" Class="reg8,low"/>
      <Instance Name="r8w" Ord="0" B="1" X="2" R="4" W="0" Class="reg16"/>
      <Instance Name="r8d" Ord="0" B="1" X="2" R="4" W="0" Class="base32, index32, reg32"/>
      <Instance Name="r8" Ord="0" B="1" X="2" R="4" W="8" Class="base64, index64, reg64"/>
    </Register>
```

there are various declarations of two different registers, `rax` and `r8`.    They differ in register width but have the same ordinal value.   The class specification of each gives class names that can be referenced from the `Name` attribute of an addressing mode or operand.

The `Class` attribute within the Register is currently unused, but should be chosen to group registers in some way as it will eventually be used when the `Compiling` entity is supported.   On the x64 the grouping is done based on which segment register is related to the register, when it is used for indexing.

In each `Instance` node, the required attributes are:

* `Name` - the textual representation of the register
* `Class` - one or more class names for this register

In this example the `Ord`, `B`, `X`, `R`, and `W` attributes are attribute variables associated with the register.

As another example,

```xml
    <Register Class="general">
      <Instance Name="cl" Ord="1" B="0" X="0" R="0" W="0" Class="reg8,low"/>
      <Instance Name="cx" Ord="1" B="0" X="0" R="0" W="0" Class="reg16"/>
      <Instance Name="ecx" Ord="1" B="0" X="0" R="0" W="0" Class="base32, index32, reg32"/>
      <Instance Name="rcx" Ord="1" B="0" X="0" R="0" W="8" Class="base64, index64, reg64"/>
      <Instance Name="r9b" Ord="1" B="1" X="2" R="4" W="0" Class="reg8,low"/>
      <Instance Name="r9w" Ord="1" B="1" X="2" R="4" W="0" Class="reg16"/>
      <Instance Name="r9d" Ord="1" B="1" X="2" R="4" W="0" Class="base32, index32, reg32"/>
      <Instance Name="r9" Ord="1" B="1" X="2" R="4" W="8" Class="base64, index64, reg64"/>
    </Register>
```
this is pretty much the same as the original example, except it uses `rcx` and `r9` instead of `rax` and `r8`.   The 
ordinal has changed correspondingly.

Note that the class name of `general` is also the same as before, since these registers are in a related group.  A similar example for the stack pointer shows the `Class` name on the `Register` node changing:

```xml
    <Register Class="stack">
      <Instance Name="spl" Ord="4" B="0" X="0" R="0" W="0" Class="reg8t,low"/>
      <Instance Name="sp" Ord="4" B="0" X="0" R="0" W="0" Class="reg16"/>
      <Instance Name="esp" Ord="4" B="0" X="0" R="0" W="0" Class="base32, reg32"/>
      <Instance Name="rsp" Ord="4" B="0" X="0" R="0" W="8" Class="base64, reg64"/>
      <Instance Name="r12b" Ord="4" B="1" X="2" R="4" W="0" Class="reg8,low"/>
      <Instance Name="r12w" Ord="4" B="1" X="2" R="4" W="0" Class="reg16"/>
      <Instance Name="r12d" Ord="4" B="1" X="2" R="4" W="0" Class="base32, index32, reg32"/>
      <Instance Name="r12" Ord="4" B="1" X="2" R="4" W="8" Class="base64, index64, reg64"/>
    </Register>
```

because we are basing the classes on how the indexing works on the x64, and there is a subtle difference between indexing based on a stack register and indexing based on a more general register.

A register may be consumed by an addressing mode or operand, for example:

```xml
    <Address Name="'reg:reg8'" Class="rm8, reg8, reg8rm" Coding="*'mand' 0x40+'reg.B'+'R'+'W':8 'op' 3:2 'mod':3 'reg.Ord':3"/>
```

Here the name field shows a reference to address-related variable class `reg8`.   Since `reg` is of class type `reg8` if the name is `cl` it would match the `Name` attribute.   Then in the coding, the statement `'reg.Ord':3` would take the `Ord` attribute value from the `Instance` definition of `cl`, e.g. the result would be to encode the value 1 into 3 bits.

#### Addressing Modes group

Addressing modes are the core of the parser.   They correlate either the textual value for a register, or a combination of registers, numbers, and addressing modes, to a coding sequence.

In this example:

```xml
    <Address Name="'reg:reg8'" Class="rm8, reg8, reg8rm" Coding="*'mand' 0x40+'reg.B'+'R'+'W':8 'op' 3:2 'mod':3 'reg.Ord':3"/>
```

the `Name` attribute gives a rendition of the text that will match the input text.   Here it is an address related variable named `reg` which has a class `reg8`.   As in other examples, `reg8` is the class on the 8-bit register specifications.   So in the x64 adl file this matches the name of an 8-bit register.

The class names here could be used in any other `Address` specification.   For example:

```xml
    <Address Name="'rm:rm8'" Class="rm" Coding= ...
```

can match the token sequence for any address mode with a class `rm8`.   Including the one in the first example.  The coding sequence on the matched mode will reference any variables found in the matched address mode or possibly coding state variables.

The coding sequence `Coding="*'mand' 0x40+'reg.B'+'R'+'W':8 'op' 3:2 'mod':3 'reg.Ord':3"` in the first example optionally encodes an attribute variable called `mand` if it exists.   Then it takes the register variable and encodes the sum of its `B` attribute with the `R` and `W` attributes given at an enclosing scope, e.g. an Opcode or Operand node is expected to specify these.   That is streamed as an 8-bit quantity.   Then the `op` attribute is streamed (its size is also specified at a higher level)  followed by the number 3 in two bits, the mod attribute in 3 bits, and the `Ord` attribute on the register in 3 bits.   

Note that the sum of the number of bits is expected to be a multiple of 8 because this processor has instructions that are byte-oriented.   So the codings in all address modes must be arranged to stream exactly a multiple of 8 bits.  On other processors including risc processors the titak number of bits in an instruction may need to be a multiple of 16 or even 32, because instructions are a multiple of this size. In this adl file, `mand` has a size attribute of 8 bits, as does `op`.

In a final example:

```xml
    <Address Name="[ds:'base:base32'+'index:index32'*'sc:scale']" Class="rm,frm,rm8,rm16,rm32,rm64,mmrm,sserm,rm32bit,rm64bit" Coding="'addr32' *'mand' 0x40+'W'+'R'+'index.X'+'base.B':8 'op' 0:2 'mod':3 0x4:3 'sc':2 'index.Ord':3 'base.Ord':3"/>
```

we have a complex `Name` attribute.   Everything outside single quotes is literal text.   Then we have `'base:base32'` which is an address-related variable that happens to be for a register specification, `'index:index32'` which is another address-related variable for a register specification, and `'sc:scale'` which happens to be an address-related variable for a number specification.   This might match an x64 addressing mode such as:

`[ds:eax+esi*4]` or `[ds:ebp + eax*2]`.

If one wanted to leave out the scale factor instead of specifying `*1`, they would have to write a separate address mode specification for it.

#### Opcodes

`Opcode` nodes are the primary method of associating an instruction mnemonic and a set of operands, to addressing modes and thus codings.

There are two ways of writing opcode nodes; the class-based one associates a set of `Operand` nodes that form the allowed addressing modes for a group of mnemonics.   In this case the referencing opcodes use the class name of the class-based opcode as kind of a 'subroutine' in each mnemonic specification, filling in some attribute-based variables for the parts which would be specific to the mnemonic.

Otherwise in the case where an `Opcode` doesn't share operands with other instructions, the `Opcode` node can directly have `Operand` nodes that tell how to perform the translation.

For a simple example:

```xml
    <Opcode Name="cmpxchg8b">
      <Operands Name="'rm:rm'" mod="1" op="0x0f:8 0xc7:8" R="0" W="0"/>
    </Opcode>
```

Here we are defining an opcode with `Name="cmpxchg8b"`.   The `Name` gives the mnemonic of the opcode in text format.

It has one operand combination which has a `Name` that is a variable named `rm` with a class name of `rm`.   In the source code, this will match any address mode with a class name of `rm`.   

One could also match registers or numbers, but in this case that would be useless because this operand definition does not specify a coding and there isn't an explicit usable coding associated with the operands.  It might make sense though if the coding were specified directly in the operand.

This operand defines several attribute variables to be consumed by the coding sequence of the referenced address mode.   In particular, note that in this case the `op` variable specifies a coding which is two bytes long.

This definition would not be directly shared by other mnemonics since it doesn't reference a class-based operand.

As another simple example:

```xml
    <Opcode Name="cmpsq">
          <Operands Name="empty" Coding="0x48:8 'data64' 0xa7:8"/>
    </Opcode>
```

here the `empty` keyword means that there are no operands for this opcode.    The coding is directly specified in the operand in this case.

we could just as easily have written:

```xml
    <Opcode Name="cmpsq" op="0xa7:8">
          <Operands Name="empty" Coding="0x48:8 'data64' 'op'"/>
    </Opcode>
```

if for some reason we wanted to cut and past the operand specification.    But usually one would just use a class-based operand for that.

As a simple example of a class-based operand we might have:

```xml
    <Opcode Class="single">
      <Operands Name="empty" Coding="0x40:8 'op'"/>
    </Opcode>
```
This again has the `empty` keyword as the operand name, meaning there are no operands for these kinds of mnemonics.   

The fact that the opcode node has a class but no name means it can be used by multiple mnemonics.   Here the coding indicates that there will be an eight-bit prefix of 0x40, followed by the codings for the attribute variable `op` which is specified in the original opcode.   It might "called" as follows:

```xml
    <Opcode Name="aaa" op="'no64' 0x37:8" Class="single"/>
```

here the presence of both a mnemonic and a class name means this is a mnemonic definition, that references the common operands found in the class `single`.   But it specifies an attribute based variable `op` which is the specific opcode that getss coded in the class based operands.

Either type of opcode may have multiple allowed operands:

```xml
    <Opcode Name="cmpxchg" op1="0x0f:8 0xb0:8">
      <Operands Name="{byte} 'rm:rm8', 'modreg:reg8'" op="'op1'" mod="'modreg.Ord'" R="'modreg.R'" W="'modreg.W'"/>
      <Operands Name="{byte} 'rm:rm8', 'modreg:reg8t'" mand="0x40:8" op="'op1'" mod="'modreg.Ord'" R="'modreg.R'" W="'modreg.W'"/>
      <Operands Name="{word} 'rm:rm16', 'modreg:reg16'" op="'data16' 'op1'+1:8" mod="'modreg.Ord'" R="'modreg.R'" W="'modreg.W'"/>
      <Operands Name="{dword} 'rm:rm32', 'modreg:reg32'" op="'data32' 'op1'+1:8" mod="'modreg.Ord'" R="'modreg.R'" W="'modreg.W'"/>
      <Operands Name="{qword} 'rm:rm64', 'modreg:reg64'" op="'data64' 'op1'+1:8" mod="'modreg.Ord'" R="'modreg.R'" W="'modreg.W'"/>
    </Opcode
```

where again values enclosed in `{}` are optional literals and other characters outside the `''` pairs are literal.

In the following Operand node:

```xml
      <Operands Name="byte 'rm:rm8',  {byte} 'omem:mem8'" op="0x80:8" R="0" W="0" Coding="native 'omem':8"/>
```

the class rm8 already has a coding sequence as an addressing mode.   The coding sequence in this operand node overrides that encoding, but since we want to reuse the original coding after all we say `native`.   Then for purposes of the referenced instruction we encode a constant value after the original encoding.

#### Prefixes group

Prefix nodes specify prefixes that can precede instructions.   For example on the x64 the `rep` prefix tells a string instruction to repeat until a counter reaches zero:

```
    rep movsb
```
        
The `rep` prefix might be specified with the following in the ADL file.

```xml
    <Prefix Name="rep" Coding="0xf3:8"/>
```

It is still up to user code to parse out the token but the generated code for the parser will at least stick it in a handy hash-table and also process discovered prefixes while creating coding sequences.

#### Parsing order

The operands nodes within an opcode node can be located uniquely by looking up the mnemonic, and then are processed sequentially within the list of operands.   Internally, the operands are arranged in a tree and the leafs are traversed either until something is found or until the tree runs out of leaves.    There is special code to deal with the ambiguity involved in optional tokens.

The order they appear in is important, since the sequential processing means that if a match is found, subsequent operands will be ignored.   Some of the things to look out for when generating an opcode table are as follows:

* explicit registers
* numbers of different widths
* optional quantities

If an explicit register is used, it should appear in the operand list before any usage of that register by class.   For example if there is a register class reg8 which encompasses all possible 8-bit registers, one would write an operand that uses that register class.   If one then wants to override that operand for a register with a specific name to use for example a more compact encoding, one should write a similar rule which encompasses only that register and place it before the original rule.

For example, on the x64 there are registers `al`,`cl`,`dl`,`bl` which are part of a register class `reg8`.   But sometimes it is possible to have a more compact representation for register `al` using some addressing modes.    A rule would be written for the register class `reg8` and another rule would be written for register al, then the one for register al would appear before the rule for register class `reg8` so that it would be matched first.

If two operands differ only by numbers that have different widths, it is important that the number with the lesser width appear first.   This is because for small numbers, both operands will match the number but one would want the smaller instruction coding to take precedence for such numbers.

When there is an optional quantity in an operand, the operand will be processed whether or not the optional quantity is present.   So if two operands only differ in an optional quantity, the one that is desired to be the default should come first.

### Compiling entity

The `Compiling` entity isn't fully fleshed out or otherwise supported yet.   As such it won't be documented here, other than to say there may be multiple profiles for example one could use a different profile for different bit widths.

## Code level interface

The ADL compiler generates C++ code which can be interfaced to an external tool, e.g. a compiler or assembler.   There is some glue logic which is necessary to tie the generated code into whatever compiler or assembler is being used.   This section describes the interface.

Note that the OASM assembler with Orange C already implements this interface (as does the Orange C compiler), although there are some customizations that would have to be performed for 64 bit numbers or to stream big endian numbers properly.

This section is split into two sections, `Support Code` details necessary code and headers that must be provided for the ADL-generated code to work, and `Calling Interface` specifies how to set up the arguments to successfully call the parser and get a byte-code sequence back.

### Support code

#### Generated files

The ADL compiler generates the following files:

* `AdlStructures.h`      - structures used for interfacing to the adl generated parser   
* `AdlFunctions.cpp`     - generic functions used with the parser
* `xxxxOperand.h`        - custom structure for storing operands
* `xxxxParser.h`         - header for the parsing state machine and parsing related functions
* `xxxxParser.cpp`       - the parsing state machine and parsing related functions
* `xxxxInstructions.h`   - enumerations for opcodes and tokens
* `xxxxInstructions.cpp` - text for opcodes and tokens, InputToken definitions for tokens

where `xxxx` is replaced by the processor name given at the beginning of the adl file.  In our example these would be:

`x64Operand.h`
`x64Parser.h`
`x64Parser.cpp`
`x64Instructions.h`
`x64Instructions.cpp`

These don't form a complete parser; as indicated earlier there must be glue code to transform the opcodes and operands prior to passing them to the parser.  And then the output of the parser must be tied into the surrounding code.

The parser defines a parsing structure called `xxxxParser` (x64Parser in our example) that inherits from a user-provided class `InstructionParser`.  The inheriting is performed in the manner to take care of the case where a single assembler program might use the same parsing infrastructure with different adl-generated parsers to successfully parse multiple assembly language.   

The `InstructionParser` class is usually defined in the file pair `InstructionParser.cpp` and `InstructionParser.h`.   An additional user-prepared file `xxxxStub.cpp` can be used to hold miscellaneous functions that aren't directly related to parsing instructions.


#### InstructionParser class

A minimal implementation of `InstructionParser.h` might look something like this:

```c++
// include various generated structures
#include "AdlStructures.h"
// include other stuff needed by this file


// an assembler-specific definition of a section/segment, used in headers to provide data for processing
class Section;
// an assembler-specific definition of a file, used in headers to provide data for processing.   Maybe other things would be
// accessible from here, such as a tokenizer to tokenize the input stream.
class AsmFile;

class InstructionParser
{
  public:
    InstructionParser()
    {
    }

    // instantiates the xxxxParser class.   
    // usually defined in xxxxStub.cpp
    static InstructionParser* GetInstance();

    // set a default assembler state based on something like command line switches, e.g. processor bits or other state variables
    // usually defined in xxxxStub.cpp
    virtual void Setup(Section* sect) = 0;
    
    // Initialize the xxxxParser instance.   
    // Defined by the code generator in xxxParser.cpp
    virtual void Init() = 0;
    // parse out the arguments for a section, e.g. bit size or other desired states that can be set in a section definition
    // usually defined in xxxxStub.cpp
    virtual bool ParseSection(AsmFile* fil, Section* sect) = 0;
    // generic routine to parse out any directives that are related to state variables
    // usually defined in xxxxStub.cpp
    virtual bool ParseDirective(AsmFile* fil, Section* sect) = 0;
    // can be used to parse attributes that come before an instruction, e.g. for gas compatibility
    // usually defined in xxxxStub.cpp
    virtual std::string ParsePreInstruction(const std::string& op, bool doParse);
    // returns the big endian flag from the ADL file
    // Defined by the code generator in xxxParser.h
    virtual bool IsBigEndian() = 0;
    
    // replace the numeric token at tokenpos that has an integer value oldval (if it exists) with a numeric token holding newval
    // usually defined in InstructionParser.cpp
    bool SetNumber(int tokenPos, int oldVal, int newVal);

  protected:
    // check the numeric value at tokenpos against the parameters as specified in the number class definition from the ADL file.
    // set up a structure in the 'numeric' slot below and return true if it matches
    // otherwise return false.
    // usually defined in InstructionParser.cpp
    bool ParseNumber(int relOfs, int sign, int bits, int needConstant, int tokenPos);
    
    // main dispatcher for the parser.   Some fields below have to be set up properly for it to work.
    // defined by the code generator in xParser.cpp
    virtual asmError DispatchOpcode(int opcode) = 0;
    // hash table, mapping between token names and token values
    // defined by the code generator in xParser.cpp
    std::unordered_map<std::string, int> tokenTable;
    // hash table, mapping between opcode names and opcode values
    // defined by the code generator in xParser.cpp
    std::unordered_map<std::string, int> opcodeTable;
    // hash table mapping for any instruction prefixes such as the 'rep' prefixes in the x64
    // defined by the code generator in xParser.cpp
    std::unordered_map<std::string, int> prefixTable;
    
    // numeric operands returned from the parser
    // generally used to figure out where to put things like fixups and how they relate to the input stream
    // filled in by the call to DispatchOpcode()
    std::list<Numeric*> operands;
    // coding structures that have to be cleaned up at some point
    // filled in by the call to DispatchOpcode()    
    std::list<Coding*> CleanupValues;
    // prefix values parsed out of the input stream.
    // Dispatch will place them before the rest of the instruction when streaming the coding
    std::list<int> prefixes;
    // input tokens.   Parsing the input stream results in this list.
    // The SetNumber and ParseNumber routines, above, reference this list with the 'tokenPos' parameter
    // must be filled in before the call to DispatchOpcode()
    std::vector<InputToken*> inputTokens;
    // the coding values get streamed into this structure
    // after the call to DispatchOpcode(), this will hold the byte stream streamed by whichever coding was chosen
    // the 'operands' variable will have more information about how to place fixups within this stream
    BitStream bits;
    // tbe SetNumber and ParseNumber routines will leave a newly constructed 'Numeric' structure here,
    // it will be put in the 'operands' array for use in handling fixups and so forth
    Numeric* numeric;
    // Set to true by DispatchOpcode(), if all the tokens in the inputTokens vector have been consumed.
    bool eol;
};
```

A further function defined in `x64Parser.h` needs to be defined as well:

```c++
// Perform a math function, as used in a coding.   E.g unary or binary function with one or two operands.
// usually defined in x64Stub.cpp
int x64Parser::DoMath(char op, int left, int right);
```

#### Auxilliary structures

Several auxilliary structures are defined in `AdlStructures.h`.   They are as follows:

`AdlExprNode`      - base class for user-defined expressions
`BitStream`        - codings are streamed into an instance of this class.   After `DispatchOpcode()` returns will have the byte stream.
`Coding`           - internal structure used to store pieces of codings, only used in generated code
`Numeric`          - a description of a number.   Can be used e.g. to determine fixups
`InputToken`       - one of the incoming tokens.   Set up prior to calling `DispatchOpcode()`

`AdlStructures.h` also defines an enumeration `asmError` for return codes from the Dispatch function.

`AdlExprNode` has one member of interest:

```c++
    // stores a register or token value.   Can also be used to store other things when used as a base class.
    long long ival;
```
`AdlExprNodes` are stored in `Numeric` and `InputToken` instances, and can form the base class for some broader expression class.

`BitStream` is the destination for codings and has three members of interest - 

```c++
    // Call to empty the stream, e.g. before DispatchOpcode();
    void Reset();
    // Call to get the number of bits that have been streamed.
    int GetBits();
    // call to get the streamed bytes into array dest with maximum length size
    void GetBytes(unsigned char* dest, int size);
```
`Numeric` is filled in during parsing and has five members of interest:

```c++
    // the original expression, probably a derived class.
    // can be used to determine labels and so forth for fixups.
    AdlExprNode* node;
    // position within the instruction that the operand appears (in bits)
    int pos = 0;
    // relative offset used for calculating constant values for relative branches
    // specified in the adl file
    // offset from the expressed position, in bytes
    int relOfs = 0;
    // size of the number in bits
    int size = 0;
    // true if the number was actually used by the parser.
    int used = 0;

```
`Coding` is used internally, and isn't of general interest.

`InputToken` is filled in before calling Dispatch and has two members of interest:

```c++
    // the token type.
    enum
    {
        TOKEN,
        REGISTER,
        NUMBER,
        LABEL
    } type;
    // an expression related to the token.   
    AdlExprNode* val;
```

#### State variable setters and getters

To ease the access of state variables, the adl compiler generates setters and getters for them in x64Parser.h.    They are named similarly to the statevariable with a `Set` or `Get` prefix and operate on integers.

As an example consider the `processorbits` state variable we have been using throughout this text.   The setters and getter will be defined as follows:

```c++
    void Setprocessorbits(int v) { ...; }
    int Getprocessorbits() { return ...; }
```

### Calling interface

To parse code, the function Dispatch is called:

```c++
    asmError DispatchOpcode(int opcode);
```

This function takes one of the opcode enumerations, and returns one of the `asmError` enumerations.   To get one of the opcode enumerations, one can parse out the opcode mnemonic then look it up in the opcodeTable.

Before calling `DispatchOpcode`, the calling code must do some initialization to set up prefixes and tokens.

1. call `bits.Reset()`
2. Parse out any prefix tokens, look them up in the `prefixTable` then add them to the `prefixes` list.
3. Parse out the operand tokens, look them up in the `tokensTable`, then add them to the `tokens` list.
4. Parse out the mnemonic, look it up in the opcode table, then pass it as an argument

After the return from `DispatchOpcode()` the first thing is to look at the return code.   

If the return code is `AERR_NONE` then the bits array has a byte stream for the instruction, and you can retrieve it for use in generating the instruction with code like the following:

```c++
            unsigned char buf[32];
            bits.GetBytes(buf, (bits.GetBits() + 7) / 8);
            return new Instruction(buf, (bits.GetBits() + 7) / 8);
```

where `Instruction` is some item that caches instruction bytes.

At the same time the `operands` list will have numeric operands that can be processed to add fixup records.

Otherwise if the return value is not `AERR_NONE` some error occurred that can be displayed.

At some point the `operands` and `cleanupValues` arrays will need to have their members deleted to free up the memory.

#### Error codes returned from parser

The following codes can be returned from the `Dispatch()` function:

    AERR_NONE,                       - no error, coding sequence was generated
    AERR_SYNTAX,                     - syntax error, bad token sequence
    AERR_OPERAND,                    - operand error, this operand could not be used here
    AERR_BADCOMBINATIONOFOPERANDS,   - operand error, the operands are used in an invalid combination
    AERR_UNKNOWNOPCODE,              - invalid opcode, mnemonic is unknown
    AERR_INVALIDINSTRUCTIONUSE       - invalid instruction, the 'invalid' keyword was detected in the generated coding sequence.

#### Populating the input tokens array

To come up with a token stream suitable for the parser to process, the basic idea is to look up each token string in the operands in the token table.   If it exists then an `InputToken` of either `REGISTER` or `TOKEN` may be created.   If the value associated with the token string is greater than or equal to 1000, it is a `REGISTER` token, otherwise it is a `TOKEN` token.  The ival of the AdlExprNode associated with the token must be set to the token value or the token value - 1000, depending on whether it is a `TOKEN` or a `REGISTER` type.   For these purposes it doesn't really matter if the AdlExprNode is derived from or not as only the ival is necessary.

Numbers that aren't in the token table can also be parsed out at this point, they would result in an `InputToken` of type `NUMBER`.   Here the `AdlExprNode` would likely have been derived from to generate some type of expression node that allows determination of whether the value is a constant or a label for purposes of the `ParseNumber` and `SetNumber` callbacks.

#### Interpreting the operand array

After `DispatchOpcode()` returns, the byte stream is retrievable from the `bits` member.   But there still may be a need to generate fixup records, e.g. for labels in other sections that will be relocated later, or for external labels that aren't even visible at the time of the assembly.  In other words for constants that may be calculated by the linker.

Here we iterate through the `operand` array.   If the `used` flag is set on an operand, and it has a non-zero size, and if examination of the associated AdlExprNode (which has probably been derived from) results in a non-constant value, then it would be reasonable to attach a fixup to the instruction bytes.   The `Numeric` structure has info about where to place the fixup, e.g. the pos field gives its position in bits and the size field gives its size in bits.   The `relOfs` field gives information for helping to resolve relative branches within the same section.
