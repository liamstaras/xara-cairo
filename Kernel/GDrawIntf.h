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


#ifndef INC_GDRAWASM
#define	INC_GDRAWASM

#include "gdraw.h"
#include "gdrawcon.h"

class CriticalSection
{
public:
	CriticalSection()
	{
#ifdef RALPH
		Temp = 0;
		::EnterCriticalSection(&sm_Section);
#endif
	}

	~CriticalSection()
	{
#ifdef RALPH
		::LeaveCriticalSection(&sm_Section);
#endif
	}

	static void Init()
	{
#ifdef RALPH
		if (!sm_bInit)
		{
			::InitializeCriticalSection(&sm_Section);
			sm_bInit = TRUE;
		}
#endif
	}

	static void DeInit()
	{
#ifdef RALPH
		if (sm_bInit)
		{
			::DeleteCriticalSection(&sm_Section);
			sm_bInit = FALSE;
		}
#endif
	}


private:
	DWORD Temp;
	static CRITICAL_SECTION sm_Section;
	static BOOL sm_bInit;
};

/*******************************************************************************************

>	class GDrawAsm : public GDrawContext

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Purpose:	A C++ wrapped around the assembler in GDRAW.DLL.

				The caller should only ever need one instance of one of these (really its
				inherited brothers) and keep it around for the duration of the program.
				The keeper of the pointer to this is GRenderRegion.

				If real GDraw is not available (e.g. Win16) then this returns FALSE
				from the Init function and all other member functions return failure codes.

	SeeAlso:	GDrawContext

********************************************************************************************/ 


class GDrawAsm : public GDrawContext
{
public:

	GDrawAsm();
	GDrawAsm(DWORD ThreadID);
	~GDrawAsm();

	virtual BOOL Init();
	virtual BOOL Init(GDrawAsm* pSource);

	virtual eError GetLastError();

	virtual INT32 GetGraduationTableSize();
	virtual INT32 GetGraduationTableLength();
	virtual INT32 GetLongGraduationTableSize();
	virtual INT32 GetLongGraduationTableLength();
	virtual INT32 GetTransparentGraduationTableSize();
	virtual INT32 GetTransparentGraduationTableLength();
	virtual INT32 GetLongTransparentGraduationTableSize();
	virtual INT32 GetLongTransparentGraduationTableLength();

	virtual void SetStackLimit( LPVOID );
	virtual void SetStackSize( DWORD );

	virtual BOOL SetupBitmap( INT32 Width, INT32 Height, INT32 Depth, LPBYTE Bits, DWORD Format, BOOL DoBiCompression = FALSE );
	virtual BOOL SetBitmap( CONST LPBITMAPINFOHEADER BitmapInfo, CONST LPBYTE Bitmap, DWORD BitmapFormat = 2);
	virtual BOOL SetMatrix( GMATRIX* );
	virtual void SetAntialiasFlag( BOOL );
	virtual void SetAntialiasQualityFlag( BOOL );
	virtual void SetTileSmoothingFlag( BOOL );
	virtual void SetTileFilteringFlag( BOOL );
	virtual void SetMaxFilterSize( UINT32 );
	virtual BOOL Sharpen( INT32 );
	virtual BOOL Blur( INT32 );
	virtual void SetHintingFlag( BOOL );
	virtual void SetFlatness( DWORD );
	virtual void TransformPath( LPPOINT, LPPOINT, DWORD, GMATRIX* );

	virtual INT32 MaxScale( CONST GMATRIX *Matrix, CONST RECT *BBox );

