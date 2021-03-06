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

// implementation file for Camelots Font Managler, supposidly

/*
*/

#ifndef INC_FONTMAN
#define INC_FONTMAN

#include "fontbase.h"
// #include "ccpanose.h" - included in fontbase.h

#define DEFAULTFONTNAME _T("Times New Roman")
#define DEFAULTHANDLE	0
#define ILLEGALFHANDLE	0x7FFF
#define FIRSTFONTHANDLE 1

#define HNDLINVALID(handle) ((handle)==(ILLEGALFHANDLE))
#define HNDLVALID(handle) ((handle)!=(ILLEGALFHANDLE))


/********************************************************************************************

>	class CachedFontItem : public ListItem
	   		
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/95
	Purpose:	A font item cached by the font manager
	SeeAlso:	

********************************************************************************************/

class CachedFontItem : public ListItem
{
	friend class FontManager;
	friend class FontComponent;

	CC_DECLARE_DYNCREATE( CachedFontItem )

	public:
		 CachedFontItem();
		~CachedFontItem();

		String_64* 				GetFontName();
		WORD					GetFontHandle() { return Handle; }
		FontClass				GetFontClass();
		BOOL 					Compare(String_64* pFontName);
		ENUMLOGFONT*			GetEnumLogFont();
		OUTLINETEXTMETRIC*		GetOutlineTextMetric();

		void					SetIsCorrupt(BOOL flag) { FlagIsCorrupt = flag; };
		void					SetIsReplaced(BOOL flag) { FlagIsReplaced = flag; };

		BOOL					IsFullyCached();
		BOOL					IsCorrupt() { return FlagIsCorrupt; }
		BOOL					IsValid() { return IsFullyCached(); };
		BOOL					IsReplaced() { return FlagIsReplaced; };
		

 	private:
		BOOL		 			SetCacheData(ENUMLOGFONT* lpelf);
		void 					DeleteMetrics();
		void					DeleteFontClass();
		void					Delete();
		void					Dump();

	private:
 		FontBase*				pFontClass;						// The class of font (TRUETYPE, ATM etc)
		WORD					Handle;							// The allocated font handle
		ENUMLOGFONT*			pEnumLogFont;					// A description of the font
		OUTLINETEXTMETRIC*		pOutlineTextMetric;
		BOOL					FlagIsCorrupt;
		BOOL					FlagIsReplaced;
};

/********************************************************************************************

>	class FontManager : public CCObject
	   		
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/95
	Purpose:	The font manager controlling class
	SeeAlso:	This font manager class acts as both a cache of used fonts in the various
				documents loaded into Camelot and an interface to the various font managers
				available to us.

********************************************************************************************/

class FontManager :	public CCObject
{
	CC_DECLARE_DYNCREATE( FontManager )
	
	public:
		 FontManager();
		~FontManager();

//**************************************************************************************
// The Font Manager interface. Font based calls
		
		BOOL 			Init();
		BOOL			LegalFontClass(INT32 Class, FontClass& RealClass);

		WORD			CacheNamedFont(String_64 *FontName, FontClass Class=FC_UNDEFINED);
		CachedFontItem* AddFont(String_64* Name, FontClass Class, WORD& Handle);

		CachedFontItem*		GetFont();
		CachedFontItem*		GetFont(WORD Handle);
		CachedFontItem*		GetFont(String_64 *pFontName, FontClass Class=FC_UNDEFINED);
		WORD				GetFontHandle(String_64* pFontName, FontClass Class=FC_UNDEFINED);
		String_64*			GetFontName(WORD Handle);
		BOOL				GetFontName(WORD Handle, String_64& OutputString);
		FontClass			GetFontClass(WORD Handle);
		ENUMLOGFONT*		GetEnumLogFont(WORD Handle);
		OUTLINETEXTMETRIC*	GetOutlineTextMetric(WORD Handle);

		CachedFontItem* GetFirstFont();
		CachedFontItem* GetNextFont(CachedFontItem* pItem);
		CachedFontItem* GetCachedFont(WORD Handle);

		INT32			DoesFontExist(String_64 *pFontName, BOOL SearchCache, BOOL SearchInstalled);
		BOOL			IsFontValid(WORD Handle);
		BOOL			IsFontInstalled(String_64* pFontName, FontClass Class=FC_UNDEFINED);
		BOOL			IsFontDefault(WORD Handle);
		BOOL			IsFontDefault(CachedFontItem* pItem);
		BOOL			IsFontReplaced(WORD Handle);
		BOOL 			IsFontReplaced(String_64* pFontName, FontClass Class=FC_UNDEFINED);

