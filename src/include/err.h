#ifndef ERR_H
#define ERR_H
#include <stddef.h>

typedef enum
{
	low,       /* Caused by a user, inconsequential e.g. Trying to delete a char that does not exist */
	medium,    /* Caused by a user, consequential   e.g. Unknown command                             */
	high,      /* Caused by bad high level code     e.g. Invalid parameters to function in config    */
	critical,  /* Should not happen                 e.g. Important object null                       */
	terminal,  /* Application cannot continue       e.g. Out of memory                               */
	errlvl_end
} errlvl;

#define ERRLOC Line: __LINE__ File: __FILE__
#define ERR_NEW_CALL(level, title, details) err_new(level, #title, #details)
#define ERR_NEW(level, title, details) ERR_NEW_CALL(level, title, details ERRLOC)

#define CHECK_NULL_PRM(funct, var, rtn)								\
	if (var == NULL) {													\
		ERR_NEW(high,													\
				funct  : NULL argument, Argument name: var);			\
		return rtn;													\
	}

#define CHECK_ALLOC(funct, var, rtn)									\
	if (var == NULL) {													\
		ERR_NEW(terminal,												\
				funct : Out of memory,								\
				Could not allocate memory for var);			\
		return rtn;													\
	}

#define TRACE_NONZRO(funct, call, var, rtn)							\
	if (var) {															\
		ERR_NEW(err_last_lvl,											\
				funct : Call call failed,							\
				call returned nonzero return code);				\
		return rtn;													\
	}

#define TRACE_NONZRO_CALL(funct, call, rtn) TRACE_NONZRO(funct, call, call, rtn)

#define TRACE_NULL(funct, call, var, rtn)								\
	if (var == NULL) {													\
		ERR_NEW(err_last_lvl,											\
				funct : Call call failed,						\
				call returned nonzero return code ERRLOC); 		\
		return rtn;													\
	}

typedef struct err_s err;

errlvl err_min_quit_lvl;
errlvl err_min_alert_lvl;
errlvl err_min_detail_lvl;

errlvl err_last_lvl;

void err_init(void);

void err_new(errlvl level, const char *title, const char *details);

err *err_get(errlvl level, size_t i);

err *err_pop(void);

errlvl err_get_lvl(err *e);

const char *err_get_details(err *e);

const char *err_get_title(err *e);

#endif /* ERR_H */
