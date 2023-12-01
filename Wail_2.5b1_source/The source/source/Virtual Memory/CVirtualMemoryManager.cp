// =================================================================================
//	CVirtualMemoryManager.cp					©1999, Charles Lechasseur
// =================================================================================
//
// class responsible for creating and managing "virtual streams".
//
// a virtual stream is a stream that can exist in different formats.
// a "format" is a way of storing the stream's data. it could be on disk,
// in RAM, or any other way you could think of. it is called "virtual" because
// of its obvious resemblance to Virtual RAM, but it could be used for other
// purposes also.
//
// so how does this virtual streaming all works, anyway?
//
// at the very beginning, you must initialize the virtual memory
// system. this is done with the UVirtualMemory class. optionally,
// you can specify, when doing this init, what will be the *default manager*
// for further virtual streams to be created. (see below).
//
// depending on the type of virtual memory manager you're using, you might
// have to call other initialization routines. these would logically be specified
// in their respective sub-classes of CVirtualMemoryManager.
//
// the second step is to create a stream of type CVirtualStream. each object
// of this class needs to have a parent virtual memory manager object. if it
// doesn't get one, it uses the default one passed to the init above.
//
// the CVirtualStream class is simply a "stub" class that asks its virtual memory
// manager to perform all its stream operations. upon creating the CVirtualStream,
// the object asks its virtual memory manager to allocate a "private stream". if
// this works, it is then passed an opaque reference to that stream. whenever a
// stream function is called for the CVirtualStream, it simply calls the appropriate
// function of its virtual memory manager, passing it its private reference.
// thus the virtual memory manager is responsible for all stream-based operations
// for the streams it creates.
//
// the kind of info stored in the stream reference is all up to the virtual
// memory manager to decide. it might a pointer to something, or an index, whatever.
// the type VirtualStreamRefT is defined as a signed 32-bit number.
//
// a typical use of a virtual memory manager is demonstrated in the
// CBasicVirtualMemoryManager class, that implements a basic virtual memory system
// that's suitable for storing streams that won't change size once allocated.

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

#include "CVirtualMemoryManager.h"


#pragma mark --- Initialization/Destruction ---

// ---------------------------------------------------------------------------
//	¥ CVirtualMemoryManager
// ---------------------------------------------------------------------------
// initialization of the virtual memory manager.
// subclasses must provide their own constructors since this one does nothing.

CVirtualMemoryManager::CVirtualMemoryManager()
{
}


// ---------------------------------------------------------------------------------
//		¥ CVirtualMemoryManager
// ---------------------------------------------------------------------------------
// copy constructor.
// a virtual memory manager can have a copy constructor - it is up to you.
// in any case, we provide this stub copy constructor here so that you can
// call it if needed.

CVirtualMemoryManager::CVirtualMemoryManager(
	const CVirtualMemoryManager&	inOriginal )
{
#pragma unused( inOriginal )
}


// ---------------------------------------------------------------------------
//	¥ ~CVirtualMemoryManager
// ---------------------------------------------------------------------------
// disposal of any allocated resources.
// destructors of subclasses are responsible for deallocating all allocated
// resources. this includes ALL STREAMS created! is it a mistake to deallocate
// a virtual memory manager BEFORE deallocating all streams it created.
// typically you'll want to destroy it at the very end of the application.

CVirtualMemoryManager::~CVirtualMemoryManager()
{
}


#pragma mark --- Allocation/Deallocation of Streams ---


// ---------------------------------------------------------------------------------
//		¥ AllocateStream
// ---------------------------------------------------------------------------------
// allocates a stream and returns a reference to it.
// inSize is the desired size of the stream to be created.
// subclasses must override to allow stream allocation.

VirtualStreamRefT
CVirtualMemoryManager::AllocateStream(
	SInt32	inSize )
{
#pragma unused( inSize )

	SignalStringLiteral_( "AllocateStream not overridden." );
	
	return virtualStreamRef_NULL;
}


// ---------------------------------------------------------------------------------
//		¥ DeallocateStream
// ---------------------------------------------------------------------------------
// given a reference to a private stream, deallocates all storage for that stream.
// subclasses must override to allow deallocation of streams.

void
CVirtualMemoryManager::DeallocateStream(
	VirtualStreamRefT	inStreamRef )
{
#pragma unused( inStreamRef )

	SignalStringLiteral_( "DeallocateStream not overridden." );
}


// =================================================================================
//	LStream Functions
// =================================================================================
// these functions are similar to those of LStream, except that they accept
// an extra parameter - a reference to a private stream that the virtual
// memory manager has allocated. this is called automatically by a CVirtualStream
// for every stream-based operation it encounters. it is the responsability of
// the virtual memory manager to implement all stream operations for the stream
// it creates. refer to LStream for a description of each function.


#pragma mark --- LStream functions ---


// ---------------------------------------------------------------------------
//		¥ SetMarker
// ---------------------------------------------------------------------------

void
CVirtualMemoryManager::SetMarker(
	VirtualStreamRefT	inStreamRef,
	SInt32				inOffset,
	EStreamFrom			inFromWhere )
{
#pragma unused( inStreamRef, inOffset, inFromWhere )

	SignalStringLiteral_( "SetMarker nor overridden." );
}


// ---------------------------------------------------------------------------
//		¥ GetMarker
// ---------------------------------------------------------------------------

SInt32
CVirtualMemoryManager::GetMarker(
	VirtualStreamRefT	inStreamRef ) const
{
#pragma unused( inStreamRef )

	SignalStringLiteral_( "GetMarker not overridden." );
	
	return 0;
}


// ---------------------------------------------------------------------------
//		¥ SetLength
// ---------------------------------------------------------------------------

