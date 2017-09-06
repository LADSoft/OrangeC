/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include "compiler.h"
#include <stdarg.h>

#define ERROR			1
#define WARNING			2
#define TRIVIALWARNING	4
#define ANSIERROR 	8
#define ANSIWARNING 16
#define CPLUSPLUSERROR 32

extern COMPILER_PARAMS cparams ;
extern ARCH_ASM *chosenAssembler;
extern int instantiatingTemplate;
extern int structLevel;

#ifndef CPREPROCESSOR
extern char infile[256];
extern FILE *listFile;
extern FILE *errFile;
extern HASHTABLE *labelSyms;
extern NAMESPACEVALUES *globalNameSpace;
extern INCLUDES *includes;
#endif

extern int preprocLine;
extern char *preprocFile;
extern LEXCONTEXT *context;
extern int templateNestingCount;
extern LIST *externals;

int currentErrorLine;
SYMBOL *theCurrentFunc;

static LIST *listErrors;
static char *currentErrorFile;
enum e_kw skim_end[] = { end, 0 };
enum e_kw skim_closepa[] = { closepa, semicolon, end, 0 };
enum e_kw skim_semi[] = { semicolon, end, 0 };
enum e_kw skim_semi_declare[] = { semicolon, 0 };
enum e_kw skim_closebr[] = { closebr, semicolon, end, 0 };
enum e_kw skim_comma[]= {comma, closepa, closebr, semicolon, end, 0 };
enum e_kw skim_colon[] = {colon, kw_case, kw_default, semicolon, end, 0 };

