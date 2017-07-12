struct win{
        WINDOW*wnd;
        WINDOW*wnd2;
        WINDOW*wnd3;
        WINDOW*subwnd;
        WINDOW*subwnd2;
        WINDOW*subwnd3;
};

struct win create_interface();
void del_interface(struct win*);
