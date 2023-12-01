// =================================================================================
//	CShuttleWork.cp					©1999, Charles Lechasseur
// =================================================================================
//
// Class that handles most of the work involved in shuttling sounds.

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

#include "CShuttleWork.h"

#include "WailShuttleConstants.h"

#include <UModalDialogs.h>


// enums
#pragma mark -- enums --

enum EMthonSets
{
	set_8bit = 0,
	set_16bit
};

enum EInstallationChoices
{
	install_Dont = 0,
	install_Source,
	install_Shuttle
};


// constants
#pragma mark -- constants --

const	ResIDT		rPPob_ChooseDestination			= 1500;
const	ResIDT		rPPob_ChooseInstallationMethod	= 1501;

const	PaneIDT		pane_InFileItselfRadio			= 'File';
const	PaneIDT		pane_InNewFileRadio				= 'Copy';

const	ResIDT		rPPob_WrongNumberOfClasses		= 1501;

const	PaneIDT		pane_DontInstallSounds			= 'Dont';
const	MessageT	msg_DontInstallSounds			= pane_DontInstallSounds;
const	PaneIDT		pane_InstallFromSource			= 'Sour';
const	MessageT	msg_InstallFromSource			= pane_InstallFromSource;
const	PaneIDT		pane_InstallFromShuttle			= 'Shut';
const	MessageT	msg_InstallFromShuttle			= pane_InstallFromShuttle;


// ---------------------------------------------------------------------------
//	¥ CShuttleWork
// ---------------------------------------------------------------------------

CShuttleWork::CShuttleWork(
	const FSSpec&	inFile,
	LCommander		*inSuper )
	: LCommander( inSuper )
{
	// let's create the source LStream.
	mSourceFile = new LFileStream( inFile );
	mSourceFile->OpenDataFork( fsRdPerm );
	
	// let's create the patch LStream.
//	mPatchFile = new CResourceStream( rShuttleData_Type, rShuttleData_ID );
	CreatePatchFile();
	
	// destination file doesn't exist yet.
	mDestinationFile = nil;
	
	// progress dialog doesn't exist yet.
	mProgressDialog = nil;
	
	mIsDone = false;
}


// ---------------------------------------------------------------------------
//	¥ ~CShuttleWork
// ---------------------------------------------------------------------------

CShuttleWork::~CShuttleWork()
{
	if (mSourceFile != nil)
		delete mSourceFile;
	if (mPatchFile != nil)
		delete mPatchFile;
	if (mDestinationFile != nil)
		delete mDestinationFile;
	if (mProgressDialog != nil)
		delete mProgressDialog;
}


// ---------------------------------------------------------------------------------
//		¥ DoWork
// ---------------------------------------------------------------------------------
//

