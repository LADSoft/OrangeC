#ifdef __BUILDING_LSCRTL_DLL

#include <functional>

// the compiler needs to be worked on to fix this problem, it is too big of a project right now though...
std::bad_function_call::bad_function_call() { }
std::bad_function_call::bad_function_call(const std::bad_function_call&) { }
std::bad_function_call::~bad_function_call() { }
#endif
