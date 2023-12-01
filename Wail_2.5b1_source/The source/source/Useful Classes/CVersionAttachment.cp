// =================================================================================
//	CVersionAttachment.cp					©2000, Charles Lechasseur
// =================================================================================
//
// an attachment that replaces the descriptor of any pane with
// the version number of the application, either taken from the short string
// or made up of the various versions contained in the 'vers' resource ID 1.

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

#include "CVersionAttachment.h"


// character constants for the various stages.

const char	stage_Development		= 'd';
const char	stage_Alpha				= 'a';
const char	stage_Beta				= 'b';
const char	stage_Final				= 'f';


// ---------------------------------------------------------------------------
//	¥ CVersionAttachment						Constructor	[public]
// ---------------------------------------------------------------------------

CVersionAttachment::CVersionAttachment(
	bool	inUseShortString )
	: LAttachment( msg_FinishCreate, true ),
	  mUseShortString( inUseShortString )
{
}


// ---------------------------------------------------------------------------
//	¥ CVersionAttachment					Stream Constructor	[public]
// ---------------------------------------------------------------------------

CVersionAttachment::CVersionAttachment(
	LStream *inStream )
	: LAttachment( inStream )
{
	// read params.
	*inStream >> mUseShortString;

	// we only listen to msg_FinishCreate.
	SetMessage( msg_FinishCreate );
}


// ---------------------------------------------------------------------------
//	¥ ~CVersionAttachment							Destructor	[public]
// ---------------------------------------------------------------------------

CVersionAttachment::~CVersionAttachment()
{
}


// ---------------------------------------------------------------------------
//		¥ ExecuteSelf
// ---------------------------------------------------------------------------

void
CVersionAttachment::ExecuteSelf(
	MessageT		inMessage,
	void*			ioParam )
{
#pragma unused( ioParam )

	// ignore any message except finish create.
	if (inMessage == msg_FinishCreate)
	{
		// get 'vers' resource ID 1 and lock it.
		StResource theVersResource( 'vers', 1 );
		StHandleLocker theVersLocker( theVersResource );
		
		// get the VersRec* from that handle.
		VersRec* theVersRecP = reinterpret_cast<VersRec*> (*theVersResource);
		
		// get necessary version string.
		LStr255 theVersionString;
		if (mUseShortString)
			theVersionString = theVersRecP->shortVersion;
		else
			GetVersionString( theVersRecP->numericVersion, theVersionString );
			
		// set our parent's descriptor.
		LPane* theParentPane = dynamic_cast<LPane*> (GetOwnerHost());
		if (theParentPane != nil)
			theParentPane->SetDescriptor( theVersionString );
		
		// delete this object since it's no longer needed.
		delete this;
	}
}


#pragma mark -


// ---------------------------------------------------------------------------
//		¥ GetVersionString									[static]
// ---------------------------------------------------------------------------
// given a NumVersion record from a 'vers' resource, returns
// the version string, using the "standard" notation.
//
// inDotSymbol will be used as the dot symbol. it defaults to '.' (duh).

void
CVersionAttachment::GetVersionString(
	const NumVersion&	inNumVersion,
	StringPtr			outVersionString,
	SInt16				inVersionStringSize,
	char				inDotSymbol )
{
	LStr255 theString;
	
	// majorRev field of the NumVersion struct holds the major revision number in BCD.
	// translate that to decimal by taking the 4 high bytes, multiplying them by 10,
	// and add that to the 4 low bytes.
	// then add that to the string.
	SInt32 theMajorRevision = (((inNumVersion.majorRev & 0xF0) >> 4) * 10)
							  + (inNumVersion.majorRev & 0x0F);
	theString += theMajorRevision;
	
	// add a dot between major rev and minor rev.
	theString += inDotSymbol;
	
	// the minor revision is in the higher 4 bytes of minorAndBugRev.
	SInt32 theMinorRevision = (inNumVersion.minorAndBugRev & 0xF0) >> 4;
	theString += theMinorRevision;
	
	// figure out bug revision. we'll add that to the string only if it's not 0.
	// it's stored in the lower 4 bytes of minorAndBugRev.
	SInt32 theBugRevision = (inNumVersion.minorAndBugRev & 0x0F);
	if (theBugRevision != 0)
	{
		// add a dot symbol.
		theString += inDotSymbol;
		
		// add the bug revision.
		theString += theBugRevision; 
	}
	
	// check out the stage character.
	char theStageChar = '\0';
	if (inNumVersion.stage == developStage)
		theStageChar = stage_Development;
	else if (inNumVersion.stage == alphaStage)
		theStageChar = stage_Alpha;
	else if (inNumVersion.stage == betaStage)
		theStageChar = stage_Beta;
	else if (inNumVersion.stage == finalStage)
		theStageChar = stage_Final;
		
	// we'll only print the non-release revision if we're not in the final stage.
	if ((theStageChar != '\0') && (theStageChar != stage_Final))
	{
		// add the stage character.
		theString += theStageChar;
		
		// add the non-release revision. this one's not in BCD.
		SInt32 theNonReleaseRevision = inNumVersion.nonRelRev;
		theString += theNonReleaseRevision;
	}
	
	// copy our string in the output string before exiting.
	LString::CopyPStr( theString, outVersionString, inVersionStringSize );
}