#include <curses.h>
#include "err.h"
#include "line.h"
#include "ui.h"
#include "wincont.h"
#include "wintree.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

wintree *ui_rootwin;

volatile char ui_more_left_char;
volatile char ui_more_right_char;
volatile char ui_window_vertical_char;
volatile char ui_window_horizontal_char;
volatile char ui_window_corner_char;

const int ui_key_resize = KEY_RESIZE;

face *ui_window_border_face;
face *ui_window_border_selected_face;

int ui_display_line_part(const char *text, const char *end, size_t maxposx, size_t posy, size_t *posx);
int ui_display_wintree_border(wintree *tree, face *f);
int ui_display_wintree_line(wintree *tree, lineno ln);
int ui_display_wintree(wintree *tree, face *borderface);

void ui_set_defaults(void);
void ui_set_char_defaults(void);

int ui_initsys(void)
{
    initscr();
    start_color();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    ui_set_defaults();

    return 0;
}

void ui_set_defaults(void)
{
    ui_more_left_char         = '\253';
    ui_more_right_char        = '\273';
    ui_window_vertical_char   = '|';
    ui_window_horizontal_char = '-';
    ui_window_corner_char     = '\'';

    ui_window_border_selected_face = face_init(COLOR_RED, COLOR_WHITE);
    ui_window_border_face          = face_init(COLOR_RED, COLOR_BLACK);
    ui_window_border_face->bright  = 1;
}

int ui_display_wintree_line(wintree *tree, lineno ln)
{
    wincont *cont;
    line    *l;
    size_t   posx, posy, textstart, textend, facenum, maxposx;
    const char *text;
    vec     *faces;
    face    *face;

    posx = wintree_get_posx(tree);
    posy = wintree_get_posy(tree) + ln;
    cont = wintree_get_content(tree);

    maxposx = posx + wintree_get_sizex(tree) - 2;

    l     = wincont_get_line(cont, ln);

    if (l == NULL)
        return 0;

    text  = wincont_get_line_text_const(cont, l);
    faces = line_get_faces(l);
    text  = line_get_text(l);

    facenum   = 0;
    textend   = 0;
    textstart = 0;

    while (facenum < vec_len(faces))
    {
        face = vec_get(faces, facenum);
        textstart = textend;
        textend   = face->start;
        if (ui_display_line_part(text + textstart, text + textend, maxposx, posy, &posx) == 0)
            return 0;

        textstart = textend;
        textend   = face->end;
        attron(face_get_attr(face));
        if (ui_display_line_part(text + textstart, text + textend, maxposx, posy, &posx) == 0)
        {
            attroff(face_get_attr(face));
            return 0;
        }
        attroff(face_get_attr(face));
    }

    textstart = textend;

    if (ui_display_line_part(text + textstart, (const char *)UINTPTR_MAX, maxposx, posy, &posx) == 1)
        while (posx++ < maxposx)
            mvaddch(posy, posx, ' ');

    return 0;
}

int ui_display_line_part(const char *text, const char *end, size_t maxposx, size_t posy, size_t *posx)
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

int ui_display_wintree_border(wintree *tree, face *f)
{
    size_t posx, posy, lastposx, lastposy;

    attron(face_get_attr(f));

    posx = wintree_get_posx(tree);
    posy = wintree_get_posy(tree);

    if (wintree_get_sizey(tree) == 0 || wintree_get_sizex(tree) == 0)
        return -1;

    lastposy = posy + wintree_get_sizey(tree) - 1;
    lastposx = posx + wintree_get_sizex(tree) - 1;

    fprintf(stderr, "%d\n", lastposy);

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

    attroff(face_get_attr(f));

    return 0;
}

int ui_display_wintree(wintree *tree, face *borderface)
{
    size_t ln;

    ui_display_wintree_border(tree, borderface);

/*
    ln = wintree_get_sizey(tree) - 1;
    while (ln--)
        ui_display_wintree_line(tree, ln);
*/
    return 0;
}

int ui_display_wintrees(void)
{
    wintree *selected, *curr;

    selected = wintree_get_selected();
    ui_display_wintree(selected, ui_window_border_selected_face);
    curr = wintree_iter_next(selected);

    while (curr != selected)
    {
        ui_display_wintree(curr, ui_window_border_face);
        curr = wintree_iter_next(curr);
    } while (curr != selected);

    return 0;
}

int ui_killsys(void)
{
    endwin();
    return 0;
}

int ui_resize(void)
{
    size_t termy, termx;
    getmaxyx(stdscr, termy, termx);

    fputs("Gonna set root size\n", stderr);
    wintree_set_root_size(termx, termy);
    fputs("Set root size, gonna display wintrees\n", stderr);
    ui_display_wintrees();

    return 0;
}
