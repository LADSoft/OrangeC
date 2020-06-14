/* Software License Agreement
 *
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 */

#include "compiler.h"
#include <stdarg.h>
#include "PreProcessor.h"
#include "Utils.h"
#include "Errors.h"
#include "ccerr.h"
#include "config.h"
#include "declare.h"
#include "lex.h"
#include "template.h"
#include "occparse.h"
#include "stmt.h"
#include "symtab.h"
#include "unmangle.h"
#include "OptUtils.h"
#include "memory.h"
#include "mangle.h"
#include "lex.h"
#include "types.h"
#include "help.h"
#define ERROR 1
#define WARNING 2
#define TRIVIALWARNING 4
#define ANSIERROR 8
#define ANSIWARNING 16
#define CPLUSPLUSERROR 32

namespace Parser
{

int currentErrorLine;
SYMBOL* theCurrentFunc;
enum e_kw skim_end[] = {end, kw_none};
enum e_kw skim_closepa[] = {closepa, semicolon, end, kw_none};
enum e_kw skim_semi[] = {semicolon, end, kw_none};
enum e_kw skim_semi_declare[] = {semicolon, kw_none};
enum e_kw skim_closebr[] = {closebr, semicolon, end, kw_none};
enum e_kw skim_comma[] = {comma, closepa, closebr, semicolon, end, kw_none};
enum e_kw skim_colon[] = {colon, kw_case, kw_default, semicolon, end, kw_none};
enum e_kw skim_templateend[] = {gt, semicolon, end, kw_none};

static Optimizer::LIST* listErrors;
static const char* currentErrorFile;

static struct
{
    const char* name;
    int level;
} errors[] = {
    {"Unknown error", ERROR},
    {"Too many errors or warnings", ERROR},
    {"Constant too large", ERROR},
    {"Expected constant", ERROR},
    {"Invalid constant", ERROR},
    {"Invalid floating point constant", ERROR},
    {"Invalid character constant", ERROR},
    {"Unterminated character constant", ERROR},
    {"Invalid string constant", ERROR},
    {"Unterminated string constant", ERROR},
    {"String constant too long", ERROR},
    {"Syntax error: %c expected", ERROR},
    {"Syntax error: string constant expected", ERROR},
    {"Expected constant or address", ERROR},
    {"Expected integer type", ERROR},
    {"Expected integer expression", ERROR},
    {"Identifier expected", ERROR},
    {"Multiple declaration of '%s'", ERROR},
    {"Undefined symbol '%s'", ERROR},
    {"Too many identififiers in type", ERROR},
    {"Unexpected end of file", ERROR},
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
    {"Unknown symbol '%c'", ERROR},
    {"Size of '%s' is unknown or zero", ERROR},
    {"Size of the type '%s' is unknown or zero", ERROR},
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
    {"Bit field must be a non-static member of a structured type", ERROR},
    {"Bit field type must be 'int', 'unsigned', or '_Bool'", ANSIERROR},
    {"Bit field must be an integer type", ERROR},
    {"Bit field too large", ERROR},
    {"Bit field must contain at least one bit", ERROR},
    {"%s", ERROR},  //"'%s' is not a member of '%s'", ERROR },
    {"Pointer to structure required to left of '%s'", ERROR},
    {"Structure required to left of '%s'", ERROR},
    {"Storage class '%s' not allowed here", ERROR},
    {"_absolute storage class requires address", ERROR},
    {"Too many storage class specifiers", ERROR},
    {"Declarations for '%s' use both 'extern' and 'static' storage classes", WARNING | ANSIERROR},
    {"Static object '%s' may not be declared within inline function", ERROR}, /* fixme */
    {"Static object '%s' may not be referred to within inline function", ERROR},
    {"Linkage specifier not allowed here", ERROR},
    {"Too many linkage specifiers in declaration", ERROR},
    {"Declaration syntax error", ERROR},
    {"Redeclaration for '%s' with different qualifiers", ERROR},
    {"Declaration not allowed here", ERROR},
    {"'%s' has already been included", ERROR},
    {"Missing type in declaration", WARNING},
    {"Missing type for parameter '%s'", WARNING},
    {"'%s' is a C99 or C11 keyword - did you mean to specify C99 or C11 mode?", WARNING},
    {"Too many types in declaration", ERROR},
    {"Too many identifiers in declaration", ERROR},
    {"Type '%s' requires C99", ERROR},
    {"Type 'void' not allowed here", ERROR},
    {"Incompatible type conversion", ERROR},
    {"Multiple declaration for type '%s'", ERROR},
    {"Too many type qualifiers", ERROR},
    {"Type mismatch in redeclaration of '%s'", ERROR},
    {"Two operands must evaluate to same type", ERROR},
    {"Variably modified type requires C99", ERROR},
    {"Variably modified type allowed only in functions or parameters", ERROR},
    {"Unsized Variable length array may only be used in a prototype", ERROR},
    {"Array qualifier requires C99", ERROR},
    {"Array qualifier must be on last element of array", ERROR},
    {"Array qualifier must be on a function parameter", ERROR},
    {"Only first array size specifier may be empty", ERROR},
    {"Array size specifier must be of integer type", ERROR},
    {"Array declaration or use needs closebracket", ERROR},
    {"Array initialization designator not within array bounds", ERROR},
    {"Array must have at least one element", ERROR},
    {"Nonsized array must be last element of structure", ERROR},
    {"Array expected", ERROR},
    {"Array designator out of range", ERROR},
    {"Type mismatch in redeclaration of function '%s'", ERROR},
    {"Untyped parameters must be used with a function body", ERROR},
    {"Function parameter expected", ERROR},
    {"Parameter '%s' missing name", ERROR},
    {"Functions may not be part of a structure or union", ERROR},
    {"Functions may not return functions or arrays", ERROR},
    {"Function should return a value", WARNING},
    {"Type void must be unnamed and the only parameter", ERROR},
    {"'inline' not allowed here", ERROR},
    {"'main' may not be declared as inline", ERROR},
    {"Function takes no arguments", ERROR},
    {"Call to function '%s' without a prototype", TRIVIALWARNING},
    {"Argument list too long in call to '%s'", ERROR},
    {"Argument list too short in call to '%s'", ERROR},
    {"Call of nonfunction", ERROR},
    {"Type mismatch in parameter '%s' in call to '%s'", ERROR},
    {"extern object may not be initialized", ERROR},
    {"typedef may not be initialized", ERROR},
    {"object with variably modified type may not be initialized", ERROR},
    {"Too many initializers", ERROR},
    {"Initialization designator requires C99", ERROR},
    {"Non-constant initialization requires C99", ERROR},
    {"Bitwise object may not be initialized", ERROR},
    {"Object of type '%s' may not be initialized", ERROR}, /* fixme */
    {"Objects with _absolute storage class may not be initialized", ERROR},
    {"Constant variable '%s' must be initialized", ERROR},
    {"String type mismatch in initialization", TRIVIALWARNING},
    {"Multiple initialization of '%s'", ERROR},
    {"Initialization bypassed", WARNING},
    {"Object with variably modified type in line '%s' bypassed by goto", ERROR},
    {"Nonportable pointer conversion", WARNING},
    {"Nonportable pointer comparison", WARNING},
    {"Nonportable pointer conversion", ERROR},
    {"Suspicious pointer conversion", WARNING},
    {"Dangerous pointer conversion", ERROR},
    {"String constant may not be used as initializer here", ERROR},
    {"Lvalue required", ERROR},
    {"Invalid indirection", ERROR},
    {"'%s' used without prior assignment", WARNING},
    {"Possible incorrect assignment", WARNING},
    {"Ansi forbids automatic conversion from void in assignment", ERROR},
    {"'%s' assigned a value that is never used", TRIVIALWARNING},
    {"Parameter '%s' unused", TRIVIALWARNING},
    {"'%s' unused", TRIVIALWARNING},
    {"static object '%s' is unused", TRIVIALWARNING},
    {"'%s' hides declaration at outer scope", WARNING},
    {"Cannot use 'void' value here", ERROR},
    {"Cannot modify a const object", ERROR},
    {"Expression syntax error", ERROR},
    {"signed/unsigned mismatch in '%s' comparison", TRIVIALWARNING},
    {"sizeof may not be used with a function designator", ERROR},
    {"Invalid structure assignment", ERROR},
    {"Invalid structure operation", ERROR},
    {"Invalid use of complex", ERROR},
    {"Invalid use of floating point", ERROR},
    {"Invalid pointer operation", ERROR},
    {"Invalid pointer addition", ERROR},
    {"Invalid pointer subtraction", ERROR},
    {"Cannot take the address of a bit field", ERROR},
    {"Cannot take address of a register variable", ERROR},
    {"Must take address of memory location", ERROR},
    {"Cannot use variable of type 'bool' here", ERROR},
    {"conditional needs ':'", ERROR},
    {"switch selection expression must be of integral type", ERROR},
    {"'break' without enclosing loop or switch", ERROR},
    {"'case' without enclosing switch", ERROR},
    {"Duplicate case '%s'", ERROR},
    {"case value must be an integer expression", ERROR},
    {"'continue' without enclosing loop", ERROR},
    {"'default' without enclosing switch", ERROR},
    {"switch statement needs open parenthesis", ERROR},
    {"switch statement needs close parenthesis", ERROR},
    {"switch statement already has default", ERROR},
    {"do statement needs while", ERROR},
    {"do-while statement needs open parenthesis", ERROR},
    {"do-while statement needs close parenthesis", ERROR},
    {"while statement needs open parenthesis", ERROR},
    {"while statement needs close parenthesis", ERROR},
    {"for statement needs semicolon", ERROR},
    {"for statement needs open parenthesis", ERROR},
    {"for statement needs close parenthesis", ERROR},
    {"if statement needs open parenthesis", ERROR},
    {"if statement needs close parenthesis", ERROR},
    {"Misplaced else statement", ERROR},
    {"goto statement needs a label specifier", ERROR},
    {"Duplicate label '%s'", ERROR},
    {"Undefined label '%s'", ERROR},
    {"Return statement should have value", WARNING},
    {"Attempt to return a value of type void", ERROR},
    {"Value specified in return of function with return type 'void'", WARNING},
    {"Type mismatch in return statement", WARNING},
    {"Returning address of a local variable", WARNING},
    {"Expression has no effect", WARNING},
    {"Unexpected block end marker", ERROR}, /* fixme */
    {"Unreachable code", TRIVIALWARNING},
    {"Ansi forbids omission of semicolon", ERROR},
    {"Pointer or reference to reference is not allowed", ERROR},
    {"Array of references is not allowed", ERROR},
    {"Reference variable '%s' must be initialized", ERROR},
    {"Reference initialization requires Lvalue", ERROR},
    {"Reference initialization of type '%s' cannot be bound to type '%s'", ERROR},
    {"Reference member '%s' in a class without constructors", ERROR},
    {"Reference member '%s' not initialized in class constructor", ERROR},
    {"Qualified reference variable not allowed", WARNING},
    {"Attempt to return reference to local variable", ERROR},
    {"Cannot convert '%s' to '%s'", ERROR},
    {"Cannot cast '%s' to '%s'", ERROR},
    {"Invalid use of namespace '%s'", ERROR},
    {"Cannot declare namespace within function", ERROR},
    {"'%s' is not a namespace", ERROR},
    {"Namespace '%s' not previously declared as inline", ERROR},
    {"Expected namespace name", ERROR},
    {"Cyclic using directive", TRIVIALWARNING},
    {"%s", ERROR},
    {"Creating temporary for variable of type '%s'", TRIVIALWARNING},
    {"Type name expected", ERROR},
    {"Ambiguity between '%s' and '%s'", ERROR},
    {"Unknown linkage specifier '%s'", WARNING},
    {"char16_t or char32_t constant too long", ERROR},
    {"Non-terminated raw string in file started at line %d", ERROR},
    {"Invalid character in raw string", ERROR},
    {"'main' may not be declared as static", ERROR},
    {"'main' may not be declared as constexpr", ERROR},
    {"constexpr expression is not const", ERROR},
    {"constexpr function '%s' does not evaluate to const", ERROR},
    {"constexpr declaration requires initializer or function body", ERROR},
    {"Variable style constexpr declaration needs simple type", ERROR},
    {"Function returning reference must return a value", ERROR},
    {"Qualified name not allowed here", ERROR},
    {"%s", ERROR},
    {"Qualifier '%s' is not a class or namespace", ERROR},
    {"Linkage mismatch in overload of function '%s'", ERROR},
    {"Overload of '%s' differs only in return type", ERROR},
    {"Could not find a match for '%s'", ERROR},
    {"Redeclaration of default argument for '%s' not allowed", ERROR},
    {"Default argument may not be a parameter or local variable", ERROR},
    {"Default argument may not be a pointer or reference to function", ERROR},
    {"Default argument not allowed in typedef", ERROR},
    {"Default argument may not use 'this'", ERROR},
    {"Default argument missing after parameter '%s'", ERROR},
    {"Deleting or defaulting existing function '%s'", ERROR},
    {"Reference to deleted function '%s'", ERROR},
    {"'main' may not be deleted", ERROR},
    {"Enumerated constant too large for base type", ERROR},
    {"Enumerated type needs name", ERROR},
    {"Enumerated base type must not be ommitted here", ERROR},
    {"Redeclaration of enumeration scope or base type", ERROR},
    {"Definition of enumeration constant not allowed here", ERROR},
    {"Attempt to use scoped enumeration as integer", WARNING},
    {"Cannot take the size of a bit field", ERROR},
    {"Cannot take the size of an unfixed enumeration", ERROR},
    {"'auto' variable '%s' needs initialization", ERROR},
    {"Type 'auto' not allowed in this context", ERROR},
    {"Reference initialization discards qualifiers", ERROR},
    {"Initialization of lvalue reference cannot use rvalue of type '%s'", ERROR},
    {"Global anonymous union not static", ERROR},
    {"Union having base classes not allowed", ERROR},
    {"Union as a base class not allowed", ERROR},
    {"Redefining access for '%s' not allowed", ERROR},
    {"'%s' is not accessible", ERROR},
    {"Declaring nonfunction '%s' virtual is not allowed", ERROR},
    {"Member function '%s' already has final or override specifier", ERROR},
    {"Namespace within class not allowed", ERROR},
    {"'%s' is not an unambiguous base class of '%s'", ERROR},
    {"Return type for virtual functions '%s' and '%s' not covariant", ERROR},
    {"Overriding final virtual function '%s' not allowed", ERROR},
    {"Function '%s' does not override another virtual function", ERROR},
    {"Ambiguity between virtual functions '%s' and '%s'", ERROR},
    {"Virtual function '%s' has been deleted while virtual function '%s' has not", ERROR},
    {"Inheriting from final base class '%s' not allowed", ERROR},
    {"Function '%s' in locally defined class needs inline body", ERROR},
    {"Class member has same name as class", ERROR},
    {"Pure specifier needs constant value of zero", ERROR},
    {"Pure, final, or override specifier needs virtual function", ERROR},
    {"Functions may not be a part of a structure or union", ERROR},
    {"Ambiguous member definition for '%s'", ERROR},
    {"Use of non-static member '%s' without object", ERROR},
    {"Non-const function '%s' called for const object", ERROR},
    {"Base class '%s' is included more than once", ERROR},
    {"Only non-static member functions may be 'const' or 'volatile'", ERROR},
    {"Illegal use of member pointer", ERROR},
    {"_Noreturn function is returning", ERROR},
    {"Duplicate type in generic expression", ERROR},
    {"Too many defaults in generic expression", ERROR},
    {"Invalid type in generic expression", ERROR},
    {"Invalid expression in generic expression", ERROR},
    {"Could not find a match for generic expression", ERROR},
    {"Invalid storage class for thread local variable", ERROR},
    {"Thread local variable cannot have auto or register storage class", ERROR},
    {"Functions cannot be thread local", WARNING},
    {"Mismatched thread_local storage class specifier", ERROR},
    {"Qualifiers not allowed with atomic type specifier", WARNING},
    {"Function or array not allowed as atomic type", ERROR},
    {"Redeclaration of '%s' outside its class not allowed", ERROR},
    {"Definition of type '%s' not allowed here", ERROR},
    {"Pointers and arrays cannot be friends", ERROR},
    {"Declarator not allowed here", ERROR},
    {"'this' may only be used in a member function", ERROR},
    {"operator '%s' not allowed", ERROR},
    {"operator '%s' must be a function", ERROR},
    {"operator '%s' must have no parameters", ERROR},
    {"operator '%s' must have one parameter", ERROR},
    {"operator '%s' must have zero or one parameter", ERROR},
    {"operator '%s' must have parameter of type int", ERROR},
    {"operator '%s' must be nonstatic when used as a member function", ERROR},
    {"operator '%s' must have structured or enumeration parameter when used as a non-member", ERROR},
    {"operator '%s' must have two parameters", ERROR},
    {"operator '%s' must have one or two parameters", ERROR},
    {"operator '%s' must have second parameter of type int", ERROR},
    {"operator '%s' must be a nonstatic member function", ERROR},
    {"operator '%s' must return a reference or pointer type", ERROR},
    {"Cannot create instance of abstract class '%s'", ERROR},
    {"Class '%s' is abstract because of ''%s' = 0'", ERROR},
    {"operator \"\" requires empty string", ERROR},
    {"operator \"\" requires identifier", ERROR},
    {"'%s' requires namespace scope", ERROR},
    {"Invalid parameters for '%s'", ERROR},
    {"Could not find a match for literal suffix '%s'", ERROR},
    {"Literal suffix mismatch", ERROR},
    {"Structured type '%s' not fully defined", ERROR},
    {"Incorrect use of destructor syntax", ERROR},
    {"Constructor or destructor for '%s' must be a function", ERROR},
    {"Default may only be used on a special function", ERROR},
    {"Constructor for '%s' must have body", ERROR},
    {"Constructor or destructor cannot be declared const or volatile", ERROR},
    {"Initializer list requires constructor", ERROR},
    {"Constructor or destructor cannot have a return type", ERROR},
    {"Cannot take address of constructor or destructor", ERROR},
    {"Pointer type expected", ERROR},
    {"Objects of type '%s' cannot be initialized with { }", ERROR},
    {"Member name required", ERROR},
    {"Member initializer required", ERROR},
    {"'%s' is not a member or base class of '%s'", ERROR},
    {"'%s' must be a nonstatic member", ERROR},
    {"Cannot find an appropriate constructor for class '%s'", ERROR},
    {"Cannot find a matching copy constructor for class '%s'", ERROR},
    {"Cannot find a default constructor for class '%s'", ERROR},
    {"'%s' is not a member of '%s', because the type is not defined", ERROR},
    {"Destructor for '%s' must have empty parameter list", ERROR},
    {"Reference member '%s' is not initialized in class constructor", ERROR},
    {"Constant member '%s' is not initialized in class constructor", ERROR},
    {"Improper use of typedef '%s'", ERROR},
    {"Return type is implicit for 'operator %s'", ERROR},
    {"Invalid Psuedo-Destructor", ERROR},
    {"Destructor name must match class name", ERROR},
    {"Must call or take the address of a member function", ERROR},
    {"Need numeric expression", ERROR},
    {"Identifier '%s' cannot have a type qualifier", ERROR},
    {"Lambda function outside function scope cannot capture variables", ERROR},
    {"Invalid lambda capture mode", ERROR},
    {"Capture item listed multiple times", ERROR},
    {"Explicit capture blocked", ERROR},
    {"Implicit capture blocked", ERROR},
    {"Cannot default parameters of lambda function", ERROR},
    {"Cannot capture this", ERROR},
    {"Must capture variables with 'auto' storage class or 'this'", ERROR},
    {"Lambda function must have body", ERROR},
    {"For-range iterator begin() and end() have mismatched types", ERROR},
    {"For-range iterator begin() and/or end() are missing", ERROR},
    {"Operator ++() missing on for-range iterator type", ERROR},
    {"Operator *() missing on for-range iterator type", ERROR},
    {"Operator ==() missing on for-range iterator type", ERROR},
    {"Indirection on for-range iterator returns wrong type", ERROR},
    {"For-range expression requires structured or array type", ERROR},
    {"For-range array expression unsized", ERROR},
    {"For-range declarator cannot be initialized", ERROR},
    {"For declarator must be initialized", ERROR},
    {"Value narrowing within { } is not allowed", ERROR},
    {"Nonstructured initializer list expected", ERROR},
    {"Cannot use initializer list here", ERROR},
    {"Initializer list requires overloaded operator []", ERROR},
    {"Cannot use array of structures as function argument", ERROR},
    {"Anonymous union cannot contain function or nested type", ERROR},
    {"Anonymous union must contain only public members", ERROR},
    {"Anonymous union must contain only nonstatic data members", ERROR},
    {"Goto on line %s bypasses initialization", CPLUSPLUSERROR | WARNING},
    {"Try keyword requires compound statement", ERROR},
    {"One or more catch handlers expected", ERROR},
    {"Catch handler requires compound statement", ERROR},
    {"catch (...) must be last catch handler", ERROR},
    {"Catch handler without try", ERROR},
    {"Constructor or Destructor returns a value", ERROR},
    {"Cannot use goto to enter a try block or catch handler", ERROR},
    {"Exception specifier of virtual function '%s' must be at least as restrictive as base class declarations", ERROR},
    {"Exception specifier of function '%s' must match earlier declarations", ERROR},
    {"Use of typeid requires '#include <typeinfo>'", ERROR},
    {"Multiple return types specified", ERROR},
    {"Cannot place attribute specifiers here", ERROR},
    {"Cannot place attribute argument clause here", ERROR},
    {"Only constructors or conversion functions may be explicit", ERROR},
    {"Implicit use of explicit constructor or conversion function", ERROR},
    {"Type '%s' is not a defined class with virtual functions", ERROR},
    {"Template may not be declared in class defined within function scope", ERROR},
    {"Template must be declared at global scope or within a class", ERROR},
    {"Packed template parameter cannot have default", ERROR},
    {"'Class' template parameter missing default", ERROR},
    {"'Class' template parameter default must refer to type", ERROR},
    {"Template 'template' parameter missing default", ERROR},
    {"'Non-type' template parameter has invalid type", ERROR},
    {"Type mismatch in default for 'non-type' template parameter", ERROR},
    {"'%s' was not previously declared as a template", ERROR},
    {"'%s' was previously declared as a template", ERROR},
    {"Template parameters do not agree with previous declaration", ERROR},
    {"Missing default values in template declaration after '%s'", ERROR},
    {"Redefinition of default value for '%s' in template redeclaration", ERROR},
    {"'Template' template parameter must name a class", ERROR},
    {"Templates must be classes, functions or data", ERROR},
    {"Cannot partially specialize a function template", ERROR},
    {"Partial specialization missing parameter from template header", ERROR},
    {"Specialization of '%s' cannot be declared before primary template", ERROR},
    {"Too few arguments passed to template '%s'", ERROR},
    {"Too many arguments passed to template '%s'", ERROR},
    {"Incorrect arguments passed to template '%s'", ERROR},
    {"Cannot instantiate template '%s' because it is not defined", ERROR},
    {"Cannot generate template specialization from '%s'", ERROR},
    {"Cannot use template '%s' without specifying specialization parameters", ERROR},
    {"Invalid template parameter", ERROR},
    {"Body has already been defined for function '%s'", ERROR},
    {"Invalid explicit specialization of '%s'", ERROR},
    {"Storage class 'extern' not allowed here", ERROR},
    {"Template '%s' is already instantiated", WARNING},
    {"Use . or -> to call '%s'", ERROR},
    {"Template argument must be a constant expression", ERROR},
    {"Pack ... specifier not allowed here", ERROR},
    {"Pack ... specifier on variable requires packed template parameter", ERROR},
    {"Pack ... specifier on expression requires packed function parameter", ERROR},
    {"Pack ... specifier must be used on function parameter", ERROR},
    {"Pack ... specifier must be used on function argument", ERROR},
    {"sizeof... argument must be a template parameter pack", ERROR},
    {"Multiple pack ... specifiers not same size", ERROR},
    {"Packed ... template parameter must be 'class' template parameter", ERROR},
    {"Packed ... template parameter expected to hold structured types", ERROR},
    {"'Class' template parameter expected to hold structured types", ERROR},
    {"Pack ... specifier required here", ERROR},
    {"'Class' template parameter expected", ERROR},
    {"Structured type expected", ERROR},
    {"Packed template parameter not allowed here", ERROR},
    {"In template instantiation started here", WARNING},
    {"Invalid use of type '%s'", ERROR},
    {"Requires template<> header", ERROR},
    {"Mutable member '%s' must be non-const", ERROR},
    {"Dependent type '%s' not a class or structured type", ERROR},
    {"Dependent type '%s' is not defined in structured type '%s'", ERROR},
    {"Constructor '%s' is not allowed", ERROR},
    {"Constant member '%s' in a class without constructors", ERROR},
    {"Delete of pointer to undefined type '%s'", WARNING},
    {"Arithmetic with pointer of type 'void *'", WARNING},
    {"Overloaded function '%s' is ambiguous in this context", ERROR},
    {"Use of an initializer-list requires '#include <initializer_list>'", ERROR},
    {"Use '&' to take the address of a member function", ERROR},
    {"ISO C++ forbids in-class initialization of non-const static member '%s'", ERROR},
    {"Need packed template parameter", ERROR},
    {"Mismatched types '%s' and '%s' while infering lambda return type", ERROR},
    {"Exception specifier blocks exceptions thrown from '%s'", WARNING},
    {"Exception specifier blocks locally thrown exception of type '%s'", WARNING},
    {"undefined external '%s'", ERROR},
    {"Delegating constructor call must be the only initializer", ERROR},
    {"Mismatch on packed template types", ERROR},
    {"Cannot use new() to allocate a reference", ERROR},
    {"This feature may only be used for imported namespaces", ERROR},
    {"Cannot use the address of a managed object", ERROR},
    {"__property qualifier not allowed on functions", ERROR},
    {"Getter for property '%s' already defined", ERROR},
    {"Setter for property '%s' already defined", ERROR},
    {"Only simple property declarations are supported", ERROR},
    {"Property cannot be declared inside function", ERROR},
    {"Must define getter for property '%s'", ERROR},
    {"Cannot modify property '%s' because there is no setter", ERROR},
    {"Cannot take address of property '%s'", ERROR},
    {"__entrypoint cannot be used on non-function '%s'", ERROR},
    {"Mismatched return types for function '%s' with auto return type", ERROR},
    {"Function '%s' with auto return type not fully defined yet", ERROR},
    {"Cannot use referenced auto as decltype argument", ERROR},
    {"constexpr function uses goto statement", ERROR},
    {"%s", WARNING},
    {"'auto' not allowed in 'using =' declaration", ERROR},
    {"'auto' not allowed in parameter declaration", ERROR},
    {"'auto' not allowed as a conversion function type", ERROR},
    {"Only base elements of an MSIL array may be assigned", ERROR},
    {"Use of __catch or __fault or __finally must be preceded by __try", ERROR},
    {"Expected __catch or __fault or __finally", ERROR},
    {"__fault or __finally can appear only once per __try block", ERROR},
    {"static function '%s' is undefined", TRIVIALWARNING},
    {"Missing type specifier for identifier '%s'", WARNING},
    {"Cannot deduce auto type from '%s'", ERROR},
    {"%s: dll interface member may not be declared in dll interface class", ERROR},
    {"%s: attempting to redefine dll interface linkage for class", WARNING},
    {"Ignoring __attribute__ specifier", WARNING},
    {"Ignoring __declspec specifier", WARNING},
    {"Invalid alignment value", ERROR},
    {"Conversion of character string to 'char *' deprecated", WARNING},
    {"Function call needs argument list", WARNING},
    {"Return value would require a temporary variable", ERROR},
    {"Declaration of '%s' conflicts with previous declaration", ERROR},
    {"Expected end of template arguments near undefined type '%s'", ERROR},
    {"Expected type.   Did you mean to use 'typename'?", ERROR},
    {"Minimum alignment for '%s' not achieved", WARNING},
    {"Invalid size used with attribute((vector()))", ERROR},
    {"Need an arithmetic type with attribute((vector()))", ERROR},
    {"Type mismatch with attribute((copy()))", ERROR},
    {"Invalid cleanup function", ERROR},
    {"Null terminated string required", ERROR},
    {"Attribute namespace '%s' is not an attribute", ERROR},
    {"Attribute '%s' does not exist", ERROR},
    {"Attribute '%s' does not exist in attribute namespace '%s'", ERROR},
    {"static function '%s' is declared but never defined", TRIVIALWARNING},
    {"Array of function pointers not supported when generating System.Action references", ERROR},
    {"Passing a function pointer to native function '%s' not supported when generating System.Action references", ERROR},
};

static bool ValidateWarning(int num)
{
    if (num && num < sizeof(errors) / sizeof(errors[0]))
    {
        if (!(errors[num].level & ERROR))
        {
            return true;
        }
    }
    printf("Warning: /w index %d does not correspond to a warning\n", num);
    return false;
}
void DisableWarning(int num)
{
    if (ValidateWarning(num))
    {
        Warning::Instance()->SetFlag(num, Warning::Disable);
    }
}
void EnableWarning(int num)
{
    if (ValidateWarning(num))
    {
        Warning::Instance()->ClearFlag(num, Warning::Disable);
    }
}
void WarningOnlyOnce(int num)
{
    if (ValidateWarning(num))
    {
        Warning::Instance()->SetFlag(num, Warning::OnlyOnce);
    }
}
void WarningAsError(int num)
{
    if (ValidateWarning(num))
    {
        Warning::Instance()->SetFlag(num, Warning::AsError);
    }
}
void AllWarningsAsError()
{
    for (int i = 0; i < sizeof(errors) / sizeof(errors[0]); i++)
        Warning::Instance()->SetFlag(i, Warning::AsError);
}
void AllWarningsDisable()
{
    for (int i = 0; i < sizeof(errors) / sizeof(errors[0]); i++)
        Warning::Instance()->SetFlag(i, Warning::Disable);
}
void PushWarnings() { Warning::Instance()->Push(); }
void PopWarnings() { Warning::Instance()->Pop(); }
void DisableTrivialWarnings()
{
    Warning::Instance()->Clear();
    if (!Optimizer::cparams.prm_warning)
        for (int i = 0; i < sizeof(errors) / sizeof(errors[0]); i++)
            Warning::Instance()->SetFlag(i, Warning::Disable);
    if (!Optimizer::cparams.prm_extwarning)
        for (int i = 0; i < sizeof(errors) / sizeof(errors[0]); i++)
            if (errors[i].level & TRIVIALWARNING)
                Warning::Instance()->SetFlag(i, Warning::Disable);
}
static int total_errors;
int diagcount;

int TotalErrors() { return total_errors + Errors::GetErrorCount(); }
void errorinit(void)
{
    total_errors = diagcount = 0;
    currentErrorFile = nullptr;
}

static char kwtosym(enum e_kw kw)
{
    switch (kw)
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
static bool IsReturnErr(int err)
{
    switch (err)
    {
            //        case ERR_FUNCTION_SHOULD_RETURN_VALUE:
        case ERR_CALL_FUNCTION_NO_PROTO:
        case ERR_RETURN_MUST_RETURN_VALUE:
        case ERR_RETURN_NO_VALUE:
        case ERR_RETMISMATCH:
        case ERR_FUNCTION_RETURNING_ADDRESS_STACK_VARIABLE:
            return Optimizer::chosenAssembler->arch->erropts & EO_RETURNASERR;
        default:
            return false;
    }
}
static bool alwaysErr(int err)
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
            return true;
        default:
            return false;
    }
}
static bool ignoreErrtemplateNestingCount(int err)
{
    switch (err)
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
        case ERR_EXPRESSION_HAS_NO_EFFECT:
        case ERR_FUNCTION_SHOULD_RETURN_VALUE:
        case ERR_AMBIGUITY_BETWEEN:
        case ERR_NO_DEFAULT_CONSTRUCTOR:
        case ERR_NO_APPROPRIATE_CONSTRUCTOR:
        case ERR_ILL_STRUCTURE_ASSIGNMENT:
        case ERR_ILL_STRUCTURE_OPERATION:
            return true;
    }
    return false;
}
bool printerrinternal(int err, const char* file, int line, va_list args)
{
    char buf[2048];
    char infunc[2048];
    const char* listerr;
    char nameb[265], *name = nameb;
    if (Optimizer::cparams.prm_makestubs || inDeduceArgs || (templateNestingCount && ignoreErrtemplateNestingCount(err)))
        return false;
    if (!file)
    {
        if (context)
        {
            LEXEME* lex = context->cur ? context->cur->prev : context->last;
            line = lex->line;
            file = lex->file;
        }
        else
        {
            file = "unknown";
        }
    }
    strcpy(nameb, file);
    if (strchr(infile, '\\') != 0 || strchr(infile, ':') != 0)
    {
        name = Utils::FullQualify(nameb);
    }
    if (TotalErrors() > Optimizer::cparams.prm_maxerr)
        return false;
    if (!alwaysErr(err) && currentErrorFile && !strcmp(currentErrorFile, preProcessor->GetRealFile().c_str()) &&
        preProcessor->GetRealLineNo() == currentErrorLine)
        return false;
    if (err >= sizeof(errors) / sizeof(errors[0]))
    {
        Optimizer::my_sprintf(buf, "Error %d", err);
    }
    else
    {
        vsprintf(buf, errors[err].name, args);
    }
    if (IsReturnErr(err) || (errors[err].level & ERROR) || (Optimizer::cparams.prm_ansi && (errors[err].level & ANSIERROR)) ||
        (Optimizer::cparams.prm_cplusplus && (errors[err].level & CPLUSPLUSERROR)))
    {
        if (!Optimizer::cparams.prm_quiet)
            printf("Error(%3d)   ", err);
        if (Optimizer::cparams.prm_errfile)
            fprintf(errFile, "Error   ");
        listerr = "ERROR";
        total_errors++;
        currentErrorFile = preProcessor->GetRealFile().c_str();
        currentErrorLine = preProcessor->GetRealLineNo();
    }
    else
    {
        if (Warning::Instance()->IsSet(err, Warning::Disable))
            return false;
        if (Warning::Instance()->IsSet(err, Warning::OnlyOnce))
            if (Warning::Instance()->IsSet(err, Warning::Emitted))
                return false;
        Warning::Instance()->SetFlag(err, Warning::Emitted);
        if (Warning::Instance()->IsSet(err, Warning::AsError))
        {
            if (!Optimizer::cparams.prm_quiet)
                printf("Error(%3d)   ", err);
            if (Optimizer::cparams.prm_errfile)
                fprintf(errFile, "Error   ");
            listerr = "ERROR";
            total_errors++;
            currentErrorFile = preProcessor->GetRealFile().c_str();
            currentErrorLine = preProcessor->GetRealLineNo();
        }

        else
        {
            if (!Optimizer::cparams.prm_quiet)
                printf("Warning(%3d) ", err);
            if (Optimizer::cparams.prm_errfile)
                fprintf(errFile, "Warning ");
            listerr = "WARNING";
        }
    }
    if (theCurrentFunc && err != ERR_TOO_MANY_ERRORS && err != ERR_PREVIOUS && err != ERR_TEMPLATE_INSTANTIATION_STARTED_IN)
    {
        strcpy(infunc, " in function ");
        unmangle(infunc + strlen(infunc), theCurrentFunc->sb->decoratedName);
    }
    else
        infunc[0] = 0;
    if (!Optimizer::cparams.prm_quiet)
        printf(" %s(%d):  %s%s\n", name, line, buf, infunc);
    if (Optimizer::cparams.prm_errfile)
        fprintf(errFile, " %s(%d):  %s%s\n", name, line, buf, infunc);
    if (TotalErrors() >= Optimizer::cparams.prm_maxerr)
    {
        error(ERR_TOO_MANY_ERRORS);
#ifdef PARSER_ONLY
        exit(0);
#else
        exit(1);
#endif
    }

    return true;
}
int printerr(int err, const char* file, int line, ...)
{
    bool canprint = false;
    va_list arg;
    va_start(arg, line);
    canprint = printerrinternal(err, file, line, arg);
    va_end(arg);
    if (instantiatingTemplate && canprint)
    {
        printerrinternal(ERR_TEMPLATE_INSTANTIATION_STARTED_IN, preProcessor->GetErrFile().c_str(), preProcessor->GetErrLineNo(),
                         nullptr);
    }
    return canprint;
}
void pperror(int err, int data) { printerr(err, preProcessor->GetErrFile().c_str(), preProcessor->GetErrLineNo(), data); }
void pperrorstr(int err, const char* str) { printerr(err, preProcessor->GetErrFile().c_str(), preProcessor->GetErrLineNo(), str); }
void preverror(int err, const char* name, const char* origFile, int origLine)
{
    if (printerr(err, preProcessor->GetErrFile().c_str(), preProcessor->GetErrLineNo(), name))
        if (origFile && origLine)
            printerr(ERR_PREVIOUS, origFile, origLine, name);
}
void preverrorsym(int err, SYMBOL* sp, const char* origFile, int origLine)
{
    char buf[2048];
    unmangle(buf, sp->sb->decoratedName);
    if (origFile && origLine)
        preverror(err, buf, origFile, origLine);
}
void errorat(int err, const char* name, const char* file, int line) { printerr(err, file, line, name); }
void errorcurrent(int err) { printerr(err, preProcessor->GetErrFile().c_str(), preProcessor->GetErrLineNo()); }
void getns(char* buf, SYMBOL* nssym)
{
    if (nssym->sb->parentNameSpace)
    {
        getns(buf, nssym->sb->parentNameSpace);
        strcat(buf, "::");
    }
    strcat(buf, nssym->name);
}
void getcls(char* buf, SYMBOL* clssym)
{
    if (clssym->sb->parentClass)
    {
        getcls(buf, clssym->sb->parentClass);
        strcat(buf, "::");
    }
    else if (clssym->sb->parentNameSpace)
    {
        getns(buf, clssym->sb->parentNameSpace);
        strcat(buf, "::");
    }
    strcat(buf, clssym->name);
}
void errorqualified(int err, SYMBOL* strSym, NAMESPACEVALUELIST* nsv, const char* name)
{
    char buf[4096];
    char unopped[2048];
    const char* last = "typename";
    char lastb[2048];
    memset(buf, 0, sizeof(buf));
    if (strSym)
    {
        unmangle(lastb, strSym->sb->decoratedName);
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
            unmang1(unopped, name + 1, last, false);
        }
    }
    else
    {
        strcpy(unopped, name);
    }
    Optimizer::my_sprintf(buf, "'%s' is not a member of '", unopped);
    if (strSym)
    {
        typeToString(buf + strlen(buf), strSym->tp);
        //        getcls(buf, strSym);
    }
    else if (nsv)
    {
        getns(buf, nsv->valueData->name);
    }
    strcat(buf, "'");
    if (strSym && !strSym->tp->syms)
        strcat(buf, " because the type is not defined");
    printerr(err, preProcessor->GetErrFile().c_str(), preProcessor->GetErrLineNo(), buf);
}
void errorNotMember(SYMBOL* strSym, NAMESPACEVALUELIST* nsv, const char* name)
{
    errorqualified(ERR_NAME_IS_NOT_A_MEMBER_OF_NAME, strSym, nsv, name);
}
void error(int err) { printerr(err, preProcessor->GetErrFile().c_str(), preProcessor->GetErrLineNo()); }
void errorint(int err, int val) { printerr(err, preProcessor->GetErrFile().c_str(), preProcessor->GetErrLineNo(), val); }
void errorstr(int err, const char* val)
{
    printerr(err, preProcessor->GetErrFile().c_str(), preProcessor->GetErrLineNo(), (char*)val);
}
void errorstr2(int err, const char* val, const char* two)
{
    printerr(err, preProcessor->GetErrFile().c_str(), preProcessor->GetErrLineNo(), (char*)val, (char*)two);
}
void errorsym(int err, SYMBOL* sym)
{
    char buf[2048];
    if (sym->sb)
    {
        if (!sym->sb->decoratedName)
        {
            SetLinkerNames(sym, lk_cdecl);
        }
        unmangle(buf, sym->sb->decoratedName);
    }
    else
    {
        strcpy(buf, sym->name);
    }
    printerr(err, preProcessor->GetErrFile().c_str(), preProcessor->GetErrLineNo(), buf);
}
void errorsym(int err, SYMBOL* sym, int line, const char* file)
{
    char buf[2048];
    if (!sym->sb->decoratedName)
    {
        SetLinkerNames(sym, lk_cdecl);
    }
    unmangle(buf, sym->sb->decoratedName);
    printerr(err, file, line, buf);
}
void errorsym2(int err, SYMBOL* sym1, SYMBOL* sym2)
{
    char one[2048], two[2048];
    unmangle(one, sym1->sb->decoratedName);
    unmangle(two, sym2->sb->decoratedName);
    printerr(err, preProcessor->GetErrFile().c_str(), preProcessor->GetErrLineNo(), one, two);
}
void errorstrsym(int err, const char* name, SYMBOL* sym2)
{
    char two[2048];
    unmangle(two, sym2->sb->decoratedName);
    printerr(err, preProcessor->GetErrFile().c_str(), preProcessor->GetErrLineNo(), name, two);
}
void errorstringtype(int err, char* str, TYPE* tp1)
{
    char tpb1[4096];
    memset(tpb1, 0, sizeof(tpb1));
    typeToString(tpb1, tp1);
    printerr(err, preProcessor->GetErrFile().c_str(), preProcessor->GetErrLineNo(), str, tpb1);
}

