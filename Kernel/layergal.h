/*
 * xara-cairo, a vector drawing program
 *
 * Based on Xara XL, Copyright (C) 1993-2006 Xara Group Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

// The Layers Gallery header file

#ifndef INC_LAYERGAL
#define INC_LAYERGAL

//#include "dialogop.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "undoop.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "layer.h"

class LayerSGallery;

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

enum MoveLayerType
{
	MOVELAYER_UPONE,
	MOVELAYER_DOWNONE
};



//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------



/********************************************************************************************

>	class OpLayerGalParam: 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/8/94
	Purpose:	Holds the parameters for the OpLayerGalChange to perform its stuff on
	SeeAlso:	-

********************************************************************************************/

enum OpLayerGalReason
{
	LAYER_NONE,
	LAYER_NEW,
	LAYER_DELETE,
	LAYER_CHANGE,
	LAYER_CHANGE_NAME,
	LAYER_MOVE,
	LAYER_COPY,
	LAYER_VISIBLE,
	LAYER_ALLVISIBLE,
	LAYER_LOCKED,
	LAYER_MULTILAYER,
	LAYER_TOGGLEBACKGROUND,
	// special frame flags
	LAYER_SOLID,
	LAYER_OVERLAY,
	LAYER_MAKEFRAME,
	LAYER_ACTIVE,
	// special frame related items
	FRAME_SELECT,
	FRAME_NEW,
	FRAME_COPY,
	FRAME_DELETE,
	FRAME_MOVE,
	FRAME_ALLVISIBLE,
	FRAME_MULTILAYER,
	// Special page background reason 
	PAGEBACKGROUND_DEFAULT		// actually deletes the page background layer
};

class OpLayerGalParam
{
public:

	Layer*				pLayer;
	LayerStatus			Status;
	BOOL				NewState;
	BOOL				ToggleBackground;
	Node*				pContextNode;
	LayerSGallery*		pLayerSGal;
	AttachNodeDirection	AttDir;
	BOOL				MoveRedraw;	// Flag that tells the LAYER_MOVE code whether it should redraw after the op

	// Vars used by LAYER_CHANGE
	BOOL		OldVisibleState;
	BOOL		OldLockedState;
	String_256 	OldName;
	BOOL		VisibleState;
	BOOL		LockedState;
	String_256 	NewName;

	OpLayerGalParam(OpLayerGalParam& Other)
	{
		pLayer 			 = Other.pLayer;
		Status 			 = Other.Status;
		NewState 		 = Other.NewState;
		ToggleBackground = Other.ToggleBackground;
		pContextNode 	 = Other.pContextNode;
		pLayerSGal 		 = Other.pLayerSGal;
		AttDir 			 = Other.AttDir;
		MoveRedraw 		 = Other.MoveRedraw;

		VisibleState 	 = Other.VisibleState;
		LockedState 	 = Other.LockedState;
		OldVisibleState  = Other.OldVisibleState;
		OldLockedState   = Other.OldLockedState;
		OldName 		 = Other.OldName;
		NewName 		 = Other.NewName;

		Reason 			 = Other.Reason;
		pParentSpread 	 = Other.pParentSpread;
	}

//-------------
	OpLayerGalParam(OpLayerGalReason ThisReason,Spread* pThisParentSpread)
	 : 	pLayer(NULL), ToggleBackground(FALSE),
	 	pContextNode(NULL), pLayerSGal(NULL),
		Reason(ThisReason), pParentSpread(pThisParentSpread)  { /* empty */ }

	OpLayerGalParam() : pLayer(NULL), ToggleBackground(FALSE),
	 	pContextNode(NULL), pLayerSGal(NULL),
		Reason(LAYER_NONE), pParentSpread(NULL)  { /* empty */ }

	Spread*				GetParentSpread() 	{ return pParentSpread; }
	OpLayerGalReason 	GetReason() 		{ return Reason; }

private:
	// These params can only be initialised via the constructor
	OpLayerGalReason	Reason;
	Spread*				pParentSpread;
};


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

/********************************************************************************************

>	class LayerStateAction : public Action

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Purpose:	An action which changes the state of a layer or the layer system
	SeeAlso:	-

********************************************************************************************/

class LayerStateAction : public Action
{

CC_DECLARE_DYNCREATE(LayerStateAction)

public:
	LayerStateAction();
	~LayerStateAction();
	virtual ActionCode Execute();
	static ActionCode Init( UndoableOperation* pOp,
							ActionList* pActionList,
							OpLayerGalParam Param);
protected:
	OpLayerGalParam Param;
};

/********************************************************************************************

>	class OpLayerGalChange: public UndoableOperation

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/8/94
	Purpose:	Performs an undoable change (new, delete, change) to a single layer.
	SeeAlso:	-

********************************************************************************************/

#define OPTOKEN_LAYERGALCHANGE      _T("LayerGalChange")		// Optoken for the layer gallery change operation 

class CCAPI OpLayerGalChange: public UndoableOperation
{         

