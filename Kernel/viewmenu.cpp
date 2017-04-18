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
// Context sensitive menu for DocViews
//
/*
*/

#include "camtypes.h"
#include "viewmenu.h"
#include "menuops.h"
#include "coldlog.h"
#include "snapops.h"
#include "blobs.h"
#include "mkshapes.h"
#include "mkstroke.h"
#include "combshps.h"
#include "selector.h"
#include "appprefs.h"	// OPTOKEN_PAGESIZEDLG
#include "guides.h"
#include "makebmp.h"
#include "opimgset.h"
#include "hlinkdlg.h"
#include "layergal.h"	// OPTOKEN_DELETEPAGEBACKGROUND
//#include "brushdlg.h"
#include "brushop.h"
#include "cutop.h"		// OPTOKEN_PASTE	matt-23/08/2000

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe

// An implement to match the Declare in the .h file.
// If you have many classes, it is recommended to place them all together, here at the start of the file
CC_IMPLEMENT_MEMDUMP(ViewContextMenu, ContextMenu)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW




/********************************************************************************************

>	BOOL ViewContextMenu::BuildOverView(Spread* pSpread, DocCoord ClickPos, ClickModifiers ClickMods)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if menu built OK
				FALSE (and sets error) otherwise
	Purpose:	To build the View pop-up menu.
				This function is complicated by the fact that it offers the current tool
				a chance to build menu items for any section of the view pop-up menu.
	Errors:		-

********************************************************************************************/

BOOL ViewContextMenu::BuildOverView(Spread* pSpread, DocCoord ClickPos, ClickModifiers ClickMods)
{
	ERROR2IF(pSpread==NULL,FALSE,"BuildOverView passed NULL spread pointer");

	BOOL ok = TRUE;

	//---------------------------------------------//
	// First ask selected nodes if they want to supply items and if so show the blob menu...
	if (AskBlobsForItems(pSpread, ClickPos, ClickMods))
	{
		// If they supply items then we won't add the normal items about the selection
		return TRUE;
	}

	//---------------------------------------------//
	// Find what the click was on and show the object menu...
//	NodeRenderableInk* pHitNode = NodeRenderableInk::FindSimpleAtPoint(pSpread, ClickPos);

	NodeRenderableInk* pHitNode = FindNodeAtPoint(pSpread,ClickPos);
	if (pHitNode)
	{
		BOOL HitNodeIsGuideline = IS_A(pHitNode,NodeGuideline);

		if (!HitNodeIsGuideline)		// BODGE so that guidelines (which can't be selected) can have a pop-up menu
		{
			// Now find out if there's just one node selected and if so ask it for some items...
			SelRange* pSel = GetApplication()->FindSelection();
			if (pSel!=NULL)
			{
				Node* pNode = pSel->FindFirst();
				if (pNode && pSel->FindNext(pNode) == NULL)
				{
					// There's only one object selected so let's ask it about itself...
					((NodeRenderableInk*) pNode)->OnNodePopUp(pSpread, ClickPos, this);
				}
			}
		}
		else
			pHitNode->OnNodePopUp(pSpread, ClickPos, this);

		if (!HitNodeIsGuideline)		// BODGE to stop guideline pop-up menus having irrelevant menu items
		{
			// Build the rest of the standard object menu...
			// Effect commands
			ok = ok && BuildEffectCommands();
			// Edit commands
			ok = ok && BuildTransferCommands(TRUE);
			// Arrange commands
		//	WEBSTER-ranbirr-13/11/96
//	#ifndef WEBSTER
			ok = ok && BuildCommand(OPTOKEN_MAKE_SHAPES);
//	#endif //webster
			ok = ok && BuildCommand(OPTOKEN_CONVERTTOBITMAP);
	// WEBSTER-ranbirr-13/11/96
	// Now Taken out by vector stroking code Neville 2/10/97
	#ifdef VECTOR_STROKING
		//	ok = ok && BuildCommand(OPTOKEN_MAKE_STROKE);
	#endif // VECTOR_STROKING
			ok = ok && BuildCommand(OPTOKEN_COMBINESHAPES,TRUE);
			MenuItem* pCombineRoot = GetLastItem();
	//	WEBSTER-ranbirr-13/11/96
//	#ifndef WEBSTER
				ok = ok && BuildCommand(OPTOKEN_ADDSHAPES, FALSE, pCombineRoot);
				ok = ok && BuildCommand(OPTOKEN_SUBTRACTSHAPES, FALSE, pCombineRoot);
				ok = ok && BuildCommand(OPTOKEN_INTERSECTSHAPES, FALSE, pCombineRoot);
				ok = ok && BuildCommand(OPTOKEN_SLICESHAPES, FALSE, pCombineRoot);
//	#endif //webster

	#ifndef WEBSTER
			// Imagesetting submenu
			ok = ok && BuildCommand(OPTOKEN_IMAGESETTING, TRUE);
			pCombineRoot = GetLastItem();
				ok = ok && BuildCommand(OPTOKEN_OVERPRINTFILL,	  FALSE, pCombineRoot);
				ok = ok && BuildCommand(OPTOKEN_OVERPRINTLINE,	  TRUE,  pCombineRoot);
				ok = ok && BuildCommand(OPTOKEN_PRINTONALLPLATES, FALSE, pCombineRoot);
	#endif //webster
			// Utils commands
			ok = ok && BuildCommand(OPTOKEN_WEBADDRESSDLG);
			ok = ok && BuildCommand(OPTOKEN_COLOUREDITDLG, TRUE);
PORTNOTE("other", "Removed brush edit dialog from popup menu")
#ifndef EXCLUDE_FROM_XARALX
			ok = ok && BuildCommand(OPTOKEN_BRUSHEDIT_DLG);
#endif
			ok = ok && BuildCommand(OPTOKEN_SELECTBRUSH);
		}

		return ok;
	}

	//---------------------------------------------//
	// If on white space then show the white space menu...
	// Window commands
//	MENUITEM OPTOKEN_TOOLBARDLG
//		MENUITEM OPTOKEN_VIEWCOLOURBAR		
//		MENUITEM OPTOKEN_VIEWSTATUSBAR
//		MENUITEM OPTOKEN_VIEWSCROLLBARS
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	ok = ok && BuildCommand(OPTOKEN_WINDOWNEWVIEW);
	ok = ok && BuildCommand(OPTOKEN_VIEWFULLSCREEN,TRUE);
#else
	ok = ok && BuildCommand(OPTOKEN_WINDOWNEWVIEW,TRUE);
#endif //webster

	// Paste option - we don't need to do the
	// BuildTransferCommands as the Cut and Copy operations
	// are never applicable here...	matt-23/08/2000
	ok = ok && BuildCommand(OPTOKEN_PASTE, TRUE);

	// View quality
	ok = ok && BuildCommand(OPTOKEN_WINDOWQUALITY,FALSE);
	MenuItem* pQualityRoot = GetLastItem();
	ok = ok && BuildCommand(OPTOKEN_QUALITYANTIALIASED, FALSE, pQualityRoot);
	ok = ok && BuildCommand(OPTOKEN_QUALITYNORMAL, FALSE, pQualityRoot);
	ok = ok && BuildCommand(OPTOKEN_QUALITYSIMPLE, FALSE, pQualityRoot);
	ok = ok && BuildCommand(OPTOKEN_QUALITYOUTLINE, FALSE, pQualityRoot);
	// View commands
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	ok = ok && BuildCommand(OPTOKEN_SHOWGRID);
	ok = ok && BuildCommand(OPTOKEN_SHOWGUIDES,TRUE);
#else
	ok = ok && BuildCommand(OPTOKEN_SHOWGRID,TRUE);
#endif //webster
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	ok = ok && BuildCommand(OPTOKEN_SNAPTOGRID);
	ok = ok && BuildCommand(OPTOKEN_SNAPTOGUIDES);
	ok = ok && BuildCommand(OPTOKEN_SNAPTOOBJECTS,TRUE);
#else
	ok = ok && BuildCommand(OPTOKEN_SNAPTOGRID,TRUE);
#endif //webster
	ok = ok && BuildCommand(OPTOKEN_DELETEPAGEBACKGROUND);
	// Page commands (almost "Properties...")
	ok = ok && BuildCommand(OPTOKEN_PAGESIZEDLG);

	return ok;
}




