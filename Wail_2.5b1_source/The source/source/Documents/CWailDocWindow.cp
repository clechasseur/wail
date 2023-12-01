// =================================================================================
//	CWailDocWindow.cp					©1999, Charles Lechasseur
// =================================================================================
//
// CWailDocWindow objects will be tied to CWailDocument objects and will be responsible
// for most of the editing of sound files. when created, they will be passed a filled
// CWailSoundFileData object and will take care of editing it. all document-related
// tasks like saving, opening, etc. will be handled by CWailDocument objects.
//
// we store in this object a pointer to a CWailSoundFileData object. however it is
// not initialized at all when creating the window. the CWailDocument object will
// read this from a sound file and set it using the SetSoundFileData member function.

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


#include "CWailDocWindow.h"

#include "UWailActions.h"

#include "UWailClassNames.h"

#include <UModalDialogs.h>
#include <LFileTypeList.h>
#include <UStandardDialogs.h>

#include "UJukebox.h"

#include "CWailProgressDialog.h"
#include "CWailWindowChooser.h"

#include "CSingleColumnTextListBox.h"
#include "CBroadcastingEditField.h"
#include "CBroadcastingListBox.h"
#include "CInFileStream.h"
#include "StOldResFile.h"
#include "UCode99.h"
#include "UBroadcasting.h"
#include "UMoreDrawingState.h"

#include "CWailSoundStream.h"

#include "C_PatchFile.h"

#include "UWailPreferences.h"
#include "UMoreModalDialogs.h"
#include "UWailFind.h"

#include "WailCommands.h"
#include "WailTypes.h"
#include "WailShuttleConstants.h"
#include "WailMessages.h"


// ---------------------------------------------------------------------------
//	¥ CWailDocWindow							Constructor	[public]
// ---------------------------------------------------------------------------

CWailDocWindow::CWailDocWindow()
{
	InitWailDocWindow();
}


// ---------------------------------------------------------------------------------
//	¥ CWailDocWindow
// ---------------------------------------------------------------------------------
// first constructor derived from LWindow. i don't know what it does :)

CWailDocWindow::CWailDocWindow(
	ResIDT				inWINDid,
	UInt32				inAttributes,
	LCommander			*inSuperCommander)
	: CTrackableWindow(inWINDid, inAttributes, inSuperCommander)
{
	InitWailDocWindow();
}
							

// ---------------------------------------------------------------------------------
//	¥ CWailDocWindow
// ---------------------------------------------------------------------------------
// second constructor derived from LWindow. i don't know what it does :)
					
CWailDocWindow::CWailDocWindow(
	LCommander			*inSuperCommander,
	const Rect			&inGlobalBounds,
	ConstStringPtr		inTitle,
	SInt16				inProcID,
	UInt32				inAttributes,
	WindowPtr			inBehind )
	: CTrackableWindow(inSuperCommander, inGlobalBounds, inTitle, inProcID,
			inAttributes, inBehind)
{
	InitWailDocWindow();
}


// ---------------------------------------------------------------------------
//	¥ CWailDocWindow					Stream Constructor	[public]
// ---------------------------------------------------------------------------

CWailDocWindow::CWailDocWindow(
	LStream *inStream )
	: CTrackableWindow(inStream)
{
	InitWailDocWindow();
}


// ---------------------------------------------------------------------------
//	¥ CWailDocWindow						Copy Constructor	[public]
// ---------------------------------------------------------------------------

CWailDocWindow::CWailDocWindow(
	const CWailDocWindow& inOriginal )
	: CTrackableWindow(inOriginal)
{
	InitWailDocWindow();
}


// ---------------------------------------------------------------------------
//	¥ ~CWailDocWindow							Destructor	[public]
// ---------------------------------------------------------------------------

CWailDocWindow::~CWailDocWindow()
{
	// if our sound file data exists, destroy it.
	DeleteSoundFileData();
}


// ---------------------------------------------------------------------------------
//		¥ ListenToMessage
// ---------------------------------------------------------------------------------
// we are listening to message from our controls.

void
CWailDocWindow::ListenToMessage(
	MessageT	inMessage,
	void		*ioParam)
{
	// we only listen to messages when we have a sound file data. in fact, it's
	// a mistake if this function gets called when mSoundFileData is nil... so:
	ThrowIfNil_( mSoundFileData );
	
	if (mSoundFileData != nil)
	{
		// extract the current class.
		CWailSoundClass *theClass;
		mSoundFileData->mSoundClasses.FetchItemAt( mCurrentClass + 1, theClass );
		
		// declare a few local variables we might need.
		LPane *thePane;
		SInt32 theSelectedClass;
		Boolean isEnabled;
		Boolean isUsingMark;
		UInt16 theMark;
		Str255 theName;
		
		// handle message.
		switch (inMessage)
		{
			// messages from controls:
			
			case msg_ClassListSingleClick:
				// see what class is selected.
				thePane = FindPaneByID( pane_ClassList );
				theSelectedClass = thePane->GetValue();	// value is selected item for ListBoxes.
				// see if it's changed.
				if (theSelectedClass != mCurrentClass)
				{
					// it's changed, select the new class.
					SelectClass( theSelectedClass );
				}
				break;
				
			case msg_ClassListDoubleClick:
				// what do i care if you double-click? :)
				break;
				
			case msg_8bitSoundsListDoubleClick:
				ObeyCommand( cmd_Play8bitSound, ioParam );
				break;
			
			case msg_16bitSoundsListDoubleClick:
				ObeyCommand( cmd_Play16bitSound, ioParam );
				break;
				
			case msg_SoundsListDeleteKey:
				// check if we can delete sounds.
				FindCommandStatus( cmd_DeleteSound, isEnabled, isUsingMark, theMark, theName );
				if (isEnabled)
					ObeyCommand( cmd_DeleteSound, ioParam );
				break;
				
			case msg_SoundsListEnterKey:
				// check if we can play sounds.
				FindCommandStatus( cmd_PlaySound, isEnabled, isUsingMark, theMark, theName );
				if (isEnabled)
					ObeyCommand( cmd_PlaySound, ioParam );
				break;
				
			case msg_Send8bitDrag:
				HandleDragSend( *((SMouseDownEvent*) ioParam), true );
				break;
				
			case msg_Receive8bitDrag:
				HandleDragReceive( *((Handle*) ioParam), true );
				break;
				
			case msg_Send16bitDrag:
				HandleDragSend( *((SMouseDownEvent*) ioParam), false );
				break;
				
			case msg_Receive16bitDrag:
				HandleDragReceive( *((Handle*) ioParam), false );
				break;
				
			case msg_Flag1CheckBox:
			case msg_Flag2CheckBox:
			case msg_Flag3CheckBox:
			case msg_Flag4CheckBox:
			case msg_Flag5CheckBox:
			case msg_Flag6CheckBox:
			case msg_Flag7CheckBox:
				// all handled together now.
				HandleFlagChange( inMessage, theClass );
				break;
				
			case msg_ClassIDEditFieldChanged:
				if (!mIsChangingClass) // we don't listen to those when we're changing class.
				{
					// read class ID and set it.
					LEditField* theEditField = (LEditField*) FindPaneByID( pane_ClassIDEditField );
					theClass->mClassID = (SInt16) theEditField->GetValue();
					SetDirty( true ); // we're dirty.
				}
				break;
				
			case msg_ClassIDEditFieldEnterPressed:
			case msg_ClassIDEditFieldLostTarget:
				if (!mIsChangingClass) // we don't listen to those when we're changing class.
				{
					// TODO: handle those
				}
				break;
				
			case msg_ChancePopupMenuChanged:
				if (!mIsChangingClass) // we don't listen to those when we're changing class.
				{
					// get old & new Chance values.
					SInt16 theOldValue = theClass->mChance;
					SInt16 theNewValue = GetChance();
					
					// set new value.
					theClass->mChance = theNewValue;
					
					// we're dirty.
					SetDirty( true );
					
					// if we're not performing an action, post one to
					// allow undo/redo of this change.
					if (!mIsPerformingAction)
						ActionChangeChance( theOldValue, theNewValue );
				}
				break;
				
			case msg_LowPitchEditFieldChanged:
				if (!mIsChangingClass) // we don't listen to those when we're changing class.
				{
					// read low pitch and set it.
					LEditField* theEditField = (LEditField*) FindPaneByID( pane_LowPitchEditField );
					theClass->mLowPitch = (Fixed) theEditField->GetValue();
					SetDirty( true ); // we're dirty.
				}
				break;
				
			case msg_HighPitchEditFieldChanged:
				if (!mIsChangingClass) // we don't listen to those when we're changing class.
				{
					// read high pitch and set it.
					LEditField* theEditField = (LEditField*) FindPaneByID( pane_HighPitchEditField );
					theClass->mHighPitch = (Fixed) theEditField->GetValue();
					SetDirty( true ); // we're dirty.
				}
				break;
				
			case msg_SoftVolumeRadioButton:
			case msg_MediumVolumeRadioButton:
			case msg_LoudVolumeRadioButton:
			case msg_Volume3RadioButton:
				// all handled together now.
				HandleVolumeChange( inMessage, theClass );
				break;
				
			case msg_Remap8bitCheckBox:
				HandleRemapping( theClass );
				break;
		}
	}
}


// ---------------------------------------------------------------------------
//		¥ ObeyCommand
// ---------------------------------------------------------------------------
//	Handle a command

Boolean
CWailDocWindow::ObeyCommand(
	CommandT	inCommand,
	void		*ioParam)
{
	Boolean		cmdHandled = true;
	
	switch (inCommand) {
	
		case cmd_CompareWithFile:
			cmdHandled = CompareWithAnotherFile();
			break;
			
		case cmd_CompareWithWindow:
			cmdHandled = CompareWithAnotherWindow();
			break;
		
		case cmd_AddClass:
			cmdHandled = AddClass();
			break;
			
		case cmd_AddClasses:
			cmdHandled = AddClasses();
			break;
			
		case cmd_RemoveClass:
			cmdHandled = RemoveClass();
			break;
			
		case cmd_FindClassByID:
			cmdHandled = FindClassByID();
			break;
			
		case cmd_FindClassByName:
			cmdHandled = FindClassByName();
			break;
			
		case cmd_FindClassByNumber:
			cmdHandled = FindClassByNumber();
			break;
			
		case cmd_DeleteSound:
			cmdHandled = ObeyCommand( cmd_Clear, ioParam );
			break;
		
		// for sound-related commands, we must
		// handle them differently depending on the active sound list (8-bit or 16-bit).
		
		case cmd_Cut:
		case cmd_Copy:
		case cmd_Paste:
		case cmd_Clear:
		case cmd_PlaySound:
		case cmd_LoopSound:
		case cmd_ImportSys7Sound:
		case cmd_ExportSys7Sound:
		case cmd_ImportAiffSound:
		case cmd_ExportAiffSound:
			LListBox *theListBox = (LListBox*) LCommander::GetTarget();
			Boolean is8bitList = (theListBox == (LListBox*) FindPaneByID( pane_8bitSoundsList ));
			switch (inCommand) // again
			{
				case cmd_Cut:
				case cmd_Copy:
				case cmd_Clear:
					cmdHandled = CutCopyClearSound( inCommand, is8bitList );
					break;
					
				case cmd_Paste:
					cmdHandled = PasteSound( is8bitList );
					break;
					
				case cmd_PlaySound:
				case cmd_LoopSound:
					cmdHandled = PlaySound( inCommand == cmd_LoopSound, is8bitList );
					break;
					
				case cmd_ImportSys7Sound:
					cmdHandled = ImportSys7Sound( is8bitList );
					break;
				
				case cmd_ExportSys7Sound:
					cmdHandled = ExportSys7Sound( is8bitList );
					break;
				
				case cmd_ImportAiffSound:
					cmdHandled = ImportAiffSound( is8bitList );
					break;
				
				case cmd_ExportAiffSound:
					cmdHandled = ExportAiffSound( is8bitList );
					break;
			}
			break;
			
		// backward compatibility:
		
		case cmd_Play8bitSound:
		case cmd_Play16bitSound:
			cmdHandled = ObeyCommand( cmd_PlaySound, ioParam );
			break;
			
		// other commands:
		
		default:
			cmdHandled = LWindow::ObeyCommand(inCommand, ioParam);
			break;
	}
		
	return cmdHandled;
}


