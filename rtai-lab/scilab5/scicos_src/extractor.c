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
#include <math.h>
#include "scicos_block.h"
#include "dynlib_scicos_blocks.h"
/*--------------------------------------------------------------------------*/ 
SCICOS_BLOCKS_IMPEXP void extractor(scicos_block *block,int flag)
{
  int i = 0,j = 0;
  if(flag==1){
    for(i=0;i<block->nipar;++i){
      j=block->ipar[i]-1;
      if(j<0) j=0;
      if(j>=block->insz[0]) j=block->insz[0]-1;
      block->outptr[0][i]=block->inptr[0][j];
    }
  }
}
/*--------------------------------------------------------------------------*/ 
