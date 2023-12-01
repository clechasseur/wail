// =================================================================================
//	C_PatchFile.cp					©1998-1999, Charles Lechasseur
// =================================================================================
//
// This file includes a function to create a file using a given FSSpec, and copy some
// data in it. all file information is stored in a "MInf" resource, and the content
// of the file's initial data and resource forks are stored in "MAKE" resources.
//
// this code is copied from the PatchIt! code, ©1998 Charles Lechasseur. modified
// quickly to fit Wail's purpose.
//
// mental note: convert this to a LFile derivate ASAP.
//
// initial creation 99/03/16
	
/* Copyright (c) 2000-2002, Charles Lechasseur
**  All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
** 
** - Redistributions of source code must retain the above copyright notice,
**   this list of conditions and the following disclaimer.
** 
** - Redistributions in binary form must reproduce the above copyright notice,
**   this list of conditions and the following disclaimer in the
**   documentation and/or other materials provided with the distribution.
** 
** - The name of Charles Lechasseur may not be used to endorse or promote
**   products derived from this software without specific prior written permission. 
** 
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
** FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CHARLES LECHASSEUR OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
** BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
** BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
*/
	
#include "C_PatchFile.h"

#include "StOldResFile.h"


// ---------------------------------------------------------------------------------
//		¥ MakeFileFromMakeInfo
// ---------------------------------------------------------------------------------
// makes a new shuttle file according to Wail's make info.

void MakeFileFromMakeInfo ( const FSSpec& inMacFileSpec )
{
	OSErr err;
	
	// keep track of old resource fork.
	StOldResFile theOldResFile;
	
	// read make info.
	MakeInfoResourceHandle makeInfo =
		(MakeInfoResourceHandle) ::GetResource (resourceType_MakeInfo, MInf_WailShuttleInfo);
	ThrowIfResError_();	
	
	// create file forks as necessary.
	if ((**makeInfo).makeData)
	{
		ThrowIfOSErr_( ::FSpCreate (&inMacFileSpec, (**makeInfo).makeCreator,
													(**makeInfo).makeType, smSystemScript) );
	}
	if ((**makeInfo).makeRsrc)
	{
		::FSpCreateResFile (&inMacFileSpec, (**makeInfo).makeCreator,
						  					(**makeInfo).makeType, smSystemScript);
		ThrowIfResError_();
	}
	
	// copy both forks as necessary.
	Handle makeData;
	if ((**makeInfo).makeData)
	{
		// read make data.
		makeData = ::GetResource (resourceType_MakeContent, MAKE_WailShuttleData);
		err = ::ResError ();
		if ( (err == noErr) &&
			 (makeData == nil) )
			err = resNotFound;
		ThrowIfOSErr_( err );
		
		// get resource size.
		long count = ::GetResourceSizeOnDisk (makeData);
		ThrowIfResError_();
		
		// detach data.
		::DetachResource (makeData);
		ThrowIfResError_();
		
		// open app data fork.
		short refNum;
		ThrowIfOSErr_( ::FSpOpenDF (&inMacFileSpec, fsRdWrPerm, &refNum) );
		
		// write fork.
		{
			StHandleLocker theHandleLocker( makeData );
			ThrowIfOSErr_( ::FSWrite (refNum, &count, *makeData) );
		}
		
		// close file.
		ThrowIfOSErr_( ::FSClose (refNum) );
		
		// release make data.
		::DisposeHandle (makeData);
	}
	if ((**makeInfo).makeRsrc)
	{
		// read make data.
		makeData = ::GetResource (resourceType_MakeContent, MAKE_WailShuttleResources);
		err = ResError ();
		if ( (err == noErr) &&
			 (makeData == nil) )
			err = resNotFound;
		ThrowIfOSErr_( err );
		
		// get resource size.
		long count = ::GetResourceSizeOnDisk (makeData);
		ThrowIfResError_();
		
		// detach data.
		::DetachResource (makeData);
		ThrowIfResError_();
		
		// open app resource fork.
		short refNum;
		ThrowIfOSErr_( ::FSpOpenRF (&inMacFileSpec, fsRdWrPerm, &refNum) );
		
		// write fork.
		{
			StHandleLocker theHandleLocker( makeData );
			ThrowIfOSErr_( ::FSWrite (refNum, &count, *makeData) );
		}
		
		// close file.
		ThrowIfOSErr_( ::FSClose (refNum) );
		
		// release make data.
		::DisposeHandle (makeData);
	}
	
	// release make info.
	::ReleaseResource ((Handle) makeInfo);
	ThrowIfResError_();
}