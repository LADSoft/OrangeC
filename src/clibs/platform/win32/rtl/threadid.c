#include <windows.h>
int _RTL_FUNC _threadid()
{
  return GetCurrentThreadId();
}
int _RTL_FUNC __threadid()
{
  return GetCurrentThreadId();
}
