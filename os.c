//Multithreading

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

void *calcPrime(void *inp){
	int *input = (int *)inp;
	int flags = 0;	
	for(int i=2; i<=*input/2; i++){
		if(*input%i == 0){
			flags++;
			break;
		}
	}
	if(flags == 0){
		printf("%d is a prime number \n",*input);
	}else{
		printf("%d is not a prime number \n",*input);
	}
}

void *armstrong(void *inp){
	int *input = (int *)inp;
	int temp = *input;
	int n = 0;
	float res = 0.0;
	for(temp = *input; temp!=0; ++n){
		temp/=10;
	}
	for(temp = *input; temp!=0; temp/=10){
		int remainder = temp % 10;
		int tempres = 1;
		for(int j=0; j<n; j++){
			tempres *= remainder;
		}
		res += tempres;
	}
	if((int)res == *input){
		printf("%d is an armstrong \n", *input);
	}else{
		printf("%d is not an armstrong \n", *input);
	}
}

void *factorial(void *inp){
	int *input = (int *)inp;
	long long int res = 1;
	for(int i=1; i<=*input; i++){
		res*=i;
	}
	printf("Factorial of %d is %lld \n", *input, res);
}

int main(){
	pthread_t threads[6];
	int n;
	while(1>0){
		printf("Enter your value:- \n");
		scanf("%d", &n);
		pthread_create(&threads[0], NULL, calcPrime, (void *)&n);
		pthread_create(&threads[1], NULL, factorial, (void *)&n);
		pthread_create(&threads[2], NULL, armstrong, (void *)&n);
		pthread_join(threads[0], NULL);
		pthread_join(threads[1], NULL);
		pthread_join(threads[2], NULL);
	}
	exit(0);
}


____________________________________________________________

//Producer consumer 

#include <stdio.h>
int empty=5,full=0,s=1;
void wait(int* s){
    while(*s<=0);
    (*s)--;
}
void signal(int* s){
    (*s)++;
}
void producer(int buffer[]){
    if(empty<=0){
        printf("buffer is full");
        return;
    }
    wait(&empty);
    wait(&s);
    printf("Enter a item to produce");
    scanf("%d",&buffer[full]);
    signal(&s);
    signal(&full);
}
void consumer(int buffer[]){
    if(full<=0){
        printf("buffer is empty\n");
        return;
    }
    wait(&full);
    wait(&s);
    printf("%d is consumed.",buffer[full]);
    signal(&s);
    signal(&empty);
}
int main() {
    int a[5],ch;
    do{
       
        printf("\n1.Produce\n2.Consume\n3.Exit\nEnter your choice: ");
        scanf("%d",&ch);
        switch(ch){
            case 1:
            producer(a);
            break;
            case 2:
            consumer(a);
            break;
            default:
            printf("Enter valid choice!!!");
            break;
        }
    }while(ch!=3);

    return 0;
}


____________________________________________________________

//Reader writer

#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define MAXTHREAD 5  /* define # readers */

int SharedData;

void* reader();
void* writer();

sem_t rLock,wLock;                /* establish que */

int rc = 0;             /* number of processes reading or wanting to */

int main()
{
    pthread_t rId[MAXTHREAD],wId;
    int i,k;
    //int ids[MAXTHREAD]; /*readers and initialize mutex, q and db-set them to 1*/


    sem_init(&rLock,0,1);
    sem_init(&wLock,0,1);


    for(i=0; i<MAXTHREAD; i++)
    {
	k = i+1;
        if(pthread_create(&rId[i],0,reader,&k)!=0)
	{
            perror("Cannot create reader!");
            exit(1);
        }
    }

    if(pthread_create(&wId,0,writer,0)!=0){
        perror("Cannot create writer");
        exit(1);
    }

    pthread_join(wId,0);
    for(i=0; i<MAXTHREAD; i++)
    {
    	pthread_join(rId[i],0);
    }
    
    sem_destroy (&rLock);
    sem_destroy (&wLock);
 
	

    return 0;
}

