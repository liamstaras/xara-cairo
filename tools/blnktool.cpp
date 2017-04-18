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

// Implementation of the blank tool

/*
*/

#include "camtypes.h"
#include "oilfiles.h"
#include "csrstack.h"
//#include "viewrc.h"
//#include "markn.h"
#include "blobs.h"			// for blob rendering.
//#include "mike.h"			// <******* Remember to include your own personal .h file

#include "blnktool.h"		// <******* Remember to change to include the tool's header file


CC_IMPLEMENT_MEMDUMP(BlankTool,Tool_v1)
CC_IMPLEMENT_DYNCREATE(BlankInfoBarOp,InformationBarOp)

// Must come after the last CC_IMPLEMENT.. macro
#define new CAM_DEBUG_NEW     

// These are still char* while we wait for resource technology to be developed for modules
LPTSTR BlankTool::FamilyName	= _T("Blank Tools");
LPTSTR BlankTool::ToolName 	= _T("Blank Tool");
LPTSTR BlankTool::Purpose 	= _T("Blank manipulation");
LPTSTR BlankTool::Author 	= _T("Buster");

// Init those other useful static vars
BOOL			BlankTool::s_bCurrentTool 			= FALSE;
BlankInfoBarOp*	BlankTool::s_pBlankInfoBarOp		= NULL;

/********************************************************************************************

>	BlankTool::BlankTool()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Purpose:	Default Constructor.
				Other initialisation is done in BlankTool::Init which is called by the Tool Manager
	SeeAlso:	BlankTool::Init

********************************************************************************************/

BlankTool::BlankTool()
{
	m_pcCurrentCursor = NULL;
	m_pcNormalBlankCursor = NULL;
}

/********************************************************************************************

>	BlankTool::~BlankTool()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Purpose:	Destructor (Virtual). Does nothing.

********************************************************************************************/

BlankTool::~BlankTool()
{
}


/********************************************************************************************

>	BOOL BlankTool::Init( INT32 Pass )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Returns:	FALSE if it does not want to be created, TRUE otherwise
	Purpose:	Used to check if the Tool was properly constructed
	SeeAlso:	BlankTool::BlankTool

********************************************************************************************/

BOOL BlankTool::Init()
{
	// Declare all your ops here and only succeed if all declarations succeed

	BOOL ok = TRUE;

	// This section reads in the infobar definition and creates an instance of
	// BlankInfoBarOp.  Also pBlankInfoBarOp, the ptr to the tool's infobar, is set up
	// after the infobar is successfully read and created.
	if (ok)
	{
		s_pBlankInfoBarOp = new BlankInfoBarOp(this);
		ERROR2IF(s_pBlankInfoBarOp==NULL, FALSE, "Can't create Blank tool Infobar");
	}

	return (ok);
}


/********************************************************************************************

>	void BlankTool::Describe(void *InfoPtr)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		InfoPtr - 	A pointer to a tool info block. It is passed cast to void* as
							the version of the tool is unknown at this point. Later versions 
							of the Tool class may have more items in this block, that this 
							tool will not use
	Outputs:	InfoPtr - 	The structure pointed to by InfoPtr will have had all the info
							that this version of the Tool knows about
	Purpose:	Allows the tool manager to extract information about the tool

********************************************************************************************/

void BlankTool::Describe(void* InfoPtr)
{
	// Cast structure into the latest one we understand.
	ToolInfo_v1* Info = (ToolInfo_v1*) InfoPtr;

	Info->InfoVersion = 1;
	
	Info->InterfaceVersion = GetToolInterfaceVersion();  // You should always have this line.
		
	// These are all arbitrary at present.
	Info->Version = 1;
	Info->ID      = GetID();
	Info->TextID  = _R(IDS_BLANK_TOOL);

	Info->Family  = FamilyName;
	Info->Name    = ToolName;
	Info->Purpose = Purpose;
	Info->Author  = Author;

	Info->BubbleID = _R(IDBBL_BLANK_TOOLBOX);
}

/********************************************************************************************

>	virtual void BlankTool::SelectChange(BOOL isSelected)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		isSelected	- TRUE  = tool has been selected
							- FALSE = tool has been deselected
	Outputs:	-
	Returns:	-
	Purpose:	Starts up and closes down the blank tool
	Errors:		Debug warning if creating the cursor fails.
	SeeAlso:	-

********************************************************************************************/