void errortype(int err, TYPE* tp1, TYPE* tp2)
{
    char tpb1[4096], tpb2[4096];
    memset(tpb1, 0, sizeof(tpb1));
    memset(tpb2, 0, sizeof(tpb2));
    typeToString(tpb1, tp1);
    if (tp2)
        typeToString(tpb2, tp2);
    printerr(err, preProcessor->GetErrFile().c_str(), preProcessor->GetErrLineNo(), tpb1, tpb2);
}
void errorabstract(int error, SYMBOL* sp)
{
    SYMBOL* sp1;
    errorsym(error, sp);
    sp1 = calculateStructAbstractness(sp, sp);
    if (sp)
    {
        errorsym2(ERR_ABSTRACT_BECAUSE, sp, sp1);
    }
}
void errorarg(int err, int argnum, SYMBOL* declsp, SYMBOL* funcsp)
{
    char argbuf[2048];
    char buf[2048];
    if (declsp->sb->anonymous)
        Optimizer::my_sprintf(argbuf, "%d", argnum);
    else
    {
        unmangle(argbuf, declsp->sb->decoratedName);
    }
    unmangle(buf, funcsp->sb->decoratedName);
    currentErrorLine = 0;
    printerr(err, preProcessor->GetErrFile().c_str(), preProcessor->GetErrLineNo(), argbuf, buf);
}
static BALANCE* newbalance(LEXEME* lex, BALANCE* bal)
{
    BALANCE* rv = (BALANCE*)Alloc(sizeof(BALANCE));
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
static void setbalance(LEXEME* lex, BALANCE** bal)
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
            if (!*bal || (*bal)->type != BAL_BEGIN)
                *bal = newbalance(lex, *bal);
            (*bal)->count++;
            break;
        case openpa:
            if (!*bal || (*bal)->type != BAL_PAREN)
                *bal = newbalance(lex, *bal);
            (*bal)->count++;
            break;

        case openbr:
            if (!*bal || (*bal)->type != BAL_BRACKET)
                *bal = newbalance(lex, *bal);
            (*bal)->count++;
            break;
        default:
            break;
    }
}

