/*
 * Project: rtai_cpp - RTAI C++ Framework 
 *
 * File: $Id: rtai_wrapper.h,v 1.1 2004/06/06 14:13:43 rpm Exp $
 *
 * Copyright: (C) 2001,2002 Erwin Rol <erwin@muffin.org>
 *
 * Licence:
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
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

#ifndef __RTAI_WRAPPER_H__
#define __RTAI_WRAPPER_H__

#define RT_TASK struct rt_task_struct

/**
 * The aim of this file is to contain all RTAI functions that can by no
 * means be included with the C++ compiler.
 */

#ifdef __cplusplus
extern "C"  {
#endif

#include <rtai_types.h>
	
void __rt_get_global_lock(void);

void __rt_release_global_lock(void);

int __hard_cpu_id(void);

extern RT_TASK*  __rt_task_init(void (*rt_thread)(int), int data,
                                int stack_size, int priority, int uses_fpu,
	                        void(*signal)(void));

extern int __rt_task_delete(RT_TASK *task);


#ifdef __cplusplus
}
#endif

#endif /* __RTAI_WRAPPER_H__ */