Boolean
CShuttleWork::DoWork()
{
	// let's check if the destination file exists.
	if ((!mIsDone) && (mDestinationFile == nil))
	{
		AskForDestinationFile();
	}
	
	// let's see if the progress dialog has been created.
	if ((!mIsDone) && (mProgressDialog == nil))
	{
		mProgressDialog = new CWailProgressDialog( 0, /* we don't know the size yet. */
												   progressString_Shuttling, this );
		mProgressDialogInitialized = false;
	}
	
	// let's figure out how many classes must be shuttled.
	if ((!mIsDone) && (!mProgressDialogInitialized))
	{
		// it is a possibility that the source file and the patch data do not contain the
		// same number of classes. in this case, we must warn the user that these are
		// different and that shuttling to this file might end up changing the number of sounds
		// in the sound file.
		
		// get the number of classes in each file.
		SMthonSoundHeader header;
		
		ReadSoundFileHeader( mSourceFile, header );
		mNumClassesSource = header.mNumClasses;
		
		ReadSoundFileHeader( mPatchFile, header );
		mNumClassesShuttle = header.mNumClasses;
		
		// see if they're different.
		SInt32 choice;
		if (mNumClassesSource != mNumClassesShuttle)
		{
			// ask the user what to do.
			choice = AskForInstallationMethod();
			
			// see if user cancelled.
			if (choice == install_Dont)
				// we're done.
				mIsDone = true;
			else
			{
				// we must set the number of classes according to the user's answer.
				if (choice == install_Source)
					mNumClasses = mNumClassesSource;
				else
					mNumClasses = mNumClassesShuttle;
			}
		}
		else
		{
			// both are the same. pick one. :)
			mNumClasses = mNumClassesSource;
		}
		
		// initialize progress bar.
		if (!mIsDone)
		{
			mProgressDialog->SetProgressBarSize( mNumClasses * 2 );
			mProgressDialogInitialized = true;
			mCurrentSet = 0;
			mCurrentClass = -1;
		}
	}
	
	// let's see if the destination file has been created.
	if ((!mIsDone) && (!mDestinationFileExists))
	{
		// get the source file's creator and type.
		FInfo sourceInfo;
		FSSpec theSpecifier;
		mSourceFile->GetSpecifier( theSpecifier );
		ThrowIfOSErr_( ::FSpGetFInfo( &theSpecifier, &sourceInfo ) );

		// create the file.
		mDestinationFile->CreateNewDataFile( sourceInfo.fdCreator, sourceInfo.fdType,
											 smSystemScript );
		// open it.
		mDestinationFile->OpenDataFork( fsRdWrPerm );
		mDestinationFileExists = true;
	}
	
	// let's see if we're ready to shuttle sounds.
	if (!mIsDone)
	{
		// see if header is installed.
		if (mCurrentClass == -1)
		{
			// we must install the header.
			InstallHeader();
		}
		else
		{
			// we must install a sound class.
			InstallClass( mCurrentSet, mCurrentClass );
			// increment progress bar.
			mProgressDialog->Increment();
		}
		
		// increment class counter.
		mCurrentClass++;
		// see if we're done.
		if (!(mCurrentClass < mNumClasses))
			if (mCurrentSet < 1)
			{
				// we're not done - go to 16-bit set.
				mCurrentSet++;
				mCurrentClass = 0;
			}
			else 
			{
				// we're done.
				mIsDone = true;
				
				// maybe we need to exchange data if it was a temp file.
				if (mOverwriteSource)
				{
					// close the dest file.
					mDestinationFile->CloseDataFork();
					// close the source file.
					mSourceFile->CloseDataFork();
					// exchange data.
					FSSpec theSourceSpecifier,
						   theDestinationSpecifier;
					mSourceFile->GetSpecifier( theSourceSpecifier );
					mDestinationFile->GetSpecifier( theDestinationSpecifier );
					ThrowIfOSErr_( ::FSpExchangeFiles( &theSourceSpecifier,
													   &theDestinationSpecifier ) );
					// delete temp file.
					ThrowIfOSErr_( ::FSpDelete( &theDestinationSpecifier ) );
				}
			}
	}

	return mIsDone;
}


// ---------------------------------------------------------------------------------
//		¥ CreatePatchFile
// ---------------------------------------------------------------------------------
// creating the patch file object is now more complex. it requires that we
// find an FSSpec to ourselves with the Process Manager, and then create
// an inset file stream to read our data. we also need to fetch the data
// offset from our resource.

void
CShuttleWork::CreatePatchFile()
{
	// get the data offset.
	SInt32 theDataOffset = 0;
	{
		// fetch our resource. it contains the offset of the patch data
		// inside the data fork.
		StResource theResource( rShuttleData_Type, rShuttleData_ID, true, true );
		
		// the resource contains a signed 32-bit integer.
		theDataOffset = **((SInt32**) ((Handle) theResource));
	}
	
	// get the serial number of the current process - i.e., us.
	ProcessSerialNumber thePSN;
	ThrowIfOSErr_( ::GetCurrentProcess( &thePSN ) );
	
	// get more info about us.
	ProcessInfoRec theInfoRec;
	FSSpec theSpec;
	
	// fill important fields.
	theInfoRec.processInfoLength = sizeof(ProcessInfoRec);	// dumb.
	theInfoRec.processName = nil;	// don't care about that.
	theInfoRec.processAppSpec = &theSpec;
	
	ThrowIfOSErr_( ::GetProcessInformation( &thePSN, &theInfoRec ) );
	
	// create our inset file stream from that spec and data offset.
	mPatchFile = new CInFileStream( theSpec, theDataOffset );
	
	// open its data fork. this will be closed when the object is destroyed
	// in this object's destructor.
	mPatchFile->OpenDataFork( fsRdPerm );
}


// ---------------------------------------------------------------------------------
//		¥ AskForDestinationFile
// ---------------------------------------------------------------------------------
// asks the user for the destination file of the shuttling.

