// =================================================================================
//	UWailPreferences.cp					©1999, Charles Lechasseur
// =================================================================================
//
// Wail-specific prefs handling.

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

#include "UWailPreferences.h"

#include "WailTypes.h"

#include <LPageController.h>
#include <LMultiPanelView.h>

#include <UStandardDialogs.h>

#include "FullPath.h"

#include "UProcesses.h"


// instantiating static variables.

#pragma mark --- Static variables ---

CPreferences*	UWailPreferences::sPreferences = nil;
	
Boolean			UWailPreferences::sWarnBeforeDeleting;
Boolean			UWailPreferences::sWarnBeforeRemapping;
SInt16			UWailPreferences::sCompareSettings;
Boolean			UWailPreferences::sIgnoreCaseInNameFind;
Boolean			UWailPreferences::sExactMatchInNameFind;
Boolean			UWailPreferences::sWarnBeforeDeletingClass;
		
FSSpec			UWailPreferences::sDeprecatedClassNamesFile;
AliasHandle		UWailPreferences::sClassNamesFileAlias = nil;
		
SInt32			UWailPreferences::sRAMToProtect;


// default values for fields

#pragma mark --- Fields' Default Values ---

const Boolean			default_WarnBeforeDeleting			= TRUE;
const Boolean			default_WarnBeforeRemapping			= TRUE;
const SInt16			default_CompareSettings 			= compareSetting_Separately;
const Boolean			default_IgnoreCaseInNameFind		= TRUE;
const Boolean			default_ExactMatchInNameFind		= FALSE;
const Boolean			default_WarnBeforeDeletingClass		= TRUE;

FSSpec					default_DeprecatedClassNamesFile	= { 0, 0, "\p" };
const ResIDT			STR_DefaultDeprecatedClassNamesFile	= 1001;

const SInt32			default_RAMToProtect				= 1024L * 1024L;	// 1 meg.


// constants - prefs fields IDs.

#pragma mark --- Field IDs ---

const SInt16	fieldID_WarnBeforeDeleting			= 1000;
const SInt16	fieldID_WarnBeforeRemapping			= 1001;
const SInt16	fieldID_CompareSettings				= 1002;
const SInt16	fieldID_IgnoreCaseInNameFind		= 1003;
const SInt16	fieldID_ExactMatchInNameFind		= 1004;
const SInt16	fieldID_WarnBeforeDeletingClass		= 1005;

const SInt16	fieldID_DeprecatedClassNamesFile	= 2000;
const SInt16	fieldID_ClassNamesFileAlias			= 2001;

const SInt16	fieldID_RAMToProtect				= 3000;


// name of the Wail prefs file

#pragma mark --- Prefs file name ---

const ResIDT	STR_WailPrefsFileName			= 1000;


// dialog ID of the prefs dialog

#pragma mark --- Prefs dialog ID ---

const ResIDT	dialog_WailPrefs				= 1500;


// panes of the prefs dialog

#pragma mark --- Prefs dialog panes ---

const PaneIDT	pane_PageController						= 'Page';
const PaneIDT	pane_MultiPanelView						= 'Mult';

const PaneIDT	pane_WarnBeforeDeletingCheckBox			= 'Wa!S';
const PaneIDT	pane_WarnBeforeRemappingCheckBox		= 'Wa!R';
const PaneIDT	pane_WarnBeforeDeletingClassCheckBox	= 'Wa!C';
const PaneIDT	pane_CompareTogetherRadio				= '?Tog';
const PaneIDT	pane_CompareSeparatelyRadio				= '?Sep';

const PaneIDT	pane_ClassNamesFileNameCaption			= 'CnFN';
const PaneIDT	pane_ClassNamesFileChooseButton			= '?CFN';

const PaneIDT	pane_RAMToProtectEditField				= 'Rt<>';


// messages of the prefs dialog

#pragma mark --- Prefs dialog messages ---