static struct {
    char *name;
    int	level;
} errors[] = {
{ "Unknown error", ERROR },
{"Too many errors or warnings", ERROR },
{ "Constant too large",  ERROR },
{"Expected constant", ERROR },
{"Invalid constant", ERROR },
{"Invalid floating point constant", ERROR },
{"Invalid character constant", ERROR },
{"Unterminated character constant", ERROR },
{"Invalid string constant", ERROR },
{"Unterminated string constant", ERROR },
{"String constant too long", ERROR },
{"Syntax error: %c expected", ERROR },
{"Syntax error: string constant expected", ERROR},
{"Expected constant or address", ERROR},
{"Expected integer type", ERROR },
{"Expected integer expression", ERROR },
{"Identifier expected", ERROR},
{"Multiple declaration of '%s'", ERROR },
{"Undefined symbol '%s'", ERROR },
{"Too many identififiers in type", ERROR },
{"Unexpected end of file", ERROR },
{"File not terminated with End Of Line character", TRIVIALWARNING},
{"Nested Comments", TRIVIALWARNING},
{"Non-terminated comment in file started at line %d", WARNING},
{"Non-terminated preprocessor conditional in include file started at line %d", ERROR},
{"#elif without #if", ERROR},
{"#else without #if", ERROR},
{"#endif without #if", ERROR},
{"Macro substitution error", ERROR},
{"Incorrect macro argument in call to '%s'", ERROR},
{"Unexpected end of line in directive", ERROR},
{"Unknown preprocessor directive '%s'", ERROR},
{"#error: '%s'", ERROR},
{"Expected include file name", ERROR},
{"Cannot open include file \"%s\"", ERROR},
{"Invalid macro definition", ERROR},
{"Redefinition of macro '%s' changes value", ANSIWARNING},
{"error: %s", ERROR},
{"warning: %s", WARNING},
{"Previous declaration of '%s' here", WARNING},
#ifndef CPREPROCESSOR
{"Unknown symbol '%c'", ERROR},
{"Size of '%s' is unknown or zero", ERROR},
{"Size of the type '%s' is unknown or zero", ERROR },
{"Tag '%s' not defined as this type", ERROR},
{"Structure body has no members", ERROR},
{"Anonymous unions are nonportable", TRIVIALWARNING | ANSIERROR},
{"Anonymous structures are nonportable", TRIVIALWARNING | ANSIERROR},
{"Structured type '%s' previously defined", ERROR},
{"Type may not contain a structure with incomplete array", ERROR},
{"Enumerated type '%s' previously defined", ERROR},
{"Trailing comma in enum declaration", ANSIERROR},
{"Tag '%s' not defined as this type", ERROR},
{"Enumerated type may not be forward declared", ERROR},
{"Bit field must be a non-static member of a structured type", ERROR },
{"Bit field type must be 'int', 'unsigned', or '_Bool'", ANSIERROR },
{"Bit field must be an integer type", ERROR },
{"Bit field too large", ERROR },
{"Bit field must contain at least one bit", ERROR },
{"%s", ERROR }, //"'%s' is not a member of '%s'", ERROR },
{"Pointer to structure required to left of '%s'", ERROR},
{"Structure required to left of '%s'", ERROR },
{"Storage class '%s' not allowed here", ERROR },
{"_absolute storage class requires address", ERROR },
{"Too many storage class specifiers", ERROR },
{"Declarations for '%s' use both 'extern' and 'static' storage classes", WARNING | ANSIERROR },
{"Static object '%s' may not be declared within inline function", ERROR }, /* fixme */
{"Static object '%s' may not be referred to within inline function", ERROR },
{"Linkage specifier not allowed here", ERROR },
{"Too many linkage specifiers in declaration", ERROR },
{"Declaration syntax error", ERROR },
{"Redeclaration for '%s' with different qualifiers", ERROR },
{"Declaration not allowed here", ERROR },
{"'%s' has already been included", ERROR },
{"Missing type in declaration", WARNING },
{"Missing type for parameter '%s'", WARNING },
{"'%s' is a C99 or C11 keyword - did you mean to specify C99 or C11 mode?", WARNING },
{"Too many types in declaration", ERROR },
{"Too many identifiers in declaration", ERROR },
{"Type '%s' requires C99", ERROR },
{"Type 'void' not allowed here", ERROR },
{"Incompatible type conversion", ERROR },
{"Multiple declaration for type '%s'", ERROR },
{"Too many type qualifiers", ERROR },
{"Type mismatch in redeclaration of '%s'", ERROR },
{"Two operands must evaluate to same type", ERROR },
{"Variably modified type requires C99", ERROR },
{"Variably modified type allowed only in functions or parameters", ERROR },
{"Unsized Variable length array may only be used in a prototype", ERROR },
{"Array qualifier requires C99", ERROR },
{"Array qualifier must be on last element of array", ERROR },
{"Array qualifier must be on a function parameter", ERROR },
{"Only first array size specifier may be empty", ERROR },
{"Array size specifier must be of integer type", ERROR },
{"Array declaration or use needs closebracket", ERROR },
{"Array initialization designator not within array bounds", ERROR },
{"Array must have at least one element", ERROR },
{"Nonsized array must be last element of structure", ERROR },
{"Array expected", ERROR },
{"Array designator out of range", ERROR },
{"Type mismatch in redeclaration of function '%s'", ERROR },
{"Untyped parameters must be used with a function body", ERROR },
{"Function parameter expected", ERROR },
{"Parameter '%s' missing name", ERROR },
{"Functions may not be part of a structure or union", ERROR },
{"Functions may not return functions or arrays", ERROR },
{"Function should return a value", WARNING },
{"Type void must be unnamed and the only parameter", ERROR },
{"'inline' not allowed here", ERROR },
{"'main' may not be declared as inline", ERROR },
{"Function takes no arguments", ERROR },
{"Call to function '%s' without a prototype", TRIVIALWARNING },
{"Argument list too long in call to '%s'", ERROR },
{"Argument list too short in call to '%s'", ERROR },
{"Call of nonfunction", ERROR },
{"Type mismatch in parameter '%s' in call to '%s'", ERROR },
{"extern object may not be initialized", ERROR },
{"typedef may not be initialized", ERROR },
{"object with variably modified type may not be initialized", ERROR },
{"Too many initializers", ERROR },
{"Initialization designator requires C99", ERROR },
{"Non-constant initialization requires C99", ERROR },
{"Bitwise object may not be initialized", ERROR },
{"Object of type '%s' may not be initialized", ERROR }, /* fixme */
{"Objects with _absolute storage class may not be initialized", ERROR },
{"Constant variable '%s' must be initialized", ERROR },
{"String type mismatch in initialization", TRIVIALWARNING },
{"Multiple initialization of '%s'", ERROR },
{"Initialization bypassed", WARNING },
{"Object with variably modified type in line '%s' bypassed by goto", ERROR },
{"Nonportable pointer conversion", WARNING },
{"Nonportable pointer comparison", WARNING },
{"Nonportable pointer conversion", ERROR },
{"Suspicious pointer conversion", WARNING },
{"Dangerous pointer conversion", ERROR },
{"String constant may not be used as initializer here", ERROR },
{"Lvalue required", ERROR },
{"Invalid indirection", ERROR },
{"'%s' used without prior assignment", WARNING },
{"Possible incorrect assignment", WARNING },
{"Ansi forbids automatic conversion from void in assignment", ERROR },
{"'%s' assigned a value that is never used", TRIVIALWARNING },
{"Parameter '%s' unused", TRIVIALWARNING },
{"'%s' unused", TRIVIALWARNING },
{"static object '%s' is unused", TRIVIALWARNING },
{"'%s' hides declaration at outer scope", WARNING },
{"Not an allowed type", ERROR },
{"Cannot modify a const object", ERROR },
{"Expression syntax error", ERROR },
{"signed/unsigned mismatch in '%s' comparison", TRIVIALWARNING },
{"sizeof may not be used with a function designator", ERROR},
{"Invalid structure assignment", ERROR },
{"Invalid structure operation", ERROR },
{"Invalid use of complex", ERROR },
{"Invalid use of floating point", ERROR },
{"Invalid pointer operation", ERROR },
{"Invalid pointer addition", ERROR },
{"Invalid pointer subtraction", ERROR },
{"Cannot take the address of a bit field", ERROR },
{"Cannot take address of a register variable", ERROR },
{"Must take address of memory location", ERROR },
{"Cannot use variable of type 'bool' here", ERROR },
{"conditional needs ':'", ERROR },
{"switch selection expression must be of integral type", ERROR },
{"'break' without enclosing loop or switch", ERROR },
{"'case' without enclosing switch", ERROR },
{"Duplicate case '%s'", ERROR },
{"case value must be an integer expression", ERROR },
{"'continue' without enclosing loop", ERROR },
{"'default' without enclosing switch", ERROR },
{"switch statement needs open parenthesis", ERROR },
{"switch statement needs close parenthesis", ERROR },
{"switch statement already has default", ERROR },
{"do statement needs while", ERROR },
{"do-while statement needs open parenthesis", ERROR },
{"do-while statement needs close parenthesis", ERROR },
{"while statement needs open parenthesis", ERROR },
{"while statement needs close parenthesis", ERROR },
{"for statement needs semicolon", ERROR },
{"for statement needs open parenthesis", ERROR },
{"for statement needs close parenthesis", ERROR },
{"if statement needs open parenthesis", ERROR },
{"if statement needs close parenthesis", ERROR },
{"Misplaced else statement", ERROR },
{"goto statement needs a label specifier", ERROR },
{"Duplicate label '%s'", ERROR },
{"Undefined label '%s'", ERROR },
{"Return statement should have value", WARNING },
{"Attempt to return a value of type void", ERROR },
{"Value specified in return of function with return type 'void'", WARNING },
{"Type mismatch in return statement", WARNING },
{"Returning address of a local variable", WARNING },
{"Expression has no effect", WARNING },
{"Unexpected block end marker", ERROR }, /* fixme */
{"Unreachable code", TRIVIALWARNING },
{"Ansi forbids omission of semicolon", ERROR },
{"Pointer or reference to reference is not allowed", ERROR },
{"Array of references is not allowed", ERROR },
{"Reference variable '%s' must be initialized", ERROR },
{"Reference initialization requires Lvalue", ERROR },
{"Reference initialization of type '%s' cannot be bound to type '%s'", ERROR},
{"Reference member '%s' in a class without constructors", ERROR },
{"Reference member '%s' not initialized in class constructor", ERROR },
{"Qualified reference variable not allowed", WARNING },
{"Attempt to return reference to local variable", ERROR },
{"Cannot convert '%s' to '%s'", ERROR },
{"Cannot cast '%s' to '%s'", ERROR },
{"Invalid use of namespace '%s'", ERROR },
{"Cannot declare namespace within function", ERROR },
{"'%s' is not a namespace", ERROR },
{"Namespace '%s' not previously declared as inline", ERROR },
{"Expected namespace name", ERROR },
{"Cyclic using directive", TRIVIALWARNING },
{"%s", ERROR },
{"Creating temporary for variable of type '%s'", TRIVIALWARNING },
{"Type name expected", ERROR },
{"Ambiguity between '%s' and '%s'", ERROR },
{"Unknown linkage specifier '%s'", WARNING },
{"char16_t or char32_t constant too long", ERROR },
{"Non-terminated raw string in file started at line %d", ERROR},
{"Invalid character in raw string", ERROR},
{"'main' may not be declared as static", ERROR },
{"'main' may not be declared as constexpr", ERROR },
{"constexpr expression is not const", ERROR },
{"constexpr function does not evaluate to const", ERROR },
{"constexpr declaration requires initializer or function body", ERROR },
{"Variable style constexpr declaration needs simple type", ERROR },
{"Function returning reference must return a value", ERROR },
{"Qualified name not allowed here", ERROR },
{"%s", ERROR },
{"Qualifier '%s' is not a class or namespace", ERROR },
{"Linkage mismatch in overload of function '%s'", ERROR },
{"Overload of '%s' differs only in return type", ERROR },
{"Could not find a match for '%s'", ERROR },
{"Redeclaration of default argument for '%s' not allowed", ERROR },
{"Default argument may not be a parameter or local variable", ERROR },
{"Default argument may not be a pointer or reference to function", ERROR },
{"Default argument not allowed in typedef", ERROR },
{"Default argument may not use 'this'", ERROR},
{"Default argument missing after parameter '%s'", ERROR},
{"Deleting or defaulting existing function '%s'", ERROR },
{"Reference to deleted function '%s'", ERROR },
{"'main' may not be deleted", ERROR },
{"Enumerated constant too large for base type", ERROR },
{"Enumerated type needs name", ERROR },
{"Enumerated base type must not be ommitted here", ERROR },
{"Redeclaration of enumeration scope or base type", ERROR },
{"Definition of enumeration constant not allowed here", ERROR },
{"Attempt to use scoped enumeration as integer", WARNING },
{"Cannot take the size of a bit field", ERROR },
{"Cannot take the size of an unfixed enumeration", ERROR },
{"'auto' variable '%s' needs initialization", ERROR },
{"Type 'auto' not allowed in this context", ERROR },
{"Reference initialization discards qualifiers", ERROR },
{"Initialization of lvalue reference cannot use rvalue of type '%s'", ERROR },
{"Global anonymous union not static", ERROR },
{"Union having base classes not allowed", ERROR },
{"Union as a base class not allowed", ERROR },
{"Redefining access for '%s' not allowed", ERROR },
{"'%s' is not accessible", ERROR },
{"Declaring nonfunction '%s' virtual is not allowed", ERROR },
{"Member function '%s' already has final or override specifier", ERROR },
{"Namespace within class not allowed", ERROR },
{"'%s' is not an unambiguous base class of '%s'", ERROR},
{"Return type for virtual functions '%s' and '%s' not covariant", ERROR },
{"Overriding final virtual function '%s' not allowed", ERROR },
{"Function '%s' does not override another virtual function", ERROR },
{"Ambiguity between virtual functions '%s' and '%s'", ERROR },
{"Virtual function '%s' has been deleted while virtual function '%s' has not", ERROR },
{"Inheriting from final base class '%s' not allowed", ERROR },
{"Function '%s' in locally defined class needs inline body", ERROR },
{"Class member has same name as class", ERROR },
{"Pure specifier needs constant value of zero", ERROR },
{"Pure, final, or override specifier needs virtual function", ERROR },
{"Functions may not be a part of a structure or union", ERROR },
{"Ambiguous member definition for '%s'", ERROR },
{"Use of non-static member '%s' without object", ERROR },
{"Non-const function '%s' called for const object", ERROR },
{"Base class '%s' is included more than once", ERROR },
{"Only non-static member functions may be 'const' or 'volatile'", ERROR },
{"Illegal use of member pointer", ERROR },
{"_Noreturn function is returning", ERROR },
{"Duplicate type in generic expression", ERROR },
{"Too many defaults in generic expression", ERROR },
{"Invalid type in generic expression", ERROR },
{"Invalid expression in generic expression", ERROR },
{"Could not find a match for generic expression", ERROR },
{"Invalid storage class for thread local variable", ERROR },
{"Thread local variable cannot have auto or register storage class", ERROR },
{"Functions cannot be thread local", WARNING },
{"Mismatched thread_local storage class specifier", ERROR },
{"Qualifiers not allowed with atomic type specifier", WARNING },
{"Function or array not allowed as atomic type", ERROR },
{"Redeclaration of '%s' outside its class not allowed", ERROR },
{"Definition of type '%s' not allowed here", ERROR },
{"Pointers and arrays cannot be friends", ERROR },
{"Declarator not allowed here", ERROR },
{"'this' may only be used in a member function", ERROR },
{"operator '%s' not allowed", ERROR },
{"operator '%s' must be a function", ERROR },
{"operator '%s' must have no parameters", ERROR },
{"operator '%s' must have one parameter", ERROR },
{"operator '%s' must have zero or one parameter", ERROR },
{"operator '%s' must have parameter of type int", ERROR },
{"operator '%s' must be nonstatic when used as a member function", ERROR },
{"operator '%s' must have structured or enumeration parameter when used as a non-member", ERROR },
{"operator '%s' must have two parameters", ERROR },
{"operator '%s' must have one or two parameters", ERROR },
{"operator '%s' must have second parameter of type int", ERROR },
{"operator '%s' must be a nonstatic member function", ERROR },
{"operator '%s' must return a reference or pointer type", ERROR },
{"Cannot create instance of abstract class '%s'", ERROR},
{"Class '%s' is abstract because of ''%s' = 0'", ERROR },
{"operator \"\" requires empty string", ERROR },
{"operator \"\" requires identifier", ERROR },
{"'%s' requires namespace scope", ERROR },
{"Invalid parameters for '%s'", ERROR },
{"Could not find a match for literal suffix '%s'", ERROR },
{"Literal suffix mismatch", ERROR },
{"Structured type '%s' not defined", ERROR },
{"Incorrect use of destructor syntax", ERROR },
{"Constructor or destructor for '%s' must be a function", ERROR },
{"Default may only be used on a special function", ERROR },
{"Constructor for '%s' must have body", ERROR },
{"Constructor or destructor cannot be declared const or volatile", ERROR },
{"Initializer list requires constructor", ERROR },
{"Constructor or destructor cannot have a return type", ERROR },
{"Cannot take address of constructor or destructor", ERROR },
{"Pointer type expected", ERROR },
{"Objects of type '%s' cannot be initialized with { }", ERROR },
{"Member name required", ERROR },
{"Member initializer required", ERROR },
{"'%s' is not a member or base class of '%s'", ERROR },
{"'%s' must be a nonstatic member", ERROR },
{"Cannot find an appropriate constructor for class '%s'", ERROR },
{"Cannot find a matching copy constructor for class '%s'", ERROR },
{"Cannot find a default constructor for class '%s'", ERROR },
{"'%s' is not a member of '%s', because the type is not defined", ERROR },
{"Destructor for '%s' must have empty parameter list", ERROR },
{"Reference member '%s' is not initialized in class constructor", ERROR },
{"Constant member '%s' is not initialized in class constructor", ERROR },
{"Improper use of typedef '%s'", ERROR },
{"Return type is implicit for 'operator %s'", ERROR },
{"Invalid Psuedo-Destructor", ERROR},
{"Destructor name must match class name", ERROR},
{"Must call or take the address of a member function", ERROR},
{"Need numeric expression", ERROR },
{"Identifier '%s' cannot have a type qualifier", ERROR },
{"Lambda function outside function scope cannot capture variables", ERROR },
{"Invalid lambda capture mode", ERROR },
{"Capture item listed multiple times", ERROR },
{"Explicit capture blocked", ERROR },
{"Implicit capture blocked", ERROR },
{"Cannot default parameters of lambda function", ERROR },
{"Cannot capture this", ERROR },
{"Must capture variables with 'auto' storage class or 'this'", ERROR },
{"Lambda function must have body", ERROR },
{"For-range iterator begin() and end() have mismatched types", ERROR },
{"For-range iterator begin() and/or end() are missing", ERROR },
{"Operator ++() missing on for-range iterator type", ERROR },
{"Operator *() missing on for-range iterator type", ERROR },
{"Operator ==() missing on for-range iterator type", ERROR },
{"Indirection on for-range iterator returns wrong type", ERROR },
{"For-range expression requires structured or array type", ERROR },
{"For-range array expression unsized", ERROR },
{"For-range declarator cannot be initialized", ERROR },
{"For declarator must be initialized", ERROR },
{"Initialization from higher type", WARNING },
{"Nonstructured initializer list expected", ERROR },
{"Cannot use initializer list here", ERROR },
{"Initializer list requires overloaded operator []", ERROR },
{"Cannot use array of structures as function argument", ERROR },
{"Anonymous union cannot contain function or nested type", ERROR },
{"Anonymous union must contain only public members", ERROR },
{"Anonymous union must contain only nonstatic data members", ERROR },
{"Goto bypasses initialization", CPLUSPLUSERROR | WARNING },
{"Try keyword requires compound statement", ERROR },
{"One or more catch handlers expected", ERROR },
{"Catch handler requires compound statement", ERROR },
{"catch (...) must be last catch handler", ERROR },
{"Catch handler without try", ERROR },
{"Constructor or Destructor returns a value", ERROR },
{"Cannot use goto to enter a try block or catch handler", ERROR },
{"Exception specifier of virtual function '%s' must be at least as restrictive as base class declarations", ERROR },
{"Exception specifier of function '%s' must match earlier declarations", ERROR },
{"Use of typeid requires '#include <typeinfo>'", ERROR },
{"Multiple return types specified", ERROR },
{"Cannot place attribute specifiers here", ERROR },
{"Cannot place attribute argument clause here", ERROR },
{"Only constructors or conversion functions may be explicit", ERROR },
{"Implicit use of explicit constructor or conversion function", ERROR },
{"Type '%s' is not a defined class with virtual functions", ERROR },
{"Template may not be declared in class defined within function scope", ERROR},
{"Template must be declared at global scope or within a class", ERROR },
{"Packed template parameter cannot have default" ,ERROR },
{"'Class' template parameter missing default" ,ERROR },
{"'Class' template parameter default must refer to type" ,ERROR },
{"Template 'template' parameter missing default" ,ERROR },
{"'Non-type' template parameter has invalid type", ERROR },
{"Type mismatch in default for 'non-type' template parameter", ERROR },
{"'%s' was not previously declared as a template", ERROR },
{"'%s' was previously declared as a template", ERROR },
{"Template parameters do not agree with previous declaration", ERROR },
{"Missing default values in template declaration after '%s'", ERROR },
{"Redefinition of default value for '%s' in template redeclaration", ERROR },
{"'Template' template parameter must name a class", ERROR },
{"Templates must be classes or functions", ERROR },
{"Cannot partially specialize a function template", ERROR },
{"Partial specialization missing parameter from template header", ERROR },
{"Specialization of '%s' cannot be declared before primary template", ERROR },
{"Too few arguments passed to template '%s'", ERROR },
{"Too many arguments passed to template '%s'", ERROR },
{"Incorrect arguments passed to template '%s'", ERROR },
{"Cannot instantiate template '%s' because it is not defined", ERROR },
{"Cannot generate template specialization from '%s'", ERROR },
{"Cannot use template '%s' without specifying specialization parameters", ERROR },
{"Invalid template parameter", ERROR },
{"Body has already been defined for function '%s'", ERROR },
{"Invalid explicit specialization of '%s'", ERROR },
{"Storage class 'extern' not allowed here", ERROR },
{"Template '%s' is already instantiated", WARNING },
{"Use . or -> to call '%s'", ERROR },
{"Template argument must be a constant expression", ERROR },
{"Pack ... specifier not allowed here", ERROR },
{"Pack ... specifier on variable requires packed template parameter", ERROR },
{"Pack ... specifier on expression requires packed function parameter", ERROR },
{"Pack ... specifier must be used on function parameter", ERROR },
{"Pack ... specifier must be used on function argument", ERROR },
{"sizeof... argument must be a template parameter pack", ERROR },
{"Multiple pack ... specifiers not same size", ERROR },
{"Packed ... template parameter must be 'class' template parameter", ERROR },
{"Packed ... template parameter expected to hold structured types", ERROR },
{"'Class' template parameter expected to hold structured types", ERROR },
{"Pack ... specifier required here", ERROR },
{"'Class' template parameter expected", ERROR },
{"Structured type expected", ERROR },
{"Packed template parameter not allowed here", ERROR },
{"In template instantiation started here", WARNING },
{"Invalid use of type '%s'", ERROR },
{"Requires template<> header", ERROR },
{"Mutable member '%s' must be non-const", ERROR },
{"Dependent type '%s' not a class or structured type", ERROR },
{"Dependent type '%s' is not defined in structured type '%s'", ERROR },
{"Constructor '%s' is not allowed", ERROR },
{"Constant member '%s' in a class without constructors", ERROR },
{"Delete of pointer to undefined type '%s'", WARNING },
{"Arithmetic with pointer of type 'void *'", WARNING },
{"Overloaded function '%s' is ambiguous in this context", ERROR },
{"Use of an initializer-list requires '#include <initializer_list>'", ERROR },
{"Use '&' to take the address of a member function", ERROR },
{"ISO C++ forbids in-class initialization of non-const static member '%s'", ERROR },
{"Need packed template parameter", ERROR },
{"Mismatched types '%s' and '%s' while infering lambda return type", ERROR },
{"Exception specifier blocks exceptions thrown from '%s'", WARNING },
{"Exception specifier blocks locally thrown exception of type '%s'", WARNING },
{"undefined external '%s'", ERROR },
{"Delegating constructor call must be the only initializer", ERROR },
{"Mismatch on packed template types", ERROR },
{"Cannot use new() to allocate a reference", ERROR },
{"This feature may only be used for imported namespaces", ERROR },
{"Cannot use the address of a managed object", ERROR },
{"__property qualifier not allowed on functions", ERROR },
{ "Getter for property '%s' already defined", ERROR },
{ "Setter for property '%s' already defined", ERROR },
{ "Only simple property declarations are supported", ERROR },
{ "Property cannot be declared inside function", ERROR },
{ "Must define getter for property '%s'", ERROR },
{ "Cannot modify property '%s' because there is no setter", ERROR },
{ "Cannot take address of property '%s'", ERROR },
{ "__entrypoint cannot be used on non-function '%s'", ERROR },

#endif
} ;

