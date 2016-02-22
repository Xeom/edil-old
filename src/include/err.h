#ifndef ERR_H
#define ERR_H

typedef enum
{
	low,       /* Caused by a user, inconsequential e.g. Trying to delete a char that does not exist */
	medium,    /* Caused by a user, consequential   e.g. Unknown command                             */
	high,      /* Caused by bad high level code     e.g. Invalid parameters to function in config    */
	critical,  /* Should not happen                 e.g. Important object null                       */
	terminal,  /* Application cannot continue       e.g. Out of memory                               */
	errlvl_end
} errlvl;

typedef struct err_s err;

errlvl err_min_quit_lvl;
errlvl err_min_alert_lvl;
errlvl err_min_detail_lvl;

void err_init(void);

void err_new(errlvl level, const char *title, const char *details);

err *err_get(err *e, errlvl level, size_t i);

err *err_pop(err *e);

errlvl err_get_lvl(err *e);

const char *err_get_details(err *e);

const char *err_get_title(err *e);

#endif /* ERR_H */
