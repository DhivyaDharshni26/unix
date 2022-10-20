#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t mutex;

void* thread(void* arg)
{
    sem_wait(&mutex);
    printf("\nEntered critical section..\n");

    //critical section sode
    sleep(4);

    //signal
    printf("\nJust Exiting critical section...\n");
    sem_post(&mutex);
}


int main()
{
    sem_init(&mutex, 0, 1);
    pthread_t t1,t2;
    pthread_create(&t1,NULL,thread,NULL);
//    sleep(2);
    pthread_create(&t2,NULL,thread,NULL);
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    sem_destroy(&mutex);
    return 0;
}

///////////////////////SLEEPING BARBER//////////////////////////////
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<errno.h>
#include<sys/ipc.h>
#include<semaphore.h>

#define N 5

time_t end_time;/*end time*/
sem_t mutex,customers,barbers;/*Three semaphors*/
int count=0;/*The number of customers waiting for haircuts*/

void barber(void *arg);
void customer(void *arg);

int main(int argc,char *argv[])
{
        pthread_t id1,id2;
        int status=0;
        end_time=time(NULL)+20;/*Barber Shop Hours is 20s*/

        /*Semaphore initialization*/
        sem_init(&mutex,0,1);
        sem_init(&customers,0,0);
        sem_init(&barbers,0,1);

        /*Barber_thread initialization*/
        status=pthread_create(&id1,NULL,(void *)barber,NULL);
        if(status!=0)
                perror("create barbers is failure!\n");
        /*Customer_thread initialization*/
        status=pthread_create(&id2,NULL,(void *)customer,NULL);
        if(status!=0)
                perror("create customers is failure!\n");

        /*Customer_thread first blocked*/
     pthread_join(id2,NULL);
        pthread_join(id1,NULL);

        exit(0);
}

void barber(void *arg)/*Barber Process*/
{
        while(time(NULL)<end_time || count>0)
        {
                sem_wait(&customers);/*P(customers)*/
                sem_wait(&mutex);/*P(mutex)*/
                count--;
                printf("Barber:cut hair,count is:%d.\n",count);
                sem_post(&mutex);/*V(mutex)*/
                sem_post(&barbers);/*V(barbers)*/
                sleep(3);
        }
}
void customer(void *arg)/*Customers Process*/
{
        while(time(NULL)<end_time)
        {
                sem_wait(&mutex);/*P(mutex)*/
                if(count<N)
                {
                        count++;
                        printf("Customer:add count,count is:%d\n",count);
                        sem_post(&mutex);/*V(mutex)*/
                        sem_post(&customer);/*V(customers)*/
                        sem_wait(&barbers);/*P(barbers)*/
                }
                else
                        /*V(mutex)*/
                        /*If the number is full of customers,just put the mutex lock let go*/
                        sem_post(&mutex);
                sleep(1);
        }
}
///////////////////cars and tunnels///////////////////////
#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<string.h>

sem_t  mutexNorth, mutexSouth;
void* tunnel(void* arg)
{
        if(strcmp(arg,"North")==0)
                sem_wait(&mutexSouth);
        else
                sem_wait(&mutexNorth);
        printf("\n %s : Entered \n",(char*)arg);
        //CriticalSection
        sleep(3);
        printf("\n %s : Exiting \n",(char*)arg);
        if(strcmp(arg,"North")==0)
                sem_post(&mutexSouth);
        else
                sem_post(&mutexNorth);
        return 0;
}

int main()
{
        sem_init(&mutexNorth,0,1);
        sem_init(&mutexSouth,0,1);
        pthread_t t1,t2,t3,t4,t5,t6,t7,t8;
        char *msg1="North",*msg2="South";
        pthread_create(&t1,NULL,tunnel,(void*)msg1);
        pthread_create(&t2,NULL,tunnel,(void*)msg1);
        pthread_create(&t3,NULL,tunnel,(void*)msg1);
        pthread_create(&t4,NULL,tunnel,(void*)msg2);
        pthread_create(&t5,NULL,tunnel,(void*)msg1);
        pthread_create(&t6,NULL,tunnel,(void*)msg2);
        pthread_create(&t7,NULL,tunnel,(void*)msg1);
        pthread_create(&t8,NULL,tunnel,(void*)msg2);
       pthread_join(t1,NULL);
        pthread_join(t2,NULL);
        pthread_join(t3,NULL);
        pthread_join(t4,NULL);
        pthread_join(t5,NULL);
        pthread_join(t6,NULL);
        pthread_join(t7,NULL);
        pthread_join(t8,NULL);
        sem_destroy(&mutexNorth);
        sem_destroy(&mutexSouth);
}




/////////////////////DINERS PHILOSOPHERS/////////////////
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define N 5
#define THINKING 2
#define HUNGRY 1
#define EATING 0
#define LEFT (num_of_philosopher + 4) % N
#define RIGHT (num_of_philosopher + 1) % N

