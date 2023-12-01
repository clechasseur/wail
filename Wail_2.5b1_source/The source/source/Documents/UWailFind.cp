// =================================================================================
//	UWailFind.cp					©2002, Charles Lechasseur
// =================================================================================
//
// Utility functions and classes related to Find functions in Wail.

/* Copyright (c) 2002, Charles Lechasseur
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

#include "UWailFind.h"

#include "UWailPreferences.h"


// =================================================================================
//		StFindClassNameDialogHandler
// =================================================================================
// StDialogHandler subclass to use to display the "Find class by name" dialog.
// this class handles the prefs checkboxes in the dialog.


// constants for the "Find class by name" dialog

const	ResIDT		rPPob_FindClassByNameDialog		= 1004;

const	PaneIDT		pane_WhatClassNameEditField		= 'Name';
const	PaneIDT		pane_IgnoreCaseCheckbox			= '!Cas';
const	PaneIDT		pane_ExactMatchCheckbox			= '=Exa';

const	MessageT	msg_IgnoreCaseChanged			= '!Cas';
const	MessageT	msg_ExactMatchChanged			= '=Exa';


// ---------------------------------------------------------------------------
//		¥ StFindClassNameDialogHandler
// ---------------------------------------------------------------------------

StFindClassNameDialogHandler::StFindClassNameDialogHandler(
	LCommander*		inSuper )
	: StDialogHandler( rPPob_FindClassByNameDialog, inSuper ),
	  mClassNameEditField( nil ),
	  mIgnoreCaseControl( nil ),
	  mExactMatchControl( nil ),
	  mPrefsDirty( false )
{
	// try to find our controls.
	LWindow* ourDialog = GetDialog();
	ThrowIfNULL_( ourDialog );
	
	mClassNameEditField = dynamic_cast<LEditField*>(
							ourDialog->FindPaneByID( pane_WhatClassNameEditField ));
	ThrowIfNil_( mClassNameEditField );
	
	mIgnoreCaseControl = dynamic_cast<LControl*>(
							ourDialog->FindPaneByID( pane_IgnoreCaseCheckbox ));
	ThrowIfNil_( mIgnoreCaseControl );
	
	mExactMatchControl = dynamic_cast<LControl*>(
							ourDialog->FindPaneByID( pane_ExactMatchCheckbox ));
	ThrowIfNil_( mExactMatchControl );
	
	// set control's initial values according to prefs.
	if (UWailPreferences::IgnoreCaseInNameFind())
		mIgnoreCaseControl->SetValue( Button_On );
	if (UWailPreferences::ExactMatchInNameFind())
		mExactMatchControl->SetValue( Button_On );
		
	// reset "dirtyness" of prefs here (it might have been modified by the SetValues above).
	mPrefsDirty = false;
		
	// set dialog's focus to the class name edit field.
	ourDialog->SetLatentSub( mClassNameEditField );
	
	// show our dialog.
	ourDialog->Show();
}


// ---------------------------------------------------------------------------
//		¥ ListenToMessage
// ---------------------------------------------------------------------------
// catches messages by controls we handle ourselves and passes unhandled
// messages back to inherited function for storing.

void
StFindClassNameDialogHandler::ListenToMessage(
	MessageT		inMessage,
	void*			ioParam )
{
	switch (inMessage)
	{
		case msg_IgnoreCaseChanged:
			UWailPreferences::SetIgnoreCaseInNameFind(
				mIgnoreCaseControl->GetValue() == Button_On );
			mPrefsDirty = true;	// will need to write prefs once dialog is done.
			break;
			
		case msg_ExactMatchChanged:
			UWailPreferences::SetExactMatchInNameFind(
				mExactMatchControl->GetValue() == Button_On );
			mPrefsDirty = true;	// will need to write prefs once dialog is done.
			break;
			
		case msg_OK:
		case msg_Cancel:
			// dialog is now done. if needed, save prefs.
			if (mPrefsDirty)
				UWailPreferences::Write();
			// call the inherited version to store message.
			StDialogHandler::ListenToMessage( inMessage, ioParam );
			break;
			
		default:
			// not a message we handle, call inherited to store it.
			StDialogHandler::ListenToMessage( inMessage, ioParam );
			break;
	}
}


// ---------------------------------------------------------------------------
//		¥ GetClassName
// ---------------------------------------------------------------------------
// returns the class name entered in the dialog. can be used once the dialog
// is done and the user pressed OK to know which class to look for.

void
StFindClassNameDialogHandler::GetClassName(
	StringPtr		outClassName ) const
{
	mClassNameEditField->GetDescriptor( outClassName );
}