// ---------------------------------------------------------------------------
//		¥ FindCommandStatus
// ---------------------------------------------------------------------------
//	Pass back whether a Command is enabled and/or marked (in a Menu)

void
CWailDocWindow::FindCommandStatus(
	CommandT	inCommand,
	Boolean		&outEnabled,
	Boolean		&outUsesMark,
	UInt16		&outMark,
	Str255		outName)
{
	// declare a few member variables we might need.
	LListBox *theListBox;

	switch (inCommand) {
	
		case cmd_CompareWithFile:
			outEnabled = true;	// always possible.
			break;
		
		case cmd_CompareWithWindow:
			outEnabled = CWailWindowChooser::HasMoreThanOneWindow();
			break;
	
		case cmd_Cut:
		case cmd_Copy:
		case cmd_Clear:
		case cmd_DeleteSound:
		case cmd_PlaySound:
		case cmd_LoopSound:
			// a sound list box must be targeted, and have an item selected.
			theListBox = (LListBox*) LCommander::GetTarget();
			if ( (theListBox == (LListBox*) FindPaneByID( pane_8bitSoundsList )) ||
				 (theListBox == (LListBox*) FindPaneByID( pane_16bitSoundsList )))
			{
				outEnabled = (theListBox->GetValue() != -1);
			}
			break;
			
		case cmd_Paste:
			// a sound list must be targeted, and have room for a sound.
			// oops: the scrap must also have a sound :)
			theListBox = (LListBox*) LCommander::GetTarget();
			if (theListBox == (LListBox*) FindPaneByID( pane_8bitSoundsList ))
			{				
				CWailSoundClass *theClass;
				LClipboard theClipboard;
				mSoundFileData->mSoundClasses.FetchItemAt( mCurrentClass + 1, theClass );
				outEnabled = (theClass->mNum8bitSounds < 5) &&
							 (theClipboard.GetData( 'snd ', nil ) != 0);
			}
			else if (theListBox == (LListBox*) FindPaneByID( pane_16bitSoundsList ))
			{
				CWailSoundClass *theClass;
				LClipboard theClipboard;
				mSoundFileData->mSoundClasses.FetchItemAt( mCurrentClass + 1, theClass );
				outEnabled = (theClass->mNum16bitSounds < 5) &&
							 (theClipboard.GetData( 'snd ', nil ) != 0);
			}
			break;
			
		case cmd_SelectAll:
			outEnabled = false;
			break;
			
		case cmd_AddClass:
		case cmd_AddClasses:
			outEnabled = true; // we can always add a class.
			break;
			
		case cmd_RemoveClass:
			// a class must be selected.
			outEnabled = (mCurrentClass != -1);
			break;
			
		case cmd_FindClassByID:
		case cmd_FindClassByName:
		case cmd_FindClassByNumber:
			outEnabled = true; // we can always search the class list.
			break;
			
		case cmd_ImportSys7Sound:
		case cmd_ImportAiffSound:
			// a sound list must be targeted, and have room for a sound.
			theListBox = (LListBox*) LCommander::GetTarget();
			if (theListBox == (LListBox*) FindPaneByID( pane_8bitSoundsList ))
			{				
				CWailSoundClass *theClass;
				mSoundFileData->mSoundClasses.FetchItemAt( mCurrentClass + 1, theClass );
				outEnabled = (theClass->mNum8bitSounds < 5);
			}
			else if (theListBox == (LListBox*) FindPaneByID( pane_16bitSoundsList ))
			{
				CWailSoundClass *theClass;
				mSoundFileData->mSoundClasses.FetchItemAt( mCurrentClass + 1, theClass );
				outEnabled = (theClass->mNum16bitSounds < 5);
			}
			break;
			
		case cmd_ExportSys7Sound:
		case cmd_ExportAiffSound:
			FindCommandStatus( cmd_Copy, outEnabled, outUsesMark, outMark, outName );
				// same requirements as Copy.
			break;
	
		default:
			LWindow::FindCommandStatus(inCommand, outEnabled,
									outUsesMark, outMark, outName);
			break;
	}
}


// ---------------------------------------------------------------------------------
//		¥ SelectClass
// ---------------------------------------------------------------------------------
// selects the given class and refreshes all panes in the window.
//
// if inDoRefreshClassList is true, we must also refresh the Class ListBox.

void
CWailDocWindow::SelectClass(
	SInt32		inWhichClass,
	Boolean		inDoRefreshClassList )
{
	if (inDoRefreshClassList)
	{
		// we must select the Class in the ListBox, scroll it into view,
		// and refresh the pane.
		LListBox* theListBox = (LListBox*) FindPaneByID( pane_ClassList );
		Cell theCell = {(SInt16) inWhichClass, 0};
		theListBox->SelectOneCell( theCell );
		theListBox->MakeCellVisible( theCell );
		theListBox->Refresh();
	}

	// first, we must reset all class panes.
	StValueChanger<Boolean> setChangingClass( mIsChangingClass, true );
	
	if (inWhichClass == -1)
		ResetAllClassPanes();
	else {
		Clear8bitSoundsList();
		Clear16bitSoundsList();
	}
	mCurrentClass = inWhichClass;
	
	// now, if a new class is selected, fill panes.
	if (inWhichClass != -1)
		FillAllClassPanes();
}


// ---------------------------------------------------------------------------------
//		¥ FillAllClassPanes
// ---------------------------------------------------------------------------------
// fills all panes that display class information to their correct state, according
// to our data.

void
CWailDocWindow::FillAllClassPanes()
{
	// get the current class.
	CWailSoundClass *theClass;
	mSoundFileData->mSoundClasses.FetchItemAt( mCurrentClass + 1, theClass );
	
	// fill sound lists.
	Fill8bitSoundsList();
	Fill16bitSoundsList();
	
	// set controls.
	LControl *theControl;
	
	theControl = (LControl*) FindPaneByID( pane_Flag1CheckBox );
	theControl->Enable();
	theControl->SetValue( ::BitAnd( theClass->mFlags, flag_CannotBeRestarted )
							== flag_CannotBeRestarted );
	
	theControl = (LControl*) FindPaneByID( pane_Flag2CheckBox );
	theControl->Enable();
	theControl->SetValue( ::BitAnd( theClass->mFlags, flag_ResistsPitchChange )
							== flag_ResistsPitchChange );
	
	theControl = (LControl*) FindPaneByID( pane_Flag3CheckBox );
	theControl->Enable();
	theControl->SetValue( ::BitAnd( theClass->mFlags, flag_CannotChangePitch )
							== flag_CannotChangePitch );
	
	theControl = (LControl*) FindPaneByID( pane_Flag4CheckBox );
	theControl->Enable();
	theControl->SetValue( ::BitAnd( theClass->mFlags, flag_NoObstruct )
							== flag_NoObstruct );
	
	theControl = (LControl*) FindPaneByID( pane_Flag5CheckBox );
	theControl->Enable();
	theControl->SetValue( ::BitAnd( theClass->mFlags, flag_NoMediaObstruct )
							== flag_NoMediaObstruct );
	
	theControl = (LControl*) FindPaneByID( pane_Flag6CheckBox );
	theControl->Enable();
	theControl->SetValue( ::BitAnd( theClass->mFlags, flag_IsAmbient )
							== flag_IsAmbient );
							
	theControl = (LControl*) FindPaneByID( pane_Flag7CheckBox );
	theControl->Enable();
	theControl->SetValue( ::BitAnd( theClass->mFlags, flag_DoesntSelfAbort )
							== flag_DoesntSelfAbort );
	
	theControl = (LControl*) FindPaneByID( pane_SoftVolumeRadioButton );
	theControl->Enable();
	theControl->SetValue( theClass->mVolume == volume_Soft );
	
	theControl = (LControl*) FindPaneByID( pane_MediumVolumeRadioButton );
	theControl->Enable();
	theControl->SetValue( theClass->mVolume == volume_Medium );
	
	theControl = (LControl*) FindPaneByID( pane_LoudVolumeRadioButton );
	theControl->Enable();
	theControl->SetValue( theClass->mVolume == volume_Loud );
	
	theControl = (LControl*) FindPaneByID( pane_Volume3RadioButton );
	theControl->Enable();
	theControl->SetValue( theClass->mVolume == volume_Volume3 );
	
	theControl = (LControl*) FindPaneByID( pane_Remap8bitCheckBox );
	theControl->Enable();
	if (theClass->mRemap8bit)
	{
		theControl->SetValue( 1 );
		CSingleColumnTextListBox *theListBox
			= (CSingleColumnTextListBox*) FindPaneByID( pane_16bitSoundsList );
		theListBox->Disable();
	}
	else
	{
		theControl->SetValue( 0 );
	}
							
	// set class ID and class number.
	LEditField* theEditField;
	theEditField = (LEditField*) FindPaneByID( pane_ClassIDEditField );
	theEditField->Enable();
	theEditField->SetValue( theClass->mClassID );
	
	LCaption* theCaption;
	theCaption = (LCaption*) FindPaneByID( pane_ClassNumberCaption );
	theCaption->SetValue( mCurrentClass );
	
	// set chance, low and high pitch.
	SetChance( theClass->mChance, state_Enable, false );
	
	theEditField = (LEditField*) FindPaneByID( pane_LowPitchEditField );
	theEditField->Enable();
	theEditField->SetValue( theClass->mLowPitch );
	
	theEditField = (LEditField*) FindPaneByID( pane_HighPitchEditField );
	theEditField->Enable();
	theEditField->SetValue( theClass->mHighPitch );
}


// ---------------------------------------------------------------------------------
//		¥ ResetAllClassPanes
// ---------------------------------------------------------------------------------
// resets all panes that display class information to their natural state - i.e.,
// empty lists, 0-value controls, class ID of -1.

