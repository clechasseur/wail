// =================================================================================
//	CTrackableWindow.cp					©2000, Charles Lechasseur
// =================================================================================
//
// a window that can be tracked via a CWindowTracker attachment.
// any window to be tracked by the CWindowTracker must inherit from this class
// instead of LWindow. it can be safely substituted for LWindow anywhere
// since it has the same constructors.
//
// it should also be possible to register CTrackableWindow with LWindow's class ID
// so that all LWindows created from a stream would be CTrackableWindows instead.
// instead of the following line in your app's constructor:
//
// RegisterClass_(LWindow);
//
// use the following:
//
// RegisterClassID_(CTrackableWindow, LWindow::class_ID);
//
// this has not been tested. note that it only works when creating a window from a PPob
// that defines the window as an LWindow with class ID 'wind'. if you changed that class ID
// (so that the window is a custom window), then the custom window's class must itself
// inherit from CTrackableWindow.
//
// NOTE: to do its magic, make sure that this class' FinishCreateSelf method gets called.

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

#include "CTrackableWindow.h"

#include "CWindowTracker.h"


// ---------------------------------------------------------------------------
//	¥ CTrackableWindow						Constructor	[public]
// ---------------------------------------------------------------------------

CTrackableWindow::CTrackableWindow()
	: LWindow()
{
	mWindowTracker = nil;
}


// ---------------------------------------------------------------------------
//	¥ CTrackableWindow						Constructor	[public]
// ---------------------------------------------------------------------------

CTrackableWindow::CTrackableWindow(
	const SWindowInfo&	inWindowInfo )
	: LWindow( inWindowInfo )
{
	mWindowTracker = nil;
}


// ---------------------------------------------------------------------------
//	¥ CTrackableWindow						Constructor	[public]
// ---------------------------------------------------------------------------

CTrackableWindow::CTrackableWindow(
	ResIDT				inWINDid,
	UInt32				inAttributes,
	LCommander*			inSuperCommander )
	: LWindow( inWINDid, inAttributes, inSuperCommander )
{
	mWindowTracker = nil;
}


// ---------------------------------------------------------------------------
//	¥ CTrackableWindow						Constructor	[public]
// ---------------------------------------------------------------------------

CTrackableWindow::CTrackableWindow(
	LCommander*			inSuperCommander,
	const Rect&			inGlobalBounds,
	ConstStringPtr		inTitle,
	SInt16				inProcID,
	UInt32				inAttributes,
	WindowPtr			inBehind )
	: LWindow( inSuperCommander, inGlobalBounds, inTitle, inProcID, inAttributes, inBehind )
{
	mWindowTracker = nil;
}


// ---------------------------------------------------------------------------
//	¥ CTrackableWindow						Constructor	[public]
// ---------------------------------------------------------------------------

CTrackableWindow::CTrackableWindow(
	LStream*			inStream )
	: LWindow( inStream )
{
	mWindowTracker = nil;
}


// ---------------------------------------------------------------------------
//		¥ FinishCreateSelf
// ---------------------------------------------------------------------------
// adds the window to the list of window tracked by a CWindowTracker, if present.
// remembers that window tracker so we can remove the window from it in our destructor.

void
CTrackableWindow::FinishCreateSelf()
{
	// try to get a window tracker.
	mWindowTracker = CWindowTracker::GetCurrentWindowTracker();
	if (mWindowTracker != nil)
	{
		// we have a tracker, register with it.
		mWindowTracker->AddWindow( this );
	}
}


// ---------------------------------------------------------------------------
//	¥ ~CTrackableWindow							Destructor	[public]
// ---------------------------------------------------------------------------
// if the window was added to a CWindowTracker when constructed, removes
// the window from that window tracker.

CTrackableWindow::~CTrackableWindow()
{
	if (mWindowTracker != nil)
		mWindowTracker->RemoveWindow( this );
}
