/*
 * xara-cairo, a vector drawing program
 *
 * Based on Xara LX, Copyright (C) 1993-2006 Xara Group Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, version 2.
 *
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */


/***********************************************************************************************

>	#define ENSURE(expr,description)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/93
	Inputs:		expr:          Expression to test
				description:   Description of why failure occured.
							   If NULL is specified then then no description field
							   will be displayed in the message box. 

	Returns:	- 
	Purpose:    This macro is similar to the ASSERT macro, the only difference
				being that a description is provided of why the failure occured.
				The symbol _ENSURES is defined if the ENSURE macro expands to something,
				else is undefined if the ENSURE macro has no effect. IMPORTANT NOTE:
				ENSURE is obsolete. Use ERROR2xx or ERROR3xx macros instead.
	Errors:		-
	SeeAlso:	-

***********************************************************************************************/
                                                          
#ifndef INC_ENSURE
#define INC_ENSURE

void CCAPI EnsureFailedLine(const TCHAR * AssertDescription, const char * lpszFileName, INT32 nLine);
	
// Set this #define to build ensures into retail builds too
//#define _INCLUDE_ENSURES_IN_RETAIL

	#ifdef _DEBUG
		// Debug ensure
		#define ENSURE(expr,description) ((expr) ? (void)0:  EnsureFailedLine(_T(description),__FILE__,__LINE__))
		#define _ENSURES
	#else 
		#ifdef _INCLUDE_ENSURES_IN_RETAIL
			// Retail ensures same as debug ensures at the moment
			#define ENSURE(expr,description) ((expr) ? (void)0:  EnsureFailedLine(NULL,___FILE__,__LINE__))
			#define _ENSURES
		#else
			// Ensure that evaluates to nothing
			#define ENSURE(expr,description) ((void)0)
		#endif
	#endif 
#endif