void
CShuttleWork::AskForDestinationFile()
{
	// ask the user if he wishes to install sounds in a copy of the file.
		
	Boolean isDone;
	Boolean isGood;
	{
		StDialogHandler theDialogHandler( rPPob_ChooseDestination, this );
		
		isDone = false;
		isGood = false;
		while (!isDone)
		{
			MessageT theMessage = theDialogHandler.DoDialog();
			
			switch (theMessage)
			{
				case msg_Cancel:
					isDone = true;
					break;
					
				case msg_OK:
					{
						// check which radio was selected.
						LStdRadioButton *theRadio = dynamic_cast<LStdRadioButton*>
							((theDialogHandler.GetDialog())->FindPaneByID( pane_InFileItselfRadio ));
						ThrowIfNil_( theRadio );
						
						mOverwriteSource = ( theRadio->GetValue() == 1 );
						
						isGood = true;
						isDone = true;
					}
					break;
			}
		}
	}
	
	if (isGood)
	{
		// let's create a FSSpec for the new file.
		FSSpec theFileSpec;
		if (mOverwriteSource)
		{
			// we must create a temp file in the same folder as the source file.
			mSourceFile->GetSpecifier( theFileSpec );
			LString::AppendPStr(
				theFileSpec.name, (ConstStringPtr) "\ptemp", sizeof (StrFileName) );
			
			mDestinationFile = new LFileStream( theFileSpec );
			ThrowIfNil_( mDestinationFile );
			mDestinationFileExists = false; // not created yet.
		}
		else
		{
			// we must ask the user where he wishes to save the new file.
			StandardFileReply		reply;
			
			mSourceFile->GetSpecifier( theFileSpec );
			LStr255 				name = theFileSpec.name;
			name += (ConstStringPtr) "\p copy";
			
			UDesktop::Deactivate();
			::StandardPutFile( "\pSave file where?", (ConstStringPtr) name, &reply );
			UDesktop::Activate();
			
			if (reply.sfGood)
			{
				mDestinationFile = new LFileStream( reply.sfFile );
				ThrowIfNil_( mDestinationFile );
				mDestinationFileExists = false; // not created yet.
				
				// maybe we need to replace another file.
				if (reply.sfReplacing)
					ThrowIfOSErr_( ::FSpDelete( &reply.sfFile ) );
			}
			else
				// user cancelled.
				mIsDone = true;
		}
	}
	else
		// user cancelled.
		mIsDone = true;
}


// ---------------------------------------------------------------------------------
//		¥ AskForInstallationMethod
// ---------------------------------------------------------------------------------
// asks the user what to do when the source file and shuttle data contain
// incorrect number of sounds.

SInt32
CShuttleWork::AskForInstallationMethod()
{
	// ask the user if he wishes to install sounds, and if yes, from which data.

	Boolean isDone;
	Boolean isGood;
	SInt32 choice;
	{
		StDialogHandler theDialogHandler( rPPob_ChooseInstallationMethod, this );
		
		isDone = false;
		isGood = false;
		choice = install_Dont;
		while (!isDone)
		{
			MessageT theMessage = theDialogHandler.DoDialog();
			
			switch (theMessage)
			{
				case msg_Cancel:
					{
						isDone = true;
						choice = install_Dont;
					}
					break;
					
				case msg_OK:
					{
						isGood = true;
						isDone = true;
						
						// check which radio was selected.
						LStdRadioButton* theRadio = dynamic_cast<LStdRadioButton*>
							((theDialogHandler.GetDialog())->FindPaneByID( pane_DontInstallSounds ));
						ThrowIfNil_( theRadio );
						
						if (theRadio->GetValue())
							choice = install_Dont;
						
						theRadio = dynamic_cast<LStdRadioButton*>
							((theDialogHandler.GetDialog())->FindPaneByID( pane_InstallFromSource ));
						ThrowIfNil_( theRadio );
						
						if (theRadio->GetValue())
							choice = install_Source;
							
						theRadio = dynamic_cast<LStdRadioButton*>
							((theDialogHandler.GetDialog())->FindPaneByID( pane_InstallFromShuttle ));
						ThrowIfNil_( theRadio );
						
						if (theRadio->GetValue())
							choice = install_Shuttle;
					}
					break;
			}
		}
	}
	
	return choice;
}


// ---------------------------------------------------------------------------------
//		¥ InstallHeader
// ---------------------------------------------------------------------------------
// installs the marathon sound file header in the destination file and enlarges the
// file so it can contain all the needed class headers.

void
CShuttleWork::InstallHeader()
{
	// create the header.
	SMthonSoundHeader header;
	header.mNumClasses = mNumClasses;
	
	// install it.
	WriteSoundFileHeader( mDestinationFile, header );
	
	// initialize current offset.
	mCurrentOffset = sizeof (SMthonSoundHeader) +
					 sizeof (SMthonSoundClass) * (mNumClasses * 2);
	
	// enlarge sound file.
	mDestinationFile->SetLength( mCurrentOffset );
}


