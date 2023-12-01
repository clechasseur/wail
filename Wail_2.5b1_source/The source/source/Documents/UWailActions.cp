// =================================================================================
//	UWailActions.cp					©1999, Charles Lechasseur
// =================================================================================
//
// classes describing Wail undoable actions.

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

#include "UWailActions.h"

#include "UMoreDrawingState.h"
#include "CSingleColumnTextListBox.h"

#include "UWailClassNames.h"


#pragma mark -- Constants --

// constants describing the resource ID and position of the various Undo/Redo strings.

const	ResIDT	rSTRL_RedoSoundActionsStringList		= 1000;
const	ResIDT	rSTRL_UndoSoundActionsStringList		= 1001;

const	SInt16	actionString_RemoveSound				= 1;
const	SInt16	actionString_AddSound					= 2;

const	ResIDT	rSTRL_RedoClassActionsStringList		= 1002;
const	ResIDT	rSTRL_UndoClassActionsStringList		= 1003;

const	SInt16	actionString_RemoveClass				= 1;
const	SInt16	actionString_RemoveClasses				= 2;
const	SInt16	actionString_AddClass					= 3;
const	SInt16	actionString_AddClasses					= 4;

const	ResIDT	rSTRL_RedoFlagsActionsStringList		= 1004;
const	ResIDT	rSTRL_UndoFlagsActionsStringList		= 1005;

const	SInt16	actionString_ChangeFlag					= 1;
const	SInt16	actionString_ChangeVolume				= 2;
const	SInt16	actionString_DontRemap8bit				= 3;
const	SInt16	actionString_Remap8bit					= 4;
const	SInt16	actionString_ChangeChance				= 5;


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ CAddSoundAction							Constructor	[public]
// ---------------------------------------------------------------------------

CAddSoundAction::CAddSoundAction(
	LStream*			inSound,
	SInt32				inClassNumber,
	Boolean				inIs8bit,
	CWailDocWindow*		inWindow )
	: LAction( rSTRL_RedoSoundActionsStringList,
			   actionString_AddSound,
			   /* already done? */ false )
{
	mSound = inSound; // we now "own" that LStream.
	mClassNumber = inClassNumber;
	mIs8bit = inIs8bit;
	mWindow = inWindow;
}


// ---------------------------------------------------------------------------
//	¥ ~CAddSoundAction							Destructor	[public]
// ---------------------------------------------------------------------------

CAddSoundAction::~CAddSoundAction()
{
	// if the action was undoed, the sound doesn't belong anywhere anymore,
	// so we need to delete the stream.
	if ( !IsDone() )
		delete mSound;
}


// ---------------------------------------------------------------------------------
//		¥ RedoSelf
// ---------------------------------------------------------------------------------
// gets called by Redo when the action needs to be Redone. also functions as DoSelf
// the first time the action is posted.
//
// we need to add the sound to the given sound list.

void
CAddSoundAction::RedoSelf()
{
	// get the current class.
	CWailSoundClass *theClass;
	mWindow->GetSoundFileData()->mSoundClasses.FetchItemAt( mClassNumber + 1, theClass );

	// store this stream in the class' member variable.
	if (mIs8bit)
		theClass->m8bitSounds[theClass->mNum8bitSounds] = mSound;
	else
		theClass->m16bitSounds[theClass->mNum16bitSounds] = mSound;
	
	// add one to the number of sounds.
	if (mIs8bit)
		theClass->mNum8bitSounds++;
	else
		theClass->mNum16bitSounds++;
	
	// mark the window as dirty.
	mWindow->SetDirty( true );
	
	// if the current class is not in view, select it and scroll it in view.
	if (mClassNumber != mWindow->GetCurrentClass())
		mWindow->SelectClass( mClassNumber,
							  /* refresh list? */ true );
	else
		// refresh the sound list.
		if (mIs8bit)
		{
			mWindow->Clear8bitSoundsList();
			mWindow->Fill8bitSoundsList();
		}
		else
		{
			mWindow->Clear16bitSoundsList();
			mWindow->Fill16bitSoundsList();
		}
}


// ---------------------------------------------------------------------------------
//		¥ UndoSelf
// ---------------------------------------------------------------------------------
// gets called by Undo when the action needs to be Undone.
//
// we need to remove the last sound from the given sound list.