void
CWailDocWindow::ResetAllClassPanes()
{
	// clear sound lists.
	Clear8bitSoundsList();
	Clear16bitSoundsList();
	
	// reset controls.
	LControl *theControl;
	
	theControl = (LControl*) FindPaneByID( pane_Flag1CheckBox );
	theControl->SetValue( 0 );
	theControl->Disable();
	
	theControl = (LControl*) FindPaneByID( pane_Flag2CheckBox );
	theControl->SetValue( 0 );
	theControl->Disable();
	
	theControl = (LControl*) FindPaneByID( pane_Flag3CheckBox );
	theControl->SetValue( 0 );
	theControl->Disable();
	
	theControl = (LControl*) FindPaneByID( pane_Flag4CheckBox );
	theControl->SetValue( 0 );
	theControl->Disable();
	
	theControl = (LControl*) FindPaneByID( pane_Flag5CheckBox );
	theControl->SetValue( 0 );
	theControl->Disable();
	
	theControl = (LControl*) FindPaneByID( pane_Flag6CheckBox );
	theControl->SetValue( 0 );
	theControl->Disable();
	
	theControl = (LControl*) FindPaneByID( pane_Flag7CheckBox );
	theControl->SetValue( 0 );
	theControl->Disable();
	
	theControl = (LControl*) FindPaneByID( pane_SoftVolumeRadioButton );
	theControl->SetValue( 0 );
	theControl->Disable();
	
	theControl = (LControl*) FindPaneByID( pane_MediumVolumeRadioButton );
	theControl->SetValue( 0 );
	theControl->Disable();
	
	theControl = (LControl*) FindPaneByID( pane_LoudVolumeRadioButton );
	theControl->SetValue( 0 );
	theControl->Disable();
	
	theControl = (LControl*) FindPaneByID( pane_Volume3RadioButton );
	theControl->SetValue( 0 );
	theControl->Disable();
	
	theControl = (LControl*) FindPaneByID( pane_Remap8bitCheckBox );
	theControl->SetValue( 0 );
	theControl->Disable();
	
	// reset class ID and class number.
	LEditField* theEditField;
	theEditField = (LEditField*) FindPaneByID( pane_ClassIDEditField );
	theEditField->SetDescriptor( "\p-1" );
	theEditField->Disable();
	
	LCaption* theCaption;
	theCaption = (LCaption*) FindPaneByID( pane_ClassNumberCaption );
	theCaption->SetDescriptor( "\p-1" );
	
	// reset chance, low and high pitch.
	SetChance( 0, state_Disable, false );
	
	theEditField = (LEditField*) FindPaneByID( pane_LowPitchEditField );
	theEditField->SetDescriptor( "\p0.000" );
	theEditField->Disable();
	
	theEditField = (LEditField*) FindPaneByID( pane_HighPitchEditField );
	theEditField->SetDescriptor( "\p0.000" );
	theEditField->Disable();
}


// ---------------------------------------------------------------------------------
//		¥ FillClassList
// ---------------------------------------------------------------------------------
// fills up the class ListBox with the info contained in our data object.
// this assumes that the list is empty. don't be stupid. (hey, i'm talking to myself :)

void
CWailDocWindow::FillClassList()
{
	// find the ListBox.
	CSingleColumnTextListBox *theListBox;
	theListBox = (CSingleColumnTextListBox*) FindPaneByID( pane_ClassList );

	// get number of classes.
	UInt32 numClasses = mSoundFileData->mSoundClasses.GetCount();
	
	// create a progress dialog if we have more than one class.
	CWailProgressDialog	*theProgressDialog = nil;
	if (numClasses > 1)
		theProgressDialog = new CWailProgressDialog( numClasses,
										   			 progressString_LoadingClassNames,
										   			 this );
	
	{
		// disable list drawing to speed things up.
		StDisableListDrawing theDrawingDisabler( *theListBox );
		
		// loop for classes and add them to the list.
		SInt32 i;
		for (i = 1; i <= numClasses; i++)
		{
			LStr255 theRowString;
			UWailClassNames::GetClassName( i, theRowString );
			
			// add the name.
			theListBox->AddRow( 0xffff, (ConstStringPtr) theRowString );
			
			// increment progress bar.
			if (theProgressDialog != nil)
				theProgressDialog->Increment();
		}
	}
	
	// delete the progress dialog.
	if (theProgressDialog != nil)
		delete theProgressDialog;
}


// ---------------------------------------------------------------------------------
//		¥ Fill8bitSoundsList
// ---------------------------------------------------------------------------------
// fills up the 8-bit sounds ListBox with the info contained in our data object.
// this assumes that the list is empty. don't be stupid. (hey, i'm talking to myself :)

void
CWailDocWindow::Fill8bitSoundsList()
{
	// find the ListBox.
	CSingleColumnTextListBox *theListBox;
	theListBox = (CSingleColumnTextListBox*) FindPaneByID( pane_8bitSoundsList );
	
	// enable it.
	theListBox->Enable();

	// get current class.
	CWailSoundClass *theClass;
	mSoundFileData->mSoundClasses.FetchItemAt( mCurrentClass + 1, theClass );
	
	// for sounds, we'll write their size in bytes as their names.
	// loop and add names.
	SInt16 i;
	for (i = 1; i <= theClass->mNum8bitSounds; i++)
	{
		LStr255 theRowString;
		theRowString += theClass->m8bitSounds[i - 1]->GetLength();
		
		// add the name.
		theListBox->AddRow( 0xffff, (ConstStringPtr) theRowString );
	}
}


// ---------------------------------------------------------------------------------
//		¥ Fill16bitSoundsList
// ---------------------------------------------------------------------------------
// fills up the 16-bit sounds ListBox with the info contained in our data object.
// this assumes that the list is empty. don't be stupid. (hey, i'm talking to myself :)

void
CWailDocWindow::Fill16bitSoundsList()
{
	// find the ListBox.
	CSingleColumnTextListBox *theListBox;
	theListBox = (CSingleColumnTextListBox*) FindPaneByID( pane_16bitSoundsList );

	// enable it.
	theListBox->Enable();

	// get current class.
	CWailSoundClass *theClass;
	mSoundFileData->mSoundClasses.FetchItemAt( mCurrentClass + 1, theClass );
	
	// for sounds, we'll write their size in bytes as their names.
	// loop and add names.
	SInt16 i;
	for (i = 1; i <= theClass->mNum16bitSounds; i++)
	{
		LStr255 theRowString;
		theRowString += theClass->m16bitSounds[i - 1]->GetLength();
		
		// add the name.
		theListBox->AddRow( 0xffff, (ConstStringPtr) theRowString );
	}
}


// ---------------------------------------------------------------------------------
//		¥ ClearClassList
// ---------------------------------------------------------------------------------
// clears the class ListBox. this is pretty straightforward.

void
CWailDocWindow::ClearClassList()
{
	// find the ListBox.
	CSingleColumnTextListBox *theListBox;
	theListBox = (CSingleColumnTextListBox*) FindPaneByID( pane_ClassList );
	
	// deselect any selected items.
	theListBox->UnselectAllCells();
	mCurrentClass = -1;
	
	// empty it.
	theListBox->RemoveRows( 0, 0x7fff );
}


// ---------------------------------------------------------------------------------
//		¥ Clear8bitSoundsList
// ---------------------------------------------------------------------------------
// clears the 8-bit sounds ListBox. this is pretty straightforward.

void
CWailDocWindow::Clear8bitSoundsList()
{
	// find the ListBox.
	CSingleColumnTextListBox *theListBox;
	theListBox = (CSingleColumnTextListBox*) FindPaneByID( pane_8bitSoundsList );
	
	// deselect any selected items.
	theListBox->UnselectAllCells();
	
	// empty it.
	theListBox->RemoveRows( 0, 0x7fff );
	
	// disable the pane.
	theListBox->Disable();
}


// ---------------------------------------------------------------------------------
//		¥ Clear16bitSoundsList
// ---------------------------------------------------------------------------------
// clears the 16-bit sounds ListBox. this is pretty straightforward.

void
CWailDocWindow::Clear16bitSoundsList()
{
	// find the ListBox.
	CSingleColumnTextListBox *theListBox;
	theListBox = (CSingleColumnTextListBox*) FindPaneByID( pane_16bitSoundsList );
	
	// deselect any selected items.
	theListBox->UnselectAllCells();
	
	// empty it.
	theListBox->RemoveRows( 0, 0x7fff );
	
	// disable the pane.
	theListBox->Disable();
}


// ---------------------------------------------------------------------------------
//		¥ SetSoundFileData
// ---------------------------------------------------------------------------------
// sets our internal sound file data.

void
CWailDocWindow::SetSoundFileData(
	CWailSoundFileData	*inData )
{
	if (inData != mSoundFileData)
	{
		// first, let's see if we already had data.
		if (mSoundFileData != nil)
		{
			// we did. ok, we must clear the class list
			// and delete the data object.
			DeleteSoundFileData();
		}
		
		// we're ready to store the new data.
		mSoundFileData = inData;
		
		// fill up the class list.
		FillClassList();
		// reset class panes, since there's no class selected.
		ResetAllClassPanes();
		// deselect everything.
		ThrowIf_( !LCommander::SwitchTarget( this ) );
		
		// refresh the list.
		FindPaneByID( pane_ClassList )->Refresh();
		
		// set the dirty flag off. if we're loading new data, we're certainly not dirty.
		SetDirty( false );
		// ...and make sure no class is selected.
		mCurrentClass = -1;
		
		// post a nil action so that any leftover undoable actions is cleared.
		PostAction( nil );
	}
}


// ---------------------------------------------------------------------------------
//		¥ GetSoundFileData
// ---------------------------------------------------------------------------------
// returns our sound file data.
// (should i inline this?)

CWailSoundFileData*
CWailDocWindow::GetSoundFileData()
{
	return mSoundFileData;
}


// ---------------------------------------------------------------------------------
//		¥ DeleteSoundFileData
// ---------------------------------------------------------------------------------
// deletes our file data. this is called when Revert is selected. you'd better
// call SetSoundFileData pretty quickly after that :)

void
CWailDocWindow::DeleteSoundFileData()
{
	if (mSoundFileData != nil)
	{
		SelectClass( -1, false );
		ClearClassList();
		delete mSoundFileData;
		mSoundFileData = nil;
		
		// post a nil action to clear any leftover undoable actions in the queue.
		PostAction( nil );
	}
}


// ---------------------------------------------------------------------------------
//		¥ SetDirty
// ---------------------------------------------------------------------------------
// sets the window's "dirtiness".

void
CWailDocWindow::SetDirty(
	Boolean		inDirty )
{
	mDirty = inDirty;
}


// ---------------------------------------------------------------------------
//		¥ GetChance
// ---------------------------------------------------------------------------
// returns the value of the "Chance" control in the window.

