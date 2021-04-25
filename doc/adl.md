# Architecture Description Language

The architecture description language (ADL) is intended to form a high-level specification of architecture-specific data such as instruction codings.   Later, it may be used to generate compiler backends.   ADL files are processed by a parser, which then generates useable C++ code to include in for example an assembler.   The codings could also be used for a disassembler and/or simulator at some future point.

The ADL specifies a way of finding the correct sequence of byte codes, then streaming them out to a byte array.   The streaming is done with a resolution of as low as 1 bit, so arbitrary byte codes can be streamed out while still preserving individual entities such as a register or scale factor at the source level.

This documentation has two major parts; a description of the ADL and a description of how to interface the generated code e.g. to an assembler.

## ADL description

An ADL file is an xml file.   The outer data tag is ADL.

The ADL file has three major top-level entities.

The `Processor` entity gives the name of the architecture.   In the future it may give other architectually specific information.

The `Coding` entity gives information for assemblers/dissassemblers.   It allows a translation to be performed between a source-code representation,  and the byte sequence required by a CPU supporting the architecture.

The 'Compiling' entity is not currently supported, in the future it will be used to specify instruction sequences for the compiler to use as well as things like peephole optimizations and register allocation information.

An example file is as follows:

```xml
  <ADL>
	<Processor Name="x64"/>
    <Coding>
    <!-- coding data -->
    </Coding
    <Compiling profile="bit32"/>
    <!-- compiler data -->
    </Compiling>
  </ADL>
```


### Processor entity

The `Processor` entity simply gives the name of the architecture, in text format

For example:

```xml
	<Processor Name="x64"/>
```

### Coding entity

The `Coding` entity gives the correlation between a source code representation, and the corresponding instruction byte codes.   It has 8 groups of definitions, which together form the basis for translation.

These groups are

* Param                      specifies various parameters such as case sensitivity and endianness
* Number                     specifies number formats or numeric constants
* State Variables            specifies variables that can be for example to choose between different CPU modes
* Coding States              specifies parts of a coding sequence that depend on the assembler state
* Register definitions       specifies registers in terms of ordinals and the address modes that can use them.
* Double Registers           specifies which registers will be used as register pairs when selecting an integer size equal to twice the register width
* Addressing Modes           specifies the relationship between addressing modes and coding sequences
* Opcodes                    specifies the relationship between opcodes and their allowed operands, and expands upon the coding sequences

This information is specified free form in the confines of the `Coding` entity, without further grouping in the xml file.

For example:

```xml
	<Coding>
		<Param Name="Case Sensitive" Value="false"/>
		<!-- more params -->
		<Number Class="signed8">
			<Instance Value="-#:8"/>
		</Number>
		<!-- more number formats -->
		<StateVars>
			<Instance  Name="processorbits" Init="16"/>
		</StateVars>
        <!-- more state variables -->
		<State Name="addr32">
			<When Cond="'processorbits'==32" Coding=""/>
			<When Cond="'processorbits'!=32" Coding="0x67:8"/>
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
		<Address Name="['base:base32']"	Class="rm,frm,rm8,rm16,rm32,rm64,mmrm,sserm,rm32bit,rm64bit" Coding="'addr32' *'mand' 0x40+'W'+'R'+'base.B':8 'op' 0:2 'mod':3 'base.Ord':3"/>
		<!-- more addressing modes
		<Opcode Name="fnstenv">
			<Operands Name="'rm:rm'" op="0xd9:8" mod="6" R="0" W="0"/>
		</Opcode>
		<!-- more opcode definitions>
	</Coding>
```

This is an incomplete example with some statements taken from the x64 ADL file.   There are a number of use cases not shown here.

#### Basic concepts

There are a number of basic concepts which are used in various places within the ADL.   These include:

* Conditionals       allows different uses of an entity depending on a specified condition
* Classes            allows linkage of one entity to another
* Variables          allows accessing a register, number, or address mode that is specified in the source code
* Codings            allows specifying the bit stream that makes up the instruction bytes
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

this is because a Cond attribute exists and compares a processor bits variable with 16.

The following `State` node encodes an x86 data size prefix into the coding sequence, if the processor is not in 16-bit mode.

```xml
    <State Name="data16">
      <When Cond="'processorbits'==16" Coding=""/>
      <When Cond="'processorbits'!=16" Coding="0x66:8"/>
    </State>
```

