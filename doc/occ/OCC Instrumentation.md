## OCC Instrumentation

Occ has a variety of options for instrumenting the code in various ways.   For example there are profile stubs generated at the beginning and ending of each function, and various stack canaries used for buffer overflow and uninitialized variable detection.

Note that there is currently no differentiation between system headers and user code;  if instrumententation is turned on it 
will affect templates and inline functions in system headers as well.

### Stack checking

     /C+S    add stack checking code
         
This switch adds calls to the run-time library's stack checking code.  If the stack overruns, an error is generated.


### Profiler calls 

     /Z    add profiler calls
     
This switch adds calls to a profiler module at the beginning and ending of each compiled function.   While intendeded for a profiler, I have also used it to display something on function entry/exit to get a feel for program flow.

### Stack Protector Canaries

These switches are used to detect the case where an attacker overflows a buffer with the intent of modifying the function return address to some desired place.   This is done by placing what is called a 'canary' at the top of the stack frame;   the canary is given a random value that changes on each program run so an attacker won't be able to reliably replace it with the correct value.   If at the end of the function the canary does not have the correct value, the program aborts with an error message.

The functionality for each switch is the same, but each switch selects a different class of functions to guard.

#### Basic stack protection

     -fstack-protector

This guards functions that have a character buffer greater than 8 bytes or that have a call to 'alloca' against overflow.

#### Strong stack protection

     -fstack-protector-strong

In addition to basic protection, it guards against functions that have any array access, or functions where the address of a local variable is used.

#### Blanket stack protection

     -fstack-protector-all

This puts stack protection on all functions regardless of whether there could possibly be a threat.

#### Explicit stack protection

     -fstack-protector-explicit

This puts stack protection on functions that are marked with __attribute__((stack_protect))

### Runtime checks

Stack protection is used to thwart threats; runtime checks are similar but they are usually used more to check for bugs in the code.

#### Buffer overflow

     -fruntime-object-overflow

Each array or structure on the stack is given its own canary.    The canaries are set to '0xcccccccc' when the function starts (by setting the entire stack frame to that value).   AT the end of the function all the canaries are checked, and if any of them differs from the expected value the program aborts with an error message.

     -fruntime-uninitialized-variable

Each arithmethmetic value is given its own canary, which as in the last option is set to '0xcccccccc' when the function starts.   Any time the variable is accessed or its address is taken, the canary is set to 0.   Each time the variable is accessed for use the canary is checked; if it is nonzero the program aborts with an error message.   In this way it can be validated that the variable is used correctly.

Note there are some caveats...  first, just because a variables' address is taken doesn't mean that that variable will be asssigned to, however, the assumption of this option is that it does.   Second, this option works best when combined with the -g flag...   results may vary if the program isn't compiled for debug when this flag is used.