// =================================================================================
//	CBeeProgressBar.cp					©1999, Charles Lechasseur
// =================================================================================
//
// Implementation of a progress bar as a pane. Based upon the progress bar developed
// for the Beengineª (©1997-1998, Charles Lechasseur).

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

#include "CBeeProgressBar.h"


// ---------------------------------------------------------------------------
//	¥ CBeeProgressBar					Stream Constructor	[public]
// ---------------------------------------------------------------------------

CBeeProgressBar::CBeeProgressBar(LStream *inStream)
	: LControl(inStream)
{
	// read IDs of patterns.
	inStream->ReadData( &mForePatternID, sizeof(mForePatternID) );
	inStream->ReadData( &mBackPatternID, sizeof(mBackPatternID) );
	
	// read padding of progress bar.
	inStream->ReadData( &mPadding, sizeof(mPadding) );
	
	// read booleans.
	inStream->ReadData( &mDrawImmediately, sizeof(mDrawImmediately) );
	inStream->ReadData( &mDrawOffscreen, sizeof(mDrawOffscreen) );
	
	// get PixPats.
	mForePixPat = ::GetPixPat( mForePatternID );
	if (::ResError() != noErr)
		mForePixPat = nil;
		
	mBackPixPat = ::GetPixPat( mBackPatternID );
	if (::ResError() != noErr)
		mForePixPat = nil;
		
	// create GWorld if necessary.
	mGWorld = nil;	// in case it's not allocated.
	if (mDrawOffscreen)
	{
		Rect theFrame;
		CalcLocalFrameRect( theFrame );
		mGWorld = new LGWorld( theFrame );
	}
}


// ---------------------------------------------------------------------------
//	¥ ~CBeeProgressBar							Destructor	[public]
// ---------------------------------------------------------------------------

CBeeProgressBar::~CBeeProgressBar()
{
	// delete PixPats if present.
	if (mForePixPat != nil)
		::DisposePixPat( mForePixPat );
	if (mBackPixPat != nil)
		::DisposePixPat( mBackPixPat );
		
	// delete GWorld if present.
	if (mGWorld != nil)
		delete mGWorld;
}


// ---------------------------------------------------------------------------------
//		¥ SetDrawOffscreen
// ---------------------------------------------------------------------------------
// create or delete GWorld after this operation if necessary.

void
CBeeProgressBar::SetDrawOffscreen(
	Boolean	inDrawOffscreen )
{
	// only modify if different.
	if (inDrawOffscreen != mDrawOffscreen)
	{
		// set variable.
		mDrawOffscreen = inDrawOffscreen;
	
		// create or delete GWorld accordingly.
		if (inDrawOffscreen)
		{
			Rect theFrame;
			CalcLocalFrameRect( theFrame );
			mGWorld = new LGWorld( theFrame );
		}
		else
		{
			delete mGWorld;
			mGWorld = nil;
		}
	}
}


// ---------------------------------------------------------------------------------
//		¥ SetValue
// ---------------------------------------------------------------------------------
// we must redraw the progress bar after a value change if it was different.

void
CBeeProgressBar::SetValue(
	SInt32	inValue )
{
	// save current value.
	SInt32 curValue = GetValue();
		
	// call LControl's inherited to change value.
	LControl::SetValue( inValue );
	
	// redraw if value is different.
	if (curValue != inValue)
	{
		if (mDrawImmediately)
			Draw( nil );
		else
			Refresh();
	}
}


// ---------------------------------------------------------------------------
//		¥ SetMaxValue													 
// ---------------------------------------------------------------------------
// we must redraw the progress bar after setting its length if it was different.

void
CBeeProgressBar::SetMaxValue(
	SInt32	inMaxValue )
{
	// save current value.
	SInt32 curMaxValue = GetMaxValue();
		
	// call LControl's inherited to change value.
	LControl::SetMaxValue( inMaxValue );
	
	// redraw if value is different.
	if (curMaxValue != inMaxValue)
	{
		if (mDrawImmediately)
			Draw( nil );
		else
			Refresh();
	}
}


// ---------------------------------------------------------------------------------
//		¥ DrawSelf
// ---------------------------------------------------------------------------------
// draw progress bar.

void
CBeeProgressBar::DrawSelf()
{
	// perform some housekeeping.
	::PenNormal();
	ApplyForeAndBackColors();

	// calculate local rect.
	Rect theRect;
	CalcLocalFrameRect( theRect );
	
	// begin offscreen drawing if necessary.
	if (mDrawOffscreen)
		mGWorld->BeginDrawing();

	// draw frame.
	if (mBackPixPat != nil)
		::FillCRect( &theRect, mBackPixPat );	// fill only if back PixPat exists.
	::FrameRect( &theRect );
	
	// draw content.
	if (mForePixPat != nil)	//...only if we have our fore PixPat.
	{
		// calculate total length of progress bar.
		short totalLength = (theRect.right - theRect.left);
			
		// calculate current length according to value.
		short length = ((float) GetValue() / (float) GetMaxValue()) * totalLength;
		if ((GetValue() == GetMaxValue()) || (length > (totalLength - 1)))
			length = totalLength - 1;
			
		// calculate the rect we need to fill.
		Rect tempRect;
		::SetRect (&tempRect, 	theRect.left + 1 + mPadding,
								theRect.top + 1 + mPadding,
								theRect.left + length - mPadding,
								theRect.bottom - 1 - mPadding);
								
		// fill rect.
		::FillCRect( &tempRect, mForePixPat );
	}
	
	// dump content of offscreen GWorld if necessary.
	if (mDrawOffscreen)
	{
		mGWorld->EndDrawing();
		::RGBForeColor( &Color_Black );
		::RGBBackColor( &Color_White );
		mGWorld->CopyImage( UQDGlobals::GetCurrentPort(), theRect );
	}
}