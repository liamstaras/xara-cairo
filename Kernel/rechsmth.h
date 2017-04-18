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


#ifndef INC_RECHSMTH
#define INC_RECHSMTH




/***********************************************************************************************

>	class BitmapSmoothingRecordHandler : public CamelotRecordHandler

	Author:		Andy_Hills (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/11/00
	Purpose:	Handles the reading of document bitmap smoothing flag record
	SeeAlso:	-

***********************************************************************************************/

class BitmapSmoothingRecordHandler : public CamelotRecordHandler
{
	// Give my name in memory dumps
	CC_DECLARE_DYNAMIC(BitmapSmoothingRecordHandler);

public:
	BitmapSmoothingRecordHandler() : CamelotRecordHandler() {}
	~BitmapSmoothingRecordHandler() {}

	virtual UINT32*	GetTagList();
	virtual BOOL	HandleRecord(CXaraFileRecord* pCXaraFileRecord);

#ifdef XAR_TREE_DIALOG
	virtual void GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr);
#endif

};

#endif //INC_RECHSMTH
