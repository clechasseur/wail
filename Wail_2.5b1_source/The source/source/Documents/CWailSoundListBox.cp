// =================================================================================
//	CWailSoundListBox.cp					©1999, Charles Lechasseur
// =================================================================================
//
// List box to handle sound lists in Wail.

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

#include "CWailSoundListBox.h"


// static variables
TArray<CWailSoundListBox*>	CWailSoundListBox::sListBoxes;


// ---------------------------------------------------------------------------
//	¥ CWailSoundListBox					Stream Constructor	[public]
// ---------------------------------------------------------------------------

CWailSoundListBox::CWailSoundListBox(LStream *inStream)
	: CBroadcastingListBox(inStream),
	  LDragAndDrop( UQDGlobals::GetCurrentPort(), this )
{
	// read drag messages.
	*inStream >> mDragSendMessage;
	*inStream >> mDragReceiveMessage;
	
	// are we 8-bit?
	*inStream >> mIs8bit;
	
	// we're not dragging right now.
	mIsDragging = false;

	// add ourself to the list of objects.
	sListBoxes.AddItem( this );
}


// ---------------------------------------------------------------------------
//	¥ ~CWailSoundListBox							Destructor	[public]
// ---------------------------------------------------------------------------

CWailSoundListBox::~CWailSoundListBox()
{
	// remove ourself from the list of objects.
	sListBoxes.Remove( this );
}


// ---------------------------------------------------------------------------
//		¥ SelectOneCell
// ---------------------------------------------------------------------------
//	we override SelectOneCell. when a sound is selected, all sounds of other
// 	list boxes must be deselected. this is done by iterating the static array
//	of list boxes of this type.
//	we don't need to override SelectAllCells since this is a single-selection list.

void
CWailSoundListBox::SelectOneCell(
	Cell	inCell)
{
	// first, deselect all other sounds.
	UnselectOthersCells();
			
	// next, call our parent's routine to handle selection.
	CBroadcastingListBox::SelectOneCell( inCell );
}


// ---------------------------------------------------------------------------
//		¥ EnterDropArea
// ---------------------------------------------------------------------------
//	hiliting the drop area when receiving a drag.
//	only hilite it if we're not receiving one of our own drags.

void
CWailSoundListBox::EnterDropArea(
	DragReference	inDragRef,
	Boolean			inDragHasLeftSender)
{
	if (!mIsDragging)
		LDragAndDrop::EnterDropArea( inDragRef, inDragHasLeftSender );
}


// ---------------------------------------------------------------------------
//		¥ DragIsAcceptable
// ---------------------------------------------------------------------------
//	checks if a drag is acceptable by this pane.

Boolean
CWailSoundListBox::DragIsAcceptable(
	DragReference	inDragRef)
{
	Boolean isAcceptable = false;

	// we don't accept drags if we're full, if it's one of our own drags or if we don't have
	// a drag message to send.
	if ((((**mMacListH).dataBounds.bottom - (**mMacListH).dataBounds.top) < 5) &&
		(!mIsDragging) && (mDragReceiveMessage != msg_Nothing))
	{
		// everything OK, call parent's routine to check for drag item flavors.
		isAcceptable = LDragAndDrop::DragIsAcceptable( inDragRef );
	}
	
	return isAcceptable;
}


// ---------------------------------------------------------------------------
//		¥ ItemIsAcceptable
// ---------------------------------------------------------------------------
//	checks if an item dropped on the list is acceptable.

Boolean
CWailSoundListBox::ItemIsAcceptable(
	DragReference	inDragRef,
	ItemReference	inItemRef )
{
	// non-flavor-related conditions are checked in DragIsAcceptable; here we only
	// check if the item's data itself is OK.

	// we only accept sounds.
	FlavorFlags theFlags;
	Boolean hasASound = (::GetFlavorFlags( inDragRef, inItemRef, 'snd ', &theFlags ) == noErr);
	
	return hasASound;
}


// ---------------------------------------------------------------------------------
//		¥ DoDragReceive
// ---------------------------------------------------------------------------------
//	called when a drag is received. overridden because the drop area needs to
//	be unhilited after the drag is received completely. (is this a bug in LDropArea?)

