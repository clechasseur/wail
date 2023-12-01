// =================================================================================
//	CSingleColumnListBox.cp					©1999, Charles Lechasseur
// =================================================================================
//
// CSingleColumnListBox, is, quite obviously, a class derived from LListBox that
// only has one column. the column is created by the constructor. nothing prevents
// you from adding columns with LAddColumn later on, however.

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

#include "CSingleColumnListBox.h"

// ---------------------------------------------------------------------------
//	¥ CSingleColumnListBox							Constructor	[public]
// ---------------------------------------------------------------------------

CSingleColumnListBox::CSingleColumnListBox()
{
	AddColumn();
}


// ---------------------------------------------------------------------------
//	¥ CSingleColumnListBox		Constructor from input parameters [public]
// ---------------------------------------------------------------------------

CSingleColumnListBox::CSingleColumnListBox(
	const SPaneInfo		&inPaneInfo,
	Boolean				inHasHorizScroll,
	Boolean				inHasVertScroll,
	Boolean				inHasGrow,
	Boolean				inHasFocusBox,
	MessageT			inDoubleClickMessage,
	SInt16				inTextTraitsID,
	SInt16				inLDEFid,
	LCommander			*inSuper )
	: LListBox ( inPaneInfo, inHasHorizScroll, inHasVertScroll, inHasGrow,
		inHasFocusBox, inDoubleClickMessage, inTextTraitsID, inLDEFid, inSuper )
{
	AddColumn();
}


// ---------------------------------------------------------------------------
//	¥ CSingleColumnListBox					Stream Constructor	[public]
// ---------------------------------------------------------------------------

CSingleColumnListBox::CSingleColumnListBox(
	LStream *inStream )
	: LListBox( inStream )
{
	AddColumn();
}


// ---------------------------------------------------------------------------
//	¥ CSingleColumnListBox						Copy Constructor	[public]
// ---------------------------------------------------------------------------

CSingleColumnListBox::CSingleColumnListBox(
	const CSingleColumnListBox& inOriginal )
	: LListBox( inOriginal )
{
	AddColumn();
}


// ---------------------------------------------------------------------------
//	¥ ~CSingleColumnListBox							Destructor		[public]
// ---------------------------------------------------------------------------

CSingleColumnListBox::~CSingleColumnListBox()
{
}


// ---------------------------------------------------------------------------------
//	¥ AddColumn														[protected]
// ---------------------------------------------------------------------------------
// adds one column to the list, at the very beginning. this is called by the constructors
// and should not be called manually (hence the protected status).

void
CSingleColumnListBox::AddColumn()
{
	ThrowIf_( ::LAddColumn( 1, 0, mMacListH ) != 0 );
}
