// =================================================================================
//	CSingleColumnTextListBox.h					©1999, Charles Lechasseur
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


#pragma once
#include "CSingleColumnListBox.h"

class CSingleColumnTextListBox: public CSingleColumnListBox
{
	public:
	// Public Enums and Constants
		enum { class_ID = 'C1Tl' };
	
	// Public Functions

		//Default Constructor
							CSingleColumnTextListBox();
		
		//Constructor from input parameters
							CSingleColumnTextListBox(
								const SPaneInfo		&inPaneInfo,
								Boolean				inHasHorizScroll,
								Boolean				inHasVertScroll,
								Boolean				inHasGrow,
								Boolean				inHasFocusBox,
								MessageT			inDoubleClickMessage,
								SInt16				inTextTraitsID,
								SInt16				inLDEFid,
								LCommander			*inSuper );
		
		// Stream Constructor 
							CSingleColumnTextListBox(
								LStream 			*inStream );
		// Copy Constructor
							CSingleColumnTextListBox(
								const CSingleColumnTextListBox& inCSingleColumnTextListBox );
		//Destructor
		virtual				~CSingleColumnTextListBox();
		
		// Adding and removing rows
		
		void		AddRow( SInt16 inWhere, ConstStringPtr inContent )
						{
							AddRows( inWhere, 1, inContent );
						}
								
		void		AddRows( SInt16 inWhere, SInt16 inHowMany, ConstStringPtr inContent );

		void		RemoveRow( SInt16 inWhich )
						{
							RemoveRows( inWhich, 1 );
						}

		void		RemoveRows( SInt16 inWhere, SInt16 inHowMany );
		
	private:
	// Private Functions
		// Defensive programming. No  operator=
		CSingleColumnTextListBox&			operator=(const CSingleColumnTextListBox&);
};
