/*  Scicos
*
*  Copyright (C) 2015 INRIA - METALAU Project <scicos@inria.fr>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, see <http://www.gnu.org/licenses/>.
*
* See the file ./license.txt
*/
/*--------------------------------------------------------------------------*/ 
#include "scicos_block4.h"
#include "dynlib_scicos_blocks.h"
/*--------------------------------------------------------------------------*/ 
SCICOS_BLOCKS_IMPEXP void matz_reim(scicos_block *block,int flag)
{
	int i = 0;

	int mu = GetOutPortRows(block,1);
	int nu = GetOutPortCols(block,1);
	double *u1r=GetRealInPortPtrs(block,1);
	double *u1i=GetImagInPortPtrs(block,1);
	double *y1=GetRealOutPortPtrs(block,1);
	double *y2=GetRealOutPortPtrs(block,2);

	for (i=0;i<mu*nu;i++) 
	{
		*(y1+i)=*(u1r+i);
		*(y2+i)=*(u1i+i);
	}
}
/*--------------------------------------------------------------------------*/ 
