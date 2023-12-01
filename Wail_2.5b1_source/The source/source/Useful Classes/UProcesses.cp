// =================================================================================
//	UProcesses.cp					©2003, Charles Lechasseur
// =================================================================================
//
// Utility classes and functions related to processes.

/* Copyright (c) 2003, Charles Lechasseur
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

#include "UProcesses.h"


// =================================================================================
//		UProcesses class
// =================================================================================
// Various utility functions related to processes.


// ---------------------------------------------------------------------------
//		¥ GetProcessAppSpec
// ---------------------------------------------------------------------------
// returns the FSSpec of the application file of the given process.

OSErr
UProcesses::GetProcessAppSpec(
	const ProcessSerialNumber&	inPSN,
	FSSpec&						outAppSpec )
{
	// fill process info record.
	ProcessInfoRec theProcessInfo;
	theProcessInfo.processInfoLength = sizeof(ProcessInfoRec);
	theProcessInfo.processName = nil;
	theProcessInfo.processAppSpec = &outAppSpec;
	
	// get process info.
	return ::GetProcessInformation( &inPSN, &theProcessInfo );
}


// ---------------------------------------------------------------------------
//		¥ GetCurrentProcessAppSpec
// ---------------------------------------------------------------------------
// returns the FSSpec of the application file of this process.

OSErr
UProcesses::GetCurrentProcessAppSpec(
	FSSpec&	outAppSpec )
{
	// get serial number of this process.
	ProcessSerialNumber thePSN;
	OSErr err = ::GetCurrentProcess( &thePSN );
	if (err == noErr)
	{
		// got our PSN, get our spec.
		err = GetProcessAppSpec( thePSN, outAppSpec );
	}
	
	return err;
}
