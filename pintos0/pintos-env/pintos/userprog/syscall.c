#include "userprog/syscall.h"
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/palloc.h"
#include "userprog/process.h"
#include "devices/shutdown.h"

static void syscall_handler (struct intr_frame *);

typedef void (*handler) (struct intr_frame *);
static void syscall_exit (struct intr_frame *f);
static void syscall_write (struct intr_frame *f);
static void syscall_wait (struct intr_frame *f);
static void syscall_exec (struct intr_frame *f);

#define SYSCALL_MAX_CODE 19
static handler call[SYSCALL_MAX_CODE + 1];

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");

  /* Any syscall not registered here should be NULL (0) in the call array. */
  memset(call, 0, SYSCALL_MAX_CODE + 1);

  /* Check file lib/syscall-nr.h for all the syscall codes and file
   * lib/user/syscall.c for a short explanation of each system call. */
  call[SYS_EXIT]  = syscall_exit;   // Terminate this process.
  call[SYS_WRITE] = syscall_write;  // Write to a file.

  // Add syscall handler for syscall WAIT 
  call[SYS_WAIT] = syscall_wait;
  // Add syscall handler for syscall EXEC
  call[SYS_EXEC] = syscall_exec;
}

static void
syscall_handler (struct intr_frame *f)
{
  // syscall_code is the int to which the stack pointer is pointing to 
  // e.g. SYS_WRITE, SYS_EXIT
  int syscall_code = *((int*)f->esp);
  // call is an array of "struct handler" 
  // intr_frame = interrupt frame 
  // f is a pointer to a intr_frame 
  // call[syscall_code] returns pointer to function
  // f is the paramiter of the function 
  // basically this is a call to the corrisponding syscall handler with 
  // parameter == to f. 
  call[syscall_code](f);
}

static void
syscall_exit (struct intr_frame *f)
{
  int *stack = f->esp;
  struct thread* t = thread_current ();
  t->exit_status = *(stack+1);
  thread_exit ();
}

static void
syscall_write (struct intr_frame *f)
{
  int *stack = f->esp;
  ASSERT (*(stack+1) == 1); // fd 1
  char * buffer = *(stack+2);
  int    length = *(stack+3);
  putbuf (buffer, length);
  f->eax = length;
}

// WAIT CALL: 
/* Handling function */
static void 
syscall_wait(struct intr_frame *f){
  // implement 
  int *stack = f->esp;
  int pid = *(stack+1); //pid_t == int 
  // TODO : think 
  // should find tid based on pid ?? 
  f->eax = process_wait(pid);
}

// EXEC CALL:
/* Handling function */
static void 
syscall_exec(struct intr_frame *f){
  //implement
}