int total_errors;
int diagcount ;

void errorinit(void)
{
    total_errors = diagcount = 0;
    currentErrorFile = NULL;
}

static char kwtosym(enum e_kw kw)
{
    switch( kw)
    {
        case openpa:
            return '(';
        case closepa:
            return ')';
        case closebr:
            return ']';
        case semicolon:
            return ';';
        case begin:
            return '{';
        case end:
            return '}';
        case assign:
            return '=';
        case colon:
            return ':';
        case lt:
            return '<';
        case gt:
            return '>';
        default:
            return '?';
    }
};
static BOOLEAN IsReturnErr(int err)
{
    switch (err)
    {
//        case ERR_FUNCTION_SHOULD_RETURN_VALUE:
        case ERR_CALL_FUNCTION_NO_PROTO:
        case ERR_RETURN_MUST_RETURN_VALUE:
        case ERR_RETURN_NO_VALUE:
        case ERR_RETMISMATCH:
        case ERR_FUNCTION_RETURNING_ADDRESS_STACK_VARIABLE:
            return chosenAssembler->arch->erropts & EO_RETURNASERR;
        default:
            return FALSE;
    }
}
static BOOLEAN alwaysErr(int err)
{
    switch (err)
    {
        case ERR_TOO_MANY_ERRORS:
        case ERR_UNDEFINED_IDENTIFIER:
        case ERR_ABSTRACT_BECAUSE:
        case ERR_REF_MUST_INITIALIZE:
        case ERR_CONSTANT_MUST_BE_INITIALIZED:
        case ERR_REF_MEMBER_MUST_INITIALIZE:
        case ERR_CONSTANT_MEMBER_MUST_BE_INITIALIZED:
        case ERR_CANNOT_ACCESS:
        case ERR_REF_CLASS_NO_CONSTRUCTORS:
        case ERR_CONST_CLASS_NO_CONSTRUCTORS:
            return TRUE;
        default:
            return FALSE;
    }
}
static BOOLEAN ignoreErrtemplateNestingCount(int err)
{
    switch(err)
    {
        case ERR_NEED_CONSTANT_OR_ADDRESS:
        case ERR_NEED_INTEGER_TYPE:
        case ERR_NEED_INTEGER_EXPRESSION:
        case ERR_MISMATCHED_STRUCTURED_TYPE_IN_REDEFINITION:
        case ERR_STRUCT_MAY_NOT_CONTAIN_INCOMPLETE_STRUCT:
        case ERR_ORIGINAL_TYPE_NOT_ENUMERATION:
        case ERR_BIT_FIELD_INTEGER_TYPE:
        case ERR_MEMBER_NAME_EXPECTED:
        case ERR_POINTER_TO_STRUCTURE_EXPECTED:
        case ERR_STRUCTURED_TYPE_EXPECTED:
        case ERR_INCOMPATIBLE_TYPE_CONVERSION:
        case ERR_TYPE_MISMATCH_IN_REDECLARATION:
        case ERR_TWO_OPERANDS_SAME_TYPE:
        case ERR_ARRAY_INDEX_INTEGER_TYPE:
        case ERR_TYPE_MISMATCH_IN_ARGUMENT:
        case ERR_NOT_AN_ALLOWED_TYPE:
        case ERR_SWITCH_SELECTION_INTEGRAL:
        case ERR_RETMISMATCH:
        case ERR_CANNOT_CONVERT_TYPE:
        case ERR_CANNOT_CAST_TYPE:
        case ERR_NO_OVERLOAD_MATCH_FOUND:
        case ERR_POINTER_TYPE_EXPECTED:
        case ERR_NEED_TYPEINFO_H:
            return TRUE;
    }
    return FALSE;
}
BOOLEAN printerrinternal(int err, char *file, int line, va_list args)
{
    char buf[2048];
    char infunc[2048];
    char *listerr;
    char nameb[265], *name = nameb;
    if (cparams.prm_makestubs || templateNestingCount && ignoreErrtemplateNestingCount(err))
        return FALSE;
    if (!file)
    {
#ifndef CPREPROCESSOR
        if (context)
        {
            LEXEME *lex = context->cur ? context->cur->prev : context->last;
            line = lex->line;
            file = lex->file;
        }
        else
        {
            file = "unknown";
        }
#else
        file = "unknown";
#endif
    }
    strcpy(nameb, file);
    if (strchr(infile, '\\') != 0 || strchr(infile, ':') != 0)
    {
        name = fullqualify(nameb);
    }
    if (total_errors > cparams.prm_maxerr)
        return FALSE;
    if (!alwaysErr(err) && currentErrorFile && !strcmp(currentErrorFile, includes->fname) && 
        includes->line == currentErrorLine)
        return FALSE;
    if (err >= sizeof(errors)/sizeof(errors[0]))
    {
        my_sprintf(buf, "Error %d", err);
    }
    else
    {
          vsprintf(buf, errors[err].name, args);
    }
    if (IsReturnErr(err) || (errors[err].level & ERROR) || (cparams.prm_ansi && (errors[err].level & ANSIERROR)) 
        || (cparams.prm_cplusplus && (errors[err].level & CPLUSPLUSERROR)))
    {
        if (!cparams.prm_quiet)
            printf("Error   ");
#ifndef CPREPROCESSOR
        if (cparams.prm_errfile)
            fprintf(errFile, "Error   ");
#endif
        listerr = "ERROR";
        total_errors++;
        currentErrorFile = file;
        currentErrorLine = line;
    }
    else
    {
        if (!cparams.prm_warning)
            return FALSE;
        if (errors[err].level & TRIVIALWARNING)
            if (!cparams.prm_extwarning)
                return FALSE;
        if (!cparams.prm_quiet)
            printf("Warning ");
#ifndef CPREPROCESSOR
        if (cparams.prm_errfile)
            fprintf(errFile, "Warning ");
#endif
        listerr = "WARNING";
    }
#ifndef CPREPROCESSOR
    if (theCurrentFunc && err != ERR_TOO_MANY_ERRORS && err != ERR_PREVIOUS && err != ERR_TEMPLATE_INSTANTIATION_STARTED_IN)
    {
        strcpy(infunc, " in function ");
        unmangle(infunc + strlen(infunc), theCurrentFunc->errname);
    }
    else
#endif
        infunc[0] = 0;
    if (!cparams.prm_quiet)
        printf(" %s(%d):  %s%s\n", name, line, buf, infunc);
#ifndef CPREPROCESSOR
    if (cparams.prm_errfile)
        fprintf(errFile, " %s(%d):  %s%s\n", name, line, buf, infunc);
    AddErrorToList(listerr, buf);
    if (total_errors == cparams.prm_maxerr)
        error(ERR_TOO_MANY_ERRORS);
#endif
    return TRUE;
}
int printerr(int err, char *file, int line, ...)
{
    BOOLEAN canprint = FALSE;
    va_list arg;	
    va_start(arg, line);
    canprint = printerrinternal(err,file,line,arg);
    va_end(arg);
    if (instantiatingTemplate && canprint)
    {
        printerrinternal(ERR_TEMPLATE_INSTANTIATION_STARTED_IN, includes->fname, includes->line, NULL);
    }
    return canprint;
}
void pperror(int err, int data)
{
    printerr(err, preprocFile, preprocLine, data);
}
void pperrorstr(int err, char *str)
{
    printerr(err, preprocFile, preprocLine, str);
}
void preverror(int err, char *name, char *origfile, int origline)
{
    if (printerr(err, preprocFile, preprocLine, name))
        if (origfile && origline)
            printerr(ERR_PREVIOUS, origfile, origline, name);
}
#ifndef CPREPROCESSOR
void preverrorsym(int err, SYMBOL *sp, char *origfile, int origline)
{
    char buf[2048];
    unmangle(buf, sp->errname);
    if (origfile && origline)
        preverror(err, buf, origfile, origline);
}
#endif
void errorat(int err, char *name, char *file, int line)
{
    printerr(err, file, line, name);
}
void errorcurrent(int err)
{
    printerr(err, includes->fname, includes->line);
}
void getns(char *buf, SYMBOL *nssym)
{
    if (nssym->parentNameSpace)
    {
        getns(buf, nssym->parentNameSpace);
        strcat(buf, "::");
    }
    strcat(buf, nssym->name);
}
void getcls(char *buf, SYMBOL *clssym)
{
    if (clssym->parentClass)
    {
        getcls(buf, clssym->parentClass);
        strcat(buf, "::");
    }
    else if (clssym->parentNameSpace)
    {
        getns(buf, clssym->parentNameSpace);
        strcat(buf, "::");
    }
    strcat(buf, clssym->name);
}
void errorqualified(int err, SYMBOL *strSym, NAMESPACEVALUES *nsv, char *name)
{
    char buf[4096];
    char unopped[2048];
    char *last = "typename";
    char lastb[2048];
    memset(buf, 0, sizeof(buf));
    if (strSym)
    {
        unmangle(lastb, strSym->decoratedName);
        last = strrchr(lastb, ':');
        if (last)
            last++;
        else
            last = lastb;        
    }
    if (*name == '$')
    {
        *unopped = 0;
        if (name[1] == 'b' || name[1] == 'o')
        {
            unmang_intrins(unopped, name, last);
        }
        else
        {
            unmang1(unopped, name+1, last, FALSE);
        }
    }
    else
    {
        strcpy(unopped, name);
    }
    my_sprintf(buf, "'%s' is not a member of '", unopped);
    if (strSym)
    {
        typeToString(buf + strlen(buf), strSym->tp);
//        getcls(buf, strSym);
    }    
    else if (nsv)
    {
        getns(buf, nsv->name);
    }
    strcat(buf, "'");
    if (strSym && !strSym->tp->syms)
        strcat(buf, " because the type is not defined");
    printerr(err, preprocFile, preprocLine, buf);
}
void errorNotMember( SYMBOL *strSym, NAMESPACEVALUES *nsv, char *name)
{
    errorqualified(ERR_NAME_IS_NOT_A_MEMBER_OF_NAME, strSym, nsv, name);
}
void error(int err)
{
    printerr(err, preprocFile, preprocLine);
}
void errorint(int err, int val)
{
    printerr(err, preprocFile, preprocLine, val);
}
void errorstr(int err, char *val)
{
    printerr(err, preprocFile, preprocLine, val);
}
void errorstr2(int err, char *val, char *two)
{
    printerr(err, preprocFile, preprocLine, val, two);
}
void errorsym(int err, SYMBOL *sym)
{
    char buf[2048];
#ifdef CPREPROCESSOR
    strcpy(buf, sym->name);
#else
    if (!sym->errname)
    {
        SetLinkerNames(sym, lk_cdecl);
    }
    unmangle(buf, sym->errname);
#endif
    printerr(err, preprocFile, preprocLine, buf);
}
#ifndef CPREPROCESSOR
void errorsym2(int err, SYMBOL *sym1, SYMBOL *sym2)
{
    char one[2048], two[2048];
    unmangle(one, sym1->errname);
    unmangle(two, sym2->errname);
    printerr(err, preprocFile, preprocLine, one, two);
}
void errorstrsym(int err, char *name, SYMBOL *sym2)
{
    char two[2048];
    unmangle(two, sym2->errname);
    printerr(err, preprocFile, preprocLine, name, two);
}
void errorstringtype(int err, char *str, TYPE *tp1)
{
    char tpb1[4096];
    memset(tpb1, 0, sizeof(tpb1));
    typeToString(tpb1, tp1);
    printerr(err, preprocFile, preprocLine, str, tpb1);
}
                                                   
