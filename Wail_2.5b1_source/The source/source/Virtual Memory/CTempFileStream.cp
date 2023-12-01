// =================================================================================
//	CTempFileStream.cp					©1999, Charles Lechasseur
// =================================================================================
//
// a file stream whose file is automatically created in the temporary
// directory of the default (system) volume. the file is automatically
// created and opened when the object is created, and it is automatically
// closed and destroyed when the object is deleted.
//
// use as an ordinary stream and all should go fine.

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

#include "CTempFileStream.h"


// static variables

SInt32	CTempFileStream::sTempFileNum = 0;


// ---------------------------------------------------------------------------
//	¥ CTempFileStream							Constructor	[public]
// ---------------------------------------------------------------------------
// allocates a file, with a size of inDesiredSize bytes. if you don't know
// the size yet, pass 0 in this field.

CTempFileStream::CTempFileStream(
	SInt32	inDesiredSize )
	: LFileStream()
{
	// allocate file.
	CreateTempFile();
	
	// open its data fork.
	OpenDataFork( fsRdWrPerm );
	
	// set its size if non-zero.
	if (inDesiredSize != 0)
		SetLength( inDesiredSize );
}


// ---------------------------------------------------------------------------
//	¥ ~CTempFileStream							Destructor	[public]
// ---------------------------------------------------------------------------

CTempFileStream::~CTempFileStream()
{
	try
	{
		// close data fork.
		CloseDataFork();
		
		// delete temp file.
		DeleteTempFile();
	}
	
	catch (...)
	{
		// we can't throw. at least signal.
		SignalStringLiteral_( "Exception catched in ~CTempFileStream()" );
	}
}


// ---------------------------------------------------------------------------------
//		¥ CreateTempFile
// ---------------------------------------------------------------------------------
// creates a temp file to store our data.

void
CTempFileStream::CreateTempFile()
{
	// find an FSSpec for our temp file.
	FSSpec theSpec;
	GetBestFSSpec( theSpec );
	
	// use that spec as our spec.
	SetSpecifier( theSpec );
	
	// create the file.
	CreateNewDataFile( GetCreator(), GetFileType() );
}


// ---------------------------------------------------------------------------------
//		¥ DeleteTempFile
// ---------------------------------------------------------------------------------
// deletes the temp file created before.

void
CTempFileStream::DeleteTempFile()
{
	FSSpec theSpec;
	GetSpecifier( theSpec );
	ThrowIfOSErr_( ::FSpDelete( &theSpec ) );
}
		

// ---------------------------------------------------------------------------------
//		¥ GetBestFSSpec
// ---------------------------------------------------------------------------------
// finds the best FSSpec for our temp file.
// looks in the temporary directory on the default volume.

void
CTempFileStream::GetBestFSSpec(
	FSSpec&	outSpec ) const
{
	// locate the temp folder of the default volume.
	SInt16 theVRefNum;
	SInt32 theDirID;
	ThrowIfOSErr_(
		::FindFolder( kOnSystemDisk, kTemporaryFolderType, true, &theVRefNum, &theDirID )
				);
				
	// find best name for a new file.
	Str255 theName;
	GetBestName( theVRefNum, theDirID, theName );
	
	// make a spec out of all this.
	OSErr err = ::FSMakeFSSpec( theVRefNum, theDirID, theName, &outSpec );
	
	// if it's not found, we're happy...
	if (err == fnfErr)
		err = noErr;
	// ...but it we found it, it means the other function didn't do its job right. crap.
	else if (err == noErr)
		err = fnfErr;	// weird huh? :)
		
	// throw in case of an error.
	ThrowIfOSErr_( err );
}


// ---------------------------------------------------------------------------------
//		¥ GetBestName
// ---------------------------------------------------------------------------------
// finds a random name for the temp file, to be located in the specified
// directory and volume.

StringPtr
CTempFileStream::GetBestName(
	SInt16	inVRefNum,
	SInt32	inParID,
	Str255	outName ) const 
{
	Boolean foundOne = false;
	
	while (!foundOne)
	{
		// try generating a random name.
		
		// we use a static variable to count the number of temp files we allocated.
		// turn this into a string and make that the name we use.
		::NumToString( sTempFileNum, outName );
		
		// increment this variable after each use.
		sTempFileNum++; 
		
		// see if this file exists.
		FSSpec theSpec;
		OSErr err = ::FSMakeFSSpec( inVRefNum, inParID, outName, &theSpec );
		
		// file is ok to use if it doesn't exist.
		foundOne = (err == fnfErr);
		
		// a bdNamErr is ok. why does the OS throws this at us, i have no idea...
		if (err == bdNamErr)
			err = noErr;
		
		// if not, then throw the error if one occured.
		if (!foundOne)
			ThrowIfOSErr_( err );
	}
	
	return outName;
}


// ---------------------------------------------------------------------------------
//		¥ GetFileType
// ---------------------------------------------------------------------------------
// returns the file type used for the temp file.

OSType
CTempFileStream::GetFileType() const
{
	return '????';
}


// ---------------------------------------------------------------------------------
//		¥ GetCreator
// ---------------------------------------------------------------------------------
// returns the creator used for the temp file.

OSType
CTempFileStream::GetCreator() const
{
	return '????';
}