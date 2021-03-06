#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <pthread.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <curses.h>
#include <malloc.h>
#include "libwindow.h"
#include <unistd.h>
pthread_mutex_t m1=PTHREAD_MUTEX_INITIALIZER;

struct win interface;

struct client{
        char name[160];
        struct client *next;
};

struct msg{
	long type;
	long pid;
	char text[160];
};

struct descriptor{
	int fd_name;
	int fd_message;
};

struct client  *users=NULL;

void del_spisok(){
	struct client *tmp;
	tmp=users;
	while(users!=NULL){
		users=tmp->next;
		free(tmp);
	}
}
void wqueue(int *fd_message)
{
	char exit[6]="_exit\0";
	struct msg mes;
	wprintw(interface.subwnd3,"Enter your message\n");
	while(1){
	 	pthread_mutex_lock(&m1);
		wrefresh(interface.subwnd3);
		wgetstr(interface.subwnd3, mes.text);
		wclear(interface.subwnd3);
 		pthread_mutex_unlock(&m1);
                mes.type=2;
                mes.pid=getpid();
                msgsnd(fd_message,&mes,sizeof(struct msg),0);
                if((strcmp(mes.text,exit))==0){
            		break;
		}
		sleep(1);
	}
}

void rqueue(struct descriptor *d)
{
	int test; int err; int i=0;
	char exit[6]="_exit\0";
	struct msg mes, name; struct client *tmp, *p_tmp, *del_tmp;
 	while(1){
		test=msgrcv(d->fd_message,&mes, sizeof(struct msg),getpid(),IPC_NOWAIT);
		if(test==-1){
			err=errno;
			if(err!=ENOMSG){
				perror(msgrcv);
				return 1;
			}
		}
		else{
			if((strstr(mes.text,exit))!=NULL)
				break;
			else{
				pthread_mutex_lock(&m1);
				if(i==18){
				 	i=0;
					wclear(interface.subwnd);
					wrefresh(interface.subwnd);
				}
				mvwprintw(interface.subwnd,i,0,"%s\n", mes.text);
				wrefresh(interface.subwnd);
				pthread_mutex_unlock(&m1);
				i++;
				sleep(1);
			}
		}
		test=msgrcv(d->fd_name,&name, sizeof(struct msg),getpid(),IPC_NOWAIT);
		if(test==-1){
			err=errno;
			if(err!=ENOMSG){
				perror(msgrcv);
				return 1;
			}
		}
		else{
			tmp=users; p_tmp=NULL; del_tmp=NULL;
			pthread_mutex_lock(&m1);
			wclear(interface.subwnd2);
			while(tmp->next!=NULL){
				if(strcmp(name.text,tmp->name)==0){
					del_tmp=tmp;
					tmp=tmp->next;
					p_tmp->next=tmp;
					free(del_tmp);
				}
				wprintw(interface.subwnd2,"%s\n",tmp->name);
				p_tmp=tmp;
				if(tmp->next!=NULL)
					tmp=tmp->next;
			}
			if((strcmp(name.text,tmp->name)!=0)&&(del_tmp==NULL)){
				wprintw(interface.subwnd2,"%s\n",tmp->name);
				tmp->next=malloc(sizeof(struct client));
				tmp=tmp->next;
				strcpy(tmp->name,name.text);
				tmp->next=NULL;
				wprintw(interface.subwnd2,"%s\n",tmp->name);
			}

			else{
				if(strcmp(name.text,tmp->name)==0){
					p_tmp->next=tmp->next;
					free(tmp);
				}
				//else
				//	wprintw(interface.subwnd2,"%s\n",tmp->name);
			}
			wrefresh(interface.subwnd2);
			pthread_mutex_unlock(&m1);
			sleep(1);
		}
	}
}

int main()
{
	key_t id; struct descriptor d;
	struct msg name; pthread_t tid_w, tid_r;
	int log=0; char exit[6]="_exit\0";
	interface=create_interface();
	id=ftok("server.c", 'S');
	d.fd_message=msgget(id, 0);
	id=ftok("server.c", 'L');
	d.fd_name=msgget(id, 0);

	wprintw(interface.subwnd3,"Enter your name\n");
	wgetstr(interface.subwnd3, name.text);
	users=malloc(sizeof(struct client));
	strcpy(users->name,name.text);
	users->next=NULL;
	wprintw(interface.subwnd2,"%s\n", users->name);
	wrefresh(interface.subwnd2);
	name.type=1;
	name.pid=getpid();
	msgsnd(d.fd_name,&name,sizeof(struct msg),0);
	pthread_create(&tid_w,NULL,wqueue, (void*)d.fd_message);
	pthread_create(&tid_r,NULL,rqueue, (void*)&d);
	pthread_join(tid_w,NULL);
	pthread_join(tid_r,NULL);
	del_interface(&interface);
	del_spisok();

}
