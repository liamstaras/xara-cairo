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


/*
*/

#include "camtypes.h"

#include "dll_lib.h"
#include "viewmod.h"



// Base new DLLs on this file.

extern "C" void * MODULE_FN CreateModule(UINT32 Type)
{
	switch (Type)
	{
		case 1:
			return new ViewModule;
		// Add further moduless here...
	}
	
	return NULL;
}


// This function is used when getting info on all the modules that are around (app startup).

extern "C" UINT32 MODULE_FN GetNumberOfModules()
{
	return 1; // Increase this as you add more modules.
}


