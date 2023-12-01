// =================================================================================
//	CVirtualStream.cp					©1999, Charles Lechasseur
// =================================================================================
//
// a stream stored in virtual memory.
//
// the "format" of a virtual stream is always unknown. it might be a Handle stream,
// or it might be stored somewhere on disk, or in a compressed memory buffer... you
// never know.
//
// when created, a CVirtualStream needs to have a virtual memory manager to use
// when it needs to call stream routines. if you don't supply one, it will use
// the default one supplied to the Initialize routine (see UVirtualMemory.cp).

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


#include "CVirtualStream.h"

#include "UVirtualMemory.h"
#include "UStreamUtils.h"


#pragma mark --- Constructors/Destructor ---


// ---------------------------------------------------------------------------
//	¥ CVirtualStream					Default Constructor	[public]
// ---------------------------------------------------------------------------
// allocates a 0-bytes sized stream.

CVirtualStream::CVirtualStream(
	CVirtualMemoryManager*	inManager )
	: mManager( inManager ),
	  mStreamRef( virtualStreamRef_NULL )
{
	// if we don't have a manager, use the default one.
	if (mManager == NULL)
		mManager = UVirtualMemory::GetDefaultManager();
		
	// if we don't have one now, BOOM!
	ThrowIfNULL_(mManager);
	
	// allocate a 0-bytes stream.
	mStreamRef = mManager->AllocateStream( 0L );
	
	// if we can't allocate the stream, throw a memFullErr exception.
	if (mStreamRef == virtualStreamRef_NULL)
		throw (ExceptionCode) (memFullErr);
}


// ---------------------------------------------------------------------------
//	¥ CVirtualStream					Parameterized Constructor	[public]
// ---------------------------------------------------------------------------
// allocates a stream with a size of inSize bytes.
// doesn't initialize those bytes to anything.

CVirtualStream::CVirtualStream(
	SInt32					inSize,
	CVirtualMemoryManager*	inManager )
	: mManager( inManager ),
	  mStreamRef( virtualStreamRef_NULL )
{
	// if we don't have a manager, use the default one.
	if (mManager == NULL)
		mManager = UVirtualMemory::GetDefaultManager();
		
	// if we don't have one now, BOOM!
	ThrowIfNULL_(mManager);

	// allocate a stream with inSize bytes.
	mStreamRef = mManager->AllocateStream( inSize );
	
	// if we can't allocate the stream, throw a memFullErr exception.
	if (mStreamRef == virtualStreamRef_NULL)
		throw (ExceptionCode) (memFullErr);
}


// ---------------------------------------------------------------------------
//	¥ CVirtualStream						Copy Constructor	[public]
// ---------------------------------------------------------------------------

