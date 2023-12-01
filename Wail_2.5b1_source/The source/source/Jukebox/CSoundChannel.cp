// =================================================================================
//	CSoundChannel.cp					©1999, Charles Lechasseur
// =================================================================================
//
// wrapper class for handling sounds played in a Sound Manager channel.

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

#include "CSoundChannel.h"

#include <A4Stuff.h>


// ---------------------------------------------------------------------------
//	¥ CSoundChannel								[Constructor]
// ---------------------------------------------------------------------------
// allocates a new sound channel, readying it for sound play.
// the input parameters are similar to those of SndNewChannel.
// look them up in Think Ref or Toolbox Assistant for more info.

CSoundChannel::CSoundChannel(
	SInt16	inSynth,
	SInt32	inInit)
	: mChannel( nil ),
	  mSound( nil ),
	  mSoundState( 0 ),
	  mOwnSound( false ),
	  mLooping( false ),
	  mCallBackProc( CSoundChannel::StaticSoundChannelCallBack )
{
	// init our channel.
	ThrowIfOSErr_( ::SndNewChannel( &mChannel, inSynth, inInit, mCallBackProc ) );
	ThrowIfNil_( mChannel );
	
	// the callback function will need a pointer to us in order to call our local callback,
	// so we need to stuff that into a field of the channel record.
	mChannel->userInfo = (SInt32) this;
}


// ---------------------------------------------------------------------------
//	¥ ~CSoundChannel							[Destructor]
// ---------------------------------------------------------------------------

CSoundChannel::~CSoundChannel(void)
{
	// stop playing.
	StopPlaying();

	// dispose of our channel.
	if (mChannel != nil)
	{
		// deallocate memory.
		::SndDisposeChannel( mChannel, true );
		mChannel = nil;
	}
}


// ---------------------------------------------------------------------------------
//		¥ GetStatus
// ---------------------------------------------------------------------------------
// queries the Sound Manager channel about its status and returns it.

void
CSoundChannel::GetStatus(
	SCStatus&	outStatus )
{
	ThrowIfOSErr_( ::SndChannelStatus( mChannel, sizeof(SCStatus), &outStatus ) );
}
				
							
// ---------------------------------------------------------------------------------
//		¥ IsBusy
// ---------------------------------------------------------------------------------
// returns true if the channel is busy. note that this is not the same as currently
// playing - it could be busy but paused. if you want to know if the sound is
// *currently* playing (i.e. can be heard), call IsPlaying.

Boolean
CSoundChannel::IsBusy()
{
	// get channel status.
	SCStatus theStatus;
	GetStatus( theStatus );
	
	// return true if currently busy.
	return theStatus.scChannelBusy;
}


// ---------------------------------------------------------------------------------
//		¥ IsPaused
// ---------------------------------------------------------------------------------
// returns true if the channel is currently playing a sound but the sound is paused.

Boolean
CSoundChannel::IsPaused()
{
	// get channel status.
	SCStatus theStatus;
	GetStatus( theStatus );
	
	// return true if currently paused.
	return theStatus.scChannelPaused;
}


// ---------------------------------------------------------------------------------
//		¥ IsPlaying
// ---------------------------------------------------------------------------------
// returns true if a sound is currently playing in the channel and is NOT paused.
// to know if the channel is simply busy, paused or not, call IsBusy.

Boolean
CSoundChannel::IsPlaying()
{
	// querying the channel for status twice might be a little bit slower. but
	// it's worth it to allow subclasses to modify this behavior if needed.
	return (IsBusy() && !IsPaused());
}


// ---------------------------------------------------------------------------------
//		¥ DoCommand
// ---------------------------------------------------------------------------------
// sends a command to the channel. if inImmediate is true, sends it immediately,
// bypassing the current command queue (see SndDoImmediate). [default for inImmediate: false]

void
CSoundChannel::DoCommand(
	const SndCommand&	inCommand,
	Boolean				inImmediate )
{
	OSErr err;

	if (inImmediate)
		err = ::SndDoImmediate( mChannel, &inCommand );
	else
		err = ::SndDoCommand( mChannel, &inCommand, false );
		
	ThrowIfOSErr_( err );
}


// ---------------------------------------------------------------------------------
//		¥ DoCommand
// ---------------------------------------------------------------------------------
// same as above, but instead of giving a SndCommand struct as parameter, give
// it all the different elements of a standard SndCommand struct.
// for the lazy type (like me :)

void
CSoundChannel::DoCommand(
	SInt16				inCmd,
	SInt16				inParam1,
	SInt32				inParam2,
	Boolean				inImmediate )
{
	SndCommand theCommand = { inCmd, inParam1, inParam2 };
	
	DoCommand( theCommand, inImmediate );
}


// ---------------------------------------------------------------------------------
//		¥ PlaySound
// ---------------------------------------------------------------------------------
// starts playing the given sound, stopping any previously playing sounds.
//
// if inOwnSound is true, we assume ownership of the sound and dispose of
// it when we're done playing it (we assume DisposeHandle is OK for this task).
// [default: true]
//
// if inAsync is true, the sound is playing asynchronously and the call returns
// immediately. if it is false, the call doesn't return until the sound is done playing.
// [default: true]
//
// if inLooping is true, the sound will loop once it is done playing (start again).
// this parameter is ignored if inAsync is false.
// [default: false]

