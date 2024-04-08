#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "syscall.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_trace(void)
{
  int mask;
  argint(0, &mask);
  if(mask < 0) {
    return -1;
  }
  myproc()->mask = mask;
  return 0;
}

uint64 sys_pgaccess(void)
{
  uint64 va;
  int num;
  uint64 result;
  argaddr(0, &va);
  argint(1, &num);
  argaddr(2, &result);
  struct proc* p = myproc();
  if (p == 0) return 1;
  int ans = 0;
  pagetable_t pgtbl = p->pagetable;
  for(int idx = 0; idx < num; idx++)
  {
    pte_t *pte = walk(pgtbl, va+(uint64)(idx*PGSIZE), 0);
    if(pte != 0 && *pte & PTE_A){
      ans |= (1 << idx);
      *pte ^= PTE_A;
    }
  }
  return copyout(pgtbl, result, (char *)&ans, sizeof(int));
}