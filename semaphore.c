#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"

#define SEMNUM 32

struct semaphore {
  int value;
  int active;
  struct spinlock lock;
} sem[SEMNUM];

void
seminit(void)
{
  for(int i = 0; i < SEMNUM; ++i) {
    initlock(&sem[i].lock, "semaphore");
    sem[i].active = sem[i].value = 0;
  }
}

int
sem_init(int num, int max)
{
  if(num < 0 || num >= SEMNUM) return -1;

  acquire(&sem[num].lock);
  if(sem[num].active != 0) return -1;
  sem[num].value = max;
  sem[num].active = 1;
  release(&sem[num].lock);

  return 0;
}

int
sem_destroy(int num)
{
  if(num < 0 || num >= SEMNUM) return -1;

  acquire(&sem[num].lock);
  if(sem[num].active != 1) return -1;
  sem[num].active = 0;
  release(&sem[num].lock);

  return 0;
}

int
sem_wait(int num, int count)
{
  if(num < 0 || num >= SEMNUM) return -1;

  acquire(&sem[num].lock);
  if(!sem[num].active) return -1;
  while(sem[num].value <= count - 1) {
    sleep(&sem[num], &sem[num].lock);
  }
  sem[num].value -= count;
  release(&sem[num].lock);

  return 0;
}

int
sem_signal(int num, int count)
{
  if(num < 0 || num >= SEMNUM) return -1;

  acquire(&sem[num].lock);
  if(!sem[num].active) return -1;
  sem[num].value += count;
  if(sem[num].value > 0){
    wakeup(&sem[num]);
  }
  release(&sem[num].lock);

  return 0;
}
