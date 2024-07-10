#include <stdio.h>

#include <stdlib.h>

#include <stdbool.h>

#include <pthread.h>

#include <unistd.h>

#include <sys/msg.h>

#include <sys/types.h>

#include <sys/ipc.h>

#include <sys/shm.h>

#include <string.h>

#include <semaphore.h>

#include <fcntl.h>

#include <sys/stat.h>

#include <sys/wait.h>

#define MAX_SIZE 100

#define mm 100

#define MAX_THREADS 100

#define SEM_NAME "/sem95_main"

#define MAX_SEMAPHORES 30



int qsize = 0;

int readcount_arr[30]={0};

int mqid;

key_t mqkey;

//message rec_msg;



struct messege{

    long intended_to;

    int sequence_no;

    int operation_no;

    char filename[100];

    char content[100];

};

typedef struct messege message;



struct Queue {

    int front, rear;

    int capacity;

    int *array;

};



struct Queue* createQueue(int capacity) {

    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));

    queue->capacity = capacity;

    queue->front = queue->rear = -1;

    queue->array = (int*)malloc(queue->capacity * sizeof(int));

    return queue;

}



bool isQueueEmpty(struct Queue* queue) {

    return queue->front == -1;

}



void enqueue(struct Queue* queue, int item) {

    if (isQueueEmpty(queue))

        queue->front = 0;



    queue->rear++;

    queue->array[queue->rear] = item;

}



int dequeue(struct Queue* queue) {

    int item = queue->array[queue->front];



    if (queue->front == queue->rear)

        queue->front = queue->rear = -1;

    else

        queue->front++;



    return item;

}

struct helper{

	

	message m;

	int thread_index;

};



typedef struct helper helper;


struct ThreadArgs1 {

    int vertex;

    bool *visited;

    int (*adjMatrix)[MAX_SIZE];

    int size;

    pthread_t *threads;

    int *threadCount;

    struct Queue *Q1;

    struct Queue *ans;

};



struct ThreadArgs {

    int vertex;

    bool *visited;

    int (*adjMatrix)[MAX_SIZE];

    int size;

    pthread_t *threads;

    int *threadCount;

    int *path; 

    int node;// Path to store the vertices in the current thread

};



//struct ThreadArgs2 {

   // message m1;

    // Path to store the vertices in the current thread

//};



pthread_t thread_identifiers[MAX_THREADS];

int thread_status[MAX_THREADS] = {0};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;






// aaj ka yaha aaega









void readAdjMatrix(const char *filename, int adjMatrix[MAX_SIZE][MAX_SIZE], int *size) {

    FILE *file = fopen(filename, "r");

    if (file == NULL) {

        perror("Error opening file ");

        exit(EXIT_FAILURE);

    }



    // Read the number of nodes from the first row

    fscanf(file, "%d", size);



    if (*size <= 0 || *size > MAX_SIZE) {

        fprintf(stderr, "Invalid number of nodes in the file.\n");

        exit(EXIT_FAILURE);

    }



    // Read the adjacency matrix from the subsequent rows

    for (int i = 0; i < *size; i++) {

        for (int j = 0; j < *size; j++) {

            fscanf(file, "%d", &adjMatrix[i][j]);

        }

    }



    fclose(file);

}





void *processLevel(void *args) {

    struct ThreadArgs1 *threadArgs = (struct ThreadArgs1 *)args;



    int vertex = threadArgs->vertex;

    bool *visited = threadArgs->visited;

    int (*adjMatrix)[MAX_SIZE] = threadArgs->adjMatrix;

    int size = threadArgs->size;

    struct Queue* queue = threadArgs->Q1;



    for (int i = 0; i < size; i++) {

        if (adjMatrix[vertex][i] == 1 && !visited[i]) {

            visited[i] = true;

            enqueue(queue,i);

        }

    }



    free(args);

    pthread_exit(NULL);

}



void bfs(int start, bool *visited1, int adjMatrix[MAX_SIZE][MAX_SIZE], int size,pthread_t *threads, int *threadCount,struct Queue* ans) {

    struct Queue* queue = createQueue(size);

	

    visited1[start] = true;

    enqueue(queue, start);

    qsize++;

	//printf("queue size: %d\n",qsize);

    int l1=1;

    //printf("\n\n");

    while (!isQueueEmpty(queue)) {

        //printf("\nLevel %d\n",l1);

        l1++;

        int vertex = dequeue(queue);

        //printf("%d  ",vertex+1);

        enqueue(ans,vertex+1);

        qsize--;

        //printf("\nsize: %d \n",size);

        for (int i = 0; i < size; i++) {

            if (adjMatrix[vertex][i] == 1 && !visited1[i]) {

                visited1[i] = true;

                //printf("here  \n");

                enqueue(queue, i);

                qsize++;

            }

        }

        int mm1 = qsize;

        //printf("\nmm : %d\n",mm1);

        for(int i=0;i<mm1;i++)

        {

            struct ThreadArgs1 *args = malloc(sizeof(struct ThreadArgs1));

            args->vertex = dequeue(queue);

            qsize--;

            args->visited = visited1;

            args->adjMatrix = adjMatrix;

            args->size = size;

            args->threads = threads;

            args->threadCount = threadCount;

            args->Q1 = queue;

            args->ans= ans;

            //printf("%d  ",args->vertex+1);

            enqueue(ans,args->vertex+1);

            pthread_create(&threads[*threadCount], NULL, processLevel, (void *)args);

            threadCount++; //socho

        }

        for(int i=0;i<mm;i++)

        {

            pthread_join(threads[i],NULL);

        }



    }



    free(queue->array);

    free(queue);

}





