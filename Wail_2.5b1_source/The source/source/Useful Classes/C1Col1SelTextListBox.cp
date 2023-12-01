// =================================================================================
//	C1Col1SelTextListBox.cp					©1999, Charles Lechasseur
// =================================================================================
//
// C1Col1SelTextListBox is derived from CSingleColumnTextListBox, but it doesn't
// allow the selection of multiple cells. it has no new functions, so you can simply
// base your ListBox on it and get all the functionalities.

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

#include "C1Col1SelTextListBox.h"


// ---------------------------------------------------------------------------
//	¥ C1Col1SelTextListBox							Constructor	[public]
// ---------------------------------------------------------------------------

C1Col1SelTextListBox::C1Col1SelTextListBox()
{
}


// ---------------------------------------------------------------------------
//	¥ C1Col1SelTextListBox			Constructor from input parameters [public]
// ---------------------------------------------------------------------------

C1Col1SelTextListBox::C1Col1SelTextListBox(
	const SPaneInfo		&inPaneInfo,
	Boolean				inHasHorizScroll,
	Boolean				inHasVertScroll,
	Boolean				inHasGrow,
	Boolean				inHasFocusBox,
	MessageT			inDoubleClickMessage,
	SInt16				inTextTraitsID,
	SInt16				inLDEFid,
	LCommander			*inSuper )
	: CSingleColumnTextListBox ( inPaneInfo, inHasHorizScroll, inHasVertScroll, inHasGrow,
		inHasFocusBox, inDoubleClickMessage, inTextTraitsID, inLDEFid, inSuper )
{
}


// ---------------------------------------------------------------------------
//	¥ C1Col1SelTextListBox					Stream Constructor	[public]
// ---------------------------------------------------------------------------

C1Col1SelTextListBox::C1Col1SelTextListBox(
	LStream *inStream)
	: CSingleColumnTextListBox(inStream)
{
}


// ---------------------------------------------------------------------------
//	¥ C1Col1SelTextListBox						Copy Constructor	[public]
// ---------------------------------------------------------------------------

C1Col1SelTextListBox::C1Col1SelTextListBox(
	const C1Col1SelTextListBox& inOriginal)
	: CSingleColumnTextListBox(inOriginal)
{
}


// ---------------------------------------------------------------------------
//	¥ ~C1Col1SelTextListBox							Destructor	[public]
// ---------------------------------------------------------------------------

C1Col1SelTextListBox::~C1Col1SelTextListBox()
{
}


// ---------------------------------------------------------------------------
//		¥ FindCommandStatus
// ---------------------------------------------------------------------------
//	Pass back the status of a Command

void
C1Col1SelTextListBox::FindCommandStatus(
	CommandT	inCommand,
	Boolean		&outEnabled,
	Boolean		&outUsesMark,
	UInt16		&outMark,
	Str255		outName)
{
	switch (inCommand) {
		
		case cmd_SelectAll:	
			// we don't allow multiple selections.
			outEnabled = false;
			break;
			
		default:
			CSingleColumnTextListBox::FindCommandStatus(inCommand, outEnabled,
													outUsesMark, outMark, outName);
			break;
	}
}


// ---------------------------------------------------------------------------
//		¥ HandleKeyPress
// ---------------------------------------------------------------------------
// we must override this function, because the user might navigate the list
// while holding modifier keys, resulting in multiple selections. we can correct
// this by copying the event record, stripping it from its modifier keys,
// and passing it back to our parent's HandleKeyPress routine.
//
// note: this will, however, prevent type selection from working correctly...
// but since it is not yet implemented in Pro3, it's not a big deal :)

Boolean
C1Col1SelTextListBox::HandleKeyPress(
	const EventRecord	&inKeyEvent)
{
	// make a copy of the event and strip its modifiers.
	EventRecord theEvent = inKeyEvent;
	theEvent.modifiers = 0;
	// pass it back to our parent's routine.
	return CSingleColumnTextListBox::HandleKeyPress(theEvent);
}


// ---------------------------------------------------------------------------
//		¥ ClickSelf
// ---------------------------------------------------------------------------
//	Respond to Click inside an ListBox
//
// here we want to prevent the user from selecting multiple cells by holding down
// modifier keys. we can do this by clearing the modifier parameter from the event record,
// and pass the event to our parent's ClickSelf routine.

void
C1Col1SelTextListBox::ClickSelf(
	const SMouseDownEvent	&inMouseDown)
{
	// create a duplicate of the event, without the modifiers.
	SMouseDownEvent theEvent = inMouseDown;
	theEvent.macEvent.modifiers = 0;
	// pass it to parent's routine.
	CSingleColumnTextListBox::ClickSelf(theEvent);
}