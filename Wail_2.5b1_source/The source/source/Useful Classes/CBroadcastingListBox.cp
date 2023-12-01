// =================================================================================
//	CBroadcastingListBox.cp					©1999, Charles Lechasseur
// =================================================================================
//
// a CBroadcastingListBox is a C1Col1SelTextListBox that will broadcast a
// message when a particular key is pressed while it is the target.
// messages are defined in Constructor and can be linked to various keys.
// (except keys already handled by a standard ListBox, such as arrow keys.)
// it will include a pointer to itself in the ioParam of the message.
//
// Version: 1.0
//
// supported keys: Backspace, Delete, Enter, Return, Space Bar
//
// Version History:
//
// 1.0 - initial release.

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

#include "CBroadcastingListBox.h"


// constants

const UInt16	charCode_SpaceBar	= (UInt16) ' ';


// ---------------------------------------------------------------------------
//	¥ CBroadcastingListBox							Constructor	[public]
// ---------------------------------------------------------------------------

CBroadcastingListBox::CBroadcastingListBox()
{
	mBackspaceMessage = 
	mDeleteMessage = 
	mEnterMessage = 
	mReturnMessage = 
	mSpaceBarMessage = msg_Nothing;
}


// ---------------------------------------------------------------------------
//	¥ CBroadcastingListBox					Stream Constructor	[public]
// ---------------------------------------------------------------------------

CBroadcastingListBox::CBroadcastingListBox(LStream *inStream)
	: C1Col1SelTextListBox(inStream)
{
	MessageT	theMessages[5];
	inStream->ReadData( theMessages, sizeof(MessageT) * 5 );
	
	mBackspaceMessage = theMessages[0];
	mDeleteMessage = theMessages[1];
	mEnterMessage = theMessages[2];
	mReturnMessage = theMessages[3];
	mSpaceBarMessage = theMessages[4];
}


// ---------------------------------------------------------------------------
//	¥ CBroadcastingListBox						Copy Constructor	[public]
// ---------------------------------------------------------------------------

CBroadcastingListBox::CBroadcastingListBox(const CBroadcastingListBox& inOriginal)
	: C1Col1SelTextListBox(inOriginal)
{
	mBackspaceMessage = inOriginal.mBackspaceMessage;
	mDeleteMessage = inOriginal.mDeleteMessage;
	mEnterMessage = inOriginal.mEnterMessage;
	mReturnMessage = inOriginal.mReturnMessage;
	mSpaceBarMessage = inOriginal.mSpaceBarMessage;
}


// ---------------------------------------------------------------------------
//	¥ ~CBroadcastingListBox							Destructor	[public]
// ---------------------------------------------------------------------------

CBroadcastingListBox::~CBroadcastingListBox()
{
}


// ---------------------------------------------------------------------------
//		¥ HandleKeyPress
// ---------------------------------------------------------------------------
// respond to key press
//
// if a supported key is pressed, we must send the appropriate message.

Boolean
CBroadcastingListBox::HandleKeyPress(
	const EventRecord	&inKeyEvent)
{
	// call parent's routine.
	Boolean keyHandled = C1Col1SelTextListBox::HandleKeyPress(inKeyEvent);
	// check if handled.
	if (!keyHandled) // delete and backspace are not handled by ListBoxes.
	{
		// check if the key is delete or backspace.
		UInt16 theKey = (UInt16) (inKeyEvent.message & charCodeMask);
		keyHandled = true; // except if a key is not supported.
		switch (theKey)
		{
			case kBackspaceCharCode:
				if (mBackspaceMessage != msg_Nothing)
					BroadcastMessage( mBackspaceMessage, this );
				break;
				
			case kDeleteCharCode:
				if (mDeleteMessage != msg_Nothing)
					BroadcastMessage( mDeleteMessage, this );
				break;
				
			case kEnterCharCode:
				if (mEnterMessage != msg_Nothing)
					BroadcastMessage( mEnterMessage, this );
				break;
				
			case kReturnCharCode:
				if (mReturnMessage != msg_Nothing)
					BroadcastMessage( mReturnMessage, this );
				break;
				
			case charCode_SpaceBar:
				if (mSpaceBarMessage != msg_Nothing)
					BroadcastMessage( mSpaceBarMessage, this );
				break;
				
			default:
				// key not supported.
				keyHandled = false;
				break;
		}
	}
		
	return keyHandled;
}