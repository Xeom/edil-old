#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "err.h"
#include "line.h"
#include "wincont.h"

#include "ui.h"

wintree *ui_rootwin;

volatile char ui_more_left_char;
volatile char ui_more_right_char;
volatile char ui_window_vertical_char;
volatile char ui_window_horizontal_char;
volatile char ui_window_corner_char;

const int ui_key_resize = KEY_RESIZE;

face *ui_window_border_face;
face *ui_window_border_selected_face;

hook *ui_on_resize;

static int ui_display_line_part(const char *text, const char *end, size_t maxposx, size_t posy, size_t *posx);
static int ui_display_wintree_border(wintree *tree);
static int ui_display_wintree_line(wintree *tree, lineno ln);
static int ui_display_wintree(wintree *tree);

static void ui_set_defaults(void);
static void ui_highlight_selected(void);

int ui_initsys(void)
{
    initscr();
    start_color();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    ui_set_defaults();

    ui_on_resize = hook_init(2);

    return 0;
}

int ui_killsys(void)
{
    endwin();

    hook_free(ui_on_resize);

    return 0;
}

static void ui_set_defaults(void)
{
    ui_more_left_char         = '\253';
    ui_more_right_char        = '\273';
    ui_window_vertical_char   = '|';
    ui_window_horizontal_char = '-';
    ui_window_corner_char     = '\'';

    ui_window_border_selected_face = face_init(COLOR_BLACK, COLOR_WHITE);
    ui_window_border_face          = face_init(COLOR_RED, COLOR_BLACK);
    ui_window_border_face->bright  = 1;
}

static int ui_display_wintree_line(wintree *tree, lineno ln)
{
    wincont *cont;
    line    *l;
    size_t   posx, posy, textstart, textend, maxposx;
    const char *text;
    vec     *faces;

    posx    = wintree_get_posx(tree);
    posy    = wintree_get_posy(tree) + ln;
    cont    = wintree_get_content(tree);

    maxposx = posx + wintree_get_sizex(tree) - 2;

    l       = wincont_get_line(cont, ln);

    if (l == NULL)
        return 0;

    text  = wincont_get_line_text_const(cont, l);
    faces = line_get_faces(l);
    text  = line_get_text(l);

    textend   = 0;
    textstart = 0;

    vec_foreach(faces, face *, f,
                textstart = textend;
                textend   = f->start;

                if (ui_display_line_part(text + textstart,
                                         text + textend,
                                         maxposx, posy, &posx) == 0)
                    return 0;

                textstart = textend;
                textend   = f->end;
                attron(face_get_attr(f));

                if (ui_display_line_part(text + textstart,
                                         text + textend,
                                         maxposx, posy, &posx) == 0)
                {
                    attroff(face_get_attr(f));
                    return 0;
                }
                attroff(face_get_attr(f));
        )

    textstart = textend;

    if (ui_display_line_part(text + textstart, (const char *)UINTPTR_MAX, maxposx, posy, &posx) == 1)
        while (posx++ < maxposx)
            mvaddch(posy, posx, ' ');

    return 0;
}

static int ui_display_line_part(const char *text, const char *end, size_t maxposx, size_t posy, size_t *posx)
{
    while (*posx <= maxposx)
    {
        if (text == '\0' || text >= end)
            return 1;

        mvaddch(posy, *posx, *(text++));
        ++(*posx);
    }

    return 0;
}

static int ui_display_wintree_border(wintree *tree)
{
    size_t posx, posy, lastposx, lastposy;

    posx = wintree_get_posx(tree);
    posy = wintree_get_posy(tree);

    if (wintree_get_sizey(tree) == 0 || wintree_get_sizex(tree) == 0)
        return -1;

    lastposy = posy + wintree_get_sizey(tree) - 1;
    lastposx = posx + wintree_get_sizex(tree) - 1;

    while (posx < lastposx)
    {
        mvaddch(lastposy, posx, ui_window_horizontal_char);
        ++posx;
    }

    while (posy < lastposy)
    {
        mvaddch(posy, lastposx, ui_window_vertical_char);
        ++posy;
    }

    mvaddch(lastposy, lastposx, ui_window_corner_char);

    return 0;
}

static int ui_display_wintree(wintree *tree)
{
    size_t ln;

    ui_display_wintree_border(tree);

/*
    ln = wintree_get_sizey(tree) - 1;
    while (ln--)
        ui_display_wintree_line(tree, ln);
*/
    return 0;
}

static void ui_highlight_selected(void)
{
    size_t   posx, posy, sizex, sizey;
    wintree *selected;

    selected = wintree_get_selected();

    if (selected == NULL)
        return;

    posx  = wintree_get_posx(selected);
    posy  = wintree_get_posy(selected);
    sizex = wintree_get_sizex(selected);
    sizey = wintree_get_sizey(selected);

    face_display_at(ui_window_border_selected_face, posx, posy + sizey - 1, sizex, 1);
    face_display_at(ui_window_border_selected_face, posx + sizex - 1, posy, 1, sizey);
}

int ui_display_wintrees(void)
{
    int      borderattr;
    wintree *curr;

    borderattr = face_get_attr(ui_window_border_face);
    curr       = wintree_iter_start();

    attron(borderattr);

    while (curr)
    {
        ui_display_wintree(curr);
        curr = wintree_iter_next(curr);
    }

    attroff(borderattr);

    ui_highlight_selected();

    return 0;
}

int ui_resize(void)
{
    size_t termy, termx;

    getmaxyx(stdscr, termy, termx);

    hook_call(ui_on_resize, &termy, &termx);

    wintree_set_root_size(termx, termy);

    ui_display_wintrees();

    return 0;
}
