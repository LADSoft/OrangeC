### Context-Related Extensions

 
 The Context Related extensions are used to define preprocessor contexts.  A preprocessor context can be used to create a memory of state information, between different invocations of multiline macros.  For example, a set of macros could be defined to mimic high-level control functions such as loops and if statements.
 
 Many combinations of contexts can exist simultaneously.  **OAsm** maintains a stack of all open contexts, pushing new contexts on the top of the stack and removing old contents from the top of the stack.  Each context has a name, and a state which can include context-specific variables and definitions.  The name of the context on top of the stack can be examined to determine what the current context is, or changed.  It might be useful to change it for example if two macros maintain a context, but a third macro might change the context based on its arguments, e.g. to allow processing by a fourth macro which would not succeed if the name on top of the context stack wasn't correct.
 
 Within a context, context-specific definitions and labels may be defined for reference from other macros.  Each context-specific definition is in scope while that context is in scope, e.g. while the context is on top of the context-stack.  If a context is open multiple times simultaneously, each instance of the open context is unique, even though the textual representation labels in in the source code for that context may be the same.
 
 Context-specific definitions and labels start with the sequence '%$' and then contain a label start character and other label characters, just like other identifiers.  For example:
 
 %$HelloWorld
 
 could be used in a context-specific definition or label, and would signify that that label goes with the current context.  As an example consider the two macros
 
>     %macro BEGIN 0
>     %push MyBegin
>     %$HelloWorld:
>     %endmacro
 
>     %macro FOREVER 0
>     %ifctx MyBegin
>         jmp %$HelloWorld
>     %pop
>     %else
>     %error FOREVER loop without matching BEGIN
>     %endif
>     %endmacro
>     
 could be used to implement an infinite loop as follows, if the macros are used in a pair.  
 
>         BEGIN
>         inc EAX
>         FOREVER
 
 Contexts can also have 'local' definitions:
 
>     %push  MY_CONTEXT
>     %define %$four 4
 
 results in a definition that will only be valid while this instance of MY_CONTEXT is on top of the context stack.
 
 When contexts are used, they don't have to appear within a multiline macro definition, but it is often useful to use them this way.
 
 Note:  **OAsm** does not separate context-specific label names into different namespaces.  Instead, a prefix is inserted before the symbol's name and the symbol is entered in the global symbol table.  The prefix takes the form of a non-local label, with context-instance identifying information.  This identifying information is simply an integer followed by the character '$'.  For example if the context instance number is 54, the label %$Hello would be translated to:
 
>     ..@54$Hello
 
 by the preprocessor.  Non-local labels of this general form should be avoided as they may collide with labels defined locally within a context.  This also applies to locally defined **%define** statements.


#### %push

 **%push** creates a new context and pushes it on the top of the context stack:
 
>     %push CONTEXT_NAME
 
 'local' definitions can be made within this context as indicated in the introduction.
 
 If %push is used multiple times with the same context name, each context is unique even though the names are the same.  So for example:
 
>     %push MY_CONTEXT
>     %$contextLabel:
>     %push MY_CONTEXT
>     %$contextLabel:
 
 is valid, because the two labels are named locally to the context and are in different context instances.  When the label is used, it will be matched to the context currently on top of the context stack.


#### %pop

 **%pop** removes the context at the top of the context stack:
 
>     %push MY_CONTEXT
>     %pop 
 
 results in MY_CONTEXT no longer being active, and the context that was below it on the context-stack becomes active.  Note, you should use any labels or definitions that are specific to a context before it is popped.  Once a context is popped off the stack, its state is never recoverable.


#### %repl

 **%repl** changes the name of the context at the top of the context-stack.  For example:
 
>     %push MY_CONTEXT
 
 creates a context called MY_CONTEXT.  If that is followed by:
 
>     %repl  NEW_NAME
 
 the context will now be called NEW\_NAME.  When a context is renamed this way, all previous local definitions and labels are still accessible while that context is on top of the context stack.  The only affect of renaming the context is that conditionals which act on context names will be matched against the new name instead of the old one.


#### %ifctx

 **%ifctx** is a _%if-style conditional_ that takes a context name as an argument.  If the context name matches the name of the context on top of the context stack, the next block is assembled, otherwise it is not.
 
 For example:
 
>     %push MY_NAME
>     %ifctx MY_NAME
>         mov eax,4
>     %endif
 
 will result in the mov statement being assembled because the top of the context stack is named MY_NAME, whereas:
 
>     %push MY_NAME
>     %ifctx  ANOTHER_NAME
>         mov eax,4
>     %endif
 
 will result in nothing being assembled because the name of the top of the context stack does not match the argument to **%ifctx**.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%if-style conditionals_.


#### %ifnctx

 **%ifnctx** is a _%if-style conditional_ that takes a context name as an argument.  If the context name does not match the name of the context on top of the context stack, the next block is assembled, otherwise it is not.
 
 For example:
 
>     %push MY_NAME
>     %ifnctx MY_NAME
>         mov eax,4
>     %endif
 
 will result in nothing being assembled because the name of the context on top of the stack matches the argument.
 
>     %push MY_NAME
>     %ifnctx  ANOTHER_NAME
>         mov eax,4
>     %endif
 
 will result in the mov being assembled because the names do not match.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%if-style conditionals_.


#### %elfictx

 **%elifctx** is a _%elif-style conditional_ that takes a context name as an argument.  If the context name matches the name of the context on top of the context stack, the next block is assembled, otherwise it is not.
 
 For example:
 
>     %push MY_NAME
 %if 44
>     %elifctx MY_NAME
>         mov eax,4
>     %endif
 
 will result in the mov statement being assembled because the top of the context stack is named MY_NAME.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%elif-style conditionals_.


#### %elifnctx

 
 **%elifnctx** is a _%elif-style conditional_ that takes a context name as an argument.  If the context name does not match the name of the context on top of the context stack, the next block is assembled, otherwise it is not.
 
 For example:
 
>     %push MY_NAME
 %if 44
>     %elifctx MY_NAME
>         mov eax,4
>     %endif
 
 will result in nothing being assembled because the names match.
 
 See the section on [Conditional Processing](Conditional%20Processing.md) for more on _%elif-style conditionals_.