// =================================================================================
//	CVirtualMemoryManager.h					©1999, Charles Lechasseur
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

#ifndef __CVIRTUALMEMORYMANAGER__
#define __CVIRTUALMEMORYMANAGER__
#pragma once


// type used to identify references to private virtual streams
typedef SInt32	VirtualStreamRefT;


// an equivalent for NULL, but for private stream references:
const VirtualStreamRefT		virtualStreamRef_NULL	= 0;


// CVirtualMemoryManager class

class CVirtualMemoryManager
{
	public:
	// Public Functions
	
		//Default Constructor
							CVirtualMemoryManager();
		//Copy Constructor
							CVirtualMemoryManager(
										const CVirtualMemoryManager&	inOriginal );
		//Destructor
		virtual				~CVirtualMemoryManager();
		
		// Stream allocation and disposal
		
		virtual VirtualStreamRefT	AllocateStream(
										SInt32				inSize );
		virtual void				DeallocateStream(
										VirtualStreamRefT	inStreamRef );
										
		// LStream mimic functions
		
		virtual void				SetMarker(
										VirtualStreamRefT	inStreamRef,
										SInt32				inOffset,
										EStreamFrom			inFromWhere );
		virtual SInt32				GetMarker(
										VirtualStreamRefT	inStreamRef ) const;
		
		virtual void				SetLength(
										VirtualStreamRefT	inStreamRef,
										SInt32				inLength );
		virtual SInt32				GetLength(
										VirtualStreamRefT	inStreamRef ) const;
										
		virtual ExceptionCode		PutBytes(
										VirtualStreamRefT	inStreamRef,
										const void*			inBuffer,
										SInt32&				ioByteCount );
										
		virtual ExceptionCode		GetBytes(
										VirtualStreamRefT	inStreamRef,
										void*				outBuffer,
										SInt32&				ioByteCount );
										
		// Buffer accessors
		
		virtual Boolean				CanGetBuffer(
										VirtualStreamRefT	inStreamRef ) const;
		virtual const void*			GetBuffer(
										VirtualStreamRefT	inStreamRef ) const;
										
		// utility functions
		
		virtual Boolean				CanGetStreamsCount() const;
		virtual SInt32				GetStreamsCount() const;
		
		virtual Boolean				CanGetIthStream() const;
		virtual SInt32				GetIndexOfFirstStream() const;
		virtual VirtualStreamRefT	GetIthStream(
										SInt32				inStreamIndex ) const;
	
	private:
	
		// Defensive programming. No operator=
		CVirtualMemoryManager&			operator=(const CVirtualMemoryManager&);
};


#endif //__CVIRTUALMEMORYMANAGER__