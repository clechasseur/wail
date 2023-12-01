// =================================================================================
//	CWailDocApp.cp					©1998, Charles Lechasseur
// =================================================================================
//	Wail is a Marathon Sound File editor.
//
//	version 1 was developed with REALbasic but was never finalized.
//	version 2 is written directly in C++ using PowerPlant.
//
//	the CWailDocApp doesn't do much, merely creating doc windows.
//	everything will be handled in CWailDocument objects.

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

// PP headers:
#include <LGrowZone.h>
#include <LUndoer.h>
#include <LIconPane.h>
#include <LPageController.h>
#include <LMultiPanelView.h>
#include <LGroupBox.h>
#include <LMenu.h>
#include <LMenuBar.h>
#include <LString.h>
#include <LWindow.h>
#include <LDocument.h>
#include <LActiveScroller.h>
#include <LTableView.h>
#include <LColumnView.h>
#include <LTextColumn.h>
#include <PP_Messages.h>
#include <UDesktop.h>
#include <UDrawingState.h>
#include <UMemoryMgr.h>
#include <UReanimator.h>
#include <URegistrar.h>
#include <UTextTraits.h>
#include <UModalDialogs.h>
#include <LFileTypeList.h>
#include <UStandardDialogs.h>

// Application header:
#include "CWailDocApp.h"

// Documents headers:
#include "CWailDocument.h"
#include "CWailDocWindow.h"
#include "CWailSoundClassListBox.h"
#include "CWailSoundListBox.h"
#include "CWailWindowChooserDialog.h"
#include "UWailClassNames.h"

// Useful classes headers:
#include "CSingleColumnListBox.h"
#include "CSingleColumnTextListBox.h"
#include "C1Col1SelTextListBox.h"
#include "CBeeProgressBar.h"
#include "CFreeRAMCaption.h"
#include "CBroadcastingEditField.h"
#include "CBroadcastingListBox.h"
#include "CFixedEditField.h"
#include "CVersionAttachment.h"
#include "UMoreModalDialogs.h"
#include "CWindowTracker.h"
#include "CTruncCaption.h"

// Virtual Memory headers:
#include "UVirtualMemory.h"
#include "CBasicVirtualMemoryManager.h"

// prefs headers:
#include "UWailPreferences.h"

// Jukebox headers:
#include "UJukebox.h"

// constants files:
#include "WailCommands.h"
#include "WailTypes.h"


// constants
#pragma mark -- Constants --

const	ResIDT	PPob_AboutBox		= 1001;

const	ResIDT	MENU_Window			= 133;

const	SInt32	memory_RamProtected	= 1024L * 1024L;	// we protect 1 Meg of RAM.


// =================================================================================
//		¥ Main Program
// =================================================================================
#pragma mark -

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
	
	// Initialize PP environment internals. 
	UEnvironment::InitEnvironment(); // why is this left out of the default Doc PP stationery?

	// Install a GrowZone function to catch  low memory situations.
	// Parameter is the size of the memory reserve in bytes.
	new LGrowZone( 20000 );

	// Create the application object and run it.
	CWailDocApp	theApp;
	theApp.Run();
	
	return 0;
}

#pragma mark -


// ---------------------------------------------------------------------------------
//	Static member variables of CWailDocApp
// ---------------------------------------------------------------------------------
#pragma mark -- Static members of CWailDocApp --

// none right now.


// ---------------------------------------------------------------------------------
//		¥ CWailDocApp
// ---------------------------------------------------------------------------------

