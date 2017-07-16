#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>

struct spisok{
	long type;
	char *str[200];
};

struct msg{
	long type;
	long pid;
        char text[160];
};

struct client{
	char name[160];
	long pid;
 	struct client *next;
};

int main(){
	key_t id; int fd_message, fd_name; long pid;
	struct msg message, name;
	struct client *root=NULL;
	struct client *tmp, *p_tmp, *del_tmp;
	int test, err, log; char exit[6]="_exit\0";

	id=ftok("server.c", 'S');
	fd_message=msgget(id, IPC_CREAT|0666);
	id=ftok("server.c", 'L');
	fd_name=msgget(id, IPC_CREAT|0666);

	while(1){
		test=msgrcv(fd_name,&name,sizeof(struct msg),1L,IPC_NOWAIT);
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
				strcpy(root->name,name.text);
				root->pid=name.pid;
				root->next=NULL;
			}
			else{
				tmp=root;
				while(tmp->next!=NULL)
					tmp=tmp->next;
				tmp->next=malloc(sizeof(struct client));
				tmp=tmp->next;
				strcpy(tmp->name,name.text);
				tmp->pid=name.pid;
				tmp->next=NULL;
			}
			p_tmp=tmp;
			tmp=root; pid=name.pid;
			while(tmp->next!=NULL){
				name.type=tmp->pid;
				msgsnd(fd_name,&name,sizeof(struct msg),0);
				strcpy(name.text,tmp->name);
				name.type=pid;
				msgsnd(fd_name,&name,sizeof(struct msg),0);
				strcpy(name.text,p_tmp->name);
				tmp=tmp->next;
			}
		}
		test=msgrcv(fd_message,&message,sizeof(struct msg),2L,IPC_NOWAIT);
		if(test==-1){
			err=errno;
                        if(err==ENOMSG){
                                continue;
                        }
                        perror(msgrcv);
                        return 1;
		}
		p_tmp=NULL; tmp=root; log=0;
		while(tmp!=NULL){
			if(tmp->pid==message.pid){
				log=1;
				break;
			}
			tmp=tmp->next;
		}
		tmp=root;
		if((strcmp(exit,message.text)!=0)&&(log==1)){
			while(tmp!=NULL){
				message.type=tmp->pid;
				msgsnd(fd_message,&message,sizeof(struct msg),0);
				tmp=tmp->next;
			}
		}
		else{
			if(log==1){
				message.type=message.pid;
				msgsnd(fd_message,&message,sizeof(struct msg),0);
				while(tmp->pid!=message.pid){
					p_tmp=tmp;
					tmp=tmp->next;
				}
				del_tmp=tmp;
				strcpy(name.text,tmp->name);
				tmp=root;
				while(tmp!=NULL){
                        	        name.type=tmp->pid;
                	                msgsnd(fd_name,&name,sizeof(struct msg),0);
        	                        tmp=tmp->next;
	                        }
				if(del_tmp!=root)
					p_tmp->next=del_tmp->next;
				else
					root=del_tmp->next;
			 	free(del_tmp);
			}
		//	else
		//		msgsnd(fd_message,&message,sizeof(struct msg),0);

		}
	}
	msgctl(fd_message, IPC_RMID, 0);
	msgctl(fd_name, IPC_RMID, 0);
}

