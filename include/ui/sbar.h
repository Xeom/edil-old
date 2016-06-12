#if !defined(UI_SBAR_H)
# define UI_SBAR_H

extern char *ui_sbar_content;

int ui_sbar_initsys(void);

int ui_sbar_killsys(void);

int ui_sbar_draw(void);

int ui_sbar_set(const char *content);

#endif /* UI_SBAR_H */
