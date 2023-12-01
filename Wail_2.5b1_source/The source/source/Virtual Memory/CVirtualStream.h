// =================================================================================
//	CVirtualStream.h					©1999, Charles Lechasseur
// =================================================================================

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

#ifndef __CVIRTUALSTREAM__
#define __CVIRTUALSTREAM__
#pragma once

#include <LStream.h>

#include "CVirtualMemoryManager.h"


class CVirtualStream: public LStream
{
	public:
	// Public Functions

		// Default Constructor
							CVirtualStream(
								CVirtualMemoryManager*	inManager = NULL );
		
		// Parameterized constructor
							CVirtualStream(
								SInt32					inSize,
								CVirtualMemoryManager*	inManager = NULL );
		
		// Copy Constructor
							CVirtualStream(
								const CVirtualStream& 	inOriginal );
		
		// Destructor
		virtual				~CVirtualStream();
		
		// Accessors
		
		CVirtualMemoryManager*	GetManager() const { return mManager; }
		
		// LStream functions
		
		virtual void			SetMarker(
									SInt32			inOffset,
									EStreamFrom		inFromWhere);
		virtual SInt32			GetMarker() const;
		
		virtual void			SetLength(
									SInt32			inLength);
		virtual SInt32			GetLength() const;
		
		virtual ExceptionCode	PutBytes(
									const void		*inBuffer,
									SInt32			&ioByteCount);
								
		virtual ExceptionCode	GetBytes(
									void			*outBuffer,
									SInt32			&ioByteCount);
									
		// Buffer accessors
		
		virtual Boolean			CanGetBuffer() const;
		virtual const void*		GetBuffer() const;
		
	private:
	// Member Variables and Classes
	
		CVirtualMemoryManager*		mManager;
		VirtualStreamRefT			mStreamRef;
	
	// Private Functions
		// Defensive programming. No  operator=
		CVirtualStream&			operator=(const CVirtualStream&);
};


#endif //__CVIRTUALSTREAM__
