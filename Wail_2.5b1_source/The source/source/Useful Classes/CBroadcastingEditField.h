// =================================================================================
//	CBroadcastingEditField.h					©1999, Charles Lechasseur
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
#include <LEditField.h>
#include <LBroadcaster.h>


// forward declarations.

class CBroadcastingEditField;


// Possible return codes for the target switch confirmation messages.

enum EBETargetSwitchConfirm
{
	beTargetSwitch_Allow,		// Allow the switch
	beTargetSwitch_Prevent,		// Refuse the switch
	beTargetSwitch_PassUp		// Don't care - pass up command chain
};


// Struct used when sending target switch confirmation messages.

struct SBETargetSwitchConfirm
{
	CBroadcastingEditField*		mSender;		// the edit field that sends the message.
	LCommander*					mNewTarget;		// if switch occurs, this will be the new target
	EBETargetSwitchConfirm*		mAllowSwitch;	// set this to allow/prevent the switch.
};


// CBroadcastingEditField class declaration

class CBroadcastingEditField: public LEditField,
							  public LBroadcaster
{
	public:
	// Public Enums and Constants
		enum { class_ID = 'CBed' };
	
	// Public Functions

		//Default Constructor
							CBroadcastingEditField();
							
		// Stream Constructor 
							CBroadcastingEditField(
								LStream 						*inStream );
		// Copy Constructor
							CBroadcastingEditField(
								const CBroadcastingEditField& 	inCBroadcastingEditField );
		//Destructor
		virtual				~CBroadcastingEditField();
		
	// Accessors
	
		MessageT			GetTextChangedMessage() const { return mTextChangedMessage; }
		void				SetTextChangedMessage(
								MessageT	inTextChangedMessage )
									{ mTextChangedMessage = inTextChangedMessage; }
									
		MessageT			GetPressedEnterMessage() const { return mPressedEnterMessage; }
		void				SetPressedEnterMessage(
								MessageT	inPressedEnterMessage )
									{ mPressedEnterMessage = inPressedEnterMessage; }
									
		MessageT			GetSingleClickMessage() const { return mSingleClickMessage; }
		void				SetSingleClickMessage(
								MessageT	inSingleClickMessage )
									{ mSingleClickMessage = inSingleClickMessage; }
									
		MessageT			GetDoubleClickMessage() const { return mDoubleClickMessage; }
		void				SetDoubleClickMessage(
								MessageT	inDoubleClickMessage )
									{ mDoubleClickMessage = inDoubleClickMessage; }
		
	// Public functions
	
		virtual void		UserChangedText();
		
		virtual Boolean		HandleKeyPress(
								const EventRecord&	inKeyEvent );
								
	protected:
	// Protected functions
	
		virtual void		ClickSelf(
								const SMouseDownEvent&	inMouseDown );
								
		virtual Boolean		AllowBeTarget(
								LCommander*				inNewTarget );
									
		virtual Boolean		AllowDontBeTarget(
								LCommander*				inNewTarget );							
									
		virtual void		BeTarget();
		virtual void		DontBeTarget();
		
	private:
	// Member Variables and Classes
	
		MessageT			mTextChangedMessage;	// Message to be broadcast when the
													// text changes.
			
		MessageT			mPressedEnterMessage;	// Message to be broadcast when the
													// user presses the Enter or Return key.
													
		MessageT			mSingleClickMessage;	// Message to be broadcast when the
													// user clicks once on the pane.
												
		MessageT			mDoubleClickMessage;	// Message to be broadcast when the
													// user double-clicks on the pane.
													
		MessageT			mBeTargetMessage;		// Message to be broadcast when the
													// panes becomes the target.
													
		MessageT			mDontBeTargetMessage;	// Message to be broadcast when the
													// pane stops being the target.
													
		MessageT			mAllowBeTargetMessage;	// Message to be broadcast when the
													// pane is asked if it wants to become
													// the target.
													
		MessageT			mAllowDontBeTargetMessage;	// Message to be broadcast when
													// the pane is asked if it wants to
													// stop becoming the target.
													
	// Private Functions
		// Defensive programming. No  operator=
		CBroadcastingEditField&			operator=(const CBroadcastingEditField&);
};
