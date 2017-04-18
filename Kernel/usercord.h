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

// UserCoords - ie SpreadCoords relative to the user specified origin

#ifndef INC_USERCORD
#define INC_USERCORD


class DocCoord;
class Spread;

/********************************************************************************************
>	class UserCoord : public Coord

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/95
	Purpose:	Encapsulate coord system relative to a user specified origin
				on a spread by spread basis
********************************************************************************************/

class UserCoord : public Coord
{
public:
	UserCoord() : Coord() {};
	UserCoord(INT32 x, INT32 y) : Coord(x,y) {};

	DocCoord ToSpread(Spread* pSpread);
};

#endif  // INC_USERCORD
