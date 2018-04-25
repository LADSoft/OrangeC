### Token Type Classification Conditionals

 
>     ace;">The Token Type Classification conditionals are used in [Conditional Processing](Conditional%20Processing.md).  They take a character stream, and determine what kind of token the assembler will think it would be if it were to assemble the stream as part of processing an instruction or directive.  This is useful for example within multiline macro definitions, to change the behavior of a macro based on the type of data in one or more of the arguments.  Token Type classification directives can detect one of three types of token:  labels, numbers, and strings.


#### %ifid

  **%ifid** is a _%if-style conditional_ that detects if the token could be a label, and processes the following block if it is.
 
>     ace;">>     ace;">%ifid  myLabel
>     ace;">    mov eax,3
>     ace;">%endif
 
 would result in the mov statement being assembled because myLabel matches a character sequence that could be used in a label.  It does not matter if myLabel has actually been defined; the fact that it could be assembled as a label is all that is needed.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%if-style conditionals_.


#### %ifnid

  **%ifnid** is a _%if-style conditional_ that detects if the token could be a label, and processes the following block if it is not.
 
>     ace;">%ifnid 5
>     ace;">    mov eax,3
>     ace;">%endif
 
 would result in the mov statement being assembled because 5 is a number, and does not match the character sequence required for a label.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%if-style conditionals_.


#### %elifid

 
  **%elifid** is a _%elif-style conditional_ that detects if the token could be a label, and processes the following block if it is.
 
>     ace;">%if 1 == 2
>     ace;">>     ace;">%elifid  myLabel
>     ace;">    mov eax,3
>     ace;">%endif
 
 would result in the mov statement being assembled because myLabel matches a character sequence that could be used in a label.  It does not matter if myLabel has actually been defined; the fact that it could be assembled as a label is all that is needed.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%elif-style conditionals_.


#### %elifnid

 
  **%elifnid** is a _%elif-style conditional_ that detects if the token could be a label, and processes the following block if it is not.
 
>     ace;">%if 1 == 2
>     ace;">>     ace;">%elifnid  5
>     ace;">    mov eax,3
>     ace;">%endif
 
  would result in the mov statement being assembled because 5 is a number, and does not match the character sequence required for a label.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%elif-style conditionals_.


#### %ifnum

 
  **%ifnum** is a _%if-style conditional_ that detects if the token is a number, and processes the following block if it is.
 
>     ace;">>     ace;">%ifnum  5
>     ace;">    mov eax,3
>     ace;">%endif
 
 would result in the mov statement being assembled because 5 is a number.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%if-style conditionals_.


#### %ifnnum

 **%ifnnum** is a _%if-style conditional_ that detects if the token is a number, and processes the following block if it is not.
 
>     ace;">>     ace;">%ifnnum  5
>     ace;">    mov eax,3
>     ace;">%endif
 
 would result in the mov statement not being assembled because 5 is a number.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%if-style conditionals_.


#### %elifnum

 
  **%elifnum** is a _%elif-style conditional_ that detects if the token could be a label, and processes the following block if it is.
 
>     ace;">%if 1 == 2
>     ace;">>     ace;">%elifnum 5
>     ace;">    mov eax,3
>     ace;">%endif
 
 would result in the mov statement being assembled because 5 is a number.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%elif-style conditionals_.


#### %elifnnum

 **%elifnum** is a _%elif-style conditional_ that detects if the token is a number and processes the following block if it is not.
 
>     ace;">%if 1 == 2
>     ace;">>     ace;">%elifnnum  hi
>     ace;">    mov eax,3
>     ace;">%endif
 
 would result in the mov statement being assembled because 'hi' is not a number, it matches the format for a label.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%elif-style conditionals_.


#### %ifstr

 
  **%ifstr** is a _%if-style conditional_ that detects if the token is a string, and processes the following block if it is.
 
>     ace;">>     ace;">%ifstr  "This is a string"
>     ace;">    mov eax,3
>     ace;">%endif
 
 would result in the mov statement being assembled because the argument is a string.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%if-style conditionals_.


#### %ifnstr

 
  **%ifnstr** is a _%if-style conditional_ that detects if the token is a string, and processes the following block if it is not.
 
>     ace;">>     ace;">%ifnstr  "This is a string"
>     ace;">    mov eax,3
>     ace;">%endif
 
 would result in nothing being assembled because the argument is a string.  But:
 
>     ace;">%ifnstr 5
>     ace;">    mov eax,3
>     ace;">%endif
 
 would result in the mov being assembled, because 5 is a number not a string.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%if-style conditionals_.


#### %elifstr

 
  **%elifstr** is a _%elif-style conditional_ that detects if the token is a string, and processes the following block if it is.
 
>     ace;">%if 1 == 2
>     ace;">%elifstr  "This is a string"
>     ace;">    mov eax,3
>     ace;">%endif
 
 would result in the mov statement being assembled because the argument is a string.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%elif-style conditionals_.


#### %elifnstr

  
  **%elifnstr** is a _%elif-style conditional_ that detects if the token is a string, and processes the following block if it is.
 
>     ace;">%if 1 == 2
>     ace;">%elifnstr 5
>     ace;">    mov eax,3
>     ace;">%endif
 
 would result in the mov statement being assembled because 5 is a number, not a string.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%elif-style conditionals_.
  
   
 