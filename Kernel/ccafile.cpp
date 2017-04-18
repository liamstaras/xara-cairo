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




#include "camtypes.h"

#include "ccafile.h"

#include <errno.h>

//#include "mario.h"
//#include "tim.h"

#include "zstream.h"
//#include "nev.h"
#include "ralphdoc.h"
                        

CC_IMPLEMENT_DYNAMIC(CCAsynchDiskFile, CCDiskFile);


PORTNOTE("other","CAM_DEBUG_NEW won't work")
#ifndef EXCLUDE_FROM_XARALX
#define new CAM_DEBUG_NEW
#endif
#define ME "Chris"

/********************************************************************************************

>	asynchstreambuf::~asynchstreambuf()


	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/08/96
	Purpose:	constructor

********************************************************************************************/

asynchstreambuf::asynchstreambuf()
{	
	
	
	DontFail = FALSE;
}

/********************************************************************************************

>	asynchstreambuf::~asynchstreambuf()


	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/08/96
	Purpose:	destructor - clean up event and critical section

********************************************************************************************/

asynchstreambuf::~asynchstreambuf()
{
	
}
/********************************************************************************************

>	asynchstreambuf::::SetLoadContext(LoadContext *pLContext)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/08/96
	Purpose:	attach a load context for this buffer (used to syb#nchorise the read and write 
				threads) and initialise the handle array used in WaitForMulitpleObjects()

********************************************************************************************/

void asynchstreambuf::SetLoadContext(LoadContext *pLContext)
{ 

	pLoadContext= pLContext;
	WakeEvents[0] =	RalphDocument::g_hAbortDownLoadEvent;
	WakeEvents[1] = pLoadContext->hDataAvailableEvent;


}


/********************************************************************************************

>	asynchstreambuf:: seekoff(streamoff sOff,ios::seekdir eDir,INT32 mode)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/08/96
	Inputs:		Standard streambuf
	Purpose:	before the normal seek operation is attempted we test to see whether it is
				possible - if not this thread will wait until a "xsputn" wakes us -

********************************************************************************************/


streampos  asynchstreambuf::seekoff(streamoff sOff,ios::seekdir eDir,INT32 mode)
{

	if(pLoadContext == NULL)
		return 0;

	// test whether this seek can succeed and sleep if not
	// the write thread will wake us with a hDataAvailableEvent
	while(sOff>pLoadContext->TotalBytes && !pLoadContext->AsynchDownLoadComplete)
	{
		
		TRACEUSER( ME, _T("can't seekoff - go to sleep.... \n"));
		
		// note this is an auto-reset event
		// i.e. it will clear after this call returns... 
		DWORD we = WaitForMultipleObjects(2,WakeEvents,FALSE,INFINITE);
		
		// the EndDownLoad event - set the flag
		if(we == WAIT_OBJECT_0)
		{
			pLoadContext->AsynchDownLoadComplete = TRUE;
			TRACEUSER( "Chris", _T("...... Abort thread %x \n"),::GetCurrentThreadId());
		}
		
	}

	// return the absolute file position
	streampos retPos = filebuf::seekoff(sOff,eDir,mode);
	
	pLoadContext->BytesAvailable = pLoadContext->TotalBytes - retPos ;

	TRACEUSER( ME, _T("seekoff to %d Bytes Available = %d\n"),sOff,pLoadContext->BytesAvailable); 

	return retPos ;

}

/********************************************************************************************

>	asynchstreambuf::seekpos(streampos sPos,INT32 mode)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/08/96
	Inputs:		Standard streambuf
	Purpose:	before the normal seek operation is attempted we test to see whether it is
				possible - if not this thread will wait until a "xsputn" wakes us -

********************************************************************************************/

streampos  asynchstreambuf::seekpos(streampos sPos,INT32 mode)
{

	
	if(pLoadContext == NULL)
		return 0;


	// test whether this seek can succeed - sleep if not
	// the write thread will wake us with a hDataAvailableEvent
	while(sPos>=pLoadContext->TotalBytes && !pLoadContext->AsynchDownLoadComplete)
	{
		
		TRACEUSER( ME, _T("can't seekpos - go to sleep.... \n"));
		
		// note this is an auto-reset event
		// i.e. it will clear after this call returns... 
		DWORD we = WaitForMultipleObjects(2,WakeEvents,FALSE,INFINITE);
		
		// the EndDownLoad event - set the flag
		if(we == WAIT_OBJECT_0)
		{
			pLoadContext->AsynchDownLoadComplete = TRUE;
			TRACEUSER( "Chris", _T("...... Abort thread %x \n"),::GetCurrentThreadId());
		}
		
	}

	// return the absolute file position
	streampos retPos = filebuf::seekpos(sPos,mode);
	
	pLoadContext->BytesAvailable = pLoadContext->TotalBytes - retPos;
	
	TRACEUSER( ME, _T("seekpos to %d Bytes Available = %d\n"),sPos,pLoadContext->BytesAvailable); 

	return retPos;
}