/********************************************************************************************

>	NodeRenderableInk* ViewContextMenu::FindNodeAtPoint(Spread* pSpread, DocCoord ClickPos)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/09/95
	Inputs:		pSpread = ptr to spread
				ClickPos = pos of click within the given spread
	Outputs:	-
	Returns:	ptr to node under click, or NULL if none found
	Purpose:	This copes with finding which node is under a click.
				It uses SelectorTool::FindPreProcessClickNode() to see if there is a node
				that would not get detected by a straight call to NodeRenderableInk::FindSimpleAtPoint()

				If this fails, it then returns the result returned by NodeRenderableInk::FindSimpleAtPoint()

	Errors:		-

********************************************************************************************/

NodeRenderableInk* ViewContextMenu::FindNodeAtPoint(Spread* pSpread, DocCoord ClickPos)
{
#ifndef STANDALONE
// selector tool not present in stand alone version
	NodeRenderableInk* pNode = SelectorTool::FindPreProcessClickNode(pSpread,ClickPos);
#else
	NodeRenderableInk* pNode = NULL;
#endif

	if (pNode == NULL)
		pNode = NodeRenderableInk::FindSimpleAtPoint(pSpread,ClickPos);

	return pNode;
}

/********************************************************************************************

>	BOOL ViewContextMenu::AskBlobsForItems(Spread* pSpread, DocCoord ClickPos, ClickModifiers ClickMods)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if node-related menu is allowed to be opened
				FALSE otherwise
	Purpose:	Ask the selected nodes whether they want the menu to appear and if so
				to add some menu items.
	Errors:		-

********************************************************************************************/

BOOL ViewContextMenu::AskBlobsForItems( Spread* pSpread,
										DocCoord ClickPos,
										ClickModifiers ClickMods
										)
{
	// Test for a click on a fill attribute first, as they aren't NodeRenderableInks, and so
	// won't be included in the tests below.
//	if (AttrFillGeometry::CheckAttrClick(ClickStart, TypeOfClick, ClickMods, StartSpread))
//	{
//		return TRUE;
//	}

	// Look for a selected object to pass the click to.
	SelRange* pSel = GetApplication()->FindSelection();
	if (pSel==NULL)
		return FALSE;

	Node* pNode = pSel->FindFirst();
	if (pNode == NULL)
		return FALSE;

	// Providing we are displaying object blobs, try passing the click on to each object
	// in the selection.
	BlobManager* pBlobs = GetApplication()->GetBlobManager();
	if (pBlobs && pBlobs->GetCurrentInterest().Object)
	{
		// For all selected objects . . .
		while (pNode != NULL)
		{
			// Is this node ink-renderable?
			if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableInk)))
			{
				// Yes.  Does it want the click?
				if (((NodeRenderableInk*) pNode)->OnBlobPopUp(pSpread, ClickPos, this))
				{
					// An object processed the click, so indicate that there's no more to do.
					return TRUE;
				}
			}

			// Try the next object in the selection.
			pNode = pSel->FindNext(pNode);
		}
	}

	return FALSE;
}




