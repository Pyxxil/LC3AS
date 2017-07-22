#ifndef LIB_EXTRA
#define LIB_EXTRA

#ifndef _MSC_VER
#if __has_attribute(always_inline)
#define ALWAYS_INLINE __attribute__((always_inline))
#else
#define ALWAYS_INLINE
#endif
#else
#define ALWAYS_INLINE
#endif

#endif
