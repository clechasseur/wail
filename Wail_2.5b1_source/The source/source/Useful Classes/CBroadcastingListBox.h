// =================================================================================
//	CBroadcastingListBox.h					©1999, Charles Lechasseur
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
#include "C1Col1SelTextListBox.h"


// CBroadcastingListBox class declaration

#pragma mark -- CBroadcastingListBox class declaration --

class CBroadcastingListBox: public C1Col1SelTextListBox
{
	public:
	// Public Enums and Constants
		enum { class_ID = 'CBlb' };
	
	// Public Functions

		//Default Constructor
							CBroadcastingListBox();
		// Note: Add non-stream constructors here
		
		// Stream Constructor 
							CBroadcastingListBox( LStream *inStream );
		// Copy Constructor
							CBroadcastingListBox(const CBroadcastingListBox& inCBroadcastingListBox);
		//Destructor
		virtual				~CBroadcastingListBox();
		
	// overridden functions
	
		virtual Boolean		HandleKeyPress(
								const EventRecord&	inKeyEvent);
		
	private:
	// Member variables
	
		MessageT			mBackspaceMessage;	// message to send when Backspace is hit.
		MessageT			mDeleteMessage;		// message to send when Delete is hit.
		MessageT			mEnterMessage;		// message to send when Enter is hit.
		MessageT			mReturnMessage;		// message to send when Return is hit.
		MessageT			mSpaceBarMessage;	// message to send when Space Bar is hit.
	
	// Private Functions
		// Defensive programming. No  operator=
		CBroadcastingListBox&			operator=(const CBroadcastingListBox&);
};
