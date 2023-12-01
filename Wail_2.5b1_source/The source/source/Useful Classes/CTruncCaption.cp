// =================================================================================
//	CTruncCaption.cp					©2003, Charles Lechasseur
// =================================================================================
//
// A caption that truncates its text when drawing it on screen.

/* Copyright (c) 2003, Charles Lechasseur
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

#include "CTruncCaption.h"


// ---------------------------------------------------------------------------
//	¥ CTruncCaption						Constructor	[public]
// ---------------------------------------------------------------------------

CTruncCaption::CTruncCaption()
	: mTruncWhere( truncEnd )
{
}


// ---------------------------------------------------------------------------
//	¥ CTruncCaption						Parametrized Constructor [public]
// ---------------------------------------------------------------------------

CTruncCaption::CTruncCaption(
	const SPaneInfo&		inPaneInfo,
	ConstStringPtr			inString,
	ResIDT					inTextTraitsID,
	TruncCode				inTruncWhere )
	: LCaption( inPaneInfo, inString, inTextTraitsID ),
	  mTruncWhere( inTruncWhere )
{
}


// ---------------------------------------------------------------------------
//	¥ CTruncCaption						Stream Constructor	[public]
// ---------------------------------------------------------------------------

CTruncCaption::CTruncCaption(
	LStream*	inStream )
	: LCaption( inStream )
{
	*inStream >> mTruncWhere;
}


// ---------------------------------------------------------------------------
//	¥ CTruncCaption						Copy Constructor	[public]
// ---------------------------------------------------------------------------

CTruncCaption::CTruncCaption(
	const CTruncCaption&	inOriginal )
	: LCaption( inOriginal ),
	  mTruncWhere( inOriginal.GetTruncWhere() )
{
}


// ---------------------------------------------------------------------------
//		¥ DrawSelf
// ---------------------------------------------------------------------------
// truncates the caption's text and draws it.
//
// mostly copied from LCaption::DrawSelf.

void
CTruncCaption::DrawSelf()
{
	// get caption text.
	Str255 theText;
	GetDescriptor( theText );
	
	// get pane frame.
	Rect theFrame;
	CalcLocalFrameRect( theFrame );
	
	// set our text traits.
	SInt16 theJust = UTextTraits::SetPortTextTraits( GetTextTraitsID() );
	
	// set colors.
	RGBColor theTextColor;
	::GetForeColor( &theTextColor );
	ApplyForeAndBackColors();
	::RGBForeColor( &theTextColor );
	
	// now that text is ready for drawing, truncate it.
	::TruncString( theFrame.right - theFrame.left, theText, GetTruncWhere() );

	// got proper text, now draw it.
	UTextDrawing::DrawWithJustification( reinterpret_cast<Ptr>(&theText[1]), theText[0],
		theFrame, theJust, true );
}
