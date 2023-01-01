#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

using namespace std;


#define BUFFER_SIZE 16
#define NUM_OF_TEST_CASE 4

typedef struct {
	int buf[BUFFER_SIZE];
	int buff_length;
}BUFFER_t;


BUFFER_t Buffer = {0};
char storage[BUFFER_SIZE]; 
int count = 0; 

pthread_mutex_t buffer_mutex;
pthread_mutex_t storage_mutex; 

sem_t sem_empty_buffer;
sem_t sem_filled_buffer;
sem_t sem_data_available; 
sem_t sem_data_processed; 


int validate_sequence(char* p_storage){
    int valid_data = 0; 
    for(int i = 1; i<BUFFER_SIZE; i++){
       
    if((p_storage[i] == '\x5A') && (p_storage[i-1] == '\xA5')){
        valid_data++; 
        }
    }
    return valid_data; 
}

void *transmitter(void *arg) {
        unsigned char* p_data = (unsigned char*)arg; 
		sem_wait(&sem_empty_buffer);
		pthread_mutex_lock(&buffer_mutex);
		for(int i = 0; i<BUFFER_SIZE;i++){
		    Buffer.buf[i] = p_data[i]; 
		    Buffer.buff_length++; 
		}
		pthread_mutex_unlock(&buffer_mutex);
		sem_post(&sem_filled_buffer);
		
	    return NULL;
}

void *receiver(void *arg) {
        for(int i = 0; i<NUM_OF_TEST_CASE; i++){
		sem_wait(&sem_filled_buffer);
		pthread_mutex_lock(&buffer_mutex);
		sem_wait(&sem_data_processed); 
        pthread_mutex_lock(&storage_mutex);
	   	for(int i = 0; i<BUFFER_SIZE;i++){
		    storage[i] = Buffer.buf[i]; 
		    Buffer.buff_length--; 
		}
		pthread_mutex_unlock(&storage_mutex);
		pthread_mutex_unlock(&buffer_mutex); 
		sem_post(&sem_data_available);
		sem_post(&sem_empty_buffer);
        }
	    return NULL;
}

void* background_data_processor(void* arg){
  
    for(int i = 0; i<NUM_OF_TEST_CASE; i++){
    sem_wait(&sem_data_available); 
    pthread_mutex_lock(&storage_mutex); 
    count = validate_sequence(storage);
    cout<<"Number of valid sequence:" << count << endl ; 
    pthread_mutex_unlock(&storage_mutex);
    sem_post(&sem_data_processed);
     }
         return NULL;
}

int main()
{
	pthread_t thread_receiver_id;
	pthread_t thread_dataprocessor_id;
	pthread_t thread_test_case_id[NUM_OF_TEST_CASE]; 

	pthread_mutex_init(&buffer_mutex, NULL);
	pthread_mutex_init(&storage_mutex, NULL);
    sem_init(&sem_empty_buffer, 0, 1);
    sem_init(&sem_filled_buffer, 0, 0);
    sem_init(&sem_data_available, 0, 0);
    sem_init(&sem_data_processed, 0, 1);

    //Test data
    unsigned char data[NUM_OF_TEST_CASE][16]
                    { {0xA5, 0x5A, 0xFF, 0xD8, 0xA5, 0x5A, 0x5A, 0xA5, 0x5A, 0x66, 0x78, 0x12, 0x56, 0x89, 0x5C, 0x66}, //3 valid sequences
                      {0xA5, 0x5A, 0xFF, 0xD8, 0x88, 0x90, 0x5A, 0x65, 0x5C, 0x66, 0x78, 0x12, 0x56, 0x89, 0x5C, 0x66}, //1 valid sequence
                      {0x00, 0x78, 0xFF, 0xD8, 0xA5, 0xCD, 0x5A, 0xA5, 0x89, 0x66, 0x78, 0x12, 0x56, 0x89, 0x5C, 0x66}, //0 valid sequence
                      {0x00, 0xA0, 0x0F, 0x08, 0x05, 0x50, 0x50, 0x05, 0x50, 0x66, 0x78, 0x12, 0x56, 0x00, 0x0C, 0x06}  //0 valid sequence
                    };
                    
                    
    //Creating Receiver thread
	pthread_create(&thread_receiver_id, NULL, receiver, NULL);
	
	//Creating Data Processor thread
	pthread_create(&thread_dataprocessor_id, NULL, background_data_processor, NULL);
	
	//Creating Transmitter threads 
	for(int i = 0; i<NUM_OF_TEST_CASE; i++){
	pthread_create(&thread_test_case_id[0], NULL, transmitter, &data[i][0]);
   }

	//Joining all threads
    pthread_join(thread_receiver_id,NULL); 
    pthread_join(thread_dataprocessor_id,NULL); 
    for(int i = 0; i<NUM_OF_TEST_CASE; i++){
        pthread_join(thread_test_case_id[0],NULL); 
     }

	pthread_mutex_destroy(&buffer_mutex);
	pthread_mutex_destroy(&storage_mutex);
	
	sem_destroy(&sem_empty_buffer);
	sem_destroy(&sem_filled_buffer);
	sem_destroy(&sem_data_available);
	sem_destroy(&sem_data_processed);
	return 0;
}