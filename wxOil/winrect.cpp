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


/*
*/


#include "camtypes.h"




// Naughty!!
#define Swap(a,b)       { (a)^=(b), (b)^=(a), (a)^=(b); }



/********************************************************************************************

>	WinRect::WinRect()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/8/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for WinRect class.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

WinRect::WinRect() : wxRect()
{
}




/********************************************************************************************

>	WinRect::WinRect(INT32 left, INT32 top, INT32 right, INT32 bottom)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/8/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for WinRect class.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

WinRect::WinRect(INT32 Left, INT32 Top, INT32 Right, INT32 Bottom)
	: wxRect( Left, Top, Right - Left, Bottom - Top )
{
}




/********************************************************************************************

>	OilRect WinRect::ToOil(View *pView) const

	Author: 	Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/93
	Inputs:		Pointer to the view that we are converting in.
	Purpose:	Converts a WinRect to an OilRect.
	Errors:		None.

********************************************************************************************/

OilRect WinRect::ToOil(View *pView) const
{
	// Get pixel size for this view
	// NB. Previously this was done with 4 function calls!  - Tim
	FIXED16 PixelWidth, PixelHeight;
	pView->GetPixelSize(&PixelWidth, &PixelHeight);

	// Do the conversion and return the results.
	return OilRect(OilCoord(LongMulFixed16( x,			PixelWidth ),
						   -LongMulFixed16( y + height,	PixelHeight ) ),
				   OilCoord(LongMulFixed16( x + width,	PixelWidth ),
				  		   -LongMulFixed16( y,			PixelHeight ) ) );
}


