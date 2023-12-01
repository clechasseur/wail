// =================================================================================
//	CWailProgressDialog.cp					©1999, Charles Lechasseur
// =================================================================================
//
// CWailProgressDialog is designed to handle simple progress dialogs for short tasks.
// cosntructor builds and shows the dialog. then, call Increment to move the progress
// bar. you can Reset it too, if you wish. destructor will take care of removing the
// dialog.

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

#include "CWailProgressDialog.h"

// constants for the progress dialog:

const ResIDT	PPob_WailProgressDialog			= 2000;
const PaneIDT	pane_ProgressDlogProgressBar	= 'PBar';

const ResIDT	STRL_WailProgressStrings		= 2000;


// ---------------------------------------------------------------------------
//	¥ CWailProgressDialog
// ---------------------------------------------------------------------------

CWailProgressDialog::CWailProgressDialog(
	SInt32		inProgressBarSize,
	SInt32		inWindowTitleIndex,
	LCommander*	inSuper)
{
	// get a new window.
	mWindow = LWindow::CreateWindow( PPob_WailProgressDialog, inSuper );
	
	// set its title.
	mWindow->SetDescriptor( LStr255( STRL_WailProgressStrings, inWindowTitleIndex ) );
	
	// get the progress bar.
	mProgressBar = (LControl*) mWindow->FindPaneByID( pane_ProgressDlogProgressBar );
	
	// set its min and max length.
	mProgressBar->SetMinValue( 0 );
	SetProgressBarSize( inProgressBarSize );
	
	// reset it.
	Reset();
	
	// show the window.
	mWindow->Show();
	// force redraw.
	mWindow->Activate();
	mWindow->Refresh();
}


// ---------------------------------------------------------------------------
//	¥ ~CWailProgressDialog
// ---------------------------------------------------------------------------

CWailProgressDialog::~CWailProgressDialog()
{
	// hide window.
	mWindow->Hide();

	// delete the window.
	delete mWindow;
}


// ---------------------------------------------------------------------------------
//		¥ GetProgressBarSize
// ---------------------------------------------------------------------------------
// returns the current maximum size of the progress bar.

SInt32
CWailProgressDialog::GetProgressBarSize() const
{
	return mProgressBar->GetMaxValue();
}


// ---------------------------------------------------------------------------------
//		¥ SetProgressBarSize
// ---------------------------------------------------------------------------------
// sets the maximum size of the progress bar.

void
CWailProgressDialog::SetProgressBarSize(
	SInt32	inProgressBarSize )
{
	mProgressBar->SetMaxValue( inProgressBarSize );
}


// ---------------------------------------------------------------------------------
//		¥ Increment
// ---------------------------------------------------------------------------------
// increments the progress bar by the given value. default is 1.

void
CWailProgressDialog::Increment(
	SInt32 	inHowMuch )
{
	mProgressBar->IncrementValue( inHowMuch );
}


// ---------------------------------------------------------------------------------
//		¥ Reset
// ---------------------------------------------------------------------------------
// resets the progress bar.

void
CWailProgressDialog::Reset()
{
	mProgressBar->SetValue( 0 );
}


// ---------------------------------------------------------------------------
//		¥ Show
// ---------------------------------------------------------------------------
// shows the progress dialog.

void
CWailProgressDialog::Show()
{
	mWindow->Show();
}


// ---------------------------------------------------------------------------
//		¥ Hide
// ---------------------------------------------------------------------------
// hides the progress dialog.

void
CWailProgressDialog::Hide()
{
	mWindow->Hide();
}
