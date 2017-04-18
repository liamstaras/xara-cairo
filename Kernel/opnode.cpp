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

// Implementation of the node operations

#include "camtypes.h"
#include "opnode.h"	

#include "objchge.h"	
//#include "tmpltres.h"	



CC_IMPLEMENT_DYNAMIC(RemoveAttributeParam, OpParam)
CC_IMPLEMENT_DYNCREATE(OpRemoveAttributeFromSelection, SelOperation)

#define new CAM_DEBUG_NEW


#define ENSURE_NOT_NULL(Pointer)		ERROR2IF(Pointer == NULL, 0, "NULL Args")
#ifndef VOID_ENSURE_NOT_NULL
#define VOID_ENSURE_NOT_NULL(p)		if (p == NULL) {	ERROR2RAW("NULL Args")	}
#endif
#define ENSURE_KIND(pNode, Kind)		ERROR3IF(!pNode->IS_KIND_OF(Kind), "pNode is not kind of " #Kind);

/********************************************************************************************

>	BOOL OpRemoveAttributeFromSelection::Declare()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Will - I wish I hadn't)
	Created:	09/07/97

	Purpose:	Declares the OpDescriptor for OpRemoveAttributeFromSelection. Should be
				called on start up.

	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
********************************************************************************************/
BOOL OpRemoveAttributeFromSelection::Declare()
{
	
	// Register the opdescriptors for the OpRemoveAttributeFromSelection operation

	return (RegisterOpDescriptor(	0,
								 	_R(IDS_DELETEATTROP),
									CC_RUNTIME_CLASS(OpRemoveAttributeFromSelection),
								 	OPTOKEN_DELETEATTR,
								 	OpRemoveAttributeFromSelection::GetState,
 									0,	/* help ID */
 									0,
 									0,
 									0,
									SYSTEMBAR_ILLEGAL,			// For now !
									FALSE, 						// Receive messages
									FALSE,
									FALSE,
									0,
									0 // (GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION)
	       )); 
}               


/********************************************************************************************

>	OpState	OpRemoveAttributeFromSelection::GetState(String_256*, OpDescriptor* OpDesc)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Will - I wish I hadn't)
	Created:	09/07/97

	Purpose:	For finding OpRemoveAttributeFromSelection state.

	Returns:	The state of the OpRemoveAttributeFromSelection operation

	Notes:		Always available cos it ain't in no menu
				
********************************************************************************************/
OpState	OpRemoveAttributeFromSelection::GetState(String_256*, OpDescriptor* OpDesc)
{
	OpState OpSt;

	OpSt.Greyed = ( FALSE );  
												
	return(OpSt);   
}


/********************************************************************************************

>	virtual void OpRemoveAttributeFromSelection::GetOpName(String_256* OpName) 

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Will - I wish I hadn't)
	Created:	09/07/97

	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 

	Outputs:	The undo string for the operation

********************************************************************************************/
void OpRemoveAttributeFromSelection::GetOpName(String_256* OpName) 
{ 
	*OpName += String_256(_R(IDS_ATTR_CHANGE));
}  


/********************************************************************************************

>	virtual void OpRemoveAttributeFromSelection::DoWithParam(OpDescriptor* pOpDesc, OpParam* pOpParam)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Will - I wish I hadn't)
	Created:	09/07/97

	Purpose:	Performs the OpRemoveAttributeFromSelection operation.
				It removes all attributes with the given AttributeIdentifier from the
				selected objects.

	Inputs:		pOpDesc:	See base class
				pOpParam:	A RemoveAttributeParam containing the AttributeIdentifier of
							the attributes to remove.

********************************************************************************************/
void OpRemoveAttributeFromSelection::DoWithParam(OpDescriptor* pOpDesc, OpParam* pOpParam)
{    
	VOID_ENSURE_NOT_NULL(pOpParam);
	ENSURE_KIND(pOpParam, RemoveAttributeParam);

	const RemoveAttributeParam* const pRemoveParam = (RemoveAttributeParam*)pOpParam;

	// Objects used to mark changed nodes, so that parents will update after attr replacement
	ObjChangeFlags cFlags;
	cFlags.Attribute	= TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);

	BOOL Ok = TRUE;

	Ok = DoStartSelOp(FALSE, FALSE, FALSE, FALSE);	// FALSE's == don't render the blobs

	// Obtain the current selection
	SelRange* const pSelRange = GetApplication()->FindSelection();
	Ok = (pSelRange != NULL);

	if (Ok)
	{

		if (Ok)
		{
			Range CurrentSelection(*pSelRange);
			// Mark nodes that will allow this to happen, 
			// and error if no nodes will let it happen
	
			Ok = CurrentSelection.AllowOp(&ObjChange);
		}
	}

	if (Ok)
	{
		AttributeIdentifier AttrIDToDelete = pRemoveParam->GetAttrIDToDelete();

		Node* pNode = pSelRange->FindFirst();
		while (pNode != NULL && Ok)
		{
			NodeAttribute* const pAttr = FindAttributeOfClassID(pNode, AttrIDToDelete);
			if (pAttr != NULL)
			{
				Ok = DoRemove(pAttr);
			}

			pNode = pSelRange->FindNext(pNode);
		}
	}

	if (Ok)
	{
		// Update all parents of this
		ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
		Ok = UpdateChangedNodes(&ObjChange);
	}

	if (!Ok)
	{
		FailAndExecute();
	}

	End(); // End of operation
} 


/********************************************************************************************

>	NodeAttribute* OpRemoveAttributeFromSelection::FindAttributeOfClassID(Node* const pNode, 
																AttributeIdentifier AttrID)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08//07/97

	Purpose:	Support function to find an attribute of the given class

	Inputs:		pNode : The attribute to go
				AttrID:	The AttributeIdentifier of the attribute to find

	Returns:	An attribute of the given class or NULL if none found

********************************************************************************************/
NodeAttribute* OpRemoveAttributeFromSelection::FindAttributeOfClassID(Node* const pNode, AttributeIdentifier AttrID)
{
	NodeAttribute* pAttr = NodeAttribute::FindFirstAppliedAttr(pNode);

	while (pAttr)
	{
		if (pAttr->GetAttributeClassID() == AttrID)
			return pAttr;

		pAttr = NodeAttribute::FindPrevAppliedAttr(pAttr);
	}

	return NULL;
}



/********************************************************************************************

>	BOOL OpRemoveAttributeFromSelection::DoRemove(NodeAttribute* const pOldAttr)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08//07/97

	Purpose:	Support function to remove the attribute

	Inputs:		pOldAttr : The attribute to go
	Returns:	TRUE if successful 
				FALSE if the operation should be aborted (TIDYUP THEN CALL End()!) 

********************************************************************************************/
BOOL OpRemoveAttributeFromSelection::DoRemove(NodeAttribute* const pOldAttr)
{
	ENSURE_NOT_NULL(pOldAttr);		// well use a reference then!

	BOOL Ok = TRUE;

	Node* CurrentNode = pOldAttr->FindParent();
	ERROR3IF(CurrentNode == NULL, "Can't find parent node in Replace Attr");

	// Now we have done with the old attribute, so lets hide it, so
	// the changes can be undone
	Ok = DoHideNode(pOldAttr, 
					TRUE,		// Include the subtree size (not that there is one)
					NULL,		// the hidden node
					TRUE);		// make sure the attribute's HidingNode function is called

	return Ok;
}
