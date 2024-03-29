#if !defined(UI_CORE_H)
# define UI_CORE_H
# include "head.h"

# include "hook.h"

extern hook ui_on_resize_pre;
extern hook ui_on_resize_post;

int ui_initsys(void);

int ui_killsys(void);

int ui_refresh(void);

int ui_draw(void);

int ui_resize(void);

#endif /* UI_CORE_H */
