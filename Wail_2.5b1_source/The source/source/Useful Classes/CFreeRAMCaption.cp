// =================================================================================
//	CFreeRAMCaption.cp					©1999, Charles Lechasseur
// =================================================================================
//
// Caption that lists the amount of free RAM.

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

#include "CFreeRAMCaption.h"


// ---------------------------------------------------------------------------
//	¥ CFreeRAMCaption							Constructor	[public]
// ---------------------------------------------------------------------------

CFreeRAMCaption::CFreeRAMCaption(
	EFindRAMLeft	inHowToFindRAMLeft )
{
	InitRAMCaption( inHowToFindRAMLeft );
}


// ---------------------------------------------------------------------------
//	¥ CFreeRAMCaption						Copy Constructor	[public]
// ---------------------------------------------------------------------------

CFreeRAMCaption::CFreeRAMCaption(
	const CFreeRAMCaption& inOriginal )
	: LCaption( inOriginal )
{
	// no need to re-init. we have the free RAM from the other caption.
}


// ---------------------------------------------------------------------------------
//	¥ CFreeRAMCaption						Parameterized Constructor	[public]
// ---------------------------------------------------------------------------------

CFreeRAMCaption::CFreeRAMCaption(
	const SPaneInfo	&inPaneInfo,
	ConstStringPtr	inString,
	ResIDT			inTextTraitsID,
	EFindRAMLeft	inHowToFindRAMLeft )
	: LCaption( inPaneInfo, inString, inTextTraitsID )
{
	InitRAMCaption( inHowToFindRAMLeft );
}


// ---------------------------------------------------------------------------
//	¥ CFreeRAMCaption					Stream Constructor	[public]
// ---------------------------------------------------------------------------

CFreeRAMCaption::CFreeRAMCaption(
	LStream *inStream )
	: LCaption( inStream )
{
	SInt16 howToFindRAMLeft;
	*inStream >> howToFindRAMLeft;

	InitRAMCaption( (EFindRAMLeft) howToFindRAMLeft );
}


// ---------------------------------------------------------------------------
//	¥ ~CFreeRAMCaption							Destructor	[public]
// ---------------------------------------------------------------------------

CFreeRAMCaption::~CFreeRAMCaption()
{
}


// ---------------------------------------------------------------------------------
//		¥ InitRAMCaption
// ---------------------------------------------------------------------------------
// initializes the caption so it displays the amount of free RAM.

void
CFreeRAMCaption::InitRAMCaption(
	EFindRAMLeft	inHowToFindRAMLeft )
{
	// check RAM left (in kilobytes).
	SInt32 ramLeft;
	switch (inHowToFindRAMLeft)
	{
		case ramLeft_FreeMem:
			ramLeft = ::FreeMem() / 1024L;
			break;
			
		case ramLeft_MaxBlock:
			ramLeft = ::MaxBlock() / 1024L;
			break;
			
		default:	// incorrect way of specifying RAM left!
			SignalStringLiteral_( "Incorrect way of calculating free RAM." );
			ramLeft = 0;
			break;
	}

	// add ' k' after that and set it as our string.
	LStr255 theString( ramLeft );
	theString += "\p k";
	SetDescriptor( theString );
}