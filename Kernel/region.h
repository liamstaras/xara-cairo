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


// REGION.H
//
// Created: 08/6/93 by Will
//

#ifndef INC_REGION
#define INC_REGION


/********************************************************************************************

>	class Region

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/6/93
	Purpose:	The class defines an area of the screen. At the moment it is a DocRect,
				but eventually it will be decribed by a path or something.
		
********************************************************************************************/

class Region
{
public:
	Region();
	Region(DocRect RectRegion);

	BOOL MergeRegion(Region *RegionToMerge);
private:
	DocRect CurrentRegion;
};

#endif
