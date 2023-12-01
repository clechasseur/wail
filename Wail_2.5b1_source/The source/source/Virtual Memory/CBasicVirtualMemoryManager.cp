// =================================================================================
//	CBasicVirtualMemoryManager.cp					©1999, Charles Lechasseur
// =================================================================================
//
// a concrete implementation of a basic virtual memory manager.
//
// when created, the basic virtual memory manager will accept as parameter
// the amount of RAM it *cannot* access. all other RAM is accessible.
//
// then, when it allocates a stream, it checks if allocating it as a LHandleStream
// would make it go over his limit. if it does, it allocates it as a temp file
// linked to an CTempFileStream instead. (the CTempFileStream class is simply
// a subset of the LFileStream class that allows the file to be automatically created
// as a temp file.)
//
// note that once a stream is allocated, it cannot change. it cannot go back in RAM
// if at some point there is room, nor can it be dumped to disk if RAM gets low.
//
// this implementation might be suitable if you need a very basic scratch disk method
// and if your stream are not likely to change size by a large amount.

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

#include "CBasicVirtualMemoryManager.h"

#include "CTempFileStream.h"


#pragma mark --- Initialization/Destruction ---

// ---------------------------------------------------------------------------
//	¥ CBasicVirtualMemoryManager
// ---------------------------------------------------------------------------
// initialization of the virtual memory manager.
//
// inMemProtected is the amount of RAM that the virtual memory manager
// cannot use to allocate RAM-based streams.

CBasicVirtualMemoryManager::CBasicVirtualMemoryManager(
	SInt32	inMemProtected )
	: mMemProtected( inMemProtected )
{
}


// ---------------------------------------------------------------------------
//	¥ ~CBasicVirtualMemoryManager
// ---------------------------------------------------------------------------
// disposal of any allocated resources.
//
// disposes of all allocated streams.

CBasicVirtualMemoryManager::~CBasicVirtualMemoryManager()
{
	// dispose of all allocated streams.
	while (mAllocatedStreams.GetCount() > 0)		// while the array is not empty...
	{
		LStream* theStream = mAllocatedStreams[1];	// get first stream...
		mAllocatedStreams.Remove( theStream );		// remove it from the array...
		delete theStream;							// ...and delete it.
	}
}


#pragma mark --- Allocation/Deallocation of Streams ---


// ---------------------------------------------------------------------------------
//		¥ AllocateStream
// ---------------------------------------------------------------------------------
// allocates a stream and returns a reference to it.
// inSize is the desired size of the stream to be created.

VirtualStreamRefT
CBasicVirtualMemoryManager::AllocateStream(
	SInt32	inSize )
{
	LStream* theStream = NULL;

	// try allocating it in RAM.
	try
	{
		// let's check amount of free RAM.
		SInt32 ramLeft = ::MaxBlock();
		
		// if we're short of RAM, throw an exception that will be catched below.
		if (ramLeft <= (mMemProtected + inSize + sizeof(LHandleStream)))
			throw memFullErr;
			
		// try allocating a handle of desired size. this might fail too. who knows.
		Handle theHandle = ::NewHandle( inSize );
		if (theHandle == nil)
			throw ::MemError();
			
		// it worked. create a stream.
		theStream = new LHandleStream( theHandle );	// stream now "owns" the handle.
	}
	
	catch (...)
	{
		// allocation in RAM failed for some reason. try allocating on disk.
		theStream = new CTempFileStream( inSize );
	}
	
	// if the stream is null here, big oops.
	ThrowIfNULL_( theStream );
	
	// store the stream in our array.
	mAllocatedStreams.AddItem( theStream );
	
	// return it typecasted to a ref.
	return (VirtualStreamRefT) theStream;
}


// ---------------------------------------------------------------------------------
//		¥ DeallocateStream
// ---------------------------------------------------------------------------------
// given a reference to a private stream, deallocates all storage for that stream.

void
CBasicVirtualMemoryManager::DeallocateStream(
	VirtualStreamRefT	inStreamRef )
{
	// ref is simply the stream to be deallocated.
	LStream* theStream = (LStream*) inStreamRef;
	
	// remove it from our array and delete it.
	mAllocatedStreams.Remove( theStream );
	delete theStream;
}


// =================================================================================
//	LStream Functions
// =================================================================================
// all stream functions simply call the function for the stream
// passed as parameter. (the ref is really a LStream*.)


#pragma mark --- LStream functions ---


// ---------------------------------------------------------------------------
//		¥ SetMarker
// ---------------------------------------------------------------------------

void
CBasicVirtualMemoryManager::SetMarker(
	VirtualStreamRefT	inStreamRef,
	SInt32				inOffset,
	EStreamFrom			inFromWhere )
{
	((LStream*) inStreamRef)->SetMarker( inOffset, inFromWhere );
}


// ---------------------------------------------------------------------------
//		¥ GetMarker
// ---------------------------------------------------------------------------

SInt32
CBasicVirtualMemoryManager::GetMarker(
	VirtualStreamRefT	inStreamRef ) const
{
	return ((LStream*) inStreamRef)->GetMarker();
}


// ---------------------------------------------------------------------------
//		¥ SetLength
// ---------------------------------------------------------------------------

void
CBasicVirtualMemoryManager::SetLength(
	VirtualStreamRefT	inStreamRef,
	SInt32				inLength )
{
	((LStream*) inStreamRef)->SetLength( inLength );
}


// ---------------------------------------------------------------------------
//		¥ GetLength
// ---------------------------------------------------------------------------

SInt32
CBasicVirtualMemoryManager::GetLength(
	VirtualStreamRefT	inStreamRef ) const
{
	return ((LStream*) inStreamRef)->GetLength();
}


// ---------------------------------------------------------------------------
//		¥ PutBytes
// ---------------------------------------------------------------------------

ExceptionCode
CBasicVirtualMemoryManager::PutBytes(
	VirtualStreamRefT	inStreamRef,
	const void*			inBuffer,
	SInt32&				ioByteCount )
{
	return ((LStream*) inStreamRef)->PutBytes( inBuffer, ioByteCount );
}


// ---------------------------------------------------------------------------
//		¥ GetBytes
// ---------------------------------------------------------------------------

ExceptionCode
CBasicVirtualMemoryManager::GetBytes(
	VirtualStreamRefT	inStreamRef,
	void*				outBuffer,
	SInt32&				ioByteCount )
{
	return ((LStream*) inStreamRef)->GetBytes( outBuffer, ioByteCount );
}