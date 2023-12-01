// =================================================================================
//	CWailSoundListBox.h					©1999, Charles Lechasseur
// =================================================================================

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


#pragma once

#include "CBroadcastingListBox.h"
#include <LDragAndDrop.h>


class CWailSoundListBox: public CBroadcastingListBox,
						 public LDragAndDrop
{
	public:
	// Public Enums and Constants
		enum { class_ID = 'WSLl' };
	
	// Public Functions

		// Stream Constructor 
							CWailSoundListBox( LStream *inStream );
		//Destructor
		virtual				~CWailSoundListBox();
		
		// selection
		virtual void		SelectOneCell(
								Cell				inCell);
								
		// drag'n'drop support
		virtual void		EnterDropArea(
								DragReference		inDragRef,
								Boolean				inDragHasLeftSender);
		
		virtual Boolean		DragIsAcceptable(
								DragReference		inDragRef);
		virtual Boolean		ItemIsAcceptable(
								DragReference		inDragRef,
								ItemReference		inItemRef);
								
		virtual void		DoDragReceive(
								DragReference		inDragRef);
		virtual void		ReceiveDragItem(
								DragReference		inDragRef,
								DragAttributes		inDragAttrs,
								ItemReference		inItemRef,
								Rect				&inItemBounds);
								
	protected:
	// Protected member functions
	
		// selection handling across list boxes
		virtual void		UnselectOthersCells();
	
		// click handling
		virtual void		Click(
								SMouseDownEvent 	&inMouseDown);
		virtual void		ClickSelf(
								const SMouseDownEvent	&inMouseDown);
		
	private:
	// Member Variables
	
	MessageT				mDragSendMessage;		// message to broadcast in order to start a drag.
	MessageT				mDragReceiveMessage;	// message to broadcast in order to receive a drag.
	Boolean					mIsDragging;			// are we dragging?
	
	Boolean					mIs8bit;				// are we 8-bit?
	
	// static variables
	
	static TArray<CWailSoundListBox*>		sListBoxes;	// list of all boxes of this type.
	
	// Private Functions
		// Defensive programming. No  operator=
		CWailSoundListBox&			operator=(const CWailSoundListBox&);
		// Defensive programming. No copy Constructor
							CWailSoundListBox(const CWailSoundListBox& inCWailSoundListBox);
};