const MessageT	msg_WarnBeforeDeleting					= 'Wa!S';
const MessageT	msg_WarnBeforeRemapping					= 'Wa!R';
const MessageT	msg_WarnBeforeDeletingClass				= 'Wa!C';
const MessageT	msg_CompareTogether						= '?Tog';
const MessageT	msg_CompareSeparately					= '?Sep';

const MessageT	msg_ChooseClassNamesFile				= '?CFN';

const MessageT	msg_RAMToProtectTextChanged				= 'Rt<>';


// =================================================================================
//	UWailPreferences class
// =================================================================================

#pragma mark -


// ---------------------------------------------------------------------------------
//		¥ Initialize
// ---------------------------------------------------------------------------------
// initializes the prefs system by creating an instance of a CPreferences object.
// also registers all prefs fields. does not actually read or reset the prefs.

void
UWailPreferences::Initialize()
{
	// create a CPreferences object.
	sPreferences = new CPreferences(
						LStr255( STR_WailPrefsFileName, 0 ),
						fileCreator_Wail,
						fileType_Prefs,
						/* create prefs folder? */ true );
	ThrowIfNil_( sPreferences );
	
	// the Class Names spec default is not really const. it's a global thing though,
	// but its name is stored in a resource - so we need to read it now.
	LString::CopyPStr( LStr255( STR_DefaultDeprecatedClassNamesFile, 0 ),
					   default_DeprecatedClassNamesFile.name );
					   
	// allocate memory for Handle-based fields.
	sClassNamesFileAlias = reinterpret_cast<AliasHandle>(::NewHandle( 0 ));
	ThrowIfNil_( sClassNamesFileAlias );
	
	// register all the prefs fiels.
	RegisterField( *sPreferences,
				   sWarnBeforeDeleting,
				   default_WarnBeforeDeleting,
				   fieldID_WarnBeforeDeleting ); 
	RegisterField( *sPreferences,
				   sWarnBeforeRemapping,
				   default_WarnBeforeRemapping,
				   fieldID_WarnBeforeRemapping );
	RegisterField( *sPreferences,
				   sCompareSettings,
				   default_CompareSettings,
				   fieldID_CompareSettings );
	RegisterField( *sPreferences,
				   sIgnoreCaseInNameFind,
				   default_IgnoreCaseInNameFind,
				   fieldID_IgnoreCaseInNameFind );
	RegisterField( *sPreferences,
				   sExactMatchInNameFind,
				   default_ExactMatchInNameFind,
				   fieldID_ExactMatchInNameFind );
	RegisterField( *sPreferences,
				   sWarnBeforeDeletingClass,
				   default_WarnBeforeDeletingClass,
				   fieldID_WarnBeforeDeletingClass );
							 
	RegisterField( *sPreferences,
				   sDeprecatedClassNamesFile,
				   default_DeprecatedClassNamesFile,
				   fieldID_DeprecatedClassNamesFile );
	sPreferences->RegisterHandleField( reinterpret_cast<Handle>(sClassNamesFileAlias),
									   ::NewHandle( 0 ),
									   fieldID_ClassNamesFileAlias );
							
	RegisterField( *sPreferences,
				   sRAMToProtect,
				   default_RAMToProtect,
				   fieldID_RAMToProtect );
}


// ---------------------------------------------------------------------------------
//		¥ Finalize
// ---------------------------------------------------------------------------------
// cleanup. call this at the end of your code (application's destructor).

void
UWailPreferences::Finalize()
{
	// delete the prefs object.
	if (sPreferences != nil)
		delete sPreferences;
		
	// deallocate Handle-based fields memory.
	::DisposeHandle( reinterpret_cast<Handle>(sClassNamesFileAlias) );
	sClassNamesFileAlias = nil;
}
		

// ---------------------------------------------------------------------------------
//		¥ Read
// ---------------------------------------------------------------------------------
// reads prefs from the file into memory.