SInt16
CWailDocWindow::GetChance() const
{
	LStdPopupMenu* thePopupMenu = dynamic_cast<LStdPopupMenu*>(FindPaneByID( pane_ChancePopupMenu ));
	SignalIf_( thePopupMenu == NULL );
	
	SInt16 theChance = chance_Always;
	switch (thePopupMenu->GetValue())
	{
		case popupValue_10Percent:
			theChance = chance_10Percent;
			break;
		
		case popupValue_20Percent:
			theChance = chance_20Percent;
			break;
		
		case popupValue_30Percent:
			theChance = chance_30Percent;
			break;
		
		case popupValue_40Percent:
			theChance = chance_40Percent;
			break;
		
		case popupValue_50Percent:
			theChance = chance_50Percent;
			break;
		
		case popupValue_60Percent:
			theChance = chance_60Percent;
			break;
		
		case popupValue_70Percent:
			theChance = chance_70Percent;
			break;
		
		case popupValue_80Percent:
			theChance = chance_80Percent;
			break;
		
		case popupValue_90Percent:
			theChance = chance_90Percent;
			break;
		
		case popupValue_Always:
			theChance = chance_Always;
			break;
		
		default:
			SignalStringLiteral_( "Unsupported popup value for Chance" );
			break;
	}
	
	return theChance;
}


// ---------------------------------------------------------------------------
//		¥ SetChance
// ---------------------------------------------------------------------------
// sets the value of the "Chance" control in the window.
//
// inStateEffect determines whether we enable/disable the control or simply
// avoid changing its state.
//
// if inBroadcastChange is false, we prevent the control from broadcasting its
// value change message when the chance is set.

void
CWailDocWindow::SetChance(
	SInt16				inChance,
	EControlStateEffect	inStateEffect,
	bool				inBroadcastChange )
{
	LStdPopupMenu* thePopupMenu = dynamic_cast<LStdPopupMenu*>(FindPaneByID( pane_ChancePopupMenu ));
	SignalIf_( thePopupMenu == NULL );
	
	// enable/disable control if needed.
	switch (inStateEffect)
	{
		case state_NoEffect:
			// no effect, don't change state.
			break;
		
		case state_Disable:
			thePopupMenu->Disable();
			break;
			
		case state_Enable:
			thePopupMenu->Enable();
			break;
			
		default:
			SignalStringLiteral_( "Wrong value for inStateEffect" );
			break;
	}
		
	// round chance value to get an exact popup value candidate.
	inChance = CWailSoundClass::RoundChance( inChance );
	
	// determine new popup value.
	SInt32 thePopupValue = popupValue_Always;
	switch (inChance)
	{
		case chance_10Percent:
			thePopupValue = popupValue_10Percent;
			break;
			
		case chance_20Percent:
			thePopupValue = popupValue_20Percent;
			break;
			
		case chance_30Percent:
			thePopupValue = popupValue_30Percent;
			break;
			
		case chance_40Percent:
			thePopupValue = popupValue_40Percent;
			break;
			
		case chance_50Percent:
			thePopupValue = popupValue_50Percent;
			break;
			
		case chance_60Percent:
			thePopupValue = popupValue_60Percent;
			break;
			
		case chance_70Percent:
			thePopupValue = popupValue_70Percent;
			break;
			
		case chance_80Percent:
			thePopupValue = popupValue_80Percent;
			break;
			
		case chance_90Percent:
			thePopupValue = popupValue_90Percent;
			break;
			
		case chance_Always:
			thePopupValue = popupValue_Always;
			break;
			
		default:
			SignalStringLiteral_( "Badly-rounded Chance value" );
			break;
	}
	
	// if we must not let control broadcast the change, disable broadcasting
	// before setting new value.
	StBroadcasterStateSaver theBroadcasterSaver( thePopupMenu, inBroadcastChange );
	
	// set control value.
	thePopupMenu->SetValue( thePopupValue );
	
	// broadcaster state saver restores the control's broadcasting state when destroyed here.
}


// ---------------------------------------------------------------------------------
//		¥ InitWailDocWindow
// ---------------------------------------------------------------------------------
// inits our member variables. called by the constructors.

void
CWailDocWindow::InitWailDocWindow()
{
	// init our sound file data to nil.
	mSoundFileData = nil;
	// we're clean. beam us up, scotty.
	SetDirty( false );
	// we're not changing class now.
	mIsChangingClass = false;
	// we're not performing actions right now.
	mIsPerformingAction = false;
	// we're not remapping now.
	mIsRemapping = false;
	// no selected class.
	mCurrentClass = -1;
}


// ---------------------------------------------------------------------------
//		¥ FinishCreateSelf
// ---------------------------------------------------------------------------
// this is called after all subpanes are created. we will use this to link ourselves
// to our controls, so we can listen to their messages.

void
CWailDocWindow::FinishCreateSelf()
{
	CTrackableWindow::FinishCreateSelf();

	// listen to our broadcasting panes (controls, lists, etc.)
	UCode99::LinkListenerToBroadcasters( this, this );
	
	// let's try to add an LUndoer here, since adding one in the Constructor
	// doesn't seem to work (anymore?!?). [2.0a6]
	AddAttachment( new LUndoer );
}


// ---------------------------------------------------------------------------------
//		¥ HandleFlagChange
// ---------------------------------------------------------------------------------
// changes the class flags when the user selects a check box.

void
CWailDocWindow::HandleFlagChange(
	MessageT		 inMessage,
	CWailSoundClass* inClass )
{
	if (!mIsChangingClass) // we don't listen to those when we're changing class.
	{
		// switch to select the proper flag.
		SInt16 theFlag;
		switch (inMessage)
		{
			case msg_Flag1CheckBox:
				// Cannot be restarted flag.
				theFlag = flag_CannotBeRestarted;
				break;
			
			case msg_Flag2CheckBox:
				// Resists pitch change flag.
				theFlag = flag_ResistsPitchChange;
				break;
				
			case msg_Flag3CheckBox:
				// Cannot change pitch flag.
				theFlag = flag_CannotChangePitch;
				break;
				
			case msg_Flag4CheckBox:
				// Can't be obstructed flag.
				theFlag = flag_NoObstruct;
				break;
				
			case msg_Flag5CheckBox:
				// Can't be media obstructed flag.
				theFlag = flag_NoMediaObstruct;
				break;
				
			case msg_Flag6CheckBox:
				// Is Ambient flag.
				theFlag = flag_IsAmbient;
				break;
				
			case msg_Flag7CheckBox:
				// Doesn't self-abort flag
				theFlag = flag_DoesntSelfAbort;
				break;
				
			default:
				// a mistake. this function was probably called by error. don't change anything.
				SignalStringLiteral_( "HandleFlagChange called with wrong parameter" );
				theFlag = 0x0000;
				break;
		}

		// toggle the flag if it isn't empty.
		if (theFlag != 0x0000)
		{
			inClass->mFlags = ::BitXor( inClass->mFlags, theFlag );
			
			// since we are surely changed, we are therefore dirty.
			SetDirty( true );
			
			// if we're not performing an action, create one to allow undo.
			if (!mIsPerformingAction)
				ActionChangeFlag( inMessage );
		}
	}
}


// ---------------------------------------------------------------------------------
//		¥ HandleVolumeChange
// ---------------------------------------------------------------------------------
// changes the volume when the user selects a radio.

void
CWailDocWindow::HandleVolumeChange(
	MessageT		 inMessage,
	CWailSoundClass* inClass )
{
	if (!mIsChangingClass) // we don't listen to those when we're changing class.
	{
		// switch to select the volume and appropriate control.
		SInt16 theVolume;
		LControl* theControl;
		switch (inMessage)
		{
			case msg_SoftVolumeRadioButton:
				// Soft volume.
				theVolume = volume_Soft;
				theControl = (LControl*) FindPaneByID( pane_SoftVolumeRadioButton );
				break;
				
			case msg_MediumVolumeRadioButton:
				// Medium volume.
				theVolume = volume_Medium;
				theControl = (LControl*) FindPaneByID( pane_MediumVolumeRadioButton );
				break;
				
			case msg_LoudVolumeRadioButton:
				// Loud volume.
				theVolume = volume_Loud;
				theControl = (LControl*) FindPaneByID( pane_LoudVolumeRadioButton );
				break;
				
			case msg_Volume3RadioButton:
				// Volume3 volume. used by ZPC.
				theVolume = volume_Volume3;
				theControl = (LControl*) FindPaneByID( pane_Volume3RadioButton );
				break;
				
			default:
				// a mistake. this function was probably called by error. don't change anything.
				SignalStringLiteral_( "HandleVolumeChange called with wrong parameter" );
				return;
				break;
		}
		
		// if we didn't find the control, throw.
		ThrowIfNil_( theControl );

		// change the volume if the radio is selected.
		if (theControl->GetValue())
		{
			SInt16 oldVolume = inClass->mVolume;	// save old volume.
			inClass->mVolume = theVolume;		// set new volume.
			
			// since we are surely changed, we are therefore dirty.
			SetDirty( true );
			
			// if we're not performing an action, create one to allow undo.
			if (!mIsPerformingAction)
				ActionChangeVolume( oldVolume, theVolume );
		}
	}
}


// ---------------------------------------------------------------------------------
//		¥ HandleRemapping
// ---------------------------------------------------------------------------------
// responds to the message sent by the "Remap 8-bit" check box.

void
CWailDocWindow::HandleRemapping(
	CWailSoundClass* inClass )
{
	if ((!mIsChangingClass) && (!mIsRemapping)) 
		// we don't listen to this when we're changing class or remapping.
	{
		// tell window we're currently processing the remapping message.
		mIsRemapping = true;
	
		// let's see the current state of the control
		LControl *theControl = 
			(LControl*) FindPaneByID( pane_Remap8bitCheckBox );
		if (theControl->GetValue())
		{
			// let's see if we currently have 16-bit sounds.
			Boolean canContinue = true;
			
			if ((inClass->mNum16bitSounds > 0) &&
				UWailPreferences::WarnBeforeRemapping())
			{
				// we must warn the user that all 16-bit sounds will be cleared.
				StDontShowAgainDialogHandler theHandler(
												rPPob_WarnDelete16bitSoundsDialog,
												msg_DontShowAgainCheckBox,
												this );
				Boolean isDone = false;
				canContinue = false;
				Boolean needToWritePrefs = false;
				while (!isDone)
				{
					MessageT theMessage = theHandler.DoDialog();
					
					switch(theMessage)
					{
						case msg_OK:
							isDone = true;
							canContinue = true;
							break;
							
						case msg_Cancel:
							isDone = true;
							break;
							
						case msg_DontShowAgain:
							UWailPreferences::SetWarnBeforeRemapping( false );
							needToWritePrefs = true;
							break;
							
						case msg_ShowAgain:
							UWailPreferences::SetWarnBeforeRemapping( true );
							needToWritePrefs = true;
							break;
					}
				}
				
				if (needToWritePrefs)
					UWailPreferences::Write();
				
				theHandler.GetDialog()->Hide();	// hide it until we're done.
				
				if (!canContinue)
				{
					// we must un-check the radio.
					theControl->SetValue( 0 );
				}
			}
			
			if (canContinue)
			{
				// create an undoable action to perform the checking of the remapping check box.
				ActionRemap8bit();
			}
		}
		else
		{
			// create an undoable action to perform the removal of the remapping check box.
			ActionDontRemap8bit();
		}
		
		// we are no longer processing the remapping message.
		mIsRemapping = false;
	}
}


// ---------------------------------------------------------------------------------
//		¥ HandleDragSend
// ---------------------------------------------------------------------------------
//	need to initiate a drag of the currently selected sound.

