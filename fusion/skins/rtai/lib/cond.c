/*
 * Copyright (C) 2001,2002,2003,2004 Philippe Gerum <rpm@xenomai.org>.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */

#include <sys/types.h>
#include <errno.h>
#include <rtai/syscall.h>
#include <rtai/mutex.h>
#include <rtai/cond.h>

extern int __rtai_muxid;

static inline int __init_skin (void)

{
    __rtai_muxid = XENOMAI_SYSCALL2(__xn_sys_attach,RTAI_SKIN_MAGIC,NULL);
    return __rtai_muxid;
}


int rt_cond_create (RT_COND *cond,
		    const char *name)
{
    if (__rtai_muxid < 0 && __init_skin() < 0)
	return -ENOSYS;

    return XENOMAI_SKINCALL2(__rtai_muxid,
			     __rtai_cond_create,
			     cond,
			     name);
}

int rt_cond_bind (RT_COND *cond,
		  const char *name)
{
    return XENOMAI_SKINCALL2(__rtai_muxid,
			     __rtai_cond_bind,
			     cond,
			     name);
}

int rt_cond_delete (RT_COND *cond)

{
    return XENOMAI_SKINCALL1(__rtai_muxid,
			     __rtai_cond_delete,
			     cond);
}

int rt_cond_wait (RT_COND *cond,
		  RT_MUTEX *mutex,
		  RTIME timeout)
{
    return XENOMAI_SKINCALL3(__rtai_muxid,
			     __rtai_cond_wait,
			     cond,
			     mutex,
			     &timeout);
}

int rt_cond_signal (RT_COND *cond)

{
    return XENOMAI_SKINCALL1(__rtai_muxid,
			     __rtai_cond_signal,
			     cond);
}

int rt_cond_broadcast (RT_COND *cond)

{
    return XENOMAI_SKINCALL1(__rtai_muxid,
			     __rtai_cond_broadcast,
			     cond);
}

int rt_cond_inquire (RT_COND *cond,
		     RT_COND_INFO *info)
{
    return XENOMAI_SKINCALL2(__rtai_muxid,
			     __rtai_cond_inquire,
			     cond,
			     info);
}
