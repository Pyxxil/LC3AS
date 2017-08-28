#ifndef LIB_EXTRA
#define LIB_EXTRA

/*!
 * Technically, none of these are required (and the compiler probably knows
 * better), but it's kind of fun to use.
 */

#if !defined(_MSC_VER)
#if __has_attribute(always_inline)
#define ALWAYS_INLINE __attribute__((always_inline)) inline
#else
#define ALWAYS_INLINE inline
#endif
#else
// MSVC doesn't have __has_attribute, nor __attribute__((always_inline))
#define ALWAYS_INLINE __force_inline
#endif

#endif
