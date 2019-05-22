#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"


static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}


// WHY DOES IT COMPLAIN ABOUT ABSENSE OF OUTPUT?
static void
syscall_handler (struct intr_frame * f) 
{
  //stack pointer 
  int * stackp = f->esp;
  //for each syscall f->esp (stack pointer) points to enum = syscall_value
  int syscall_value = *stackp; 

  //Depending on the syscall_value handle the syscall properly
  switch (syscall_value)
  {
    case SYS_WRITE:
      //Check lib/user/syscall.c function write()
      //args[0] : syscall_value, args[1] : fd = file descriptor, args[2] : buffer
      //args[3] : size
      //Need to check that fd = file descriptor is = 1 because 1 is reserved for stdout
      ASSERT(*(int *)(f->esp+1) == 1);
      char * buffer = *(char *)((f->esp) + 2);
      int len = *(int *)((f->esp) + 3);
      //Use function putbuf() to print to stdout
      putbuf(buffer, len);
      //Return len of input --> WHY??
      f->eax = len;
      break;
    case SYS_EXIT:
      //When calling exit syscall we need to save the exit_status of the thread in the 
      //thread field exit_status. It is initialized in thread_create() with dafault value -1.
      struct thread * t = thread_current();
      t->exit_status = *(int *)((f->esp)+1); 
      thread_exit();
      break;
  }
  
  
  // PREVIOUS CODE
  // printf ("system call!\n");
  // thread_exit ();
}