/*-------------------------------------------------------------------------*/

void errskim(LEXEME** lex, enum e_kw* skimlist)
{
    BALANCE* bal = 0;
    while (true)
    {
        if (!*lex)
            break;
        if (!bal)
        {
            int i;
            enum e_kw kw = KW(*lex);
            for (i = 0; skimlist[i] != kw_none; i++)
                if (kw == skimlist[i])
                    return;
        }
        setbalance(*lex, &bal);
        *lex = getsym();
    }
}
void skip(LEXEME** lex, enum e_kw kw)
{
    if (MATCHKW(*lex, kw))
        *lex = getsym();
}
bool needkw(LEXEME** lex, enum e_kw kw)
{
    if (lex && MATCHKW(*lex, kw))
    {
        *lex = getsym();
        return true;
    }
    else
    {
        errorint(ERR_NEEDY, kwtosym(kw));
        return false;
    }
}
void specerror(int err, const char* name, const char* file, int line) { printerr(err, file, line, name); }
}  // namespace Parser
void diag(const char* fmt, ...)
{
    using namespace Parser;
    if (!templateNestingCount)
    {
        if (Optimizer::cparams.prm_diag)
        {
            va_list argptr;

            va_start(argptr, fmt);
            printf("Diagnostic: ");
            vprintf(fmt, argptr);
            if (theCurrentFunc)
                printf(":%s", theCurrentFunc->sb->decoratedName);
            printf("\n");
            va_end(argptr);
        }
        diagcount++;
    }
}
namespace Parser
{
static bool hasGoto(STATEMENT* stmt)
{
    while (stmt)
    {
        switch (stmt->type)
        {
            case st_block:
            case st_switch:
            case st_try:
            case st_catch:
            case st___try:
            case st___catch:
            case st___finally:
            case st___fault:
                if (hasGoto(stmt->lower))
                    return true;
                break;
            case st_declare:
            case st_expr:
                break;
            case st_goto:
                return true;
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
    return false;
}
static bool hasDeclarations(STATEMENT* stmt)
{
    while (stmt)
    {
        switch (stmt->type)
        {
            case st_block:
            case st_switch:
            case st_try:
            case st_catch:
            case st___try:
            case st___catch:
            case st___finally:
            case st___fault:
                if (hasDeclarations(stmt->lower))
                    return true;
                break;
            case st_declare:
            case st_expr:
                if (stmt->hasvla || stmt->hasdeclare)
                    return true;
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
                diag("unknown stmt type in hasDeclarations");
                break;
        }
        stmt = stmt->next;
    }
    return false;
}
static void labelIndexes(STATEMENT* stmt, int* min, int* max)
{
    while (stmt)
    {
        switch (stmt->type)
        {
            case st_block:
            case st_switch:
            case st_try:
            case st_catch:
            case st___try:
            case st___catch:
            case st___finally:
            case st___fault:
                labelIndexes(stmt->lower, min, max);
                break;
            case st_declare:
            case st_expr:
                break;
            case st_goto:
            case st_select:
            case st_notselect:
            case st_label:
                if (stmt->label < *min)
                    *min = stmt->label;
                if (stmt->label > *max)
                    *max = stmt->label;
                break;
            case st_return:
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
                diag("unknown stmt type in hasDeclarations");
                break;
        }
        stmt = stmt->next;
    }
}
static VLASHIM* mkshim(_vlaTypes type, int level, int label, STATEMENT* stmt, VLASHIM* last, VLASHIM* parent, int blocknum,
                       int blockindex)
{
    VLASHIM* rv = (VLASHIM*)Alloc(sizeof(VLASHIM));
    if (last && last->type != v_return && last->type != v_goto)
    {
        rv->backs = (Optimizer::LIST*)(Optimizer::LIST*)Alloc(sizeof(Optimizer::LIST));
        rv->backs->data = last;
    }
    rv->type = type;
    rv->level = level;
    rv->label = label;
    rv->line = stmt->line;
    rv->file = stmt->file;
    rv->stmt = stmt;
    rv->blocknum = blocknum;
    rv->blockindex = blockindex;
    rv->parent = parent;
    return rv;
}
/* thisll be sluggish if there are lots of gotos & labels... */
static VLASHIM* getVLAList(STATEMENT* stmt, VLASHIM* last, VLASHIM* parent, VLASHIM** labels, int minLabel, int* blocknum,
                           int level, bool* branched)
{
    int curBlockNum = (*blocknum)++;
    int curBlockIndex = 0;
    VLASHIM *rv = nullptr, **cur = &rv, *nextParent = nullptr;
    while (stmt)
    {
        switch (stmt->type)
        {
            case st_switch:
            {
                bool first = true;
                CASEDATA* cases = stmt->cases;
                while (cases)
                {
                    *cur = mkshim(v_branch, level, cases->label, stmt, last, parent, curBlockNum, curBlockIndex++);
                    last = *cur;
                    if (first)
                        nextParent = last;
                    first = false;
                    cur = &(*cur)->next;
                    cases = cases->next;
                }
            }
                // fallthrough
            case st_block:
            case st_try:
            case st_catch:
            case st___try:
            case st___catch:
            case st___finally:
            case st___fault:
                if (stmt->lower && stmt->lower->type == st_goto)
                {
                    // unwrap the goto for purposes of these diagnostics
                    *cur = mkshim(v_goto, level, stmt->lower->label, stmt->lower, last, parent, curBlockNum, curBlockIndex++);
                    last = *cur;
                    last->checkme = stmt->lower->explicitGoto;
                    cur = &(*cur)->next;
                    *branched = true;
                }
                else
                {
                    *cur = mkshim(v_blockstart, level, stmt->label, stmt, last, parent, curBlockNum, curBlockIndex++);
                    last = *cur;
                    cur = &(*cur)->next;
                    if (!nextParent)
                        nextParent = last;
                    last->lower = getVLAList(stmt->lower, last, nextParent, labels, minLabel, blocknum, level + 1, branched);
                    if (stmt->blockTail)
                    {
                        *cur = mkshim(v_blockend, level, stmt->label, stmt, last, parent, curBlockNum, curBlockIndex++);
                        last = *cur;
                        cur = &(*cur)->next;
                    }
                }
                break;
            case st_declare:
            case st_expr:
                if (*branched)
                {
                    if (stmt->hasvla)
                    {
                        *cur = mkshim(v_vla, level, stmt->label, stmt, last, parent, curBlockNum, curBlockIndex++);
                        last = *cur;
                        cur = &(*cur)->next;
                    }
                    else if (stmt->hasdeclare)
                    {
                        *cur = mkshim(v_declare, level, stmt->label, stmt, last, parent, curBlockNum, curBlockIndex++);
                        last = *cur;
                        cur = &(*cur)->next;
                    }
                    nextParent = last;
                }
                break;
            case st_return:
                *branched = true;
                *cur = mkshim(v_return, level, stmt->label, stmt, last, parent, curBlockNum, curBlockIndex++);
                last = *cur;
                cur = &(*cur)->next;
                break;
            case st_select:
            case st_notselect:
                *branched = true;
                *cur = mkshim(v_branch, level, stmt->label, stmt, last, parent, curBlockNum, curBlockIndex++);
                last = *cur;
                cur = &(*cur)->next;
                nextParent = last;
                break;
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
                *branched = true;
                *cur = mkshim(v_goto, level, stmt->label, stmt, last, parent, curBlockNum, curBlockIndex++);
                last = *cur;
                last->checkme = stmt->explicitGoto;
                cur = &(*cur)->next;
                break;
            case st_label:
                *cur = mkshim(v_label, level, stmt->label, stmt, last, parent, curBlockNum, curBlockIndex++);
                last = *cur;
                cur = &(*cur)->next;
                labels[stmt->label - minLabel] = last;
                break;
            default:
                diag("unknown stmt type in checkvla");
                break;
        }
        stmt = stmt->next;
    }
    return rv;
}
static void fillPrevious(VLASHIM* shim, VLASHIM** labels, int minLabel)
{
    while (shim)
    {
        VLASHIM* selected;
        Optimizer::LIST* prev;
        switch (shim->type)
        {
            case v_blockstart:
                fillPrevious(shim->lower, labels, minLabel);
                break;
            case v_declare:
            case v_vla:
            case v_return:
            case v_label:
            case v_blockend:
                break;
            case v_goto:
            case v_branch:
                selected = labels[shim->label - minLabel];
                if (selected)
                {
                    prev = (Optimizer::LIST*)(Optimizer::LIST*)Alloc(sizeof(Optimizer::LIST));
                    prev->data = shim;
                    prev->next = selected->backs;
                    selected->backs = prev;
                    shim->fwd = selected;
                }
                break;
            default:
                diag("unknown shim type in fillPrevious");
                break;
        }
        shim = shim->next;
    }
}
static void vlaError(VLASHIM* gotoShim, VLASHIM* errShim)
{
    char buf[256];
    Optimizer::my_sprintf(buf, "%d", gotoShim->line);
    currentErrorLine = 0;
    specerror(ERR_GOTO_BYPASSES_VLA_INITIALIZATION, buf, errShim->file, errShim->line);
}
static void declError(VLASHIM* gotoShim, VLASHIM* errShim)
{
    char buf[256];
    Optimizer::my_sprintf(buf, "%d", gotoShim->line);
    currentErrorLine = 0;
    specerror(ERR_GOTO_BYPASSES_INITIALIZATION, buf, errShim->file, errShim->line);
}
static bool scanGoto(VLASHIM* shim, VLASHIM* gotoshim, VLASHIM* matchshim, int* currentLevel)
{
    if (shim == matchshim || shim->level < matchshim->level)
        return true;
    if (shim && !shim->mark && shim != gotoshim)
    {
        shim->mark = true;
        if (shim->level <= *currentLevel)
        {
            *currentLevel = shim->level;
            if (shim->type == v_declare)
                declError(gotoshim, shim);
            if (shim->type == v_vla)
                vlaError(gotoshim, shim);
        }
        Optimizer::LIST* lst = shim->backs;
        while (lst)
        {
            if (lst->data == matchshim)
                return true;
            lst = lst->next;
        }
        lst = shim->backs;
        while (lst)
        {
            VLASHIM* s = (VLASHIM*)lst->data;
            if (!s->checkme && scanGoto(s, gotoshim, matchshim, currentLevel))
                return true;
            lst = lst->next;
        }
    }
    return false;
}
void unmarkGotos(VLASHIM* shim)
{
    while (shim)
    {
        shim->mark = false;
        if (shim->type == v_blockstart)
            unmarkGotos(shim->lower);
        shim = shim->next;
    }
}
static void validateGotos(VLASHIM* shim, VLASHIM* root)
{
    while (shim)
    {
        VLASHIM* selected;
        Optimizer::LIST* prev;
        switch (shim->type)
        {
            case v_blockstart:
                validateGotos(shim->lower, root);
                break;
            case v_declare:
            case v_vla:
            case v_return:
            case v_label:
            case v_blockend:
            case v_branch:
                break;
            case v_goto:
                if (shim->checkme && shim->fwd)
                {
                    VLASHIM* sgoto = shim;
                    VLASHIM* fwd = sgoto->fwd;
                    while (sgoto->level > fwd->level)
                        sgoto = sgoto->parent;
                    while (sgoto->level < fwd->level)
                        fwd = fwd->parent;
                    while (sgoto->blocknum != fwd->blocknum)
                    {
                        int n = sgoto->level, m = fwd->level;
                        if (n >= m)
                            sgoto = sgoto->parent;
                        if (n <= m)
                            fwd = fwd->parent;
                    }
                    unmarkGotos(root);
                    int level = shim->fwd->level;
                    if (sgoto->blockindex > fwd->blockindex)
                    {
                        // goto is after label
                        // drill up in the label block until we hit the label
                        scanGoto(shim->fwd, shim, fwd, &level);
                    }
                    else
                    {
                        // goto is before label
                        // drill up in the label block until we hit the goto
                        scanGoto(shim->fwd, shim, sgoto, &level);
                    }
                }
                break;
            default:
                diag("unknown shim type in validateGotos");
                break;
        }
        shim = shim->next;
    }
}
void checkGotoPastVLA(STATEMENT* stmt, bool first)
{
    if (hasGoto(stmt) && hasDeclarations(stmt))
    {
        int min = INT_MAX, max = INT_MIN;
        labelIndexes(stmt, &min, &max);
        if (min > max)
            return;
        VLASHIM** labels = (VLASHIM**)Alloc((max + 1 - min) * sizeof(VLASHIM*));

        int blockNum = 0;
        bool branched = false;
        VLASHIM* list = getVLAList(stmt, nullptr, nullptr, labels, min, &blockNum, 0, &branched);
        fillPrevious(list, labels, min);
        validateGotos(list, list);
    }
}
void checkUnlabeledReferences(BLOCKDATA* block)
{
    int i;
    for (i = 0; i < labelSyms->size; i++)
    {
        SYMLIST* hr = labelSyms->table[i];
        while (hr)
        {
            SYMBOL* sp = hr->p;
            if (sp->sb->storage_class == sc_ulabel)
            {
                STATEMENT* st;
                specerror(ERR_UNDEFINED_LABEL, sp->name, sp->sb->declfile, sp->sb->declline);
                sp->sb->storage_class = sc_label;
                st = stmtNode(nullptr, block, st_label);
                st->label = sp->sb->offset;
            }
            hr = hr->next;
        }
    }
}
void checkUnused(HASHTABLE* syms)
{
    int i;
    for (i = 0; i < syms->size; i++)
    {
        SYMLIST* hr = syms->table[i];
        while (hr)
        {
            SYMBOL* sp = hr->p;
            if (sp->sb->storage_class == sc_overloads)
                sp = (SYMBOL*)sp->tp->syms->table[0]->p;
            if (!sp->sb->attribs.inheritable.used && !sp->sb->anonymous)
            {
                if (sp->sb->assigned || sp->sb->altered)
                {
                    if (sp->sb->storage_class == sc_auto || sp->sb->storage_class == sc_register ||
                        sp->sb->storage_class == sc_parameter)
                        errorsym(ERR_SYM_ASSIGNED_VALUE_NEVER_USED, sp);
                }
                else
                {
                    if (sp->sb->storage_class == sc_parameter)
                        errorsym(ERR_UNUSED_PARAMETER, sp);
                    else
                        errorsym(ERR_UNUSED_VARIABLE, sp);
                }
            }
            hr = hr->next;
        }
    }
}
void findUnusedStatics(NAMESPACEVALUELIST* nameSpace)
{
    int i;
    for (i = 0; i < nameSpace->valueData->syms->size; i++)
    {
        SYMLIST* hr = nameSpace->valueData->syms->table[i];
        while (hr)
        {
            SYMBOL* sp = hr->p;
            if (sp)
            {
                if (sp->sb->storage_class == sc_namespace)
                {
                    findUnusedStatics(sp->sb->nameSpaceValues);
                }
                else
                {
                    if (sp->sb->storage_class == sc_overloads)
                    {
                        SYMLIST* hr1 = sp->tp->syms->table[0];
                        while (hr1)
                        {
                            SYMBOL* sp1 = (SYMBOL*)hr1->p;
                            if (sp1->sb->isInline && !sp1->sb->inlineFunc.stmt && !sp1->sb->templateLevel)
                            {
                                errorsym(ERR_UNDEFINED_IDENTIFIER, sp1);
                            }
                            else if (sp1->sb->storage_class == sc_static && !sp1->sb->inlineFunc.stmt &&
                                     !(sp1->sb->templateLevel || sp1->sb->instantiated))
                            {
                                if (sp1->sb->attribs.inheritable.used)
                                    errorsym(ERR_UNDEFINED_STATIC_FUNCTION, sp1, eofLine, eofFile);
                                else
                                    errorsym(ERR_STATIC_FUNCTION_USED_BUT_NOT_DEFINED, sp1, eofLine, eofFile);
                            }
                            hr1 = hr1->next;
                        }
                    }
                    else
                    {
                        currentErrorLine = 0;
                        if (sp->sb->storage_class == sc_static && !sp->sb->attribs.inheritable.used)
                            errorsym(ERR_UNUSED_STATIC, sp);
                        currentErrorLine = 0;
                        if (sp->sb->storage_class == sc_global || sp->sb->storage_class == sc_static ||
                            sp->sb->storage_class == sc_localstatic)
                            /* void will be caught earlier */
                            if (!isfunction(sp->tp) && !isarray(sp->tp) && sp->tp->size == 0 && !isvoid(sp->tp) &&
                                sp->tp->type != bt_any && !sp->sb->templateLevel)
                                errorsym(ERR_UNSIZED, sp);
                    }
                }
            }
            hr = hr->next;
        }
    }
}
static void usageErrorCheck(SYMBOL* sp)
{
    if ((sp->sb->storage_class == sc_auto || sp->sb->storage_class == sc_register || sp->sb->storage_class == sc_localstatic) &&
        !sp->sb->assigned && !sp->sb->attribs.inheritable.used && !sp->sb->altered)
    {
        if (!structLevel || !sp->sb->deferredCompile)
            errorsym(ERR_USED_WITHOUT_ASSIGNMENT, sp);
    }
    sp->sb->attribs.inheritable.used = true;
}
static SYMBOL* getAssignSP(EXPRESSION* exp)
{
    SYMBOL* sp;
    switch (exp->type)
    {
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
            return nullptr;
    }
}
static void assignmentAssign(EXPRESSION* left, bool assign)
{
    while (castvalue(left))
    {
        left = left->left;
    }
    if (lvalue(left))
    {
        SYMBOL* sp;
        sp = getAssignSP(left->left);
        if (sp)
        {
            if (sp->sb->storage_class == sc_auto || sp->sb->storage_class == sc_register || sp->sb->storage_class == sc_parameter)
            {
                if (assign)
                    sp->sb->assigned = true;
                sp->sb->altered = true;
                //				sp->sb->attribs.inheritable.used = false;
            }
        }
    }
}
void assignmentUsages(EXPRESSION* node, bool first)
{
    FUNCTIONCALL* fp;
    if (node == 0)
        return;
    switch (node->type)
    {
        case en_auto:
            if (node->v.sp->sb)
                node->v.sp->sb->attribs.inheritable.used = true;
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
                assignmentUsages(node->left, false);
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
            assignmentUsages(node->left, false);
            break;
        case en_assign:
        case en__initblk:
        case en__cpblk:
            assignmentUsages(node->right, false);
            assignmentUsages(node->left, true);
            assignmentAssign(node->left, true);
            break;
        case en_autoinc:
        case en_autodec:
            assignmentUsages(node->left, false);
            assignmentAssign(node->left, true);
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
            assignmentUsages(node->left, false);
            assignmentUsages(node->right, false);
            break;
        case en_mp_as_bool:
        case en_blockclear:
        case en_argnopush:
        case en_not_lvalue:
        case en_thisref:
        case en_lvalue:
        case en_funcret:
            assignmentUsages(node->left, false);
            break;
        case en_atomic:
            assignmentUsages(node->v.ad->flg, false);
            assignmentUsages(node->v.ad->memoryOrder1, false);
            assignmentUsages(node->v.ad->memoryOrder2, false);
            assignmentUsages(node->v.ad->address, false);
            assignmentUsages(node->v.ad->third, false);
            break;
        case en_func:
            fp = node->v.func;
            {
                INITLIST* args = fp->arguments;
                while (args)
                {
                    assignmentUsages(args->exp, false);
                    args = args->next;
                }
                if (Optimizer::cparams.prm_cplusplus && fp->thisptr && !fp->fcall)
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
        case en__initobj:
        case en__sizeof:
            break;
        default:
            diag("assignmentUsages");
            break;
    }
}
static int checkDefaultExpression(EXPRESSION* node)
{
    FUNCTIONCALL* fp;
    bool rv = false;
    if (node == 0)
        return 0;
    switch (node->type)
    {
        case en_auto:
            if (!node->v.sp->sb->anonymous)
                rv = true;
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
                INITLIST* args = fp->arguments;
                while (args)
                {
                    rv |= checkDefaultExpression(args->exp);
                    args = args->next;
                }
            }
            if (fp->sp->sb->parentClass && fp->sp->sb->parentClass->sb->islambda)
                rv |= 2;
            break;
        case en_stmt:
        case en_templateparam:
        case en_templateselector:
        case en__initobj:
        case en__sizeof:
            break;
        default:
            diag("rv |= checkDefaultExpression");
            break;
    }
    return rv;
}
void checkDefaultArguments(SYMBOL* spi)
{
    INITIALIZER* p = spi->sb->init;
    int r = 0;
    while (p)
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
}  // namespace Parser