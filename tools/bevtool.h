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

// The FreeHand Tool
// Created by Rik on 2/9/93



#ifndef INC_BEVELTOOL
#define INC_BEVELTOOL

#ifdef BUILDSHADOWS

#include "wrkrect.h"
#include "bevinfo.h"
#include "dragtool.h"

class Cursor;
class BevelInfoBarOp;
class NodeBevelController;
class OpDragCreateBevel;
class AttrJoinType;

#define OPTOKEN_DRAGBEVEL _T("DragBevel")

/********************************************************************************************

>	class OpDragBevel : public SelOperation

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/8/99
	Purpose:	Copy of the blend nodes operation in blndtool.h

********************************************************************************************/

class OpDragBevel : public SelOperation
{
CC_DECLARE_DYNCREATE(OpDragBevel);

public:
	// Construction/Destruction
	OpDragBevel();                     
	~OpDragBevel();

	// The all important Do functions
	BOOL DoDrag(BevelInfoBarOp * pOp, DocCoord & PointerPos, BOOL bDragOnBlob,
		DocRect *pBlobRect = NULL);
	virtual BOOL SnappingDrag() { return FALSE; }
	
	// Virtual functions needed for the dragging operations to work
	virtual void DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread*, BOOL bSolidDrag);
	virtual void DragFinished(	DocCoord PointerPos, 
								ClickModifiers ClickMods, Spread*, 
								BOOL Success, BOOL bSolidDrag);

	// sets up the dragging information necessary to render the drag blobs
	BOOL SetupDragInfo();

	// Some Render functions to will draw the EORed drag box
	void RenderDragBlobs(INT32 Width,Spread* pSpread,BOOL bKeepDirections=FALSE);

	// Some Render functions to will draw the EORed drag box
	void RenderDragBlobs(DocRect Rect,Spread* pSpread, BOOL bSolidDrag);
		
	// These functions required for the OpDescriptor class
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);

	void GetOpName(String_256* OpName);

private:
	
	BevelInfoBarOp * m_pBarOp;

	BOOL m_bHasDragged;
	DocCoord m_LastPointerPos;
	DocCoord m_ThisPointerPos;

	DocRect m_BlobRect;

	// the selection's inside bounding rect to be contoured
	DocRect m_SelRect;

	// the summed path of all nodes
	Path * m_pPathList;
	UINT32 m_NumPaths;
	LineWidthAttribute** m_pPathLineWidthList; // MRH

	// whether each path is inner or outer
	BOOL * m_pPathOuterList;

	// the start indexes of paths which are to be summed together
	UINT32 * m_pSetList;
	UINT32 m_NumSets;

	BOOL m_bDragStartedOnBlob;

//	INT32 m_Flatness;
	double m_Flatness;

	JointType m_JoinType;

	// Karim 11/05/2000
	// Tests whether a specific node should have its outline taken into account when
	// the user drags a bevel.
	BOOL NodeCanBeDragBevelled(Node* pSubNode, Node* pNode, BOOL bBevelNodesExist);

	void AlterPointerPosToAccountForBlob(DocCoord * pPoint);

	// in dragging, we have the starting width and also the calculated width
	// from the pointers starting position. Thus, when dragging we calculate the
	// offset of the pointer from the original starting drag width, and apply
	// this offset to the original width to give the width to render the drag blobs
	// to.
	
	// the starting width to use
	MILLIPOINT m_OriginalWidth;

	// the starting drag width to use when calculating the offset of the pointer
	MILLIPOINT m_StartDragWidth;


};

/********************************************************************************************
>	class BevelBlob : public ListItem

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/99
	Purpose:	It renders the bevel tool blob (i,e, light angle) per attribute
********************************************************************************************/

class BevelBlob : public ListItem
{
CC_DECLARE_MEMDUMP(BevelBlob);
public:
	BevelBlob();
	~BevelBlob();

	// adds an attribute to my list
	BOOL AddNode(NodeBevelController * pAttr);

	// access functions
	// sets the angle for the blob and also sets all the angles in my bevel controller
	// list's angles too, if SetList is TRUE
	void SetAngle(INT32 Angle, BOOL bSetList = TRUE);
	void SetTilt(INT32 Angle, BOOL bSetList = TRUE);

	INT32  GetAngle()						 { return m_BlobAngle;  }
	BOOL AmDragging()					 { return m_bDragging;  }

	// blob rendering stuff
	void RenderBlob(RenderRegion* pRegion);

	DocRect GetRect()				 { return m_BevelNodeListRect; }

	// checks the attributes to see if I'm still valid
	BOOL IsBlobValid();

	// returns TRUE for a click on my blob
	BOOL HitTest(const DocCoord &dc);

	// calculates my angle depending on the given doc-coord
	// used to do drags
	void CalculateAngle(const DocCoord &dc, BOOL Constrain = FALSE);

	List * GetList() { return &m_BevelNodeList; }

	// renders the nodes with the given light angle (used in dragging)
	void RenderNodes();

