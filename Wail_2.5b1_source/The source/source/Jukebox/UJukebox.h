// =================================================================================
//	UJukebox.h					©1999, Charles Lechasseur
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

#include "CJukebox.h"


class UJukebox
{
	public:
	// Public Functions
	
		// Initialization/Finalization
		
		static void				Initialize(
									CJukebox*	inJukebox );
		static void				Finalize();
		
		// accessing the jukebox object
		
		static CJukebox*		GetJukebox();
		
		// info about this class
		
		static Boolean			IsInitialized();
		
		// getting channel objects
		
		static CSoundChannel*	GetChannel(
									SInt32			inWhich = LArray::index_First );
		static CSoundChannel*	GetNextChannel();
		
		// channel operations
		
		static void				PlaySound(
									SndListHandle	inSound,
									Boolean			inOwnSound = true,
									Boolean			inAsync = true,
									Boolean			inLoop = false,
									SInt32			inWhichChannel = LArray::index_First );
								
		static void				PausePlaying(
									SInt32			inWhichChannel = LArray::index_First );
		static void				ResumePlaying(
									SInt32			inWhichChannel = LArray::index_First );
		
		static void				StopPlaying(
									SInt32			inWhichChannel = LArray::index_First );
									
		// mighty-stop
		
		static void				StopAll();
	
	protected:
	// Member variables
	
		static CJukebox*		sJukebox;		// the global jukebox object.
		
		static Boolean			sInitialized;	// true if we're initialized.
		
	private:
		// Can't create objects of this class.
							UJukebox();
		virtual				~UJukebox();
};


// ---------------------------------------------------------------------------------
//		¥ GetJukebox
// ---------------------------------------------------------------------------------
// returns the global CJukebox object.
// inlined here for speed.

inline
CJukebox*
UJukebox::GetJukebox()
{
	return sJukebox;
}


// ---------------------------------------------------------------------------------
//		¥ IsInitialized
// ---------------------------------------------------------------------------------
// returns true if the UJukebox system has been properly initialized.
// inlined here for speed.

inline
Boolean
UJukebox::IsInitialized()
{
	return sInitialized;
}