void
UWailPreferences::Read()
{
	ThrowIfOSErr_( sPreferences->Read() );
}


// ---------------------------------------------------------------------------------
//		¥ Write
// ---------------------------------------------------------------------------------
// writes prefs from memory to the file.

void
UWailPreferences::Write()
{
	ThrowIfOSErr_( sPreferences->Write() );
}


// ---------------------------------------------------------------------------------
//		¥ Reset
// ---------------------------------------------------------------------------------
// reset prefs to their default values.

void
UWailPreferences::Reset()
{
	ThrowIfOSErr_( sPreferences->Reset() );
}
		

// ---------------------------------------------------------------------------------
//		¥ Edit
// ---------------------------------------------------------------------------------
// shows a dialog to allow users to edit the preferences.

void
UWailPreferences::Edit(
	LCommander*	inSuper )
{
	// save prefs state.
	ThrowIfOSErr_( sPreferences->SaveState() );

	// create the dialog.
	StWailPrefsDialogHandler theDialogHandler( inSuper );
	
	// loop until OK or Cancel is selected.
	MessageT message;
	do
	{
		message = theDialogHandler.DoDialog();
	} while ((message != msg_OK) && (message != msg_Cancel));
	
	// if user selected Cancel, we must reload the saved state.
	if (message == msg_Cancel)
		ThrowIfOSErr_( sPreferences->RestoreState() );
	else if (message == msg_OK)	// user selected ok. write prefs.
		Write();
}


// ---------------------------------------------------------------------------
//		¥ GetClassNamesFile
// ---------------------------------------------------------------------------
// returns the class names file spec. if possible, uses an alias; if the
// alias is not available, then fall back to the deprecated FSSpec-based solution.

void
UWailPreferences::GetClassNamesFile(
	FSSpec& outSpec )
{
	// check if we can use an alias.
	bool foundSpec = false;
	if (::GetHandleSize( reinterpret_cast<Handle>(sClassNamesFileAlias) ) > 0)
	{
		// get this app's FSSpec.
		FSSpec thisApp;
		OSErr err = UProcesses::GetCurrentProcessAppSpec( thisApp );
	
		// try resolving the alias to find a file, possibly relatively from our app.
		FSSpec theTempSpec;
		Boolean wasUpdated = FALSE;
		err = ::ResolveAlias( (err == noErr ? &thisApp : nil),
							  sClassNamesFileAlias,
							  &theTempSpec,
							  &wasUpdated );
		if (err == noErr)
		{
			// got spec, copy it to caller.
			outSpec = theTempSpec;
			foundSpec = true;
			
			// if alias was updated, we might need to save that.
			if (wasUpdated)
			{
				// set deprecated field so it's updated too.
				sDeprecatedClassNamesFile = outSpec;
			
				try
				{
					Write();
				}
				
				catch (...) { }	// if that didn't work, the spec will still be valid.
			}
		}
	}
	
	// if we didn't find the spec using the alias, use the deprecated FSSpec field.
	if (!foundSpec)
	{
		outSpec = sDeprecatedClassNamesFile;
		
		// call Setter to create an alias to this spec - next time the alias
		// will be valid. then, write the prefs.
		try
		{
			SetClassNamesFile( outSpec );
			Write();
		}
		
		catch (...) { }	// if that didn't work, the spec will still be valid.
	}
}


// ---------------------------------------------------------------------------
//		¥ SetClassNamesFile
// ---------------------------------------------------------------------------
// sets the class names file spec. creates an alias to this file to save in
// the prefs and also saves it in the deprecated FSSpec field.