	// colour
	virtual void SetHalftoneOrigin( DWORD, DWORD );
	virtual BOOL SetDitherStyle(DitherStyle Style = DITHER_GREY_ORDERED);
	virtual BOOL AddToGraduationTable( COLORREF, BOOL, GraduationTable*, DWORD );
	virtual BOOL BuildGraduationTable( COLORREF, COLORREF, DWORD, GraduationTable* );
	virtual BOOL BuildTransparencyTable( DWORD, DWORD, TransparentGradTable* );
	virtual BOOL SetColour( COLORREF );
	virtual BOOL SetSolidColour( COLORREF, DWORD, DWORD);
	virtual BOOL SetWordColour( DWORD );
	virtual BOOL SetTransparency( COLORREF, TransparencyEnum );
	virtual BOOL ConvertBitmap( LPBITMAPINFOHEADER, LPBYTE, LPBITMAPINFOHEADER, LPBYTE, INT32 );
	virtual pcLOGPALETTE SelectPalette( INT32 );
	virtual BOOL SetGraduation(
		DWORD Style,
		CONST GraduationTable *Table,
		CONST POINT *PointA,
		CONST POINT *PointB,
		CONST POINT *PointC
	) ;
	virtual BOOL SetTransparentGraduation(
		DWORD Style,
		CONST TranspGradTable *Table,
		CONST POINT *PointA,
		CONST POINT *PointB,
		CONST POINT *PointC
	) ;
	virtual BOOL SetGraduation4(
		DWORD Style,
		CONST GraduationTable *Table,
		CONST POINT *PointA,
		CONST POINT *PointB,
		CONST POINT *PointC,
		CONST POINT *PointD
	);
	virtual BOOL SetTransparentGraduation4(
		DWORD Style,
		CONST TranspGradTable *Table,
		CONST POINT *PointA,
		CONST POINT *PointB,
		CONST POINT *PointC,
		CONST POINT *PointD
	);
	virtual BOOL SetBias(UINT32,double) ;
	virtual BOOL SetGain(UINT32,double) ;
	virtual BOOL SetBrightness(double) ;
	virtual BOOL SetContrast(double) ;
	virtual BOOL SetGamma(double) ;
	virtual BOOL SetPostGamma(double) ;
	virtual BOOL SetSaturation(double) ;
	virtual BOOL SetContone( UINT32, COLORREF, COLORREF );
	virtual BOOL SetInputRange( UINT32, BYTE, BYTE );
	virtual BOOL SetOutputRange( UINT32, BYTE, BYTE );
	virtual BOOL SetBitmapFill( LPBITMAPINFOHEADER, LPBYTE, DWORD, LPPOINT, COLORREF, LPRGBQUAD,
									LPBYTE, LPBYTE, LPBYTE, LPBYTE, DWORD );

	virtual BOOL SetTransparentBitmapFill( LPBITMAPINFOHEADER, LPBYTE, DWORD, LPPOINT, COLORREF, BYTE*, DWORD);

	virtual BOOL SetPerspectiveBitmapFill( LPBITMAPINFOHEADER, LPBYTE, DWORD, LPPOINT, COLORREF, LPRGBQUAD,
											LPBYTE, LPBYTE, LPBYTE, LPBYTE, DWORD );

	virtual BOOL SetPerspectiveTransparentBitmapFill( LPBITMAPINFOHEADER, LPBYTE, DWORD, LPPOINT, COLORREF, BYTE*, DWORD);

	virtual BOOL SetTransparencyLookupTable(CONST BYTE * Table);
	
	// rendering
	virtual BOOL StrokePath( LPPOINT, const BYTE *, UINT32, BOOL, DWORD, CapStyles, JoinStyles, const DashType* );
	virtual BOOL FillPath( LPPOINT, const BYTE *, UINT32, DWORD );
	virtual BOOL FillRectangle( LPRECT );
	virtual BOOL FillPoint( LPPOINT );
	virtual INT32 StrokePathToPath(
		CONST POINT *IPoints,
		CONST BYTE  *ITypes,
		DWORD ILength,
		POINT *OPoints,
		BYTE  *OTypes,
		DWORD OLength,
		BOOL Close,
		DWORD LineWidth,
		CapStyles LineCaps,
		JoinStyles LineJoin,
		CONST DashType *Dash);
	virtual BOOL GetStatistics( LPPOINT, const BYTE *, UINT32, DWORD, STATISTICS *);

	// misc
	virtual BOOL ClipRectangle( LPRECT );
	virtual void ClearChangedBBox();
	virtual BOOL GetChangedBBox( LPRECT );
	virtual BOOL SetChangedBBox( LPRECT );

	virtual COLORREF ConvertHSVtoRGB(COLORREF);
	virtual COLORREF ConvertRGBtoHSV(COLORREF);

	virtual DWORD GetMaxBitmapWidth(void);
	virtual DWORD GetMaxBitmapDepth(void);

	// New GDraw fill types
	virtual BOOL Set3WayGraduation(DWORD Style,
		COLORREF ColourA, COLORREF ColourB, COLORREF ColourD,
		CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointD);

	virtual BOOL Set3WayGraduation4(DWORD Style,
		COLORREF ColourA, COLORREF ColourB, COLORREF ColourD,
		CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointC, CONST POINT *PointD);

	virtual BOOL Set4WayGraduation(DWORD Style,
		COLORREF ColourA, COLORREF ColourB, COLORREF ColourC, COLORREF ColourD,
		CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointD);

	virtual BOOL Set4WayGraduation4(DWORD Style,
		COLORREF ColourA, COLORREF ColourB, COLORREF ColourC, COLORREF ColourD,
		CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointC, CONST POINT *PointD);

	virtual BOOL SetTransparent3WayGraduation(DWORD Style,
		BYTE ValueA, BYTE ValueB, BYTE ValueD,
		CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointD);

