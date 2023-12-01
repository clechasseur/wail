// =================================================================================
//	UMoreModalDialogs.h					©1999, Charles Lechasseur
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

#include <UModalDialogs.h>


// UMoreModalDialogs class
#pragma mark == UMoreModalDialogs ==

class UMoreModalDialogs
{
public:

	static Boolean		AskOneQuestion(
							LCommander	*inSuper,
							ResIDT		inDialogID );
							
	static Boolean		AskOneQuestionWithShowAgain(
							LCommander	*inSuper,
							ResIDT		inDialogID,
							MessageT	inShowAgainMessage,
							Boolean&	outShowAgain );
};


#pragma mark -


// StNoAboutDialogHandler class
#pragma mark == StNoAboutDialogHandler ==

class StNoAboutDialogHandler : public StDialogHandler
{
public:
						StNoAboutDialogHandler(
								ResIDT			inDialogResID, 
								LCommander		*inSuper);
						
						StNoAboutDialogHandler(
								LWindow			*inWindow,
								LCommander		*inSuper);
								
	virtual				~StNoAboutDialogHandler();
	
	virtual void		FindCommandStatus(
								CommandT		inCommand,
								Boolean			&outEnabled,
								Boolean			&outUsesMark,
								UInt16			&outMark,
								Str255			outName);
};


#pragma mark -


// StDontShowAgainDialogHandler class
#pragma mark == StDontShowAgainDialogHandler ==

const MessageT	msg_DontShowAgain	= '1Sh<';
const MessageT	msg_ShowAgain		= '0Sh<';

class StDontShowAgainDialogHandler : public StDialogHandler
{
public:
						StDontShowAgainDialogHandler(
								ResIDT			inDialogResID, 
								MessageT		inControlMessage,
								LCommander		*inSuper);
						
						StDontShowAgainDialogHandler(
								LWindow			*inWindow,
								MessageT		inControlMessage,
								LCommander		*inSuper);
								
	virtual				~StDontShowAgainDialogHandler();
	
	virtual void		ListenToMessage(
								MessageT		inMessage,
								void			*ioParam);
								
protected:

	// members
	
	MessageT			mControlMessage;
};