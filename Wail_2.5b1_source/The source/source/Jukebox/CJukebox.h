// =================================================================================
//	CJukebox.h					©1999, Charles Lechasseur
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

#include "CSoundChannel.h"


// constants

const SInt32	soundChannel_Any = -1;		// special flag: use any channel available.


class CJukebox
{
	public:
	// Public Functions
		//Default Constructor
								CJukebox(
									SInt32	inNumChannels,
									SInt16	inSynth = 0,
									SInt32	inInit = 0 );
		//Destructor
		virtual					~CJukebox();
		
		// getting channel objects
		
		virtual CSoundChannel*	GetChannel(
									SInt32			inWhich = LArray::index_First ) const;
		virtual CSoundChannel*	GetNextChannel();
		
		// channel operations
		
		virtual void			PlaySound(
									SndListHandle	inSound,
									Boolean			inOwnSound = true,
									Boolean			inAsync = true,
									Boolean			inLoop = false,
									SInt32			inWhichChannel = LArray::index_First );
								
		virtual void			PausePlaying(
									SInt32			inWhichChannel = LArray::index_First );
		virtual void			ResumePlaying(
									SInt32			inWhichChannel = LArray::index_First );
		
		virtual void			StopPlaying(
									SInt32			inWhichChannel = LArray::index_First );
									
		// mighty-stop
		
		virtual void			StopAll();
		
	protected:
		// Member Variables and Classes

		TArray<CSoundChannel*>	mChannels;		// our array of channels.
		
		ArrayIndexT				mNextChannel;	// next channel that is to receive a sound.
	
	private:
		// Defensive programming. No copy constructor nor operator=
							CJukebox(const CJukebox&);
		CJukebox&			operator=(const CJukebox&);
};
