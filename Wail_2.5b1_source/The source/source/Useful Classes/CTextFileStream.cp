// =================================================================================
//	CTextFileStream.cp					©1999, Charles Lechasseur
// =================================================================================
//
// file stream with utility functions related to text files:
//
//	LineLength
//		gets the length of the current line (up to the next '\n' char, included).
//
//	GetLine
//		reads the next line of text (up to the next '\n' char, included).

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

#include "CTextFileStream.h"


// ---------------------------------------------------------------------------
//	¥ CTextFileStream							Constructor	[public]
// ---------------------------------------------------------------------------

CTextFileStream::CTextFileStream()
{
}


// ---------------------------------------------------------------------------
//	¥ CTextFileStream					Constructor from FSSpec	[public]
// ---------------------------------------------------------------------------

CTextFileStream::CTextFileStream(
	const FSSpec	&inFileSpec )
	: LFileStream( inFileSpec )
{
}


// ---------------------------------------------------------------------------
//	¥ CTextFileStream					Constructor	from Alias [public]
// ---------------------------------------------------------------------------

CTextFileStream::CTextFileStream(
	AliasHandle		inAlias,
	Boolean			&outWasChanged,
	FSSpec			*inFromFile )
	: LFileStream( inAlias, outWasChanged, inFromFile )
{
}


// ---------------------------------------------------------------------------
//	¥ ~CTextFileStream							Destructor	[public]
// ---------------------------------------------------------------------------

CTextFileStream::~CTextFileStream()
{
}


// ---------------------------------------------------------------------------------
//		¥ LineLength
// ---------------------------------------------------------------------------------
// gets the length of the current line (up to the next '\n' char, included).

SInt32
CTextFileStream::LineLength()
{
	SInt32 lineLength = 0;

	// save marker. we'll need to set this back when we end the function.
	SInt32 saveMarker = GetMarker();
		
	try
	{
		// get a byte until we have a \n.
		char buffer = '\0';
		SInt32 readSize = 1;
		while (!AtEnd() && (buffer != '\n') && (buffer != '\r'))
		{
			ThrowIfError_( GetBytes( &buffer, readSize ) );
			ThrowIf_( readSize == 0 );	// we should have checked for that in the while loop.
			if ((buffer != '\n') && (buffer != '\r'))
				++lineLength;
		}
	}
	
	catch (...)
	{
		// restore marker before rethrowing.
		SetMarker( saveMarker, streamFrom_Start );
		
		throw;
	}
	
	// restore marker.
	SetMarker( saveMarker, streamFrom_Start );
	
	// return the result.
	return lineLength;
}


// ---------------------------------------------------------------------------------
//		¥ GetLine
// ---------------------------------------------------------------------------------
// reads the next line of text (up to the next '\n' char, included).

SInt32
CTextFileStream::GetLine(
	char*			outBuffer,
	SInt32			inBufferSize )
{
	SInt32 bytesRead = 0;
	
	// read byte per byte. we need to do that, otherwise we'd call LineLength and read twice.
	char buffer = '\0';
	while (!AtEnd() &&
		   (bytesRead <= inBufferSize) &&
		   (buffer != '\n') &&
		   (buffer != '\r'))
	{
		// read one char.
		SInt32 readSize = 1;
		ReadBlock( &buffer, readSize );
		ThrowIf_( readSize == 0 );
		if ((buffer != '\n') && (buffer != '\r'))
		{
			outBuffer[bytesRead] = buffer;
			++bytesRead;
		}
	}
	
	// last char needs to be turned into a '\0'.
	outBuffer[bytesRead] = '\0';
	
	// return number of bytes actually read.
	return bytesRead;
}