// ===========================================================================
//	CResourceStream.h			   ©1999, Charles Lechasseur
// ===========================================================================
//
//	A Stream whose bytes are in a resource on disk.

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

#ifdef PowerPlant_PCH
#include PowerPlant_PCH
#endif

#include "CResourceStream.h"


// ---------------------------------------------------------------------------
//	¥ CResourceStream							Default Constructor		  [public]
// ---------------------------------------------------------------------------

CResourceStream::CResourceStream()
{
	mMacResourceH = nil;						// No data yet
}


// ---------------------------------------------------------------------------
//	¥ CResourceStream(Handle)					Parameterized Constructor [public]
// ---------------------------------------------------------------------------
//	Construct from an existing resource.
//
//	The CResourceStream object assumes ownership of the resource. Make sure
//	the resource is not already loaded in memory. If it is, call WriteResource to
//	write any changes to disk, then call ReleaseResource to release it, prior
//	to calling this. If you want to load the resource yourself, call ReleaseDataHandle.
//
//	inCurrentForkOnly specifies if we must look in the current resource file only,
//	or in all resource files. (default: false)

CResourceStream::CResourceStream(
	ResType			inMacResourceType,
	ResIDT			inMacResourceID,
	Boolean			inCurrentForkOnly)
{
	OSErr 		err;

	mMacResourceType = inMacResourceType;
	mMacResourceID = inMacResourceID;
	
	::SetResLoad( false );
	ThrowIfResError_();
	
	// get the resource handle.
	if (inCurrentForkOnly) {
		mMacResourceH = ::Get1Resource( mMacResourceType, mMacResourceID );
	} else {
		mMacResourceH = ::GetResource( mMacResourceType, mMacResourceID );
	}
	err = ::ResError();
	
	::SetResLoad( true );
	ThrowIfResError_();
	
	if ((err == noErr) && (mMacResourceH == nil))
		err = resNotFound;
	ThrowIfOSErr_( err );
	
	// get resource length.
	LStream::SetLength( ::GetResourceSizeOnDisk( mMacResourceH ) );
	ThrowIfResError_();
}


// ---------------------------------------------------------------------------
//	¥ CResourceStream(Handle)					Parameterized Constructor [public]
// ---------------------------------------------------------------------------
//	Construct from a new resource.
//
//	The CResourceStream object creates the resource on disk and assumes ownership
//	of its handle. If you want to load the resource yourself, call ReleaseDataHandle.

CResourceStream::CResourceStream(
	ResType			inMacResourceType,
	ResIDT			inMacResourceID,
	ConstStringPtr	inMacResourceName)
{
	OSErr 		err;

	mMacResourceType = inMacResourceType;
	mMacResourceID = inMacResourceID;
	
	// we must create an empty resource on disk.
	Handle macH = ::NewHandle( 0 );
	ThrowIfNil_( macH );
	::AddResource( macH, mMacResourceType, mMacResourceID, inMacResourceName );
	ThrowIfResError_();
	::ReleaseResource( macH );
	ThrowIfResError_();
	
	// now we must get its handle.
	::SetResLoad( false );
	ThrowIfResError_();
	
	mMacResourceH = ::Get1Resource( mMacResourceType, mMacResourceID );
	err = ::ResError();
	
	::SetResLoad( true );
	ThrowIfResError_();
	
	if ((err == noErr) && (mMacResourceH == nil))
		err = resNotFound;
	ThrowIfOSErr_( err );
	
	// set length.
	LStream::SetLength( 0 );
}


// ---------------------------------------------------------------------------
//	¥ ~CResourceStream						Destructor				  [public]
// ---------------------------------------------------------------------------

CResourceStream::~CResourceStream()
{
	if (mMacResourceH != nil) {
		::ReleaseResource(mMacResourceH);
		//ThrowIfResError_();
			// do not throw out of a destructor. (not a good ideaª)
	}
}


// ---------------------------------------------------------------------------
//	¥ SetLength														  [public]
// ---------------------------------------------------------------------------
//	Set the length, in bytes, of the ResourceStream

void
CResourceStream::SetLength(
	Int32	inLength)
{
	if (mMacResourceH == nil) {				// We cannot create a new resource ourself.
		SignalCStr_( "CResourceStream::SetLength called for uninitialized resource." );
		ThrowIfOSErr_( resNotFound );
		
	} else {								// Resize resource on disk.
		::SetResourceSize(mMacResourceH, inLength);
		ThrowIfResError_();
		
		LStream::SetLength(inLength);		// Adjust length count
	}
}


// ---------------------------------------------------------------------------
//		¥ PutBytes
// ---------------------------------------------------------------------------
//	Write bytes from a buffer to a ResourceStream
//
//	Returns an error code and passes back the number of bytes actually
//	written, which may be less than the number requested if an error occurred.
//
//	Grows resource if necessary.

ExceptionCode
CResourceStream::PutBytes(
	const void	*inBuffer,
	Int32		&ioByteCount)
{
	ExceptionCode	err = noErr;
	Int32	endOfWrite = GetMarker() + ioByteCount;
	
	if (endOfWrite > GetLength()) {	// Need to grow resource
									
		Try_ {
			SetLength(endOfWrite);
		}
		
		Catch_(inErr) {				// Grow failed. Write only what fits.
			ioByteCount = GetLength() - GetMarker();
			err = inErr;
		} EndCatch_
	}
									// Copy bytes into resource.
	if (ioByteCount > 0) {			//   Byte count will be zero if
									//   mMacResourceH is nil
		if (mMacResourceH != nil) {
			::WritePartialResource( mMacResourceH, GetMarker(), inBuffer, ioByteCount);
			
		} else {
			SignalCStr_( "CResourceStream::PutBytes called for uninitialized resource." );
			ioByteCount = 0;
			
		}
		
		SetMarker(ioByteCount, streamFrom_Marker);
	}
	
	return err;
}


