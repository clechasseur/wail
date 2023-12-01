// =================================================================================
//	CBasicVirtualMemoryManager.h					©1999, Charles Lechasseur
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

#ifndef __CBASICVIRTUALMEMORYMANAGER__
#define __CBASICVIRTUALMEMORYMANAGER__
#pragma once

#include "CVirtualMemoryManager.h"


// CBasicVirtualMemoryManager class

class CBasicVirtualMemoryManager : public CVirtualMemoryManager
{
	public:
	// Public Functions
	
		//Default Constructor
							CBasicVirtualMemoryManager(
								SInt32	inMemProtected );
		//Destructor
		virtual				~CBasicVirtualMemoryManager();
		
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
										
	protected:
	
		// Member variables
		
		SInt32						mMemProtected;	// amount of untouchable RAM.
		
		TArray<LStream*>			mAllocatedStreams;	// all allocated streams.
	
	private:
	
		// Defensive programming. No operator=
		CBasicVirtualMemoryManager&			operator=(const CBasicVirtualMemoryManager&);
		// Defensive programming. No copy constructor
							CBasicVirtualMemoryManager(
										const CBasicVirtualMemoryManager& );
};


#endif //__CBASICVIRTUALMEMORYMANAGER__