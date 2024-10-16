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




Pthread


Version 1


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


// Define the thread data structure
typedef struct {
    int *array;
    int start;
    int end;
    int search;
    int *count;
    pthread_mutex_t *mutex;  // Mutex to synchronize count updates
} ThreadData;


// Thread function to search for occurrences of 'search' in a segment of the array
void* search_scores(void* arg) {
    ThreadData *data = (ThreadData*) arg;
    int local_count = 0;


    for (int i = data->start; i < data->end; i++) {
        if (data->array[i] == data->search) {
            local_count++;
        }
    }


    // Lock the mutex before updating the shared count
    pthread_mutex_lock(data->mutex);
    *(data->count) += local_count;
    pthread_mutex_unlock(data->mutex);


    return NULL;
}


int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <file> <S> <threads> <segment_size>\n", argv[0]);
        return EXIT_FAILURE;
    }


    const char *filename = argv[1];
    int search_value = atoi(argv[2]);
    int num_threads = atoi(argv[3]);
    int segment_size = atoi(argv[4]);


    // Open the file
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("fopen");
        return EXIT_FAILURE;
    }


    // Read scores into an array
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);


    int num_scores = file_size / sizeof(int);
    int *scores = malloc(file_size);
    fread(scores, sizeof(int), num_scores, file);
    fclose(file);


    // Create threads and perform search
    pthread_t threads[num_threads];
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    int count = 0;


    // Create threads
    for (int i = 0; i < num_threads; i++) {
        ThreadData *data = malloc(sizeof(ThreadData));
        data->array = scores;
        data->start = i * segment_size;
        data->end = (i + 1) * segment_size;
        if (data->end > num_scores) data->end = num_scores; // Handle last segment
        data->search = search_value;
        data->count = &count;
        data->mutex = &mutex;


        pthread_create(&threads[i], NULL, search_scores, data);
    }


    // Join threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }


    // Print result
    printf("Number of students who scored %d: %d\n", search_value, count);


    // Clean up
    pthread_mutex_destroy(&mutex);
    free(scores);


    return EXIT_SUCCESS;
}




Version 2


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#define MAX_SCORE 100


typedef struct {
    int *scores;
    int start;
    int end;
    int target;
    int count;
} ThreadData;


void *search_score(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    int count = 0;
    for (int i = data->start; i < data->end; i++) {
        if (data->scores[i] == data->target) {
            count++;
        }
    }
    data->count = count;
    return NULL;
}


int main() {
    int N, M, S;
    printf("Enter the number of students (N): ");
    scanf("%d", &N);
    printf("Enter the number of threads (M): ");
    scanf("%d", &M);
    printf("Enter the score to search for (S): ");
    scanf("%d", &S);


    // Allocate memory for student scores
    int *scores = (int *)malloc(N * sizeof(int));
    if (scores == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }


    // Generate random scores for students
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        scores[i] = rand() % (MAX_SCORE + 1);
    }


    // Divide work among threads
    pthread_t threads[M];
    ThreadData thread_data[M];
    int segment_size = N / M;


    for (int i = 0; i < M; i++) {
        thread_data[i].scores = scores;
        thread_data[i].start = i * segment_size;
        thread_data[i].end = (i == M - 1) ? N : (i + 1) * segment_size;
        thread_data[i].target = S;
        thread_data[i].count = 0;


        pthread_create(&threads[i], NULL, search_score, &thread_data[i]);
    }


    // Aggregate results from all threads
    int total_count = 0;
    for (int i = 0; i < M; i++) {
        pthread_join(threads[i], NULL);
        total_count += thread_data[i].count;
    }


    printf("Number of students who scored %d: %d\n", S, total_count);


    // Free allocated memory
    free(scores);


    return 0;
}














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












Producer Consumer








server.c


#include <stdio.h>
#include <stdlib.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/ipc.h>


