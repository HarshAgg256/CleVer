#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>

struct message
{
    long intended_to;
    int sequence_no;
    int operation_no;
    char filename[100];
    char content[100];
};

typedef struct message message;

int main() {

    int msg_id;
	key_t key=ftok("Members.txt",60);
	
	if(key==-1) 
     {
       	perror("Error generating a key\n");
       	exit(1); 
     }
     	
	msg_id=msgget(key,0);
	
	if(msg_id==-1) 
     {
       	perror("Error creating a message queue\n");
       	exit(1); 
     }
     	
    message msg;
	msg.intended_to = 101;
	msg.sequence_no = 18632178; 
	char comp;

    while (1) 
    {
		printf("Want to terminate the application? Press Y (Yes) or N (No): ");
        scanf(" %c",&comp);
        
        if (comp=='Y' || comp== 'y') 
        {
			msg.operation_no = 5;
			strcpy(msg.content, "Y");
			msgsnd(msg_id, &msg, sizeof(message), 0);
			break;
        }
		else if (comp!= 'N' && comp != 'n') 
		{
			printf("Invalid input.\n");
        }
    }
	return 0;
}