void
CWailDocWindow::HandleDragSend(
	const SMouseDownEvent&	inMouseDown,
	Boolean					inIs8bit )
{
	// fetch appropriate list box.
	LListBox* theListBox;
	if (inIs8bit)
		theListBox = dynamic_cast<LListBox*> (FindPaneByID( pane_8bitSoundsList ));
	else
		theListBox = dynamic_cast<LListBox*> (FindPaneByID( pane_16bitSoundsList ));
	ThrowIfNil_( theListBox );
	
	// fetch current selection.
	Cell theCell;
	theListBox->GetLastSelectedCell( theCell );
	
	// if there is no selection, just return.
	if (theCell.h == -1)
		return;
	
	// fetch current class.
	CWailSoundClass* theSoundClass;
	mSoundFileData->mSoundClasses.FetchItemAt( mCurrentClass + 1, theSoundClass );
	
	// fetch sound associated with selection.
	LStream* theSoundStream;
	if (inIs8bit)
		theSoundStream = theSoundClass->m8bitSounds[theCell.h];
	else
		theSoundStream = theSoundClass->m16bitSounds[theCell.h];
	ThrowIfNil_( theSoundStream );
	
	// allocate a temp handle to store the marathon sound.
	Handle theMthonSoundH = ::NewHandle( theSoundStream->GetLength() );
	ThrowIfNil_( theMthonSoundH );

	{
		// lock the handle to use it as a buffer.
		StHandleLocker theHandleLocker( theMthonSoundH );
		
		// read the sound from the stream.
		theSoundStream->SetMarker( 0, streamFrom_Start );
		theSoundStream->ReadBlock( *theMthonSoundH, theSoundStream->GetLength() );
	}
	
	// turn the sound into a mac sound.
	Handle theMacSoundH = CWailSoundClass::TurnMthonSoundIntoMacSound( theMthonSoundH );
	
	// dispose of the marathon sound.
	::DisposeHandle( theMthonSoundH );
	
	// calculate frame for the drag.
	Rect theFrame;
	ListHandle theListH = theListBox->GetMacListH();
	::LRect( &theFrame, theCell, theListH );
	
	{
		// lock the sound.
		StHandleLocker theHandleLocker( theMacSoundH );
		
		// create the drag.
		LDragTask theDragTask( inMouseDown.macEvent, theFrame, 1, 'snd ',
							   *theMacSoundH, ::GetHandleSize( theMacSoundH ), 0 );
	
		// do we need to delete the sound (i.e. was it dropped in trash)?
		if (theDragTask.DropLocationIsFinderTrash())
		{
			// we do need to delete the sound.
			ProcessCommand( cmd_DeleteSound, nil );
		}
	}
	
	// dispose of the sound.
	::DisposeHandle( theMacSoundH );
}


// ---------------------------------------------------------------------------------
//		¥ HandleDragReceive
// ---------------------------------------------------------------------------------
//	receiving a sound from a drag.

void
CWailDocWindow::HandleDragReceive(
	Handle&					inSoundHandle,
	Boolean					inIs8bit )
{
	// turn the sound into a Marathon sound.
	Handle theMthonSoundH = CWailSoundClass::TurnMacSoundIntoMthonSound( inSoundHandle );
	
	// create a stream to store the sound.
	CWailSoundStream *theHandleStream = new CWailSoundStream( theMthonSoundH );
		// the stream now "owns" the handle and will dispose of it when destroyed.
	
	// add a sound to the current class.
	ActionAddSound( theHandleStream, inIs8bit );
}


// ---------------------------------------------------------------------------------
//		¥ CompareWithAnotherFile
// ---------------------------------------------------------------------------------
// compares the content of this window with the content of another marathon
// file and eliminates all similar classes from this window.

Boolean
CWailDocWindow::CompareWithAnotherFile()
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
		theDialogOptions->dialogOptionFlags &= ~(kNavAllowPreviews);
	}

	// ask user to choose another file.
	FSSpec theFile;
	if (theFileChooser.AskChooseOneFile( theTypeList, theFile ))
	{
		// create a file stream from this file and open it.
		LFileStream* theFileStream = new LFileStream( theFile );
		ThrowIfNil_( theFileStream );
		theFileStream->OpenDataFork( fsRdPerm );
		
		// read its data.
		CWailSoundFileData theSoundFileData( theFileStream, true );
		
		// close file.
		theFileStream->CloseDataFork();
		
		// delete the stream.
		delete theFileStream;
		
		// compare data with ours.
		mSoundFileData->CompareAndKeepOnlyDiffs( theSoundFileData );
		
		// re-select first class to show changes if a class is currently selected.
		if (mCurrentClass != -1)
			SelectClass( 0, true );
			
		// comparing makes us dirty.
		SetDirty( true );
	}

	return true;
}


// ---------------------------------------------------------------------------------
//		¥ CompareWithAnotherWindow
// ---------------------------------------------------------------------------------
// compares the content of this window with the content of another window
// and eliminates all similar classes from this window.

Boolean
CWailDocWindow::CompareWithAnotherWindow()
{
	// ask user to choose another window.
	CWailWindowChooser theChooser( this );
	CWailDocWindow* theWindow = theChooser.ChooseWindow( this );

	// if user selected a window, compare with it.
	if (theWindow != nil)
	{
		// get its data.
		CWailSoundFileData* theSoundFileData = theWindow->GetSoundFileData();
		
		// compare data with ours.
		mSoundFileData->CompareAndKeepOnlyDiffs( *theSoundFileData );
		
		// re-select first class to show changes if a class is currently selected.
		if (mCurrentClass != -1)
			SelectClass( 0, true );
			
		// comparing makes us dirty.
		SetDirty( true );
	}

	return true;
}


// ---------------------------------------------------------------------------------
//		¥ AddClass
// ---------------------------------------------------------------------------------
// adds a class to our class list.

Boolean
CWailDocWindow::AddClass()
{
	ActionAddClasses( 1 );
	
	return true;
}


// ---------------------------------------------------------------------------------
//		¥ AddClasses
// ---------------------------------------------------------------------------------
// adds multiple classes to our class list.

Boolean
CWailDocWindow::AddClasses()
{
	// ask the user how many classes he wishes to add.
	SInt32 howManyClasses = 1;
	
	Boolean isGood = UModalDialogs::AskForOneNumber( this,
													  rPPob_AddClassesDialog,
													  pane_HowManyClassesEditField,
													  howManyClasses );
									
	if (isGood)
	{
		// see if the number is valid.
		SInt32 maxNumClasses = 32767 - ( mSoundFileData->mSoundClasses.GetCount() );
		
		if ( (howManyClasses < 1) || (howManyClasses > maxNumClasses) )
		{
			// not. show an alert.
			LStr255 maxNumClassesString( maxNumClasses );
			::ParamText( (ConstStringPtr) maxNumClassesString, "\p", "\p", "\p" );
			UModalAlerts::Alert( rALRT_TooManyClassesAlert );
		}
		else
		{
			// it is. add that many classes.
			ActionAddClasses( howManyClasses );
		}
	}
	
	return true;
}


// ---------------------------------------------------------------------------------
//		¥ RemoveClass
// ---------------------------------------------------------------------------------
// removes the select class from our class list.

Boolean
CWailDocWindow::RemoveClass()
{
	// maybe ask user to confirm?
	Boolean canContinue = true;

	if (UWailPreferences::WarnBeforeDeletingClass())
	{
		// ask user to confirm.
		StDontShowAgainDialogHandler theHandler(
										rPPob_WarnDeleteClassDialog,
										msg_DontShowAgainCheckBox,
										this );
		Boolean isDone = false;
		canContinue = false;
		Boolean needToWritePrefs = false;
		while (!isDone)
		{
			MessageT theMessage = theHandler.DoDialog();
			
			switch(theMessage)
			{
				case msg_OK:
					isDone = true;
					canContinue = true;
					break;
					
				case msg_Cancel:
					isDone = true;
					break;
					
				case msg_DontShowAgain:
					UWailPreferences::SetWarnBeforeDeletingClass( false );
					needToWritePrefs = true;
					break;
					
				case msg_ShowAgain:
					UWailPreferences::SetWarnBeforeDeletingClass( true );
					needToWritePrefs = true;
					break;
			}
		}
		
		if (needToWritePrefs)
			UWailPreferences::Write();
	}

	if (canContinue)
	{
		// remove the class.
		ActionRemoveClass();
	}
	
	
	return true;
}


// ---------------------------------------------------------------------------------
//		¥ FindClassByID
// ---------------------------------------------------------------------------------
// displays a dialog to ask for a Class ID, then checks if a class with that ID exists
// in the file. if there is one, it selects it.

Boolean
CWailDocWindow::FindClassByID()
{
	SInt32 theClassID = 0;

	// ask the user to enter a class ID.
	Boolean isOKed = UModalDialogs::AskForOneNumber( this, 
													rPPob_FindClassByIDDialog,
													pane_WhatClassIDEditField,
													theClassID );
													
	if (isOKed)
	{
		// look for that ID.
		SInt32 i;
		Boolean foundIt = false;
		CWailSoundClass *theClass;
		CWailProgressDialog *theProgressDialog = new CWailProgressDialog(
													mSoundFileData->mSoundClasses.GetCount(),
													progressString_SearchingForClass,
													this );
		for (i = 0; (i < mSoundFileData->mSoundClasses.GetCount()) && !foundIt; i++)
		{
			mSoundFileData->mSoundClasses.FetchItemAt( i + 1, theClass );
			if (theClass->mClassID == theClassID)
				foundIt = true;
			theProgressDialog->Increment();
		}
		i--;
		delete theProgressDialog;
		
		if (foundIt)
		{
			// we did find it.
			// see if it's changed.
			if (mCurrentClass != i)
			{
				// it's changed. 
				SelectClass( i, /* refresh the list? */ true );
			}
		}
		else
			// we did not find it. just beep.
			::SysBeep(1);
	}
				
	return isOKed;									
}


// ---------------------------------------------------------------------------------
//		¥ FindClassByName
// ---------------------------------------------------------------------------------
// displays a dialog to ask for a Class name, then checks if a class with that name exists
// in the file. if there is one, it selects it.

