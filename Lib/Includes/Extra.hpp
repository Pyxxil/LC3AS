#ifndef LIB_EXTRA
#define LIB_EXTRA

#if __has_attribute(always_inline)
#define ALWAYS_INLINE __attribute__((always_inline))
#else
#define ALWAYS_INLINE
#endif

#endif