void BlankTool::SelectChange(BOOL isSelected)
{
	if (isSelected)
	{
		if (!CreateCursors()) return;
		m_CurrentCursorID = CursorStack::GPush(m_pcNormalBlankCursor, FALSE);		// Push cursor but don't display now
		m_pcCurrentCursor = m_pcNormalBlankCursor;

		// This tool is now the current one
		s_bCurrentTool = TRUE;

		// Create and display the tool's info bar
		s_pBlankInfoBarOp->Create();
	}
	else
	{
		// Deselection - destroy the tool's cursors, if they exist.
		if (m_pcCurrentCursor != NULL)
		{
			CursorStack::GPop(m_CurrentCursorID);
			m_pcCurrentCursor = NULL;
			m_CurrentCursorID = 0;
		}
		DestroyCursors();

		// Remove the info bar from view by deleting the actual underlying window
		s_pBlankInfoBarOp->Delete();

		// ensure any tool object blobs are removed.
		BlobManager* BlobMgr = GetApplication()->GetBlobManager();
		if (BlobMgr != NULL)
		{
			BlobStyle bsRemoves;
			bsRemoves.ToolObject = TRUE;
			BlobMgr->RemoveInterest(bsRemoves);
		}

		// No longer the current tool
		s_bCurrentTool = FALSE;
	}
}

/********************************************************************************************

>	BOOL BlankTool::CreateCursors()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if all the blank tool cursors have been successfully created
	Purpose:	Creates all the blank tool cursors
	SeeAlso:	-

********************************************************************************************/

BOOL BlankTool::CreateCursors()
{
	// This tool has just been selected.  Create the cursors.
	m_pcNormalBlankCursor = new Cursor(this, _R(IDCSR_BLANKTOOLDEFAULT));

	if ( m_pcNormalBlankCursor==NULL || !m_pcNormalBlankCursor->IsValid())
	{
		DestroyCursors();
		return FALSE;
	}
	else
		return TRUE;
}

/********************************************************************************************

>	void BlankTool::DestroyCursors()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroys all the blank tool cursors
	SeeAlso:	-

********************************************************************************************/

void BlankTool::DestroyCursors()
{
	if (m_pcNormalBlankCursor != NULL) delete m_pcNormalBlankCursor;
}

/********************************************************************************************

>	void BlankTool::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		PointerPos 	- 	The DocCoord of the point where the mouse button was clicked
				Click 		-	Describes the type of click that was detected. 
				ClickMods 	-	Indicates which buttons caused the click and which modifers were
								pressed at the same time
				pSpread 	- 	The spread in which the click happened
	Returns:	-
	Purpose:	To handle a Mouse Click event for the Blank Tool.
	SeeAlso:	Tool::MouseClick; ClickType; ClickModifiers

********************************************************************************************/

void BlankTool::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )
{
	if (ClickMods.Menu) return;							// Don't do anything if the user clicked the Menu button

	// Make sure this click is one that we want
	if ( Click == CLICKTYPE_SINGLE || Click == CLICKTYPE_DOUBLE)
	{
	}

	if ( Click == CLICKTYPE_DRAG )
	{
	}
}

/********************************************************************************************

>	void BlankTool::OnMouseMove( DocCoord PointerPos,Spread* pSpread, ClickModifiers ClickMod )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		PointerPos 	- 	The DocCoord of the point where the mouse has moved to
				pSpread 	- 	The spread in which the move occurred
				ClickMods	- 	The state of the various modifiers at the time of the mouse move
	Returns:	TRUE if it handled the Click, FALSE otherwise
	Purpose:	To handle a Mouse Move event for the Blank Tool.
	SeeAlso:	Tool::MouseClick; ClickType; ClickModifiers

********************************************************************************************/

void BlankTool::OnMouseMove(DocCoord PointerPos,Spread* pSpread,ClickModifiers ClickMods)
{
	// You can use this func to do things like change the pointer shape, or
	// alter the text in the status bar

	// Use this func to change your curor
	// 		CursorStack::GSetTop(Cursor* pCursor, CurrentCursorID)

	// Display status bar text for the current position
	DisplayStatusBarHelp(PointerPos, pSpread, ClickMods);
}

