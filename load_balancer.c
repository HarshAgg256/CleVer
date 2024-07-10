#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/msg.h>
#include<sys/ipc.h>
#include<string.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<limits.h>
#include<sys/shm.h>

struct message
{
    long intended_to;
    int sequence_no;
    int operation_no;
    char filename[100];
    char content[100];
};

typedef struct message message;

int main()
{
    int mqid;
    key_t mqkey;

    mqkey = ftok("Members.txt",60);
    mqid = msgget(mqkey, 0666 | IPC_CREAT);

    message m1;

    while(1)
    {
        msgrcv(mqid, &m1, sizeof(message), 101, 0);
        printf("%d\n",m1.operation_no);
        printf("Here I am\n");

        switch(m1.operation_no)
        {
            case 1:
            {	
            	printf("in LB case 1\n");
                m1.intended_to = 102;
                msgsnd(mqid, &m1, sizeof(message), 0);
                break;
            }

            case 2:
            {
            	printf("in LB case 1\n");
                m1.intended_to = 102;
                msgsnd(mqid, &m1, sizeof(message), 0);
                break;
            }

            case 3:
            {
                if((m1.sequence_no)%2 != 0)
                {
                    m1.intended_to = 103;
                }
                else
                {
                    m1.intended_to = 104;
                }
				
				printf("Hello\n");
                msgsnd(mqid, &m1, sizeof(message), 0);
                break;
            }

            case 4:
            {
                if((m1.sequence_no)%2 != 0)
                {
                    m1.intended_to = 103;
                }
                else
                {
                    m1.intended_to = 104;
                }

                msgsnd(mqid, &m1, sizeof(message), 0);
                break;
            }
            
            case 5:
            {
            	m1.intended_to = 102;
            	msgsnd(mqid, &m1, sizeof(message), 0);
            	
            	m1.intended_to = 103;
            	msgsnd(mqid, &m1, sizeof(message), 0);
            	
            	m1.intended_to = 104;
            	msgsnd(mqid, &m1, sizeof(message), 0);
            	
        		sleep(5);
        		
        		if (msgctl(mqid, IPC_RMID, NULL) == -1) 
        		{
        			perror("Error deleting message queue");
        			return 1;
    			}
    			
    			exit(1);
            }
            
        }
    }
}