void
CSoundChannel::PlaySound(
	SndListHandle	inSound,
	Boolean			inOwnSound,
	Boolean			inAsync,
	Boolean			inLoop )
{
	// stop any previously playing sounds.
	if (IsBusy())
		StopPlaying();
		
	if (!inAsync)
	{
		// lock the sound.
		SignedByte theState = ::HGetState( (Handle) inSound );
		::HLock( (Handle) inSound );
		
		// play the sound and wait 'till it's done.
		ThrowIfOSErr_( ::SndPlay( mChannel, inSound, false ) );
		
		// if we own the sound, dispose of it. otherwise, simply unlock it.
		if (inOwnSound)
			::DisposeHandle( (Handle) inSound );
		else
			::HSetState( (Handle) inSound, theState );
	}
	else
	{
		// store this sound and mark it as owned or not.
		mSound = inSound;
		mOwnSound = inOwnSound;
		
		// lock this sound. IMPORTANT!
		mSoundState = ::HGetState( (Handle) mSound );
		::HLock( (Handle) mSound );
		
		// start the sound playing asynchronously. this will return immediately.
		ThrowIfOSErr_( ::SndPlay( mChannel, mSound, true ) );
		
		// we must immediately send a callback command in order to inform the
		// channel that we want to be aware of the moment the sound is done playing.
		// we pass the value of our A4-world in order to gain access to our globals
		// in the callback procedure.
		DoCommand( callBackCmd, 0, GetCurrentA4() );
		
		// save the looping option for later.
		mLooping = inLoop;
	}
}


// ---------------------------------------------------------------------------------
//		¥ PausePlaying
// ---------------------------------------------------------------------------------
// if the channel is currently playing a sound, pauses it.

void
CSoundChannel::PausePlaying()
{
	if (IsPlaying())
		DoCommand( pauseCmd, 0, 0, true );
}


// ---------------------------------------------------------------------------------
//		¥ ResumePlaying
// ---------------------------------------------------------------------------------
// if the channle is currently playing a sound and paused, resumes the sound playing.

void
CSoundChannel::ResumePlaying()
{
	if (IsPaused())
		DoCommand( resumeCmd, 0, 0, true );
}
		

// ---------------------------------------------------------------------------------
//		¥ StopPlaying
// ---------------------------------------------------------------------------------
// if a sound is currently playing in the channel (paused or not), stops it.
// also disposes of the sound if we own it.

void
CSoundChannel::StopPlaying()
{
	// send quiet and flush commands to stop the sound.
	DoCommand( quietCmd, 0, 0, true );
	DoCommand( flushCmd, 0, 0, true );
		
	// if we have a sound, delete it.
	if (mSound != nil)
	{
		// if we own the sound, dispose of it. otherwise, simply unlock it.
		if (mOwnSound)
			::DisposeHandle( (Handle) mSound );
		else
			::HSetState( (Handle) mSound, mSoundState );
	}
			
	// reset all sound variables.
	mSound = nil;
	mSoundState = 0;
	mOwnSound = false;
	mLooping = false;
		
#if __CSoundChannelUsesPP
	// we might need to update menus now, to allow commands to be aware of the
	// existence of a sound playing.
	LCommander::SetUpdateCommandStatus(true);
#endif //__CSoundChannelUsesPP
}


// ---------------------------------------------------------------------------------
//		¥ StaticSoundChannelCallBack
// ---------------------------------------------------------------------------------
// static callback function that's common to all CSoundChannel objects.
// simply calls the appropriate SoundChannelCallBack function of the
// CSoundChannel object associated with the given channel.

pascal void
CSoundChannel::StaticSoundChannelCallBack(
	SndChannelPtr	inChannel,
	SndCommand*		inCommand )
{
	// restore A4. they're in param2 of the command.
	SInt32 oldA4 = SetA4( inCommand->param2 );
	
	try
	{
		// call the callback function of the CSoundChannel object.
		// its pointer is hidden in the userInfo parameter of the channel record.
		((CSoundChannel*) inChannel->userInfo)->SoundChannelCallBack( *inCommand );
	}
	
	catch (...) { }	// don't propagate exception into the Toolbox!
	
	// restore previous A4.
	SetA4( oldA4 );
}
								
								
// ---------------------------------------------------------------------------------
//		¥ SoundChannelCallBack
// ---------------------------------------------------------------------------------
// called by the Sound Manager when it receives a callBackCmd. in other words, when
// a sound is done playing. here, we either have to dispose of it or restart it.

void
CSoundChannel::SoundChannelCallBack(
	SndCommand&		inCommand )
{
	// if we loop, simply restart the sound.
	if (mLooping)
	{
		// start the sound again.
		ThrowIfOSErr_( ::SndPlay( mChannel, mSound, true ) );
		
		// re-issue the callback command so we can catch that again.
		DoCommand( inCommand );
	}
	else	// if we don't loop, stop the sound.
		StopPlaying();
}