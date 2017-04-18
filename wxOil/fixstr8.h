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


#ifndef INC__FIXSTRING8
#define INC__FIXSTRING8

#include "basestr.h"


/*************************************************************************************
	class String_8 : public StringBase
	
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12th August 1993	
	Purpose:	Extends its String base class to become fixed-length and atomically
				allocatable, ie. a failed constructor call for this class will never
				leave the object in an illegal state.
*************************************************************************************/

class CCAPI String_8 : public StringBase
{
	static const INT32 FIX_LEN_BUFSIZE;

private:
	void CopyConstruct( const StringBase &other );
	
public:
	String_8(const TCHAR *psz = TEXT(""));
	String_8( const StringBase &other )	{ CopyConstruct( other ); }
	String_8( const String_8 &other )	{ CopyConstruct( other ); }
	String_8(UINT32 resID, UINT32 hinst = 0);
	String_8(const wxString& wxs);
	virtual ~String_8();

	virtual BOOL Alloc(INT32 nSize);

private:
	TCHAR fixedbuf[FIX_LEN_BUFM(8)];
};




/**************************************************************************************
>	virtual String_8::~String_8()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12th August 1993
	Purpose:	Destroys this fixed-length string.
***************************************************************************************/

inline String_8::~String_8()
{
	// If the string is using the fixed-length buffer then make sure the
	// base class doesn't try to delete[] it.
	if (text == fixedbuf) text = 0;
}

#endif	// INC__FIXSTRING8



////////////////////////////////////////////////////////////////////////////////////////