int main() {
    key_t k1, k2, k3, k4;
    k1 = 9999;
    k3 = 9997; 
    k4 = 9996; 


    int mutexid, inid, outid;
    if((mutexid = shmget(k1, sizeof(int), 0666 | IPC_CREAT)) == -1) {
        perror("SHMGET");
        exit(1);
    }
    
    if((inid = shmget(k3, sizeof(int), 0666 | IPC_CREAT)) == -1) {
        perror("SHMGET");
        exit(1);
    }
    if((outid = shmget(k4, sizeof(int), 0666 | IPC_CREAT)) == -1) {
        perror("SHMGET");
        exit(1);
    }


    int *mutex, *data, *in, *out;


    // Attach shared memory segments
    if((mutex = shmat(mutexid, (void *)NULL, 0)) == (void *)-1) {
        perror("SHMAT");
        exit(1);
    }
    if((in = shmat(inid, (void *)NULL, 0)) == (void *)-1) {
        perror("SHMAT");
        exit(1);
    }
    if((out = shmat(outid, (void *)NULL, 0)) == (void *)-1) {
        perror("SHMAT");
        exit(1);
    }
    *in =0;
    *out=0;
    *mutex=0;




    int c;
    do {
        printf("\nWhich one do you want...\n1. Producer\n2. Consumer\n3. Exit\nEnter your choice: ");
        scanf("%d", &c);
        if (c == 1) {
            system("gnome-terminal -- ./pro");
        } else if (c == 2) {
            system("gnome-terminal -- ./con");
        } else if (c != 3) {
            printf("\nInvalid choice\n");
        }
    } while (c != 3);
    return 0;
}






producer.c


#include<stdio.h>
#include<stdlib.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/ipc.h>


#define BUFFER_SIZE 5


int main(){
    key_t k1, k2, k3, k4;
    k1 = 9999;
    k2 = 9998;
    k3 = 9997; 
    k4 = 9996; 


    int mutexid, dataid, inid, outid;
    if((mutexid = shmget(k1, sizeof(int), 0666 | IPC_CREAT)) == -1) {
        perror("SHMGET");
        exit(1);
    }
    if((dataid = shmget(k2, BUFFER_SIZE * sizeof(int), 0666 | IPC_CREAT)) == -1) {
        perror("SHMGET");
        exit(1);
    }
    if((inid = shmget(k3, sizeof(int), 0666 | IPC_CREAT)) == -1) {
        perror("SHMGET");
        exit(1);
    }
    if((outid = shmget(k4, sizeof(int), 0666 | IPC_CREAT)) == -1) {
        perror("SHMGET");
        exit(1);
    }


    int *mutex, *data, *in, *out;


    // Attach shared memory segments
    if((mutex = shmat(mutexid, (void *)NULL, 0)) == (void *)-1) {
        perror("SHMAT");
        exit(1);
    }
    if((data = shmat(dataid, (void *)NULL, 0)) == (void *)-1) {
        perror("SHMAT");
        exit(1);
    }
    if((in = shmat(inid, (void *)NULL, 0)) == (void *)-1) {
        perror("SHMAT");
        exit(1);
    }
    if((out = shmat(outid, (void *)NULL, 0)) == (void *)-1) {
        perror("SHMAT");
        exit(1);
    }


    int choice;
    do {
        if ((*in + 1) % BUFFER_SIZE != *out) { 
            if (*mutex == 0) {
                *mutex = 1;
                
                int temp;
                printf("Enter a number to produce: ");
                scanf("%d", &temp);
                data[*in] = temp; 
                printf("buffer[%d] = %d\n", *in, temp);
                *in = (*in + 1) % BUFFER_SIZE; 
                *mutex = 0; 
            } else {
                printf("Other Process is in use! Try again.\n");
            }
        } else {
            printf("Buffer is full! Cannot produce more.\n");
        }


        printf("Do you want to continue producing? (1.Yes 2.No): ");
        scanf("%d", &choice);
    } while (choice != 2);


    return 0;
}




