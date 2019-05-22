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


static unsigned fd = 3;
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

/**
 * Terminates this thread with exit status given
 * This function is used for the exit call handler, 
 * as well as for every seg-fault-like situation
 */
static void terminate_thread(int exit_status) {
#ifdef USERPROG
  thread_current()->exit_status = exit_status;
#endif
  thread_exit ();
}

/**
 * Given fd, return the file pointer or NULL, if
 * no file exists with this descriptor or fd does not
 * belong to this process
 */
static struct file * get_file_by_fd(int file_descriptor) {
  struct fd_item item = {
    .id = file_descriptor,
  };
  struct hash_elem * element = hash_find(&thread_current()->fd_hashmap, &item.elem);
  if(!element) return NULL;
  return hash_entry(element, struct fd_item, elem)->f;
}


static void
syscall_exit (struct intr_frame *f)
{
  int *stack = f->esp;
  terminate_thread(*(stack+1));
}

static void
syscall_write (struct intr_frame *f)
{
  int *stack = f->esp;
  //ASSERT (*(stack+1) == 1); // fd 1
  int fd_stack = *(stack+1);
  char * buffer = *(stack+2);
  int    length = *(stack+3);
  if (fd_stack == 1) {
    putbuf (buffer, length);
    f->eax = length;
  } else {
    // if not stdout
    struct file * file_to_write = get_file_by_fd(fd_stack);
    if (!file_to_write || 
        !is_user_vaddr(buffer) || 
        !buffer || 
        !is_user_vaddr(buffer) ||
        !pagedir_get_page(thread_current()->pagedir, buffer)) {
      terminate_thread(-1);
      return;
    }
    f->eax = file_write(file_to_write, buffer, length);
  }
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
  if (!filename || !pagedir_get_page(thread_current()->pagedir, filename)) {
    terminate_thread(-1);
    return;
  }
  int size = *(stack+2);
  f->eax = filesys_create(filename, size);
}

static void syscall_remove (struct intr_frame *f){
  int *stack = f->esp;
  char * filename = *(stack+1);
  f->eax = filesys_remove(filename);
}

static void syscall_open (struct intr_frame *f){
  int *stack = f->esp;
  char * filename = *(stack+1);
  if (!filename || !pagedir_get_page(thread_current()->pagedir, filename)) {
    terminate_thread(-1);
    return;
  }
  struct fd_item * i = malloc(sizeof(struct fd_item));
  if(!i) {
    printf("Holy macro! We are out of memory as we have no space for the descriptor!\n");
    terminate_thread(-1);
  }
  i->id = fd++;
  struct file * file_opened = filesys_open(filename);
  if (file_opened) {
    i->f = file_opened;
    hash_insert(&(thread_current()->fd_hashmap), &i->elem);
    //ASSERT(hash_find(&(thread_current()->fd_hashmap), &i->elem));
    f->eax = i->id;
  } else {
    f->eax = -1;
  }
}

static void syscall_close (struct intr_frame *f){
  
}

static void syscall_filesize (struct intr_frame *f){
  int * stack_pointer = f->esp;
  int file_descriptor = *(stack_pointer + 1);
  struct file * file = get_file_by_fd(file_descriptor);
  if(!file) {
    //PANIC("AN INVALID FD WAS PASSED! THE BEHAVIOR IS UNDEFINED");
    // TODO: CHECK OUT THE PROPER BEHAVIOR IN CASE OF INVALID FD
    f->eax = -1;
  }
  f->eax = file_length(file);
}

static void syscall_read (struct intr_frame *f) {
  int * stack_pointer = f->esp;
  
  /* ---- VARIABLE INITIALIZATION ---- */
  const int file_descriptor = *(stack_pointer + 1);
  char * const buffer = *(stack_pointer + 2);
  const int buffer_size = *(stack_pointer + 3);


  /* ---- GETTING FILE FROM THE DESCRIPTOR ---- */
  struct file * const file = get_file_by_fd(file_descriptor);
  if(!file) {
    terminate_thread(-1);
    return;
  }


  /* ---- CHECKING THE BUFFER POINTER ON VALIDITY ---- */
  if (!buffer || !is_user_vaddr(buffer)) {
    terminate_thread(-1);
    return;
  }


  /* ---- READING THE FILE INTO BUFFER ---- */
  f->eax = file_read(file, buffer, buffer_size);

}

static void syscall_seek (struct intr_frame *f) {
  int * stack = f->esp;
  int file_descriptor = *(stack+1);
  int pos = *(stack+2);
  struct file * open_file = get_file_by_fd(file_descriptor);
  if (!open_file) {
    terminate_thread(-1);
    return;
  }
  file_seek(open_file, pos);
}

static void syscall_tell (struct intr_frame *f){
  int * stack = f->esp;
  int file_descriptor = *(stack+1);
  struct file * open_file = get_file_by_fd(file_descriptor);
  if (!open_file) {
    terminate_thread(-1);
    return;
  }
  f->eax = file_tell(open_file);
}

static void syscall_halt (struct intr_frame *f){
  shutdown_power_off();
}