/********************************************************************************************

>	CCFile& asynchstreambuf::xsputn(char * c,INT32 n)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/08/96
	Purpose:	the 'write' function - wake the read thread

********************************************************************************************/

INT32  asynchstreambuf::xsputn(const char * c,INT32 n)
{

	
	if(pLoadContext == NULL)
		return 0;


	//for(INT32 i=0;i<n;i++)
	//	TRACEUSER( ME, _T("%c"),c[i]);

	INT32 ret = filebuf::xsputn(c,n);
	
	
	// total available
	pLoadContext->BytesAvailable+=ret;
	
	// absolute total
	pLoadContext->TotalBytes+=ret;

	TRACEUSER( ME, _T("Put TotalBytes = %d BytesAvailable = %d\n"),pLoadContext->TotalBytes,pLoadContext->BytesAvailable); 

	// flush the buffer - otherwise xsgetn might try to read data that is still buffered
	sync();

	// wake the read thread
	SetEvent(pLoadContext->hDataAvailableEvent);

	return ret;
}
/********************************************************************************************

>	CCFile& asynchstreambuf::xsgetn(char * c,INT32 n)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/08/96
	Purpose:	the 'read' function - block if data is not available

********************************************************************************************/

INT32  asynchstreambuf::xsgetn(char * c,INT32 n)
{


	if(pLoadContext == NULL)
		return 0;

/*	if(pLoadContext->BytesAvailable == 0)
	{
		AfxDebugBreak();
	}*/

	TRACEUSER( ME, _T("request %d Get - Bytes Available = %d\n"),n,pLoadContext->BytesAvailable); 

	// test whether this read can succed and sleep if not
	// the write thread will wake us with a hDataAvailableEvent
	while(!DataAvailable(n)&& !pLoadContext->AsynchDownLoadComplete)
	{
		
		TRACEUSER( "Chris", _T("###### Waiting in thread %x \n"),::GetCurrentThreadId());
		
		// note this is an auto-reset event
		// i.e. it will clear after this call returns... 
	
		DWORD we = WaitForMultipleObjects(2,WakeEvents,FALSE,INFINITE);
		
		// the EndDownLoad event - set the flag
		if(we == WAIT_OBJECT_0)
		{
			pLoadContext->AsynchDownLoadComplete = TRUE;
			TRACEUSER( "Chris", _T("...... Abort thread %x \n"),::GetCurrentThreadId());
		}
		
		TRACEUSER( "Chris", _T("###### Wake thread %x \n"),::GetCurrentThreadId());
		
			
	}
	
		
	if(DontFail && pLoadContext->BytesAvailable<n)
	{	
		n = pLoadContext->BytesAvailable;
	
	}

	if(pLoadContext->TotalBytes == 0)
	{
		return n;
	}

	INT32 ret = filebuf::xsgetn(c,n);
	
		

	TRACEUSER( ME, _T("received   %d \n"),ret); 


//	if(ret==1)
//		TRACEUSER( ME, _T("%c"),*c);
 
	
	// decrement  bytes available after the read
	pLoadContext->BytesAvailable -= ret;

	return ret;
}

/********************************************************************************************

>	CCFile& asynchstreambuf::DataNotAvailable()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/08/96
	Purpose:	

********************************************************************************************/

BOOL asynchstreambuf::DataAvailable(INT32 Length)
{	
	

	if(pLoadContext == NULL)
		return 0;

	//TRACEUSER( ME, _T("asking for %d got  = %d\n"),Length,BytesAvailable); 

	//UINT32 BufferAvail = (UINT32)(IOFile->rdbuf()->in_avail());
	if (pLoadContext->BytesAvailable > Length) 
	{
		//	TRACEUSER( ME, _T("YES Data Available !!! \n"));
		return TRUE ;	
	}

	//TRACEUSER( ME, _T("NO Data Available \n"));
	return FALSE;
}


