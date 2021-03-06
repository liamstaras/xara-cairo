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


// Implementation of the Layer properties tab of the layer properties dialog box

/*

*/

#include "camtypes.h"

// WEBSTER - markn 15/1/97
// Don't need this tab in Webster
//#ifndef WEBSTER

#include "layer.h"
#include "layerprp.h"
//#include "lyrprop.h"	// dialog/gadget ids
//#include "nev.h"		// error messages
#include "prpslyrs.h"
#include "sglayer.h"
#include "sprdmsg.h"


CC_IMPLEMENT_DYNAMIC(LayerPropertiesTab, LayerPropertyTabs)   

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe

/********************************************************************************************

>	LayerPropertiesTab::LayerPropertiesTab() 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/09/95
	Inputs:		-	
	Outputs:	-
	Returns:	-
	Purpose:	LayerPropertiesTab constructor. Creates a non-undoable operation
	Errors:		-
	SeeAlso:	LayerPropertyTabsDlg; LayerPropertiesTab;

********************************************************************************************/

LayerPropertiesTab::LayerPropertiesTab()
{   
	// does nothing at present
}   	     

/********************************************************************************************

>	LayerPropertiesTab::~LayerPropertiesTab()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/09/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	LayerPropertiesTab destructor
	Errors:		-
	SeeAlso:	LayerPropertyTabsDlg; LayerPropertyTabs;

********************************************************************************************/
																				
LayerPropertiesTab::~LayerPropertiesTab()
{   
}        

/********************************************************************************************

>	BOOL LayerPropertiesTab::Init()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/09/95
	Inputs:		-
	Outputs:	-
	Returns:	True if initialised ok, False if failed.
	Purpose:	LayerPropertiesTab initialisation routine
	Errors:		-
	SeeAlso:	LayerPropertyTabsDlg::Init; LayerPropertyTabs::Init;

********************************************************************************************/
																				
BOOL LayerPropertiesTab::Init()
{   
	return TRUE;
}        

/******************************************************************************************

>	CDlgResID LayerPropertiesTab::GetPageID()()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/09/95
	Inputs:		-
	Outputs:	-
	Returns:	The dialog ID of this tab section.	  
	Purpose:	Allows the options dialog code to determine the dialog ID of this section.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

CDlgResID LayerPropertiesTab::GetPageID()
{
	return _R(IDD_TAB_LAYER_PROPERTIES);
}

/******************************************************************************************

>	BOOL LayerPropertiesTab::IsPropertyRequired()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/09/95
	Returns:	Returns True if this tab is a program option.	  
	Purpose:	Allows the layer propertiesto determine if this tab is required.

******************************************************************************************/

BOOL LayerPropertiesTab::IsPropertyRequired()
{
	return TRUE;	// This tab is required 
}


/******************************************************************************************

>	BOOL LayerPropertiesTab::CommitSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> & Markn
	Created:	28/09/95
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in this tab of the layer properties dialog box.
				Called when ok or apply now is pressed on the main dialog box.
	Errors:		-
	SeeAlso:	LayerPropertiesTab::InitSection()

******************************************************************************************/

BOOL LayerPropertiesTab::CommitSection()
{
TRACEUSER( "Neville", _T("LayerPropertiesTab::CommitSection\n"));
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"LayerPropertiesTab::CommitSection called with no dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;			// Talk to page failed to return now

	Layer* pLayer = GetActiveLayer();

	// Only do the op if we have a layer with a different set of properties
	if (pLayer != NULL && HavePropertiesChanged(pLayer))
	{
		// Initialise the param structure
		OpLayerGalParam Param(LAYER_CHANGE, pSpread);
		Param.pLayer = pLayer;

		Param.VisibleState  =  pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_LAYERTAB_VISIBLE));
		Param.LockedState   = !pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_LAYERTAB_EDITABLE));
		Param.NewName		=  pPropertiesDlg->GetStringGadgetValue(_R(IDC_LAYERTAB_NAME),NULL);

		// Invoke the operation
		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_LAYERGALCHANGE); 
		if (pOpDesc != NULL)
			pOpDesc->Invoke((OpParam*)&Param);
		else
		{
			ERROR3("Couldn't find OPTOKEN_LAYERGALCHANGE op descriptor");
		}
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL LayerPropertiesTab::GreySection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> & Markn
	Created:	28/09/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has closed all documents 
	Errors:		-
	SeeAlso:	LayerPropertiesTab::UngreySection;

********************************************************************************************/

BOOL LayerPropertiesTab::GreySection()
{
TRACEUSER( "Neville", _T("GreySection in LayerPropertiesTab section\n"));
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"LayerPropertiesTab::GreySection called with no dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;	// Talk to page failed to return now

	// Only update if we are not already grey 
	if (GreyStatus == TRUE)
		return TRUE;

	// Call our central greying/ungreying function
	ok = ChangeControlStatus(FALSE);
	
	GreyStatus = TRUE;

	return ok;
} 

