// =================================================================================
//	CWindowTracker.cp					©2000, Charles Lechasseur
// =================================================================================
//
// an attachment that tracks the opening and closing of windows.
// any window inheriting from CTrackableWindow is tracked.
//
// this class normally works with the singleton principle: create a single
// instance of this class, attached to the application object.
// then use the static GetCurrentWindowTracker method to access the object from
// elsewhere in your program.
//
// additionally, this class can manage a Window menu. to do this,
// pass either the LMenu* of the menu to manage, or its menu ID.
// it is safe to pass the menu ID even before the menu bar is created.
//
// when managing a window menu, CWindowTracker adds its tracked windows to the end
// of the menu. it will assign command key equivalents of [1..0] to the first 10
// windows it gets. it will take care of properly updating the menu, enabling it,
// and putting a check mark beside the active window.
//
// note: the keyboard shortcuts are handled like CodeWarrior. this means that if,
// for example, you have the following windows:
//
// Window 1 (Cmd-1)
// Sounds (Cmd-2)
// More Sounds (Cmd-3)
//
// if you close Sounds, More Sounds will *keep* its Cmd-3 equivalent. the next
// window to open will get Cmd-2 as its keyboard shortcuts. this is a bit
// different from other window menu attachments i've seen, but i feel this
// makes the menu easier to use because the shortcuts don't change when you
// close a window.
//
// (CL - the window menu management should probably be commented a bit better.)

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

#include "CWindowTracker.h"


// static members

CWindowTracker*	CWindowTracker::sWindowTracker = nil;


// ---------------------------------------------------------------------------
//	¥ CWindowTracker							Constructor	[public]
// ---------------------------------------------------------------------------
// if inMenuID is not 0, we assume the user wants us to manage
// the menu whose MENU resource ID is inMenuID.

CWindowTracker::CWindowTracker(
	ResIDT		inMenuID )
{
	InitWindowTracker( inMenuID, nil );
}


// ---------------------------------------------------------------------------
//	¥ CWindowTracker							Constructor	[public]
// ---------------------------------------------------------------------------
// if inMenu is not nil, we assume the user wants us to manage that menu.

CWindowTracker::CWindowTracker(
	LMenu*		inMenu )
{
	InitWindowTracker( 0, inMenu );
}


// ---------------------------------------------------------------------------
//		¥ InitWindowTracker
// ---------------------------------------------------------------------------
// inits the window tracker object. if we have a menu to manage, tries to
// fetch the LMenu* if necessary. if we can't, we'll fetch it later.
// (we have to make this possible since this attachment might very well
// be added to the application object in its constructor, and thus before
// the menu bar gets created.)

void
CWindowTracker::InitWindowTracker(
	ResIDT		inMenuID,
	LMenu*		inMenu )
{
	sWindowTracker = this;

	mMenuID = inMenuID;
	mMenu = inMenu;
	mWindowsCount = 0;
	mInitialItemsCount = -1;
	
	// if we have our menu but not its ID, get it from the LMenu.
	if ((mMenu != nil) && (mMenuID == 0))
		mMenuID = mMenu->GetMenuID();
		
	// if we don't have our menu but we have an ID, try to fetch the menu
	// from the menu bar if it exists.
	if ((mMenu == nil) && (mMenuID != 0))
		FindLMenuPointer();
	
	// pre-add 10 slots for windows which will be accessible via Cmd-[0..9].
	// do this even if we don't manage a menu. it's doesn't do harm and simplifies code.
	mWindows.AdjustAllocation( 10 );
	for (int i = 0; i < 10; ++i)
		mWindows.AddItem( (LWindow*) nil );
}


// ---------------------------------------------------------------------------
//	¥ ~CWindowTracker							Destructor	[public]
// ---------------------------------------------------------------------------

CWindowTracker::~CWindowTracker()
{
	// no need to clear the array of its content.
	// we don't need to free the windows.
}


#pragma mark -


