/*
* Scilab ( http://www.scilab.org/ ) - This file is part of Scilab
* Copyright (C) 2009 DIGITEO
* Copyright (C) 2009 Allan CORNET
* 
* This file must be used under the terms of the CeCILL.
* This source file is licensed as described in the file COPYING, which
* you should have received as part of this distribution.  The terms
* are also available at    
* http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
*
*/

/*--------------------------------------------------------------------------*/ 
#ifndef __DYNLIB_SCICOS_BLOCKS_H__
#define __DYNLIB_SCICOS_BLOCKS_H__

#ifdef _MSC_VER
#ifdef SCICOS_BLOCKS_EXPORTS
#define SCICOS_BLOCKS_IMPEXP __declspec(dllexport)
#else
#define SCICOS_BLOCKS_IMPEXP __declspec(dllimport)
#endif
#else
#define SCICOS_BLOCKS_IMPEXP
#endif

#endif /* __DYNLIB_SCICOS_BLOCKS_H__ */
/*--------------------------------------------------------------------------*/ 
