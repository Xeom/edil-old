#if !defined(BUFFER_LOG_H)
# define BUFFER_LOG_H
# include "head.h"

# include "buffer/point.h"
# include <stdio.h>

int buffer_log_initsys(void);

FILE *buffer_log_point_stream(point *p);

#endif /* BUFFER_LOG_H */
