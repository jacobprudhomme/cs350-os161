#include <types.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <kern/wait.h>
#include <lib.h>
#include <syscall.h>
#include <current.h>
#include <proc.h>
#include <thread.h>
#include <addrspace.h>
#include <copyinout.h>
#include <mips/trapframe.h>
#include "opt-A2.h"
#if OPT_A2
#include <kern/fcntl.h>
#include <vfs.h>
#include <wchan.h>
#endif /* OPT_A2 */

/* this implementation of sys__exit does not do anything with the exit code */
/* this needs to be fixed to get exit() and waitpid() working properly */
void sys__exit(int exitcode) {
  struct addrspace *as;
  struct proc *p = curproc;

#if OPT_A2
#else
  /* for now, just include this to keep the compiler from complaining about
     an unused variable */
  (void)exitcode;
#endif /* OPT_A2 */

  DEBUG(DB_SYSCALL,"Syscall: _exit(%d)\n",exitcode);

  KASSERT(curproc->p_addrspace != NULL);
  as_deactivate();
  /*
   * clear p_addrspace before calling as_destroy. Otherwise if
   * as_destroy sleeps (which is quite possible) when we
   * come back we'll be calling as_activate on a
   * half-destroyed address space. This tends to be
   * messily fatal.
   */
  as = curproc_setas(NULL);
  as_destroy(as);

  /* detach this thread from its process */
  /* note: curproc cannot be used after this call */
  proc_remthread(curthread);

#if OPT_A2
  spinlock_acquire(&p->p_lock);
  p->p_exited = true;
  p->p_exitcode = exitcode;
  spinlock_release(&p->p_lock);

  unsigned num_children = array_num(p->p_children);
  for (unsigned i = 0; i < num_children; i++) {
    struct proc *child = (struct proc *)array_get(p->p_children, i);
    spinlock_acquire(&child->p_lock);
    child->p_parent = NULL;
    spinlock_release(&child->p_lock);
  }

  wchan_wakeone(p->p_wchan);

  if (!p->p_parent) {
    proc_destroy(p);
  }
#else
  /* if this is the last user process in the system, proc_destroy()
     will wake up the kernel menu thread */
  proc_destroy(p);
#endif /* OPT_A2 */

  thread_exit();

  /* thread_exit() does not return, so we should never get here */
  panic("return from thread_exit in sys_exit\n");
}

#if OPT_A2
int sys_getpid(pid_t *retval) {
  *retval = curproc->pid;
  return 0;
}
#else
/* stub handler for getpid() system call                */
int
sys_getpid(pid_t *retval)
{
  /* for now, this is just a stub that always returns a PID of 1 */
  /* you need to fix this to make it work properly */
  *retval = 1;
  return(0);
}
#endif /* OPT_A2 */

/* stub handler for waitpid() system call                */
int
sys_waitpid(pid_t pid,
	    userptr_t status,
	    int options,
	    pid_t *retval)
{
  int exitstatus;
  int result;

#if OPT_A2
  if (options != 0) {
    return EINVAL;
  }

  struct proc *child = NULL;
  unsigned child_index;
  unsigned num_children = array_num(curproc->p_children);
  for (unsigned i = 0; i < num_children; i++) {
    struct proc *cur_child = (struct proc *)array_get(curproc->p_children, i);
    if (cur_child->pid == pid) {
      child = cur_child;
      child_index = i;
    }
  }

  if (!child) {
    return ECHILD;
  }

  spinlock_acquire(&child->p_lock);
  while (!child->p_exited) {
    wchan_lock(child->p_wchan);
    spinlock_release(&child->p_lock);
    wchan_sleep(child->p_wchan);
    spinlock_acquire(&child->p_lock);
  }
  spinlock_release(&child->p_lock);

  array_remove(curproc->p_children, child_index);

  exitstatus = _MKWAIT_EXIT(child->p_exitcode);

  proc_destroy(child);

  result = copyout((void *)&exitstatus, status, sizeof(int));
  if (result) {
    return result;
  }

  *retval = pid;

  return 0;
#else
  /* this is just a stub implementation that always reports an
     exit status of 0, regardless of the actual exit status of
     the specified process.
     In fact, this will return 0 even if the specified process
     is still running, and even if it never existed in the first place.

     Fix this!
  */

  if (options != 0) {
    return(EINVAL);
  }
  /* for now, just pretend the exitstatus is 0 */
  exitstatus = 0;
  result = copyout((void *)&exitstatus,status,sizeof(int));
  if (result) {
    return(result);
  }
  *retval = pid;
  return(0);
#endif /* OPT_A2 */
}

#if OPT_A2
int sys_execv(userptr_t progname) {
  int result = 0;

  char *kprogname;
  struct addrspace *as, *old_as;
  struct vnode *v;
  vaddr_t entrypoint, stackptr;

  size_t progname_len = strlen((char *)progname) + 1;
  kprogname = kmalloc(progname_len * sizeof(char));
  result = copyinstr(progname, kprogname, progname_len, NULL); /* MAYBE HERE (cast progname to const_userptr_t?) */
  if (result) {
    return result;
  }

  result = vfs_open(kprogname, O_RDONLY, 0, &v);
  if (result) {
    kfree(kprogname);
    return result;
  }

  as = as_create();
  if (as == NULL) {
    vfs_close(v);
    kfree(kprogname);
    return ENOMEM;
  }

  as_deactivate();
  old_as = curproc_setas(as);
  as_activate();
  as_destroy(old_as);

  result = load_elf(v, &entrypoint);
  vfs_close(v);
  if (result) {
    kfree(kprogname);
    return result;
  }

  result = as_define_stack(as, &stackptr);
  if (result) {
    kfree(kprogname);
    return result;
  }

  enter_new_process(0, NULL, stackptr, entrypoint);

  panic("enter_new_process returned\n");
  return EINVAL;
}

int sys_fork(struct trapframe *tf, pid_t *retval) {
  int result = 0;

  struct proc *child_proc = proc_create_runprogram("Child Process");
  if (child_proc == NULL) {
    return ENOMEM;
  }

  struct addrspace *parent_as = curproc_getas();
  struct addrspace *child_as;
  result = as_copy(parent_as, &child_as);
  if (result) {
    proc_destroy(child_proc);
    return result;
  }

  child_proc->p_addrspace = child_as;
  child_proc->p_parent = curproc;
  result = array_add(curproc->p_children, child_proc, NULL);
  if (result) {
    as_destroy(child_as);
    proc_destroy(child_proc);
    return result;
  }

  struct trapframe *tf_copy = kmalloc(sizeof(struct trapframe));
  memcpy(tf_copy, tf, sizeof(struct trapframe));
  result = thread_fork("Child Thread", child_proc, &enter_forked_process, tf_copy, 0); // MAYBE HERE (should data be cast before passing?)
  if (result) {
    as_destroy(child_as);
    proc_destroy(child_proc);
    return result;
  }

  *retval = child_proc->pid;

  return 0;
}
#endif /* OPT_A2 */

