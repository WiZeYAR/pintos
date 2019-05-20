#include "userprog/syscall.h"
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/synch.h"
#include "threads/palloc.h"
// #include "threads/thread.h"
#include "userprog/process.h"
#include "devices/shutdown.h"
#include "lib/user/syscall.h"


static unsigned fd = 0;
static void syscall_handler (struct intr_frame *);
typedef void (*handler) (struct intr_frame *);
//Assignment 4
static void syscall_exit (struct intr_frame *f);
static void syscall_write (struct intr_frame *f);
//Assignment 5
static void syscall_wait (struct intr_frame *f);
static void syscall_exec (struct intr_frame *f);
// Assignment 6
static void syscall_create (struct intr_frame *f);
static void syscall_remove (struct intr_frame *f);
static void syscall_open (struct intr_frame *f);
static void syscall_close (struct intr_frame *f);
static void syscall_filesize (struct intr_frame *f);
static void syscall_read (struct intr_frame *f);
static void syscall_seek (struct intr_frame *f);
static void syscall_tell (struct intr_frame *f);
static void syscall_halt (struct intr_frame *f);

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

  //Assignment 5
  // Add syscall handler for syscall WAIT 
  call[SYS_WAIT] = syscall_wait;
  // Add syscall handler for syscall EXEC
  call[SYS_EXEC] = syscall_exec;

  //Assignment 6
  call[SYS_CREATE] = syscall_create;
  call[SYS_REMOVE] = syscall_remove;
  call[SYS_OPEN] = syscall_open;
  call[SYS_CLOSE] = syscall_close;
  call[SYS_FILESIZE] = syscall_filesize;
  call[SYS_READ] = syscall_read;
  call[SYS_SEEK] = syscall_seek;
  call[SYS_TELL] = syscall_tell;
  call[SYS_HALT] = syscall_halt;
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
  int *stack = f->esp;
  pid_t pid = *(stack+1);
  f->eax = process_wait(pid);
}

// EXEC CALL:
/* Handling function */
static void 
syscall_exec(struct intr_frame *f) {
  int *stack = f->esp;
  char * command = *(stack+1);
  /* Check if address is a user virtual address */
  if (!is_user_vaddr(command) || !pagedir_get_page(thread_current()->pagedir, command)){
    f->eax = -1;
    return;
  }
  tid_t res = process_execute(command);
  if (!thread_current()->child_load_success){
    f->eax = -1; 
    return;
  }
  f->eax = res;
}

// Assignment 6
static void syscall_create(struct intr_frame *f){
  int *stack = f->esp;
  char * filename = *(stack+1);
  int size = *(stack+2);
  f->eax = filesys_create(filename, size);
}

static void syscall_remove (struct intr_frame *f){
  int *stack = f->esp;
  char * filename = *(stack+1);
  f->eax = filesys_remove(filename);
}

static void syscall_open (struct intr_frame *f){
  /*
  int *stack = f->esp;
  char * filename = *(stack+1);
  struct fd_item * i = malloc(sizeof(struct fd_item));
  i->id = fd++;
  i->f = filesys_open(filename);
  hash_insert(FD_HASHMAP, &i->elem);
  */
}

static void syscall_close (struct intr_frame *f){

}
static void syscall_filesize (struct intr_frame *f){

}
static void syscall_read (struct intr_frame *f){

}
static void syscall_seek (struct intr_frame *f){

}
static void syscall_tell (struct intr_frame *f){

}
static void syscall_halt (struct intr_frame *f){
  shutdown_power_off();
}
