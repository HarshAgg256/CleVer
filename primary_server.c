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
#include<pthread.h>
#include<sys/shm.h>
#include<semaphore.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/wait.h>

#define MAX_THREADS 100
#define SEM_NAME "/sem95_main"
#define MAX_SEMAPHORES 30

struct message
{
    long intended_to;
    int sequence_no;
    int operation_no;
    char filename[100];
    char content[100];
};

typedef struct message message;

struct helper
{
    message m;
    int thread_index;
};

typedef struct helper helper;

//struct filename_sem_mapping
//{
//    char filename[100];
//    sem_t* r_sem;

//    sem_t* rw_sem;

//    int read_count;
//};

//typedef struct filename_mutex_mapping filename_mutex_mapping;

//struct shared_mutexes

//{

	//filename_mutex_mapping map[MAX_THREADS];

	//int filename_mutex_count;
	
	

//};

//pthread_mutex_t mutex_of_mutex = PTHREAD_MUTEX_INITIALIZER;

//typedef struct shared_mutexes shared_mutexes;

//#define SEM_NAME_1 "/Mutex_of_Mutex"
//#define SEM_NAME_2 "/File_mutex"


int mqid;
key_t mqkey;

pthread_t thread_identifiers[MAX_THREADS];
int thread_status[MAX_THREADS] = {0};
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
//filename_mutex_mapping filename_mutex_map[MAX_THREADS];  
//int filename_mutex_count = 0;



//key_t sm_key;

//int shared_mem_id;

//shared_mutexes *sm;

sem_t *semaphore_main = NULL;
sem_t *file_semaphores_r[MAX_SEMAPHORES] = {NULL};
sem_t *file_semaphores_rw[MAX_SEMAPHORES] = {NULL};

void *write_func(void *arg)
{
    helper *h2 = (helper *)arg;

    message m2 = h2->m;
    int thread_ind = h2->thread_index; 
	
	//pthread_mutex_lock(&mutex_of_mutex);
	
	//printf("\nbefore sleep_mom\n");
    //pthread_mutex_t *filename_mutex = NULL;
    
    //printf("\nmutex of mutex\n");
	
    //pthread_mutex_unlock(&mutex_of_mutex);
	
	//printf("\nout mutex of mutex\n");
	int dec = 0;
		
	int i = 1;
	while(m2.filename[i]!='.')
	{
		dec = dec*10 + (m2.filename[i] - 48);
		i++;
	}
	printf("dec2: %d\n",dec);
    sem_wait(file_semaphores_rw[dec]);
	printf("\n file sem starting...\n");
    FILE *file = fopen(m2.filename, "w");

    int shm_id = shmget(m2.sequence_no, 0, 0666);
    char *shm_address = shmat(shm_id, NULL, SHM_RDONLY);
    char *shm_add = shm_address;
    int n1 = 0;
    int digits = 0;
    while(*shm_add != '\n')
    {
    	n1 = n1*10;
    	n1 = n1 + (*(shm_add) - 48);
    	shm_add++;
    	digits++;
    }
    
    printf("%d",2*(n1)*(n1) + digits + 1);
    int size_yy = 2*(n1)*(n1) + digits + 1;
    //printf("\n%d",size_yy);

    fwrite(shm_address, 1, size_yy, file);
    fclose(file);
    shmdt(shm_address);

    if((m2.operation_no) == 1)
    {
        m2.intended_to = (long)m2.sequence_no;
        strcpy(m2.content,"File successfully added"); 
    }
    else
    {
        m2.intended_to = (long)m2.sequence_no;
        strcpy(m2.content,"File successfully modified");
    }

    msgsnd(mqid,&m2,sizeof(message),0);
	
	printf("\nfile_sem ending...\n");
    sem_post(file_semaphores_rw[dec]); 
    

    pthread_mutex_lock(&mutex);
    thread_status[thread_ind] = 1;
    pthread_mutex_unlock(&mutex);

    free(h2);
    pthread_exit(NULL);
}



int main()
{	
	
	mqkey = ftok("Members.txt",60);
    mqid = msgget(mqkey,0);

	//sm_key = ftok("shared_mem.txt",70);

    //shared_mem_id = shmget(sm_key,sizeof(shared_mutexes),0666 | IPC_CREAT);

	semaphore_main = sem_open(SEM_NAME, O_CREAT, 0666,1);
    //Mutex_of_Mutex = sem_open(SEM_NAME_1,O_CREAT,0666,1 );
	//File_mutex = sem_open(SEM_NAME_2,O_CREAT,0666,1 );
	

    //sm = (shared_mutexes *)shmat(shared_mem_id,NULL,0);

	//sm->filename_mutex_count = 0;
	
	message m1;
    int thread_count = 0;

    while(1)
    {
    printf("Hi\n");
    	
    	printf("Hi2\n");
        helper *h1 = (helper *)malloc(sizeof(helper));
        
        msgrcv(mqid,&m1,sizeof(message),102,0);
        printf("in the ps");
		if(m1.operation_no==5)
		{
			break;
			
		}
		int dec = 0;
		
		int i = 1;
		while(m1.filename[i]!='.')
		{
			dec = dec*10 + (m1.filename[i] - 48);
			i++;
		}
		printf("dec: %d\n",dec);
		char name_file[20];
		strcpy(name_file,m1.filename);
		strcat(name_file,"_r");
		sem_wait(semaphore_main);
		file_semaphores_r[dec] = sem_open(name_file,0);
		if (file_semaphores_r[dec] == SEM_FAILED) {
            file_semaphores_r[dec] = sem_open(name_file,O_CREAT, 0666,1);
        }
        
        strcat(name_file,"w");
		file_semaphores_rw[dec] = sem_open(name_file,0);
		if (file_semaphores_rw[dec] == SEM_FAILED) {
            file_semaphores_rw[dec] = sem_open(name_file,O_CREAT, 0666,1);
        }
		sem_post(semaphore_main);
		h1->m = m1;
        h1->thread_index = thread_count;

        pthread_create(&thread_identifiers[thread_count], NULL, write_func, (void *)h1);
        thread_count++;
        
       // sem_close(semaphore_main);
        //sem_unlink(SEM_NAME);
        
        //sem_close(file_semaphores_rw[dec]);
        //sem_unlink(m1.filename);
        
        //sem_close(semaphore_main);
        //sem_unlink(SEM_NAME);
        
        
    }

    for(int i = 0; i <= thread_count; i++) 
    {
        pthread_mutex_lock(&mutex);
        if (thread_status[i] == 0) 
        {
            pthread_mutex_unlock(&mutex);
            pthread_join(thread_identifiers[i], NULL);
        } 
        else 
        {
            pthread_mutex_unlock(&mutex);
        }
    }
    
    sem_close(semaphore_main);
    sem_unlink(SEM_NAME);


    return 0;
}