void errortype (int err, TYPE *tp1, TYPE *tp2)
{
    char tpb1[4096], tpb2[4096];
    memset(tpb1, 0, sizeof(tpb1));
    memset(tpb2, 0, sizeof(tpb2));
    typeToString(tpb1, tp1);
    if (tp2)
        typeToString(tpb2, tp2);
    printerr(err, preprocFile, preprocLine, tpb1, tpb2);
}
void errorabstract(int error, SYMBOL *sp)
{
    SYMBOL *sp1;
    errorsym(error, sp);
    sp1 = calculateStructAbstractness(sp, sp);
    if (sp)
    {
        errorsym2(ERR_ABSTRACT_BECAUSE, sp, sp1);
    }
}
void errorarg(int err, int argnum, SYMBOL *declsp, SYMBOL *funcsp)
{
    char argbuf[2048];
    char buf[2048];
    if (declsp->anonymous)
        my_sprintf(argbuf,"%d",argnum);
    else
    {
        unmangle(argbuf, declsp->errname);
    }
    unmangle(buf, funcsp->errname);
    currentErrorLine = 0;
    printerr(err, preprocFile, preprocLine, argbuf, buf);
}
static BALANCE *newbalance(LEXEME *lex, BALANCE *bal)
{
    BALANCE *rv = (BALANCE *)Alloc(sizeof(BALANCE));
    rv->back = bal;
    rv->count = 0;
    if (KW(lex) == openpa)
        rv->type = BAL_PAREN;
    else if (KW(lex) == openbr)
        rv->type = BAL_BRACKET;
    else
        rv->type = BAL_BEGIN;
    return (rv);
}
static void setbalance(LEXEME *lex, BALANCE **bal)
{
    switch (KW(lex))
    {
        case end:
            while (*bal && (*bal)->type != BAL_BEGIN)
            {
                (*bal) = (*bal)->back;
            }
            if (*bal && !(--(*bal)->count))
                (*bal) = (*bal)->back;
            break;
        case closepa:
            while (*bal && (*bal)->type != BAL_PAREN)
            {
                (*bal) = (*bal)->back;
            }
            if (*bal && !(--(*bal)->count))
                (*bal) = (*bal)->back;
            break;
        case closebr:
            while (*bal && (*bal)->type != BAL_BRACKET)
            {
                (*bal) = (*bal)->back;
            }
            if (*bal && !(--(*bal)->count))
                (*bal) = (*bal)->back;
            break;
        case begin:
            if (! *bal || (*bal)->type != BAL_BEGIN)
                *bal = newbalance(lex, *bal);
            (*bal)->count++;
            break;
        case openpa:
            if (! *bal || (*bal)->type != BAL_PAREN)
                *bal = newbalance(lex, *bal);
            (*bal)->count++;
            break;

        case openbr:
            if (! *bal || (*bal)->type != BAL_BRACKET)
                *bal = newbalance(lex, *bal);
            (*bal)->count++;
            break;
    }
}

