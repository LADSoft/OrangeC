#include <stdatomic.h>
/*
void __atomic_flag_wait__( volatile atomic_flag* __a__ )
{ while ( atomic_flag_test_and_set( __a__ ) ); }

void __atomic_flag_wait_explicit__( volatile atomic_flag* __a__,
                                    memory_order __x__ )
{ while ( atomic_flag_test_and_set_explicit( __a__, __x__ ) ); }

*/
#define LOGSIZE 4

static atomic_flag volatile __atomic_flag_anon_table__[ 1 << LOGSIZE ] =
{
    ATOMIC_FLAG_INIT, ATOMIC_FLAG_INIT, ATOMIC_FLAG_INIT, ATOMIC_FLAG_INIT,
    ATOMIC_FLAG_INIT, ATOMIC_FLAG_INIT, ATOMIC_FLAG_INIT, ATOMIC_FLAG_INIT,
    ATOMIC_FLAG_INIT, ATOMIC_FLAG_INIT, ATOMIC_FLAG_INIT, ATOMIC_FLAG_INIT,
    ATOMIC_FLAG_INIT, ATOMIC_FLAG_INIT, ATOMIC_FLAG_INIT, ATOMIC_FLAG_INIT,
};

volatile atomic_flag* __atomic_flag_for_address__( const volatile void* __z__ )
{
    uintptr_t __u__ = (uintptr_t)__z__;
    __u__ += (__u__ >> 2) + (__u__ << 4);
    __u__ += (__u__ >> 7) + (__u__ << 5);
    __u__ += (__u__ >> 17) + (__u__ << 13);
    if ( sizeof(uintptr_t) > 4 ) __u__ += (__u__ >> 31);
    __u__ &= ~((~(uintptr_t)0) << LOGSIZE);
    return __atomic_flag_anon_table__ + __u__;
}