/********************************************************************************************

>	void CCAsynchDiskFile::DownloadHasEnded(HRESULT Status)
	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/08/96
	Purpose:	

********************************************************************************************/

void asynchstreambuf::DownloadHasEnded(HRESULT Status)
{	


	if(pLoadContext == NULL)
		return ;

	// set the exit state - this will be tested in in all the wait loops
	pLoadContext->AsynchDownLoadComplete = TRUE;
	
	TRACEUSER( "Chris", _T("###### Waking thread %x \n"),::GetCurrentThreadId());
		
	
	// wake the read thread
	SetEvent(pLoadContext->hDataAvailableEvent);
}




/********************************************************************************************

										CCCAsynchDiskFile

/********************************************************************************************

>	CCAsynchDiskFile::CCDiskFile(UINT32 bufferSize,
						   BOOL ErrorReporting = TRUE, BOOL ExceptionThrowing = FALSE)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/08/96
	Inputs:		bufferSize	- Size of the fixed size buffer (default = 1024)
							- if size > 0 then create a buffer of the specified size
							- if size = 0 then create a file without a buffer
				ReportErrors flag
				ThrowException flag
	Purpose:	thread safe wrapper around CCDiskFile

********************************************************************************************/

CCAsynchDiskFile::CCAsynchDiskFile(UINT32 bufferSize,
									BOOL ErrorReporting, BOOL ExceptionThrowing)
		:CCDiskFile( new async_fstream(),bufferSize, ErrorReporting,ExceptionThrowing)
{                      
	
}



/********************************************************************************************

										CCCAsynchDiskFile

*/

/********************************************************************************************

>	CCAsynchDiskFile::CCDiskFile(UINT32 bufferSize,
						   BOOL ErrorReporting = TRUE, BOOL ExceptionThrowing = FALSE)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/08/96
	Inputs:		bufferSize	- Size of the fixed size buffer (default = 1024)
							- if size > 0 then create a buffer of the specified size
							- if size = 0 then create a file without a buffer
				ReportErrors flag
				ThrowException flag
	Purpose:	thread safe wrapper around CCDiskFile

********************************************************************************************/

CCAsynchDiskFile::CCAsynchDiskFile(PathName fPath, 
									INT32 fileMode, 
									INT32 fileAccess,
									UINT32 bufferSize,
									BOOL ErrorReporting, 
									BOOL ExceptionThrowing)
		:CCDiskFile(new async_fstream(),
						fPath, 
						fileMode, 
						fileAccess,
						bufferSize,
						ErrorReporting,
						ExceptionThrowing)
{                      

}




/********************************************************************************************

>	CAsynchCDiskFile::~CCAsynchDiskFile()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/08/96
	Purpose:	thread safe wrapper around CCDiskFile

********************************************************************************************/

CCAsynchDiskFile::~CCAsynchDiskFile()
{

}

/********************************************************************************************

>	CCFile& CCAsynchDiskFile::Size()
	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/08/96
	Purpose:	

********************************************************************************************/

INT32 CCAsynchDiskFile::Size()
{	
	// we can't possibly know how big the file will be cause we haven't downloaded 
	// it yet - for our purposes we will return hugely big
	return 0xFFFFFFF;
}


/********************************************************************************************

>	BOOL CCAsynchDiskFile::eof() const

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/96
	Purpose:	Checks eof of the file

********************************************************************************************/

BOOL CCAsynchDiskFile::eof() const
{
	return(CCDiskFile::eof());
/*	LoadContext* pLoadContext = ((asynchstreambuf*)(IOFile->rdbuf()))->GetLoadContext();
	if (pLoadContext->AsynchDownLoadComplete)
	{
		return(CCDiskFile::eof());
	}

	return(FALSE);*/
}


/********************************************************************************************

>	CCFile::GotError( UNIT errorID )
>	CCFile::GotError( UNIT errorID , const TCHAR*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/08/94
	Inputs:		Unsigned interger which is the error id and possible error message string
	Outputs:	None
	Returns:	None
	Purpose:	Error handler for the CCFile class. It uses flags set in the constructor
				to determine if the user requires errors to be reported or exceptions
				thrown. 
	Errors:		None
	SeeAlso:	SetReportErrors; SetThrowExceptions

********************************************************************************************/

