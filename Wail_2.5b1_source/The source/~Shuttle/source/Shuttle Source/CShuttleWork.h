// =================================================================================
//	CShuttleWork.h					©1999, Charles Lechasseur
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

#include <LCommander.h>

#include "CWailSoundFileData.h"

#include "CWailProgressDialog.h"
#include "CInFileStream.h"


// CShuttleWork class

class CShuttleWork : public LCommander
{
	public:
	// Public Functions
		//Default Constructor
							CShuttleWork(
									const FSSpec&				inFile,
									LCommander					*inSuper );
		//Destructor
		virtual				~CShuttleWork();
		
	// Other functions
	
		virtual Boolean		DoWork();
		
	protected:
	// Protected functions
	
	// Initialization of the patch file
	
		void				CreatePatchFile();
	
	// Installation steps
	
		void				AskForDestinationFile();
		SInt32				AskForInstallationMethod();
		void				InstallHeader();
		void				InstallClass(
									SInt16						inWhichSet,
									SInt16						inWhichClass );
	
	// Marathon-related
		void				ReadSoundFileHeader(
									LStream						*inFromWhere,
									SMthonSoundHeader&			outHeader );
		void				WriteSoundFileHeader(
									LStream						*inToWhere,
									const SMthonSoundHeader&	inHeader );
									
		void				ReadSoundClass(
									LStream						*inFromWhere,
									SInt16						inWhichSet,
									SInt16						inWhichClass,
									SInt16						inMaxNumClases,
									SMthonSoundClass&			outClass );
		void				WriteSoundClass(
									LStream						*inToWhere,
									SInt16						inWhichSet,
									SInt16						inWhichClass,
									SInt16						inMaxNumClasses,
									const SMthonSoundClass&		inClass );
									
		void				ReadSound(
									LStream						*inFromWhere,
									SInt32						inOffset,
									SInt32						inSize,
									Ptr&						outSound );
		void				WriteSound(
									LStream						*inToWhere,
									SInt32						inOffset,
									SInt32						inSize,
									Ptr							inSound );
		
	private:
		// Member Variables and Classes

		LFileStream			*mSourceFile;
		CInFileStream		*mPatchFile;
							
		LFileStream			*mDestinationFile;
		Boolean				mOverwriteSource;
		Boolean				mDestinationFileExists;
		
		CWailProgressDialog	*mProgressDialog;
		Boolean				mProgressDialogInitialized;
		SInt16				mNumClasses;
		SInt16				mNumClassesSource;
		SInt16				mNumClassesShuttle;
		
		SInt16				mCurrentSet;
		SInt16				mCurrentClass;
		SInt16				mCurrentSound;
		SInt32				mCurrentOffset;
		
		Boolean				mIsDone;
	
		// Defensive programming. No copy constructor nor operator=
							CShuttleWork(const CShuttleWork&);
		CShuttleWork&		   operator=(const CShuttleWork&);
};