void
UWailPreferences::SetClassNamesFile(
	const FSSpec& inSpec )
{
	// get this app's FSSpec.
	FSSpec thisApp;
	OSErr err = UProcesses::GetCurrentProcessAppSpec( thisApp );

	// try creating an alias to the file, with a possible relative path to our app.
	AliasHandle theAliasH;
	err = ::NewAlias( (err == noErr ? &thisApp : nil), &inSpec, &theAliasH );
	if (err == noErr)
	{
		// alias created successfully, copy it to the field.
		(**theAliasH).userType = fileCreator_Wail;
		StHandleLocker theAliasLocker( reinterpret_cast<Handle>(theAliasH) );
		err = ::PtrToXHand( *theAliasH,
							reinterpret_cast<Handle>(sClassNamesFileAlias),
							(**theAliasH).aliasSize );
		if (err != noErr)
		{
			// failed to copy alias to field; reset field.
			::SetHandleSize( reinterpret_cast<Handle>(sClassNamesFileAlias), 0 );
		}
	}
	else
	{
		// couldn't create an alias; reset field.
		::SetHandleSize( reinterpret_cast<Handle>(sClassNamesFileAlias), 0 );
	}
	
	// also save the file in the deprecated FSSpec field for old versions.
	sDeprecatedClassNamesFile = inSpec;
}


#pragma mark -


// =================================================================================
//	StWailPrefsDialogHandler
// =================================================================================
// subclass of StDialogHandler for displaying Wail's prefs dialog.
// call DoDialog, waiting for msg_OK or msg_Cancel - all other messages are handled
// internally by this class.
//
// note: prefs are modified on-the-fly when panes are clicked. call the prefs'
// SaveState routine to save the state of you wish to be able to Cancel after the dialog
// is done.


// ---------------------------------------------------------------------------------
//		¥ Constructor
// ---------------------------------------------------------------------------------
// reads all prefs fields and sets the various panes accordingly.
// then, shows the dialog.

StWailPrefsDialogHandler::StWailPrefsDialogHandler(
	LCommander		*inSuper )
	: StDialogHandler( dialog_WailPrefs, inSuper )
{
	// link the page controller to the muti-panel view.
	LinkMultiPanelViewToPageController();
	
	// get pointers to panes.
	GetPanePointers();
	
	// init panes according to prefs.
	InitPanesAccordingToPrefs();	// duh-like comments :)
	
	// link panes to this object so we listen to them.
	ListenToPanes();
	
	// show the dialog.
	GetDialog()->Show();
}


// ---------------------------------------------------------------------------------
//		¥ Destructor
// ---------------------------------------------------------------------------------

StWailPrefsDialogHandler::~StWailPrefsDialogHandler()
{
}


// ---------------------------------------------------------------------------------
//		¥ ListenToMessage
// ---------------------------------------------------------------------------------
// calls HandleMessage with the message received.
// if HandleMessages handles the message, don't store it.
// otherwise, store it as usual to have to returned to caller.

void
StWailPrefsDialogHandler::ListenToMessage(
	MessageT	inMessage,
	void*		ioParam )
{
	if (!HandleMessage( inMessage ))							// if not handled...
		StDialogHandler::ListenToMessage( inMessage, ioParam );	// ...store it.
}


// ---------------------------------------------------------------------------------
//		¥ LinkMultiPanelViewToPageController
// ---------------------------------------------------------------------------------
// makes the MultiPanelView listen to the PageController so it automatically
// switches pages when the user selects one.

void
StWailPrefsDialogHandler::LinkMultiPanelViewToPageController()
{
	LPageController* thePageController =
		dynamic_cast<LPageController*> (GetDialog()->FindPaneByID( pane_PageController ));
	ThrowIfNil_( thePageController );
	LMultiPanelView* theMultiPanelView =
		dynamic_cast<LMultiPanelView*> (GetDialog()->FindPaneByID( pane_MultiPanelView ));
	ThrowIfNil_( theMultiPanelView );
	
	thePageController->AddListener( this );	// JIT (just-in-case) programmingª
	thePageController->AddListener( theMultiPanelView );
	
	// make sure to create all panels so we can store pointers to their panes.
	theMultiPanelView->CreateAllPanels();
}


