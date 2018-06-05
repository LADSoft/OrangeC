### Text Comparison Conditionals

 
 The Text Comparison Conditionals are used in [Conditional Processing](Conditional%20Processing.md), to compare two peices of textual data.  Each takes as an argument a list of two character sequences, separated by a comma.  Each sequence is stripped of leading and trailing spaces, and then the sequences are compared.  It does not matter if the sequences are enclosed in quotes.
 
 Depending on the result of the comparison, successive code will be assembled or ignored based on the specific directive specified.
 
 Text Comparison Conditionals are useful for example in conjunction with **%macro** and **%imacro**, to evaluate macro arguments.


#### %ifidn

 **%ifidn** is a _%if-style conditional_ that compares the two peices of textual data in a case-sensitive manner, and the accompanying code block is assembled if the two peices of data match.  For example:
 
>     %define HELLO goodbye
>     %ifidn HELLO , goodbye
>         mov eax,3
>     %endif
 
 would result in the mov statement being assembled because the substitution for HELLO matches the text goodbye.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%if-style conditionals_.


#### %ifnidn

  **%ifnidn** is a _%if-style conditional_ that compares the two peices of textual data in a case-sensitive manner, and the accompanying code block is assembled if the two peices of data do not match.  For example:
 
>     %define HELLO goodbye
>     %ifnidn HELLO , goodbye
>         mov eax,3
>     %endif
 
 would result in nothing being assembled because the substitution for HELLO matches the text goodbye.  Alternatively:
 
>     %define HELLO goodbye
>     %ifnidn HELLO , hello
>         mov eax,3
>     %endif
 
 would result in the the mov instruction being assembled.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%if-style conditionals_.


#### %elifidn

 **%elifidn** is a _%elif-style conditional_ that compares the two peices of textual data in a case-sensitive manner, and the accompanying code block is assembled if the two peices of data match.  For example:
 
>     %define HELLO goodbye
 %if HELLO = 1
>     %elifidn HELLO , goodbye
>         mov eax,3
>     %endif
 
 would result in the mov statement being assembled because the substitution for HELLO matches the text goodbye.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%elif-style conditionals_.


####


#### %elifnidn

  **%elifnidn** is a _%elif-style conditional_ that compares the two peices of textual data in a case-sensitive manner, and the accompanying code block is assembled if the two peices of data do not match.  For example:
 
>     %define HELLO goodbye
 %if HELLO = 1
>     %elifnidn goodbye , HELLO
>         mov eax,3
>     %endif
 
 would result in nothing being assembled because the substitution for HELLO matches the text goodbye.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%elif-style conditionals_.


#### %ifidni

  **%ifidni** is a _%if-style conditional_ that compares the two peices of textual data in a case-insensitive manner, and the accompanying code block is assembled if the two peices of data match.  For example:
 
>     %define HELLO goodbye
>     %ifidni HELLO , GOODBYE
>         mov eax,3
>     %endif
 
 would result in the mov statement being assembled because the substitution for HELLO matches the text GOODBYE, when considered in a case-insensitive fashion.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%if-style conditionals_.


#### %ifnidni

   **%ifnidni** is a _%if-style conditional_ that compares the two peices of textual data in a case-insensitive manner, and the accompanying code block is assembled if the two peices of data do not match.  For example:
 
>     %define HELLO goodbye
>     %ifnidni HELLO , GOODBYE
>         mov eax,3
>     %endif
 
 would result in nothing being assembled because the substitution for HELLO matches the text GOODBYE when considered in a case-insensitive manner.  Alternatively:
 
>     %define HELLO goodbye
>     %ifnidni HELLO , hello
>         mov eax,3
>     %endif
 
 would result in the the mov instruction being assembled.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%if-style conditionals_.


#### %elifidni

  **%elifidni** is a _%elif-style conditional_ that compares the two peices of textual data in a case-insensitive manner, and the accompanying code block is assembled if the two peices of data match.  For example:
 
>     %define HELLO goodbye
 %if HELLO = 1
>     %elifidni HELLO , GOODBYE
>         mov eax,3
>     %endif
 
 would result in the mov statement being assembled because the substitution for HELLO matches the text GOODBYE when considered in a case-insensitive manner.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%elif-style conditionals_.


#### %elifnidni

  
  **%elifnidni** is a _%elif-style conditional_ that compares the two peices of textual data in a case-insensitive manner, and the accompanying code block is assembled if the two peices of data do not match.  For example:
 
>     %define HELLO goodbye
 %if HELLO = 1
>     %elifnidn GOODBYE , HELLO
>         mov eax,3
>     %endif
 
 would result in nothing being assembled because the substitution for HELLO does matches the text GOODBYE, when considered in a case-insensitive manner.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%elif-style conditionals_.