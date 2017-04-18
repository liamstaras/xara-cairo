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



#ifndef  Included_CBiasGainDlg
#define  Included_CBiasGainDlg




#include "rnddlgs.h"
#include "transop.h"//"selop.h"
#include "biasgain.h"
#include "biasgdgt.h"
#include "bitmapgriddropdown.h"



#define  OPTOKEN_BIASGAIN_DLG  _T("CBiasGainDlg")
#define  OPTOKEN_OPBIASGAIN    _T("OpBiasGain")




//-------------------------------------------------------------------------------------------------
//
//	class  CBiasGainDlg
//
//	Author:     Harrison Ainsworth
//	Date:		05/99
//	Purpose:	be a dialog for inputing a bias/gain choice. to be owned and called from a 
//				CBiasGainGadget
//	Implemen-	DoWithParam() is supplied with a pointer to the owning gadget (in the OpParam), this 
//	tation:		is mainly used to tell the gadget when the dlg is running, which the gadget uses to 
//				ensure that only one (of its) dlgs is running at a time. So: you can have multiple of
//				these biasgain dialogs running, but each is used for a different purpose and is owned 
//				by a separate gadget.
//
//-------------------------------------------------------------------------------------------------

class  CBiasGainDlg  :  public  DialogOp
{         
	CC_DECLARE_DYNCREATE( CBiasGainDlg )

public: //-----------------------------------------------------------------------------------------

	// standard object services -------------------------------
	
						CBiasGainDlg ();
	
						~CBiasGainDlg ();
	//					CBiasGainDlg ( const CBiasGainDlg& );
	//CBiasGainDlg&		operator= ( const CBiasGainDlg& );

	// --------------------------------------------------------


	// constants ----------------------------------------------
	static const UINT32	IDD;
	static const CDlgMode	Mode;


	// usual interface ----------------------------------------
	static	BOOL		Init ();
	static	OpState		GetState ( String_256*,  OpDescriptor* );

	void				Do ( OpDescriptor* );
						// OwningGadgetParams: param1 is a CBiasGainGadget*, and param2 is a CProfileBiasGain*
	void				DoWithParam ( OpDescriptor*,  OpParam* OwningGadgetParams );
	virtual	MsgResult	Message( Msg* );

	
	// DMc
	// Sets the profile for the dialog, and updates all the controls
	//void SetProfile(CProfileBiasGain const &Profile);

	// CGS
	void InvokeVia (CBiasGainGadget& pInvokeWith, CProfileBiasGain* pInvokeOn, BOOL bMany);
	void ReInitialiseDialog (CProfileBiasGain* ReInitOn, BOOL bMany);
	void DisableAllControls ();
	void EnableAllControls ();
	void ToggleFillProfile () { useFillProfile = TRUE; }
	void ToggleTranspFillProfile () { useTranspFillProfile = TRUE; }
	// we need direct access the dialogs 'edited' biasgain within elans feathering bar ....
	CProfileBiasGain GetCurrentDialogProfile ();

protected: //--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--

	virtual	void		DrawCurve ( const CProfileBiasGain& CurveShape,  ReDrawInfoType* ExtraInfo );
	virtual	void		DrawCurveInterpolate ( const CProfileBiasGain& CurveShape,  ReDrawInfoType* ExtraInfo );
	virtual	void		DrawFillInterpolate ( const CProfileBiasGain& CurveShape,  ReDrawInfoType* ExtraInfo );
	virtual	void		DrawTranspFillInterpolate ( const CProfileBiasGain& CurveShape,  ReDrawInfoType* ExtraInfo );


	// sliders interface --------------------------------------
	virtual	void		InitSliders  ( CProfileBiasGain const& );
	virtual	void		WriteSliders ( CProfileBiasGain const& );
	virtual	void		ReadSliders  ( CProfileBiasGain& );
	virtual void		InitiProfileCombobox();
			void		InitOneSlider( CGadgetID GadgetID );
			void		WriteOneSlider ( AFp ValueMinus1ToPlus1, CGadgetID GadgetID );
			AFp			ReadOneSlider ( CGadgetID GadgetID );
			void		InitBiasGainGadget ( CProfileBiasGain const& );

	static const INT32	kSliderLength_s;
	static const INT32	kSliderHalfLength_s;
	static const INT32	kSliderMin_s;
	static const INT32	kSliderMax_s;


	// edit boxes interface -----------------------------------
	virtual	void		InitEditBoxes  ( CProfileBiasGain const& );
	virtual	void		WriteEditBoxes ( CProfileBiasGain const& );
	virtual	void		ReadEditBoxes  ( CProfileBiasGain& );

private: /////////////////////////////////////////////////////////////

	// functions ----------------------------------------------
	//
	// message handlers
	virtual	void		HandleSliderPosChanging ( CDlgMessage const&,  CProfileBiasGain& );
	virtual	void		HandleSliderPosSet      ( CDlgMessage const&,  CProfileBiasGain& );
	virtual	void		HandleCommit            ( CDlgMessage const&,  CProfileBiasGain& );
	virtual	void		HandleInput             ( CDlgMessage const&,  CProfileBiasGain& );
	virtual	void		HandleBiasEditChange    ( CDlgMessage const&,  CProfileBiasGain& );

	// communicate bias gain values to outside
	virtual void		BroadcastMessage ( CDlgMessage const&,  CProfileBiasGain const& )		const;


	// members ------------------------------------------------
	// 
	// ptr to the owning gadget
	CBiasGainGadget*	pOwningGadget_m;

	// the 'contained' profile gadget		(CGS - 10/1/99)
	CBiasGainGadget BiasGainGadget;

	// identifier for this particular instance
	CGadgetID			Id_m;

	CBitmapGridDropDown* m_pobddStandardProfile;

	// the curve representation
	CProfileBiasGain	BiasGain_m;

	// whether we disabled all controls or not (i.e.  whether we were asked to display MANY
	// profiles
	BOOL manyDisabledControls;

	// whether we should render an object profile or a fill profile internally
	// (by default we render using an object profile)
	BOOL useFillProfile;

	// whether we should render an object profile or a transparency fill profile internally
	// (by default we render using an object profile)
	BOOL useTranspFillProfile;


private:
	// Copying is disallowed
						CBiasGainDlg ( const CBiasGainDlg& );
	CBiasGainDlg&		operator= ( const CBiasGainDlg& );


}; 



#endif//Included_CBiasGainDlg
