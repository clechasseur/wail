// =================================================================================
//	CBroadcastingEditField.cp					©1999, Charles Lechasseur
// =================================================================================
//
// CBroadcastingEditField is a variant of LEditField that broadcasts various messages
// in different circumstances.
//
// Version history:
//	Version			  Date				Description
//	-------			--------			-----------
//	  1.0			99/03/05			Initial release. only the "Text Changed" message
//										is broadcast.
//
//    1.1			03/01/07			Now handles "Pressed enter", "Single-click" and
//										"Double-click" messages. Added "Be target",
//										"Don't be target", "Allow be target" and
//										"Allow don't be target" messages.

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

#include "CBroadcastingEditField.h"


// ---------------------------------------------------------------------------
//	¥ CBroadcastingEditField							Constructor	[public]
// ---------------------------------------------------------------------------
// initializes all the messages to msg_Nothing.

CBroadcastingEditField::CBroadcastingEditField()
{
	mTextChangedMessage =
	mPressedEnterMessage =
	mSingleClickMessage =
	mDoubleClickMessage =
	mBeTargetMessage =
	mDontBeTargetMessage =
	mAllowBeTargetMessage =
	mAllowDontBeTargetMessage = msg_Nothing;
}


// ---------------------------------------------------------------------------
//	¥ CBroadcastingEditField					Stream Constructor	[public]
// ---------------------------------------------------------------------------

CBroadcastingEditField::CBroadcastingEditField(
	LStream *inStream )
	: LEditField(inStream)
{
	MessageT	theMessages[8];
	inStream->ReadData( theMessages, sizeof(MessageT) * 8 );
	
	mTextChangedMessage = theMessages[0];
	mPressedEnterMessage = theMessages[1];
	mSingleClickMessage = theMessages[2];
	mDoubleClickMessage = theMessages[3];
	mBeTargetMessage = theMessages[4];
	mDontBeTargetMessage = theMessages[5];
	mAllowBeTargetMessage = theMessages[6];
	mAllowDontBeTargetMessage = theMessages[7];
}


// ---------------------------------------------------------------------------
//	¥ CBroadcastingEditField						Copy Constructor	[public]
// ---------------------------------------------------------------------------

CBroadcastingEditField::CBroadcastingEditField(
	const CBroadcastingEditField& inOriginal )
	: LEditField(inOriginal)
{
	mTextChangedMessage = inOriginal.mTextChangedMessage;
	mPressedEnterMessage = inOriginal.mPressedEnterMessage;
	mSingleClickMessage = inOriginal.mSingleClickMessage;
	mDoubleClickMessage = inOriginal.mDoubleClickMessage;
	mBeTargetMessage = inOriginal.mBeTargetMessage;
	mDontBeTargetMessage = inOriginal.mDontBeTargetMessage;
	mAllowBeTargetMessage = inOriginal.mAllowBeTargetMessage;
	mAllowDontBeTargetMessage = inOriginal.mAllowDontBeTargetMessage;
}


// ---------------------------------------------------------------------------
//	¥ ~CBroadcastingEditField							Destructor	[public]
// ---------------------------------------------------------------------------

CBroadcastingEditField::~CBroadcastingEditField()
{
}


// ---------------------------------------------------------------------------
//		¥ UserChangedText
// ---------------------------------------------------------------------------
//	Text of EditField has changed as a result of user action
//
//	we must broadcast the mTextChangedMessage when this happens.

void
CBroadcastingEditField::UserChangedText()
{
	if (mTextChangedMessage != msg_Nothing)
		BroadcastMessage( mTextChangedMessage, this );
}


// ---------------------------------------------------------------------------
//		¥ HandleKeyPress
// ---------------------------------------------------------------------------
// Handles key presses and broadcasts messages for appropriate keystrokes.

Boolean
CBroadcastingEditField::HandleKeyPress(
	const EventRecord&	inKeyEvent )
{
	// call parent routine to handle key press itself.
	Boolean handled = LEditField::HandleKeyPress( inKeyEvent );
	
	// check for keys with special meanings.
	UInt16 theCharCode = (UInt16) (inKeyEvent.message & charCodeMask);
	switch (theCharCode)
	{
		case kEnterCharCode:
		case kReturnCharCode:
			// user pressed enter, broadcast our message.
			if (mPressedEnterMessage != msg_Nothing)
				BroadcastMessage( mPressedEnterMessage, this );
			break;
	}
	
	return handled;
}


