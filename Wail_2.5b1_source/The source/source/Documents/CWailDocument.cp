// =================================================================================
//	CWailDocument.cp				©1998, Charles Lechasseur
// =================================================================================
//	CWailDocument objects are responsible for linking Sound editing windows
//	(CWailDocWindow) with Marathon Sound Files (CMthonSoundFile).
//
//	class derived from LSingleDoc (each window being associated with a single file,
//	and vice versa).

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

#include <LFile.h>
#include <LPlaceHolder.h>
#include <LPrintout.h>
#include <LString.h>
#include <LWindow.h>
#include <PP_Messages.h>
#include <UMemoryMgr.h>
#include <UWindows.h>
#include <UStandardDialogs.h>

#include "StOldResFile.h"
#include "CInFileStream.h"

#include "CWailDocument.h"

#include "CWailDocWindow.h"
#include "CWailSoundFileData.h"

#include "C_PatchFile.h"

#include "WailTypes.h"
#include "WailCommands.h"
#include "WailShuttleConstants.h"


// ---------------------------------------------------------------------------------
//		¥ CWailDocument(LCommander*, FSSpec*)
// ---------------------------------------------------------------------------------


CWailDocument::CWailDocument(
	LCommander	*inSuper,
	FSSpec		*inFileSpec,
	OSType		inFileType,
	OSType		inFileCreator )
		: LSingleDoc( inSuper )
{
	// Create window for our document.
	mWindow = nil;
	mWindow = LWindow::CreateWindow( rPPob_SoundWindow, this );
	ThrowIfNil_(mWindow);
	
	if ( inFileSpec == nil ) {

		// since we don't have a file, we'll default to infinity sound file type.
		mFileType = fileType_MarathonInfinitySound;
		mFileCreator = fileCreator_MarathonInfinitySound;

		// open a new document.
		CreateNewDoc();
		
	} else {
	
		// save file type and creator.
		mFileType = inFileType;
		mFileCreator = inFileCreator;
	
		// Display contents of file in window.
		OpenFile( *inFileSpec );
	
	}
	
	// Make the window visible.
	//mWindow->Show();
	// not. we'll make it visible *after* the window's data is loaded.
}


// ---------------------------------------------------------------------------------
//		¥ NameNewDoc
// ---------------------------------------------------------------------------------

void
CWailDocument::NameNewDoc()
{
	// Setup the window title. Start with the default title.
	LStr255	theTitle( "\pUntitled" );

	// Find the first available title. We could also check the window
	// pane id if we wanted to make sure we didn't collide with other
	// window types.
	SInt32	theNumber = 0;
	while ( UWindows::FindNamedWindow( theTitle ) != nil ) {

		// An existing window has the current name
		// Increment counter and try again.
		++theNumber;
		theTitle = "\pUntitled ";
		theTitle += static_cast<SInt32>(theNumber);

	}		
	
	// Finally, set window title.
	mWindow->SetDescriptor( theTitle );
	mIsSpecified = false;
}


// ---------------------------------------------------------------------------------
//		¥ CreateNewDoc
// ---------------------------------------------------------------------------------

void
CWailDocument::CreateNewDoc()
{
	// create an empty CWailSoundFileData object.
	CWailSoundFileData *theSoundFileData = new CWailSoundFileData;
			
	// are we a shuttle?
	mIsShuttle = false; // not yet supported.
	
	// store this data in the window.
	((CWailDocWindow*) mWindow)->SetSoundFileData( theSoundFileData );
		// window now "owns" the sound file data and will take care of it.	
	
	// Set the window title.
	NameNewDoc();
	
	// show the window.
	mWindow->Show();
}


// ---------------------------------------------------------------------------------
//		¥ OpenFile
// ---------------------------------------------------------------------------------

void
CWailDocument::OpenFile(
	FSSpec	&inFileSpec )
{
	mFile = nil;
	
	// Create a new file stream object.
	LFileStream *theFileStream = new LFileStream( inFileSpec );
	
	// Open the data fork.
	theFileStream->OpenDataFork( fsRdWrPerm );
	
	// create a CWailSoundFileData object with the content of that file.
	// make the data object uses mapped views of the file to read sounds.
	// this means that the data object will own the stream.
	CWailSoundFileData* theSoundFileData = new CWailSoundFileData( theFileStream, true );
			
	// are we a shuttle?
	mIsShuttle = false; // not yet supported.
	
	// store this data in the window.
	((CWailDocWindow*) mWindow)->SetSoundFileData( theSoundFileData );
		// window now "owns" the sound file data and will take care of it.	
	
	// Set the window title to the name of the file and
	// flag that the document has an associated file.
	mWindow->SetDescriptor( inFileSpec.name );
	mIsSpecified = true;

	// show the window.
	mWindow->Show();

	// create an LFile to store.
	LFile *theFile = new LFile( inFileSpec );

	// store the file.
	mFile = theFile;
}


