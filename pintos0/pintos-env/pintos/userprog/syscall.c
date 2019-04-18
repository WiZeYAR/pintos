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

/*
    Assignment 4) Handling system calls 
    Need to handle all the system calls
    1) Pop sys_call number 
    2) Check which enum -> type of system call -> For each system call type there is a specifi handling 
    In this assignment handle exit() and printf() system calls 
    !! intr_frame * f --> has info about stack which is f->eps 
    !! when finish system call we need to save value into f-> eax
    !! print smtg putbuf() takes a buffer of char    
*/
static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  printf ("system call!\n");
  thread_exit ();
}
