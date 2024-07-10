#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>
#include <limits.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/shm.h>

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
	key_t msq_key = ftok("Members.txt", 60);

    if(msq_key == -1)
    {
        perror("Error generating a key\n");
        exit(1);
    }

    int msq_id = msgget(msq_key, 0);

    if (msq_id == -1)
    {
        perror("Error creating a message queue\n");
        exit(1);
    }

    message sent_msg;
    message received_msg;
    
    while (1)
    {
        int instruction_no;
        int sequence;
        char str[100];
        printf("1. Add a new graph to the database\n");
        printf("2. Modify an existing graph of the database\n");
        printf("3. Perform DFS on an existing graph of the database\n");
        printf("4. Perform BFS on an existing graph of the database\n");

        printf("Enter Sequence Number: ");
        scanf("%d", &sequence);
        printf("Enter Operation Number: ");
        scanf("%d", &instruction_no);
        printf("Enter Graph File Name: ");
        int c;
    	while ((c = getchar()) != '\n' && c != EOF) { }
        fgets(str,100,stdin);
        str[strlen(str) - 1] = '\0';

        sent_msg.sequence_no = sequence;
        sent_msg.operation_no = instruction_no;
        strcpy(sent_msg.filename, str);
        sent_msg.intended_to = 101;

        switch (instruction_no)
        {
        case 1:
        {
            char strings[40][100];
            int numStrings;
			printf("in case 1\n");
			
            printf("Enter number of nodes of the graph: ");
            scanf("%d",&numStrings);

            while (getchar() != '\n');
            
            printf("Enter adjacency matrix, each row on a separate line and elements of a single row separated by whitespace characters\n");
            for (int i = 0; i < numStrings; ++i)
            {
                // printf("String %d: ", i + 1);
                if (fgets(strings[i], 100, stdin) == NULL)
                {
                    perror("Error reading input");
                    exit(EXIT_FAILURE);
                }

                // Remove trailing newline character
                strings[i][strcspn(strings[i], "\n")] = '\0';
            }
            
            for(int i = 0;i<numStrings;i++)
            {
            	for(int j = 0;j<strlen(strings[i]);j++)
            	{
            		printf("%c",strings[i][j]);
            	}
            	printf("\n");
            	
            }

            int totalLength = 0;
            for (int i = 0; i < numStrings; ++i)
            {
                totalLength += strlen(strings[i]) + 1; // +1 for newline character
            }

            char *concatenatedString = malloc(totalLength + 1); // +1 for null terminator
            if (concatenatedString == NULL)
            {
                perror("Error allocating memory");
                exit(EXIT_FAILURE);
            }
			
			sprintf(concatenatedString,"%d",numStrings);
			concatenatedString[strlen(concatenatedString)] = '\n';
			concatenatedString[strlen(concatenatedString)] = '\0';
			int s = strlen(concatenatedString);
			
            //concatenatedString[0] = '\0'; // Initialize the concatenated string

            for (int i = 0; i < numStrings; ++i)
            {
                strcat(concatenatedString, strings[i]);
                strcat(concatenatedString, "\n"); // Add newline between strings
            }
            
           	printf("%s\n",concatenatedString);
           	printf("\n%d",totalLength+s+1);

            int shmId = shmget(sequence, totalLength+s, IPC_CREAT | 0666);
            
            if (shmId == -1)
            {
                perror("Error creating shared memory");
                exit(EXIT_FAILURE);
            }

            char *shmAddr = shmat(shmId, NULL, 0);
            if (shmAddr == (void *)-1)
            {
                perror("Error attaching to shared memory");
                exit(EXIT_FAILURE);
            }
			
			
			
            strcpy(shmAddr, concatenatedString);

            // sending msg
            if (msgsnd(msq_id, &sent_msg, sizeof(message), 0) == -1)
            {
                perror("Client failed to message server\n");
                exit(1);
            }
            msgrcv(msq_id, &received_msg, sizeof(message), sequence, 0);

            printf("%s\n", received_msg.content);
            break;
        }
        case 2:
        {

            char strings[40][100];
            int numStrings;
            printf("in case 2\n");

            printf("Enter number of nodes of the graph: ");
            scanf("%d",&numStrings);

            // Clear the input buffer
            while (getchar() != '\n')
                ;

            printf("Enter adjacency matrix, each row on a separate line and elements of a single row separated by whitespace characters\n");
            for (int i = 0; i < numStrings; ++i)
            {
                // printf("String %d: ", i + 1);
                if(fgets(strings[i], 100, stdin) == NULL)
                {
                    perror("Error reading input");
                    exit(EXIT_FAILURE);
                }

                // Remove trailing newline character
                strings[i][strcspn(strings[i], "\n")] = '\0';
            }

            int totalLength = 0;
            for (int i = 0; i < numStrings; ++i)
            {
                totalLength += strlen(strings[i]) + 1; // +1 for newline character
            }

            char *concatenatedString = malloc(totalLength + 1); // +1 for null terminator
            if (concatenatedString == NULL)
            {
                perror("Error allocating memory");
                exit(EXIT_FAILURE);
            }

			sprintf(concatenatedString,"%d",numStrings);
			concatenatedString[strlen(concatenatedString)] = '\n';
			concatenatedString[strlen(concatenatedString)] = '\0';
			int s = strlen(concatenatedString);

            for (int i = 0; i < numStrings; ++i)
            {
                strcat(concatenatedString, strings[i]);
                strcat(concatenatedString, "\n"); // Add newline between strings
            }
			
			printf("%s\n",concatenatedString);
           	printf("\n%d",totalLength+s+1);
			
            int shmId = shmget(sequence, totalLength+s, IPC_CREAT | 0666);
            if (shmId == -1)
            {
                perror("Error creating shared memory");
                exit(EXIT_FAILURE);
            }

            char *shmAddr = shmat(shmId, NULL, 0);
            if (shmAddr == (void *)-1)
            {
                perror("Error attaching to shared memory");
                exit(EXIT_FAILURE);
            }

            strcpy(shmAddr, concatenatedString);

            if (msgsnd(msq_id, &sent_msg, sizeof(message), 0) == -1)
            {
                perror("Client failed to message server\n");
                exit(1);
            }
            msgrcv(msq_id, &received_msg, sizeof(message), sequence, 0);

            printf("%s\n", received_msg.content);
            break;
        }
        case 3:
        {
            char strings[3];

            printf("Enter starting vertex: ");
            if(fgets(strings, 3, stdin) == NULL)
                {
                    perror("Error reading input");
                    exit(EXIT_FAILURE);
                }

                
            strings[strcspn(strings, "\n")] = '\0';

            int shmId = shmget(sequence, 1024, IPC_CREAT | 0666);
            if (shmId == -1)
            {
                perror("Error creating shared memory");
                exit(EXIT_FAILURE);
            }

            char *shmAddr = shmat(shmId, NULL, 0);
            if (shmAddr == (void *)-1)
            {
                perror("Error attaching to shared memory");
                exit(EXIT_FAILURE);
            }

            strcpy(shmAddr, strings);
            printf("%s\n",strings);


            if (msgsnd(msq_id, &sent_msg, sizeof(message), 0) == -1)
            {
                perror("Client failed to message server\n");
                exit(1);
            }

            if (msgrcv(msq_id, &received_msg, sizeof(message), sequence, 0) == -1)
            {
                perror("Client couldn't receive the server response properly\n");
                exit(1);
            }
			
			printf("%s\n", received_msg.content);
			
            break;
        }
        case 4:
        {
           	char strings[3];

            printf("Enter starting vertex: ");
            if(fgets(strings, 3, stdin) == NULL)
                {
                    perror("Error reading input");
                    exit(EXIT_FAILURE);
                }

                
            strings[strcspn(strings, "\n")] = '\0';
            int shmId = shmget(sequence, 1024, IPC_CREAT | 0666);
            if (shmId == -1)
            {
                perror("Error creating shared memory");
                exit(EXIT_FAILURE);
            }

            char *shmAddr = shmat(shmId, NULL, 0);
            if (shmAddr == (void *)-1)
            {
                perror("Error attaching to shared memory");
                exit(EXIT_FAILURE);
            }

            strcpy(shmAddr, strings);


            if (msgsnd(msq_id, &sent_msg, sizeof(message), 0) == -1)
            {
                perror("Client failed to message server\n");
                exit(1);
            }

            if (msgrcv(msq_id, &received_msg, sizeof(message), sequence, 0) == -1)
            {
                perror("Client couldn't receive the server response properly\n");
                exit(1);
            }
            
            printf("%s\n", received_msg.content);

            break;
        }

        default:
        {
            printf("Invalid Request. Try again with a valid request number from the menu options given below: \n");
            continue;
        }
        }
    }
}
	

