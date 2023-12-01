// =================================================================================
//	CSingleColumnTextListBox.cp					©1999, Charles Lechasseur
// =================================================================================
//
// this class is derived from CSingleColumnListBox and it simply makes it easier
// to handle a text list. the following member functions can be used instead
// of list manager calls:
//
//	AddRow( SInt16 inWhere, ConstStringPtr inContent )
//		adds a row to the list and places the specified string in its cell.
//
//	AddRows( SInt16 inWhere, SInt16 inHowMany, ConstStringPtr inContent )
//		adds many rows to the list and places the specified string in all their cells.
//
//	RemoveRow( SInt16 inWhich )
//		removes a row from the list.
//
//	RemoveRows( SInt16 inWhere, SInt16 inHowMany )
//		removes many rows from the list.

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

#include "CSingleColumnTextListBox.h"


// ---------------------------------------------------------------------------
//	¥ CSingleColumnTextListBox							Constructor	[public]
// ---------------------------------------------------------------------------

CSingleColumnTextListBox::CSingleColumnTextListBox()
{
}


// ---------------------------------------------------------------------------
//	¥ CSingleColumnTextListBox		Constructor from input parameters [public]
// ---------------------------------------------------------------------------

CSingleColumnTextListBox::CSingleColumnTextListBox(
	const SPaneInfo		&inPaneInfo,
	Boolean				inHasHorizScroll,
	Boolean				inHasVertScroll,
	Boolean				inHasGrow,
	Boolean				inHasFocusBox,
	MessageT			inDoubleClickMessage,
	SInt16				inTextTraitsID,
	SInt16				inLDEFid,
	LCommander			*inSuper )
	: CSingleColumnListBox ( inPaneInfo, inHasHorizScroll, inHasVertScroll, inHasGrow,
		inHasFocusBox, inDoubleClickMessage, inTextTraitsID, inLDEFid, inSuper )
{
}


// ---------------------------------------------------------------------------
//	¥ CSingleColumnTextListBox					Stream Constructor	[public]
// ---------------------------------------------------------------------------

CSingleColumnTextListBox::CSingleColumnTextListBox(LStream *inStream)
	: CSingleColumnListBox(inStream)
{
}


// ---------------------------------------------------------------------------
//	¥ CSingleColumnTextListBox						Copy Constructor	[public]
// ---------------------------------------------------------------------------

CSingleColumnTextListBox::CSingleColumnTextListBox(const CSingleColumnTextListBox& inOriginal)
	: CSingleColumnListBox(inOriginal)
{
}


// ---------------------------------------------------------------------------
//	¥ ~CSingleColumnTextListBox							Destructor	[public]
// ---------------------------------------------------------------------------

CSingleColumnTextListBox::~CSingleColumnTextListBox()
{
}


// ---------------------------------------------------------------------------------
//		¥ AddRows
// ---------------------------------------------------------------------------------
// adds a certain number of rows to the list, and fills their cells with the specified text.
// inContent must be a pointer to a pascal string.

void
CSingleColumnTextListBox::AddRows(
	SInt16 			inWhere,
	SInt16 			inHowMany,
	ConstStringPtr 	inContent )
{
	// first, add the rows.
	SInt16 whereRows = ::LAddRow( inHowMany, inWhere, mMacListH );
	
	// next, fill the cells.
	Cell theCell = {0, 0};
	for (theCell.v = whereRows; theCell.v < (whereRows + inHowMany); theCell.v++)
	{
		::LSetCell( inContent + 1, inContent[0], theCell, mMacListH );
	}
}


// ---------------------------------------------------------------------------------
//		¥ RemoveRows
// ---------------------------------------------------------------------------------
// removes a certain number of rows from the list.

void
CSingleColumnTextListBox::RemoveRows(
	SInt16 			inWhere,
	SInt16 			inHowMany )
{
	::LDelRow( inHowMany, inWhere, mMacListH );
}