here processorbits was already defined as a state variable:

```xml
	<StateVars>
		<Instance  Name="processorbits" Init="16"/>
	</StateVars>
```

and the quotes around `processorbits` indicate it should be looked up in the state variables table.



##### Classes

Numbers, Registers, Address Modes, and Opcodes may have classes specified as an attribute.

Number classes are consumed by address modes or operands and are used to correlate a number in ASCII text to its coding sequence.

Register classes are also consumed by address modes; they are generally used to generate a token sequence that can parse one of several register names and come up with a unique ordinal.

Address mode classes are consumed by opcodes and are used to group multiple address modes together.   Between a token sequence specified in the address mode and the class specification, a unique coding sequence can be derived.   Note that this means two Address specifications could differ only in class to allow different codings for the same input token sequence.

Opcode classes are consumed by other opcodes, they are used to group different combinations of address modes together to find the coding sequence for the specified address mode.

for example in the number

```xml
    <Number Class="rel16" relOfs="2">
      <Instance Value="-$#:16" Cond="'processorbits'==16"/>
    </Number>
```

The class 'rel16' can be referenced as part of the source code representation in a way that will be defined in a subsequent section.   For example to link a numeric value in the source code to an Addressing mode.

Registers have a similar specification:

```xml
    <Register Class="general">
      <Instance Name="al" Ord="0" B="0" X="0" R="0" W="0" Class="reg8,low"/>
      <!-- more aliases for this register -->
	</Register>
```

here the class on the 'register' node is ignored at present; it is the class on the 'instance' node that matters.  We see that things can have more than one class.   For example, different addressing modes might reference different groups of registers.

For addressing modes we might have something like:

```xml
    <Address Name="['mem:address16']" Class="rmx,rm,frm,rm8,rm16,rm32,rm16it" Coding="'addr16' *'mand' 0x40:8 'op' 0:2 'mod':3 6:3 'mem':16"/>
```
	
Here we again have multiple classes associated with the addressing mode.   The reason for doing this is their may be variations on an addressing mode, for example on the x86 some instructions allow a full range of addressing modes, but other instructions allow only an abbreviated amount from the full set.   But the values that are there in the abbreviated set are encoded exactly the same as those in the full set, when present.

The addressing mode example also shows how classes are consumed.   Here the `Name` field specifies the source-code token sequence that results in the encoding.   'address16' was defined as a class on a `Number` like this:

```xml
    <Number Class="address16">
      <Instance Value="$#:16" Cond="'processorbits'==16"/>
    </Number>
```

More will be said about this in the next section.


##### Variables

There are four types of variables.

* State variables allow examination of assembly state
* Coding State variables allow inserting some constant value in the output bit stream, usually based on some conditional value
* Attribute variables are constants that are selected based on the choice of opcode or register
* Address-related variables hold things like register values or numbers that appear in the address mode portion of the input stream.

Variables are typically accessed by enclosing their name in quotes.    For example to access a state variable called processorbits use 'processorbits'

```xml
	<StateVars>
		<Instance  Name="processorbits" Init="16"/>
	</StateVars>
    <Number Class="rel16" relOfs="2">
      <Instance Value="-$#:16" Cond="'processorbits'==16"/>
    </Number>
```

State variables are generally used with 'Cond' attributes.   These will be detailed further in a future section.

Coding state variables are used in 'Coding' attributes.   They specify a constant value that may depend on the value of a state variable, which is streamed into the opcode byte sequence. These will also be detailed further in a future section.

Attribute variables are XML attributes with arbitrary names, that can be referenced from within a contained coding sequence.   For example a common group of addressing modes on the x86 processor always generates the same output sequence, with the exception that three bits in the middle of the sequence are related to the specific instruction being selected.   An attribute variable can specify those three bits.

Another use for an attribute variable is to specify some parameter based on a selected register name.

For example:

```xml
		<Opcode Name="fnstenv">
			<Operands Name="'rmval:rm'" op="0xd9:8" mod="6" R="0" W="0"/>
		</Opcode>
```

here 'op' is used to specified the first byte of the opcode sequence, and 'mod' is the middle three bits of the address mode.   'R' and 'W' are further attribute variables. A coding in an address mode associated with the 'rm' class will utilize this values to customize the output byte sequence.   Again, these names are arbitrary, they could just as well be 'tree' and 'moss'.