CWailDocApp::CWailDocApp()
{
	
	// Register PowerPlant class creator functions.
//	RegisterClass_(LButton);
	RegisterClass_(LCaption);
	RegisterClass_(LDialogBox);
	RegisterClass_(LEditField);
	RegisterClass_(LListBox);
	RegisterClass_(LPane);
	RegisterClass_(LPicture);
//	RegisterClass_(LPlaceHolder);
//	RegisterClass_(LPrintout);
	RegisterClass_(LScroller);
	RegisterClass_(LStdControl);
	RegisterClass_(LStdButton);
	RegisterClass_(LStdCheckBox);
	RegisterClass_(LStdRadioButton);
	RegisterClass_(LStdPopupMenu);
//	RegisterClass_(LTextEditView);
	RegisterClass_(LView);
	RegisterClass_(LWindow);

	RegisterClass_(LRadioGroup);
	RegisterClass_(LTabGroup);
	
//	RegisterClass_(LCicnButton);
//	RegisterClass_(LOffscreenView);
	RegisterClass_(LActiveScroller);
//	RegisterClass_(LTable);
	RegisterClass_(LIconPane);
//	RegisterClass_(LTextButton);
	
	RegisterClass_(LUndoer);
	RegisterClass_(LGroupBox);
	RegisterClass_(LMultiPanelView);
	RegisterClass_(LPageController);
	
	RegisterClass_(LTableView);
	RegisterClass_(LColumnView);
	RegisterClass_(LTextColumn);

	// Register Documents classes.
	RegisterClass_(CWailDocWindow);
	RegisterClass_(CWailSoundClassListBox);
	RegisterClass_(CWailSoundListBox);
	RegisterClass_(CWailWindowChooserDialog);
	
	// Register Useful classes.
	RegisterClass_(CSingleColumnListBox);
	RegisterClass_(CSingleColumnTextListBox);
	RegisterClass_(C1Col1SelTextListBox);
	RegisterClass_(CBeeProgressBar);
	RegisterClass_(CFreeRAMCaption);
	RegisterClass_(CBroadcastingEditField);
	RegisterClass_(CBroadcastingListBox);
	RegisterClass_(CFixedEditField);
	RegisterClass_(CVersionAttachment);
	RegisterClass_(CTruncCaption);
}


// ---------------------------------------------------------------------------------
//		¥ ~CWailDocApp
// ---------------------------------------------------------------------------------

CWailDocApp::~CWailDocApp()
{
	Finalize();
}

// ---------------------------------------------------------------------------
//		¥ ObeyCommand
// ---------------------------------------------------------------------------
//	Respond to commands

Boolean
CWailDocApp::ObeyCommand(
	CommandT	inCommand,
	void		*ioParam)
{
	Boolean		cmdHandled = true;

	switch (inCommand) {
	
		case cmd_Preferences:
			UWailPreferences::Edit( this );
			break;
	
		// we will handle the Stop Sound command here.
		// this will allow the sound to be stopped even if all windows are closed.
		case cmd_StopSound:
			UJukebox::StopPlaying();
			break;
			
		default:
			cmdHandled = LDocApplication::ObeyCommand(inCommand, ioParam);
			break;
	}
	
	return cmdHandled;
}


// ---------------------------------------------------------------------------
//		¥ FindCommandStatus
// ---------------------------------------------------------------------------
//	Pass back the status of a Command

void
CWailDocApp::FindCommandStatus(
	CommandT	inCommand,
	Boolean		&outEnabled,
	Boolean		&outUsesMark,
	UInt16		&outMark,
	Str255		outName)
{
	switch (inCommand) {
			
		case cmd_Preferences:
			outEnabled = true;
			break;
			
		// we will handle the Stop Sound command here.
		// this will allow the sound to be stopped even if all windows are closed.
		case cmd_StopSound:
			outEnabled = UJukebox::GetChannel()->IsPlaying();
			break;
	
		default:
			LDocApplication::FindCommandStatus(inCommand, outEnabled,
									outUsesMark, outMark, outName);
			break;
	}
}


// ---------------------------------------------------------------------------------
//		¥ ShowAboutBox
// ---------------------------------------------------------------------------------

void
CWailDocApp::ShowAboutBox()
{
	UDesktop::Deactivate();

	StNoAboutDialogHandler theHandler( PPob_AboutBox, this );
	
	Boolean isDone = false;
	while (!isDone)
	{
		MessageT theMessage = theHandler.DoDialog();
		
		if (theMessage == msg_OK)
			isDone = true;
	}
	
	UDesktop::Activate();
}


// ---------------------------------------------------------------------------------
//		¥ StartUp
// ---------------------------------------------------------------------------------

