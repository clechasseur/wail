// =================================================================================
//	CWailWindowChooserDialog.cp					©2000, Charles Lechasseur
// =================================================================================
//
// dialog for choosing a Wail document window. used by CWailWindowChooser.
// this class is responsible for handling routine chores in the dialog.

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

#include "CWailWindowChooserDialog.h"

#include <LTextColumn.h>


// ---------------------------------------------------------------------------
//	¥ CWailWindowChooserDialog					Stream Constructor	[public]
// ---------------------------------------------------------------------------

CWailWindowChooserDialog::CWailWindowChooserDialog(
	LStream *inStream )
	: LDialogBox( inStream ),
	  mWindowList( NULL ),
	  mOKButton( NULL )
{
}


// ---------------------------------------------------------------------------
//	¥ ~CWailWindowChooserDialog							Destructor	[public]
// ---------------------------------------------------------------------------

CWailWindowChooserDialog::~CWailWindowChooserDialog()
{
}


// ---------------------------------------------------------------------------
//		¥ FinishCreateSelf
// ---------------------------------------------------------------------------

void
CWailWindowChooserDialog::FinishCreateSelf()
{
	// call inherited.
	LDialogBox::FinishCreateSelf();
	
	// get pointers to panes.
	mWindowList = dynamic_cast<LTextColumn*> (FindPaneByID( pane_ChooserWindowList ));
	ThrowIfNil_( mWindowList );
	
	mOKButton = dynamic_cast<LControl*> (FindPaneByID( pane_ChooserOKButton ));
	ThrowIfNil_( mOKButton );
	
	// listen to our controls.
	mWindowList->AddListener( this );
	mOKButton->AddListener( this );
	
	// disable OK button until user selects a window.
	mOKButton->Disable();
}


// ---------------------------------------------------------------------------
//		¥ ListenToMessage
// ---------------------------------------------------------------------------

void
CWailWindowChooserDialog::ListenToMessage(
	MessageT		inMessage,
	void*			ioParam )
{
	switch (inMessage)
	{
		case msg_ChooserWindowSelected:
			// selection changed in the window list.
			// change enabled state of OK button to match.
			{
				STableCell theCell = mWindowList->GetFirstSelectedCell();
				if ((theCell.row == 0) && (theCell.col == 0))	// 0,0 = no selection
					mOKButton->Disable();
				else
					mOKButton->Enable();
			}
			break;
			
		default:
			// call inherited.
			LDialogBox::ListenToMessage( inMessage, ioParam );
			break;
	}
}