Address-related variables appear in the `Name` field of an `Operand` or `Address` entity.   They are in two parts, the name of a variable to be used in coding, and a class to associate with the address mode.   The class can indicate either another address mode, or a register.

In the above example 'rmval:rm' references a variable 'rmval' which has an address mode of 'rm'.

Bariables defined at a higher scope bleed down into whichever coding sequence is selected.   For example an attribute variable can be selected in a coding attribute locally, in a coding attribute within either the opcode or operands of a class-based opcode, or a coding attribute within an address mode.

##### Codings

Codings specify the bit sequence that will be generated for a given address mode.   They often depend on coding state variables, address-related variables, and attribute variables to fill in parts of the bit sequence which change based on assembler state, opcode, or operands.

A coding specification can be located in one of three places.   First, within the current operand node within the selected opcode node.   If there is no coding statement there, a coding statement might be specified within a contained class-based opcode.   If there is no such opcode or there is no coding statement within it, a coding statement will be found in the associated address mode.

A coding may exist in an opcode node, but for example reuse the encoding of an address mode as a sub-part of the actual encoding.

A coding attribute is a sequence of codings.   For example:

```xml
    <Address Name="['mem:address16' + 'reg:reg16']" Class="rmx,rm,frm,rm8,rm16,rm32,rm16it" Coding="'addr16' *'mand' 0x40:8 'op' ~3:2 'mod':3 'reg.Ord':3 'mem':16"/>
```

Codings are in two parts, the value, and the number of bits to be encoded.   These parts are separated by a : character.

The simplest of codings is a constant.   For example `0x40:8` specifies an encoded constant of 40 hex, encoded into 8 bits.

Another simple type of codings is a variable reference.   For example the coding 'mod':3  uses the variable `mod`, encoded into three bits.

Codings may inherit a bit size from a variable definition.   For example for the coding 'op', the x86 ADL file usually specifies that op is an 8-bit constant, for example '0xd9:8'.

Preceding the name of variable codings by '\*' means the coding is optional.   For example the 'mand' variable is defined in a few opcodes but not in other opcodes; when it is not defined nothing will be emitted.

The '.' operand access an attribute variable associated with a register.   In the above examplea ssuming reg16 is the class associted with a register, the coding 'reg.Ord':3 takes the selected register and encodes the value of its 'Ord' attribute into 3 bits.

codings may contain the following unary operands:

! ~

or the following binary operands:

+ - & | ^ < > .

where for example ~3:2 encodes 0 into two bits and 3+3:3 encodes 6 into three bits.   Here '<' is left shift and '>' is right shift.



Two keywords may be used within an encoding.

The 'native' keyword indicates that a lower level coding should be applied.   For example:

```xml
	<Opcode Name="add">
		<Operands Name="rmval:rm" Coding = "0x22:8 native 0x22:8"
	</Opcode>	
```

encodes the native encoding for the rm addressing mode between two 0x22 sentinels.

The 'illegal' keyword indicates that this coding is illegal, no opcode will be emitted and the assembler should display an error.   This couild be used to flag that an address mode that would be valid for one opcode, is not valid for another...

For example in the state variable:

```xml
    <State Name="no64">
      <When Cond="'processorbits'==64" Coding="illegal"/>
      <When Cond="'processorbits'!=64" Coding=""/>
    </State>
```	

the entire coding becomes illegal because of the reference to 'illegal'.

##### Address Names

The `Name` attribute of an `Address` node indicates the mapping from text as found in the source code.   As an example:

```xml
    <Address Name="[ds:bx+'index:index16' + 'signed:signed8offset']" Class="rmx,rm,frm,rm8,rm16,rm32,rm16it" Coding = "'addr16' *'mand' 0x40:8 'op' 1:2 'mod':3 0:2 'index.index':1 'signed':8"/>
```

Here the mapping is "\[ds:bx+'index:index16' + 'signed:signed8offset'\]".   In this example everything outside the single quote sequences is taken literally, and must exist in the source code.   In this case the values between the quotes are address-related variables, e.g. `index` is a variable name and `index16` is a register class.   `signed` is a variable name and `signed8offset` is a number class.   Note that there is no requirement that a specific class type be used in a specific place, those just happen to be the class types specified in this example.

Name attributes can also have optional text.   For example we can modify the previous example slightly:

```xml
    <Operand Name="{dword} [ds:bx+'index:index16' + 'signed:signed8offset']" Coding = "'addr16' *'mand' 0x40:8 'op' 1:2 'mod':3 0:2 'index.index':1 'signed':8"/>
```

and now the braces indicate there may optionally be a word `dword` before the rest of the operand.   Note there is currently no way of escaping the brace...

#### Param group

There are three parameters at present.

```xml
    <Param Name="CaseSensitive" Value="false"/>
    <Param Name="Endian" Value="little"/>
    <Param Name="BitsPerMAU" Value="8"/>
```

The `Name` attribute is the name of the parameter, and the `Value` attribute is the value of the parameter.

The `CaseSensitive` parameter indicates whether the source code is case sensitive.   It is used for parsing.

The `Endian` parameter indicates the endianness of the architecture.   E.G 'big' or 'little'.   It is used for streaming numbers.  

The `BitsPerMAU` parameter is the minimum number of bits that may be retrieved by the hardware.   Right now only 8 (byte) is supported.


#### Number group

There are several formats for numbers.   The first is a literal value; the second is a group of enumerations, the next is a signed or unsigned number, and the last one is a relative quantity (relative to the current code position)

The number tag gives a class name, which may be used in addressing modes. to select the number.   For example assuming addr16 is the class of a 16 bit number:

```xml
    <Number Class="addr16">
      <Instance Value="$#:16" Cond="'processorbits'==16"/>
    </Number>
```

we might have an address field that references it as follow

```xml
	<Address Name="'val:addr16'" Class="rm" Coding="0x40:8 'val':16"/>
```

Here the address-related variable in the name field gives the class name of the number type to use.

Numeric constants map a textual value to a numeric value.   The `Number` node contains an `instance` node containing a `Value` attribute.   It is the `Value` attribute that holds the mapping for the number.

Here the textual value '0' is being mapped to '0'.   The value before the ; is the textual value; the value after it is the numeric value that it will be mapped to.

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

In the example given at the start of this section:

```xml
    <Number Class="addr16">
      <Instance Value="$#:16" Cond="'processorbits'==16"/>
    </Number>
```

We are mapping a number with class 'addr16' to a mapping of $#:16.   Additionally, a `Cond` attribute specifies that the assembler must be in 16 bit mode for this kind of value to match the input stream.

The $#:16 tells several things about the number.   First, the '$' means it can match relocatable quantities (labels).   The # means parse out a number.  The :16 indicates the number of bits to stream for the coding.

Note that the number parser is not generated by the ADL compiler;   it is meant to be a hand-written addition to take into account the vagaries of numbers in whichever assembler is being developed.

The final example is a relative offset:

```xml
    <Number Class="rel16" relOfs="2">
      <Instance Value="-$#:16" Cond="'processorbits'==16"/>
    </Number>
```

This calculates a relative offset between two points in the output stream.   The relative offset is calculated as follows:   

> relative offset = saidtargetvalue-programcounter-relofs;

so that the relOfs value of two specified in the number means calculate the relative offset from the position in the code that is immediately after the entire instruction coding, including the offset, rather than from the middle of the instruction.

Here the '-' in the value attribute indicates this number is signed.   Leaving it out as in previous examples, means the number is signed.


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
      <When Cond="'processorbits'!=64" Coding="illegal"/>
      <When Cond="'processorbits'==64" Coding=""/>
    </State>
```

here the `Name` field of the `State` node gives the name of the state variable, to be used in a coding.

The `When` nodes give a coding which is based on a condition.

In this case if the state variable named  'processorbits' is equal to 64, then nothing is emitted.   Otherwise the 'illegal' coding is emitted, which results in an error.

In the following example:

```xml
    <State Name="rex">
      <When Cond="true" Coding="0x4:4"/>
    </State>
```

when the coding state variable 'rex' appears in a coding a value of four will be emitted as a four-bit sequence.

The use would be similar to:

```xml
	<Address Name="eax" class="eax" Coding="'only64' 'rex' 'op':8"/>
