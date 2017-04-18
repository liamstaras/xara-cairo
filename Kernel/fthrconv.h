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


// Implements a convolution on the bitmap pointed to by pSrcInfo
// Allocs and returns a new bitmap, original bitmap is unchanged
// NB may return NULL if failed to alloc memory for the bitmap
LPBITMAPINFO Shadow32BppBitmap(
								   const UINT32 nBlur, 
								   LPBITMAPINFO pSrcInfo, 
								   LPBYTE pSrcBits32, 
								   LPBYTE* pDestBits
							   );

// Optimised version of the above function
// NB Assumes input is already DWORD aligned
LPBITMAPINFO Shadow8BppBitmap(
								   const UINT32 nBlur, 
								   LPBITMAPINFO pSrcInfo, 
								   LPBYTE pSrcBits32, 
								   LPBYTE* pDestBits
							   );