void
CWailSoundListBox::DoDragReceive(
	DragReference	inDragRef)
{
	// call parent's routine.
	LDragAndDrop::DoDragReceive( inDragRef );
	
	// refresh to unhilite drop area.
	Refresh();
}


// ---------------------------------------------------------------------------
//		¥ ReceiveDragItem
// ---------------------------------------------------------------------------
//	receives an item dragged on the list.

void
CWailSoundListBox::ReceiveDragItem(
	DragReference	inDragRef,
	DragAttributes	inDragAttrs,
	ItemReference	inItemRef,
	Rect&			inItemBounds)
{
#pragma unused( inDragAttrs, inItemBounds )

	// shouldn't be receiving drags if we don't have a drag message.
	SignalIf_( mDragReceiveMessage == msg_Nothing );

	// get size of sound.
	Size theSoundSize;
	ThrowIfOSErr_( ::GetFlavorDataSize( inDragRef, inItemRef, 'snd ', &theSoundSize ) );
	
	// allocate storage to hold the sound.
	StHandleBlock theHandleBlock( theSoundSize );
	
	{
		// lock the handle.
		StHandleLocker theHandleLocker( theHandleBlock );
		// turn it into a buffer.
		Ptr theBuffer = *((Handle) theHandleBlock);
		
		// get sound out of drag reference.
		ThrowIfOSErr_( ::GetFlavorData( inDragRef, inItemRef, 'snd ',
										theBuffer, &theSoundSize, 0 ) );
	}
	
	// broadcast message to have sound imported.
	Handle theHandle = (Handle) theHandleBlock;
	Handle* theHandlePointer = &theHandle;
	BroadcastMessage( mDragReceiveMessage, (void*) theHandlePointer );
}


// ---------------------------------------------------------------------------------
//		¥ UnselectOthersCells
// ---------------------------------------------------------------------------------
//	unselects all the cells of all list boxes of this type, except this one, of course.

void
CWailSoundListBox::UnselectOthersCells()
{
	TArrayIterator<CWailSoundListBox*> theIterator( sListBoxes );
	CWailSoundListBox* theListBox;
	while ( theIterator.Next( theListBox ) )
		if (theListBox != this)	// deselect others, not us
			theListBox->UnselectAllCells();
}


// ---------------------------------------------------------------------------
//		¥ Click
// ---------------------------------------------------------------------------
//	responds to a click inside the list box. since we support drag'n'drop, we
//	must allow delay select clicks thru. drag'n'drop will be handled in ClickSelf.

void
CWailSoundListBox::Click(
	SMouseDownEvent	&inMouseDown)
{
	// strip delaySelect in order to call parent's routine.
	StValueChanger<Boolean> changeDelaySelect( inMouseDown.delaySelect, false );
	CBroadcastingListBox::Click( inMouseDown );
}


// ---------------------------------------------------------------------------
//		¥ ClickSelf
// ---------------------------------------------------------------------------
//	respond to a click inside the list box. since LListBox::ClickSelf doesn't
//	call SelectOneCell directly, we need to manually unselect other's cell apres-coup.
//	(this is legal since it's a one-selection list.)

void
CWailSoundListBox::ClickSelf(
	const SMouseDownEvent	&inMouseDown)
{
	// is this a drag (and we have a drag message)?
	if ((mDragSendMessage != msg_Nothing) &&
		::WaitMouseMoved( inMouseDown.macEvent.where ) &&
		LDragAndDrop::DragAndDropIsPresent())
	{
		// yes, it is.
		
		// to-do: process click for selection manually?
	
		// broadcast a message to start the drag if user selected a cell.
		if (GetValue() != -1)	// GetValue() returns index of current cell.
		{
			// set dragging flag so we don't receive our own drags.
			StValueChanger<Boolean> changeIsDragging( mIsDragging, true );
			
			// broadcast our drag send message.
			BroadcastMessage( mDragSendMessage, (void*) &inMouseDown );
				// include the event that started the drag. receiver must not dispose of it.
		}
	}
	else	// this is not a drag.
	{
		// call parent's routine to have the click processed and the message
		// correctly broadcast.
		CBroadcastingListBox::ClickSelf( inMouseDown );
		
		// now, unselect all other's cells.
		if (GetValue() != -1)	// only do this if the user hit a cell. (GetValue is current cell)
			UnselectOthersCells();
	}
}