// ---------------------------------------------------------------------------------
//		¥ IsModified
// ---------------------------------------------------------------------------------

Boolean
CWailDocument::IsModified()
{
	// we're modified if our window is modified.
	mIsModified = ((CWailDocWindow*) mWindow)->IsDirty();
	return mIsModified;
}


// ---------------------------------------------------------------------------------
//		¥ DoAESave
// ---------------------------------------------------------------------------------
// perform a "Save As" operation.

void
CWailDocument::DoAESave(
	FSSpec	&inFileSpec,
	OSType	inFileType )
{
#pragma unused( inFileType )

	// Delete the existing file object.
	// Note: this does nothing to the actual file on disk.
	delete mFile;
	mFile = nil;
	
	// Make a new file object.
	mFile = new LFile( inFileSpec );
	
	// Make new file on disk.
	mFile->CreateNewDataFile( mFileCreator, mFileType );
	
	// Write out the data.
	DoSave();

	// Change window title to reflect the new name.
	mWindow->SetDescriptor( inFileSpec.name );

	// Document now has a specified file.
	mIsSpecified = true;
}


// ---------------------------------------------------------------------------------
//		¥ DoSave
// ---------------------------------------------------------------------------------
// perform a "Save" operation.

void
CWailDocument::DoSave()
{
	// get our file's specifier.
	FSSpec theSpecifier;
	mFile->GetSpecifier( theSpecifier );
	
	// get a specifier for another file beside our file. we must first save to
	// a temp file just beside our file and then switch the two files. this is
	// because we loaded our file's data and used mapped stream views for sounds,
	// thus the file is still open as we speak.
	FSSpec theTempFile;
	LStr255 theTempName;
	SInt32 theTempNumber = 0;
	OSErr err = noErr;
	do
	{
		theTempName = "\pTmpWailSnd";
		theTempName += theTempNumber;
		err = ::FSMakeFSSpec( theSpecifier.vRefNum,
							  theSpecifier.parID,
							  (ConstStr255Param) theTempName,
							  &theTempFile );
		++theTempNumber;
	} while ((err != fnfErr) && (theTempNumber != 0));
	
	// if we cannot find a suitable file name to create a temp file, choke.
	ThrowIf_( err == noErr );

	// create a LFileStream for our temp file, then create the file proper.
	LFileStream* theTempFileStream = new LFileStream( theTempFile );
	ThrowIfNil_( theTempFileStream );
	theTempFileStream->CreateNewDataFile( mFileCreator, mFileType );

	// Open the temp file's data fork.
	theTempFileStream->OpenDataFork( fsRdWrPerm );

	// get the file data from our window and save it to the temp file.
	(((CWailDocWindow*) mWindow)->GetSoundFileData())->SaveToFile( theTempFileStream );
	
	// close the temp file's data fork.
	theTempFileStream->CloseDataFork();
	
	// delete the temp file's stream, we no longer need it.
	delete theTempFileStream;
	
	// delete our window's sound file data, since its sounds have been created by
	// mapping stream views. we now need to reload the data from the new file.
	((CWailDocWindow*) mWindow)->DeleteSoundFileData();
	
	// everything is saved and both files are closed, so exchange the temp file's
	// data with our own.
	ThrowIfOSErr_( ::FSpExchangeFiles( &theSpecifier, &theTempFile ) );
	
	// moved data ok, get rid of temp file.
	::FSpDelete( &theTempFile );
	
	// now reload the file's data.
	// Create a new file stream object.
	LFileStream* theFileStream = new LFileStream( theSpecifier );
	
	// Open the data fork.
	theFileStream->OpenDataFork( fsRdWrPerm );
	
	// create a CWailSoundFileData object with the content of that file.
	// make the data object uses mapped views of the file to read sounds.
	// this means that the data object will own the stream.
	CWailSoundFileData* theSoundFileData = new CWailSoundFileData( theFileStream, true );
	
	// store this data in the window.
	((CWailDocWindow*) mWindow)->SetSoundFileData( theSoundFileData );
		// window now "owns" the sound file data and will take care of it.	
	
	// saving makes the doc un-dirty.
	((CWailDocWindow*) mWindow)->SetDirty( false );
}


// ---------------------------------------------------------------------------------
//		¥ DoRevert
// ---------------------------------------------------------------------------------

