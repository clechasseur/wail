// =================================================================================
//	UWailClassNames.cp					©1999, Charles Lechasseur
// =================================================================================
//
// utility class for reading Wail class names.

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

#include "UWailClassNames.h"

#include "CTextFileStream.h"
#include "UWailPreferences.h"


// static members of UWailClassNames.

Boolean				UWailClassNames::sClassNamesRead	= false;
TArray<LStr255*>	UWailClassNames::sClassNamesArray;


// ---------------------------------------------------------------------------------
//		¥ ReadClassNames
// ---------------------------------------------------------------------------------
// pre-reads the class names for faster access.

void
UWailClassNames::ReadClassNames()
{
	// create name file according to prefs and check if it exists.
	FSSpec theFileSpec;
	UWailPreferences::GetClassNamesFile( theFileSpec );
	OSErr err = ::FSMakeFSSpec( theFileSpec.vRefNum, theFileSpec.parID,
								theFileSpec.name, &theFileSpec );
	if (err == noErr)
	{
		// read names.
		StCursor watch( watchCursor );
		CTextFileStream theNameFile( theFileSpec );
		
		theNameFile.OpenDataFork( fsRdPerm );
		
		char theLine[256];
		while (!theNameFile.AtEnd())
		{
			theNameFile.GetLine( theLine, 256 );
			sClassNamesArray.AddItem( new LStr255( theLine ) );
		}
		
		theNameFile.CloseDataFork();
		
		// mark is known.
		sClassNamesRead = true;
	}
}


// ---------------------------------------------------------------------------------
//		¥ DisposeClassNames
// ---------------------------------------------------------------------------------
// clears the known class names from memory.

void
UWailClassNames::DisposeClassNames()
{
	// clear content of array.
	StCursor watch( watchCursor );
	TArrayIterator<LStr255*> theIterator( sClassNamesArray );
	LStr255* theCurStringP = nil;
	while (theIterator.Next( theCurStringP ))
	{
		SignalIf_( theCurStringP == nil );
		delete theCurStringP;
	}
	sClassNamesArray.RemoveItemsAt( 0x7FFFFFFF, LArray::index_First );
	
	// mark as unknown.
	sClassNamesRead = false;
}


// ---------------------------------------------------------------------------------
//		¥ GetClassName
// ---------------------------------------------------------------------------------
// returns the name of the given class (starting at 1).

void
UWailClassNames::GetClassName(
	SInt32		inClassNumber,
	LStr255&	outClassName )
{
	Boolean foundIt = false;

	// are class names already known?
	if (sClassNamesRead)
	{
		if (((UInt32) inClassNumber) <= sClassNamesArray.GetCount())
		{
			LStr255* theStringP = sClassNamesArray[inClassNumber];
			SignalIf_( theStringP == nil );
			outClassName = *theStringP;
			foundIt = true;
		}
	}
	
	if (!foundIt)
	{
		// use a generic name.
		outClassName = "\pClass #";
		outClassName += (inClassNumber - 1); // conforming to the Fux standard.
	}
}