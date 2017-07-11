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
	char text[160];
};

struct client{
	char name[160];
	long pid;
 	struct client *next;
};

int main(){
	key_t id; int fd_message, fd_name;
	struct msgbuf message, name;
	struct client *root=NULL;
	struct client *tmp;
	int test, err;

	id=ftok("server.c", 'S');
	fd_message=msgget(id, IPC_CREAT|0666);
	id=ftok("server.c", 'L');
	fd_name=msgget(id, IPC_CREAT|0666);

	while(1){
		test=msgrcv(fd_name,&name,sizeof(struct msgname),1,IPC_NOWAIT)
		if(test==-1){
			err=errno;
			if(err!=ENOMSG){
				perror(msgrcv);
				return 1;
			}
		}
		else{
			if(root==NULL){
				root=malloc(sizeof(struct client));
				root->name=name.text;
				root->pid=name.pid;
				root->next=NULL;
			}
			else{
				tmp=root;
				while(tmp->next!=NULL)
					tmp=tmp->next;
				tmp->next=malloc(sizeof(struct client));
				tmp=tmp->next;
				tmp->name=name.text;
				tmp->pid=name.pid;
				tmp->next=NULL;
			}
		}
		test=msgrcv(fd_message,&message,sizeof(struct msgmessage),2,IPC_NOWAIT)
		if(test==-1){
			err=errno;
                        if(err==ENOMSG){
                                continue;
                        }
                        perror(msgrcv);
                        return 1;
		}

	}
	msgctl(fd_message, IPC_RMID, 0);
	msgctl(fd_name, IPC_RMID, 0);
}
