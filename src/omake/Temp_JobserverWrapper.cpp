#ifdef _WIN32
#    include "os_specific/windows/Win_Jobserver.cpp"
#elif HAVE_UNISTD_H
#    include "os_specific/Linux/Linux_Jobserver.cpp"
#else
#    error "Platform unknown and (probably) not supported"
#endif