// ---------------------------------------------------------------------------------
//		¥ GetPanePointers
// ---------------------------------------------------------------------------------
// stores pointers to the dialog's panes in our member variables, for later retrieval.

void
StWailPrefsDialogHandler::GetPanePointers()
{
	mWarnBeforeDeletingCheckBox =
		dynamic_cast<LControl*> (GetDialog()->FindPaneByID( pane_WarnBeforeDeletingCheckBox ));
	ThrowIfNil_( mWarnBeforeDeletingCheckBox );
	mWarnBeforeRemappingCheckBox =
		dynamic_cast<LControl*> (GetDialog()->FindPaneByID( pane_WarnBeforeRemappingCheckBox ));
	ThrowIfNil_( mWarnBeforeRemappingCheckBox );
	mWarnBeforeDeletingClassCheckBox =
		dynamic_cast<LControl*> (GetDialog()->FindPaneByID( pane_WarnBeforeDeletingClassCheckBox ));
	ThrowIfNil_( mWarnBeforeDeletingClassCheckBox );
	mCompareTogetherRadio =
		dynamic_cast<LControl*> (GetDialog()->FindPaneByID( pane_CompareTogetherRadio ));
	ThrowIfNil_( mCompareTogetherRadio );
	mCompareSeparatelyRadio =
		dynamic_cast<LControl*> (GetDialog()->FindPaneByID( pane_CompareSeparatelyRadio ));
	ThrowIfNil_( mCompareSeparatelyRadio );
		
	mClassNamesFileNameCaption =
		dynamic_cast<LPane*> (GetDialog()->FindPaneByID( pane_ClassNamesFileNameCaption ));
	ThrowIfNil_( mClassNamesFileNameCaption );
	mClassNamesFileChooseButton =
		dynamic_cast<LControl*> (GetDialog()->FindPaneByID( pane_ClassNamesFileChooseButton ));
	ThrowIfNil_( mClassNamesFileChooseButton );
		
	mRAMToProtectEditField =
		dynamic_cast<CBroadcastingEditField*> (GetDialog()->FindPaneByID( pane_RAMToProtectEditField ));
	ThrowIfNil_( mRAMToProtectEditField );
}


// ---------------------------------------------------------------------------------
//		¥ ListenToPanes
// ---------------------------------------------------------------------------------
// makes this object listen to all prefs panes.

void
StWailPrefsDialogHandler::ListenToPanes()
{
	mWarnBeforeDeletingCheckBox->AddListener( this );
	mWarnBeforeRemappingCheckBox->AddListener( this );
	mWarnBeforeDeletingClassCheckBox->AddListener( this );
	mCompareTogetherRadio->AddListener( this );
	mCompareSeparatelyRadio->AddListener( this );
	
	mClassNamesFileChooseButton->AddListener( this );
	
	mRAMToProtectEditField->AddListener( this );
}


// ---------------------------------------------------------------------------------
//		¥ InitPanesAccordingToPrefs
// ---------------------------------------------------------------------------------
// sets the pane's original states according to Wail prefs.

void
StWailPrefsDialogHandler::InitPanesAccordingToPrefs()
{
	// all checkboxes are originally unchecked.
	if (UWailPreferences::WarnBeforeDeleting())
		mWarnBeforeDeletingCheckBox->SetValue( Button_On );
	if (UWailPreferences::WarnBeforeRemapping())
		mWarnBeforeRemappingCheckBox->SetValue( Button_On );
	if (UWailPreferences::WarnBeforeDeletingClass())
		mWarnBeforeDeletingClassCheckBox->SetValue( Button_On );
		
	// radios are originally unselected.
	switch (UWailPreferences::CompareSettings())
	{
		case compareSetting_Together:
			mCompareTogetherRadio->SetValue( Button_On );
			break;
			
		case compareSetting_Separately:
			mCompareSeparatelyRadio->SetValue( Button_On );
			break;
	}
		
	FSSpec theSpec;
	UWailPreferences::GetClassNamesFile( theSpec );
	SetClassNamesFile( theSpec, false );
	
	mRAMToProtectEditField->SetValue( UWailPreferences::RAMToProtect() / 1024L );
}