/*-------------------------------------------------------------------------*/

void errskim(LEXEME **lex, enum e_kw *skimlist)
{
    BALANCE *bal = 0;
    while (TRUE)
    {
        if (!*lex)
            break;
        if (!bal)
        {
            int i;
            enum e_kw kw = KW(*lex);
            for (i = 0; skimlist[i]; i++)
                if ( kw == skimlist[i])
                    return ;
        }
        setbalance(*lex, &bal);
        *lex = getsym();
    }
}
void skip(LEXEME **lex, enum e_kw kw)
{
    if (MATCHKW(*lex, kw))
        *lex = getsym();
}
BOOLEAN needkw(LEXEME **lex, enum e_kw kw)
{
    if (MATCHKW(*lex, kw))
    {
        *lex = getsym();
        return TRUE;
    }
    else
    {
        errorint(ERR_NEEDY, kwtosym(kw));
        return FALSE;
    }
}
void specerror(int err, char *name, char *file, int line)
{
    printerr(err, file, line, name);
}
void diag(char *fmt, ...)
{
    if (cparams.prm_diag)
    {
        va_list argptr;
    
        va_start(argptr, fmt);
        printf("Diagnostic: ");
        vprintf(fmt, argptr);
        if (theCurrentFunc)
            printf(":%s", theCurrentFunc->decoratedName);
        printf("\n");
        va_end(argptr);
    }
    diagcount++;
}
void printToListFile(char *fmt, ...)
{
    if (cparams.prm_listfile)
    {
        va_list argptr;
    
        va_start(argptr, fmt);
        vfprintf(listFile, fmt, argptr);
        va_end(argptr);
    }
}
void ErrorsToListFile(void)
{
    if (cparams.prm_listfile)
    {
        while (listErrors)
        {
            printToListFile("%s\n", listErrors->data);
            listErrors = listErrors->next;
        }
    }
    else
        listErrors = 0;
}
void AddErrorToList(char *tag, char *str)
{
    if (cparams.prm_listfile)
    {
        char buf[512];
        char *p ;
        LIST *l;
        my_sprintf(buf, "******** %s: %s", tag, str);
        p = litlate(buf);
        l = Alloc(sizeof(LIST));
        l->data = p;
        l->next = listErrors;
        listErrors = l;	
    }
}
static BOOLEAN hasGoto(STATEMENT *stmt)
{
    while (stmt)
    {
        switch(stmt->type)
        {
            case st_block:
            case st_switch:
            case st_try:
            case st_catch:
                if (hasGoto(stmt->lower))
                    return TRUE;
                break;
            case st_declare:
            case st_expr:
                break;
            case st_goto:
                return TRUE;
            case st_return:
            case st_select:
            case st_notselect:
            case st_label:
            case st_line:
            case st_passthrough:
            case st_datapassthrough:
            case st_asmcond:
            case st_varstart:
            case st_dbgblock:
                break;
            case st_nop:
                break;
            default:
                diag("unknown stmt type in hasgoto");
                break;
        }
        stmt = stmt->next;
    }
    return FALSE;
}
static BOOLEAN findVLAs(STATEMENT *stmt)
{
    while (stmt)
    {
        switch(stmt->type)
        {
            case st_block:
            case st_switch:
            case st_try:
            case st_catch:
                if (findVLAs(stmt->lower))
                    return TRUE;
                break;
            case st_declare:
            case st_expr:
                if (stmt->hasvla || stmt->hasdeclare)
                    return TRUE;
                break;
            case st_return:
            case st_goto:
            case st_select:
            case st_notselect:
            case st_label:
            case st_line:
            case st_passthrough:
            case st_datapassthrough:
            case st_asmcond:
            case st_varstart:
            case st_dbgblock:
                break;
            case st_nop:
                break;
            default:
                diag("unknown stmt type in findvlas");
                break;
        }
        stmt = stmt->next;
    }
    return FALSE;
}
typedef struct vlaShim
{
    struct vlaShim *next, *prev;
    enum { v_label, v_goto, v_vla, v_declare, v_blockstart, v_blockend } type;
    STATEMENT *stmt;
    int level;
    int label;
    int line;
    char *file;
} VLASHIM ;
/* thisll be sluggish if there are lots of gotos & labels... */
static void getVLAList(STATEMENT *stmt, VLASHIM ***shims, VLASHIM **prev, int level)
{
    while (stmt)
    {
        switch(stmt->type)
        {
            case st_block:
            case st_switch:
            case st_try:
            case st_catch:
                **shims = Alloc(sizeof(VLASHIM));
                (**shims)->prev = *prev;
                (**shims)->type = v_blockstart;
                (**shims)->level = level ;
                (**shims)->label = stmt->label;
                (**shims)->line = stmt->line;
                (**shims)->file = stmt->file;
                (**shims)->stmt = stmt;
                *prev = **shims;
                *shims = &(**shims)->next;
                getVLAList(stmt->lower, shims, prev, level + 1);
                if (stmt->blockTail)
                {
                    **shims = Alloc(sizeof(VLASHIM));
                    (**shims)->prev = *prev;
                    (**shims)->type = v_blockend;
                    (**shims)->level = level + 1;
                    (**shims)->label = stmt->label;
                    (**shims)->line = stmt->line;
                    (**shims)->file = stmt->file;
                    (**shims)->stmt = stmt;
                    *prev = **shims;
                    *shims = &(**shims)->next;
                }
                break;
            case st_declare:
            case st_expr:
                if (stmt->hasvla)
                {
                    **shims = Alloc(sizeof(VLASHIM));
                    (**shims)->prev = *prev;
                    (**shims)->type = v_vla;
                    (**shims)->level = level;
                    (**shims)->label = stmt->label;
                    (**shims)->line = stmt->line;
                    (**shims)->file = stmt->file;
                    (**shims)->stmt = stmt;
                    *prev = **shims;
                    *shims = &(**shims)->next;
                }
                else if (stmt->hasdeclare)
                {
                    **shims = Alloc(sizeof(VLASHIM));
                    (**shims)->prev = *prev;
                    (**shims)->type = v_declare;
                    (**shims)->level = level;
                    (**shims)->label = stmt->label;
                    (**shims)->line = stmt->line;
                    (**shims)->file = stmt->file;
                    (**shims)->stmt = stmt;
                    *prev = **shims;
                    *shims = &(**shims)->next;
                }
                break;
            case st_return:
            case st_select:
            case st_notselect:
            case st_line:
            case st_passthrough:
            case st_datapassthrough:
            case st_asmcond:
            case st_varstart:
            case st_dbgblock:
                break;
            case st_nop:
                break;
            case st_goto:
                **shims = Alloc(sizeof(VLASHIM));
                (**shims)->prev = *prev;
                (**shims)->type = v_goto;
                (**shims)->level = level;
                (**shims)->label = stmt->label;
                (**shims)->line = stmt->line;
                (**shims)->file = stmt->file;
                (**shims)->stmt = stmt;
                *prev = **shims;
                *shims = &(**shims)->next;
                break;
            case st_label:
                **shims = Alloc(sizeof(VLASHIM));
                (**shims)->prev = *prev;
                (**shims)->type = v_label;
                (**shims)->level = level;
                (**shims)->label = stmt->label;
                (**shims)->line = stmt->line;
                (**shims)->file = stmt->file;
                (**shims)->stmt = stmt;
                *prev = **shims;
                *shims = &(**shims)->next;
                break;
            default:
                diag("unknown stmt type in checkvla");
                break;
        }
        stmt = stmt->next;
    }
}
static void vlaError(VLASHIM *gotoShim, VLASHIM *errShim)
{
    char buf[256];
    my_sprintf(buf, "%d", errShim->line);
    currentErrorLine = 0;
    specerror(ERR_GOTO_BYPASSES_VLA_INITIALIZATION, buf, gotoShim->file, gotoShim->line);
}
static void declError(VLASHIM *gotoShim, VLASHIM *errShim)
{
    char buf[256];
    my_sprintf(buf, "%d", errShim->line);
    currentErrorLine = 0;
    specerror(ERR_GOTO_BYPASSES_INITIALIZATION, buf, gotoShim->file, gotoShim->line);
}
void checkGotoPastVLA(STATEMENT *stmt, BOOLEAN first)
{
    
    if (hasGoto(stmt) && findVLAs(stmt))
    {
        VLASHIM *vlaList = NULL, **pvlaList = &vlaList, *prev = NULL, *gotop;
        if (first)
            while (stmt && stmt->type == st_declare)
                stmt = stmt->next;
        getVLAList(stmt, &pvlaList, &prev, 0);
        gotop = vlaList;
        while (gotop)
        {
            if (gotop->type == v_goto)
            {
                VLASHIM *lblp = vlaList, *temp;
                BOOLEAN after = FALSE;
                while (lblp)
                {
                    if (lblp == gotop)
                        after = TRUE;
                    else
                        if (lblp->type == v_label && lblp->label == gotop->label)
                        {
                            int level, minlevel;
                            if (after)
                            {
                                level = lblp->level;
                                temp = lblp->prev;
                                while (temp != gotop)
                                {
                                    if (temp->level <= level)
                                    {
                                        if (temp->type == v_vla)
                                            vlaError(gotop, temp);
                                        else if (temp->type == v_declare)
                                            declError(gotop, temp);
                                        level = temp->level;
                                    }
                                    temp = temp->prev;
                                }
                            }
                            else
                            {
                                level = gotop->level;
                                temp = gotop->prev;
                                while (lblp != temp)
                                {
                                    if (temp->level < level)
                                    {
                                        level = temp->level;
                                    }
                                    temp = temp->prev;
                                }
                                temp = lblp;
                                while (temp && temp->level > level)
                                {
                                    if (temp->type == v_vla)
                                        vlaError(gotop, temp);
                                    else if (temp->type == v_declare)
                                        declError(gotop, temp);
                                    temp = temp->prev;
                                }
                            }
                            temp = gotop;
                            minlevel = temp->level;
                            while (temp && temp->level >= level)
                            {
                                if (temp->level <= minlevel)
                                {
                                    if (temp->type == v_blockend)
                                    {
                                          STATEMENT *st = stmtNode(NULL, NULL, st_expr);
                                        *st = *gotop->stmt;
                                        gotop->stmt->type = st_expr;
                                        gotop->stmt->select = temp->stmt->blockTail->select;
                                        gotop->stmt->next = st;
                                        gotop->stmt = st;
                                    }
                                    minlevel = temp->level;
                                }
                                temp = temp->next;
                            }
                            break;
                        }
                    lblp = lblp->next;
                }
            }
            gotop = gotop->next;
        }
    }
    
}
void checkUnlabeledReferences(BLOCKDATA *block)
{
    int i;
    for (i=0; i < labelSyms->size; i++)
    {
        HASHREC *hr = labelSyms->table[i];
        while (hr)
        {
            SYMBOL *sp = (SYMBOL *)hr->p;
            if (sp->storage_class == sc_ulabel)
            {
                STATEMENT *st;
                specerror(ERR_UNDEFINED_LABEL, sp->name, sp->declfile, sp->declline);
                sp->storage_class = sc_label;
                st = stmtNode(NULL, block, st_label);
                st->label = sp->offset;
            }
            hr = hr->next;
        }
    }
}
void checkUnused(HASHTABLE *syms)
{
    int i;
    for (i=0; i < syms->size; i++)
    {
        HASHREC *hr = syms->table[i];
        while (hr)
        {
            SYMBOL *sp = (SYMBOL *)hr->p;
            if (sp->storage_class == sc_overloads)
                sp = (SYMBOL *)sp->tp->syms->table[0]->p;
            if (!sp->used && !sp->anonymous)
            {
                if (sp->assigned || sp->altered)
                {
                    if (sp->storage_class == sc_auto || 
                        sp->storage_class == sc_register ||
                        sp->storage_class == sc_parameter)
                    errorsym(ERR_SYM_ASSIGNED_VALUE_NEVER_USED, sp);
                }
                else
                {
                    if (sp->storage_class == sc_parameter)
                        errorsym(ERR_UNUSED_PARAMETER, sp);
                    else
                        errorsym(ERR_UNUSED_VARIABLE, sp);
                }
            }
            hr = hr->next;
        }
    }
}
void findUnusedStatics(NAMESPACEVALUES *nameSpace)
{
    int i;
    for (i= 0; i <nameSpace->syms->size; i++)
    {
        HASHREC *hr = nameSpace->syms->table[i];
        while (hr)
        {
            SYMBOL *sp = (SYMBOL *)hr->p;
            if (sp)
            {
                if (sp->storage_class == sc_namespace)
                {
                    findUnusedStatics(sp->nameSpaceValues);
                }
                else
                {
                    if (sp->storage_class == sc_overloads)
                    {
                        HASHREC *hr = sp->tp->syms->table[0];
                        while (hr)
                        {
                            SYMBOL *sp = (SYMBOL *)hr->p;
                            if (sp->isInline && !sp->inlineFunc.stmt && !sp->templateLevel)
                            {
                                errorsym(ERR_UNDEFINED_IDENTIFIER, sp);
                            }
                            hr = hr->next;
                        }
                    }
                    else
                    {
                        currentErrorLine = 0;
                        if (sp->storage_class == sc_static && !sp->used)
                            errorsym(ERR_UNUSED_STATIC, sp);
                        currentErrorLine = 0;
                        if (sp->storage_class == sc_global || sp->storage_class == sc_static
                            || sp->storage_class == sc_localstatic)
                            /* void will be caught earlier */
                            if (!isfunction(sp->tp) && !isarray(sp->tp) && sp->tp->size == 0 && !isvoid(sp->tp) && sp->tp->type != bt_any)
                                errorsym(ERR_UNSIZED, sp);
                    }
                }
            }
            hr = hr->next;
        }
    }
}
static void usageErrorCheck(SYMBOL *sp)
{
    if ((sp->storage_class == sc_auto || sp->storage_class == sc_register || sp->storage_class == sc_localstatic)
        && !sp->assigned && !sp->used && !sp->altered)
    {
        if (!structLevel || !sp->deferredCompile)
            errorsym(ERR_USED_WITHOUT_ASSIGNMENT, sp);
    }
    sp->used = TRUE;
}
static SYMBOL *getAssignSP(EXPRESSION *exp)
{
    SYMBOL *sp;
    switch(exp->type)
    {
        case en_label:
        case en_global:
        case en_auto:
            return exp->v.sp;
        case en_add:
        case en_structadd:
        case en_arrayadd:
            if ((sp = getAssignSP(exp->left)) != 0)
                return sp;
            return getAssignSP(exp->right);
        default:
            return NULL;
        
    }
}
static void assignmentAssign(EXPRESSION *left, BOOLEAN assign)
{
    while (castvalue(left))
    {
        left = left->left;
    }
    if (lvalue(left))
    {
        SYMBOL *sp;
        sp = getAssignSP(left->left);
        if (sp)
        {
            if (sp->storage_class == sc_auto || 
                sp->storage_class == sc_register ||
                sp->storage_class == sc_parameter)
            {
                if (assign)
                    sp->assigned = TRUE;
                sp->altered = TRUE;
//				sp->used = FALSE;
            }
        }
    }
}
void assignmentUsages(EXPRESSION *node, BOOLEAN first)
{
    FUNCTIONCALL *fp;
    if (node == 0)
        return;
    switch (node->type)
    {
        case en_auto:
            node->v.sp->used = TRUE;
            break;
        case en_const:
        case en_msil_array_access:
            break;
        case en_c_ll:
        case en_c_ull:
        case en_c_d:
        case en_c_ld:
        case en_c_f:
        case en_c_dc:
        case en_c_ldc:
        case en_c_fc:
        case en_c_di:
        case en_c_ldi:
        case en_c_fi:
        case en_c_i:
        case en_c_l:
        case en_c_ui:
        case en_c_ul:
        case en_c_c:
        case en_c_bool:
        case en_c_uc:
        case en_c_wc:
        case en_c_u16:
        case en_c_u32:        
        case en_c_string:
        case en_nullptr:
        case en_memberptr:
        case en_structelem:
            break;
        case en_global:
        case en_label:
        case en_pc:
        case en_labcon:
        case en_absolute:
        case en_threadlocal:
            break;
        case en_l_sp:
        case en_l_fp:
        case en_bits:
        case en_l_f:
        case en_l_d:
        case en_l_ld:
        case en_l_fi:
        case en_l_di:
        case en_l_ldi:
        case en_l_fc:
        case en_l_dc:
        case en_l_ldc:
        case en_l_c:
        case en_l_wc:
        case en_l_u16:
        case en_l_u32:
        case en_l_s:
        case en_l_ul:
        case en_l_l:
        case en_l_p:
        case en_l_ref:
        case en_l_i:
        case en_l_ui:
        case en_l_inative:
        case en_l_unative:
        case en_l_uc:
        case en_l_us:
        case en_l_bool:
        case en_l_bit:
        case en_l_ll:
        case en_l_ull:
        case en_l_string:
        case en_l_object:
            if (node->left->type == en_auto)
            {
                if (!first)
                    usageErrorCheck(node->left->v.sp);
            }
            else
            {
                assignmentUsages(node->left, FALSE);
            }
            break;
        case en_uminus:
        case en_compl:
        case en_not:
        case en_x_f:
        case en_x_d:
        case en_x_ld:
        case en_x_fi:
        case en_x_di:
        case en_x_ldi:
        case en_x_fc:
        case en_x_dc:
        case en_x_ldc:
        case en_x_ll:
        case en_x_ull:
        case en_x_i:
        case en_x_ui:
        case en_x_c:
        case en_x_u16:
        case en_x_u32:
        case en_x_wc:
        case en_x_uc:
        case en_x_bool:
        case en_x_bit:
        case en_x_inative:
        case en_x_unative:
        case en_x_s:
        case en_x_us:
        case en_x_l:
        case en_x_ul:
        case en_x_p:
        case en_x_fp:
        case en_x_sp:
        case en_x_string:
        case en_x_object:
        case en_trapcall:
        case en_shiftby:
/*        case en_movebyref: */
        case en_substack:
        case en_alloca:
        case en_loadstack:
        case en_savestack:
        case en_literalclass:
            assignmentUsages(node->left, FALSE);
            break;
        case en_assign:
        case en__initblk:
        case en__cpblk:
            assignmentUsages(node->right, FALSE);
            assignmentUsages(node->left, TRUE);
            assignmentAssign(node->left, TRUE);
            break;
        case en_autoinc:
        case en_autodec:
            assignmentUsages(node->left, FALSE);
            assignmentAssign(node->left, TRUE);
            break; 
        case en_add:
        case en_sub:
/*        case en_addcast: */
        case en_lsh:
        case en_arraylsh:
        case en_rsh:
        case en_rshd:
        case en_void:
        case en_voidnz:
/*        case en_dvoid: */
        case en_arraymul:
        case en_arrayadd:
        case en_arraydiv:
        case en_structadd:
        case en_mul:
        case en_div:
        case en_umul:
        case en_udiv:
        case en_umod:
        case en_ursh:
        case en_mod:
        case en_and:
        case en_or:
        case en_xor:
        case en_lor:
        case en_land:
        case en_eq:
        case en_ne:
        case en_gt:
        case en_ge:
        case en_lt:
        case en_le:
        case en_ugt:
        case en_uge:
        case en_ult:
        case en_ule:
        case en_cond:
        case en_intcall:
        case en_stackblock:
        case en_blockassign:
        case en_mp_compare:
/*		case en_array: */
            assignmentUsages(node->left, FALSE);
            assignmentUsages(node->right, FALSE);
            break;
        case en_mp_as_bool:
        case en_blockclear:
        case en_argnopush:
        case en_not_lvalue:
        case en_thisref:
        case en_lvalue:
        case en_funcret:
            assignmentUsages(node->left, FALSE);
            break;
        case en_atomic:
            assignmentUsages(node->v.ad->flg, FALSE);
            assignmentUsages(node->v.ad->memoryOrder1, FALSE);
            assignmentUsages(node->v.ad->memoryOrder2, FALSE);
            assignmentUsages(node->v.ad->address, FALSE);
            assignmentUsages(node->v.ad->third, FALSE);
            break;
        case en_func:
            fp = node->v.func;
            {
                INITLIST *args = fp->arguments;
                while (args)
                {
                    assignmentUsages(args->exp, FALSE);
                    args = args->next;
                }
                if (cparams.prm_cplusplus && fp->thisptr && !fp->fcall)
                {
                    error(ERR_MUST_CALL_OR_TAKE_ADDRESS_OF_MEMBER_FUNCTION);                    
                }
            }
            break;
        case en_stmt:
        case en_templateparam:
        case en_templateselector:
        case en_packedempty:
        case en_sizeofellipse:
            break;
        default:
            diag("assignmentUsages");
            break;
    }
}
static int checkDefaultExpression(EXPRESSION *node)
{
    FUNCTIONCALL *fp;
    BOOLEAN rv = FALSE;
    if (node == 0)
        return 0;
    switch (node->type)
    {
        case en_auto:
            if (!node->v.sp->anonymous)
                rv = TRUE;
            break;
        case en_const:
            break;
        case en_c_ll:
        case en_c_ull:
        case en_c_d:
        case en_c_ld:
        case en_c_f:
        case en_c_dc:
        case en_c_ldc:
        case en_c_fc:
        case en_c_di:
        case en_c_ldi:
        case en_c_fi:
        case en_c_i:
        case en_c_l:
        case en_c_ui:
        case en_c_ul:
        case en_c_c:
        case en_c_bool:
        case en_c_uc:
        case en_c_wc:
        case en_c_u16:
        case en_c_u32:        
        case en_nullptr:
        case en_structelem:
        case en_c_string:
            break;
        case en_global:
        case en_label:
        case en_pc:
        case en_labcon:
        case en_absolute:
        case en_threadlocal:
            break;
        case en_l_sp:
        case en_l_fp:
        case en_bits:
        case en_l_f:
        case en_l_d:
        case en_l_ld:
        case en_l_fi:
        case en_l_di:
        case en_l_ldi:
        case en_l_fc:
        case en_l_dc:
        case en_l_ldc:
        case en_l_c:
        case en_l_wc:
        case en_l_u16:
        case en_l_u32:
        case en_l_s:
        case en_l_ul:
        case en_l_l:
        case en_l_p:
        case en_l_ref:
        case en_l_i:
        case en_l_ui:
        case en_l_inative:
        case en_l_unative:
        case en_l_uc:
        case en_l_us:
        case en_l_bool:
        case en_l_bit:
        case en_l_ll:
        case en_l_ull:
        case en_l_string:
        case en_l_object:
        case en_literalclass:
            rv |= checkDefaultExpression(node->left);
            break;
        case en_uminus:
        case en_compl:
        case en_not:
        case en_x_f:
        case en_x_d:
        case en_x_ld:
        case en_x_fi:
        case en_x_di:
        case en_x_ldi:
        case en_x_fc:
        case en_x_dc:
        case en_x_ldc:
        case en_x_ll:
        case en_x_ull:
        case en_x_i:
        case en_x_ui:
        case en_x_inative:
        case en_x_unative:
        case en_x_c:
        case en_x_u16:
        case en_x_u32:
        case en_x_wc:
        case en_x_uc:
        case en_x_bool:
        case en_x_bit:
        case en_x_s:
        case en_x_us:
        case en_x_l:
        case en_x_ul:
        case en_x_p:
        case en_x_fp:
        case en_x_sp:
        case en_x_string:
        case en_x_object:
        case en_trapcall:
        case en_shiftby:
/*        case en_movebyref: */
        case en_substack:
        case en_alloca:
        case en_loadstack:
        case en_savestack:
            rv |= checkDefaultExpression(node->left);
            break;
        case en_assign:
        case en__initblk:
        case en__cpblk:
            rv |= checkDefaultExpression(node->right);
            rv |= checkDefaultExpression(node->left);
            break;
        case en_autoinc:
        case en_autodec:
            rv |= checkDefaultExpression(node->left);
            break; 
        case en_add:
        case en_sub:
/*        case en_addcast: */
        case en_lsh:
        case en_arraylsh:
        case en_rsh:
        case en_rshd:
        case en_void:
        case en_voidnz:
/*        case en_dvoid: */
        case en_arraymul:
        case en_arrayadd:
        case en_arraydiv:
        case en_structadd:
        case en_mul:
        case en_div:
        case en_umul:
        case en_udiv:
        case en_umod:
        case en_ursh:
        case en_mod:
        case en_and:
        case en_or:
        case en_xor:
        case en_lor:
        case en_land:
        case en_eq:
        case en_ne:
        case en_gt:
        case en_ge:
        case en_lt:
        case en_le:
        case en_ugt:
        case en_uge:
        case en_ult:
        case en_ule:
        case en_cond:
        case en_intcall:
        case en_stackblock:
        case en_blockassign:
        case en_mp_compare:
/*		case en_array: */
            rv |= checkDefaultExpression(node->right);
        case en_mp_as_bool:
        case en_blockclear:
        case en_argnopush:
        case en_not_lvalue:
        case en_thisref:
        case en_lvalue:
            rv |= checkDefaultExpression(node->left);
            break;
        case en_atomic:
            rv |= checkDefaultExpression(node->v.ad->flg);
            rv |= checkDefaultExpression(node->v.ad->memoryOrder1);
            rv |= checkDefaultExpression(node->v.ad->memoryOrder2);
            rv |= checkDefaultExpression(node->v.ad->address);
            rv |= checkDefaultExpression(node->v.ad->third);
            break;
        case en_func:
            fp = node->v.func;
            {
                INITLIST *args = fp->arguments;
                while (args)
                {
                    rv |= checkDefaultExpression(args->exp);
                    args = args->next;
                }
            }
            if (fp->sp->parentClass && fp->sp->parentClass->islambda)
                rv |= 2;
            break;
        case en_stmt:
        case en_templateparam:
        case en_templateselector:
            break;
        default:
            diag("rv |= checkDefaultExpression");
            break;
    }
    return rv;
}
void checkDefaultArguments(SYMBOL *spi)
{
    INITIALIZER *p = spi->init;
    int r = 0;
    while(p)
    {
        r |= checkDefaultExpression(p->exp);
        p = p->next;
    }
    if (r & 1)
    {
        error(ERR_NO_LOCAL_VAR_OR_PARAMETER_DEFAULT_ARGUMENT);
    }
    if (r & 2)
    {
        error(ERR_LAMBDA_CANNOT_CAPTURE);
    }
}
#endif