int state[N];
int phil[N] = { 0, 1, 2, 3, 4 };

// sem_t is a typedef defined in the header file as (apparently) some variety of integer.
sem_t mutex;
sem_t S[N];

void test(int num_of_philosopher)
{
                if (state[num_of_philosopher] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING)
                {
                        // state that eating
                        state[num_of_philosopher] = EATING;

                        sleep(2);

                        printf("Philosopher %d takes fork %d and %d\n", num_of_philosopher + 1, LEFT + 1, num_of_philosopher + 1);

                        printf("Philosopher %d is Eating\n", num_of_philosopher + 1);

                        /* sem_post(&S[num_of_philosopher]) has no effect
                        during takefork
                        used to wake up hungry philosophers
                        during putfork */
                        sem_post(&S[num_of_philosopher]);
        }
}
// take up Forks
void take_fork(int num_of_philosopher)
{

        sem_wait(&mutex);

        // state that hungry
        state[num_of_philosopher] = HUNGRY;

        printf("Philosopher %d is Hungry\n", num_of_philosopher + 1);

        // eat if neighbours are not eating
        test(num_of_philosopher);

        sem_post(&mutex);

        // if unable to eat wait to be signalled
        sem_wait(&S[num_of_philosopher]);


        sleep(1);
}

// put down Forks
void put_fork(int num_of_philosopher)
{

        sem_wait(&mutex);

        // state that thinking
        state[num_of_philosopher] = THINKING;

        printf("Philosopher %d putting fork %d and %d down\n",num_of_philosopher + 1, LEFT + 1, num_of_philosopher + 1);
        printf("Philosopher %d is thinking\n", num_of_philosopher + 1);

        test(LEFT);
        test(RIGHT);

        sem_post(&mutex);
}
void* philosopher(void* num)
{

        while (1)
        {
                int* i = num;

                sleep(1);

                take_fork(*i);

                sleep(0);

                put_fork(*i);
        }
}

int main()
{
        int i;
        pthread_t thread_id[N];

        // initialize the semaphores
        sem_init(&mutex, 0, 1);

        for (i = 0; i < N; i++)

        sem_init(&S[i], 0, 0);

        for (i = 0; i < N; i++) {

        // create philosopher processes
        pthread_create(&thread_id[i], NULL,philosopher, &phil[i]);

        printf("Philosopher %d is thinking\n", i + 1);
}

        for (i = 0; i < N; i++)
        {
        pthread_join(thread_id[i], NULL);
        }
}










////////////////////////////BOUNDED BUFFER///////////////

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>

/*
This program provides a possible solution for producer-consumer problem using mutex and semaphore.
I have used 5 producers and 5 consumers to demonstrate the solution. You can always play with these values.
*/

#define MaxItems 5 // Maximum items a producer can produce or a consumer can consume
#define BufferSize 5 // Size of the buffer

sem_t empty;
sem_t full;
int in = 0;
int out = 0;
int buffer[BufferSize];
pthread_mutex_t mutex;

