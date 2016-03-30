#ifndef HEAD_H
# define HEAD_H

# define REALLY_STRIFY(a) #a
# define STRIFY(a) REALLY_STRIFY()

# define REALLY_CONCAT(a, b) a ## b
# define CONCAT(a, b) REALLY_CONCAT(a, b)

# define MAX(a, b) a > b ? a : b
# define MIN(a, b) a > b ? b : a

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned short ushort;

#endif /* HEAD_H */