	virtual BOOL SetTransparent3WayGraduation4(DWORD Style,
		BYTE ValueA, BYTE ValueB, BYTE ValueD,
		CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointC, CONST POINT *PointD);

	virtual BOOL SetTransparent4WayGraduation(DWORD Style,
		BYTE ValueA, BYTE ValueB, BYTE ValueC, BYTE ValueD,
		CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointD);

	virtual BOOL SetTransparent4WayGraduation4(DWORD Style,
		BYTE ValueA, BYTE ValueB, BYTE ValueC, BYTE ValueD,
		CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointC, CONST POINT *PointD);


	virtual	BOOL SaveContext(GCONTEXT *Context);
	virtual BOOL RestoreContext(CONST GCONTEXT *Context);

	// new ones

	virtual BOOL SetConversionPalette( PLOGPALETTE );

	virtual BOOL SetMiterLimit( DWORD );
	virtual BOOL SetMatrixIntFlag( BOOL );
	virtual BOOL SetDashAdjustmentFlag( BOOL );

	virtual BOOL SetInvert( DWORD );
	virtual BOOL InitialiseWithPalette( const LOGPALETTE* );
	virtual const BYTE* ReturnBrush();
	virtual const BYTE* ReturnBrushRGB();

	virtual BOOL SetSeparationTables(const BGR *CyanSepTable = NULL,
									 const BGR *MagentaSepTable = NULL,
									 const BGR *YellowSepTable = NULL,
									 const BGR *BlackSepTable = NULL,
									 const BYTE *UnderColourRemovalTable = NULL,
									 const BYTE *BlackGenerationTable = NULL);

	virtual BOOL SetBitmapConversionTable( const BGR* BitmapConversionTable = NULL );

	virtual BOOL PlotBitmap( LPBITMAPINFOHEADER, LPBYTE, DWORD, LPPOINT, LPRGBQUAD,
									LPBYTE, LPBYTE, LPBYTE, LPBYTE);

	virtual BOOL PlotPerspectiveBitmap( LPBITMAPINFOHEADER, LPBYTE, DWORD, LPPOINT, LPRGBQUAD,
											LPBYTE, LPBYTE, LPBYTE, LPBYTE);


	virtual INT32 CalcStrokeBBox(
		CONST POINT *IPoints,
		CONST BYTE  *ITypes,
		DWORD ILength,
		LPRECT Rect,
		BOOL Close,
		DWORD LineWidth,
		CapStyles LineCaps,
		JoinStyles LineJoin,
		CONST DashType *Dash);

	virtual BOOL IsOverlap( LPPOINT, const BYTE *, UINT32, DWORD);
	virtual BOOL CalcBBox( LPPOINT, const BYTE *, DWORD, LPRECT, BOOL);

	virtual INT32 MakeRegion( LPPOINT, const BYTE *, UINT32, DWORD, REGION *, UINT32);

	virtual BOOL DeviceClipRectangle( LPRECT );
	virtual BOOL GetDeviceClipRectangle( LPRECT );
	virtual BOOL ClipRegion( const REGION* );
	virtual const REGION* GetClipRegion();

	virtual BOOL HintPath( 
		CONST POINT *Points,
		CONST BYTE  *Types,
		UINT32 Length,
		BOOL Close,
		DWORD LineWidth);

	virtual INT32 MakeUnclippedRegion( 
		CONST POINT *Points,
		CONST BYTE  *Types,
		UINT32 Length,
		DWORD Winding,
		REGION *ORegion,
		UINT32 OLength);

	virtual BOOL FillRegion( CONST REGION *Region, CONST POINT *Offset );

	virtual INT32 SetBevelContrast(UINT32 nContrast);
	virtual INT32 SetBevelLightness(UINT32 nContrast);
	virtual INT32 SetBevelDarkness(UINT32 nContrast);
	virtual INT32 TranslateBevelValue(BYTE nIndex, BYTE Colour);

#if RALPH
	// Function to initialise a context from a static data block

public:
	virtual BOOL InitContext(GCONTEXT *Context, INT32 Index = 0);

#endif

public:
	static GDrawAsm* GetContextForCurrentThread(void);


protected:
	
	// some functions to save and restore context state
	static	INT32 GetContextLength();
	virtual BOOL MakeCurrent();

	// data members
	GCONTEXT* m_pContext;
	
	
	GCONTEXT * SaveArea;
	INT32 SaveAreaLength;
	DWORD m_ThreadID;

	GDrawAsm *m_pNext;
	GDrawAsm *m_pPrevious;

	// static members
	static GDrawAsm * Current;
	static INT32 Instance;

	static GDrawAsm *sm_pListHead;

};
#endif
