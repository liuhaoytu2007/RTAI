/*
 * Project: rtai_cpp - RTAI C++ Framework 
 *
 * File: $Id: mutex.cc,v 1.4 2013/10/22 14:54:14 ando Exp $
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
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include "mutex.h"

namespace RTAI {

Mutex::Mutex()
:	Semaphore(1, RES_SEM)
{
}

Mutex::~Mutex(){
}

ScopedMutex::ScopedMutex( Mutex& m )
:	m_Mutex( m )
{
	m_Mutex.wait();
}

ScopedMutex::~ScopedMutex()
{
	m_Mutex.signal();
}

};