// ---------------------------------------------------------------------------
//		¥ AddWindow
// ---------------------------------------------------------------------------
// adds the specified window to the list of tracked windows.
// returns true if the window was successfully added, and false
// if the window was already in the list of tracked windows.

bool
CWindowTracker::AddWindow(
	LWindow*	inWindow )
{
	ArrayIndexT theIndex = mWindows.FetchIndexOf( inWindow );
	if (theIndex == LArray::index_Bad)
	{
		// try to find an empty spot.
		ArrayIndexT theNilIndex = mWindows.FetchIndexOf( (LWindow*) nil );
		
		// add the window in the spot found. if no spot was found, ask to add window at the end.
		if (theNilIndex == LArray::index_Bad)
			theNilIndex = LArray::index_Last;
		AddWindowInSpot( inWindow, theNilIndex );
	}
		
	return (theIndex == LArray::index_Bad);
}


// ---------------------------------------------------------------------------
//		¥ RemoveWindow
// ---------------------------------------------------------------------------
// removes the specified window from the list of tracked windows.
// returns true if the window was successfully removed, and false
// if the window wasn't in the list of tracked windows.

bool
CWindowTracker::RemoveWindow(
	LWindow*	inWindow )
{
	ArrayIndexT theIndex = mWindows.FetchIndexOf( inWindow );
	if (theIndex != LArray::index_Bad)
	{
		// remove the window.
		RemoveWindowFromSpot( theIndex );
	}
		
	return (theIndex != LArray::index_Bad);
}


// ---------------------------------------------------------------------------
//		¥ RemoveAllWindows
// ---------------------------------------------------------------------------
// clears the list of tracked windows.

void
CWindowTracker::RemoveAllWindows()
{
	// remove all windows after the first 10 spots.
	// reset the first 10 spots to nil;
	mWindows.RemoveItemsAt( 0x7FFFFFFF, 11 );
	for (int i = 1; i <= 10; i++)
		mWindows[i] = nil;
}


#pragma mark -


// ---------------------------------------------------------------------------
//		¥ HasWindow
// ---------------------------------------------------------------------------
// returns true if the specified window is in the list of tracked windows.

bool
CWindowTracker::HasWindow(
	LWindow*	inWindow ) const
{
	return (mWindows.FetchIndexOf( inWindow ) != LArray::index_Bad);
}


// ---------------------------------------------------------------------------
//		¥ GetWindow
// ---------------------------------------------------------------------------
// gets the window that's at the given index.
// note: this index is relative to the number of windows, not the array size.

LWindow*
CWindowTracker::GetWindow(
	ArrayIndexT		inIndex ) const
{
	// do nothing if out of range.
	if (inIndex > mWindowsCount)
		return nil;

	// look for window.
	LWindow* theWindow = nil;
	ArrayIndexT theCount = 0;
	TArrayIterator<LWindow*> theIterator( mWindows );
	while ((theCount < inIndex) && theIterator.Next( theWindow ))
		if (theWindow != nil)
			theCount++;
			
	// should not be nil here... assert that.
	SignalIf_( theWindow == nil );
	
	// return result.
	return theWindow;
}


// ---------------------------------------------------------------------------
//		¥ GetNumberOfWindows
// ---------------------------------------------------------------------------
// return the number of windows currently tracked.

UInt32
CWindowTracker::GetNumberOfWindows() const
{
	return mWindowsCount;
}


// ---------------------------------------------------------------------------
//		¥ GetWindowList
// ---------------------------------------------------------------------------
// copies our list of tracked windows in outWindowList.
// note: windows are appended to outWindowList; list is not emptied first.

void
CWindowTracker::GetWindowList(
	TArray<LWindow*>&	outWindowList ) const
{
	TArrayIterator<LWindow*> theIterator( mWindows );
	LWindow* theWindow;
	while (theIterator.Next( theWindow ))
		if (theWindow != nil)
			outWindowList.AddItem( theWindow );
}


#pragma mark -