void
CWailDocument::DoRevert()
{
	// delete the previous sound data.
	((CWailDocWindow*) mWindow)->DeleteSoundFileData();

	// get our file's specifier.
	FSSpec theSpecifier;
	mFile->GetSpecifier( theSpecifier );

	// Create a new file stream object.
	LFileStream *theFileStream = new LFileStream( theSpecifier );
	
	// Open the data fork.
	theFileStream->OpenDataFork( fsRdWrPerm );
	
	// create a CWailSoundFileData object with the content of that file.
	// make the data object uses mapped views of the file to read sounds.
	// this means that the data object will own the stream.
	CWailSoundFileData *theSoundFileData = new CWailSoundFileData( theFileStream, true );
	
	// store this data in the window.
	((CWailDocWindow*) mWindow)->SetSoundFileData( theSoundFileData );
		// window now "owns" the sound file data and will take care of it.	
	
	// reverting makes the doc un-dirty.
	((CWailDocWindow*) mWindow)->SetDirty( false );
}


// ---------------------------------------------------------------------------
//		¥ GetFileType
// ---------------------------------------------------------------------------
// returns the file type of our file.

OSType
CWailDocument::GetFileType() const
{
	return mFileType;
}


// ---------------------------------------------------------------------------
//		¥ ObeyCommand
// ---------------------------------------------------------------------------
// handles Document-specific commands.

Boolean
CWailDocument::ObeyCommand(
	CommandT	inCommand,
	void		*ioParam)
{
	Boolean		cmdHandled = true;

	switch (inCommand) {
	
		case cmd_MakeShuttleFromFile:
			cmdHandled = MakeShuttle();
			break;
			
		default:
			cmdHandled = LSingleDoc::ObeyCommand( inCommand, ioParam );
			break;
	}
	
	return cmdHandled;
}


// ---------------------------------------------------------------------------
//		¥ FindCommandStatus
// ---------------------------------------------------------------------------
//	Return whether a Command is enabled and/or marked (in a Menu)

void
CWailDocument::FindCommandStatus(
	CommandT	inCommand,
	Boolean		&outEnabled,
	Boolean		&outUsesMark,
	UInt16		&outMark,
	Str255		outName)
{
	switch (inCommand) {
	
		case cmd_MakeShuttleFromFile:
			outEnabled = true;		// we can always create a shuttle from our file.
			break;
			
		default:
			LSingleDoc::FindCommandStatus( inCommand, outEnabled,
										   outUsesMark, outMark, outName );
			break;
	}
}


// ---------------------------------------------------------------------------------
//		¥ MakeShuttle
// ---------------------------------------------------------------------------------
// creates a new shuttle from the data of our file.

Boolean
CWailDocument::MakeShuttle()
{
	// ask user to save the file.
	LStr255				defaultName( STRx_ShuttleStrings, str_ShuttleDefaultName );
	FSSpec theFile;
	bool isReplacing = false;
	bool isGood = PP_StandardDialogs::AskSaveFile( defaultName,
												   fileType_Application,
												   theFile,
												   isReplacing );
	if (isGood)
	{
		// see if the file needs to be deleted.
		if (isReplacing)
		{
			ThrowIfOSErr_( ::FSpDelete( &theFile ) );
		}
	
		// create the base Shuttle content.
		MakeFileFromMakeInfo( theFile );
	
		// save current resource fork for later.
		StOldResFile oldResFile;
	
		// create a file stream from our file.
		CInFileStream *theFileStream = new CInFileStream( theFile );
		
		// create an auto-deleter to be exception-safe.
		// this will automatically delete the file stream at the end of this block.
		StDeleter<CInFileStream> theFileStreamDeleter( theFileStream );

		// open its data fork.
		theFileStream->OpenDataFork( fsRdWrPerm );
		
		// get its length.
		SInt32 theLength = theFileStream->GetLength();
		
		// move the inset of the InFileStream to make it look like
		// it starts at the end of the file.
		theFileStream->SetInset( theLength );
		
		// save our data to the file.
		((CWailDocWindow*) mWindow)->GetSoundFileData()->SaveToFile( theFileStream );
		
		// close the data fork.
		theFileStream->CloseDataFork();
		
		// open the resource fork.
		theFileStream->OpenResourceFork( fsRdWrPerm );
		
		// we need to tell the shuttle where to start looking for its data in its own data fork.
		// we will create a resource of 4 bytes with that information and add it to the
		// shuttle as a resource.
		{
			StHandleBlock theLongHandle( 4 );
			(**((SInt32**) ((Handle) theLongHandle))) = theLength;
			
			// add the resource to the shuttle.
			::AddResource( theLongHandle,
						   rShuttleData_Type,
						   rShuttleData_ID,
						   // resource name:
						   	"\p" );
			ThrowIfResError_();
			
			// release that new resource from memory.
			::ReleaseResource( theLongHandle.Release() );
			ThrowIfResError_();
		}
		
		// close the resource fork.
		theFileStream->CloseResourceFork();
		
		// the file stream is deleted here by the StDeleter created earlier.
	}
	
	return isGood;
}