// ---------------------------------------------------------------------------
//		¥ ClickSelf
// ---------------------------------------------------------------------------
// handles mouse clicks inside the edit field, broadcasts messages when appropriate.

void
CBroadcastingEditField::ClickSelf(
	const SMouseDownEvent&	inMouseDown )
{
	// call parent's routine to handle the click.
	LEditField::ClickSelf( inMouseDown );
	
	// if we're the target, broadcast click messages.
	if (IsTarget())
	{
		if (GetClickCount() > 1)
		{
			// double-click
			if (mDoubleClickMessage != msg_Nothing)
				BroadcastMessage( mDoubleClickMessage, this );
		}
		else
		{
			// single-click
			if (mSingleClickMessage != msg_Nothing)
				BroadcastMessage( mSingleClickMessage, this );
		}
	}
}


// ---------------------------------------------------------------------------
//		¥ AllowBeTarget
// ---------------------------------------------------------------------------
// overridden to broadcast a message asking the user to confirm the switch.

Boolean
CBroadcastingEditField::AllowBeTarget(
	LCommander*	inNewTarget )
{
	// ask user if he wishes to allow the switch.
	EBETargetSwitchConfirm allowAction = beTargetSwitch_PassUp;
	if (mAllowBeTargetMessage != msg_Nothing)
	{
		// include a pointer to a struct containing the new target
		// and a pointer to our "allow switch" variable.
		// user can check the new target and modify the variable.
		SBETargetSwitchConfirm confirmInfo;
		confirmInfo.mSender = this;
		confirmInfo.mNewTarget = inNewTarget;
		confirmInfo.mAllowSwitch = &allowAction;
		BroadcastMessage( mAllowBeTargetMessage, &confirmInfo );
	}
	
	// check what we should do.
	Boolean allow;
	switch (allowAction)
	{
		case beTargetSwitch_Allow:		// user explicitely allowed the switch
			allow = true;
			break;
			
		case beTargetSwitch_Prevent:	// user explicitely refused the switch
			allow = false;
			break;
			
		default:						// user didn't care, pass up the command chain.
			SignalIfNot_( allowAction == beTargetSwitch_PassUp );
			allow = LEditField::AllowBeTarget( inNewTarget );
			break;
	}
	
	return allow;
}


// ---------------------------------------------------------------------------
//		¥ AllowBeTarget
// ---------------------------------------------------------------------------
// overridden to broadcast a message asking the user to confirm the switch.

Boolean
CBroadcastingEditField::AllowDontBeTarget(
	LCommander*	inNewTarget )
{
	// ask user if he wishes to allow the switch.
	EBETargetSwitchConfirm allowAction = beTargetSwitch_PassUp;
	if (mAllowDontBeTargetMessage != msg_Nothing)
	{
		// include a pointer to a struct containing the new target
		// and a pointer to our "allow switch" variable.
		// user can check the new target and modify the variable.
		SBETargetSwitchConfirm confirmInfo;
		confirmInfo.mSender = this;
		confirmInfo.mNewTarget = inNewTarget;
		confirmInfo.mAllowSwitch = &allowAction;
		BroadcastMessage( mAllowDontBeTargetMessage, &confirmInfo );
	}
	
	// check what we should do.
	Boolean allow;
	switch (allowAction)
	{
		case beTargetSwitch_Allow:		// user explicitely allowed the switch
			allow = true;
			break;
			
		case beTargetSwitch_Prevent:	// user explicitely refused the switch
			allow = false;
			break;
			
		default:						// user didn't care, pass up the command chain.
			SignalIfNot_( allowAction == beTargetSwitch_PassUp );
			allow = LEditField::AllowDontBeTarget( inNewTarget );
			break;
	}
	
	return allow;
}						


// ---------------------------------------------------------------------------
//		¥ BeTarget
// ---------------------------------------------------------------------------
// overridden to notify user of target switch.

void
CBroadcastingEditField::BeTarget()
{
	// call parent's routine to let it do its thing.
	LEditField::BeTarget();

	// notify user of switch.
	if (mBeTargetMessage != msg_Nothing)
		BroadcastMessage( mBeTargetMessage, this );
}


// ---------------------------------------------------------------------------
//		¥ DontBeTarget
// ---------------------------------------------------------------------------
// overridden to notify user of target switch.

void
CBroadcastingEditField::DontBeTarget()
{
	// call parent's routine to let it do its thing.
	LEditField::DontBeTarget();

	// notify user of switch.
	if (mDontBeTargetMessage != msg_Nothing)
		BroadcastMessage( mDontBeTargetMessage, this );
}
