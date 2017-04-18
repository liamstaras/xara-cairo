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

//
// SelAll.h
//
// Header for Operations to Select All and Select none
//

#ifndef INC_SELALL
#define INC_SELALL


// Op Tokens
#define OPTOKEN_EDITSELECTALL	_T("SelectAll")
#define OPTOKEN_EDITSELECTNONE	_T("SelectNone")




/******************************************************************************************

>	class SelectAllOp : public Operation

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/5/94
	Purpose:	The Select All operation.
	SeeAlso:	Operation

******************************************************************************************/

class CCAPI SelectAllOp: public Operation
{

	CC_DECLARE_DYNCREATE( SelectAllOp )

public:
	SelectAllOp();							// Constructor

	static BOOL		Init();					// Register an OpDescriptor
	                                        // Read the state of an operation
	static OpState	GetState(String_256*, OpDescriptor*);		

	void			Do(OpDescriptor*);		// "Do" function

};

/******************************************************************************************

>	class SelectNoneOp : public Operation

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/5/94
	Purpose:	The Clear Selection operation.
	SeeAlso:	Operation

******************************************************************************************/

class CCAPI SelectNoneOp: public Operation
{

	CC_DECLARE_DYNCREATE( SelectNoneOp )

public:
	SelectNoneOp();							// Constructor

	static BOOL		Init();					// Register an OpDescriptor
	                                        // Read the state of an operation
	static OpState	GetState(String_256*, OpDescriptor*);		

	void			Do(OpDescriptor*);		// "Do" function

};

#endif // INC_SELALL
