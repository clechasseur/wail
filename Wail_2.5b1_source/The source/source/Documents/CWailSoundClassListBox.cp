// =================================================================================
//	CWailSoundClassListBox.cp					©1999, Charles Lechasseur
// =================================================================================
//
// CWailSoundClassListBox is used for the ListBox containing the class names.
// we must override the default C1Col1SelTextListBox class, because we want to
// broadcast single clicks too.

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

#include "CWailSoundClassListBox.h"

#include "CWailDocWindow.h" // for message constant.

#include "WailCommands.h"


// ---------------------------------------------------------------------------
//	¥ CWailSoundClassListBox							Constructor	[public]
// ---------------------------------------------------------------------------

CWailSoundClassListBox::CWailSoundClassListBox()
{
}


// ---------------------------------------------------------------------------
//	¥ CWailSoundClassListBox		Constructor from input parameters [public]
// ---------------------------------------------------------------------------

CWailSoundClassListBox::CWailSoundClassListBox(
	const SPaneInfo		&inPaneInfo,
	Boolean				inHasHorizScroll,
	Boolean				inHasVertScroll,
	Boolean				inHasGrow,
	Boolean				inHasFocusBox,
	MessageT			inDoubleClickMessage,
	SInt16				inTextTraitsID,
	SInt16				inLDEFid,
	LCommander			*inSuper )
	: C1Col1SelTextListBox ( inPaneInfo, inHasHorizScroll, inHasVertScroll, inHasGrow,
		inHasFocusBox, inDoubleClickMessage, inTextTraitsID, inLDEFid, inSuper )
{
}


// ---------------------------------------------------------------------------
//	¥ CWailSoundClassListBox					Stream Constructor	[public]
// ---------------------------------------------------------------------------

CWailSoundClassListBox::CWailSoundClassListBox(
	LStream *inStream )
	: C1Col1SelTextListBox(inStream)
{
}


// ---------------------------------------------------------------------------
//	¥ CWailSoundClassListBox						Copy Constructor	[public]
// ---------------------------------------------------------------------------

CWailSoundClassListBox::CWailSoundClassListBox(
	const CWailSoundClassListBox& inOriginal )
	: C1Col1SelTextListBox(inOriginal)
{
}


// ---------------------------------------------------------------------------
//	¥ ~CWailSoundClassListBox							Destructor	[public]
// ---------------------------------------------------------------------------

CWailSoundClassListBox::~CWailSoundClassListBox()
{
}


// ---------------------------------------------------------------------------
//		¥ HandleKeyPress
// ---------------------------------------------------------------------------
// respond to key press
//
// we must watch for navigation keys. if the keyPress is handled by our parent's
// HandleKeyPress routine, we broadcast a single-click message (hey, it's pretty
// similar anyway :)

Boolean
CWailSoundClassListBox::HandleKeyPress(
	const EventRecord	&inKeyEvent)
{
	// call parent's routine.
	Boolean keyHandled = C1Col1SelTextListBox::HandleKeyPress(inKeyEvent);
	// check if handled. if yes, broadcast a message.
	if (keyHandled)
		BroadcastMessage(msg_ClassListSingleClick, this);
		
	return keyHandled;
}


// ---------------------------------------------------------------------------
//		¥ ClickSelf
// ---------------------------------------------------------------------------
//	Respond to Click inside the ListBox
//
// we must override ClickSelf because we don't want to allow the user to select
// more than one row. this is not possible with the standard LListBox class.
// however, we can simulate this by stripping the EventRecord of all its modifiers.
//
// in addition, we want to broadcast single-click messages too for this pane.
//
// this source has been copied from LListBox::ClickSelf and modified.
// the original source is ©1996-1998, Metrowerks

void
CWailSoundClassListBox::ClickSelf(
	const SMouseDownEvent	&inMouseDown)
{
	if (SwitchTarget(this)) {
		FocusDraw();
		
		if (::LClick(inMouseDown.whereLocal, (SInt16) 0, // we're stripping all modifiers.
					mMacListH)) {
					
			BroadcastMessage(mDoubleClickMessage, this);
		}
		else {
		
			BroadcastMessage(msg_ClassListSingleClick, this);
		}
	}
}