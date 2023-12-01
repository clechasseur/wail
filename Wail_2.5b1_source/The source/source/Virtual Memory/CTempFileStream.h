// =================================================================================
//	CTempFileStream.h					©1999, Charles Lechasseur
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

#ifndef __CTEMPFILESTREAM__
#define __CTEMPFILESTREAM__
#pragma once

#include <LFileStream.h>


class CTempFileStream: public LFileStream
{
	public:
	// Public Functions

		// Constructor
							CTempFileStream(
								SInt32	inDesiredSize = 0L );
		// Destructor
		virtual				~CTempFileStream();
	
	protected:
	// Protected Functions
	
		virtual void		CreateTempFile();				// creates the temp file.
		virtual void		DeleteTempFile();				// deletes the temp file.
		
		virtual void		GetBestFSSpec(
								FSSpec&	outSpec ) const;	// returns an FSSpec for the temp file.
		virtual StringPtr	GetBestName(
								SInt16	inVRefNum,
								SInt32	inParID,
								Str255	outName ) const;	// returns a random name for the temp file.
								
		virtual OSType		GetFileType() const;			// type of the temp file.
		virtual OSType		GetCreator() const;				// creator of the temp file.
		
	// member variables
	
		static SInt32		sTempFileNum;	// number of temp files already allocated.
		
	private:
	// Private Functions
		// Defensive programming. No  operator=
		CTempFileStream&			operator=(const CTempFileStream&);
		// Defensive programming. No copy Constructor
							CTempFileStream(const CTempFileStream&);
};


#endif //__CTEMPFILESTREAM__