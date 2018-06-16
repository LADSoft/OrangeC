## OCC Compilation Modes

### +A    disable non-ansi extensions
 
By default the compiler allows several extensions to ansi, to make coding easier.  If you want strict adherence to ansi, use this switch.  For example:
 
     OCC +A myfunc.c
 
     will enable ANSI mode
 
### +9    C99 Compatibility
 
By default the compiler compiles for pre-99 standard.  If you want extended features available in the later C99 standard, use this switch.  For example:
 
     OCC /9 myfunc.c
 
     Will enable C99 mode.