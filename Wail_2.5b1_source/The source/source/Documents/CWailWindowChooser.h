// =================================================================================
//	CWailWindowChooser.h					�1999, Charles Lechasseur
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


class CWailDocWindow;

class CWailWindowChooser : public LCommander
{
	public:
	// Public Functions
		//Default Constructor
									CWailWindowChooser(
										LCommander*	inSuper );
		//Destructor
		virtual						~CWailWindowChooser();
		
		static Boolean				HasMoreThanOneWindow();
		virtual CWailDocWindow*		ChooseWindow(
										CWailDocWindow*	inExceptThisOne );
		
		static void					SortWindowArrayAlphabetically(
										LArray&		ioArray );
		
	private:
		// Member Variables and Classes

	
		// Defensive programming. No copy constructor nor operator=
							CWailWindowChooser(const CWailWindowChooser&);
		CWailWindowChooser&			operator=(const CWailWindowChooser&);
};


// comparator for comparing LWindow* objects against their titles

class CWindowTitleComparator : public LComparator
{
	public:
	// Public functions
		//Default Constructor
									CWindowTitleComparator();
		//Destructor
		virtual						~CWindowTitleComparator();
		
		// comparing
		
		virtual SInt32				Compare(
										const void*			inItemOne,
										const void* 		inItemTwo,
										UInt32				inSizeOne,
										UInt32				inSizeTwo) const;
		
		virtual	SInt32				CompareToKey(
										const void*			inItem,
										UInt32				inSize,
										const void*			inKey) const;
										
		virtual LComparator*		Clone();
};