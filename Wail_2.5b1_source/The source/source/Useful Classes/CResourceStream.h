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

#ifndef _H_CResourceStream
#define _H_CResourceStream
#pragma once

#include <LStream.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

// ---------------------------------------------------------------------------

class	CResourceStream : public LStream {
public:
							CResourceStream();
							CResourceStream(
									ResType			inMacResourceType,
									ResIDT			inMacResourceID,
									Boolean			inCurrentForkOnly = false);
							CResourceStream(
									ResType			inMacResourceType,
									ResIDT			inMacResourceID,
									ConstStringPtr	inMacResourceName);
	virtual					~CResourceStream();
	
	virtual void			SetLength(
									SInt32		inLength);
	
	virtual ExceptionCode	PutBytes(
									const void	*inBuffer,
									SInt32		&ioByteCount);
	virtual ExceptionCode	GetBytes(
									void		*outBuffer,
									SInt32		&ioByteCount);

	void					SetDataHandle(
									ResType			inMacResourceType,
									ResIDT			inMacResourceID,
									Boolean			inCurrentForkOnly = false);
	void					SetDataHandle(
									ResType			inMacResourceType,
									ResIDT			inMacResourceID,
									ConstStringPtr	inMacResourceName);
							
	void					ReleaseDataHandle();

protected:
	Handle			mMacResourceH;
	
	ResType			mMacResourceType;
	ResIDT			mMacResourceID;
};


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif

#endif
