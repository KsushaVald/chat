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
#include "libwindow.h"


struct msg{
	long type;
	long pid;
	char text[160];
};


void wqueue(int *fd_message)
{
	char exit[6]="_exit\0";
	struct msg mes;
	while(1){
		scanf("%s", &mes.text);
                mes.type=2;
                mes.pid=getpid();
                msgsnd(fd_message,&mes,sizeof(struct msg),0);
                if((strcmp(mes.text,exit))==0){
            		break;
		}
	}
}

void rqueue(int *fd_message)
{
	int test; int err;
	char exit[6]="_exit\0";
	struct msg mes;
 	while(1){
		test=msgrcv(fd_message,&mes, sizeof(struct msg),getpid(),IPC_NOWAIT);
		if(test==-1){
			err=errno;
			if(err!=ENOMSG){
				perror(msgrcv);
				return 1;
			}
		}
		else{
			if((strcmp(exit,mes.text))==0)
				break;
		//	else
		//		printf("SERVER: %s\n", mes.text);
		}
	}
}

int main()
{
	key_t id; int fd_message, fd_name;
	struct msg name; pthread_t tid_w, tid_r;
	int log=0; char exit[6]="_exit\0";
	struct win interface=create_interface();

	id=ftok("server.c", 'S');
	fd_message=msgget(id, 0);
	id=ftok("server.c", 'L');
	fd_name=msgget(id, 0);
	if(log==0){
		//printf("Enter your name\n");
		//scanf("%s", &name.text);
		name.type=1;
		name.pid=getpid();
		log=1;
		msgsnd(fd_name,&name,sizeof(struct msg),0);
		//printf("Enter your mуssage\n");
	}
	pthread_create(&tid_w,NULL,wqueue, (void*)fd_message);
	pthread_create(&tid_r,NULL, rqueue, (void*)fd_message);
	pthread_join(tid_w,NULL);
	pthread_join(tid_r,NULL);
	del_interface(&interface);
}
