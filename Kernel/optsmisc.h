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

// Header for the handling code for the Misc tab of the options dialog box 

#ifndef INC_OPTSMISC
#define INC_OPTSMISC

#include "appprefs.h"

/********************************************************************************************

>	class MiscTab: public OptionsTabs

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/12/94
	Purpose:	Allows the user to set any options which are either too specific to
				have an associated tab or just do not fit any where else.
				This includes such things as:-
					Save preferences on exit switch and save preferences now button.
					Nudge Size
					Duplicate distance x,y
					Default constraint angle
					Current attribute settings

	SeeAlso:	OptionsTabs; AppPrefsDlg;

********************************************************************************************/

class MiscTab: public OptionsTabs
{         
	CC_DECLARE_DYNCREATE( MiscTab )  
public:

	MiscTab();
	~MiscTab();

	virtual BOOL Init();                        

	// Function to handle the messages for this tab/pane window
	virtual BOOL HandleMsg(DialogMsg* Msg);

	// Function to set up the values on this tab when being opened
	virtual BOOL InitSection();

	// Function to commit the values on this tab on ok
	virtual BOOL CommitSection();

	// Functions to handle document changing messages
	virtual BOOL GreySection();			// Called when the user has closed all documents
	virtual BOOL UngreySection();		// Called when the user has opened a document
	BOOL ChangeControlStatus(const BOOL Status); // used to grey/ungrey all controls
	virtual BOOL UpdateSection(String_256 *DocumentName);	// Called when the user has switched to a new document

	// Special function called when the import with layers state is changed
	BOOL UpdateImportWithLayers();

	// Special function called when the ask before applying attribute state is changed
	BOOL UpdateAskBefore();

	// Function called when new default units come into operation.
	virtual BOOL NewUnitsInSection();

	virtual CDlgResID GetPageID();		// Return section/page id system

	virtual BOOL IsDocumentOption();	// Return True if document option  

	virtual BOOL IsProgramOption();		// Return True if program option

protected:
	// Lots of controlling and handling functions specific to this section
	BOOL InitControlsWithUnits();		// sets up controls which display
										// measurements in units
	BOOL SetUpAngleList();				// Sets up the list of constraint angles

private:
	BOOL GreyStatus;			// Remember the state that we are currently in

}; 

#endif // INC_OPTSMISC