// ---------------------------------------------------------------------------
//		¥ AddWindowInSpot									[protected]
// ---------------------------------------------------------------------------
// adds the given window to the given spot in the array.
// if inIndex is index_Last, adds the window at the end of the array.

void
CWindowTracker::AddWindowInSpot(
	LWindow*		inWindow,
	ArrayIndexT		inIndex )
{
	// add the window.
	if (inIndex == LArray::index_Last)
		mWindows.AddItem( inWindow );
	else
	{
		ThrowIfNot_( mWindows.ValidIndex( inIndex ) );
		mWindows[inIndex] = inWindow;
	}
	
	// increment windows count.
	mWindowsCount++;
	
	// update the menu. refresh menu bar.
	UpdateMenu();
	LCommander::SetUpdateCommandStatus( true );
}


// ---------------------------------------------------------------------------
//		¥ RemoveWindowFromSpot								[protected]
// ---------------------------------------------------------------------------
// removes the window that's in the given spot in the array.
// if that spot is in the first 10 spots, doesn't remove the spot itself.

void
CWindowTracker::RemoveWindowFromSpot(
	ArrayIndexT		inIndex )
{
	// remove the window.
	if (inIndex <= 10)
		mWindows[inIndex] = nil;
	else
	{
		ThrowIfNot_( mWindows.ValidIndex( inIndex ) );
		mWindows.RemoveItemsAt( 1, inIndex );
	}
	
	// decrement windows count.
	mWindowsCount--;
	
	// update the menu. refresh menu bar.
	UpdateMenu();
	LCommander::SetUpdateCommandStatus( true );
}


#pragma mark -


// ---------------------------------------------------------------------------
//		¥ ExecuteSelf
// ---------------------------------------------------------------------------
// manages the bulk of the work in managing the Window menu.

void
CWindowTracker::ExecuteSelf(
	MessageT		inMessage,
	void*			ioParam )
{
	// if we don't have a menu, just quit, we don't care about events.
	if (mMenuID == 0)
		return;
		
	// if we have to manage a menu but don't have the LMenu* pointer,
	// try to get it here.
	if ((mMenuID != 0) && (mMenu == nil))
		FindLMenuPointer();
	
	// whatever happens, we can't go on from here on without the LMenu* pointer.
	if (mMenu == nil)
		return;
		
	// if mInitialItemsCount is -1 here, it wasn't initialized, so do it now.
	// this variable will hold the number of items in the menu before we
	// add our own items.
	if (mInitialItemsCount == -1)
		mInitialItemsCount = ::CountMenuItems( mMenu->GetMacMenuH() );
		
	// assume the host can execute itself after we're done.
	SetExecuteHost( true );
		
	// ok, check message type.
	switch (inMessage)
	{
		case msg_Event:
			// we'll update the menu when a window is activated.
			{
				EventRecord* theEvent = reinterpret_cast<EventRecord*> (ioParam);
				if ((theEvent->what == activateEvt) &&
					(theEvent->modifiers & activeFlag))
				{
					// the window being activated is not *currently* active but
					// we want to treat it as being active anyway, so find that
					// window now and pass it to UpdateMenu.
					WindowPtr theWindowPtr = reinterpret_cast<WindowPtr> (theEvent->message);
					LWindow* theWindow = LWindow::FetchWindowObject( theWindowPtr );
				
					UpdateMenu( theWindow );
				}
			}
			break;
			
		case msg_CommandStatus:
			// we can't set the command status of synthetic menu items from an attachment.
			// normally this is ok since synthetic menu items are active by default.
			// however, if the menu is empty to begin with or has no active item other
			// that our items, the menu itself will be unused and thus deactivated
			// by PowerPlant. to correct this, we mark it as used when we see that
			// PP is asking us for a command status update.
			if (mWindowsCount > 0)	// only activate the menu if we have items.
				mMenu->SetUsed( true );
			break;
			
		default:
			{
				// check if it's a synthetic command.
				ResIDT theMenuID;
				SInt16 theMenuItem;
				if (LCommander::IsSyntheticCommand( inMessage, theMenuID, theMenuItem ) &&
					(theMenuID == mMenuID))
				{
					// it is, gotta do something about it.
					// see if the index is valid.
					if (IsMenuItemOurs( theMenuItem ))
					{
						// select the given window.
						LWindow* theWindow = GetWindow( theMenuItem );
						
						// select that window. show it if it was hidden.
						theWindow->Show();
						theWindow->Select();
						
						// hey bud, we're cool. no need to kick in, 'k?
						SetExecuteHost( false );
					}
				}
			}
			break;
	}
}


