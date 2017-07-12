#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <curses.h>
#include "libwindow.h"

WINDOW* window1(int y, int x, int py, int px){
	WINDOW* wnd;
	wnd=newwin(y,x,py,px);
	box(wnd, '|','-' );
	wrefresh(wnd);
	refresh();
	return wnd;
}
WINDOW* window2(WINDOW* wnd, int y, int x, int py, int px){
	WINDOW* subwnd;
	subwnd=derwin(wnd,y,x,py,px);
	wrefresh(subwnd);
	scrollok(subwnd, TRUE);
	return subwnd;
}


struct win create_interface(){
	struct win interface;
	initscr();
        cbreak();
        refresh();
        noecho();
	interface.wnd=window1(20,40,1,1);
	interface.subwnd=window2(interface.wnd,18,30,1,1);
	interface.wnd2=window1(20,10,1,42);
	interface.subwnd2=window2(interface.wnd2,18,8,1,1);
	interface.wnd3=window1(5,52,21,1);
	interface.subwnd3=window2(interface.wnd3,3,48,1,1);
	return interface;
}

void  del_interface(struct win *interface){
	delwin(interface->wnd);
	delwin(interface->subwnd);
	delwin(interface->wnd2);
	delwin(interface->subwnd2);
	delwin(interface->wnd3);
	delwin(interface->subwnd3);
	endwin();
	exit(EXIT_SUCCESS);
}