Boolean
CWailDocWindow::FindClassByName()
{
	Str255 theClassName = "\p";

	// ask the user to enter a class name.
	Boolean isOKed = FALSE;
	{
		StFindClassNameDialogHandler theHandler( this );
		
		Boolean isDone = FALSE;
		while (!isDone)
		{
			MessageT theMessage = theHandler.DoDialog();
			
			switch (theMessage)
			{
				case msg_OK:
					isOKed = TRUE;
					isDone = TRUE;
					break;
					
				case msg_Cancel:
					isDone = TRUE;
					break;
			}
		}
		
		if (isOKed)
			theHandler.GetClassName( theClassName );
	}
													
	if (isOKed)
	{
		// look for that name.
		SInt32 i;
		Boolean foundIt = FALSE;
		CWailProgressDialog *theProgressDialog = new CWailProgressDialog(
													mSoundFileData->mSoundClasses.GetCount(),
													progressString_SearchingForClass,
													this );
		LStr255 thatClassName;
		if (UWailPreferences::IgnoreCaseInNameFind())
			thatClassName.SetCompareFunc( LString::CompareIgnoringCase );
		Boolean exactMatch = UWailPreferences::ExactMatchInNameFind();
		for (i = 0; (i < mSoundFileData->mSoundClasses.GetCount()) && !foundIt; ++i)
		{
			UWailClassNames::GetClassName( i + 1, thatClassName );
			if (exactMatch)
			{
				if (thatClassName.CompareTo( theClassName ) == 0)
					foundIt = TRUE;
			}
			else
			{
				if (thatClassName.Find( theClassName ) > 0)
					foundIt = TRUE;
			}
			theProgressDialog->Increment();
		}
		--i;
		delete theProgressDialog;
		
		if (foundIt)
		{
			// we did find it.
			// see if it's changed.
			if (mCurrentClass != i)
			{
				// it's changed. 
				SelectClass( i, /* refresh the list? */ true );
			}
		}
		else
			// we did not find it. just beep.
			::SysBeep(1);
	}
				
	return isOKed;
}


// ---------------------------------------------------------------------------------
//		¥ FindClassByNumber
// ---------------------------------------------------------------------------------
// displays a dialog to ask for a Class number, then selects the class with that
// number in the class list.

Boolean
CWailDocWindow::FindClassByNumber()
{
	SInt32 theClassNumber = 0;

	// ask the user to enter a class number.
	Boolean isOKed = UModalDialogs::AskForOneNumber( this, 
													rPPob_FindClassByNumberDialog,
													pane_WhatClassNumberEditField,
													theClassNumber );
													
	if (isOKed && (mCurrentClass != theClassNumber))
	{
		// check if this class number is valid.
		if ((theClassNumber >= 0) && (theClassNumber <= mSoundFileData->mSoundClasses.GetCount()))
		{
			// class number is valid, select this class.
			SelectClass( theClassNumber, /* refresh the list? */ true );
		}
		else
			// we did not find it. just beep.
			::SysBeep(1);
	}
				
	return isOKed;									
}


// ---------------------------------------------------------------------------------
//		¥ CutCopyClearSound
// ---------------------------------------------------------------------------------
// handles Cut, Copy or Clear for sounds.

Boolean	
CWailDocWindow::CutCopyClearSound( CommandT inCommand, Boolean inIs8bit )
{
	// get a pointer to the proper list.
	LPane* theSoundList = FindPaneByID( inIs8bit ? pane_8bitSoundsList : pane_16bitSoundsList );
	ThrowIfNil_( theSoundList );
	
	// these are handled together because they have common behavior.
	
	// if we cut or copy, get the sound and put it in the scrap.
	if ((inCommand == cmd_Cut) || (inCommand == cmd_Copy))
	{
		// check which sound is selected.
		SInt32 theSelectedSound = theSoundList->GetValue();
		if (theSelectedSound != -1)
		{
			// get the current sound class.
			CWailSoundClass* theClass;
			mSoundFileData->mSoundClasses.FetchItemAt( mCurrentClass + 1, theClass );
			
			// get the proper stream array (either 8-bit or 16-bit).
			LStream** theSounds = (inIs8bit ? theClass->m8bitSounds : theClass->m16bitSounds);
		
			// get the sound stream.
			LStream* theSoundStream = theSounds[(SInt16) theSelectedSound];
			
			// create a handle to store the sound.
			Handle theMthonSoundH = ::NewHandle( theSoundStream->GetLength() );
			ThrowIfNil_( theMthonSoundH );
			
			{
				// lock the handle so we can pass it as a buffer.
				StHandleLocker theMthonSoundHLocker( theMthonSoundH );
				
				// set the stream's marker to the beginning of the stream and read the sound.
				theSoundStream->SetMarker( 0, streamFrom_Start );
				theSoundStream->ReadBlock( *theMthonSoundH, theSoundStream->GetLength() );
			}
			
			// turn that sound into a real Mac sound (since it's a Marathon sound).
			Handle theMacSoundH = CWailSoundClass::TurnMthonSoundIntoMacSound( theMthonSoundH );
			
			// dispose of the first handle. (it's no longer of use.)
			::DisposeHandle( theMthonSoundH );
			
			// put that sound in the scrap.
			LClipboard theClipboard;
			theClipboard.SetData( 'snd ', theMacSoundH, /* reset the clipboard? */ true );
			
			// dispose of the second handle.
			::DisposeHandle( theMacSoundH );
		}
	}
	
	// if we cut or clear, we must delete the sound.
	if ((inCommand == cmd_Cut) || (inCommand == cmd_Clear))
	{
		Boolean canContinue = true;
	
		if (UWailPreferences::WarnBeforeDeleting())
		{
			// ask user to confirm.
			StDontShowAgainDialogHandler theHandler(
											rPPob_WarnDeleteSoundDialog,
											msg_DontShowAgainCheckBox,
											this );
			Boolean isDone = false;
			canContinue = false;
			Boolean needToWritePrefs = false;
			while (!isDone)
			{
				MessageT theMessage = theHandler.DoDialog();
				
				switch(theMessage)
				{
					case msg_OK:
						isDone = true;
						canContinue = true;
						break;
						
					case msg_Cancel:
						isDone = true;
						break;
						
					case msg_DontShowAgain:
						UWailPreferences::SetWarnBeforeDeleting( false );
						needToWritePrefs = true;
						break;
						
					case msg_ShowAgain:
						UWailPreferences::SetWarnBeforeDeleting( true );
						needToWritePrefs = true;
						break;
				}
			}
			
			if (needToWritePrefs)
				UWailPreferences::Write();
		}
	
		if (canContinue)
		{
			// check which sound is selected.
			SInt32 theSelectedSound = theSoundList->GetValue();
			if (theSelectedSound != -1)
			{
				// ask user to confirm.
				ActionRemoveSound( theSelectedSound, inIs8bit );
			}
		}
	}
	
	return true;
}


// ---------------------------------------------------------------------------------
//		¥ PasteSound
// ---------------------------------------------------------------------------------
// pastes the sound in the scrap in the given sound list.

Boolean
CWailDocWindow::PasteSound( Boolean inIs8bit )
{
	// get the sound from the scrap.
	Handle theMacSoundH = ::NewHandle( 0 );
	ThrowIfMemFail_( theMacSoundH );
	LClipboard theClipboard;
	theClipboard.GetData( 'snd ', theMacSoundH );
	ThrowIfNil_( theMacSoundH );
	
	// turn it into a Marathon sound.
	Handle theMthonSoundH = CWailSoundClass::TurnMacSoundIntoMthonSound( theMacSoundH );
	
	// dispose of the mac sound since it's no longer needed.
	::DisposeHandle( theMacSoundH );
	
	// create a stream to store the sound.
	CWailSoundStream* theHandleStream = new CWailSoundStream( theMthonSoundH );
		// the stream now "owns" the handle and will dispose of it when destroyed.
	
	// add a sound to the current class.
	ActionAddSound( theHandleStream, inIs8bit );
	
	return true;
}


// ---------------------------------------------------------------------------------
//		¥ ImportSys7Sound
// ---------------------------------------------------------------------------------
// imports a System 7 Sound File into the given sound list.

Boolean
CWailDocWindow::ImportSys7Sound( Boolean inIs8bit )
{
	// ask the user to choose a file.
	FSSpec theFile;
	if (PP_StandardDialogs::AskChooseOneFile( fileType_System7Sound,
											  theFile,
											  kNavDefaultNavDlogOptions & ~(kNavAllowPreviews) ))
	{
		// read the sound.
		Handle theMacSoundH;
		{
			// keep track of old res file.
			StOldResFile theOldResFile;
		
			// open the file.
			LFile theSys7File( theFile );
			theSys7File.OpenResourceFork( fsRdPerm );
			
			// read the sound resource.
			theMacSoundH = ::Get1IndResource( 'snd ', 1 );
			ThrowIfResError_();
			
			// detach it.
			::DetachResource( theMacSoundH );
			ThrowIfResError_();
			
			// close the file.
			theSys7File.CloseResourceFork();
		}
	
		// turn the sound into a Marathon sound.
		Handle theMthonSoundH = CWailSoundClass::TurnMacSoundIntoMthonSound( theMacSoundH );
		
		// dispose of the mac sound since it's no longer needed.
		::DisposeHandle( theMacSoundH );
		
		// create a stream to store the sound.
		CWailSoundStream* theHandleStream = new CWailSoundStream( theMthonSoundH );
		// the stream now "owns" the handle and will dispose of it when destroyed.
		
		// add a sound to the current class.
		ActionAddSound( theHandleStream, inIs8bit );
	}
	
	return true;
}


// ---------------------------------------------------------------------------------
//		¥ ExportSys7Sound
// ---------------------------------------------------------------------------------
// exports the currently selected sound in the given list as a System 7 Sound file.

Boolean	
CWailDocWindow::ExportSys7Sound( Boolean inIs8bit )
{
	// get the proper sound list.
	LPane* theSoundList = FindPaneByID( inIs8bit ? pane_8bitSoundsList : pane_16bitSoundsList );
	ThrowIfNil_( theSoundList );

	// check which sound is selected.
	SInt32 theSelectedSound = theSoundList->GetValue();
	ThrowIf_( theSelectedSound == -1 );

	// build default name for sound file.
	LStr255 defaultName( STRx_SoundStrings, str_SoundFileDefaultName );
	defaultName += (theSelectedSound + 1);
	
	// create a file designator, configure it.
	PP_StandardDialogs::LFileDesignator theFileDesignator;
	theFileDesignator.SetFileType( fileType_System7Sound );
	NavDialogOptions* theDialogOptions = theFileDesignator.GetDialogOptions();
	if (theDialogOptions != NULL)
	{
		theDialogOptions->dialogOptionFlags |= kNavNoTypePopup;
	}
	
	// ask user to save his System-7 sound file.
	if (theFileDesignator.AskDesignateFile( defaultName ))
	{
		FSSpec theFile;
		theFileDesignator.GetFileSpec( theFile );
	
		// delete the previous file if we're replacing it.
		if (theFileDesignator.IsReplacing())
			ThrowIfOSErr_( ::FSpDelete( &theFile ) );
			
		// get the current class.
		CWailSoundClass* theClass;
		mSoundFileData->mSoundClasses.FetchItemAt( mCurrentClass + 1, theClass );
			
		// get the proper sound stream array.
		LStream** theSounds = (inIs8bit ? theClass->m8bitSounds : theClass->m16bitSounds );
	
		// get the sound stream.
		LStream* theSoundStream = theSounds[(SInt16) theSelectedSound];
		
		// create a handle to store the sound.
		Handle theMthonSoundH = ::NewHandle( theSoundStream->GetLength() );
		ThrowIfNil_( theMthonSoundH );
		
		// lock the handle so we can pass it as a buffer.
		StHandleLocker theMthonSoundHLocker( theMthonSoundH );
		
		// set the stream's marker to the beginning of the stream and read the sound.
		theSoundStream->SetMarker( 0, streamFrom_Start );
		theSoundStream->ReadBlock( *theMthonSoundH, theSoundStream->GetLength() );
		
		// turn that sound into a real Mac sound (since it's a Marathon sound).
		Handle theMacSoundH = CWailSoundClass::TurnMthonSoundIntoMacSound( theMthonSoundH );
		
		// dispose of the first handle. (it's no longer of use.)
		::DisposeHandle( theMthonSoundH );
		
		// create a file to store the sound.
		LFile theSys7File( theFile );
		theSys7File.CreateNewFile( fileCreator_System7Sound,
								   fileType_System7Sound,
								   theFileDesignator.GetScriptCode() );
		
		// keep track of the old res file.
		StOldResFile theOldResFile;
		
		// open it.
		theSys7File.OpenResourceFork( fsRdWrPerm );
		
		// add the mac sound to it.
		::AddResource( theMacSoundH, 'snd ', ::Unique1ID('snd '), theFile.name );
			// the resource must bear the same name as the file for the finder
			// to be able to play it when you double-click on it.
		ThrowIfResError_();
			
		// write the resource to disk. (it's only in memory right now.)
		::WriteResource( theMacSoundH );
		ThrowIfResError_();
		
		// release it from memory.
		::ReleaseResource( theMacSoundH );
		ThrowIfResError_();
		
		// close the file.
		theSys7File.CloseResourceFork();
	}
	
	return true;
}