/********************************************************************************************

>	BOOL LayerPropertiesTab::UngreySection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> & Markn
	Created:	28/09/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has selected a new document 
	Errors:		-
	SeeAlso:	LayerPropertiesTab::UngreySection;

********************************************************************************************/

BOOL LayerPropertiesTab::UngreySection()
{
TRACEUSER( "Neville", _T("UngreySection in LayerPropertiesTab section\n"));
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"LayerPropertiesTab::UngreySection called with no dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());	// The LayerPropertiesTab identifier
	if (!ok)
		return TRUE;	// Talk to page failed to return now

	// Only update if we are not already ungrey 
	if (GreyStatus == FALSE)
		return TRUE;

	// Call our central greying/ungreying function
	ok = ChangeControlStatus(TRUE);
	
	GreyStatus = FALSE;

	return ok;
}

/********************************************************************************************

>	BOOL LayerPropertiesTab::ChangeControlStatus(const BOOL Status)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/09/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called to grey/ungrey all controls on this tab. 
	Errors:		-
	SeeAlso:	LayerPropertiesTab::GreySection;	LayerPropertiesTab::UngreySection;

********************************************************************************************/

BOOL LayerPropertiesTab::ChangeControlStatus(const BOOL Status)
{
	BOOL Enable;

	Layer* pLayer = GetActiveLayer();
	Enable = (pLayer != NULL) && Status;

	BOOL EnableNonFrameItem = Enable;
	//It shouldn't be in Webster builds anyway but we will take it out jus tin case
#ifndef WEBSTER
	// Check if we are on a frame layer or not
	// If we are then we must disable some items
	// At present, these are the visible and editable flags as these are not user controlled.
//	BOOL FrameLayer = FALSE;
	if (pLayer != NULL && pLayer->IsFrame())
		EnableNonFrameItem = FALSE;
#endif

	pPropertiesDlg->EnableGadget(_R(IDC_GUIDETAB_GUIDELINELIST),Enable);
	pPropertiesDlg->EnableGadget(_R(IDC_LAYERTAB_NAME),			Enable);
	pPropertiesDlg->EnableGadget(_R(IDC_LAYERTAB_VISIBLE),		EnableNonFrameItem);
	pPropertiesDlg->EnableGadget(_R(IDC_LAYERTAB_EDITABLE),		EnableNonFrameItem);

	return TRUE;
}			

/********************************************************************************************

>	BOOL LayerPropertiesTab::UpdateSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>  & Markn
	Created:	28/09/95
	Returns:	-
	Purpose:	Called when we have switched to a new document and need to update all the 
				controls on this tab. Should only init the tab if the page is present.  
				The document name allows the info field on the tab to be filled in correctly.
	Errors:		-
	SeeAlso:	LayerPropertiesTab::GreySection; LayerPropertiesTab::UngreySection; LayerPropertiesTab::InitSection;

********************************************************************************************/

BOOL LayerPropertiesTab::UpdateSection()
{
TRACEUSER( "Neville", _T("LayerPropertiesTab::UpdateSection\n"));
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"LayerPropertiesTab::UpdateSection called with no dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;		// page not present

	ShowDetails();

	// We are happy with what happened
	return TRUE;
}

/********************************************************************************************

>	BOOL LayerPropertiesTab::ShowDetails()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Shows the details of the associated layer in this tab
	Errors:		-
	SeeAlso:	LayerPropertyTabsDlg; LayerPropertiesTab;

********************************************************************************************/

BOOL LayerPropertiesTab::ShowDetails()
{
	String_256 LayerID;

	Layer* pLayer = GetActiveLayer();
	if (pLayer != NULL)
	{
		LayerID = pLayer->GetLayerID();

//		pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_LAYERTAB_VISIBLE),pLayer->IsVisible());
//		pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_LAYERTAB_EDITABLE),!pLayer->IsLocked());
		pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_LAYERTAB_VISIBLE),pLayer->GetVisibleFlagState());
		pPropertiesDlg->SetBoolGadgetSelected(_R(IDC_LAYERTAB_EDITABLE),!pLayer->GetLockedFlagState());
	}

	pPropertiesDlg->SetStringGadgetValue(_R(IDC_LAYERTAB_NAME), LayerID);

	ChangeControlStatus(!GreyStatus);

	return TRUE;
}

/********************************************************************************************

>	BOOL LayerPropertiesTab::UpdateLayerSection()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Called when the layer-related section of the tab should be updated.
	Errors:		-
	SeeAlso:	LayerPropertyTabsDlg; LayerPropertyTabs;

********************************************************************************************/