void
CAddSoundAction::UndoSelf()
{
	// get the current class.
	CWailSoundClass *theClass;
	mWindow->GetSoundFileData()->mSoundClasses.FetchItemAt( mClassNumber + 1, theClass );
	
	// substract one to the number of sounds.
	if (mIs8bit)
		theClass->mNum8bitSounds--;
	else
		theClass->mNum16bitSounds--;
	
	// remove the last sound from the list.
	if (mIs8bit)
		theClass->m8bitSounds[theClass->mNum8bitSounds] = nil;
	else
		theClass->m16bitSounds[theClass->mNum16bitSounds] = nil;
	// we don't really delete the stream, because we keep it in this object
	// in case we have to Redo. destructor will delete it if the action was not done.
	
	// if the current class is not in view, select it and scroll it in view.
	if (mClassNumber != mWindow->GetCurrentClass())
		mWindow->SelectClass( mClassNumber,
							  /* refresh list? */ true );
	else
		// refresh the sound list.
		if (mIs8bit)
		{
			mWindow->Clear8bitSoundsList();
			mWindow->Fill8bitSoundsList();
		}
		else
		{
			mWindow->Clear16bitSoundsList();
			mWindow->Fill16bitSoundsList();
		}	
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ CRemoveSoundAction							Constructor	[public]
// ---------------------------------------------------------------------------

CRemoveSoundAction::CRemoveSoundAction(
	SInt32				inClassNumber,
	SInt32				inSoundNumber,
	Boolean				inIs8bit,
	CWailDocWindow*		inWindow )
	: LAction( rSTRL_RedoSoundActionsStringList,
			   actionString_RemoveSound,
			   /* already done? */ false )
{
	mSound = nil; // we don't currently have the sound.
	mClassNumber = inClassNumber;
	mSoundNumber = inSoundNumber;
	mIs8bit = inIs8bit;
	mWindow = inWindow;
}


// ---------------------------------------------------------------------------
//	¥ ~CRemoveSoundAction							Destructor	[public]
// ---------------------------------------------------------------------------

CRemoveSoundAction::~CRemoveSoundAction()
{
	// if the action was not undoed, the sound doesn't belong anywhere anymore,
	// so we need to delete the stream.
	if ( IsDone() )
		delete mSound;
}


// ---------------------------------------------------------------------------------
//		¥ RedoSelf
// ---------------------------------------------------------------------------------
// gets called by Redo when the action needs to be Redone. also functions as DoSelf
// the first time the action is posted.
//
// we need to remove the sound from the given sound list.

void
CRemoveSoundAction::RedoSelf()
{
	// get the sound class.
	CWailSoundClass *theClass;
	mWindow->GetSoundFileData()->mSoundClasses.FetchItemAt( mClassNumber + 1, theClass );
	
	// delete the sound.
	if (mIs8bit)
	{
		mSound = theClass->m8bitSounds[mSoundNumber];
		theClass->m8bitSounds[mSoundNumber] = nil;
	}
	else
	{
		mSound = theClass->m16bitSounds[mSoundNumber];
		theClass->m16bitSounds[mSoundNumber] = nil;
	}
	// we don't delete the sound now, in case we have to Undo. we keep it in RAM.
	// destructor will delete it if necessary.
	
	// move sounds down if necessary.
	if (mSoundNumber < (mIs8bit
						 ? theClass->mNum8bitSounds - 1
						 : theClass->mNum16bitSounds - 1) )
	{
		SInt16 i;
		for (i = mSoundNumber; i < (mIs8bit
						 			? theClass->mNum8bitSounds - 1
						 			: theClass->mNum16bitSounds - 1); i++)
			if (mIs8bit)
				theClass->m8bitSounds[i] = theClass->m8bitSounds[i + 1];
			else
				theClass->m16bitSounds[i] = theClass->m16bitSounds[i + 1];
	}
	
	// reduce the number of sounds.
	if (mIs8bit)
		theClass->mNum8bitSounds--;
	else
		theClass->mNum16bitSounds--;
	
	// mark the window as dirty.
	mWindow->SetDirty( true );
	
	// if the current class is not in view, select it and scroll it in view.
	if (mClassNumber != mWindow->GetCurrentClass())
		mWindow->SelectClass( mClassNumber,
							  /* refresh list? */ true );
	else
		// refresh the sound list.
		if (mIs8bit)
		{
			mWindow->Clear8bitSoundsList();
			mWindow->Fill8bitSoundsList();
		}
		else
		{
			mWindow->Clear16bitSoundsList();
			mWindow->Fill16bitSoundsList();
		}
}


// ---------------------------------------------------------------------------------
//		¥ UndoSelf
// ---------------------------------------------------------------------------------
// gets called by Undo when the action needs to be Undone.
//
// we need to add the sound back to the given sound list.

void
CRemoveSoundAction::UndoSelf()
{
	// get the sound class.
	CWailSoundClass *theClass;
	mWindow->GetSoundFileData()->mSoundClasses.FetchItemAt( mClassNumber + 1, theClass );
	
	// move sounds back up if necessary.
	if (mSoundNumber < (mIs8bit
						 ? theClass->mNum8bitSounds
						 : theClass->mNum16bitSounds) )
	{
		SInt16 i;
		for (i = (mIs8bit
					? theClass->mNum8bitSounds
					: theClass->mNum16bitSounds); i > mSoundNumber; i--)
			if (mIs8bit)
				theClass->m8bitSounds[i] = theClass->m8bitSounds[i - 1];
			else
				theClass->m16bitSounds[i] = theClass->m16bitSounds[i - 1];
	}
	
	// put the sound back in.
	if (mIs8bit)
		theClass->m8bitSounds[mSoundNumber] = mSound;
	else
		theClass->m16bitSounds[mSoundNumber] = mSound;
	
	// add 1 to the number of sounds.
	if (mIs8bit)
		theClass->mNum8bitSounds++;
	else
		theClass->mNum16bitSounds++;
	
	// if the current class is not in view, select it and scroll it in view.
	if (mClassNumber != mWindow->GetCurrentClass())
		mWindow->SelectClass( mClassNumber,
							  /* refresh list? */ true );
	else
		// refresh the sound list.
		if (mIs8bit)
		{
			mWindow->Clear8bitSoundsList();
			mWindow->Fill8bitSoundsList();
		}
		else
		{
			mWindow->Clear16bitSoundsList();
			mWindow->Fill16bitSoundsList();
		}
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ CChangeFlagAction							Constructor	[public]
// ---------------------------------------------------------------------------

CChangeFlagAction::CChangeFlagAction(
	LControl*			inFlagCheckBox,
	Boolean				inWasChecked,
	SInt32				inClassNumber,
	CWailDocWindow*		inWindow )
	: LAction( rSTRL_RedoFlagsActionsStringList,
			   actionString_ChangeFlag,
			   /* already done? */ true )	// always already done.
{
	mFlagCheckBox = inFlagCheckBox;		// we don't own that control.
	mWasChecked = inWasChecked;
	mClassNumber = inClassNumber;
	mWindow = inWindow;
}


// ---------------------------------------------------------------------------------
//		¥ RedoSelf
// ---------------------------------------------------------------------------------
// gets called by Redo when the action needs to be Redone.
//
// we need to restore the checkbox to the value it was when the action was created.

void
CChangeFlagAction::RedoSelf()
{
	// tell window we're performing an action.
	StPerformingAction performingAction( *mWindow, true );

	// if our class is not the currently selected class, select ours.
	if (mClassNumber != mWindow->GetCurrentClass())
		mWindow->SelectClass( mClassNumber,
							  /* refresh list? */ true );
	
	// set value according to mWasChecked.
	mFlagCheckBox->SetValue( mWasChecked ? Button_On : Button_Off );
}


// ---------------------------------------------------------------------------------
//		¥ UndoSelf
// ---------------------------------------------------------------------------------
// gets called by Undo when the action needs to be Undone.
//
// we need to restore the checkbox to the value it was before the user changed it.

void
CChangeFlagAction::UndoSelf()
{
	// tell window we're performing an action.
	StPerformingAction performingAction( *mWindow, true );
	
	// if our class is not the currently selected class, select ours.
	if (mClassNumber != mWindow->GetCurrentClass())
		mWindow->SelectClass( mClassNumber,
							  /* refresh list? */ true );

	// set value according to mWasChecked.
	mFlagCheckBox->SetValue( mWasChecked ? Button_Off : Button_On );
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ CChangeVolumeAction							Constructor	[public]
// ---------------------------------------------------------------------------

CChangeVolumeAction::CChangeVolumeAction(
	LControl*			inOldVolumeControl,
	LControl*			inNewVolumeControl,
	SInt32				inClassNumber,
	CWailDocWindow*		inWindow )
	: LAction( rSTRL_RedoFlagsActionsStringList,
			   actionString_ChangeVolume,
			   /* already done? */ true )	// always already done.
{
	mOldVolumeControl = inOldVolumeControl;		// we don't own that control.
	mNewVolumeControl = inNewVolumeControl;		// we don't own that control.
	mClassNumber = inClassNumber;
	mWindow = inWindow;
}


// ---------------------------------------------------------------------------------
//		¥ RedoSelf
// ---------------------------------------------------------------------------------
// gets called by Redo when the action needs to be Redone.
//
// we need to re-select the new radio.

void
CChangeVolumeAction::RedoSelf()
{
	// tell window we're performing an action.
	StPerformingAction performingAction( *mWindow, true );

	// if our class is not the currently selected class, select ours.
	if (mClassNumber != mWindow->GetCurrentClass())
		mWindow->SelectClass( mClassNumber,
							  /* refresh list? */ true );
	
	// re-select new radio.
	mNewVolumeControl->SetValue( Button_On );
}


// ---------------------------------------------------------------------------------
//		¥ UndoSelf
// ---------------------------------------------------------------------------------
// gets called by Undo when the action needs to be Undone.
//
// we need to re-select the old radio.

void
CChangeVolumeAction::UndoSelf()
{
	// tell window we're performing an action.
	StPerformingAction performingAction( *mWindow, true );
	
	// if our class is not the currently selected class, select ours.
	if (mClassNumber != mWindow->GetCurrentClass())
		mWindow->SelectClass( mClassNumber,
							  /* refresh list? */ true );

	// re-select old radio.
	mOldVolumeControl->SetValue( Button_On );
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ CDontRemap8bitAction							Constructor	[public]
// ---------------------------------------------------------------------------

CDontRemap8bitAction::CDontRemap8bitAction(
	LControl*			inRemap8bitCheckBox,
	SInt32				inClassNumber,
	CWailDocWindow*		inWindow )
	: LAction( rSTRL_RedoFlagsActionsStringList,
			   actionString_DontRemap8bit,
			   /* already done? */ false )
{
	mRemap8bitCheckBox = inRemap8bitCheckBox;		// we don't own that control.
	mClassNumber = inClassNumber;
	mWindow = inWindow;
}


// ---------------------------------------------------------------------------------
//		¥ RedoSelf
// ---------------------------------------------------------------------------------
// gets called by Redo when the action needs to be Redone.
//
// we need to uncheck the Remap 8-bit check box.

void
CDontRemap8bitAction::RedoSelf()
{
	// tell window we're remapping.
	StRemapping remapping( *mWindow, true );

	// if our class is not the currently selected class, select ours.
	if (mClassNumber != mWindow->GetCurrentClass())
		mWindow->SelectClass( mClassNumber,
							  /* refresh list? */ true );
	
	// uncheck the Remap 8-bit check box.
	mRemap8bitCheckBox->SetValue( Button_Off );
	
	// enable the list.
	mWindow->Fill16bitSoundsList();
	// mark class as no longer re-mapped.
	CWailSoundClass *theClass;
	mWindow->GetSoundFileData()->mSoundClasses.FetchItemAt( mClassNumber + 1, theClass );
	theClass->mRemap8bit = false;
	// remapping makes us dirty.
	mWindow->SetDirty( true );
}


// ---------------------------------------------------------------------------------
//		¥ UndoSelf
// ---------------------------------------------------------------------------------
// gets called by Undo when the action needs to be Undone.
//
// we need to re-check the Remap 8-bit check box.

void
CDontRemap8bitAction::UndoSelf()
{
	// tell window we're remapping.
	StRemapping remapping( *mWindow, true );
	
	// if our class is not the currently selected class, select ours.
	if (mClassNumber != mWindow->GetCurrentClass())
		mWindow->SelectClass( mClassNumber,
							  /* refresh list? */ true );

	// re-check Remap 8-bit check box.
	mRemap8bitCheckBox->SetValue( Button_On );
	
	// since the action originated from a removal of the Remap 8-bit check, the sound list
	// is surely empty. we don't have to bother about deleting its sounds here.
	
	// we have to empty the 16-bit sounds list.
	mWindow->Clear16bitSoundsList();
	// mark the class as remapped.
	CWailSoundClass *theClass;
	mWindow->GetSoundFileData()->mSoundClasses.FetchItemAt( mClassNumber + 1, theClass );
	theClass->mRemap8bit = true;
	// remapping makes us dirty.
	mWindow->SetDirty( true );
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ CRemap8bitAction							Constructor	[public]
// ---------------------------------------------------------------------------

CRemap8bitAction::CRemap8bitAction(
	LControl*			inRemap8bitCheckBox,
	SInt32				inClassNumber,
	CWailDocWindow*		inWindow )
	: LAction( rSTRL_RedoFlagsActionsStringList,
			   actionString_Remap8bit,
			   /* already done? */ false )
{
	mRemap8bitCheckBox = inRemap8bitCheckBox;		// we don't own that control.
	mClassNumber = inClassNumber;
	mWindow = inWindow;
	
	// initialize our streams to nil.
	SInt16 i;
	for (i = 0; i < 5; i++)
		m16bitSounds[i] = nil;
	mNum16bitSounds = 0;
}


// ---------------------------------------------------------------------------
//	¥ ~CRemap8bitAction							Destructor	[public]
// ---------------------------------------------------------------------------

CRemap8bitAction::~CRemap8bitAction()
{
	// if the action was done, it means that all sounds we saved must be deleted.
	if (IsDone())
	{
		SInt16 i;
		for (i = 0; i < mNum16bitSounds; i++)
			if (m16bitSounds[i] != nil)
				delete m16bitSounds[i];
	}
}


// ---------------------------------------------------------------------------------
//		¥ RedoSelf
// ---------------------------------------------------------------------------------
// gets called by Redo when the action needs to be Redone.
//
// we need to check the Remap 8-bit check box and save 16-bit sounds.

void
CRemap8bitAction::RedoSelf()
{
	// tell window we're remapping.
	StRemapping remapping( *mWindow, true );

	// if our class is not the currently selected class, select ours.
	if (mClassNumber != mWindow->GetCurrentClass())
		mWindow->SelectClass( mClassNumber,
							  /* refresh list? */ true );
	
	// check the Remap 8-bit check box.
	mRemap8bitCheckBox->SetValue( Button_On );
	
	// get current class.
	CWailSoundClass *theClass;
	mWindow->GetSoundFileData()->mSoundClasses.FetchItemAt( mClassNumber + 1, theClass );
	
	// we must delete all 16-bit sounds but store them in case we need them later.
	SInt16 i;
	for (i = 0; i < theClass->mNum16bitSounds; i++)
	{
		m16bitSounds[i] = theClass->m16bitSounds[i];
		theClass->m16bitSounds[i] = nil;
	}
	mNum16bitSounds = theClass->mNum16bitSounds;
	theClass->mNum16bitSounds = 0;
	
	// we have to empty the 16-bit sounds list.
	mWindow->Clear16bitSoundsList();
	// mark the class as remapped.
	theClass->mRemap8bit = true;
	// remapping makes us dirty.
	mWindow->SetDirty( true );
}


// ---------------------------------------------------------------------------------
//		¥ UndoSelf
// ---------------------------------------------------------------------------------
// gets called by Undo when the action needs to be Undone.
//
// we need to uncheck the Remap 8-bit check box and place the saved sounds back.

void
CRemap8bitAction::UndoSelf()
{
	// tell window we're remapping.
	StRemapping remapping( *mWindow, true );

	// if our class is not the currently selected class, select ours.
	if (mClassNumber != mWindow->GetCurrentClass())
		mWindow->SelectClass( mClassNumber,
							  /* refresh list? */ true );
	
	// uncheck the Remap 8-bit check box.
	mRemap8bitCheckBox->SetValue( Button_Off );
	
	// get current class.
	CWailSoundClass *theClass;
	mWindow->GetSoundFileData()->mSoundClasses.FetchItemAt( mClassNumber + 1, theClass );
	
	// we must replace all 16-bit sounds where they were.
	SInt16 i;
	for (i = 0; i < mNum16bitSounds; i++)
	{
		theClass->m16bitSounds[i] = m16bitSounds[i];
		m16bitSounds[i] = nil;
	}
	theClass->mNum16bitSounds = mNum16bitSounds;
	mNum16bitSounds = 0;
	
	// enable the list.
	mWindow->Fill16bitSoundsList();
	// mark class as no longer re-mapped.
	theClass->mRemap8bit = false;
	// remapping makes us dirty.
	mWindow->SetDirty( true );
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ CChangeChanceAction							Constructor	[public]
// ---------------------------------------------------------------------------

CChangeChanceAction::CChangeChanceAction(
	SInt16				inOldValue,
	SInt16				inNewValue,
	SInt32				inClassNumber,
	CWailDocWindow*		inWindow )
	: LAction( rSTRL_RedoFlagsActionsStringList,
			   actionString_ChangeChance,
			   /* already done? */ true ),	// always already done.
	  mOldValue( inOldValue ),
	  mNewValue( inNewValue ),
	  mClassNumber( inClassNumber ),
	  mWindow( inWindow )
{
	SignalIf_( mWindow == nil );
}


// ---------------------------------------------------------------------------------
//		¥ RedoSelf
// ---------------------------------------------------------------------------------
// gets called by Redo when the action needs to be Redone.
//
// we need to set the class' Chance value to the "new value" given to us when this
// action was created.

void
CChangeChanceAction::RedoSelf()
{
	// tell window we're performing an action.
	StPerformingAction performingAction( *mWindow, true );

	// if our class is not the currently selected class, select ours.
	if (mClassNumber != mWindow->GetCurrentClass())
		mWindow->SelectClass( mClassNumber,
							  /* refresh list? */ true );
	
	// set Chance value to the new value.
	mWindow->SetChance( mNewValue, CWailDocWindow::state_NoEffect, true );
}


// ---------------------------------------------------------------------------------
//		¥ UndoSelf
// ---------------------------------------------------------------------------------
// gets called by Undo when the action needs to be Undone.
//
// we need to restore the checkbox to the value it was before the user changed it.

void
CChangeChanceAction::UndoSelf()
{
	// tell window we're performing an action.
	StPerformingAction performingAction( *mWindow, true );

	// if our class is not the currently selected class, select ours.
	if (mClassNumber != mWindow->GetCurrentClass())
		mWindow->SelectClass( mClassNumber,
							  /* refresh list? */ true );
	
	// set Chance value to the old value.
	mWindow->SetChance( mOldValue, CWailDocWindow::state_NoEffect, true );
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ CAddClassesAction							Constructor	[public]
// ---------------------------------------------------------------------------

CAddClassesAction::CAddClassesAction(
	SInt32				inHowMany,
	CWailDocWindow*		inWindow )
	: LAction( rSTRL_RedoClassActionsStringList,
			   (inHowMany > 1 ? actionString_AddClasses : actionString_AddClass),
			   /* already done? */ false ),
	  mHowMany( inHowMany ),
	  mWindow( inWindow )
{
	SignalIf_( mWindow == nil );
}


// ---------------------------------------------------------------------------------
//		¥ RedoSelf
// ---------------------------------------------------------------------------------
// gets called by Redo when the action needs to be Redone. also functions as "Do"
// when the action is first posted.
//
// we need to create a number of empty sound classes and add them to our window.

void
CAddClassesAction::RedoSelf()
{
	// get our window's sound file data.
	CWailSoundFileData* theSoundFileData = mWindow->GetSoundFileData();
	ThrowIfNULL_( theSoundFileData );
	
	// get our window's class listbox.
	CSingleColumnTextListBox* theClassListBox =
		dynamic_cast<CSingleColumnTextListBox*>(mWindow->FindPaneByID( pane_ClassList ));
	ThrowIfNULL_( theClassListBox );

	{
		// disable list drawing for speed.
		StDisableListDrawing theDrawingDisabler( *theClassListBox );	

		// add the classes.
		theSoundFileData->mSoundClasses.AdjustAllocation( mHowMany );
		for (SInt32 i = 0; i < mHowMany; ++i)
		{
			// create a new class.
			CWailSoundClass* theNewClass = new CWailSoundClass; // empty class.
			
			// add it to our data.
			theSoundFileData->mSoundClasses.AddItem( theNewClass );
			
			// add it to the list.
			LStr255 theRowString;
			UWailClassNames::GetClassName(
				(SInt32) theSoundFileData->mSoundClasses.GetCount(), theRowString );
			theClassListBox->AddRow( 0xffff, (ConstStringPtr) theRowString );
		}
	}
	
	// update the list. this fix is necessary in Pro4, so it seems. (2.2b1)
	theClassListBox->Refresh();
	
	// adding classes makes our window dirty.
	mWindow->SetDirty( true );
}


// ---------------------------------------------------------------------------------
//		¥ UndoSelf
// ---------------------------------------------------------------------------------
// gets called by Undo when the action needs to be Undone.
//
// we need to remove the classes we added in Redo.

void
CAddClassesAction::UndoSelf()
{
	// get our window's sound file data.
	CWailSoundFileData* theSoundFileData = mWindow->GetSoundFileData();
	ThrowIfNULL_( theSoundFileData );
	
	// get our window's class listbox.
	CSingleColumnTextListBox* theClassListBox =
		dynamic_cast<CSingleColumnTextListBox*>(mWindow->FindPaneByID( pane_ClassList ));
	ThrowIfNULL_( theClassListBox );
	
	// calculate the position in the list where the first class we added now is.
	// (this position is 0-based.)
	SInt32 theFirstAddedClass = theSoundFileData->mSoundClasses.GetCount() - mHowMany;

	// if the currently selected class is among those we added,
	// deselect class and clear class panes.
	if (mWindow->GetCurrentClass() >= theFirstAddedClass)
	{
		theClassListBox->UnselectAllCells();
		mWindow->SelectClass( -1 );
	}
	
	// delete classes info from the array.
	for (SInt32 i = theFirstAddedClass; i < (theFirstAddedClass + mHowMany); ++i)
	{
		CWailSoundClass* theSoundClass = NULL;
		ThrowIf_( !(theSoundFileData->mSoundClasses.FetchItemAt( i + 1, theSoundClass )) );
		delete theSoundClass;
	}
	theSoundFileData->mSoundClasses.RemoveAllItemsAfter( theFirstAddedClass );
		// note: since theFirstAddedClass is 0-based, it actually points to the
		// position just before the first added class in the array; thus passing
		// it as-is to this function results in all added classes being removed.
	
	// delete the added entries in the class list.
	theClassListBox->RemoveRows( theFirstAddedClass, mHowMany );
		
	// update the list.
	theClassListBox->Refresh();
}


#pragma mark -

// ---------------------------------------------------------------------------
//	¥ CRemoveClassesAction							Constructor	[public]
// ---------------------------------------------------------------------------

CRemoveClassesAction::CRemoveClassesAction(
	SInt32				inFirstIndex,
	SInt32				inHowMany,
	CWailDocWindow*		inWindow )
	: LAction( rSTRL_RedoClassActionsStringList,
			   (inHowMany > 1 ? actionString_RemoveClasses : actionString_RemoveClass),
			   /* already done? */ false ),
	  mFirstIndex( inFirstIndex ),
	  mHowMany( inHowMany ),
	  mWindow( inWindow )
{
	SignalIf_( mWindow == nil );
}


// ---------------------------------------------------------------------------
//	¥ ~CRemoveClassesAction							Destructor	[public]
// ---------------------------------------------------------------------------

CRemoveClassesAction::~CRemoveClassesAction()
{
	// if the action was not undoed, the sound classes don't belong anywhere anymore,
	// so we need to delete them.
	if (IsDone())
	{
		TArrayIterator<CWailSoundClass*> theIterator( mClasses );
		CWailSoundClass* theClass = NULL;
		while (theIterator.Next( theClass ))
			delete theClass;
	}
}


// ---------------------------------------------------------------------------------
//		¥ RedoSelf
// ---------------------------------------------------------------------------------
// gets called by Redo when the action needs to be Redone. also functions as DoSelf
// the first time the action is posted.
//
// we need to remove some sound classes from the given sound file.

void
CRemoveClassesAction::RedoSelf()
{
	// get our window's sound file data.
	CWailSoundFileData* theSoundFileData = mWindow->GetSoundFileData();
	ThrowIfNULL_( theSoundFileData );
	
	// get our window's class listbox.
	CSingleColumnTextListBox* theClassListBox =
		dynamic_cast<CSingleColumnTextListBox*>(mWindow->FindPaneByID( pane_ClassList ));
	ThrowIfNULL_( theClassListBox );
	
	// if the currently selected class is among those to remove or after that,
	// deselect class and clear class panes.
	SInt32 theCurrentClass = mWindow->GetCurrentClass();
	if (theCurrentClass >= mFirstIndex)
	{
		theClassListBox->UnselectAllCells();
		mWindow->SelectClass( -1 );
	}
	
	// saved class array should be empty here.
	SignalIfNot_( mClasses.GetCount() == 0 );
	
	// get the classes we need to remove and add them to our array.
	// we don't delete them right now because we want to be able to re-add them
	// later if the action is Undone.
	mClasses.AdjustAllocation( mHowMany );
	for (SInt32 i = mFirstIndex; i < (mFirstIndex + mHowMany); ++i)
	{
		CWailSoundClass* theSoundClass = NULL;
		ThrowIf_( !(theSoundFileData->mSoundClasses.FetchItemAt( i + 1, theSoundClass )) );
		mClasses.AddItem( theSoundClass );
	}
	// now remove the items from the sound file data.
	theSoundFileData->mSoundClasses.RemoveItemsAt( mHowMany, mFirstIndex + 1 );
	
	// delete the last entries of the class list. (since classes will all
	// go down one notch)
	theClassListBox->RemoveRows( theSoundFileData->mSoundClasses.GetCount(), mHowMany );
		// GetCount will effectively return X less classes that are currently
		// in the ListBox since we removed the classes from the array before...
		// however this is perfect since ListBoxes start numering rows at 0,
		// and this gives us precisely the position of the first item to remove :)
		
	// update the list.
	theClassListBox->Refresh();
	
	// removing a class makes our window dirty.
	mWindow->SetDirty( true );
}


// ---------------------------------------------------------------------------------
//		¥ UndoSelf
// ---------------------------------------------------------------------------------
// gets called by Undo when the action needs to be Undone.
//
// we need to add the sound classes back in the given sound file.

void
CRemoveClassesAction::UndoSelf()
{
	// get our window's sound file data.
	CWailSoundFileData* theSoundFileData = mWindow->GetSoundFileData();
	ThrowIfNULL_( theSoundFileData );
	
	// get our window's class listbox.
	CSingleColumnTextListBox* theClassListBox =
		dynamic_cast<CSingleColumnTextListBox*>(mWindow->FindPaneByID( pane_ClassList ));
	ThrowIfNULL_( theClassListBox );
	
	// if the currently selected class is after those we removed,
	// deselect class and clear class panes.
	SInt32 theCurrentClass = mWindow->GetCurrentClass();
	if (theCurrentClass >= mFirstIndex)
	{
		theClassListBox->UnselectAllCells();
		mWindow->SelectClass( -1 );
	}
	
	// saved class array should have the right number of classes here.
	SignalIfNot_( mClasses.GetCount() == mHowMany );
	
	{
		// disable list drawing while to speed things up.
		StDisableListDrawing theDrawingDisabler( *theClassListBox );
	
		// add the classes back into the sound file data.
		mClasses.AdjustAllocation( mHowMany );
		TArrayIterator<CWailSoundClass*> theSavedClassesIterator( mClasses );
		CWailSoundClass* theSoundClass = NULL;
		SInt32 theInsertionIndex = mFirstIndex + 1;	// since arrays are 1-based.
		while (theSavedClassesIterator.Next( theSoundClass ))
		{
			// add the class to the sound file data.
			theSoundFileData->mSoundClasses.InsertItemsAt( 1, theInsertionIndex, theSoundClass );
			++theInsertionIndex;
			
			// add a class name to the list (add it to the end, classes will go down one notch).
			LStr255 theRowString;
			UWailClassNames::GetClassName(
				theSoundFileData->mSoundClasses.GetCount(), theRowString );
			theClassListBox->AddRow( 0xffff, (ConstStringPtr) theRowString );
		}
		
		// now remove all classes from our saved list.
		mClasses.RemoveItemsAt( 0x7FFFFFFF, LArray::index_First );
	}
	
	// update the list.
	theClassListBox->Refresh();
}