// ---------------------------------------------------------------------------------
//		¥ InstallClass
// ---------------------------------------------------------------------------------
// installs the given class, along with all its sounds, in the destination file.

void
CShuttleWork::InstallClass(
	SInt16	inWhichSet,
	SInt16	inWhichClass )
{
	LStream *theSourceFile = nil;

	// get the class' header from the shuttle data.
	SMthonSoundClass theClassHeader;
	if (inWhichClass < mNumClassesShuttle)
		ReadSoundClass( mPatchFile, inWhichSet, inWhichClass, mNumClassesShuttle, theClassHeader );
	else
	{
		// shuttle data is too small. use data from the source file.
		// (this can happen if number of classes are different - one will always
		// be smaller than the other. this used to throw -39 debug alers.)
		theClassHeader.mClassID = classID_Unused;
	}
	
	// see if it a used class.
	if (theClassHeader.mClassID != classID_Unused)
	{
		// it is: source for this class will be the shuttle data.
		theSourceFile = mPatchFile;
		
		// special case: if the ID is actually -2, it means the class is
		// an empty class that we must install anyway. in this case replace
		// the ID with -1 since it's the correct ID for an unused class.
		if (theClassHeader.mClassID == classID_UnusedButInstalled)
			theClassHeader.mClassID = classID_Unused;
	}
	else
	{
		if (inWhichClass < mNumClassesSource)
		{
			// get the class' header from the source file.
			ReadSoundClass( mSourceFile, inWhichSet, inWhichClass, mNumClassesSource, theClassHeader );
			// source will be the source file.
			theSourceFile = mSourceFile;
		}
		else
		{
			// looks like source data is too small... we'll have to use the shuttle data.
			theSourceFile = mPatchFile;
		}
	}
	ThrowIf_( theSourceFile == nil );
	
	// new in 2.0a7: we must check if the class is remapped. it is if it has the same
	// first sound offset as its 8-bit equivalent.
	Boolean isRemapped = false;
	SInt32	the8bitOffset = mCurrentOffset;	// initialize safely just in case.
	
	if (inWhichSet == 1)	// only check this for 16-bit classes.
	{
		// read 8-bit header in correct file, like above.
		// keep track of the source file for this 8-bit class.
		SMthonSoundClass the8bitHeader;
		LStream* theSource8bitFile = mPatchFile;
		
		if (inWhichClass < mNumClassesShuttle)
			ReadSoundClass( theSource8bitFile, 0, inWhichClass, mNumClassesShuttle, the8bitHeader );
		else
			// shuttle data too small.
			the8bitHeader.mClassID = classID_Unused;
		
		if (the8bitHeader.mClassID == classID_Unused)
		{
			if (inWhichClass < mNumClassesSource)
			{
				theSource8bitFile = mSourceFile;
				ReadSoundClass( theSource8bitFile, 0, inWhichClass, mNumClassesSource, the8bitHeader );
			}
			else
				// source data too small.
				theSource8bitFile = mPatchFile;
		}
		
			
		// check if they have the same first sound offset.
		// (also check if the 8-bit and 16-bit classes are in the same file.)
		if ((theClassHeader.mFirstSoundOffset == the8bitHeader.mFirstSoundOffset) &&
			(theSource8bitFile == theSourceFile))
		{
			isRemapped = true;	// the class is remapped if they're the same.
			
			// if the class is remapped, we must re-fetch the offset of the 8-bit
			// class in the destination file and use it for our 16-bit class.
			ReadSoundClass( mDestinationFile, 0, inWhichClass, mNumClasses, the8bitHeader );
			the8bitOffset = the8bitHeader.mFirstSoundOffset;
		}
	}
	
	// install class header in the destination file.
	SInt32 oldFirstSoundOffset = theClassHeader.mFirstSoundOffset;
	if (!isRemapped)
		theClassHeader.mFirstSoundOffset = mCurrentOffset;
	else
		theClassHeader.mFirstSoundOffset = the8bitOffset;
	WriteSoundClass( mDestinationFile, inWhichSet, inWhichClass, mNumClasses, theClassHeader );
	theClassHeader.mFirstSoundOffset = oldFirstSoundOffset;
	
	// we transfer sounds only if the class is not remapped.
	if (!isRemapped)
	{
		// position the stream's marker at the beginning of the sounds.
		theSourceFile->SetMarker( theClassHeader.mFirstSoundOffset, streamFrom_Start );
		
		// read and install sounds.
		SInt16 i;
		for (i = 0; i < theClassHeader.mNumSounds; i++)
		{
			// get this sound's length in the source.
			SInt32 theLength;
			if (i == 0)
				// first sound.
				theLength = theClassHeader.mFirstSoundLength;
			else if (i == (theClassHeader.mNumSounds - 1))
				// last sound.
				theLength = theClassHeader.mTotalLength -
							theClassHeader.mSoundOffset[i];
			else
				// a sound in-between the first and the last.
				theLength = theClassHeader.mSoundOffset[i + 1] - 
							theClassHeader.mSoundOffset[i];
							
			// read the sound.
			Ptr theSound = ::NewPtr( theLength );
			ThrowIfNil_( theSound );
			
			ReadSound( theSourceFile, theSourceFile->GetMarker(), theLength, theSound );
			
			// write it in the destination file.
			WriteSound( mDestinationFile, mCurrentOffset, theLength, theSound );
			
			// de-allocate sound.
			::DisposePtr( theSound );
			
			// increment current offset.
			mCurrentOffset += theLength;
		}
	}
}