void *dfsThread(void *args) {

    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;



    int vertex = threadArgs->vertex;

    bool *visited = threadArgs->visited;

    int (*adjMatrix)[MAX_SIZE] = threadArgs->adjMatrix;

    int size = threadArgs->size;

    pthread_t *threads = threadArgs->threads;

    int *threadCount = threadArgs->threadCount;

    int *path = threadArgs->path;

	int node = threadArgs->node;





//    printf("%d ", vertex);

    visited[vertex] = true;



    //printf("here %d  ",vertex+1);



    path[*threadCount] = vertex+1; // Store the current vertex in the path

   // printf("\npath before : ");

//    for(int k=0;k<mm;k++)

//    {

//        printf("%d ",path[k]);

//    }





    for (int i = 0; i < size; i++) {

        //printf("being human %d\n",vertex);

        if (adjMatrix[vertex][i] == 1 && !visited[i]) {

            struct ThreadArgs *newArgs = malloc(sizeof(struct ThreadArgs));

            newArgs->vertex = i;

            newArgs->visited = visited;

            newArgs->adjMatrix = adjMatrix;

            newArgs->size = size;

            newArgs->threads = threads;

            newArgs->threadCount = threadCount;

            newArgs->path = path;

            newArgs->node = node;

            //printf("here to enjoy\n");

            pthread_create(&threads[*threadCount], NULL, dfsThread, (void *)newArgs);

            pthread_join(threads[*threadCount],NULL);

            (*threadCount)++;

        }

    }

    if(vertex!=node) {

        free(args);

        pthread_exit(NULL);

       }

}



void dfs(int start, bool *visited, int adjMatrix[MAX_SIZE][MAX_SIZE], int size, pthread_t *threads, int *threadCount, int *path) {

    struct ThreadArgs *args = malloc(sizeof(struct ThreadArgs));

    args->vertex = start;

    args->visited = visited;

    args->adjMatrix = adjMatrix;

    args->size = size;

    args->threads = threads;

    args->threadCount = threadCount;

    args->path = path;

	args->node = start;

    dfsThread((void *)args);

}


sem_t *semaphore_main = NULL;

sem_t *file_semaphores_r[MAX_SEMAPHORES] = {NULL};

sem_t *file_semaphores_rw[MAX_SEMAPHORES] = {NULL};

void *first_ever_thread(void* args)