#pragma mark -


// ---------------------------------------------------------------------------
//		¥ FindLMenuPointer
// ---------------------------------------------------------------------------
// finds the LMenu* pointer from our menu ID.
// avoids code diplication.

void
CWindowTracker::FindLMenuPointer()
{
	LMenuBar* theMenuBar = LMenuBar::GetCurrentMenuBar();
	if (theMenuBar != nil)
	{
		mMenu = theMenuBar->FetchMenu( mMenuID );
		
		// if menu is not present, it's probably a programmer error.
		SignalIf_( mMenu == nil );
	}
}


// ---------------------------------------------------------------------------
//		¥ IsMenuItemOurs
// ---------------------------------------------------------------------------
// returns true if ioMenuItem is an item that is ours in our managed menu.
// use this to determine if an item returned by IsSyntheticCommand is ours.
//
// upon exit, ioMenuItem will point to the index of the item, relative
// to our first index - i.e., if the selected item is our first item in
// the menu (after any items that were already there), then ioMenuItem will be 1.

bool
CWindowTracker::IsMenuItemOurs(
	SInt16&	ioMenuItem ) const
{
	// substract initial items count.
	ioMenuItem -= mInitialItemsCount;
	
	// if there were any items, we added a separator, so substract it.
	if (mInitialItemsCount > 0)
		ioMenuItem--;
		
	// item is ours if it's above 0 now.
	return (ioMenuItem > 0);
}


// ---------------------------------------------------------------------------
//		¥ UpdateMenu
// ---------------------------------------------------------------------------
// updates the menu we're managing with the known windows.
//
// note: when calling this on an activateEvt for a given window,
// the window will not be active until *after* the event is processed.
// to allow us to properly identify the window being activated, pass
// its LWindow* value as inActivatedWindow.
// (this parameter defaults to nil for when you're calling this at other times.)

void
CWindowTracker::UpdateMenu(
	LWindow*	inActivatedWindow )
{
	MenuHandle theMenuH = mMenu->GetMacMenuH();
	SInt16 numItems = ::CountMenuItems( theMenuH );

	// empty the menu of any items we added.
	while (numItems > mInitialItemsCount)
	{
		// delete last entry in the menu.
		::DeleteMenuItem( theMenuH, numItems );
		numItems--;
	}
	
	// if we need to add entries and the menu is not empty, add a separator.
	if ((mInitialItemsCount > 0) && (mWindowsCount > 0))
	{
		MacAppendMenu( theMenuH, "\p(-" );
		numItems++;
	}
		
	// now add our menu items.
	TArrayIterator<LWindow*> theIterator( mWindows );
	LWindow* theWindow = nil;
	while (theIterator.Next( theWindow ))
		if (theWindow != nil)
		{
			// add the item.
			Str255 theTitle;
			theWindow->GetDescriptor( theTitle );
			::MacAppendMenu( theMenuH, "\p." );
			numItems++;
			::SetMenuItemText( theMenuH, numItems, theTitle );
			
			// if this is the active window, add a checkmark.
			if (theWindow->IsActive() || (theWindow == inActivatedWindow))
				::SetItemMark( theMenuH, numItems, checkMark );
			
			// if this is a window among the first 10 spots, assign it a command key.
			ArrayIndexT theIndex = theIterator.GetCurrentIndex();
			if (theIndex <= 10)
				::SetItemCmd( theMenuH, numItems, (CharParameter) (((short) '0') + theIndex) );
		}
}
