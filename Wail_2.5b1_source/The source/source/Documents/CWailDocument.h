// =================================================================================
//	CWailDocument.h					�1998, Charles Lechasseur
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

#include <LSingleDoc.h>

class CWailDocument : public LSingleDoc {
public:
						CWailDocument(
							LCommander 		*inSuper,
							FSSpec 			*inFileSpec,
							OSType			inFileType = fileType_Default,
							OSType			inFileCreator = fileType_Default );

	virtual Boolean		IsModified();
	
	virtual void		DoAESave( FSSpec &inFileSpec, OSType inFileType );
	virtual void		DoSave();
	virtual void		DoRevert();
	
	virtual OSType		GetFileType() const;
	
	// Command handling
	
	virtual Boolean		ObeyCommand(
								CommandT		inCommand,
								void			*ioParam);
	virtual void		FindCommandStatus(
								CommandT		inCommand,
								Boolean			&outEnabled,
								Boolean			&outUsesMark,
								UInt16			&outMark,
								Str255			outName);

protected:

	void				NameNewDoc();
	
	void				CreateNewDoc();
	void				OpenFile( FSSpec &inFileSpec );
	
	// Custom command handlers
	
	Boolean				MakeShuttle();
	
private:

	// data members:
	
	Boolean				mIsShuttle;		// is our document a shuttle? not yet supported.
	
	OSType				mFileType;		// document file type
	OSType				mFileCreator;	// and creator
};