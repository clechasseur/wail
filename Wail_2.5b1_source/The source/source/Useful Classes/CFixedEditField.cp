// =================================================================================
//	CFixedEditField.cp					©1999, Charles Lechasseur
// =================================================================================
//
// An EditField which uses Fixed values for Get/SetValue.
//
// Also inherits from CBroadcastingEditField to be able to broadcast messages
// in various circumstances.

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

#include "CFixedEditField.h"


// ---------------------------------------------------------------------------
//	¥ CFixedEditField						Constructor	[public]
// ---------------------------------------------------------------------------

CFixedEditField::CFixedEditField()
{
}


// ---------------------------------------------------------------------------
//	¥ CFixedEditField					Stream Constructor	[public]
// ---------------------------------------------------------------------------

CFixedEditField::CFixedEditField(
	LStream *inStream )
	: CBroadcastingEditField( inStream )
{
}


// ---------------------------------------------------------------------------
//	¥ CFixedEditField						Copy Constructor	[public]
// ---------------------------------------------------------------------------

CFixedEditField::CFixedEditField(
	const CFixedEditField& inOriginal )
	: CBroadcastingEditField( inOriginal )
{
}


// ---------------------------------------------------------------------------
//	¥ ~CFixedEditField							Destructor	[public]
// ---------------------------------------------------------------------------

CFixedEditField::~CFixedEditField()
{
}


// ---------------------------------------------------------------------------
//		¥ GetValue
// ---------------------------------------------------------------------------
// Returns the value as a Fixed number.

SInt32
CFixedEditField::GetValue() const
{
	// get text.
	LStr255 theText;
	GetDescriptor( theText );
	
	// turn into long double.
	long double theDouble = theText;
	
	// turn into fixed.
	Fixed theFixed = X2Fix( theDouble );
	
	// return.
	return (SInt32) theFixed;
}


// ---------------------------------------------------------------------------
//		¥ SetValue
// ---------------------------------------------------------------------------
// given a Fixed number, sets the text of the edit field.

void
CFixedEditField::SetValue(
	SInt32	inValue )
{
	// turn value into long double.
	long double theDouble = Fix2X( (Fixed) inValue );
	
	// fetch system format table.
	NumberParts theNumberParts;
	LString::GetSystemNumberParts( theNumberParts );
	
	// make out format string.
	LStr255 theFormatString = "\p###";
	theFormatString += theNumberParts.data[tokThousands - 1].a[1];
	theFormatString += "\p##0";
	theFormatString += theNumberParts.data[tokDecPoint - 1].a[1];
	theFormatString += "\p000";
	
	// turn into string.
	LStr255 theText( theDouble, theFormatString );
	
	// set text.
	SetDescriptor( theText );
}