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

// Imagemap

#ifndef OVERRIDELISTITEM
#define OVERRIDELISTITEM


/********************************************************************************************

>	class OverrideListItem : public ListItem

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Purpose:	An item in an OverrideList. See class OverrideList for an
				explanation.

********************************************************************************************/

class OverrideListItem : public ListItem
{
	CC_DECLARE_DYNAMIC(OverrideListItem)

	//Override functions
public:
	virtual BOOL OverrideFromAbove(OverrideListItem* pliOther)
	{
		return FALSE;
	}

	virtual BOOL OverrideFromBelow(OverrideListItem* pliOther)
	{
		return FALSE;
	}

	//IsA functions
public:
	virtual BOOL IsAClickableArea()
	{
		return FALSE;
	}
};



#endif // OVERRIDELISTITEM

