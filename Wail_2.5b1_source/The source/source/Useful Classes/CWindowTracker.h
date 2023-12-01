// =================================================================================
//	CWindowTracker.h					©2000, Charles Lechasseur
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
#include <LAttachment.h>


class CWindowTracker : public LAttachment
{
public:

	// Constructor/destructor
	
									CWindowTracker(
										ResIDT			inMenuID = 0 );
									CWindowTracker(
										LMenu*			inMenu = nil );
	virtual							~CWindowTracker();
	
	// adding and removing windows from our list
	
	virtual bool					AddWindow(
										LWindow*		inWindow );
	virtual bool					RemoveWindow(
										LWindow*		inWindow );
	virtual void					RemoveAllWindows();
	
	// accessing the list
	
	virtual bool					HasWindow(
										LWindow*		inWindow ) const;
									
	virtual LWindow*				GetWindow(
										ArrayIndexT		inIndex ) const;
	virtual UInt32					GetNumberOfWindows() const;
	
	virtual void					GetWindowList(
										TArray<LWindow*>&	outWindowList ) const;
	
	// global access
	
	static CWindowTracker*			GetCurrentWindowTracker() { return sWindowTracker; }
	
protected:

	// members
	
	static CWindowTracker*			sWindowTracker;
	
	ResIDT							mMenuID;
	LMenu*							mMenu;
	SInt16							mInitialItemsCount;
	
	TArray<LWindow*>				mWindows;
	UInt32							mWindowsCount;
	
	// internals for adding/removing windows to/from the array
	
	void							AddWindowInSpot(
										LWindow*		inWindow,
										ArrayIndexT		inIndex );
	void							RemoveWindowFromSpot(
										ArrayIndexT		inIndex );
									
	// attachment execution
	
	virtual void					ExecuteSelf(
										MessageT		inMessage,
										void*			ioParam );
									
	// menu management
	
	void							FindLMenuPointer();
	bool							IsMenuItemOurs(
										SInt16&			ioMenuItem ) const;
	
	virtual void					UpdateMenu(
										LWindow*		inActivatedWindow = nil );
		
private:

	// init
	
	void							InitWindowTracker(
										ResIDT			inMenuID,
										LMenu*			inMenu );

	// Defensive programming. No  operator= or copy constructor
									CWindowTracker(const CWindowTracker&);
	CWindowTracker&					operator=(const CWindowTracker&);
};
