// =================================================================================
//	CWailSoundStream.cp					©1999, Charles Lechasseur
// =================================================================================
//
// stream class used to store sounds in Wail. this is used so that it is
// easy to change the implementation of the streams used to store sounds,
// without having to change existing code a lot.
//
// it has constructor for the uses i need in Wail, period. it's not very generic.
//
// current implementation (as of 2.5b1): with virtual streams or stream views.

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

#include "CWailSoundStream.h"

#include "CVirtualStream.h"
#include "CStreamView.h"


// ---------------------------------------------------------------------------
//	¥ CWailSoundStream							Constructor	with Handle
// ---------------------------------------------------------------------------
//	works much like LHandleStream's constructor. it "takes ownership"
//	of the handle (whatever that means).

CWailSoundStream::CWailSoundStream(
	Handle	inHandle )
	: mStream( NULL )
{
	try
	{
		// get handle size.
		SInt32 handleSize = ::GetHandleSize( inHandle );
		
		// allocate a virtual stream
		mStream = new CVirtualStream( handleSize );
		ThrowIfNil_( mStream );
		
		// store data from the handle into our private stream.
		::HLock( inHandle );
		WriteData( *inHandle, handleSize );
	}
	
	catch (...)
	{
		// whatever happens, dispose of handle.
		::DisposeHandle( inHandle );
		
		throw;	// rethrow.
	}
	
	// everything went fine. dispose of handle.
	::DisposeHandle( inHandle );
}


// ---------------------------------------------------------------------------
//	¥ CWailSoundStream							Constructor	with stream view
// ---------------------------------------------------------------------------
//	works much like CStreamView's constructor. it allocates a stream that
//  simply maps a particular view of another stream.
//
//  we do not assume ownership of the provided stream, though it must remain
//  alive while this object is.

CWailSoundStream::CWailSoundStream(
	LStream*			inStream,
	SInt32				inStartOffset,
	SInt32				inLength )
	: mStream( new CStreamView( inStream, inStartOffset, inLength ) )
{
}


// ---------------------------------------------------------------------------
//	¥ ~CWailSoundStream							Destructor	[public]
// ---------------------------------------------------------------------------

CWailSoundStream::~CWailSoundStream()
{
	if (mStream != NULL)
		delete mStream;
}


// =================================================================================
//	LStream functions
// =================================================================================
// functions related to streams - reading, writing, moving the marker, etc.
// simply calls the appropriate functions of our hidden stream.


#pragma mark --- LStream functions ---


// ---------------------------------------------------------------------------
//		¥ SetMarker
// ---------------------------------------------------------------------------

void
CWailSoundStream::SetMarker(
	SInt32		inOffset,
	EStreamFrom	inFromWhere )
{
	mStream->SetMarker( inOffset, inFromWhere );
}


// ---------------------------------------------------------------------------
//		¥ GetMarker
// ---------------------------------------------------------------------------

SInt32
CWailSoundStream::GetMarker() const
{
	return mStream->GetMarker();
}


// ---------------------------------------------------------------------------
//		¥ SetLength
// ---------------------------------------------------------------------------

void
CWailSoundStream::SetLength(
	SInt32	inLength )
{
	SignalIf_( dynamic_cast<CStreamView*>(mStream) != nil );

	mStream->SetLength( inLength );
}


// ---------------------------------------------------------------------------
//		¥ GetLength
// ---------------------------------------------------------------------------

SInt32
CWailSoundStream::GetLength() const
{
	return mStream->GetLength();
}


// ---------------------------------------------------------------------------
//		¥ PutBytes
// ---------------------------------------------------------------------------

ExceptionCode
CWailSoundStream::PutBytes(
	const void*	inBuffer,
	SInt32&		ioByteCount )
{
	SignalIf_( dynamic_cast<CStreamView*>(mStream) != nil );

	return mStream->PutBytes( inBuffer, ioByteCount );
}


// ---------------------------------------------------------------------------
//		¥ GetBytes
// ---------------------------------------------------------------------------

ExceptionCode
CWailSoundStream::GetBytes(
	void*	outBuffer,
	SInt32&	ioByteCount)
{
	return mStream->GetBytes( outBuffer, ioByteCount );
}