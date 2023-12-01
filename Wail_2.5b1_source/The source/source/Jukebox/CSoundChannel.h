// =================================================================================
//	CSoundChannel.h					©1999, Charles Lechasseur
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


// ---------------------------------------------------------------------------------
//	PowerPlant support
// ---------------------------------------------------------------------------------
// CSoundChannel is designed to be used without the help of PowerPlant.
// it uses standard Apple types and throws if necessary.
//
// if you want to integrate it into PowerPlant, set the variable __CSoundChannelUsesPP
// to 1. it will use PP throw macros, and will update menu status when a sound is
// stopped playing.
//
// the default is to set it to 1. set it to 0 in your prefix file if you want to
// disable PP support.

#ifndef __CSoundChannelUsesPP
#define __CSoundChannelUsesPP 1
#endif __CSoundChannelUsesPP

// if we don't use PP, we need to provide throw macros matching those of PP.
#if !__CSoundChannelUsesPP

#define ThrowIfOSErr_(err)					\
			if (err != noErr)				\
				throw err
			
#define ThrowIfNil_(pointer)				\
			if (pointer == nil)				\
				throw (OSErr) memFullErr

#endif //!__CSoundChannelUsesPP


// ---------------------------------------------------------------------------------
//	StSndCallBackProc
// ---------------------------------------------------------------------------------
// stack-based class for creating and disposing of a SndCallbackUPP.

class StSndCallBackProc
{
public:

	// Constructor
	
							StSndCallBackProc(
								SndCallBackProcPtr	inUserRoutine )
									{ mUPP = NewSndCallBackProc( inUserRoutine ); }
								
	// Destructor
								
	virtual					~StSndCallBackProc()
									{ DisposeRoutineDescriptor( mUPP ); }
	
	// casting operators
	
							operator SndCallBackUPP() { return mUPP; }
							
protected:

	// member variables
	
	SndCallBackUPP			mUPP;
	
private:

	// no copy construction or assignment
	
							StSndCallBackProc(const StSndCallBackProc&);
	StSndCallBackProc&		operator=(const StSndCallBackProc&);
};


// ---------------------------------------------------------------------------------
//	CSoundChannel class
// ---------------------------------------------------------------------------------
// wrapper class for a Sound Manager channel.

class CSoundChannel
{
	public:
	// Public Functions
		//Constructor
							CSoundChannel(
								SInt16	inSynth = 0,
								SInt32	inInit = 0 );
		//Destructor
		virtual				~CSoundChannel();
								
		// querying the channel about its status
		
		virtual void		GetStatus(
								SCStatus&		outStatus );
							
		virtual Boolean		IsBusy();
		virtual Boolean		IsPaused();
		virtual Boolean		IsPlaying();
		
		// sending commands to the channel
		
		virtual void		DoCommand(
								const SndCommand&	inCommand,
								Boolean				inImmediate = false );
		void				DoCommand(
								SInt16				inCmd,
								SInt16				inParam1,
								SInt32				inParam2,
								Boolean				inImmediate = false );
		
		// starting play
		
		virtual void		PlaySound(
								SndListHandle	inSound,
								Boolean			inOwnSound = true,
								Boolean			inAsync = true,
								Boolean			inLoop = false );
								
		virtual void		PausePlaying();
		virtual void		ResumePlaying();
		
		virtual void		StopPlaying();
									
	protected:
	// Protected functions
	
		// static callback function for all CSoundChannel objects
		
		static pascal void	StaticSoundChannelCallBack(
								SndChannelPtr	inChannel,
								SndCommand*		inCommand );
								
		// private callback function specific for this channel object
		
		virtual void		SoundChannelCallBack(
								SndCommand&		inCommand );
	
	// Member Variables and Classes
		
		SndChannelPtr			mChannel;		// our sound channel.
		
		SndListHandle			mSound;			// the sound currently playing.
		SignedByte				mSoundState;	// state of the handle (saved when playing).
		Boolean					mOwnSound;		// do we own this sound (i.e. must we release it)?
		Boolean					mLooping;		// do we loop the current sound?
		
		StSndCallBackProc		mCallBackProc;	// callback routine for our channel.
	
	private:
		// Defensive programming. No copy constructor nor operator=
							CSoundChannel(const CSoundChannel&);
		CSoundChannel&			operator=(const CSoundChannel&);
};