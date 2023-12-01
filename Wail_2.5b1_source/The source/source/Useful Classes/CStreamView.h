// =================================================================================
//	CStreamView.h					©2002, Charles Lechasseur
// =================================================================================

#pragma once

#include <LStream.h>


class CStreamView : public LStream
{
	public:
	// Public Functions
	
		//Constructor
		
								CStreamView(
									LStream*	inStreamToView,
									SInt32		inViewStartOffset,
									SInt32		inViewLength,
									bool		inOwnsStream = false );
		
		//Destructor
		
		virtual					~CStreamView();
		
		// LStream overridden functions
		
		virtual void			SetMarker(
									SInt32			inOffset,
									EStreamFrom		inFromWhere );
							
		virtual SInt32			GetMarker() const;
		
		virtual void			SetLength(
									SInt32			inLength );
								
		virtual ExceptionCode	PutBytes(
									const void*		inBuffer,
									SInt32&			ioByteCount );
									
		virtual ExceptionCode	GetBytes(
									void*			outBuffer,
									SInt32&			ioByteCount );
		
	private:
	
		// Member variables
		
		LStream*			mViewedStream;
		SInt32				mStartOffset;
		bool				mOwnsStream;
	
		// Defensive programming. No copy constructor nor operator=
							CStreamView(const CStreamView&);
		CStreamView&			operator=(const CStreamView&);
};