```


where the coding state variables are as usual enclosed in single quotes.

#### Register definitions group

Register definitions map the relationship between a textual register name, and an ordinal value.   One or more class values may be used to distinguish between multiple register names with the same name.   For example in the following:

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

there are various declarations of two different registers, rax and r8.    They differ in register width but have the same ordinal value.   The class specification of each gives class names that can be referenced from the `Name` attribute of an addressing mode or operand.

The `Class` attribute within the Register is currently unused, but should be chosen to group registers in some way as it will eventually be used when the `Compiling` entity is supported.   On the x86 the grouping is done based on which segment register is related to the register, when it is used for indexing.

In each `Instance` node, the required attributes are:

* `Name` - the textual representation of the register
* `Class` - one or more class names for this register

In this example the 'Ord', 'B', 'X', 'R', and 'W' attributes are attribute variables associated with the register.

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
this is pretty much the same as the original example, except it uses rcx and r9 instead of rax and r8.   The ordinal has changed correspondingly.

Note that the class name of 'general' is also the same as before, since these registers are in a related group.  A similar example for the stack pointer shows the `Class` name on the `Register` node changing:

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

because we are basing the classes on how the indexing works on the x86, and there is a subtle difference between indexing based on a stack register and indexing based on a more general register.

A register may be consumed by an addressing mode or operand, for example:

```xml
    <Address Name="'reg:reg8'" Class="rm8, reg8, reg8rm" Coding="*'mand' 0x40+'reg.B'+'R'+'W':8 'op' 3:2 'mod':3 'reg.Ord':3"/>
```

Here the name field shows a reference to address-related variable class 'reg8'.   Since reg is of class type reg8 if the name is CL it would match the `Name` attribute.   Then in the coding, the statement 'reg.Ord':3 would take the 'Ord' attribute value from the `Instance` definition of CL, e.g. the result would be to encode the value 1 into 3 bits.

#### Double Registers group

Double Registers are an experimental feature that isn't currently supported.   It will probably be moved to the `Compiling` entity at some future time.

#### Addressing Modes group

Addressing modes are the core of the parser.   They correlate either the textual value for a register, or a combination of registers, numbers, and addressing modes, to a coding.

In this example:

```xml
    <Address Name="'reg:reg8'" Class="rm8, reg8, reg8rm" Coding="*'mand' 0x40+'reg.B'+'R'+'W':8 'op' 3:2 'mod':3 'reg.Ord':3"/>
```

the `Name` attribute gives a rendition of the text that will match the input text.   Here it is an address related variable named 'reg' which has a class 'reg8'.   As in other examples, 'reg8' is the class on an 8-bit register instance.   So this matches the name of any 8-bit register.

The class names here could be used in any other `Address` specification.   For example:

```xml
	<Address Name="'rm:rm8'" Class="rm" Coding= ...
```

can match any other address mode with a class 'rm8'.   Including the one in the first example.   If there are other address modes with a class 'rm8' they could potentially match the input as well.

the coding `Coding="*'mand' 0x40+'reg.B'+'R'+'W':8 'op' 3:2 'mod':3 'reg.Ord':3"` in the first example optionally encodes an attribute variable called 'mand' if it exists.   Then it takes the register variable and encodes the sum of its 'B' attribute with the 'R' and 'W' attributes given at an enclosing scope, e.g. an Opcode or Operand node is expected to specify these.   That is streamed as an 8-bit quantity.   Then the 'op' attribute is streamed (its size ias specified at a higher level)  followed by the number 3 in two bits, the mod attribute in 3 bits, and the Ord attribute on the register in 3 bits.   Note that the sum of the number of its is a multiple of 8.   mand has a size attribute of 8 in this adl file, as does op.

In a final example:

```xml
    <Address Name="[ds:'base:base32'+'index:index32'*'sc:scale']" Class="rm,frm,rm8,rm16,rm32,rm64,mmrm,sserm,rm32bit,rm64bit" Coding="'addr32' *'mand' 0x40+'W'+'R'+'index.X'+'base.B':8 'op' 0:2 'mod':3 0x4:3 'sc':2 'index.Ord':3 'base.Ord':3"/>
```

we have a complex `Name` attribute.   Everything outside single quotes is literal text.   Then we have 'base:base32' which is an address-related variable that happens to be for a register instance, 'index:index32' which is another address-related variable for a register instance, and 'sc:scale' which happens to be an address-related variable for a number instance.   This might match an x86 addressing mode such as:

\[ds:eax+esi\*4\] or \[ds:ebp + eax\*2\].


#### Opcodes group

Opcode nodes are the primary method of associating an instruction mnemonic and a set of operands, to addressing modes and thus codings.

There are two ways of writing opcode nodes; the class-based one associates a set of Operand nodes that form the primary addressing modes for a group of mnemonics.   In this case the referencing opcodes use the class name of the class-based opcode as kind of a 'subroutine', filling in some attribute-based variables for the opcode-specific parts.

Otherwise in the case where an Opcode doesn't share operands with other instructions, the Opcode node can directly have Operand nodes that tell how to perform the translation.

For a simple example:

```xml
    <Opcode Name="cmpxchg8b">
      <Operands Name="'rm:rm'" mod="1" op="0x0f:8 0xc7:8" R="0" W="0"/>
    </Opcode>
