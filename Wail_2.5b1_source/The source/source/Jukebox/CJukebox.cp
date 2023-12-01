// =================================================================================
//	CJukebox.cp					©1999, Charles Lechasseur
// =================================================================================
//
// Utility class for managing a series of sound channels objects (CSoundChannel).

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

#include "CJukebox.h"


// ---------------------------------------------------------------------------
//	¥ CJukebox									[Constructor]
// ---------------------------------------------------------------------------
// pre-allocates a given number of CSoundChannel objects.
//
// the inSynth and inInit parameters will be used for all channels allocated.
// they both default to 0. look at CSoundChannel constructor for explanations.

CJukebox::CJukebox(
	SInt32	inNumChannels,
	SInt16	inSynth,
	SInt32	inInit)
{
	// allocate the given number of channels and store them in our array.
	mChannels.AdjustAllocation( inNumChannels );
	for (SInt32 i = 0; i < inNumChannels; i++)
	{
		CSoundChannel* theChannel = new CSoundChannel( inSynth, inInit );
		ThrowIfNil_( theChannel );
		
		mChannels.AddItem( theChannel );
	}
	
	// set the next channel to the first one.
	mNextChannel = LArray::index_First;
}


// ---------------------------------------------------------------------------
//	¥ ~CJukebox									[Destructor]
// ---------------------------------------------------------------------------
// disposes of all channels allocated.

CJukebox::~CJukebox()
{
	try
	{
		// deallocate all channels.
		while (mChannels.GetCount() > 0)
		{
			// get first channel, remove it from list and dispose of it.
			CSoundChannel* theChannel = mChannels[LArray::index_First];
			mChannels.Remove( theChannel );
			delete theChannel;
		}
	}
	
	catch (...) { }	// don't throw.
}


// ---------------------------------------------------------------------------------
//		¥ GetChannel
// ---------------------------------------------------------------------------------
// returns a pointer to the channel object numbered inWhich.
// channels are indexed starting at 1.

CSoundChannel*
CJukebox::GetChannel(
	SInt32	inWhich ) const
{
	return mChannels[inWhich];
}
									
									
// ---------------------------------------------------------------------------------
//		¥ GetNextChannel
// ---------------------------------------------------------------------------------									
// returns the next channel scheduled to play a given sound if the sound is to be
// played with the channel number soundChannel_Any.
//
// note: NEVER access the mNextChannel field directly! use this method instead.

CSoundChannel*
CJukebox::GetNextChannel()
{
	ArrayIndexT theChannelIndex = mNextChannel;	// start using the default next channel.
	
	// check for an empty channel. if we find one, we'll use it instead of the default one.
	TArrayIterator<CSoundChannel*> theIterator( mChannels );
	CSoundChannel* theChannel;
	Boolean foundOne = false;
	while (!foundOne && theIterator.Next( theChannel ))
		foundOne = !theChannel->IsBusy();
	
	// if we found one, use its index. otherwise, we'll use the default index and increment it.
	if (foundOne)
		theChannelIndex = theIterator.GetCurrentIndex();
	else
	{
		mNextChannel++;
		
		// check for wrapping.
		if (mNextChannel > mChannels.GetCount())
			mNextChannel = LArray::index_First;
	}
		
	// return the channel found.
	return GetChannel( theChannelIndex );
}


// ---------------------------------------------------------------------------------
//		¥ PlaySound
// ---------------------------------------------------------------------------------
// plays a sound in the given channel. if inWhichChannel is a valid index, it is used
// to determine which channel is used. if it is equal to soundChannel_Any, the next
// available channel is used, and if none are available (free), it uses the next channel
// that's waiting in line to play.
//
// any sound playing in the selected channel is stopped in favor of the new sound.
//
// all parameters except for inWhichChannel map to the same parameters of
// CSoundChannel::PlaySound. look in CSoundChannel source for more info.

void
CJukebox::PlaySound(
	SndListHandle	inSound,
	Boolean			inOwnSound,
	Boolean			inAsync,
	Boolean			inLoop,
	SInt32			inWhichChannel )
{
	// fetch proper channel.
	CSoundChannel* theChannel;
	if (inWhichChannel == soundChannel_Any)
		theChannel = GetNextChannel();
	else
		theChannel = GetChannel( inWhichChannel );
		
	// play sound in the channel.
	theChannel->PlaySound( inSound, inOwnSound, inAsync, inLoop );
}
								

// ---------------------------------------------------------------------------------
//		¥ PausePlaying
// ---------------------------------------------------------------------------------
// pauses the sound playing in the given channel.

void
CJukebox::PausePlaying(
	SInt32	inWhichChannel )
{
	CSoundChannel* theChannel = GetChannel( inWhichChannel );
	if (theChannel != nil)
		theChannel->PausePlaying();
}


// ---------------------------------------------------------------------------------
//		¥ ResumePlaying
// ---------------------------------------------------------------------------------
// resumes the sound that was paused in the given channel.

void
CJukebox::ResumePlaying(
	SInt32	inWhichChannel )
{
	CSoundChannel* theChannel = GetChannel( inWhichChannel );
	if (theChannel != nil)
		theChannel->ResumePlaying();
}
		

// ---------------------------------------------------------------------------------
//		¥ StopPlaying
// ---------------------------------------------------------------------------------
// stops the sound playing in the given channel.

void
CJukebox::StopPlaying(
	SInt32	inWhichChannel )
{
	CSoundChannel* theChannel = GetChannel( inWhichChannel );
	if (theChannel != nil)
		theChannel->StopPlaying();
}


// ---------------------------------------------------------------------------------
//		¥ StopAll
// ---------------------------------------------------------------------------------									
// stops all sounds playing in all channels.

void
CJukebox::StopAll()
{
	TArrayIterator<CSoundChannel*> theIterator( mChannels );
	CSoundChannel* theChannel;
	while (theIterator.Next( theChannel ))
		StopPlaying( theIterator.GetCurrentIndex() );
}