void* reader(void *arg)                  /* readers function to read */
{
    //sleep(1);
    int id = *(int*)arg;
    while(1)
    {

	sleep(1);
        sem_wait(&rLock);
		rc++;
		if(rc==1)
			sem_wait(&wLock);
        sem_post(&rLock);

        printf("\nReader %d Read Value:%d...",id,SharedData);
	
        sem_wait(&rLock);
            	rc--;
	 	if(rc==0)
			sem_post(&wLock);
       sem_post(&rLock);
    }

    return 0;
}

int getNext()
{	
	static int data=100;
	return data++;
}
void* writer()          /* writer's function to write */
{
    while(1)
    {
    
        sem_wait (&wLock);

            printf("\nWriter is now writing...Number of readers: %d\n",rc);
	    SharedData = getNext();
  
	sem_post (&wLock);
	sleep(1);
    }

    return 0;
}


____________________________________________________________


//Petersons Solution

Peterson 2 process


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#define NUM_SEATS 100
int totalSeats = NUM_SEATS;
int choosing[2] = {0, 0};
int turn = 0;
void* server(void* arg) {
while (true) {
int choice;
printf("Server: Total seats available: %d\n", totalSeats);
printf("Server: Waiting for client input (0 to exit, 1 to book, 2 to cancel): ");
scanf("%d", &choice);
if (choice == 0) {


break;
} else if (choice == 1) {
int count;
printf("Server: Enter the number of seats to book: ");
scanf("%d", &count);
// Critical Section
choosing[0] = 1;
turn = 1;
while (choosing[1] && turn == 1);
// Enter critical section
if (count <= totalSeats) {
totalSeats -= count;
printf("Server: %d seat(s) booked. Total seats available: %d\n", count, totalSeats);
} else {
printf("Server: Not enough seats available for booking.\n");
}
// Exit critical section
choosing[0] = 0;
} else if (choice == 2) {
int count;
printf("Server: Enter the number of seats to cancel: ");
scanf("%d", &count);
// Critical Section
choosing[0] = 1;
turn = 1;
while (choosing[1] && turn == 1);
// Enter critical section
totalSeats += count;
printf("Server: %d seat(s) canceled. Total seats available: %d\n", count, totalSeats);
// Exit critical section
choosing[0] = 0;
} else {
printf("Server: Invalid choice. Try again.\n");
}
}
pthread_exit(NULL);
}


void* client(void* arg) {
while (true) {
int choice;
printf("Client: Waiting for your choice (0 to exit, 1 to book, 2 to cancel): ");
scanf("%d", &choice);
if (choice == 0) {
break;
} else if (choice == 1 || choice == 2) {
int count;
printf("Client: Enter the number of seats: ");
scanf("%d", &count);
if (count >= 0) {
// Critical Section
choosing[1] = 1;
turn = 0;
while (choosing[0] && turn == 0);
// Enter critical section
if (choice == 1) {
printf("Client: Request to book %d seat(s).\n", count);
} else {
printf("Client: Request to cancel %d seat(s).\n", count);
}
// Exit critical section
choosing[1] = 0;
} else {
printf("Client: Invalid number of seats. Try again.\n");
}
} else {
printf("Client: Invalid choice. Try again.\n");
}
}
pthread_exit(NULL);
}
int main() {
pthread_t server_thread, client_thread;
pthread_create(&server_thread, NULL, server, NULL);
pthread_create(&client_thread, NULL, client, NULL);


pthread_join(server_thread, NULL);
pthread_join(client_thread, NULL);
printf("Program exiting...\n");
return 0;
}


___________________________________________________________

Peterson n process




#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


#define MAX_PROCESSES 10


int num_seats = 0;
int choosing[MAX_PROCESSES];
int number[MAX_PROCESSES];
int n;  // Number of processes


pthread_mutex_t input_mutex = PTHREAD_MUTEX_INITIALIZER;  // Mutex for synchronizing input operations


void enter_critical_section(int process_id) {
    for (int i = 0; i < n; i++) {
        if (i != process_id) {
            choosing[process_id] = 1;
            number[process_id] = 1 + (number[i] > number[process_id] ? number[i] : number[process_id]);
            choosing[process_id] = 0;
            while (choosing[i]) ;
            while (number[i] != 0 && (number[i] < number[process_id] || (number[i] == number[process_id] && i < process_id))) ;
        }
    }
}


