// =================================================================================
//	CShuttleDocApp.cp					©1999, Charles Lechasseur
// =================================================================================
// 	Application for shuttling sounds inside sound files.

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


// PowerPlant headers:
#include <LGrowZone.h>
#include <LMenu.h>
#include <LMenuBar.h>
#include <LString.h>
#include <LWindow.h>
#include <LDocument.h>
#include <LActiveScroller.h>
#include <PP_Messages.h>
#include <PPobClasses.h>
#include <UDesktop.h>
#include <UDrawingState.h>
#include <UMemoryMgr.h>
#include <UReanimator.h>
#include <URegistrar.h>
#include <UTextTraits.h>

// DEBUG
#include <UModalDialogs.h>

// Application headers:
#include "CShuttleDocApp.h"

// Custom panes headers:
#include "CBeeProgressBar.h"

// constants files:
#include "WailTypes.h"


// =================================================================================
//		¥ Main Program
// =================================================================================

int main( void )
{
	// Setup the throw and signal actions.
	SetDebugThrow_( debugAction_Alert );
	SetDebugSignal_( debugAction_Alert );

	// Initialize the heap. Parameter is number
	// of master handle blocks to allocate.
	InitializeHeap( 4 );
	
	// Initialize the MacOS toolbox.
	UQDGlobals::InitializeToolbox( &qd );

	// Install a GrowZone function to catch  low memory situations.
	// Parameter is the size of the memory reserve in bytes.
	new LGrowZone( 20000 );

	// Create the application object and run it.
	CShuttleDocApp	theApp;
	theApp.Run();
	
	return 0;
}


// ---------------------------------------------------------------------------------
//		¥ CShuttleDocApp
// ---------------------------------------------------------------------------------

CShuttleDocApp::CShuttleDocApp()
{
	// init variables.
	mHasFile = false;
	mIdleCount = 0;
	mShuttleWork = nil;
	SetSleepTime( 1 );	// don't sleep too much.

	// Register PowerPlant class creator functions.
	RegisterClass_(LWindow);
	RegisterClass_(LDialogBox);
	RegisterClass_(LPicture);
	RegisterClass_(LCaption);
	RegisterClass_(LStdButton);
	RegisterClass_(LStdRadioButton);
	RegisterClass_(LRadioGroup);
	
	// register custom classes
	RegisterClass_(CBeeProgressBar);
	
	// start idling to wait for a file.
	StartIdling();
}


// ---------------------------------------------------------------------------------
//		¥ ~CShuttleDocApp
// ---------------------------------------------------------------------------------

CShuttleDocApp::~CShuttleDocApp()
{
	if (mShuttleWork != nil)
		delete mShuttleWork;
}


// ---------------------------------------------------------------------------------
//		¥ SpendTime
// ---------------------------------------------------------------------------------

void
CShuttleDocApp::SpendTime(
	const EventRecord	&inMacEvent)
{
#pragma unused( inMacEvent )

	if (IsIdling())
	{
		mIdleCount++;
		
		if (mHasFile)
		{
			// we have the file. stop waiting for it and get to work.
			StopIdling();
			StartRepeating();
		}
		else
		{
			if (mIdleCount == 2)
				// we don't have the file. ask for it.
				ChooseDocument();
			else if (mIdleCount == 4)
			{
				// we don't have the file and never will. quit.
				StopRepeating();
				SendAEQuit();
			}
		}
	}
	else if (IsRepeating())
	{
		if (!mHasFile)
		{
			// an error occured.
			SignalStringLiteral_( "CShuttleDocApp::SpendTime: mFile missing." );
			StopRepeating();
			SendAEQuit();
		}
		
		if (mShuttleWork == nil)
		{
			// create a new CShuttleWork object to work on our file.
			mShuttleWork = new CShuttleWork( mFile, this );
		}
		else
		{
			// let's do some work.
			StopRepeating();
			Boolean isDone = mShuttleWork->DoWork();
			StartRepeating();
			
			if (isDone)
			{
				// we're done.
				StopRepeating();
				SendAEQuit();
			}
		}
	}
}


// ---------------------------------------------------------------------------
//		¥ CShuttleDocApp
// ---------------------------------------------------------------------------
//	Respond to commands

Boolean
CShuttleDocApp::ObeyCommand(
	CommandT	inCommand,
	void		*ioParam)
{
	Boolean		cmdHandled = true;
	
	ResIDT	theMenuID;
	SInt16	theMenuItem;
	if (IsSyntheticCommand(inCommand, theMenuID, theMenuItem)) {
	
		if (theMenuID == MENU_Apple) {
									// Handle selection from the Apple Menu
			Str255	appleItem;
			::GetMenuItemText(GetMenuHandle(theMenuID), theMenuItem, appleItem);
			::OpenDeskAcc(appleItem);
			
		} else {
			cmdHandled = LCommander::ObeyCommand(inCommand, ioParam);
		}
			
	} else {
		
		switch (inCommand) {
				
			case cmd_Quit:
				SendAEQuit();	// DEBUG! fix this!
				break;
				
			default:
				cmdHandled = LCommander::ObeyCommand(inCommand, ioParam);
				break;
		}
	}
	
	return cmdHandled;
}


// ---------------------------------------------------------------------------
//		¥ FindCommandStatus
// ---------------------------------------------------------------------------
//	Pass back whether a Command is enabled and/or marked (in a Menu)

void
CShuttleDocApp::FindCommandStatus(
	CommandT	inCommand,
	Boolean		&outEnabled,
	Boolean		&outUsesMark,
	UInt16		&outMark,
	Str255		outName)
{
	switch (inCommand) {
	
		case cmd_About:
			outEnabled = false;
			break;
			
		case cmd_Quit:
			outEnabled = true;
			break;
			
		case cmd_Undo:
			outEnabled = false;
			::GetIndString(outName, STRx_UndoEdit, str_CantRedoUndo);
			break;
			
		default:
			LCommander::FindCommandStatus(inCommand, outEnabled,
									outUsesMark, outMark, outName);
			break;
	}
}


// ---------------------------------------------------------------------------------
//		¥ StartUp
// ---------------------------------------------------------------------------------

void
CShuttleDocApp::StartUp()
{
	// 2.0a7: moved code to end of constructor. StartUp is not called when
	// app receives an OpenDoc AE.
}


// ---------------------------------------------------------------------------------
//		¥ OpenDocument
// ---------------------------------------------------------------------------------

void
CShuttleDocApp::OpenDocument(
	FSSpec	*inMacFSSpec )
{
	// check document type.
	FInfo theFileInfo;
	ThrowIfOSErr_( FSpGetFInfo( inMacFSSpec, &theFileInfo ) );
	switch (theFileInfo.fdType)
	{
		case fileType_Marathon2Sound:
		case fileType_MarathonInfinitySound:
			{
				mFile = *inMacFSSpec;
				mHasFile = true;
			}
	}
}


// ---------------------------------------------------------------------------------
//		¥ ChooseDocument
// ---------------------------------------------------------------------------------

void
CShuttleDocApp::ChooseDocument()
{
	// Deactivate the desktop.
	UDesktop::Deactivate();

	// Browse for a document.
	SFTypeList			theTypeList = { fileType_Marathon2Sound,
										fileType_MarathonInfinitySound };
	StandardFileReply	theReply;
	::StandardGetFile( nil, 2, theTypeList, &theReply );

	// Activate the desktop.
	UDesktop::Activate();
	
	// Send an apple event to open the file.	
	if ( theReply.sfGood ) {
		SendAEOpenDoc( theReply.sfFile );
	}
}
