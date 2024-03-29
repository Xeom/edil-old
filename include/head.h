#if !defined(HEAD_H)
# define HEAD_H

# define PRAGMAIFY(pragma) _Pragma (#pragma)

# if defined(PRINT_TODO)
#  define TODO(stuff) PRAGMAIFY(message ("TODO: " stuff))
# elif defined(WARN_TODO)
# else
#  define TODO(stuff)
# endif

# define REALLY_STRIFY(a) #a
# define STRIFY(a) REALLY_STRIFY(a)

# define REALLY_CONCAT(a, b) a ## b
# define CONCAT(a, b) REALLY_CONCAT(a, b)

# define MAX(a, b) ((a) > (b) ? (a) : (b))
# define MIN(a, b) ((a) > (b) ? (b) : (a))

# define ADDPTR(ptr, n) (void *)((intptr_t)(ptr) + (ptrdiff_t)(n))
# define SUBPTR(ptr, n) (void *)((intptr_t)(ptr) - (ptrdiff_t)(n))

# if ( defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) ) \
    || defined(__BIG_ENDIAN__) || defined(__BIG_ENDIAN) || defined(_BIG_ENDIAN)
#  define BIG_ENDIAN
# else
#  define LITTLE_ENDIAN
# endif

typedef unsigned int       uint;
typedef unsigned char      uchar;
typedef unsigned short     ushort;
typedef unsigned long      ulong;
typedef unsigned long long ullong;

#endif /* HEAD_H */
