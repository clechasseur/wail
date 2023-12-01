// =================================================================================
//	CSmartPeriodical.cp					©1999, Charles Lechasseur
// =================================================================================
//
// A periodical that adds two new methods to find out if it's currently idling or
// repeating. this does not check the queue, however - it checks a flag that's
// modified every time the periodical is started/stopped.

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

#include "CSmartPeriodical.h"


// ---------------------------------------------------------------------------
//	¥ CSmartPeriodical							Constructor	[public]
// ---------------------------------------------------------------------------

CSmartPeriodical::CSmartPeriodical()
	: mIsIdling( false ),
	  mIsRepeating( false )
{
}


// ---------------------------------------------------------------------------
//	¥ ~CSmartPeriodical							Destructor	[public]
// ---------------------------------------------------------------------------

CSmartPeriodical::~CSmartPeriodical()
{
}


// ---------------------------------------------------------------------------
//	¥ StartIdling													  [public]
// ---------------------------------------------------------------------------
//	Put Periodical in the Idler queue and mark it as Idling

void
CSmartPeriodical::StartIdling()
{
	// call inherited.
	LPeriodical::StartIdling();
	
	// mark as Idling
	mIsIdling = true;
}


// ---------------------------------------------------------------------------
//	¥ StopIdling													  [public]
// ---------------------------------------------------------------------------
//	Remove Periodical from the Idler queue and remove Idling flag

void
CSmartPeriodical::StopIdling()
{
	// call inherited.
	LPeriodical::StopIdling();
	
	// mark as not Idling
	mIsIdling = false;
}


// ---------------------------------------------------------------------------
//	¥ StartRepeating												  [public]
// ---------------------------------------------------------------------------
//	Put Periodical in the Repeater queue and mark it as Repeating

void
CSmartPeriodical::StartRepeating()
{
	// call inherited.
	LPeriodical::StartRepeating();
	
	// mark as Repeating.
	mIsRepeating = true;
}


// ---------------------------------------------------------------------------
//	¥ StopRepeating													  [public]
// ---------------------------------------------------------------------------
//	Remove Periodical from the Repeater queue and remove Repeating flag

void
CSmartPeriodical::StopRepeating()
{
	// call inherited.
	LPeriodical::StopRepeating();
	
	// mark as not Repeating
	mIsRepeating = false;
}


// the following two functions are inlined in the header file:
#pragma mark CSmartPeriodical::IsIdling
#pragma mark CSmartPeriodical::IsRepeating