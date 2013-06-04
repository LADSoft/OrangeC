#include <windows.h>
int _RTL_FUNC getpid()
{
  return GetCurrentProcessId();
}
int _RTL_FUNC _getpid()
{
  return GetCurrentProcessId();
}