CVirtualStream::CVirtualStream(
	const CVirtualStream& inOriginal )
	: mManager( inOriginal.GetManager() ),
	  mStreamRef( virtualStreamRef_NULL )
{
	// un-const this ref. hey, we won't touch it, i promise. :)
	CVirtualStream& theOriginal = const_cast<CVirtualStream&> (inOriginal);

	// allocate a stream containing as many bytes as the original.
	mStreamRef = mManager->AllocateStream( theOriginal.GetLength() );
	
	// if we can't allocate the stream, throw a memFullErr exception.
	if (mStreamRef == virtualStreamRef_NULL)
		throw (ExceptionCode) (memFullErr);
		
	// now we must copy everything from the original into our stream.
	// if we could get its buffer, it'd great faster.
	if (theOriginal.CanGetBuffer())
	{
		// hey, we can get its buffer, cool!
		StMarkerSaver( *this, 0L, streamFrom_Start );
		WriteBlock( theOriginal.GetBuffer(), theOriginal.GetLength() );
	}
	else
	{
		// can't get its buffer. damn. ok, allocate a temp buffer to transfer the data.
		try
		{
			StPointerBlock theTempBuffer( theOriginal.GetLength() );
			
			StMarkerSaver( theOriginal, 0L, streamFrom_Start );	// reset marker to start of stream.
			theOriginal.ReadBlock( (Ptr) theTempBuffer, theOriginal.GetLength() );
			
			// write to our stream.
			StMarkerSaver( *this, 0L, streamFrom_Start );
			WriteBlock( (Ptr) theTempBuffer, theOriginal.GetLength() );
		}
		
		catch (...)
		{
			// a problem occured. try copying it byte per byte.
			UInt8 the1byteBuffer;
			
			StMarkerSaver( *this, 0L, streamFrom_Start );
			StMarkerSaver( theOriginal, 0L, streamFrom_Start );	// starting at the beginning...
			while (!theOriginal.AtEnd())						// ...we loop 'till the end...
			{
				theOriginal >> the1byteBuffer;					// ...and copy each byte.
				*this << the1byteBuffer;
			}
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ ~CVirtualStream							Destructor	[public]
// ---------------------------------------------------------------------------
// deallocates the stream.

CVirtualStream::~CVirtualStream()
{
	try
	{
		mManager->DeallocateStream( mStreamRef );
	}
	
	catch (...)
	{
		// we can't throw. let's at least signal the error.
		SignalStringLiteral_( "~CVirtualStream catched an exception." );
	}
}


// =================================================================================
//	LStream functions
// =================================================================================
// functions related to streams - reading, writing, moving the marker, etc.
// simply calls the appropriate functions of its virtual memory manager.
// a virtual stream never implements stream behavior by itself. the virtual
// memory manager is responsible for all this.


#pragma mark --- LStream functions ---


// ---------------------------------------------------------------------------
//		¥ SetMarker
// ---------------------------------------------------------------------------

void
CVirtualStream::SetMarker(
	SInt32		inOffset,
	EStreamFrom	inFromWhere )
{
	mManager->SetMarker( mStreamRef, inOffset, inFromWhere );
}


// ---------------------------------------------------------------------------
//		¥ GetMarker
// ---------------------------------------------------------------------------

SInt32
CVirtualStream::GetMarker() const
{
	return mManager->GetMarker( mStreamRef );
}


// ---------------------------------------------------------------------------
//		¥ SetLength
// ---------------------------------------------------------------------------

void
CVirtualStream::SetLength(
	SInt32	inLength )
{
	mManager->SetLength( mStreamRef, inLength );
}


// ---------------------------------------------------------------------------
//		¥ GetLength
// ---------------------------------------------------------------------------

SInt32
CVirtualStream::GetLength() const
{
	return mManager->GetLength( mStreamRef );
}


// ---------------------------------------------------------------------------
//		¥ PutBytes
// ---------------------------------------------------------------------------

ExceptionCode
CVirtualStream::PutBytes(
	const void*	inBuffer,
	SInt32&		ioByteCount )
{
	return mManager->PutBytes( mStreamRef, inBuffer, ioByteCount );
}


// ---------------------------------------------------------------------------
//		¥ GetBytes
// ---------------------------------------------------------------------------

ExceptionCode
CVirtualStream::GetBytes(
	void*	outBuffer,
	SInt32&	ioByteCount)
{
	return mManager->GetBytes( mStreamRef, outBuffer, ioByteCount );
}


// =================================================================================
//	Buffer accessors
// =================================================================================
// functions for accessing the buffer of a stream, if possible.
// simply calls the appropriate functions of its virtual memory manager.
// see CVirtualMemoryManager.cp for an explanation of these functions.


#pragma mark --- Buffer accessors ---


// ---------------------------------------------------------------------------------
//		¥ CanGetBuffer
// ---------------------------------------------------------------------------------

Boolean
CVirtualStream::CanGetBuffer() const
{
	return mManager->CanGetBuffer( mStreamRef );
}


// ---------------------------------------------------------------------------------
//		¥ GetBuffer
// ---------------------------------------------------------------------------------

const void*
CVirtualStream::GetBuffer() const
{
	return mManager->GetBuffer( mStreamRef );
}