// =================================================================================
//	CWailProgressDialog.h					©1999, Charles Lechasseur
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


// progress dialog string IDs

enum
{
	progressString_LoadingClassNames = 1,
	progressString_SearchingForClass,
	progressString_LoadingSoundData,
	progressString_SavingSoundData,
	progressString_ComparingSoundData,
	progressString_Shuttling
};


// CWailProgressDialog class

class CWailProgressDialog
{
	public:
	// Public Enums and Constants
		
	// Public Functions
		//Default Constructor
							CWailProgressDialog(
								SInt32			inProgressBarSize,
								SInt32			inWindowTitleIndex,
								LCommander		*inSuper);
		//Destructor
		virtual				~CWailProgressDialog();
		
		// public functions
		
		SInt32				GetProgressBarSize() const;
		void				SetProgressBarSize(
								SInt32			inProgressBarSize );
		
		virtual void		Increment(
								SInt32			inHowMuch = 1);
		virtual void		Reset();
		
		void				Show();
		void				Hide();
		
	private:
		// Member Variables

		LWindow				*mWindow;		// progress bar window.
		LControl			*mProgressBar;	// for speed, we keep it here.
	
		// Defensive programming. No copy constructor nor operator=
							CWailProgressDialog(const CWailProgressDialog&);
		CWailProgressDialog&			operator=(const CWailProgressDialog&);
};