void leave_critical_section(int process_id) {
    number[process_id] = 0;
}


void* process(void* arg) {
    int process_id = *(int*)arg;
    int choice, count;


    while (1) {
        pthread_mutex_lock(&input_mutex);  // Lock for input synchronization


        printf("Process %d: Enter 1 to book seats, 2 to cancel seats, 0 to exit: ", process_id);
        scanf("%d", &choice);


        if (choice == 0) {
            pthread_mutex_unlock(&input_mutex);  // Unlock before exiting
            pthread_exit(NULL);
        }


        if (choice == 1 || choice == 2) {
            printf("Process %d: Enter number of seats: ", process_id);
            scanf("%d", &count);


            if (count < 0) {
                printf("Process %d: Invalid number of seats. Try again.\n", process_id);
                pthread_mutex_unlock(&input_mutex);  // Unlock before continuing
                continue;
            }


            enter_critical_section(process_id);


            if (choice == 1) {
                if (count <= num_seats) {
                    num_seats -= count;
                    printf("Process %d: %d seat(s) booked. Total seats available: %d\n", process_id, count, num_seats);
                } else {
                    printf("Process %d: Not enough seats available for booking.\n", process_id);
                }
            } else if (choice == 2) {
                num_seats += count;
                printf("Process %d: %d seat(s) canceled. Total seats available: %d\n", process_id, count, num_seats);
            }


            leave_critical_section(process_id);
        } else {
            printf("Process %d: Invalid choice. Try again.\n", process_id);
        }


        pthread_mutex_unlock(&input_mutex);  // Unlock after processing input
    }
    return NULL;
}


int main() {
    pthread_t threads[MAX_PROCESSES];
    int process_ids[MAX_PROCESSES];


    printf("Enter the total number of seats: ");
    scanf("%d", &num_seats);


    printf("Enter the number of processes (1 to %d): ", MAX_PROCESSES);
    scanf("%d", &n);


    if (n < 1 || n > MAX_PROCESSES) {
        printf("Invalid number of processes.\n");
        return 1;
    }


    for (int i = 0; i < n; i++) {
        choosing[i] = 0;
        number[i] = 0;
        process_ids[i] = i;
        pthread_create(&threads[i], NULL, process, &process_ids[i]);
    }


    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }


    pthread_mutex_destroy(&input_mutex);  // Destroy the mutex
    printf("Program exiting...\n");
    return 0;
}


____________________________________________________________

//Dining philosopher

#include<stdio.h>
#include<semaphore.h>
#include<pthread.h>

#define N 5

#define LEFT (i+N-1)%N
#define RIGHT (i+1)%N

#define THINKING 0
#define HUNGRY 1
#define EATING 2

sem_t stLock,pLock[N];
int state[N];

void *philosopher(void*);
void put_forks(int );
void take_forks(int );
void test(int );

void *philosopher(void* p)
{
    int i,j;
    i=*(int*)p;
    

	//for(j=0;j<5;j++)
          //	printf("\nBefore Taking Forks (%d): P-%d: %d",i,j,state[j]);
    
   	take_forks(i);
      
	for(j=0;j<5;j++)
          	printf("\nDuring Eating (%d): P-%d: %d",i,j,state[j]);

	printf("\n\nP-%d is EATING now...\n",i);

	for(j=0;j<1000000000;j++);
                
    
	put_forks(i);
    
	for(j=0;j<5;j++)

      		printf("\nAfter Putting Fork Down (%d): P-%d: %d ",i,j,state[j]);
    
}
void take_forks(int i)
{
    sem_wait(&stLock);
        state[i]=HUNGRY;
        test(i);
    sem_post(&stLock);
    sem_wait(&pLock[i]);
}
void put_forks(int i)
{
    sem_wait(&stLock);
      state[i]=THINKING;
      test(LEFT);
      test(RIGHT);
    sem_post(&stLock);
}
void test(int i)
{
   if(state[i]==HUNGRY && state[LEFT]!=EATING && state[RIGHT]!=EATING)
   {
       sem_post(&pLock[i]);
       state[i]=EATING;
       
    } 
}
int main()
{
  int i,a[5]={0,1,2,3,4},j;
  pthread_t p[5];

  sem_init(&stLock,0,1);

  for(j=0;j<N;j++)
     sem_init(&pLock[j],0,0);

  for(i=0;i<5;i++)
    pthread_create(&p[i],NULL,philosopher,&a[i]);


  for(i=0;i<5;i++)
     pthread_join(p[i],NULL);


	return 0;
}