consumer.c




#include<stdio.h>
#include<stdlib.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/ipc.h>


#define BUFFER_SIZE 5


int main() {
    key_t k1, k2, k3, k4;
    k1 = 9999;
    k2 = 9998;
    k3 = 9997;
    k4 = 9996;


    int mutexid, dataid, inid, outid;


    // Shared memory allocation
    if((mutexid = shmget(k1, sizeof(int), 0666 | IPC_CREAT)) == -1) {
        perror("SHMGET");
        exit(1);
    }
    if((dataid = shmget(k2, BUFFER_SIZE * sizeof(int), 0666 | IPC_CREAT)) == -1) {
        perror("SHMGET");
        exit(1);
    }
    if((inid = shmget(k3, sizeof(int), 0666 | IPC_CREAT)) == -1) {
        perror("SHMGET");
        exit(1);
    }
    if((outid = shmget(k4, sizeof(int), 0666 | IPC_CREAT)) == -1) {
        perror("SHMGET");
        exit(1);
    }


    int *mutex, *data, *in, *out;


    // Attach shared memory segments
    if((mutex = shmat(mutexid, (void *)NULL, 0)) == (void *)-1) {
        perror("SHMAT");
        exit(1);
    }
    if((data = shmat(dataid, (void *)NULL, 0)) == (void *)-1) {
        perror("SHMAT");
        exit(1);
    }
    if((in = shmat(inid, (void *)NULL, 0)) == (void *)-1) {
        perror("SHMAT");
        exit(1);
    }
    if((out = shmat(outid, (void *)NULL, 0)) == (void *)-1) {
        perror("SHMAT");
        exit(1);
    }


    int choice;
    do {
        if (*in != *out) {
            if (*mutex == 0) {
                *mutex = 1;  
                printf("Buffer before consuming:\n");
                int i = *out;
                while (i != *in) {
                    printf("buffer[%d] = %d\n", i, data[i]);
                    i = (i + 1) % BUFFER_SIZE;
                }


                printf("\nConsuming: buffer[%d] = %d\n", *out, data[*out]);
                *out = (*out + 1) % BUFFER_SIZE;  
                printf("\nBuffer after consuming:\n");
                i = *out;
                if (*in == *out) {
                    printf("Buffer is empty!\n");
                } else {
                    while (i != *in) {
                        printf("buffer[%d] = %d\n", i, data[i]);
                        i = (i + 1) % BUFFER_SIZE;
                    }
                }


                 
                *mutex = 0; 
            } else {
                printf("Other process is in use! Try again.\n");
            }
        } else {
            printf("Buffer is empty! Cannot consume.\n");
        }


        printf("\nDo you want to continue consuming? (1.Yes 2.No): ");
        scanf("%d", &choice);
    } while (choice != 2);


    return 0;
}










reader-writer