	CC_DECLARE_DYNCREATE( OpLayerGalChange );

public:
	OpLayerGalChange();								
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
    void 			DoWithParam(OpDescriptor*, OpParam* pOpParam);            
	void 			GetOpName(String_256* OpName);
	static BOOL		IsFrameMode();			

	BOOL DoInvalidateLayerRegion(Layer* pLayer);

	virtual BOOL Undo(); 
	virtual BOOL Redo(); 	

	// Karim 21/01/2000 - see UndoableOperation base implementation.
	virtual BOOL MayChangeNodeBounds() const { return TRUE; }

private:
	OpLayerGalReason	Reason;
	Spread* 			pSpread;
	Layer*				pOpUndoLayer;	// Ptr to a layer relevent to op reason when an Undo occurs
	Layer*				pOpRedoLayer;	// Ptr to a layer relevent to op reason when a Redo occurs
	UINT32				UndoIDS;
	BOOL				BroadcastLayerChanges;
};  

/********************************************************************************************

>	class CCAPI OpDeletePageBackground : public OpLayerGalChange

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/97
	Purpose:	Performs an undoable change of deleting the page background layer.
	SeeAlso:	-

********************************************************************************************/

#define OPTOKEN_DELETEPAGEBACKGROUND	_T("DeletePageBackground")

class CCAPI OpDeletePageBackground : public OpLayerGalChange
{         
	CC_DECLARE_DYNCREATE( OpDeletePageBackground );

public:
	OpDeletePageBackground();
	
	static OpState	GetState(String_256*, OpDescriptor*);		

	void Do( OpDescriptor * pOpDesc );
};  



//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------



/********************************************************************************************

>	class LayerDlgParam: 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/94
	Purpose:	Holds the parameters for the LayerNameDlg to perform its stuff on
	SeeAlso:	-

********************************************************************************************/

class LayerDlgParam
{
public:

	LayerDlgParam(OpLayerGalReason ThisReason,Spread* pThisSpread,Layer* pThisLayer)
	 : Reason(ThisReason),pSpread(pThisSpread), pLayer(pThisLayer) { /* empty */ }

	OpLayerGalReason 	GetReason() { return Reason; }
	Spread*				GetSpread()	{ return pSpread; }
	Layer*				GetLayer() 	{ return pLayer; }

private:
	// These params can only be initialised via the constructor
	OpLayerGalReason	Reason;
	Spread*				pSpread;
	Layer*				pLayer;
};



#define OPTOKEN_LAYERNAMEDLG _T("LayerNameDlg")

/********************************************************************************************

>	class LayerNameDlg: public DialogOp

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/94
	Purpose:	Allows the user to edit the parts of a layer.
	SeeAlso:	-

********************************************************************************************/

class LayerNameDlg: public DialogOp
{         
	CC_DECLARE_DYNCREATE( LayerNameDlg )  
public:

	LayerNameDlg();
	void DoWithParam(OpDescriptor*,OpParam*);		// "DoWithParam" function        
	BOOL Create(); 

	static const INT32 IDD;
	static const CDlgMode Mode;

	static OpState GetState(String_256*, OpDescriptor*);	
	static BOOL Init();                        
	static void StartUp(LayerDlgParam* pParam);

	virtual MsgResult Message(Msg* Message);

	void ShowLayerDetails();
	void CommitDialogValues();

private:
	static void CreateUniqueLayerCopyName(const String_256& strName, String_256* pCopyName);

	Layer*  			pLayer;
	Spread* 			pSpread;
	OpLayerGalReason 	Reason;

	static BOOL 		 IsOpen;
	static LayerNameDlg* pLayerNameDlg;
}; 



//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

#define OPTOKEN_DISPLAYLAYERGALLERY _T("DisplayLayerGallery")	// Optoken for the display layer gallery operation

/********************************************************************************************

>	class OpDisplayLayerGallery: public Operation

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/8/94
	Purpose:	Displays the layers gallery. This op can be attached to a menu, button or keypress
	SeeAlso:	-

********************************************************************************************/

class CCAPI OpDisplayLayerGallery: public Operation
{         
	CC_DECLARE_DYNCREATE( OpDisplayLayerGallery );

public:
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
    void 			Do(OpDescriptor*);
};  


//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

#define OPTOKEN_DISPLAYFRAMEGALLERY _T("DisplayFrameGallery")	// Optoken for the display frame gallery operation

PORTNOTE("galleries", "Disable frame gallery")
#ifndef EXCLUDE_FROM_XARALX
/********************************************************************************************

>	class OpDisplayFrameGallery: public Operation

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/97
	Purpose:	Displays the frame gallery. This op can be attached to a menu, button or keypress
	SeeAlso:	-

********************************************************************************************/

class CCAPI OpDisplayFrameGallery: public Operation
{         
	CC_DECLARE_DYNCREATE( OpDisplayFrameGallery );

public:
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
    void 			Do(OpDescriptor*);
};  
#endif // EXCLUDE_FROM_XARALX

#endif	// INC_LAYERGAL

