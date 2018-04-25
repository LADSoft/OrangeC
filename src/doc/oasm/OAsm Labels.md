## OAsm Labels

 
 Labels may begin with any alphabetic character, or with any of the characters '\_', '?', or '.'.  Within a label, alphabetic characters, digits, or any of the characters '\_', '$', '\#', '@', '~', '?', '.' may occur.
 
 Labels may be followed by a ':' character, but this is optional.
 
 There are various types of labels.  A standard label begins with an alphabetic character, or '\_', '?'.  Additionally 'local' labels may be defined.  Local labels always start with a '.'.  Local labels may be reused, providing there is a standard label between uses.  This allows you to use meaningless names like '.1' '.2' and so forth within a local context, instead of having to come up with unique labels every time a label is required.
 
 For example in the fragment
 
>     routine1:
>         test ax,1
>         jnz  .exit
>         ; do something
>     .exit:
>         ret
 
>     routine2:
>         cmp bx,ax
>         jc   .exit
>         ; do something
>     .exit:
>         clc
>         ret
 
>     .exit is defined twice, however, each definition follows a different standard label so the two definitions are actually different labels.
 
 Internally, each use of a local label does have a unique name, made up by concatenating the most recent standard label name with the local label name.  In the above example the internal names of the two labels are thus>     routine1.exit and>     routine2.exit.  It is possible to branch to the fully qualified name from within another context.
 
 The context for local labels changes each time a new standard label is defined.  It is sometimes desirable to define a kind of label which is neither a standard label, that would change the local label context, or a local label, which is useful only within that context.  This is done by prepending the label name with '..@'.  For example in the below:
 
 
>     routine3:
>         text ax,1
>         jnz .exit
>     ..@go3:
>         ; do something
>     .exit:
>         ret
 
>     main:
>         call ..@go3
>         ret
 
 the label>     ..@go3 is not qualified by the local label context of routine 3, nor does it start a new local label context, so>     .exit is still a local label within the context of>     routine3.
 
 **OAsm** generates two forms such labels within macro invocations, and within 'contexts' as shown in other sections.  In these cases the label starts with '..@', has a sequence of digits, then has a '.' or '@' character followed by user-specified text.  When using the nonlocal label format, these forms should be avoided to avoid clashing with assembler-generated labels.


### Special Labels

 **OAsm** defines one special label, '..start'.  This label, if used, indicates that this particular position in the code space is the first code that should be executed when the program runs.
 
 
 