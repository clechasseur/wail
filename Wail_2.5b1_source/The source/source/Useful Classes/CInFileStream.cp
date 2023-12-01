// =================================================================================
//	CInFileStream.cp					©1999, Charles Lechasseur
// =================================================================================
//
// an LFileStream derivate that is used to point to some place inside a file
// as though that was the beginning of the stream (the "inset"). for example,
// you might want to write past a certain point in a file, but treat it as
// though the new part is simply a normal stream. use this class to create
// a new stream from the existing file but with an inset so that the beginning
// of the new stream points to a particular point in the file (where you want
// to write).

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

#include "CInFileStream.h"


// ---------------------------------------------------------------------------------
//		¥ CInFileStream							Constructor	[public]
// ---------------------------------------------------------------------------------

CInFileStream::CInFileStream()
	: mInset( 0 )
{
}


// ---------------------------------------------------------------------------------
//		¥ CInFileStream							Constructor	[public]
// ---------------------------------------------------------------------------------

CInFileStream::CInFileStream(
	const FSSpec	&inFileSpec,
	SInt32			inInset )
	: LFileStream( inFileSpec ),
	  mInset( inInset )
{
}


// ---------------------------------------------------------------------------------
//		¥ CInFileStream							Constructor	[public]
// ---------------------------------------------------------------------------------

CInFileStream::CInFileStream(
	AliasHandle		inAlias,
	Boolean			&outWasChanged,
	FSSpec			*inFromFile,
	SInt32			inInset )
	: LFileStream( inAlias, outWasChanged, inFromFile ),
	  mInset( inInset )
{
}


// ---------------------------------------------------------------------------
//	¥ ~CInFileStream							Destructor	[public]
// ---------------------------------------------------------------------------

CInFileStream::~CInFileStream()
{
}


// ---------------------------------------------------------------------------
//		¥ SetMarker
// ---------------------------------------------------------------------------
// if moving from start of file, add our inset to keep in sync.

void
CInFileStream::SetMarker(
	SInt32		inOffset,
	EStreamFrom	inFromWhere)
{
	Boolean movedPastBeginning = false;

	// if from start, add our inset.
	if (inFromWhere == streamFrom_Start)
		inOffset += GetInset();
	// otherwise, make sure we don't move past the beginning.
	else if (inFromWhere == streamFrom_Marker)
	{
		// check if a negative offset would take us before the beginning.
		if ((GetMarker() + inOffset) < 0)
			movedPastBeginning = true;
	}
	else if (inFromWhere == streamFrom_End)
	{
		// check if we want to go pas the beginning. offsets are positive here.
		if ((GetLength() - inOffset) < 0)
			movedPastBeginning = true;
	}
	
	// moving before the beginning of a stream is most probably a programmer error.
	// we'll signal to that extent and throw a posErr error (just like SetFPos would).
	if (movedPastBeginning)
	{
		SignalStringLiteral_( "Tried to move before inset" );
		Throw_( posErr );
	}
	
	// if all went fine, call parent's routine.
	LFileStream::SetMarker( inOffset, inFromWhere );
}


// ---------------------------------------------------------------------------
//		¥ GetMarker
// ---------------------------------------------------------------------------
// add our inset to the result to fake a marker that starts from the inset position.

SInt32
CInFileStream::GetMarker() const
{
	return LFileStream::GetMarker() - GetInset();
}


// ---------------------------------------------------------------------------
//		¥ SetLength
// ---------------------------------------------------------------------------
// add our inset, to make sure the file has the correct size.

void
CInFileStream::SetLength(
	SInt32	inLength)
{
	// sanity check: a negative inLength would make LFileStream crash, but maybe
	// not us. i don't really know what to do with this case - does SetEOF
	// remaps all negative entries to 0? i'll signal here and set it to 0 in that case.
	if (inLength < 0)
	{
		SignalStringLiteral_( "inLength is negative" );
		inLength = 0;
	}

	LFileStream::SetLength( inLength + GetInset() );
}


// ---------------------------------------------------------------------------
//		¥ GetLength
// ---------------------------------------------------------------------------
// substract our inset from the length to return the length of the part
// that's accessible via this stream.

SInt32
CInFileStream::GetLength() const
{
	return LFileStream::GetLength() - GetInset();
}