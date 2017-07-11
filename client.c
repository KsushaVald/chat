#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

struct msgname{
	long type;
	long pid;
	char text[160];
};

struct msgmessage{
	long type;
	char  text[160];
};

int main()
{
	key_t id; int fd_message, fd_name; int err;
	struct msgname name;
	struct msgmessage mes;
	int log=0; int test; char exit[6]="_exit\0";

	id=ftok("server.c", 'S');
	fd_message=msgget(id, 0);
	id=ftok("server.c", 'L');
	fd_name=msgget(id, 0);
	while(1){
		if(log==0){
			printf("Enter your name\n");
			scanf("%s", &name.text);
			name.type=1;
			name.pid=getpid();
			log=1;
			msgsnd(fd_name,&name,sizeof(struct msgname),0);
			printf("Enter your mуssage\n");
		}
		scanf("%s", &mes.text);
		mes.type=2;
		msgsnd(fd_message,&mes,sizeof(struct msgmessage),0);
		if(strcmp(mes.text,exit)==0)
			break;
		test=msgrcv(fd_message,&mes, sizeof(struct msgmessage),getpid(),IPC_NOWAIT);
		if(test==-1){
			err=errno;
			if(err==ENOMSG){
				//continue;
			}
  			else{
				perror(msgrcv);
				return 1;
			}
		}
	}
}