void CCAsynchDiskFile::GotError( UINT32 errorID , const TCHAR* errorString)
{
	if(DontFail)
	{
		IOFile->clear(0);
		return ;
	}

	ERROR3IF(errorString == 0, "errorString is null. Oh dear.");

	Error::SetError(errorID, errorString, 0);

	// Flag that no more reads/writes should happen by forcing bad() to return True/.
	// So that if the user continues to access the file then they should get an error.
	SetBadState();
	
	// If the ReportErrors flag is set then report the error now.
	if (ReportErrors)
		InformError();

	// If the ThrowExceptions flag is set then throw an exception.
	// Use the CFileException with a generic or unknown error and make the ioserror
	// our error number.
	if (ThrowExceptions)
	{	
		TRACE( _T("CCFile GotError call to throw exception\n"));
		TRACEUSER( "Chris", _T("EXCEPTION!!!!!"));
		//AfxThrowFileException(CFileException::generic, errorID);
	}
}

void CCAsynchDiskFile::GotError( UINT32 errorID )
{
	if(DontFail)
	{
		IOFile->clear(0);
		return ;
	}
	
	// Expand certain types of error messages to make them more informative
	switch (errorID)
	{
		case _R(IDE_OPEN_ERROR):
		case _R(IDE_FILE_READ_ERROR):
		case _R(IDE_FILE_WRITE_ERROR):
			// Make the error message contain handy information.
			SetFileErrorMessage(errorID);
			break;

		case _R(IDE_ZOPEN_ERROR):
		case _R(IDE_ZFILE_READ_ERROR):
		case _R(IDE_ZFILE_WRITE_ERROR):
			// Make the error message contain handy information.
			SetZipFileErrorMessage(errorID);
			break;

		default:
			// Just set the error normally.
			Error::SetError(errorID, 0);
			break;
	}

	// Flag that no more reads/writes should happen by forcing bad() to return True.
	// So that if the user continues to access the file then they should get an error.
	SetBadState();
	
	// If the ReportErrors flag is set then report the error now.
	if (ReportErrors)
		InformError();
	
	// If the ThrowExceptions flag is set then throw an exception.
	// Use the CFileException with a generic or unknown error and make the ioserror
	// our error number.
	if (ThrowExceptions)
	{	
		TRACE( _T("CCFile GotError call to throw exception\n"));
		TRACEUSER( "Chris", _T("EXCEPTION!!!!!"));
	//	AfxThrowFileException(CFileException::generic, errorID);
	}
}



/********************************************************************************************

>	void CCAsynchDiskFile::ExitThread(HRESULT Status);
	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/08/96
	Purpose:	call the contained streambuf function to inform it that the download is over

********************************************************************************************/

void CCAsynchDiskFile::DownloadHasEnded(HRESULT Status)
{	
	// tell the stream buf that the download is complete
	((asynchstreambuf*)(IOFile->rdbuf()))->DownloadHasEnded(Status);

}




/********************************************************************************************
>	async_fstream::async_fstream()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/9/96
********************************************************************************************/

async_fstream::async_fstream()
{
	set_streambuf(new asynchstreambuf);
}


/********************************************************************************************
>	async_fstream::async_fstream(const char * name, INT32 mode, INT32 prot)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/9/96
********************************************************************************************/

async_fstream::async_fstream(const char * name, INT32 mode, INT32 prot)
{
	set_streambuf(new asynchstreambuf);
	open(name,mode,prot);
}


/********************************************************************************************
>	async_fstream::async_fstream(filedesc fd)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/9/96
********************************************************************************************/

async_fstream::async_fstream(filedesc fd)
{
	set_streambuf(new asynchstreambuf);
	attach(fd);
}


/********************************************************************************************
>	async_fstream::async_fstream(filedesc fd, char * sbuf, INT32 len)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/9/96
********************************************************************************************/

async_fstream::async_fstream(filedesc fd, char * sbuf, INT32 len)
{
	set_streambuf(new asynchstreambuf);
	setbuf(sbuf, len);
	attach(fd);
}


/********************************************************************************************
>	async_filebuf* async_fstream::set_streambuf(streambuf* pbuf)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/9/96
********************************************************************************************/

streambuf * async_fstream::set_streambuf(streambuf* pbuf)
{
	if (bp) delete bp;
	return bp = pbuf;
}