____________________________________________________________


//Deadlock Detection

#include<stdio.h>
static int mark[20];
int i, j, np, nr;


int main()
{
int alloc[10][10],request[10][10],avail[10],r[10],w[10];


printf ("\nEnter the no of the process: ");
scanf("%d",&np);
printf ("\nEnter the no of resources: ");
scanf("%d",&nr);
for(i=0;i<nr; i++)
{
printf("\nTotal Amount of the Resource R % d: ",i+1);
scanf("%d", &r[i]);
}
printf("\nEnter the request matrix:");


for(i=0;i<np;i++)
for(j=0;j<nr;j++)
scanf("%d",&request[i][j]);


printf("\nEnter the allocation matrix:");
for(i=0;i<np;i++)
for(j=0;j<nr;j++)
scanf("%d",&alloc[i][j]);
/*Available Resource calculation*/
for(j=0;j<nr;j++)
{
avail[j]=r[j];
for(i=0;i<np;i++)
{
avail[j]-=alloc[i][j];


}
}


//marking processes with zero allocation


for(i=0;i<np;i++)
{
int count=0;
 for(j=0;j<nr;j++)
   {
      if(alloc[i][j]==0)
        count++;
      else
        break;
    }
 if(count==nr)
 mark[i]=1;
}
// initialize W with avail


for(j=0;j<nr; j++)
    w[j]=avail[j];


//mark processes with request less than or equal to W
for(i=0;i<np; i++)
{
int canbeprocessed= 0;
 if(mark[i]!=1)
{
   for(j=0;j<nr;j++)
    {
      if(request[i][j]<=w[j])
        canbeprocessed=1;
      else
         {
         canbeprocessed=0;
         break;
          }
     }
if(canbeprocessed)
{
mark[i]=1;


for(j=0;j<nr;j++)
w[j]+=alloc[i][j];
}
}
}


//checking for unmarked processes
int deadlock=0;
for(i=0;i<np;i++)
if(mark[i]!=1)
deadlock=1;




if(deadlock)
printf("\n Deadlock detected");
else
printf("\n No Deadlock possible");
}




____________________________________________________________


//Bankers Algorithm

#include<stdio.h>
#include<stdlib.h>
int n,m;
void calcneed(int all[n][m],int max[n][m],int need[n][m])
{
for(int i=0;i<n;i++)
{
for(int j=0;j<m;j++)
need[i][j] = max[i][j] - all[i][j];
}
}
void isdeadlock(int all[n][m],int need[n][m],int av[m])
{
int work[m];
int safeseq[n];
for(int i=0;i<m;i++)
work[i]=av[i];
int count=0;
int finish[n];
for(int i=0;i<n;i++)
finish[i]=0;
while(count!=n)
{
int found=0;
for(int i=0;i<n;i++)
{
if(!finish[i])
{
int canallocate=1;
for(int j=0;j<m;j++)
{
if(need[i][j]>work[j])
{
canallocate=0;
break;
}
}
if(canallocate)
{
for(int r=0;r<m;r++)
{
work[r]+=all[i][r];
}


safeseq[count++]=i;
finish[i]=1;
found=1;
}
}
}


if(!found)
{
printf("Deadlock Detected....");
exit(1);
}
}


printf("Safe Sequence exists...");
for(int i=0;i<n;i++)
{
printf("P%d ",safeseq[i]);
}
}


















