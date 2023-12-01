// =================================================================================
//	CBeeProgressBar.h					©1999, Charles Lechasseur
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
#include <LControl.h>

#include <UGWorld.h>


class CBeeProgressBar: public LControl
{
	public:
	// Public Enums and Constants
		enum { class_ID = 'BPbr' };
	
	// Public Functions

		// Stream Constructor 
							CBeeProgressBar( LStream *inStream );
		//Destructor
		virtual				~CBeeProgressBar();
		
	// accessors
	
		SInt16			GetPadding() const
							{ return mPadding; }
		void			SetPadding( SInt16 inPadding )
							{ mPadding = inPadding; }
							
		Boolean			GetDrawImmediately() const
							{ return mDrawImmediately; }
		void			SetDrawImmediately( Boolean inDrawImmediately )
							{ mDrawImmediately = inDrawImmediately; }
							
		Boolean			GetDrawOffscreen() const
							{ return mDrawOffscreen; }
		void			SetDrawOffscreen( Boolean inDrawOffscreen );
		
	// value change
	
		virtual void	SetValue(
							SInt32		inValue );
							
		virtual void	SetMaxValue(
							SInt32		inMaxValue );
							
	// drawing
	
		virtual void	DrawSelf();
		
	private:
	// Member Variables and Classes
	
		ResIDT			mForePatternID;		// ID of fore pattern.
		ResIDT			mBackPatternID;		// ID of back pattern.
		
		SInt16			mPadding;			// padding of progress bar's content inside frame.
		
		Boolean			mDrawImmediately;	// draw immediately on value change?
		Boolean			mDrawOffscreen;		// use offscreen drawing?
		
		PixPatHandle	mForePixPat;		// fore pattern's PixPat.
		PixPatHandle	mBackPixPat;		// back pattern's PixPat.
	
		LGWorld*		mGWorld;			// GWorld used for drawing.
	
	// Private Functions
		// Defensive programming. No  operator=
		CBeeProgressBar&			operator=(const CBeeProgressBar&);
		// Defensive programming. No copy constructor
							CBeeProgressBar(const CBeeProgressBar& inCBeeProgressBar);
};
