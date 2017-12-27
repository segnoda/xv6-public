#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

#define NUM_THREADS 8
#define TARGET_COUNT_PER_THREAD 5000
#define SEMAPHORE_NUM 0

uint g_counter;

void counter(void *arg)
{
  for(int i = 0; i < TARGET_COUNT_PER_THREAD; i++){
    sem_wait(SEMAPHORE_NUM, 1);
    int temp = g_counter;
    temp = temp + 1;
    sleep(0);
    g_counter = temp;
    sem_signal(SEMAPHORE_NUM, 1);
  }

  exit();
}

int main(int argc, char **argv)
{
  int sem_size = 1;
  int final_target = NUM_THREADS * TARGET_COUNT_PER_THREAD;

  // Initialize semaphore to 1
  if(sem_init(SEMAPHORE_NUM, sem_size) < 0){
    printf(1, "main: error initializing semaphore %d\n", SEMAPHORE_NUM);
    exit();
  }

  printf(1, "main: initialized semaphore %d to %d\n", SEMAPHORE_NUM, sem_size);
 
  // Initialize counter
  g_counter = 0;

  // Set up thread stuff

  // Stacks
  void *stacks[NUM_THREADS];
  // Args
  int *args[NUM_THREADS];

  // Allocate stacks and args and make sure we have them all
  // Bail if something fails
  for(int i = 0; i < NUM_THREADS; i++){
    stacks[i] = (void*) malloc(4096);
    if(!stacks[i]){
      printf(1, "main: could not get stack for thread %d, exiting...\n");
      exit();
    }

    args[i] = (int*) malloc(4);
    if(!args[i]){
      printf(1, "main: could not get memory (for arg) for thread %d, exiting...\n");
      exit();
    }

    *args[i] = i;
  }

  printf(1, "main: running with %d threads...\n", NUM_THREADS);

  // Start all children
  for(int i = 0; i < NUM_THREADS; i++){
    int pid = clone(counter, args[i], stacks[i]);
    printf(1, "main: created thread with pid %d\n", pid);
  }

  // Wait for all children

  for(int i = 0; i < NUM_THREADS; i++){
    void *joinstack;
    join(&joinstack);
  } 

  // Check the result
  printf(1, "g_counter is %d\n", g_counter);
  printf(1, "final_target is %d\n", final_target);
  if(g_counter == final_target)
    printf(1, "TEST PASSED!\n");
  else
    printf(1, "TEST FAILED!\n");

  // Clean up semaphore
  sem_destroy(SEMAPHORE_NUM);

  // Exit
  exit();
}
