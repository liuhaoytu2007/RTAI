/**
 * @file
 * This file is part of the RTAI project.
 *
 * @note Copyright (C) 2005 Philippe Gerum <rpm@xenomai.org> 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <posix/syscall.h>
#include <posix/posix.h>
#include <posix/thread.h>

static int __muxid;

static pthread_t __pthread_get_current (struct task_struct *curr)

{
    xnthread_t *thread = xnshadow_thread(curr);

    if (!thread || xnthread_get_magic(thread) != PSE51_SKIN_MAGIC)
	return NULL;

    return thread2pthread(thread); /* Convert TCB pointers. */
}

int __pthread_create (struct task_struct *curr, struct pt_regs *regs)

{
    struct sched_param param;
    pthread_t internal_tid;
    pthread_attr_t attr;
    int err;

    if (curr->policy != SCHED_FIFO) /* Only allow FIFO for now. */
	return -EINVAL;

    if (!__xn_access_ok(curr,VERIFY_WRITE,__xn_reg_arg1(regs),sizeof(internal_tid)))
	return -EFAULT;

    /* Build a default thread attribute, then make sure that a few
       critical fields are set in a compatible fashion wrt to the
       calling context. */

    pthread_attr_init(&attr);
    attr.policy = curr->policy;
    param.sched_priority = curr->rt_priority;
    attr.schedparam = param;
    attr.fp = 1;
    attr.name = curr->comm;

    err = pthread_create(&internal_tid,&attr,NULL,NULL);

    if (err)
	return -err; /* Conventionally, our error codes are negative. */

    err = xnshadow_map(&internal_tid->threadbase,NULL);

    if (!err)
	__xn_copy_to_user(curr,
			  (void __user *)__xn_reg_arg1(regs),
			  &internal_tid,
			  sizeof(internal_tid));

    /* FIXME: how are we supposed to delete the unmapped thread upon
       error? */
	
    return err;
}

int __pthread_detach (struct task_struct *curr, struct pt_regs *regs)

{ 
    pthread_t internal_tid;

    if (!__xn_access_ok(curr,VERIFY_READ,__xn_reg_arg1(regs),sizeof(internal_tid)))
	return -EFAULT;

    __xn_copy_from_user(curr,
			&internal_tid,
			(void __user *)__xn_reg_arg1(regs),
			sizeof(internal_tid));

    return -pthread_detach(internal_tid);
}

int __pthread_setschedparam (struct task_struct *curr, struct pt_regs *regs)

{ 
    struct sched_param param;
    pthread_t internal_tid;
    int policy;

    if (!__xn_access_ok(curr,VERIFY_READ,__xn_reg_arg1(regs),sizeof(internal_tid)))
	return -EFAULT;

    policy = __xn_reg_arg2(regs);

    if (policy != SCHED_FIFO)
	/* User-space POSIX shadows only support SCHED_FIFO for now. */
	return -EINVAL;

    if (!__xn_access_ok(curr,VERIFY_READ,__xn_reg_arg3(regs),sizeof(param)))
	return -EFAULT;

    __xn_copy_from_user(curr,
			&internal_tid,
			(void __user *)__xn_reg_arg1(regs),
			sizeof(internal_tid));

    __xn_copy_from_user(curr,
			&param,
			(void __user *)__xn_reg_arg3(regs),
			sizeof(param));

    return -pthread_setschedparam(internal_tid,policy,&param);
}

int __sched_yield (struct task_struct *curr, struct pt_regs *regs)

{
    return -sched_yield();
}

int __pthread_make_periodic_np (struct task_struct *curr, struct pt_regs *regs)

{ 
    struct timespec startt, periodt;
    pthread_t internal_tid;

    if (!__xn_access_ok(curr,VERIFY_READ,__xn_reg_arg1(regs),sizeof(internal_tid)))
	return -EFAULT;

    if (!__xn_access_ok(curr,VERIFY_READ,__xn_reg_arg2(regs),sizeof(startt)))
	return -EFAULT;

    if (!__xn_access_ok(curr,VERIFY_READ,__xn_reg_arg3(regs),sizeof(periodt)))
	return -EFAULT;

    __xn_copy_from_user(curr,
			&internal_tid,
			(void __user *)__xn_reg_arg1(regs),
			sizeof(internal_tid));

    __xn_copy_from_user(curr,
			&startt,
			(void __user *)__xn_reg_arg2(regs),
			sizeof(startt));

    __xn_copy_from_user(curr,
			&periodt,
			(void __user *)__xn_reg_arg3(regs),
			sizeof(periodt));

    return -pthread_make_periodic_np(internal_tid,&startt,&periodt);
}

int __pthread_wait_np (struct task_struct *curr, struct pt_regs *regs)

{
    pthread_t tid = __pthread_get_current(curr);
    return tid ? -pthread_wait_np() : EPERM;
}

static xnsysent_t __systab[] = {
    [__pse51_thread_create ] = { &__pthread_create, __xn_exec_init },
    [__pse51_thread_detach ] = { &__pthread_detach, __xn_exec_any },
    [__pse51_thread_setschedparam ] = { &__pthread_setschedparam, __xn_exec_any },
    [__pse51_sched_yield ] = { &__sched_yield, __xn_exec_primary },
    [__pse51_thread_make_periodic ] = { &__pthread_make_periodic_np, __xn_exec_primary },
    [__pse51_thread_wait] = { &__pthread_wait_np, __xn_exec_primary },
};

static void __shadow_delete_hook (xnthread_t *thread)

{
    if (xnthread_get_magic(thread) == PSE51_SKIN_MAGIC &&
	testbits(thread->status,XNSHADOW))
	xnshadow_unmap(thread);
}

int __pse51_syscall_init (void)

{
    __muxid =
	xnshadow_register_interface("posix",
				    PSE51_SKIN_MAGIC,
				    sizeof(__systab) / sizeof(__systab[0]),
				    __systab,
				    NULL);
    if (__muxid < 0)
	return -ENOSYS;

    xnpod_add_hook(XNHOOK_THREAD_DELETE,&__shadow_delete_hook);
    
    return 0;
}

void __pse51_syscall_cleanup (void)

{
    xnpod_remove_hook(XNHOOK_THREAD_DELETE,&__shadow_delete_hook);
    xnshadow_unregister_interface(__muxid);
}
