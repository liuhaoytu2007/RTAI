/*
 * Project: rtai_cpp - RTAI C++ Framework 
 *
 * File: $Id: rtai_cpp.h,v 1.1 2004/06/06 14:13:43 rpm Exp $
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

#ifndef __RTAI_CPP_H__
#define __RTAI_CPP_H__

#ifdef __cplusplus
// for C++ only

#endif

/* for both C and C++ */
#ifdef __cplusplus
extern "C" {
#endif

extern void init_cpp( void  );
extern void cleanup_cpp( void );

#ifdef __cplusplus
}
#endif

#endif 
