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


// Implementation-specific definitions for fontbase.cpp. Not meant to
// included by any other file.

#include "fontbase.h"
#include <map>

/********************************************************************************************
>	class FontMetricsCacheEntry : public CC_CLASS_MEMDUMP
	   		
	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/1/96
	Purpose:	Holds a cache of char widths, ascent, descent and em for a particular font.
********************************************************************************************/

class FontMetricsCacheEntry: public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(FontMetricsCacheEntry);
	
public:
	FontMetricsCacheEntry();

	BOOL CacheFontMetrics(wxDC* pDC, CharDescription FontDesc, MILLIPOINT DefaultHeight, INT32 DesignSize);

	void CheckCharWidthsSameAsABCWidths(wxDC* pDC, CharDescription FontDesc);	// debug test

	inline static BOOL CharInCacheRange(WCHAR ch) { return (ch>=FIRSTCHAR && ch<=LASTCHAR); }

	inline INT32 GetCharWidthFromCache(WCHAR ch) { return pCharWidths[ch-FIRSTCHAR]; }

	inline MILLIPOINT      GetFontEmWidth() { return FontEmWidth; }
	inline MILLIPOINT      GetFontAscent()  { return FontAscent; }
	inline MILLIPOINT      GetFontDescent() { return FontDescent; }
	inline CharDescription GetFontDesc()    { return FontDesc; }

	inline void SetFontEmWidth(MILLIPOINT NewFontEmWidth) { FontEmWidth = NewFontEmWidth; }
	inline void SetFontAscent( MILLIPOINT NewFontAscent)  { FontAscent  = NewFontAscent; }
	inline void SetFontDescent(MILLIPOINT NewFontDescent) { FontDescent = NewFontDescent; }
	inline void SetFontDesc(CharDescription NewFontDesc)  { FontDesc    = NewFontDesc; }

protected:
	// this seems the only way to define constants in a class assigning values
	enum CharRange { FIRSTCHAR=32, LASTCHAR=127, NUMCHARS=(LASTCHAR-FIRSTCHAR+1) };

	MILLIPOINT pCharWidths[NUMCHARS];
	MILLIPOINT FontEmWidth;
	MILLIPOINT FontAscent;
	MILLIPOINT FontDescent;
	CharDescription FontDesc;
};

/********************************************************************************************
>	class FontMetricsCache : public CC_CLASS_MEMDUMP
	   		
	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/1/96
	Purpose:	Holds a cache of char widths, ascent, descent and em for a particular font.
********************************************************************************************/

class FontMetricsCache: public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(FontMetricsCache);
	
public:
	static void InvalidateCharMetrics();
	static BOOL GetCharMetrics(wxDC* pDC, WCHAR ch, CharDescription& FontDesc, CharMetrics* pCharMetrics);

protected:
	// this seems the only way to define constants in a class assigning values
	enum CacheInfo { NUMENTRIES=3 };

	static FontMetricsCacheEntry mpFontMetricsData[NUMENTRIES];
};


/********************************************************************************************

>	class CharOutlineCache : public CC_CLASS_MEMDUMP
	   		
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/95
	Purpose:	A cache for a character outline. This cache can only be accessed via the
				OILFontMan character outline calls

********************************************************************************************/

class CharOutlineCache : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(CharOutlineCache);
	
	friend class OILFontMan;
	
	private:
		#if _DEBUG
		static void Dump();
		#endif

		static DocCoord CacheCoords[OILFONTLIMIT];
		static PathVerb CacheVerbs[OILFONTLIMIT];
		static UINT32 CacheSize;
		static DocCoord LastMoveTo;
};

// Forward decleration
struct MillipointKerningPair;

/********************************************************************************************
>	class FontKerningPairsCacheEntry : public CC_CLASS_MEMDUMP
	   		
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/2000
	Purpose:	Holds a cache of kerning pairs for a font
********************************************************************************************/

class FontKerningPairsCacheEntry : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(FontKerningPairsCacheEntry);
	
public:
	FontKerningPairsCacheEntry();		// constructor
	~FontKerningPairsCacheEntry();		// destructor

	void Reinitialise(CharDescription& NewFontDesc);

	MILLIPOINT GetCharsKerning(WCHAR chLeft, WCHAR chRight);
	inline CharDescription GetFontDesc()					{ return FontDesc; }
	inline void SetFontDesc(CharDescription NewFontDesc)	{ FontDesc = NewFontDesc; }

#ifdef _DEBUG
public:
	void Dump();
#endif //_DEBUG

protected:
	CharDescription FontDesc;

protected: // Kerning data
	std::map<UINT32,INT32>* m_pPairsCacheMap;

private:
	// declare private copy constructor and assignment operator to prevent
	// copy/assignment operations - the methods are not implemented
    FontKerningPairsCacheEntry(const FontKerningPairsCacheEntry&);
	FontKerningPairsCacheEntry& operator=(const FontKerningPairsCacheEntry&);
};


/********************************************************************************************

>	class FontKerningPairsCache : public CC_CLASS_MEMDUMP
	   		
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/2000
	Purpose:	A cache for the kerning pairs for a font

********************************************************************************************/

class FontKerningPairsCache : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(FontKerningPairsCache);
	
public:
	static MILLIPOINT GetCharsKerning(wxDC* pDC, WCHAR chLeft, WCHAR chRight,
									  CharDescription& FontDesc);

#ifdef _DEBUG
public:
	static void Dump();
#endif//_DEBUG

protected:
	// this seems the only way to define constants in a class assigning values
	enum CacheInfo { NUMENTRIES=3 };

	static FontKerningPairsCacheEntry m_FontKerningPairsCacheData[NUMENTRIES];
};