void *producer(void *pno)
{
    int item;
    for(int i = 0; i < MaxItems; i++) {
        item = rand(); // Produce an random item
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        buffer[in] = item;
        printf("Producer %d: Insert Item %d at %d\n", *((int *)pno),buffer[in],in);
        in = (in+1)%BufferSize;
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
}
void *consumer(void *cno)
{
    for(int i = 0; i < MaxItems; i++) {
        sem_wait(&full);
  pthread_mutex_lock(&mutex);
        int item = buffer[out];
        printf("Consumer %d: Remove Item %d from %d\n",*((int *)cno),item, out);
        out = (out+1)%BufferSize;
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
}

int main()
{

    pthread_t pro[5],con[5];
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty,0,BufferSize);
    sem_init(&full,0,0);

    int a[5] = {1,2,3,4,5}; //Just used for numbering the producer and consumer

    for(int i = 0; i < 5; i++) {

 pthread_create(&pro[i], NULL, (void *)producer, (void *)&a[i]);
    }
    for(int i = 0; i < 5; i++) {
        pthread_create(&con[i], NULL, (void *)consumer, (void *)&a[i]);
    }

    for(int i = 0; i < 5; i++) {
        pthread_join(pro[i], NULL);
    }
    for(int i = 0; i < 5; i++) {
        pthread_join(con[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;

}

//////////////pipes-file2.c/////////////////////
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>


int main()
{

        int pipe0[2], pipe1[2];

        pipe(pipe0);

        printf("2 Pipes 2 Process");

        char buffer;
        int fd = open("f11.txt", O_RDONLY);

        if(fd == -1)
                printf("Error opening file!");
        else
        {
                while(read(fd, &buffer, 1))
                        write(pipe0[1], &buffer, 1);
        }

        pipe(pipe1);
        int forkRes = fork();

        if(forkRes == 0)
        {
                dup2(pipe0[0], 0);
                dup2(pipe1[1], 1);
                close(pipe0[1]);
                close(pipe1[0]);
                execlp("head","head","-n2",NULL);
                //execlp("grep", "grep", "admin", NULL);
        }

        else
        {
                wait(NULL);
                dup2(pipe1[0], 0);
                close(pipe0[0]);
    close(pipe0[1]);
                close(pipe1[1]);
}
}






//////////////fileopen.c/////////////////
#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>


int main()
{

        int pipe0[2], pipe1[2];

        pipe(pipe0);

        printf("2 Pipes 2 Process");

        char buffer;
        int fd = open("f11.txt", O_RDONLY);

        if(fd == -1)
                printf("Error opening file!");
        else
        {
                while(read(fd, &buffer, 1))
                        write(pipe0[1], &buffer, 1);
        }

        pipe(pipe1);
        int forkRes = fork();

        if(forkRes == 0)
        {
                dup2(pipe0[0], 0);
                dup2(pipe1[1], 1);
                close(pipe0[1]);
                close(pipe1[0]);
                execlp("grep", "grep", "admin", NULL);
        }

    else
        {
                wait(NULL);
                dup2(pipe1[0], 0);
                close(pipe0[0]);
                close(pipe0[1]);
                close(pipe1[1]);
                execlp("wc", "wc", "-l", NULL);
        }
}







////////////////////ls|wc//////////////////

//ls|wc

#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
int main(int argc,char* argk[])
{
int pfd[2];
if(pipe(pfd)==-1)
{
printf("error in pipe");
}
int id=fork();
if(id<1)
{
printf("error");
}
if(id==0)
{
close(pfd[0]);
dup2(pfd[1],1);
execlp("ls","ls",NULL);
}
else
{
wait(NULL);
close(pfd[1]);
dup2(pfd[0],0);
execlp("wc","wc",NULL);
}
}

/////////////msgpass//////////////
  GNU nano 2.5.3                                           File: msgpass.c

//PASSING A MSG FROM PARENT TO CHILD


#include<stdlib.h>
#include<stdio.h>
#include<sys/wait.h>
#include<unistd.h>
int main(int argc,char* argv[])
{
int fd[2]; //two file descriptors read and write
//fd[0]->read
//fd[1]->write
if(pipe(fd)==-1)
{
printf("error occured while creating the pipe");
}
int id=fork();
if(id<0)
{
printf("an eroor occured while forking");
}
if(id==0)//id==0 for child
{
close(fd[0]);//close reading end as child is abt to write
printf("enter a number");
int a;
scanf("%d",&a);
write(fd[1],&a,sizeof(int));//write system call syntax
close(fd[1]);//close writing end
}
else
{
close(fd[1]);//close reading end
int y;
read(fd[0],&y,sizeof(int));//read syntax
close(fd[0]);//close read end
printf("recieved from child process %d",y);
return 0;
}
}
///////////////product////////////////
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
int main(int argc,char* argv[])
{
int p1[2];//c to p
int p2[2];//p to c
if(pipe(p1)==-1)
{
printf("error");
}
if(pipe(p2)==-1)
{
printf("error");
}
int forkr=fork();
if(forkr<0)
{
printf("error");
}
if(forkr==0)
{
int y;
close(p1[0]);
close(p2[1]);
read(p2[0],&y,sizeof(y));
printf("received %d",y);
int pro;
pro=y*4;
write(p1[1],&pro,sizeof(pro));
close(p1[1]);
close(p2[0]);
}
else
{
close(p1[1]);
close(p2[0]);
x=rand()%10;
write(p2[1],&x,sizeof(x));
int su;
read(p1[0],&su,sizeof(su));
printf("product is %d",su);
close(p2[1]);
close(p1[0]);
}
}



/////////////three process//////////////
//cat f1.txt|head|wc
#include<stdio.h>
int main()
{
        int pd[2];
        int fd;

        pipe(pd);
        fd = fork();

        if(fd==0)
        {
                printf("Child 1");
                close(pd[0]);
                dup2(pd[1],1);
                execlp("cat","cat","f11.txt",NULL);
                //execlp("ls","ls",NULL);
        }
        else
        {
                wait(NULL);
                int pd1[2];
                int fd1;

                pipe(pd1);
                fd1 = fork();

                if(fd1==0)
                {
                        printf("Child 2");
                        close(pd[1]);
                        dup2(pd[0],0);
                        close(pd1[0]);
                        dup2(pd1[1],1);
                        execlp("head","head",NULL);

                }
                else
  {
                        wait(NULL);
                        printf("Parent");
                        close(pd1[1]);
                        dup2(pd1[0],0);
                        execlp("wc","wc",NULL);
                }
}
}