// ---------------------------------------------------------------------------------
//		¥ HandleMessage
// ---------------------------------------------------------------------------------
// called by DoDialog.
// if the message relates to a pane that changes a pref, changes that pref
// and return true. else return false.
// continue with event processing if this function returns false.

Boolean
StWailPrefsDialogHandler::HandleMessage(
	MessageT	inMessage )
{
	Boolean handled = false;	// assume we won't.
	
	switch (inMessage)
	{
		case msg_WarnBeforeDeleting:
			UWailPreferences::SetWarnBeforeDeleting(
				mWarnBeforeDeletingCheckBox->GetValue() == Button_On );
			handled = true;
			break;
			
		case msg_WarnBeforeRemapping:
			UWailPreferences::SetWarnBeforeRemapping(
				mWarnBeforeRemappingCheckBox->GetValue() == Button_On );
			handled = true;
			break;
			
		case msg_WarnBeforeDeletingClass:
			UWailPreferences::SetWarnBeforeDeletingClass(
				mWarnBeforeDeletingClassCheckBox->GetValue() == Button_On );
			handled = true;
			break;
			
		case msg_CompareTogether:
			if (mCompareTogetherRadio->GetValue() == Button_On)
				UWailPreferences::SetCompareSettings( compareSetting_Together );
			handled = true;
			break;
		
		case msg_CompareSeparately:
			if (mCompareSeparatelyRadio->GetValue() == Button_On)
				UWailPreferences::SetCompareSettings( compareSetting_Separately );
			handled = true;
			break;
			
		case msg_ChooseClassNamesFile:
			ChooseClassNamesFile();
			handled = true;
			break;
			
		case msg_RAMToProtectTextChanged:
			UWailPreferences::SetRAMToProtect(
				mRAMToProtectEditField->GetValue() * 1024L );
			handled = true;
			break;
	}
	
	return handled;
}


// ---------------------------------------------------------------------------------
//		¥ ChooseClassNamesFile
// ---------------------------------------------------------------------------------
// presents the user with a selection dialog, allowing to select the Class Names file.
// if user selects ok, saves it in the prefs and changes the name caption accordingly.

void
StWailPrefsDialogHandler::ChooseClassNamesFile()
{
	// ask user to select a file.
	FSSpec theFile;
	if (PP_StandardDialogs::AskChooseOneFile( fileType_Text,
											  theFile,
											  kNavDefaultNavDlogOptions & ~(kNavAllowPreviews) ))
	{
		// set it.
		SetClassNamesFile( theFile );
	}
}


// ---------------------------------------------------------------------------
//		¥ SetClassNamesFile
// ---------------------------------------------------------------------------
// sets the class names file. call when the user chose a new file. will update
// the dialog's display and set the file in the prefs (unless inUpdatePrefs
// is set to false).

void
StWailPrefsDialogHandler::SetClassNamesFile(
	const FSSpec&	inFile,
	bool			inUpdatePrefs /*= true*/ )
{
	// set file in the prefs.
	if (inUpdatePrefs)
		UWailPreferences::SetClassNamesFile( inFile );
	
	// get full path to the file to update display.
	Handle theFullPathH = nil;
	short theFullPathLength = 0;
	ThrowIfOSErr_( ::FSpGetFullPath( &inFile, &theFullPathLength, &theFullPathH ) );
	
	// make sure handle is deallocated.
	StHandleBlock theFullPathDeleter( theFullPathH );
	
	// convert full path to a string.
	LStr255 theFullPath( theFullPathH );
	
	// set full path in caption.
	mClassNamesFileNameCaption->SetDescriptor( theFullPath );
}
								