{	

	//printf("first ever\n");
	 helper *h2 = (helper *)args;
	 message rec_msg = h2->m;
	int thread_ind = h2->thread_index; 

	//message rec_msg = *((message *)args);

	int adjMatrix[MAX_SIZE][MAX_SIZE];

    int size=0;
    
    
    int dec = 0;

		

	int i = 1;

	while(rec_msg.filename[i]!='.')

	{

		dec = dec*10 + (rec_msg.filename[i] - 48);

		i++;

	}

	printf("dec2: %d\n",dec);
    
    

	//printf("here 1\n");
	
	
	
	//printf("\nMOM started \n");
	
    //sleep(5);
   
	sem_wait(file_semaphores_r[dec]);
	
	printf("\nfile mutex1 start\n");
	readcount_arr[dec]++;

     if(readcount_arr[dec] == 1)
     {
		
        sem_wait(file_semaphores_rw[dec]);
        printf("rw\n");
     }

    
    //printf("\nfile mutex1 ended\n");
    printf("\nrd_cnt: %d\n",readcount_arr[dec]);
    
    sem_post(file_semaphores_r[dec]);
    
	readAdjMatrix(rec_msg.filename, adjMatrix, &size);
	printf("\nReadinggggggggggggg\n");
    printf("%d\n",size);
    
    for(int k=0;k<size;k++)
    {
    	for(int z=0;z<size;z++)
    	{
    		printf("%d ",adjMatrix[k][z]);
		}
		printf("\n");
    }
    
	//sleep(10);
	sem_wait(file_semaphores_r[dec]);
	printf("\nfile mutex2 start\n");
     

   readcount_arr[dec]--;

     if(readcount_arr[dec] == 0)

     {
     	
        sem_post(file_semaphores_rw[dec]); 

     }

    
    
	//printf("\nfile mutex2 ended\n");
	printf("\nrd_cnt: %d\n",readcount_arr[dec]);
	
     sem_post(file_semaphores_r[dec]);
	

    int shm_id = shmget(rec_msg.sequence_no, 1024, 0666);

    char *shm_address = shmat(shm_id, NULL, SHM_RDONLY);



   int node = 0;

	int s = 0;
	
	char start_node[3];
	
	strcpy(start_node,shm_address);
	
	while(s<strlen(start_node))

	{

		node = node*10 + (start_node[s] - 48);

		s++;

	}

	

	printf("node :%d \n",node);	

	

    bool visited[MAX_SIZE] = {false};

    bool visited1[MAX_SIZE] = {false};

    pthread_t threads[MAX_SIZE];

    int threadCount = 0;

    int path[mm]={0}; // Array to store the path in each thread



	if(rec_msg.operation_no == 3){

    dfs(node-1, visited, adjMatrix, size, threads, &threadCount, path);

    //printf("here nowwwwwwwwwwwwww \n");

    char ans_dfs[100];

	int count=0;

	for(int i=0;i<mm;i++)

	{

		if(path[i]>0)

		{

			//printf("\n  %d  ",path[i]);

		}

	}

	//printf("\n");

    for(int i=0;i<mm;i++)

    {

        if(path[i]>0)

        {
			if(path[i]<=9){
        	ans_dfs[count]=path[i]+48;
        	ans_dfs[++count] = ' ';
    		count++;
			}
			else
			{
				ans_dfs[count] = (path[i]/10) + 48;
				ans_dfs[++count] = (path[i]%10) + 48;
				ans_dfs[++count] = ' ';
				count++;
			
			}
        

    	}

    }

    ans_dfs[count] = '\0';

    //printf("%s\n",ans_dfs);

    message send_msg= rec_msg;

    send_msg.intended_to = rec_msg.sequence_no;

    strcpy(send_msg.content,ans_dfs);

     

    msgsnd(mqid, &send_msg, sizeof(message), 0);

    }

    else if(rec_msg.operation_no == 4){

    struct Queue* ans = createQueue(size);

    bfs(node-1, visited1, adjMatrix, size,threads,&threadCount,ans);

    printf("In bfs\n");

    char ans_bfs[100];



    int count=0;

    while(!isQueueEmpty(ans))

    {	
		int y = dequeue(ans);
		
		
		if(y<=9)
    	{
    		ans_bfs[count++] = y+48;
    		ans_bfs[count++]= ' ';
    	}
    	else
    	{
    		ans_bfs[count++] = (y/10)+48;
    		ans_bfs[count++]= (y%10)+48;
    		ans_bfs[count++] = ' ';
    	}
    	

    }

     ans_bfs[count]='\0';



    //printf("%s\n",ans_bfs);

    message send_msg= rec_msg;



    send_msg.intended_to = rec_msg.sequence_no;



    strcpy(send_msg.content,ans_bfs);



    msgsnd(mqid, &send_msg, sizeof(message), 0);

    printf("out of bfs");

   }

	pthread_mutex_lock(&mutex);

    thread_status[thread_ind] = 1;

    pthread_mutex_unlock(&mutex);



    free(h2);

    pthread_exit(NULL);

}



int main() {

    

    int num=0;

	printf("which secondary server is this ?");

	scanf("%d",&num);

	num=num+102;

    

    mqkey = ftok("Members.txt",60);

    mqid = msgget(mqkey,0);



	
    

    semaphore_main = sem_open(SEM_NAME, O_CREAT, 0666,1);
    
	int thread_count = 0;
    //sm->filename_mutex_count = 0;
	
	message rec_msg;

    while(1){
    	printf("hi");
	
	printf("\nhi2\n");
	helper *h1 = (helper *)malloc(sizeof(helper));
	
    

    msgrcv(mqid, &rec_msg, sizeof(message),num , 0);

    //printf("in the ssd\n");
    

    printf("rec_msg: %d",rec_msg.operation_no);
	
	if(rec_msg.operation_no==5)

		{

			break;

			

		}
	
	int dec = 0;

		

		int i = 1;

		while(rec_msg.filename[i]!='.')

		{

			dec = dec*10 + (rec_msg.filename[i] - 48);

			i++;

		}

		printf("dec: %d\n",dec);

		char name_file[20];

		strcpy(name_file,rec_msg.filename);

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
	
	h1->m = rec_msg;
	h1->thread_index = thread_count;
	
	
	//pthread_t tid;

	pthread_create(&thread_identifiers[thread_count],NULL,first_ever_thread,(void *)h1);
	thread_count++;

	printf("main thread");
	

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

   

    return 0;

}
