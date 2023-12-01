// =================================================================================
//	CStreamView.cp					©2002, Charles Lechasseur
// =================================================================================
//
// A stream that offers a particular view of another stream's data. The view is
// read-only.
//
// Note that for this class to work properly, the viewed stream should remain alive
// while you're viewing it. It would also be advisable that it doesn't change size.

#include "CStreamView.h"


// ---------------------------------------------------------------------------
//	¥ CStreamView							Constructor	[public]
// ---------------------------------------------------------------------------

CStreamView::CStreamView(
	LStream*	inStreamToView,
	SInt32		inViewStartOffset,
	SInt32		inViewLength,
	bool		inOwnsStream /*= false*/ )
	: mViewedStream( inStreamToView ),
	  mStartOffset( inViewStartOffset ),
	  mOwnsStream( inOwnsStream )
{
	SignalIf_( inStreamToView == nil );
	SignalIf_( inViewStartOffset < 0 );
	SignalIf_( inViewStartOffset > inStreamToView->GetLength() );
	SignalIf_( inViewLength < 0 );
	SignalIf_( inViewLength > (inStreamToView->GetLength() - inViewStartOffset) );
	
	// set our length. (this one is kept in LStream)
	LStream::SetLength( inViewLength );
	
	// set stream offset to our start offset to look like we're starting at 0.
	mViewedStream->SetMarker( mStartOffset, streamFrom_Start );
}

			
// ---------------------------------------------------------------------------
//	¥ ~CStreamView							Destructor	[public]
// ---------------------------------------------------------------------------

CStreamView::~CStreamView()
{
	// delete stream if we own it.
	if (mOwnsStream)
	{
		delete mViewedStream;
		mViewedStream = nil;
	}
}


// ---------------------------------------------------------------------------
//		¥ SetMarker
// ---------------------------------------------------------------------------
// Moves the stream marker. We share our marker with our viewed stream, but
// we must simulate the marker's position as being inside our view.

void
CStreamView::SetMarker(
	SInt32			inOffset,
	EStreamFrom		inFromWhere )
{
	// compute real target marker position.
	SInt32 realOffset = 0;
	switch (inFromWhere)
	{
		case streamFrom_Start:
			// real offset is the user's offset plus our start offset.
			realOffset = inOffset + mStartOffset;
			break;
		
		case streamFrom_End:
			// real offset is our end offset minus the user's offset.
			realOffset = (mStartOffset + GetLength()) - inOffset;
			break;
		
		case streamFrom_Marker:
			// real offset is current marker (ours) plus user's offset.
			realOffset = GetMarker() + inOffset;
			break;
			
		default:
			// programmer error.
			SignalStringLiteral_( "Programmer error: wrong EStreamFrom value" );
			Throw_( posErr );
			break;
	}
	
	// call our stream's method to set the offset.
	mViewedStream->SetMarker( realOffset, streamFrom_Start );
	
	// call LStream method to change our marker (just to keep it in sync).
	LStream::SetMarker( inOffset, inFromWhere );
}
							

// ---------------------------------------------------------------------------
//		¥ GetMarker
// ---------------------------------------------------------------------------
// Returns the current marker position. Accounts for our view's starting offset.

SInt32
CStreamView::GetMarker() const
{
	// call viewed stream method and substract our start offset.
	return mViewedStream->GetMarker() - mStartOffset;
}
		

// ---------------------------------------------------------------------------
//		¥ SetLength
// ---------------------------------------------------------------------------
// Overridden to prevent setting the stream's length. This view is read-only.

void
CStreamView::SetLength(
	SInt32	inLength )
{
#pragma unused( inLength )

	SignalStringLiteral_( "Programmer error: setting length of stream view" );
}


// ---------------------------------------------------------------------------
//		¥ PutBytes
// ---------------------------------------------------------------------------
// Overridden to prevent writing to the stream. This view is read-only.

ExceptionCode
CStreamView::PutBytes(
	const void*		inBuffer,
	SInt32&			ioByteCount )
{
#pragma unused( inBuffer )

	SignalStringLiteral_( "Programmer error: writing to a stream view" );
	ioByteCount = 0;
	return unimpErr;
}


// ---------------------------------------------------------------------------
//		¥ GetBytes
// ---------------------------------------------------------------------------
// Reads data from the stream, making sure we don't read past our viewable area.

ExceptionCode
CStreamView::GetBytes(
	void*			outBuffer,
	SInt32&			ioByteCount )
{
	// if user is going to read past the end, block him.
	SInt32 maxRead = GetLength() - GetMarker();
	if (ioByteCount > maxRead)
		ioByteCount = maxRead;
		
	// do the reading in our viewed stream.
	return mViewedStream->GetBytes( outBuffer, ioByteCount );
}