void
CVirtualMemoryManager::SetLength(
	VirtualStreamRefT	inStreamRef,
	SInt32				inLength )
{
#pragma unused( inStreamRef, inLength )

	SignalStringLiteral_( "SetLength not overridden." );
}


// ---------------------------------------------------------------------------
//		¥ GetLength
// ---------------------------------------------------------------------------

SInt32
CVirtualMemoryManager::GetLength(
	VirtualStreamRefT	inStreamRef ) const
{
#pragma unused( inStreamRef )

	SignalStringLiteral_( "GetLength not overridden." );
	
	return 0;
}


// ---------------------------------------------------------------------------
//		¥ PutBytes
// ---------------------------------------------------------------------------

ExceptionCode
CVirtualMemoryManager::PutBytes(
	VirtualStreamRefT	inStreamRef,
	const void*			inBuffer,
	SInt32&				ioByteCount )
{
#pragma unused( inStreamRef, inBuffer )

	SignalStringLiteral_( "PutBytes not overridden." );
	
	ioByteCount = 0;
	return writErr;	// same as LStream::PutBytes.
}


// ---------------------------------------------------------------------------
//		¥ GetBytes
// ---------------------------------------------------------------------------

ExceptionCode
CVirtualMemoryManager::GetBytes(
	VirtualStreamRefT	inStreamRef,
	void*				outBuffer,
	SInt32&				ioByteCount )
{
#pragma unused( inStreamRef, outBuffer )

	SignalStringLiteral_( "GetBytes not overridden." );
	
	ioByteCount = 0;
	return readErr;	// same as LStream::GetBytes.
}


// =================================================================================
//	Buffer accessors
// =================================================================================
// in the context of virtual memory, it is not always known where the content of
// a stream lies. but it might be in RAM. if it is, it might speed things up quite
// a bit when copying data. because of this, these buffer accessors are provided.
//
// if it is possible to get a hold of a pointer pointing to the data stored in a
// private stream, CanGetBuffer will return true for that stream; otherwise it returns
// false. if it returns true, it is then possible to get a const pointer to the
// stream data with the GetBuffer function.
//
// this is not always supported by every virtual memory manager. CVirtualStream uses
// those functions a bit (for the copy constructor, for example), but it is unsafe to
// assume you can always get the buffer for a particular stream. always call
// CanGetBuffer before you try to access the buffer.


#pragma mark --- Buffer accessors ---


// ---------------------------------------------------------------------------------
//		¥ CanGetBuffer
// ---------------------------------------------------------------------------------
// returns true if it's possible to get a hold of a buffer to the private stream's
// data. note: do not attempt to copy things into memory if this is called! the
// purpose is to get the buffer *if it is already in RAM*. if not, the user
// must fall back on the GetBytes method.

Boolean
CVirtualMemoryManager::CanGetBuffer(
	VirtualStreamRefT	inStreamRef ) const
{
#pragma unused( inStreamRef )

	return false;	// unsupported by default.
}


// ---------------------------------------------------------------------------------
//		¥ GetBuffer
// ---------------------------------------------------------------------------------
// given a reference to a private stream whose buffer is accessible because it
// is already in RAM, returns a const pointer to that buffer. if for some reason
// memory is not available, returns NULL.

const void*
CVirtualMemoryManager::GetBuffer(
	VirtualStreamRefT	inStreamRef ) const
{
	// wrong to call this if CanGetBuffer returns false.
	SignalIfNot_( CanGetBuffer( inStreamRef ) );

	return NULL;	// unsupported by default.
}


// =================================================================================
//	Utility functions
// =================================================================================
// these functions are utility functions that might be useful to you.
// you are free to implement them in your virtual memory manager if desired.
// they are not essential and are not called by any other pre-built classes.


#pragma mark --- Utility functions ---


// ---------------------------------------------------------------------------------
//		¥ CanGetStreamsCount
// ---------------------------------------------------------------------------------
// returns true if it is possible for the user to know how many streams are currently
// allocated by the virtual memory manager.

Boolean
CVirtualMemoryManager::CanGetStreamsCount() const
{
	return false;	// unsupported by default.
}


// ---------------------------------------------------------------------------------
//		¥ GetStreamsCount
// ---------------------------------------------------------------------------------
// returns the number of streams currently allocated by this virtual memory manager.

SInt32
CVirtualMemoryManager::GetStreamsCount() const
{
	return 0;	// unsupported by default.
}
		

// ---------------------------------------------------------------------------------
//		¥ CanGetIthStream
// ---------------------------------------------------------------------------------
// returns true if we can fetch the reference to the ith stream allocated by this
// virtual memory manager.

Boolean
CVirtualMemoryManager::CanGetIthStream() const
{
	return false;	// unsupported by default.
}


// ---------------------------------------------------------------------------------
//		¥ GetIndexOfFirstStream
// ---------------------------------------------------------------------------------
// returns the index of the first stream allocated by this virtual memory manager.
// useful in order to know where to start when calling GetIthStream (see below).

SInt32
CVirtualMemoryManager::GetIndexOfFirstStream() const
{
	return 1;	// i like 1-based arrays. how about you? :)
}


// ---------------------------------------------------------------------------------
//		¥ GetIthStream
// ---------------------------------------------------------------------------------
// returns the reference to the ith stream allocated by this virtual memory manager.
// the starting index is yours to decide.
// note: if the index is invalid, return virtualStreamRef_NULL.

VirtualStreamRefT
CVirtualMemoryManager::GetIthStream(
	SInt32	inStreamIndex ) const
{
#pragma unused( inStreamIndex )

	return virtualStreamRef_NULL;	// unsupported by default.
}