	// changes the contrast variables for all the nodes in the list
	void ChangeContrast(INT32 NewContrast);

protected:
	// list of the attributes that I affect
	List m_BevelNodeList;

private:
	INT32 m_BlobAngle;
	DocRect m_BevelNodeListRect;
	DocRect m_BlobDocRect;
	DocCoord m_BlobCentre;
	BOOL m_bDragging;
} ;

/********************************************************************************************
>	class BevelTool : public Tool_v1

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/99
	Purpose:	Handles the bevelling tool
********************************************************************************************/

class CCAPI BevelTool : public DragTool
{
// Give my name in memory dumps
CC_DECLARE_MEMDUMP(BevelTool);

public:
	BevelTool();
	~BevelTool();
	BOOL Init();
	void Describe(void* InfoPtr);
	UINT32 GetID();
	
	// Event Handlers
	virtual void SelectChange(BOOL);
	virtual void OnClick( DocCoord, ClickType, ClickModifiers, Spread* );
	virtual void OnMouseMove(DocCoord Pos, Spread* pSpread, ClickModifiers ClickMods);
	virtual BOOL GetStatusLineText(String_256* ptext, Spread*, DocCoord, ClickModifiers);

	// Rendering the blobs functions
	virtual void RenderToolBlobs(Spread*, DocRect*);

	// renders a single blob (EORed)
	void RenderBlob(BevelBlob * pBlob);

	// makes sure the blob list is regenerated on the next RenderToolBlobsOn
	void RegenerateBlobsOnNextRender() { m_bRegenBlobsOnNextRender = TRUE; }

	// removes the blobs which exist, re-sets up the blob list and re-renders them
	void InvalidateToolBlobs();

	void SetupSelectionBlobs();

	// clears the tool's blob list
	void ClearToolBlobList() { m_BevelBlobList.DeleteAll(); }
	
	// sets all the tool blob angles (removes the blobs & then re-renders them with
	// the new angle - used by the info bar for the light angle slider changing)
	void ChangeBlobAngles(INT32 Angle);
	void ChangeBlobTilts(INT32 Tilt);

	static BOOL AmActive() { return m_bActive; }

	DocRect GetBlobRect() { return m_BlobListBounds; }

	BOOL AreToolBlobsRenderedOnSelection();

	// clears and then re-sets up the list of blobs
	// returns FALSE for no selection
	BOOL SetupBlobList();

	// this selects bevel controller nodes if just the children are selected (like
	// if the user has just a text object selected, but not its parent controller)
	// needs to be public otherwise the infobar can't use it
	void DoSelectBevelNodesFromChildren();

	// access functions to the blob list to tell if 'many' should be displayed
	// for particular attributes in multiple selections
	BOOL AreManyBevelTypes();
	BOOL AreManyBevelDirections();
	BOOL AreManyBevelIndents();
	BOOL AreManyBevelContrasts();
	BOOL AreManyBevelLightAngles();
	BOOL AreManyBevelLightTilts();

	static void DisplayStatusBarHelp(UINT32 StatusIDX);

	// re-renders the bevel nodes when a contrast change occurs - used in interactive dragging
	void ChangeContrastAndReRender(INT32 NewContrast);

	INT32 CalculateBevelWidth(DocRect &Rect, DocCoord &PointerPos);

	// static drag bevel op
	static OpDragBevel *m_pDragBevelOp;

	// enables/disables blob rendering
	void SetBlobRendering(BOOL b)
	{
		if (b)
			m_bDisableBlobRendering = FALSE;
		else
			m_bDisableBlobRendering = TRUE;
	}

protected:
	// Helper functions
	// Functions to load, remove and change the active cursor
	BOOL LoadCursors();
	void RemoveCursors();
	void ChangeCursor(Cursor* pCursor);

protected:
	static 	TCHAR* FamilyName;	// The Tools Family Name
	static 	TCHAR* ToolName;		// The Tool Name
	static 	TCHAR* Purpose;		// What the tool is for
	static 	TCHAR* Author;		// Who wrote it

	// The status line text
	String_256 StatusMsg;

	// The tools cursor
	Cursor* pNormalCursor;				// The normal cursor
	Cursor* pActiveCursor;				// The normal cursor
	Cursor* pMouseOverCursor;			// The selection cursor (for arrow blobs)
	Cursor* pSizeCursor;				// The selection cursor (for size blobs)
	INT32 CurrentCursorID;				// ID of the current cursor on the stack

	static BevelInfoBarOp * pBevelInfoBarOp; // my info bar

	// the list of blobs
	List m_BevelBlobList;

	DocRect m_BlobListBounds;

	// recalculates the bounding rects of selected nodes inside of the document
	void RecalculateSelectionBoundingBoxes(BOOL bForceRedraw=TRUE);

	static BOOL m_bActive;

	BOOL m_bRegenBlobsOnNextRender;

	FIXED16 m_LastZoom;

	BOOL m_bDisableBlobRendering;
private:

	// Status line text ID
	static UINT32	StatusID;

	// the blobs' coordinates for drag create bevels or changing bevel widths
	DocCoord m_BlobPoints[8];

	// returns TRUE if the point is over a selection blob
	BOOL IsPointOverSelectionBlob(DocCoord &Point, DocRect * pRect);

	void InformWarningContourExistsInSelection();

};

#endif  //INC_FREEHAND
#endif