// ---------------------------------------------------------------------------------
//		¥ ImportAiffSound
// ---------------------------------------------------------------------------------
// imports an AIFF Sound File into the given sound list.

Boolean
CWailDocWindow::ImportAiffSound( Boolean inIs8bit )
{
	// create list of acceptable types.
	OSType theTypes[] = { fileType_AIFFSound, fileType_AIFFCSound };
	LFileTypeList theTypeList( 2, theTypes );

	// create a file chooser, configure it.
	PP_StandardDialogs::LFileChooser theFileChooser;
	NavDialogOptions* theDialogOptions = theFileChooser.GetDialogOptions();
	if (theDialogOptions != NULL )
	{
		theDialogOptions->dialogOptionFlags &= ~(kNavAllowPreviews);
	}

	// ask the user to choose a file.
	FSSpec theFile;
	if (theFileChooser.AskChooseOneFile( theTypeList, theFile ))
	{
		Handle theMacSoundH;
		
		// create an LFileStream object from the file's spec.
		LFileStream theFileStream( theFile );
		// open the file with read permission.
		SInt16 theRefNum = theFileStream.OpenDataFork( fsRdPerm );
		
		// parse AIFF header for info.
		SoundComponentData theSoundData;
		UInt32 numFrames;
		UInt32 dataOffset;
		ThrowIfOSErr_( ::ParseAIFFHeader( theRefNum, &theSoundData, &numFrames, &dataOffset ) );
		
		// calculate real length of the sampled data.
		// there are numFrames per channel, and each frame is 8 or 16-bit long.
		SInt32 dataLength = numFrames * theSoundData.numChannels
							* (theSoundData.sampleSize / (inIs8bit ? 8 : 16));
		
		SInt16 headerLength;
		{
			// allocate handle for the sound header.
			StHandleBlock theSoundHeader( 100L );	// according to IM: Sound
			
			// try setting setup sound header to get header length.
			ThrowIfOSErr_( ::SetupSndHeader( (SndListHandle) ((Handle) theSoundHeader),
											 theSoundData.numChannels,
											 theSoundData.sampleRate,
											 theSoundData.sampleSize,
											 theSoundData.format,		// ???
											 kMiddleC,
											 0,						// so we know header length.
											 &headerLength ) );
		}
						
		// allocate true handle for the sound.
		theMacSoundH = ::NewHandle( headerLength + dataLength );
		ThrowIfNil_( theMacSoundH );
		
		{
			// lock handle so we can use it as a pointer.
			StHandleLocker theHandleLocker( theMacSoundH );
			
			// calculate offset in memory where the sound must be written.
			Ptr soundInHandleP = *theMacSoundH;
			soundInHandleP += headerLength;		// we skip the header.
			
			// read sound from stream.
			theFileStream.SetMarker( dataOffset, streamFrom_Start );
			theFileStream.ReadBlock( soundInHandleP, dataLength );
		}
		
		// setup sound header for real now.
		ThrowIfOSErr_( ::SetupSndHeader( (SndListHandle) theMacSoundH,
										 theSoundData.numChannels,
										 theSoundData.sampleRate,
										 theSoundData.sampleSize,
										 theSoundData.format,		// ???
										 kMiddleC,
										 dataLength,
										 &headerLength ) );
	
		// close file.
		theFileStream.CloseDataFork();
		
		// turn the sound into a Marathon sound.
		Handle theMthonSoundH = CWailSoundClass::TurnMacSoundIntoMthonSound( theMacSoundH );
		
		// dispose of the mac sound since it's no longer needed.
		::DisposeHandle( theMacSoundH );
		
		// create a stream to store the sound.
		CWailSoundStream* theHandleStream = new CWailSoundStream( theMthonSoundH );
			// the stream now "owns" the handle and will dispose of it when destroyed.
		
		// add a sound to the current class.
		ActionAddSound( theHandleStream, inIs8bit );
	}
	
	return true;
}


// ---------------------------------------------------------------------------------
//		¥ ExportAiffSound
// ---------------------------------------------------------------------------------
// exports the currently selected sound in the given list as an AIFF Sound file.

Boolean	
CWailDocWindow::ExportAiffSound( Boolean inIs8bit )
{
	// get the proper sound list.
	LPane* theSoundList = FindPaneByID( inIs8bit ? pane_8bitSoundsList : pane_16bitSoundsList );
	ThrowIfNil_( theSoundList );

	// check which sound is selected.
	SInt32 theSelectedSound = theSoundList->GetValue();
	ThrowIf_( theSelectedSound == -1 );
	
	// build default name for sound file.
	LStr255 defaultName( STRx_SoundStrings, str_SoundFileDefaultName );
	defaultName += (theSelectedSound + 1);
	defaultName += LStr255( STRx_SoundStrings, str_AIFFFileNameFooter );
	
	// create a file designator, configure it.
	PP_StandardDialogs::LFileDesignator theFileDesignator;
	theFileDesignator.SetFileType( fileType_AIFFSound );
	NavDialogOptions* theDialogOptions = theFileDesignator.GetDialogOptions();
	if (theDialogOptions != NULL)
	{
		theDialogOptions->dialogOptionFlags |= kNavNoTypePopup;
	}
	
	// ask user to save his System-7 sound file.
	if (theFileDesignator.AskDesignateFile( defaultName ))
	{
		FSSpec theFile;
		theFileDesignator.GetFileSpec( theFile );
	
		// delete the previous file if we're replacing it.
		if (theFileDesignator.IsReplacing())
			ThrowIfOSErr_( ::FSpDelete( &theFile ) );
			
		// get the current class.
		CWailSoundClass* theClass;
		mSoundFileData->mSoundClasses.FetchItemAt( mCurrentClass + 1, theClass );
		
		// get the proper sound stream array.
		LStream** theSounds = (inIs8bit ? theClass->m8bitSounds : theClass->m16bitSounds);
	
		// get the sound stream.
		LStream *theSoundStream = theSounds[(SInt16) theSelectedSound];
		
		// create a handle to store the sound.
		Handle theMthonSoundH = ::NewHandle( theSoundStream->GetLength() );
		ThrowIfNil_( theMthonSoundH );
		
		{
			// lock the handle so we can pass it as a buffer.
			StHandleLocker theMthonSoundHLocker( theMthonSoundH );
			
			// set the stream's marker to the beginning of the stream and read the sound.
			theSoundStream->SetMarker( 0, streamFrom_Start );
			theSoundStream->ReadBlock( *theMthonSoundH, theSoundStream->GetLength() );
		}
		
		// turn that sound into a real Mac sound (since it's a Marathon sound).
		Handle theMacSoundH = CWailSoundClass::TurnMthonSoundIntoMacSound( theMthonSoundH );
		
		// dispose of the first handle. (it's no longer of use.)
		::DisposeHandle( theMthonSoundH );
		
		// get info about the sound.
		SoundComponentData theSoundData;
		UInt32 numFrames;
		UInt32 dataOffset;
		ThrowIfOSErr_( ::ParseSndHeader( (SndListHandle) theMacSoundH,
										 &theSoundData,
										 &numFrames,
										 &dataOffset ) );
		
		// create a file to store the sound.
		LFileStream theAiffFile( theFile );
		theAiffFile.CreateNewFile( fileCreator_Unknown,
								   fileType_AIFFSound,
								   theFileDesignator.GetScriptCode() );
								   
		// open the file.
		theAiffFile.OpenDataFork( fsRdWrPerm );
		SInt16 refNum = theAiffFile.GetDataForkRefNum();
		
		// setup basic AIFF header (no length yet).
		ThrowIfOSErr_( ::SetupAIFFHeader( refNum,
										  theSoundData.numChannels,
										  theSoundData.sampleRate,
										  theSoundData.sampleSize,
										  theSoundData.format,
										  0,	// this is only to set it up - no length yet.
										  theSoundData.sampleCount ) );
										  
		// calculate real length of the sampled data.
		// there are numFrames per channel, and each frame is 8 or 16-bit long.
		SInt32 dataLength = numFrames * theSoundData.numChannels
							* (theSoundData.sampleSize / (inIs8bit ? 8 : 16));
							
		{
			// lock the sound handle so we can use it as a buffer.
			StHandleLocker theHandleLocker( theMacSoundH );
			
			
			
			// write sampled data to the file.
			theAiffFile.WriteBlock( (*theMacSoundH) + dataOffset, dataLength );
		}
		
		
		// position the marker back to the start of the file.
		theAiffFile.SetMarker( 0, streamFrom_Start );
		
		// re-setup the AIFF header, this time with length info.
		ThrowIfOSErr_( ::SetupAIFFHeader( refNum,
										  theSoundData.numChannels,
										  theSoundData.sampleRate,
										  theSoundData.sampleSize,
										  theSoundData.format,
										  dataLength,
										  theSoundData.sampleCount ) );
										  
		// close the file.
		theAiffFile.CloseDataFork();
		
		// dispose of the mac sound.
		::DisposeHandle( theMacSoundH );
	}
	
	return true;
}


// ---------------------------------------------------------------------------------
//		¥ PlaySound
// ---------------------------------------------------------------------------------
// plays the currently selected sound in the given list.

