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
#include "MALLOC.h"
#include "dynlib_scicos_blocks.h"
/*--------------------------------------------------------------------------*/ 
SCICOS_BLOCKS_IMPEXP void delay4_ui32(scicos_block *block,int flag)
{
  /* Copyright INRIA
   
     Scicos block simulator
     Ouputs nx*dt delayed input */

  unsigned long *y = NULL,*u = NULL,*oz = NULL;
  int nz = 0,mz = 0;
  int i = 0;
  
  u=Getuint32InPortPtrs(block,1);
  y=Getuint32OutPortPtrs(block,1);
  oz=Getuint32OzPtrs(block,1);
  nz=GetOzSize(block,1,2);
  mz=GetOzSize(block,1,1);

  if ((flag==1)||(flag==6)||(flag==4)){
    y[0]=oz[0];
  }else if (flag == 2){
    /*  shift buffer */
    for (i=0; i<=(mz*nz)-2; i++){
      oz[i]=oz[i+1];
    }
    /* add new point to the buffer */
    oz[(mz*nz)-1]=u[0];
  }
}
/*--------------------------------------------------------------------------*/ 
