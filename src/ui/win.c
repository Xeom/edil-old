char ui_win_left_char        = '\253';
char ui_win_right_char;      = '\273';
char ui_win_vertical_char;   = '|';
char ui_win_horizontal_char; = '-';
char ui_win_corner_char;     = '\'';


face *ui_win_frame_face;
face *ui_win_frame_sel_face;


static int ui_win_display_wintree(wintree *tree);
static int ui_win_display_wintree_border(wintree *tree);

int ui_win_initsys(void)
{
    ui_win_border_selected_face = face_init(COLOR_BLACK, COLOR_WHITE);
    ui_win_border_face          = face_init(COLOR_RED, COLOR_BLACK);
    ui_win_border_face->bright  = 1;

    return 0;
}

int ui_win_killsys(void)
{
    return 0;
}

int ui_win_refresh(void)
{
    int      borderattr;
    wintree *curr;

    borderattr = face_get_attr(ui_win_border_face);
    curr       = wintree_iter_start();

    attron(borderattr);

    while (curr)
    {
        ui_display_wintree(curr);
        curr = wintree_iter_next(curr);
    }

    attroff(borderattr);

    ui_win_highlight_selected();

    refresh();

    return 0;
}


static int ui_win_draw_wintree(wintree *tree)
{
    size_t ln;

    ui_display_wintree_frame(tree);

/*
    ln = wintree_get_sizey(tree) - 1;
    while (ln--)
        ui_display_wintree_line(tree, ln);
*/
    return 0;
}

static int ui_win_draw_wintree_frame(wintree *tree)
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