BOOL LayerPropertiesTab::UpdateLayerSection()
{
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"LayerPropertiesTab::UpdateLayerSection() called with no dialog pointer");
	return (pPropertiesDlg->TalkToPage(GetPageID()) && ShowDetails());
}

/********************************************************************************************

>	BOOL LayerPropertiesTab::HandleMsg(DialogMsg* Msg)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> & Markn
	Created:	28/09/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the messages for this layer property tab
	Errors:		-
	SeeAlso:	LayerPropertyTabsDlg; LayerPropertiesTab;

********************************************************************************************/

BOOL LayerPropertiesTab::HandleMsg(DialogMsg* Msg)
{
TRACEUSER( "Neville", _T("LayerPropertiesTab::HandleMsg\n"));
	ERROR2IF(Msg == NULL,FALSE,"LayerPropertiesTab::Message null message received");
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"LayerPropertiesTab::HandleMsg called with no dialog pointer");

	BOOL ok = pPropertiesDlg->TalkToPage(GetPageID());
	if (!ok)
		return TRUE;		// no page present = handled message
	
	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:	// Initialise controls
			GreyStatus = FALSE;	// we are ungreyed by default
			ok = InitSection();
			if (!ok)
				InformError();
			break;
		case DIM_LFT_BN_CLICKED:
		{
			UpdateApplyState();
			//switch (Msg->GadgetID)
			//{
			//}
		}
		break;

		case DIM_SELECTION_CHANGED:
		case DIM_TEXT_CHANGED:
			UpdateApplyState();
			break;

		default:
			break;
	}
	return TRUE;
}  

/******************************************************************************************

>	BOOL LayerPropertiesTab::UpdateApplyState()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	This changes the 'Apply now' state based on the settings of the active layer
				and the settings in this tab.
				If the settings are the same, the 'Apply state' is FALSE, else it are TRUE
	Errors:		-
	SeeAlso:	LayerPropertiesTab::HandleMsg()

******************************************************************************************/

BOOL LayerPropertiesTab::UpdateApplyState()
{
	BOOL OldApplyNowState = LayerPropertiesTab::GetApplyNowState();

	Layer* pLayer = GetActiveLayer();

	if (pLayer != NULL)
		LayerPropertiesTab::SetApplyNowState(HavePropertiesChanged(pLayer));

	BOOL NewApplyNowState = LayerPropertiesTab::GetApplyNowState();

	if (OldApplyNowState != NewApplyNowState)
	{
		if (NewApplyNowState)
			LayerPropertiesTab::UngreyApplyNow();
		else
			LayerPropertiesTab::GreyApplyNow();
	}

	return TRUE;
}

/******************************************************************************************

>	BOOL LayerPropertiesTab::HavePropertiesChanged(Layer* pLayer)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/95
	Inputs:		pLayer = ptr to a layer
	Outputs:	-
	Returns:	TRUE if settings in the dlg are different to the given layer's settings, else FALSE
	Purpose:	Central place for comparing settings in the dlg against the settings of a layer
	Errors:		-
	SeeAlso:	LayerPropertiesTab::CommitSection()

******************************************************************************************/

BOOL LayerPropertiesTab::HavePropertiesChanged(Layer* pLayer)
{
	BOOL 		VisibleState=  pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_LAYERTAB_VISIBLE));
	BOOL 		LockedState	= !pPropertiesDlg->GetBoolGadgetSelected(_R(IDC_LAYERTAB_EDITABLE));
	String_256 	Name		=  pPropertiesDlg->GetStringGadgetValue(_R(IDC_LAYERTAB_NAME),NULL);

//	BOOL 		CurVisible 	= pLayer->IsVisible();
//	BOOL 		CurLocked  	= pLayer->IsLocked();
	BOOL 		CurVisible 	= pLayer->GetVisibleFlagState();
	BOOL 		CurLocked  	= pLayer->GetLockedFlagState();
	String_256 	CurName 	= pLayer->GetLayerID();

	return ((VisibleState != CurVisible) 	|| 
			(LockedState	!= CurLocked)	||
			(Name			!= CurName));	
}

/******************************************************************************************

>	BOOL LayerPropertiesTab::InitSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> & Markn
	Created:	28/09/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for this tab on the layer properties dialog box.
	Errors:		-
	SeeAlso:	LayerPropertyTabsDlg; LayerPropertiesTab;

******************************************************************************************/

BOOL LayerPropertiesTab::InitSection()
{
TRACEUSER( "Neville", _T("LayerPropertiesTab::InitSection\n"));
	ERROR2IF(pPropertiesDlg == NULL,FALSE,"LayerPropertiesTab::InitSection called with no dialog pointer");

	ShowDetails();

	return TRUE;
}

//#endif // WEBSTER