// ---------------------------------------------------------------------------------
//		¥ ReadSoundFileHeader
// ---------------------------------------------------------------------------------

void
CShuttleWork::ReadSoundFileHeader(
	LStream						*inFromWhere,
	SMthonSoundHeader&			outHeader )
{
	// position the stream.
	inFromWhere->SetMarker( 0, streamFrom_Start );
	
	// read the header.
	inFromWhere->ReadBlock( &outHeader, sizeof (SMthonSoundHeader) );
}


// ---------------------------------------------------------------------------------
//		¥ WriteSoundFileHeader
// ---------------------------------------------------------------------------------

void
CShuttleWork::WriteSoundFileHeader(
	LStream						*inToWhere,
	const SMthonSoundHeader&	inHeader )
{
	// position the stream.
	inToWhere->SetMarker( 0, streamFrom_Start );
	
	// write the header.
	inToWhere->WriteBlock( &inHeader, sizeof (SMthonSoundHeader) );
}
		

// ---------------------------------------------------------------------------------
//		¥ ReadSoundClass
// ---------------------------------------------------------------------------------
//	inWhichClass starts at 0.
					
void
CShuttleWork::ReadSoundClass(
	LStream						*inFromWhere,
	SInt16						inWhichSet,
	SInt16						inWhichClass,
	SInt16						inMaxNumClasses,
	SMthonSoundClass&			outClass )
{
	// check where the class should be located.
	SInt32 classOffset = sizeof (SMthonSoundHeader)
						+ sizeof (SMthonSoundClass) *
							((inWhichSet * inMaxNumClasses) + inWhichClass);
							
	// position the stream.
	inFromWhere->SetMarker( classOffset, streamFrom_Start );
	
	// read the class.
	inFromWhere->ReadBlock( &outClass, sizeof (SMthonSoundClass) );
}
	
	
// ---------------------------------------------------------------------------------
//		¥ WriteSoundClass
// ---------------------------------------------------------------------------------
//	inWhichClass starts at 0.
	
void
CShuttleWork::WriteSoundClass(
	LStream						*inToWhere,
	SInt16						inWhichSet,
	SInt16						inWhichClass,
	SInt16						inMaxNumClasses,
	const SMthonSoundClass&		inClass )
{
	// check where the class should be located.
	SInt32 classOffset = sizeof (SMthonSoundHeader)
						+ sizeof (SMthonSoundClass) *
							((inWhichSet * inMaxNumClasses) + inWhichClass);
							
	// position the stream.
	inToWhere->SetMarker( classOffset, streamFrom_Start );
	
	// write the class.
	inToWhere->WriteBlock( &inClass, sizeof (SMthonSoundClass) );
}
									

// ---------------------------------------------------------------------------------
//		¥ ReadSound
// ---------------------------------------------------------------------------------
			
void
CShuttleWork::ReadSound(
	LStream						*inFromWhere,
	SInt32						inOffset,
	SInt32						inSize,
	Ptr&						outSound )
{
	// position the stream.
	inFromWhere->SetMarker( inOffset, streamFrom_Start );
	
	// read the sound.
	inFromWhere->ReadBlock( outSound, inSize );
}


// ---------------------------------------------------------------------------------
//		¥ WriteSound
// ---------------------------------------------------------------------------------

void
CShuttleWork::WriteSound(
	LStream						*inToWhere,
	SInt32						inOffset,
	SInt32						inSize,
	Ptr							inSound )
{
	// position the stream.
	inToWhere->SetMarker( inOffset, streamFrom_Start );
	
	// write the sound.
	inToWhere->WriteBlock( inSound, inSize );
}