/********************************************************************************************

>	void BlankTool::RenderToolBlobs(Spread* pSpread,DocRect* pDocRect)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		pSpread  = ptr to a spread
				pDocRect = ptr to DocRect of spread to render in
	Returns:	-
	Purpose:	Handles the RenderToolBlobs method.
				Renders the tool's blobs into the selected doc view.
	SeeAlso:	

********************************************************************************************/

void BlankTool::RenderToolBlobs(Spread* pSpread,DocRect* pDocRect)
{
	// Render into the selected doc view
	DocView* pDocView = DocView::GetSelected();
	if (pDocView != NULL)
	{
		// render you tool's blobs (if any) here
	}
}

/********************************************************************************************

>	void BlankTool::DisplayStatusBarHelp(DocCoord DocPos, Spread* pSpread, ClickModifiers ClickMods)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		DocPos - the document coordinate of the point to display help on
				pSpread - pointer to the spread containing DocPos
				ClickMods - the current click modifiers
	Outputs:	-
	Returns:	-
	Purpose:	Displays status help string for the given position in the status bar.
	SeeAlso:	BlankTool::GetCurrentStatusText

********************************************************************************************/

void BlankTool::DisplayStatusBarHelp(DocCoord DocPos, Spread* pSpread, ClickModifiers ClickMods)
{
	String_256 StatusMsg(_T(""));

	// Get a string from the underlying help function and display it.
	GetCurrentStatusText(&StatusMsg, pSpread, DocPos, ClickMods);
	GetApplication()->UpdateStatusBarText(&StatusMsg);							 
}

/********************************************************************************************

>	virtual BOOL BlankTool::GetStatusLineText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		ptest - pointer to a string
				pSpread points to a spread
				DocPos points to a point in a document
				ClickMods are the current click modifiers
	Outputs:	Updates the string in ptext
	Returns:	TRUE if the string was updates (FALSE if not updated)
	Purpose:	Returns the current status line help string
	SeeAlso:	BlankTool::GetCurrentStatusText, Tool_v1::GetStatusLineText

********************************************************************************************/

BOOL BlankTool::GetStatusLineText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods)
{
	// We can call the underlying help function to get a string and return the result.
	GetCurrentStatusText(ptext, pSpread, DocPos, ClickMods);
	return TRUE;
}

/********************************************************************************************

>	void BlankTool::GetCurrentStatusText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		pSpread points to a spread
				DocPos points to a point in a document
				ClickMods are the current click modifiers
	Outputs:	Updates the string in ptext
	Returns:	-
	Purpose:	Selects a suitable string for the status line based on the current location
				(as input via the parameters).
	SeeAlso:	BlankTool::GetStatusLineText, BlankTool::DisplayStatusBarHelp

********************************************************************************************/

void BlankTool::GetCurrentStatusText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods)
{
	// You must use the Spread, DocCoord and ClickModifiers to select a suitable string to be 
	// displaied in the status bar.  This is usually done via some sort of switch statement.
	// Having selected a suitable string you should do a statement like	
	//	ptext->Load(<#Insert your string ID here #>);

	// Delete this line when you display useful strings!
	ptext->Empty(); 
}

//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------

/********************************************************************************************

>	MsgResult BlankInfoBarOp::Message(Msg* Message) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		Message = The message to handle
	Outputs:	-
	Returns:	-
	Purpose:	Blank info bar dialog message handler
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MsgResult BlankInfoBarOp::Message(Msg* Message) 
{
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		// Check if the message is a CANCEL
		if (Msg->DlgMsg == DIM_CANCEL)
		{
			Close(); // Close the dialog 
		}
		else if (Msg->DlgMsg == DIM_CREATE)
		{
			// Initialise the infobar controls here
			// This is sent when you create the infobar in your tool startup code
		}
		else
		{
			// Switch statement allows you to do things when receiving messages
			// for specific gadgets
			//
			// Just uncomment the next 3 lines to get started
			//
			//		switch (Msg->GadgetID)
			//		{
			//		}
		}
	}

	// Pass the message on to the immediate blank class
	return (InformationBarOp::Message(Message));
}    
