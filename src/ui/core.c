hook *ui_on_resize;

int ui_initsys(void)
{
    initscr();
    start_color();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);

    ui_on_resize = hook_init(2);

    ui_win_initsys();
    ui_sbar_initsys();

    return 0;
}

int ui_killsys(void)
{
    endwin();

    hook_free(ui_on_resize);

    ui_win_killsys();
    ui_sbar_killsys();

    return 0;
}

int ui_refresh(void)
{
    ui_win_refresh();
    ui_sbar_refresh();

    return 0;
}

int ui_resize(void)
{
    size_t termy, termx;

    getmaxyx(stdscr, termy, termx);

    hook_call(ui_on_resize, &termy, &termx);

    wintree_set_root_size(termx, termy - 1);

    ui_refresh();

    return 0;
}
