#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"


#define BYTES_PER_STACK_ARG 4

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}


// WHY DOES IT COMPLAIN ABOUT ABSENSE OF OUTPUT?
static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  // ON THE ASSIGNMENT SLIDE WE HAVE SEEN THAT ARG IS 4B LONG, SO
  uint16_t syscall_value =  * (uint16_t *) f->esp;
  uint32_t * return_ptr  =  & (f->eax);

  switch (syscall_value)
  {
    case SYS_WRITE:
      // WE WERE NOT REALLY SURE ABOUT THE ARGUMENT TYPES (OR) SIZES
      // IT WOULD BE NICE, IF THIS IS TOLD IN THE TASK EXPLICITLY
      // NEXT TIME, SO WE DO NOT HAVE TO GUESS
      putbuf(
          (char*)    ((f->eax) + 2 * BYTES_PER_STACK_ARG), // A POINTER TO THE BUFFER WITH TEXT TO WRITE
          * (size_t*) ((f->eax) + 3 * BYTES_PER_STACK_ARG)   // A POINTER TO THE SIZE OF THE BUFFER
      );
      * return_ptr = 0; // ASSUMING THAT THE PUTBUFF ALWAYS SUCCEEDS
      break;
    case SYS_EXIT:
      * return_ptr = * (uint16_t *) (f->esp + 1 * BYTES_PER_STACK_ARG);
      thread_current()->proc_ret_code = *return_ptr;
      thread_current()->parent_waits = false;
      break;
  }
  thread_exit();
  
  
  // PREVIOUS CODE
  // printf ("system call!\n");
  // thread_exit ();
}