// ---------------------------------------------------------------------------
//		¥ GetBytes
// ---------------------------------------------------------------------------
//	Read bytes from a ResourceStream to a buffer
//
//	Returns an error code and passes back the number of bytes actually
//	read, which may be less than the number requested if an error occurred.
//
//	Errors:
//		readErr		Attempt to read past the end of the ResourceStream

ExceptionCode
CResourceStream::GetBytes(
	void	*outBuffer,
	Int32	&ioByteCount)
{
	ExceptionCode	err = noErr;
									// Upper bound is number of bytes from
									//   marker to end
	if ((GetMarker() + ioByteCount) > GetLength()) {
		ioByteCount = GetLength() - GetMarker();
		err = readErr;
	}
									// Copy bytes from resource into buffer
	if (mMacResourceH != nil) {
		::ReadPartialResource( mMacResourceH, GetMarker(), outBuffer, ioByteCount );
		
		SetMarker(ioByteCount, streamFrom_Marker);
	}
	
	return err;
}


// ---------------------------------------------------------------------------
//		¥ SetDataHandle
// ---------------------------------------------------------------------------
//	Specify a resource to use as the basis for a ResourceStream
//
//	The CResourceStream object assumes ownership of the resource. Make sure
//	the resource is not already loaded in memory. If it is, call WriteResource to
//	write any changes to disk, then call ReleaseResource to release it, prior
//	to calling this. If you want to load the resource yourself, call ReleaseDataHandle.
//
//	inCurrentForkOnly specifies if we must look in the current resource file only,
//	or in all resource files. (default: false)

void
CResourceStream::SetDataHandle(
	ResType			inMacResourceType,
	ResIDT			inMacResourceID,
	Boolean			inCurrentForkOnly)
{
	OSErr		err;

	if (mMacResourceH != nil) {				// Free existing resource Handle
		::ReleaseResource( mMacResourceH );
		ThrowIfResError_();
	}
	
	mMacResourceType = inMacResourceType;
	mMacResourceID = inMacResourceID;
	
	::SetResLoad( false );
	ThrowIfResError_();
	
	// get the resource handle.
	if (inCurrentForkOnly) {
		mMacResourceH = ::Get1Resource( mMacResourceType, mMacResourceID );
	} else {
		mMacResourceH = ::GetResource( mMacResourceType, mMacResourceID );
	}
	err = ::ResError();
	
	::SetResLoad( true );
	ThrowIfResError_();
	
	if ((err == noErr) && (mMacResourceH == nil))
		err = resNotFound;
	ThrowIfOSErr_( err );
	
	// get resource length.
	LStream::SetLength( ::GetResourceSizeOnDisk( mMacResourceH ) );
	ThrowIfResError_();
		
	SetMarker(0, streamFrom_Start);
}


// ---------------------------------------------------------------------------
//		¥ SetDataHandle
// ---------------------------------------------------------------------------
//	Specify a new resource to use as the basis for a ResourceStream
//
//	The CResourceStream object creates the resource on disk and assumes ownership
//	of its handle. If you want to load the resource yourself, call ReleaseDataHandle.

void
CResourceStream::SetDataHandle(
	ResType			inMacResourceType,
	ResIDT			inMacResourceID,
	ConstStringPtr	inMacResourceName)
{
	OSErr		err;

	if (mMacResourceH != nil) {				// Free existing resource Handle
		::ReleaseResource( mMacResourceH );
		ThrowIfResError_();
	}
	
	mMacResourceType = inMacResourceType;
	mMacResourceID = inMacResourceID;
	
	// we must create an empty resource on disk.
	Handle macH = ::NewHandle( 0 );
	ThrowIfNil_( macH );
	::AddResource( macH, mMacResourceType, mMacResourceID, inMacResourceName );
	ThrowIfResError_();
	::ReleaseResource( macH );
	ThrowIfResError_();
	
	// now we must get its handle.
	::SetResLoad( false );
	ThrowIfResError_();
	
	mMacResourceH = ::Get1Resource( mMacResourceType, mMacResourceID );
	err = ::ResError();
	
	::SetResLoad( true );
	ThrowIfResError_();
	
	if ((err == noErr) && (mMacResourceH == nil))
		err = resNotFound;
	ThrowIfOSErr_( err );
	
	// set length.
	LStream::SetLength( 0 );
		
	SetMarker(0, streamFrom_Start);
}


// ---------------------------------------------------------------------------
//		¥ ReleaseDataHandle
// ---------------------------------------------------------------------------
//	Dissociate the resource Handle from a ResourceStream.
//
//	Call this to release the resource Handle. Destroying the object is another
//	way to release it.

void
CResourceStream::ReleaseDataHandle()
{
	::ReleaseResource( mMacResourceH );
	ThrowIfResError_();

	SetMarker(0, streamFrom_Start);
	LStream::SetLength(0);
	mMacResourceH = nil;					// Reset to nil Handle
}