#include<stdio.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<string.h>
#include<unistd.h>
#define SHMSZ 1024
int main()
{
key_t key1 , key2;
int shmid;
int sec_id;
char *shm , *k , *s;//Data
char *ssm, *j , *t;
key2 = 3400; //Pattern
key1 = 3415;//Data
if((shmid = shmget(key1, SHMSZ, IPC_CREAT | 0666))<0)
{
perror("shmget");
exit(1);
}


if((shm = shmat(shmid,NULL,0)) == (char *)-1)
{
perror("shmat");
exit(1);
}
//***************************************************************
if((sec_id = shmget(key2, SHMSZ, IPC_CREAT | 0666))<0)
{
perror("shmget");
exit(1);
}
if((ssm = shmat(sec_id,NULL,0)) == (char *)-1)
{
perror("shmat");
exit(1);
}
t = ssm;
char pattern[10];
printf("\nEnter the pattern: ");
fgets(pattern,10, stdin); //Will accept till ENTER key is hit...
int len = sizeof(pattern);
for(int i = 0; i < len; i++)
{
*t = pattern[i];
*t++;
}
for (k = ssm; *k != '\0'; k++)
printf("%c",*k);
putchar('\n');


//Reader Priority
int choice;
printf("1. READER PRIORITY \n 2. WRITER PRIORITY\n");
printf("\nEnter the priority (1/2): ");
scanf("%d",&choice);
if(choice == 1)
{
char first = *ssm;
printf("\nThe first character is: %c",first);
if(first == 'R')
{
int rcount = 0;
int wcount = 0;
for (k = ssm; *k != '\0'; k++)
{
if(*k == 'R')
rcount = rcount + 1;
else if(*k == 'W')
wcount = wcount + 1;


}
printf("\nThe no. of readers is: %d",rcount);
printf("\nThe no. of writer is: %d\n",wcount);
for(int i = 0; i < rcount; i++)
{
system("gnome-terminal -- ./r.out");
sleep(2);
}


for(int i = 0; i < wcount; i++)
{
system("gnome-terminal -- ./w.out");
sleep(10);
}
}
else
{
char* h;
h = ssm;
do
{
system("gnome-terminal -- ./w.out");
sleep(10);
h++;
}while(*h != 'R');
int rcount = 0;
int wcount = 0;
for (k = h; *k != '\0'; k++)
{
if(*k == 'R')
rcount = rcount + 1;
else if(*k == 'W')
wcount = wcount + 1;


}
printf("\nThe no. of readers is: %d",rcount);
printf("\nThe no. of writer is: %d\n",wcount);
for(int i = 0; i < rcount; i++)
{
system("gnome-terminal -- ./r.out");


sleep(2);
}
for(int i = 0; i < wcount; i++)
{
system("gnome-terminal -- ./w.out");
sleep(10);
}
}
}
else if(choice == 2)
{
char first = *ssm;
printf("\nThe first character is: %c",first);
if(first == 'W')
{
int rcount = 0;
int wcount = 0;
for (k = ssm; *k != '\0'; k++)
{
if(*k == 'R')
rcount = rcount + 1;
else if(*k == 'W')
wcount = wcount + 1;


}
printf("\nThe no. of readers is: %d",rcount);
printf("\nThe no. of writer is: %d\n",wcount);
for(int i = 0; i < wcount; i++)
{
system("gnome-terminal -- ./w.out");
sleep(10);


}
for(int i = 0; i < rcount; i++)
{
system("gnome-terminal -- ./r.out");
sleep(2);
}
}
else
{
char* h;
h = ssm;
do
{
system("gnome-terminal -- ./r.out");
sleep(2);
h++;
}while(*h != 'W');
int rcount = 0;
int wcount = 0;
for (k = h; *k != '\0'; k++)
{
if(*k == 'R')
rcount = rcount + 1;
else if(*k == 'W')
wcount = wcount + 1;


}
printf("\nThe no. of readers is: %d",rcount);
printf("\nThe no. of writer is: %d\n",wcount);
sleep(3);


for(int i = 0; i < wcount; i++)
{
system("gnome-terminal -- ./w.out");
sleep(10);
}
for(int i = 0; i < rcount; i++)
{
system("gnome-terminal -- ./r.out");
sleep(2);
}
}
}
return 0;
}


