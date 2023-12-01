// =================================================================================
//	CWailWindowChooser.cp					©1999, Charles Lechasseur
// =================================================================================
//
// class to help pick an open window from a list of all open windows.
// create an instance of this class and call ChooseWindow.
// if ChooseWindow returns nil, user cancelled.

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

#include "CWailWindowChooser.h"

#include "CWailDocWindow.h"

#include "CWindowTracker.h"
#include "CWailWindowChooserDialog.h"

#include <LTextColumn.h>
#include <UModalDialogs.h>


// constants

const ResIDT	rPPob_ChooseWindowDialog	= 1007;


// ---------------------------------------------------------------------------
//	¥ CWailWindowChooser
// ---------------------------------------------------------------------------

CWailWindowChooser::CWailWindowChooser(
	LCommander*	inSuper )
	: LCommander( inSuper )
{
}


// ---------------------------------------------------------------------------
//	¥ ~CWailWindowChooser
// ---------------------------------------------------------------------------

CWailWindowChooser::~CWailWindowChooser()
{
}


// ---------------------------------------------------------------------------------
//		¥ HasMoreThanOneWindow								[static]
// ---------------------------------------------------------------------------------
// returns true if there's more that one window open.

Boolean
CWailWindowChooser::HasMoreThanOneWindow()
{
	// get number of windows.
	UInt32 theCount = 0;
	CWindowTracker* theTracker = CWindowTracker::GetCurrentWindowTracker();
	if (theTracker != nil)
		theCount = theTracker->GetNumberOfWindows();
	
	// return true if we have more than one.
	return (theCount > 1);
}


// ---------------------------------------------------------------------------------
//		¥ ChooseWindow
// ---------------------------------------------------------------------------------
// presents a list of all open windows and asks the user to choose one.
// returns a pointer to the chosen window or nil if user cancels.
//
// pass a pointer to a window if you want that window excluded from the list.

CWailDocWindow*
CWailWindowChooser::ChooseWindow(
	CWailDocWindow*	inExceptThisOne )
{
	// get current window tracker. if we don't have one, quit here.
	CWindowTracker* theTracker = CWindowTracker::GetCurrentWindowTracker();
	if (theTracker == nil)
		return nil;
	
	// get list of open windows.
	TArray<LWindow*> theWindowList;
	theTracker->GetWindowList( theWindowList );
	
	// remove the requested window, if any.
	if (inExceptThisOne != nil)
	{
		theWindowList.Remove( inExceptThisOne );
		
		// if this results in an empty list, quit here.
		if (theWindowList.GetCount() < 1)
			return nil;
	}
	
	// sort it in alphabetical order.
	SortWindowArrayAlphabetically( theWindowList );
	
	// create dialog.
	StDialogHandler theHandler( rPPob_ChooseWindowDialog, this );
	
	// get dialog pointer.
	LWindow* theDialog = theHandler.GetDialog();
	ThrowIfNil_( theDialog );
	
	// get window list pane.
	LTextColumn* theWindowListPane = dynamic_cast<LTextColumn*>
										(theDialog->FindPaneByID( pane_ChooserWindowList ));
	ThrowIfNil_( theWindowListPane );
	
	// link it to the handler. (not done automatically.)
	theWindowListPane->AddListener( &theHandler );
	
	// populate the list.
	theWindowListPane->InsertRows( theWindowList.GetCount(), 0 );
	TArrayIterator<LWindow*> theIterator( theWindowList );
	STableCell theCell( 0, 0 );
	LWindow* theWindow = nil;
	while (theIterator.Next( theWindow ) && theWindowListPane->GetNextCell( theCell ))
	{
		Str255 theWindowTitle;
		theWindow->GetDescriptor( theWindowTitle );
	
		theWindowListPane->SetCellData( theCell, &theWindowTitle[1], theWindowTitle[0] );
	}
	
	// show the dialog and wait for ok or cancel.
	theDialog->Show();
	
	MessageT theMessage;
	do
	{
		theMessage = theHandler.DoDialog();
	} while ((theMessage != msg_OK) && (theMessage != msg_Cancel));
	
	// assume user cancelled and we should return nil.
	LWindow* theReturnedWindow = nil;
	
	// if user selected ok, check selection in the list.
	if (theMessage == msg_OK)
	{
		STableCell theSelectedCell = theWindowListPane->GetFirstSelectedCell();
		TableIndexT theSelectedIndex;
		theWindowListPane->CellToIndex( theSelectedCell, theSelectedIndex );
		
		// return a pointer to that window, if any.
		if (theSelectedIndex > 0)
			theWindowList.FetchItemAt( theSelectedIndex, theReturnedWindow );
	}
	
	// return the result.
	return dynamic_cast<CWailDocWindow*> (theReturnedWindow);
}


