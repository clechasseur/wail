// =================================================================================
//	UJukebox.cp					©1999, Charles Lechasseur
// =================================================================================
//
// Utility class for managing a single, global CJukebox object.
//
// call Initializa early in your app's init, providing a valid pointer to a
// CJukebox object that's going to be used as the global Jukebox.
// UJukebox assumes ownership of this object.
//
// at the end of the application, call Finalize.

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

#include "UJukebox.h"


// static variables

CJukebox*		UJukebox::sJukebox = nil;		// the global jukebox.

Boolean			UJukebox::sInitialized = false;	// true if we're initialized.


// ---------------------------------------------------------------------------
//		¥ Initialize
// ---------------------------------------------------------------------------
// initializes the UJukebox system and keeps a reference to the global jukebox.
// the class assumes ownership of the given CJukebox object and will dispose
// of it in the Finalize routine.

void
UJukebox::Initialize(
	CJukebox*	inJukebox )
{
	// keep the jukebox around.
	sJukebox = inJukebox;
	
	// we're initialized if we have a valid jukebox.
	sInitialized = (sJukebox != nil);
}


// ---------------------------------------------------------------------------
//		¥ Finalize
// ---------------------------------------------------------------------------
// dispose of the global jukebox if allocated.

void
UJukebox::Finalize()
{
	if (IsInitialized())
	{
		// dispose of the jukebox.
		delete sJukebox;
		sJukebox = nil;
		
		// we're no longer initialized.
		sInitialized = false;
	}
}


// =================================================================================
//	CJukebox functions
// =================================================================================
// the following functions simply remap to their equivalent in CJukebox.
// they all access the global jukebox object.


CSoundChannel*
UJukebox::GetChannel(
	SInt32	inWhich )
{
	return sJukebox->GetChannel( inWhich );
}
									
									
CSoundChannel*
UJukebox::GetNextChannel()
{
	return sJukebox->GetNextChannel();
}


void
UJukebox::PlaySound(
	SndListHandle	inSound,
	Boolean			inOwnSound,
	Boolean			inAsync,
	Boolean			inLoop,
	SInt32			inWhichChannel )
{
	sJukebox->PlaySound( inSound, inOwnSound, inAsync, inLoop, inWhichChannel );
}


void
UJukebox::PausePlaying(
	SInt32	inWhichChannel )
{
	sJukebox->PausePlaying( inWhichChannel );
}


void
UJukebox::ResumePlaying(
	SInt32	inWhichChannel )
{
	sJukebox->ResumePlaying( inWhichChannel );
}
		

void
UJukebox::StopPlaying(
	SInt32	inWhichChannel )
{
	sJukebox->StopPlaying( inWhichChannel );
}


void
UJukebox::StopAll()
{
	sJukebox->StopAll();
}