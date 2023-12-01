// =================================================================================
//	UCode99.cp					©2000, Charles Lechasseur
// =================================================================================
//
// utility functions for broadcasters and listeners.
//
// wondering why Code99? it's a little like UReanimator. ask a doctor :)

/* Copyright (c) 2002, Charles Lechasseur
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

#include "UCode99.h"


// ---------------------------------------------------------------------------
//		¥ LinkListenerToBroadcasters
// ---------------------------------------------------------------------------
// a lot like UReanimator::LinkListenerToBroadcasters,
// only it doesn't rely on resources and really links ALL broadcasters
// in the given view to the given listener.

void
UCode99::LinkListenerToBroadcasters(
	LListener*			inListener,
	LView*				inView )
{
	// null pointers are probably a programmer error.
	SignalIf_( inListener == NULL );
	SignalIf_( inView == NULL );
	
	// iterate the view's subpanes.
	TArrayIterator<LPane*> theIterator( inView->GetSubPanes() );
	LPane* thePane;
	while (theIterator.Next( thePane ))
	{
		// check if this pane is a view.
		LView* theView = dynamic_cast<LView*> (thePane);
		if (theView != NULL)
		{
			// it's a view, so call this function recursively.
			LinkListenerToBroadcasters( inListener, theView );
		}
		
		// check if this pane is a broadcaster.
		LBroadcaster* theBroadcaster = dynamic_cast<LBroadcaster*> (thePane);
		if (theBroadcaster != NULL)
		{
			// link the listener to this broadcaster.
			theBroadcaster->AddListener( inListener );
		}
	}
	
	// check if the view itself is a broadcaster.
	LBroadcaster* theBroadcaster = dynamic_cast<LBroadcaster*> (inView);
	if (theBroadcaster != NULL)
	{
		// link the listener to this broadcaster.
		theBroadcaster->AddListener( inListener );
	}
}