// ---------------------------------------------------------------------------------
//		¥ SortWindowArrayAlphabetically					[static]
// ---------------------------------------------------------------------------------
// utility function for sorting a list of LWindow* objects (pointers)
// according to the window's titles.

void
CWailWindowChooser::SortWindowArrayAlphabetically(
	LArray&		ioArray )
{
	// give array a new comparator.
	ioArray.SetComparator( new CWindowTitleComparator );
		// the array assumes ownership of this comparator.
	
	// sort array.
	ioArray.Sort();
}


// =================================================================================
//	CWindowTitleComparator
// =================================================================================
// a comparator for sorting lists of LWindow* objects according to the window titles.

#pragma mark -


// ---------------------------------------------------------------------------------
//		¥ CWindowTitleComparator
// ---------------------------------------------------------------------------------

CWindowTitleComparator::CWindowTitleComparator()
{
}


// ---------------------------------------------------------------------------------
//		¥ ~~CWindowTitleComparator
// ---------------------------------------------------------------------------------

CWindowTitleComparator::~CWindowTitleComparator()
{
}


// ---------------------------------------------------------------------------------
//		¥ Compare
// ---------------------------------------------------------------------------------
// compares two windows according to their titles.

SInt32
CWindowTitleComparator::Compare(
	const void*		inItemOne,
	const void*		inItemTwo,
	UInt32			inSizeOne,
	UInt32			inSizeTwo ) const
{
	// sanity check
	SignalIfNot_( inSizeOne == sizeof(LWindow*) );
	SignalIfNot_( inSizeTwo == sizeof(LWindow*) );
	
	// get titles
	Str255 theTitleOne, theTitleTwo;
	(*((LWindow**) inItemOne))->GetDescriptor( theTitleOne );
	(*((LWindow**) inItemTwo))->GetDescriptor( theTitleTwo );
	
	// checking
	return LString::CompareBytes( theTitleOne + 1, theTitleTwo + 1,
								  theTitleOne[0], theTitleTwo[0] );
}
					
					
// ---------------------------------------------------------------------------------
//		¥ CompareToKey
// ---------------------------------------------------------------------------------
// compares a window with a "key" that must be a StringPtr.
// compares that string with the window's title.

SInt32
CWindowTitleComparator::CompareToKey(
	const void*		inItem,
	UInt32			inSize,
	const void*		inKey ) const
{
	// sanity check
	SignalIfNot_( inSize == sizeof(LWindow*) );
	
	// get title
	Str255 theTitle;
	(*((LWindow**) inItem))->GetDescriptor( theTitle );
	
	// checking
	return LString::CompareBytes( theTitle + 1, ((StringPtr) inKey) + 1,
								  theTitle[0], ((StringPtr) inKey)[0] );
}


// ---------------------------------------------------------------------------------
//		¥ Clone
// ---------------------------------------------------------------------------------
// returns a copy of this comparator.

LComparator*
CWindowTitleComparator::Clone()
{
	return new CWindowTitleComparator;
}