Writer with Priority
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include <sys/shm.h>
#include<stdbool.h>
#define SHMSZ 1024
#define MAX_LIMIT 1024
int main()
{
int shmid;


key_t key;
key = 3415;
char *shm , *s , *k;
if ((shmid = shmget(key, SHMSZ, 0666))<0)
{
perror("shmget");
exit(1);
}
if ((shm = shmat(shmid, NULL, 0)) == (char *) -1)
{
perror("shmat");
exit(1);
}
s = shm;
printf("\n%d is the only Write process that is accessing the Shared
Memory...",getpid());
bool temp = true;
while(true)
{
//printf("\nInside the While Loop!!!\n");
if(*s != '\0')
++s;
else if(*s == '\0')
break;


}
char x = *s;


printf("\nThe Data here should be null: %c",x);
char str[MAX_LIMIT];
int choice;
do
{
printf("\nEnter the String to be entered into the Shared Memory: ");
fgets(str, MAX_LIMIT, stdin); //Will accept till ENTER key is hit...
printf("\nAre you sure the string you want to enter is: %s ",str);
printf("\nIf YES -> 1 | NO -> 0: ");
scanf("%d",&choice);
}while(choice != 1);
printf("The String that will be entered into the SHM is: %s\n", str);
for(int i = 0 ; i < sizeof(str) ; i++)
{
*s++ = str[i]; //After the last mem. location, s will point to NULL...
}
sleep(5);
printf("\nThe Write process for %d Process is complete!!!\n",getpid());
for (k = s; *k != '\0'; k++)
{
printf("%c",*k);
}
putchar('\n');
return 0;
}


Reader with Priority
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include <sys/shm.h>
#define SHMSZ 1024
#define MAX_LIMIT 1024
int main()
{
int shmid;
key_t key;
key = 3415;
char *shm , *s , *k;
if ((shmid = shmget(key, SHMSZ, 0666))<0)
{
perror("shmget");
exit(1);
}
if ((shm = shmat(shmid, NULL, 0)) == (char *) -1)
{
perror("shmat");
exit(1);
}
char flag = *shm;
s = shm;
printf("\nThe data at the first location of the Shared Memory is: %c\n",flag);


for (k = shm; *k != '\0'; k++){
printf("%c",*k);
}
sleep(5);
printf("\nThe Read process of the Reader is complete!!!\n");
return 0;
}












deadlock


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

_______________________________________________________________________________________________________________________

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



______________________________________________________________________________________________________________________________


dining




#include <stdio.h> 
#include <unistd.h> 
#include <semaphore.h> 
#include <stdlib.h> 
#include <pthread.h> 
#define N 5 
#define LEFT (i + N - 1) % N 
#define RIGHT (i + 1) % N 
#define THINKING 0 
#define HUNGRY 1 
#define EATING 2 
int state[N]; 
sem_t mutex; 
sem_t s[N]; 
void test(int i) { 
if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) { 
state[i] = EATING; 
sem_post(&s[i]); 
} 
} 
void take(int i) { 
sem_wait(&mutex); 
state[i] = HUNGRY; 
printf("\nPhilosopher %d is hungry", i); 
test(i); 
sem_post(&mutex); 
sem_wait(&s[i]); 
} 
void put(int i) { 
sem_wait(&mutex); 
state[i] = THINKING; 
printf("\nPhilosopher %d is putting down forks", i); 
test(LEFT); 
test(RIGHT); 
sem_post(&mutex); 
} 
void* phil(void* i) { 
int* p = (int*)i; 
while (1) { 
printf("\nPhilosopher %d is thinking", *p); 
sleep(2); 
take(*p); 
printf("\nPhilosopher %d is eating", *p); 
sleep(2); 
put(*p); 
} 
} 
int main() { 
pthread_t philosophers[N]; 
sem_init(&mutex, 0, 1); 
int p[N]; 
for (int i = 0; i < N; i++) { 
p[i] = i; 
state[i] = THINKING; // Initialize state 
sem_init(&s[i], 0, 0); // Initialize semaphores 
} 
// Create philosopher threads 
for (int i = 0; i < N; i++) { 
pthread_create(&philosophers[i], NULL, phil, (void*)&p[i]); 
} 
// Wait for all philosopher threads (though they run indefinitely) 
for (int i = 0; i < N; i++) { 
pthread_join(philosophers[i], NULL); 
    } 
 
    return 0; 
}


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
