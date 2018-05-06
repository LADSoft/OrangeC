#define HAVE_MALLOC_H 0
#if (defined(__INTEL_COMPILER) || defined(__x86_64) || defined(__x86_64__))
#define ARCH_X86_64 1
#else
#define ARCH_X86_64 0
#endif
#define SYS_LINUX 0
#define HAVE_POSIXTHREAD 0
#define HAVE_THREAD 1
#define HAVE_LOG2F 1
#define HAVE_VECTOREXT 0
/*
#if (!defined(__INTEL_COMPILER) && !defined(_MSC_VER))
#define fseek fseeko
#define ftell ftello
#endif
*/
#define HAVE_GPL 1
#define HAVE_INTERLACED 1
#define HAVE_ALTIVEC 0
#define HAVE_ALTIVEC_H 0
#define HAVE_MMX 0
#define HAVE_ARMV6 0
#define HAVE_ARMV6T2 0
#define HAVE_NEON 0
#define HAVE_BEOSTHREAD 0
#define HAVE_WIN32THREAD 1
#define HAVE_VISUALIZE 0
#define HAVE_SWSCALE 0
#define HAVE_LAVF 0
#define HAVE_FFMS 0
#define HAVE_GPAC 0
#define HAVE_GF_MALLOC 0
#define HAVE_AVS 0
#define HAVE_CPU_COUNT 0
#if (defined(__DMC__) || defined(__ORANGEC__) || defined(__INTEL_COMPILER) || defined(_MSC_VER))
#define HAVE_STDINT 1
#endif
#define SYS_WINDOWS 1
#define HAVE_STRING_H 1