void
CWailDocApp::StartUp()
{
}


// ---------------------------------------------------------------------------
//		¥ Initialize
// ---------------------------------------------------------------------------

void
CWailDocApp::Initialize()
{
	// initialize standard dialog functions.
	PP_StandardDialogs::Load();

	// initialize prefs. (must be done *before* everything else.)
	UWailPreferences::Initialize();
	UWailPreferences::Read();	// gotta read them too.

	// initialize the VRAM system.
	SInt32 ramProtected = UWailPreferences::RAMToProtect();
	if (ramProtected == 0)	// prefs error? who knows. don't fall for this. :)
		ramProtected = memory_RamProtected;
	UVirtualMemory::Initialize(
		new CBasicVirtualMemoryManager( ramProtected )
							);
							
	// Add a CWindowTracker attachment to the app.
	LAttachment* theWindowTracker = new CWindowTracker( MENU_Window );
	ThrowIfNil_( theWindowTracker );
	AddAttachment( theWindowTracker );

	// initialize UJukebox.
	UJukebox::Initialize( new CJukebox( 1 ) );	// we want a single channel.
	
	// pre-read class names.
	UWailClassNames::ReadClassNames();
}


// ---------------------------------------------------------------------------
//		¥ Finalize
// ---------------------------------------------------------------------------

void
CWailDocApp::Finalize()
{
	// dispose of class names.
	UWailClassNames::DisposeClassNames();

	// dispose of UJukebox.
	UJukebox::Finalize();
	
	// dispose of VRAM system.
	UVirtualMemory::Finalize();
	
	// cleanup the prefs.
	UWailPreferences::Finalize();
	
	// cleanup standard dialog functions.
	PP_StandardDialogs::Unload();
}


// ---------------------------------------------------------------------------------
//		¥ OpenDocument
// ---------------------------------------------------------------------------------

void
CWailDocApp::OpenDocument(
	FSSpec	*inMacFSSpec )
{
	LDocument	*theDoc = LDocument::FindByFileSpec(*inMacFSSpec);
	
	if (theDoc != nil) {				// Document is already open
		theDoc->MakeCurrent();			// Make it the current document
		
	} else {							// Make a new Document
		// check document type.
		FInfo theFileInfo;
		ThrowIfOSErr_( FSpGetFInfo( inMacFSSpec, &theFileInfo ) );
		switch (theFileInfo.fdType)
		{
			case fileType_Marathon2Sound:
			case fileType_MarathonInfinitySound:
				theDoc = new CWailDocument(this, inMacFSSpec,
											theFileInfo.fdType, theFileInfo.fdCreator);
				break;
		}
	}
}


// ---------------------------------------------------------------------------------
//		¥ MakeNewDocument
// ---------------------------------------------------------------------------------

LModelObject *
CWailDocApp::MakeNewDocument()
{
	// Make a new empty document.
	return new CWailDocument( this, nil );
}


// ---------------------------------------------------------------------------------
//		¥ ChooseDocument
// ---------------------------------------------------------------------------------

void
CWailDocApp::ChooseDocument()
{
	// Configure allowed file type list.
	OSType theTypes[] = { fileType_Marathon2Sound,
						  fileType_MarathonInfinitySound };
	LFileTypeList theTypeList( 2, theTypes );
	
	// create a file chooser, configure options.
	PP_StandardDialogs::LFileChooser theFileChooser;
	NavDialogOptions* theDialogOptions = theFileChooser.GetDialogOptions();
	if (theDialogOptions != NULL)
	{
		theDialogOptions->dialogOptionFlags |= kNavNoTypePopup;
		theDialogOptions->dialogOptionFlags &= ~(kNavAllowPreviews);
	}
	
	// browse for a document.
	if (theFileChooser.AskOpenFile( theTypeList ))
	{
		// Send an apple event to open the file(s).
		for (SInt32 i = 1; i <= theFileChooser.GetNumberOfFiles(); ++i)
		{
			FSSpec theFile;
			theFileChooser.GetFileSpec( i, theFile );
			SendAEOpenDoc( theFile );
		}
	}
}