Boolean
CWailDocWindow::PlaySound(
	Boolean inLoop,
	Boolean inIs8bit )
{
	// get the proper sound list.
	LPane* theSoundList = FindPaneByID( inIs8bit ? pane_8bitSoundsList : pane_16bitSoundsList );
	ThrowIfNil_( theSoundList );

	// check which sound is selected.
	SInt32 theSelectedSound = theSoundList->GetValue();
	if (theSelectedSound != -1)
	{
		// get the current class.
		CWailSoundClass* theClass;
		mSoundFileData->mSoundClasses.FetchItemAt( mCurrentClass + 1, theClass );
		
		// get the proper sound stream array.
		LStream** theSounds = (inIs8bit ? theClass->m8bitSounds : theClass->m16bitSounds);
	
		// get the sound stream.
		LStream* theSoundStream = theSounds[(SInt16) theSelectedSound];
		
		// create a handle to store the sound.
		Handle theMthonSoundH = ::NewHandle( theSoundStream->GetLength() );
		ThrowIfNil_( theMthonSoundH );
		
		// lock the handle so we can pass it as a buffer.
		StHandleLocker theMthonSoundHLocker( theMthonSoundH );
		
		// set the stream's marker to the beginning of the stream and read the sound.
		theSoundStream->SetMarker( 0, streamFrom_Start );
		theSoundStream->ReadBlock( *theMthonSoundH, theSoundStream->GetLength() );
		
		// turn that sound into a real Mac sound (since it's a Marathon sound).
		Handle theMacSoundH = CWailSoundClass::TurnMthonSoundIntoMacSound( theMthonSoundH );
		
		// dispose of the first handle. (it's no longer of use.)
		::DisposeHandle( theMthonSoundH );
		
		// play the sound.
		UJukebox::PlaySound( (SndListHandle) theMacSoundH, true, true, inLoop );
	}
	
	return true;
}


// ---------------------------------------------------------------------------------
//		¥ ActionAddSound
// ---------------------------------------------------------------------------------
// adds a sound to a sound list using an undoable LAction object.

void
CWailDocWindow::ActionAddSound(
	LStream*	inSound,
	Boolean		inIs8bit )
{
	CAddSoundAction *theAction;
	
	// create the action.
	theAction = new CAddSoundAction( inSound,
									 mCurrentClass,
									 inIs8bit,
									 this );
	ThrowIfNil_( theAction );
	
	// post it.
	PostAction( theAction );
}


// ---------------------------------------------------------------------------------
//		¥ ActionRemoveSound
// ---------------------------------------------------------------------------------
// removes a sound from a sound list using an undoable LAction object.

void
CWailDocWindow::ActionRemoveSound(
	SInt32		inWhichSound,
	Boolean		inIs8bit )
{
	CRemoveSoundAction *theAction;
	
	// create the action.
	theAction = new CRemoveSoundAction( mCurrentClass,
										inWhichSound,
									    inIs8bit,
									    this );
	ThrowIfNil_( theAction );
	
	// post it.
	PostAction( theAction );
}


// ---------------------------------------------------------------------------
//		¥ ActionAddClasses
// ---------------------------------------------------------------------------
// adds empty classes to the file using an undoable action object.

void
CWailDocWindow::ActionAddClasses(
	SInt32	inHowMany )
{
	CAddClassesAction* theAction;
	
	// create the action.
	theAction = new CAddClassesAction( inHowMany,
									   this );
	ThrowIfNil_( theAction );
	
	// post it.
	PostAction( theAction );
}


// ---------------------------------------------------------------------------
//		¥ ActionRemoveClass
// ---------------------------------------------------------------------------
// removes the current class from the file using an undoable action object.

void
CWailDocWindow::ActionRemoveClass()
{
	CRemoveClassesAction* theAction;
	
	// create the action.
	theAction = new CRemoveClassesAction( mCurrentClass,
										  1,
										  this );
	ThrowIfNil_( theAction );
	
	// post it.
	PostAction( theAction );
}


// ---------------------------------------------------------------------------------
//		¥ ActionChangeFlag
// ---------------------------------------------------------------------------------
// creates an action to Undo/Redo the changes of flags.

void
CWailDocWindow::ActionChangeFlag(
	MessageT	inFlagMessage )
{
	CChangeFlagAction *theAction;
	
	// get appropriate control for this flag. controls have same IDs as their messages.
	LControl* theFlagCheckBox = (LControl*) FindPaneByID( (PaneIDT) inFlagMessage );
	// see if that control is checked or not.
	Boolean isChecked = (theFlagCheckBox->GetValue() != 0);
	
	// create the action.
	theAction = new CChangeFlagAction( theFlagCheckBox,
									   isChecked,
									   mCurrentClass,
									   this );
	ThrowIfNil_( theAction );
	
	// post it.
	PostAction( theAction );
}


// ---------------------------------------------------------------------------------
//		¥ ActionChangeVolume
// ---------------------------------------------------------------------------------
// creates an action to Undo/Redo the changes of volume.

void
CWailDocWindow::ActionChangeVolume(
	SInt16	inOldVolume,
	SInt16	inNewVolume )
{
	CChangeVolumeAction *theAction;
	
	// get control for old volume.
	LControl* theOldVolumeControl;
	switch (inOldVolume)
	{
		case volume_Soft:
			theOldVolumeControl = (LControl*) FindPaneByID( pane_SoftVolumeRadioButton );
			break;

		case volume_Medium:
			theOldVolumeControl = (LControl*) FindPaneByID( pane_MediumVolumeRadioButton );
			break;

		case volume_Loud:
			theOldVolumeControl = (LControl*) FindPaneByID( pane_LoudVolumeRadioButton );
			break;

		case volume_Volume3:
			theOldVolumeControl = (LControl*) FindPaneByID( pane_Volume3RadioButton );
			break;
	}
	
	// get control for new volume.
	LControl* theNewVolumeControl;
	switch (inNewVolume)
	{
		case volume_Soft:
			theNewVolumeControl = (LControl*) FindPaneByID( pane_SoftVolumeRadioButton );
			break;

		case volume_Medium:
			theNewVolumeControl = (LControl*) FindPaneByID( pane_MediumVolumeRadioButton );
			break;

		case volume_Loud:
			theNewVolumeControl = (LControl*) FindPaneByID( pane_LoudVolumeRadioButton );
			break;

		case volume_Volume3:
			theNewVolumeControl = (LControl*) FindPaneByID( pane_Volume3RadioButton );
			break;
	}
	
	// create the action.
	theAction = new CChangeVolumeAction( theOldVolumeControl,
									   	 theNewVolumeControl,
									   	 mCurrentClass,
									   	 this );
	ThrowIfNil_( theAction );
	
	// post it.
	PostAction( theAction );
}


// ---------------------------------------------------------------------------------
//		¥ ActionDontRemap8bit
// ---------------------------------------------------------------------------------
// creates an action to Undo/Redo the removal of the Remap 8-bit flag.

void
CWailDocWindow::ActionDontRemap8bit()
{
	CDontRemap8bitAction *theAction;
	
	// get control for Remap 8-bit check box.
	LControl* theRemap8bitCheckBox =
		(LControl*) FindPaneByID( pane_Remap8bitCheckBox );
	
	// create the action.
	theAction = new CDontRemap8bitAction( theRemap8bitCheckBox,
									   	  mCurrentClass,
									   	  this );
	ThrowIfNil_( theAction );
	
	// post it.
	PostAction( theAction );
}


// ---------------------------------------------------------------------------------
//		¥ ActionRemap8bit
// ---------------------------------------------------------------------------------
// creates an action to Undo/Redo the checking of the Remap 8-bit flag.

void
CWailDocWindow::ActionRemap8bit()
{
	CRemap8bitAction *theAction;
	
	// get control for Remap 8-bit check box.
	LControl* theRemap8bitCheckBox =
		(LControl*) FindPaneByID( pane_Remap8bitCheckBox );
	
	// create the action.
	theAction = new CRemap8bitAction( theRemap8bitCheckBox,
									  mCurrentClass,
									  this );
	ThrowIfNil_( theAction );
	
	// post it.
	PostAction( theAction );
}


// ---------------------------------------------------------------------------
//		¥ ActionChangeChance
// ---------------------------------------------------------------------------
// creates an action to Undo/Redo the changing of the Chance value.

void
CWailDocWindow::ActionChangeChance(
	SInt16			inOldValue,
	SInt16			inNewValue )
{
	CChangeChanceAction* theAction;
	
	// create the action.
	theAction = new CChangeChanceAction( inOldValue,
										 inNewValue,
										 mCurrentClass,
										 this );
	ThrowIfNil_( theAction );
	
	// post it.
	PostAction( theAction );
}


#pragma mark -

// =================================================================================
//		StRemapping
// =================================================================================
//
// Constructor saves the window's "Remapping" state and optionally changes it.
// Destructor restores the previous state.


// ---------------------------------------------------------------------------
//		¥ StRemapping
// ---------------------------------------------------------------------------

StRemapping::StRemapping(
	CWailDocWindow&		inWindow )
	: mWindow( inWindow ),
	  mSavedState( inWindow.IsRemapping() )
{
}


// ---------------------------------------------------------------------------
//		¥ StPerformingAction
// ---------------------------------------------------------------------------

StRemapping::StRemapping(
	CWailDocWindow&		inWindow,
	Boolean				inNewState )
	: mWindow( inWindow ),
	  mSavedState( inWindow.IsRemapping() )
{
	mWindow.SetIsRemapping( inNewState );
}


// ---------------------------------------------------------------------------
//		¥ ~StRemapping
// ---------------------------------------------------------------------------

StRemapping::~StRemapping()
{
	mWindow.SetIsRemapping( mSavedState );
}


#pragma mark -

// =================================================================================
//		StPerformingAction
// =================================================================================
//
// Constructor saves the window's "Performing action" state and optionally changes it.
// Destructor restores the previous state.


// ---------------------------------------------------------------------------
//		¥ StPerformingAction
// ---------------------------------------------------------------------------

StPerformingAction::StPerformingAction(
	CWailDocWindow&		inWindow )
	: mWindow( inWindow ),
	  mSavedState( inWindow.IsPerformingAction() )
{
}


// ---------------------------------------------------------------------------
//		¥ StPerformingAction
// ---------------------------------------------------------------------------

StPerformingAction::StPerformingAction(
	CWailDocWindow&		inWindow,
	Boolean				inNewState )
	: mWindow( inWindow ),
	  mSavedState( inWindow.IsPerformingAction() )
{
	mWindow.SetIsPerformingAction( inNewState );
}


// ---------------------------------------------------------------------------
//		¥ ~StPerformingAction
// ---------------------------------------------------------------------------

StPerformingAction::~StPerformingAction()
{
	mWindow.SetIsPerformingAction( mSavedState );
}


#pragma mark -

// =================================================================================
//		StChangingClass
// =================================================================================
//
// Constructor saves the window's "Changing class" state and optionally changes it.
// Destructor restores the previous state.


// ---------------------------------------------------------------------------
//		¥ StChangingClass
// ---------------------------------------------------------------------------

StChangingClass::StChangingClass(
	CWailDocWindow&		inWindow )
	: mWindow( inWindow ),
	  mSavedState( inWindow.IsChangingClass() )
{
}


// ---------------------------------------------------------------------------
//		¥ StChangingClass
// ---------------------------------------------------------------------------

StChangingClass::StChangingClass(
	CWailDocWindow&		inWindow,
	Boolean				inNewState )
	: mWindow( inWindow ),
	  mSavedState( inWindow.IsChangingClass() )
{
	mWindow.SetIsChangingClass( inNewState );
}


// ---------------------------------------------------------------------------
//		¥ ~StChangingClass
// ---------------------------------------------------------------------------

StChangingClass::~StChangingClass()
{
	mWindow.SetIsChangingClass( mSavedState );
}