		BOOL            IsFontUsedInDoc(WORD Handle, Document* pDocument);
		void            OnDocumentLoaded(Document* pDocument);

		void			GetCompatibleFont(const String_64& EncodedName, String_64& CompatibleFont, INT32& Style);
		void			EncodeFontName(String_64& FontName, String_64& Encoded, INT32 Styles);
		void			EncodeAndMapFontName(String_64& FontName, String_64& Encoded, INT32 Styles);
		INT32			DecodeFontName(const String_64& IStringRef, String_64& OStringRef);

		void			ClearTempFont();
		void			SetTempFont(FontClass Class, String_64* pFontName, ENUMLOGFONT* lpelf);
		BOOL 			TempFontValid();

		void			RefreshCache();
		void			ValidateItem(FontClass Class, String_64* pFontName, ENUMLOGFONT *lpelf);

		void			DumpFontCache();
		void			DumpFont(WORD Handle);
		
//**************************************************************************************
// Font manager - font based calls used by the new file format
public:
		WORD FindTypeface(String_64 &TypeFaceName, FontClass Class);
		WORD FindClosestFont(const CCPanose &PanoseNumber);
		// the next function needs to be called from a callback function, hence it's public.
		// however, it shouldn't be used by anyone else.
		BOOL FindClosestFontFullTry(FontClass Class, String_64 *pTypeFaceName, ENUMLOGFONT *pEnumLogFont);
private:
		WORD FindClosestFontFull(const CCPanose &PanoseNumber);
		WORD FindClosestFontQuick(const CCPanose &PanoseNumber);
		BOOL FindClosestFontQuickTry(WORD FontHandle);
		WORD FindClosestFontHandle;
		CCPanose FindClosestPanoseNumber;
		UINT32 FindClosestDistance;
		String_64 FindClosestTypeFaceName;
		FontClass FindClosestFontClass;
		ENUMLOGFONT FindClosestEnumLogFont;

//**************************************************************************************
// Character based calls
public:
		void			InvalidateCharMetrics();
		BOOL 			GetCharMetrics(CNativeDC* pDC, WCHAR ch, CharDescription& FontDesc,
											CharMetrics* pCharMetrics); // jwp 15/10/2000
		MILLIPOINT		GetCharsKerning(CNativeDC* pDC, WCHAR chLeft, WCHAR chRight,
											CharDescription& FontDesc);
		BOOL			GetCharPath(CharDescription& ChDesc,
							  		DocCoord** ppCoords,
							  		PathVerb** ppVerbs,
							  		UINT32* pNumCoords,
							  		CNativeDC* pDC=NULL);

//**************************************************************************************


	private:
		CachedFontItem* FindFont(WORD Handle);
		CachedFontItem* FindFont(String_64* pFontName, FontClass Class=FC_UNDEFINED);

		WORD 			GetNextHandle();
		CachedFontItem* GetFirstFontType(FontClass fclass);
		BOOL 			IsFullyCached(WORD Handle);
		BOOL 			TempCacheNamedFont(String_64* pFontName, FontClass=FC_UNDEFINED, INT32 Pass=1);
		BOOL 			CacheDefaultFont();
		CachedFontItem* AddTempFont(String_64* pFontName, FontClass Class, WORD& hndle);
		void			InvalidateCache();
		void			ResetDefaultFont();
		BOOL            IsFontUsedInSiblings(Node* pNode, WORD Handle, WORD CurrentHandle, UINT32 Level);

	private:
		WORD 			UniqueHandle;
		WORD			DefaultHandle;
		List 			TheFontList;
		CachedFontItem	DefaultFontItem;

	private:
		FontClass		TempFontClass;
		String_64		TempFontName;
		ENUMLOGFONT		TempEnumLogFont;
};

/********************************************************************************************

>	class EnumFonts : public OILEnumFonts
	   		
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/95
	Purpose:	This class allows kernel level items to enumerate fonts. 
				To use the class simply derive another from it and implement your own
				NewFont function. If you then call pMyEnumObj->Execute() your
				NewFont function will be called for every font the font manager and oil
				layer support.
	SeeAlso:	

********************************************************************************************/

class EnumAllFonts : public OILEnumFonts
{
	CC_DECLARE_DYNCREATE( EnumAllFonts )
	
	public:
		EnumAllFonts() {};
		virtual BOOL NewFont(FontClass Class, ENUMLOGFONT FAR* lpelf);
};

#endif