```

Here we are defining an opcode with `Name="cmpxchg8b"`.   The `Name` gives the mnemonic of the opcode in text format.

It has one operand combination which has a `Name` that is a variable named 'rm' with a class name of 'rm'.   In the source code, this will match any address mode with a class name of 'rm'.   It can also match registers or numbers, but in this case that would be useless because there isn't an explicit usable coding associated with the operands.

This operand defines several attribute variables to be consumed by the 'coding' statement of the referenced address mode.   In particular, note that in this case the 'op' variable specifies a coding which is two bytes long.

This definition would not be directly shared by other mnemonics since it doesn't reference a class-based operand.

As another simple example:

```xml
    <Opcode Name="cmpsq">
          <Operands Name="empty" Coding="0x48:8 'data64' 0xa7:8"/>
    </Opcode>
```

here the 'empty' keyword means that there are no operands for this opcode.    The coding is directly specified in the operand in this case.

we could just as easily have written:

```xml
    <Opcode Name="cmpsq" op="0xa7:8">
          <Operands Name="empty" Coding="0x48:8 'data64' 'op'"/>
    </Opcode>
```

As a simple example of a class-based operand we might have:

```xml
    <Opcode Class="single">
      <Operands Name="empty" Coding="0x40:8 'op'"/>
    </Opcode>
```
This again has the 'empty' keyword as the operand name, meaning there are no operands for these kinds of mnemonics.   

The fact that the opcode node has a class but no name means it can be used by multiple mnemonics.   Here the coding indicates that there will be an eight-bit prefix of 0x40, followed by the codings for the attribute variable 'op' which is specified in the original opcode.   It might 'called' as follows:

```xml
    <Opcode Name="aaa" op="'no64' 0x37:8" Class="single"/>
```

here the presence of both a mnemonic and a class name means this is a mnemonic definition, that references the common operands found in the class 'single'.

Either type of opcode may have multiple operands:

```xml
    <Opcode Name="cmpxchg" op1="0x0f:8 0xb0:8">
      <Operands Name="{byte} 'rm:rm8', 'modreg:reg8'" op="'op1'" mod="'modreg.Ord'" R="'modreg.R'" W="'modreg.W'"/>
      <Operands Name="{byte} 'rm:rm8', 'modreg:reg8t'" mand="0x40:8" op="'op1'" mod="'modreg.Ord'" R="'modreg.R'" W="'modreg.W'"/>
      <Operands Name="{word} 'rm:rm16', 'modreg:reg16'" op
="'data16' 'op1'+1:8" mod="'modreg.Ord'" R="'modreg.R'" W="'modreg.W'"/>
      <Operands Name="{dword} 'rm:rm32', 'modreg:reg32'" op="'data32' 'op1'+1:8" mod="'modreg.Ord'" R="'modreg.R'" W="'modreg.W'"/>
      <Operands Name="{qword} 'rm:rm64', 'modreg:reg64'" op="'data64' 'op1'+1:8" mod="'modreg.Ord'" R="'modreg.R'" W="'modreg.W'"/>
    </Opcode
```

where again values enclosed in {} are optional literals and other values outside the '' pairs are literal.

In the following Operand node:

```xml
      <Operands Name="byte 'rm:rm8',  {byte} 'omem:mem8'" op="0x80:8" R="0" W="0" Coding="native 'omem':8"/>
```

the class rm8 already has a coding as an addressing mode.   The coding in this operand node overrides that encoding, but since we want to reuse the original coding after all we say 'native'.   Then for purposes of the referenced instruction we encode a constant value after the original encoding...

### Compiling entity

The `Compiling` entity isn't fully fleshed out or otherwise supported yet.   It won't be documented here, other than to say there may be multiple profiles for example one could use a different profile for different bit widths.

## Code level interface

The ADL compiler generates C++ code which can be interfaced to an external tool, e.g. a compiler or assembler.   This section describes the interface.