int main()
{
printf("Enter number of processes: ");


scanf("%d",&n);
printf("Enter number of resources: ");


scanf("%d",&m);
//n-rows,m-columns
int av[m];
int all[n][m];
int max[n][m];
printf("Enter available matrix: ");
for(int i=0;i<m;i++)
{
scanf("%d",&av[i]);
}
printf("Enter allocation matrix: ");
for(int i=0;i<n;i++)
{
for(int j=0;j<m;j++)
{
printf("Enter allocation[%d%d] : ",i+1,j+1);
scanf("%d",&all[i][j]);
}
}
for(int i=0;i<n;i++)
{
for(int j=0;j<m;j++)
{
printf("Enter max[%d%d] : ",i+1,j+1);
scanf("%d",&max[i][j]);
}
}
int need[n][m];
calcneed(all,max,need);
printf("NEED MATRIX\n\n");
for(int i=0;i<n;i++)
{
for(int j=0;j<m;j++)
{
printf("%d ",need[i][j]);
}
printf("\n");
}
isdeadlock(all,need,av);
}


____________________________________________________________


//paging replacement techniques

#include<stdio.h>
int n,nf;
int in[100];
int p[50];
int hit=0;
int i,j,k;
int pgfaultcnt=0;

void getData()
{
    printf("\nEnter length of page reference sequence:");
    scanf("%d",&n);
    printf("\nEnter the page reference sequence:");
    for(i=0; i<n; i++)
        scanf("%d",&in[i]);
    printf("\nEnter no of frames:");
    scanf("%d",&nf);
}

void initialize()
{
    pgfaultcnt=0;
    for(i=0; i<nf; i++)
        p[i]=9999;
}

int isHit(int data)
{
    hit=0;
    for(j=0; j<nf; j++)
    {
        if(p[j]==data)
        {
            hit=1;
            break;
        }

    }

    return hit;
}

int getHitIndex(int data)
{
    int hitind;
    for(k=0; k<nf; k++)
    {
        if(p[k]==data)
        {
            hitind=k;
            break;
        }
    }
    return hitind;
}

void dispPages()
{
    for (k=0; k<nf; k++)
    {
        if(p[k]!=9999)
            printf(" %d",p[k]);
    }

}

void dispPgFaultCnt()
{
    printf("\nTotal no of page faults:%d",pgfaultcnt);
}

void fifo()
{
    initialize();
    for(i=0; i<n; i++)
    {
        printf("\nFor %d :",in[i]);

        if(isHit(in[i])==0)
        {

            for(k=0; k<nf-1; k++)
                p[k]=p[k+1];

            p[k]=in[i];
            pgfaultcnt++;
            dispPages();
        }
        else
            printf("No page fault");
    }
    dispPgFaultCnt();
}


void optimal()
{
    initialize();
    int near[50];
    for(i=0; i<n; i++)
    {

        printf("\nFor %d :",in[i]);

        if(isHit(in[i])==0)
        {

            for(j=0; j<nf; j++)
            {
                int pg=p[j];
                int found=0;
                for(k=i; k<n; k++)
                {
                    if(pg==in[k])
                    {
                        near[j]=k;
                        found=1;
                        break;
                    }
                    else
                        found=0;
                }
                if(!found)
                    near[j]=9999;
            }
            int max=-9999;
            int repindex;
            for(j=0; j<nf; j++)
            {
                if(near[j]>max)
                {
                    max=near[j];
                    repindex=j;
                }
            }
            p[repindex]=in[i];
            pgfaultcnt++;

            dispPages();
        }
        else
            printf("No page fault");
    }
    dispPgFaultCnt();
}

void lru()
{
    initialize();

    int least[50];
    for(i=0; i<n; i++)
    {

        printf("\nFor %d :",in[i]);

        if(isHit(in[i])==0)
        {

            for(j=0; j<nf; j++)
            {
                int pg=p[j];
                int found=0;
                for(k=i-1; k>=0; k--)
                {
                    if(pg==in[k])
                    {
                        least[j]=k;
                        found=1;
                        break;
                    }
                    else
                        found=0;
                }
                if(!found)
                    least[j]=-9999;
            }
            int min=9999;
            int repindex;
            for(j=0; j<nf; j++)
            {
                if(least[j]<min)
                {
                    min=least[j];
                    repindex=j;
                }
            }
            p[repindex]=in[i];
            pgfaultcnt++;

            dispPages();
        }
        else
            printf("No page fault!");
    }
    dispPgFaultCnt();
}

void lfu()
{
    int usedcnt[100];
    int least,repin,sofarcnt=0,bn;
    initialize();
    for(i=0; i<nf; i++)
        usedcnt[i]=0;

    for(i=0; i<n; i++)
    {

        printf("\n For %d :",in[i]);
        if(isHit(in[i]))
        {
            int hitind=getHitIndex(in[i]);
            usedcnt[hitind]++;
            printf("No page fault!");
        }
        else
        {
            pgfaultcnt++;
            if(bn<nf)
            {
                p[bn]=in[i];
                usedcnt[bn]=usedcnt[bn]+1;
                bn++;
            }
            else
            {
                least=9999;
                for(k=0; k<nf; k++)
                    if(usedcnt[k]<least)
                    {
                        least=usedcnt[k];
                        repin=k;
                    }
                p[repin]=in[i];
                sofarcnt=0;
                for(k=0; k<=i; k++)
                    if(in[i]==in[k])
                        sofarcnt=sofarcnt+1;
                usedcnt[repin]=sofarcnt;
            }

            dispPages();
        }

    }
    dispPgFaultCnt();
}

void secondchance()
{
    int usedbit[50];
    int victimptr=0;
    initialize();
    for(i=0; i<nf; i++)
        usedbit[i]=0;
    for(i=0; i<n; i++)
    {
        printf("\nFor %d:",in[i]);
        if(isHit(in[i]))
        {
            printf("No page fault!");
            int hitindex=getHitIndex(in[i]);
            if(usedbit[hitindex]==0)
                usedbit[hitindex]=1;
        }
        else
        {
            pgfaultcnt++;
            if(usedbit[victimptr]==1)
            {
                do
                {
                    usedbit[victimptr]=0;
                    victimptr++;
                    if(victimptr==nf)
                        victimptr=0;
                }
                while(usedbit[victimptr]!=0);
            }
            if(usedbit[victimptr]==0)
            {
                p[victimptr]=in[i];
                usedbit[victimptr]=1;
                victimptr++;
            }
            dispPages();

        }
        if(victimptr==nf)
            victimptr=0;
    }
    dispPgFaultCnt();
}

int main()
{
    int choice;
    while(1)
    {
        printf("\nPage Replacement Algorithms\n1.Enter data\n2.FIFO\n3.Optimal\n4.LRU\n5.LFU\n6.Second Chance\n7.Exit\nEnter your choice:");
        scanf("%d",&choice);
        switch(choice)
        {
        case 1:
            getData();
            break;
        case 2:
            fifo();
            break;
        case 3:
            optimal();
            break;
        case 4:
            lru();
            break;
        case 5:
            lfu();
            break;
        case 6:
            secondchance();
            break;
        default:
            return 0;
            break;
        }
    }
}


____________________________________________________________



//paging adress translation

#include<stdio.h>

int main()
{
 int ms, ps, nop, np, rempages, i, j, x, y, pa, offset;
 int s[10], fno[10][20];



printf("\nEnter the memory size -- ");
scanf("%d",&ms);

printf("\nEnter the page size -- ");
scanf("%d",&ps);

nop = ms/ps;
printf("\nThe no. of pages available in memory are -- %d ",nop);

printf("\nEnter number of processes -- ");
 scanf("%d",&np);
rempages = nop;
for(i=1;i<=np;i++)

{

printf("\nEnter no. of pages required for p[%d]-- ",i);
 scanf("%d",&s[i]);

if(s[i] >rempages)
{

printf("\nMemory is Full");
break;
}
rempages = rempages - s[i];

printf("\nEnter pagetable for p[%d] --- ",i);
 for(j=0;j<s[i];j++)
scanf("%d",&fno[i][j]);
}

printf("\nEnter Logical Address to find Physical Address ");
printf("\nEnter process no. and pagenumber and offset -- ");

scanf("%d %d %d",&x,&y, &offset);



if(x>np || y>=s[i] || offset>=ps)
printf("\nInvalid Process or Page Number or offset");

else
{ pa=fno[x][y]*ps+offset;
printf("\nThe Physical Address is -- %d",pa);

}
return 0;
}
