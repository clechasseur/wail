// =================================================================================
//	CPreferences.h					©1999, Charles Lechasseur
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

#ifndef __CPREFERENCES__
#define __CPREFERENCES__
#pragma once

#include <LPreferencesFile.h>


// struct to store info about a prefs field.
typedef struct SPreferencesField
{
	void*	mFieldP;		// pointer to the field variable.
	void*	mFieldDefault;	// points to the field's default value. we must dispose of this!
	SInt32	mFieldSize;		// size of this field's data (if 0, field is a Handle).
	SInt16	mFieldID;		// field ID. must be unique.

} SPreferencesField;

// struct to store info about a prefs saved field.
typedef struct SPreferencesSavedField
{
	void*	mFieldState;	// points to the field's state. we must dispose of this!
	SInt32	mFieldSize;		// size of this field's data (if 0, field is a Handle).

} SPreferencesSavedField;


// default type for prefs file:
const OSType	fileType_Prefs			= 'pref';


// CPreferences class.

class CPreferences
{
	public:
		//Constructor
							CPreferences(
								ConstStr255Param	inPrefsFileName,
								OSType				inCreator,
								OSType				inFileType = fileType_Prefs,
								Boolean				inCreatePrefsFolder = false );
		//Destructor
		virtual				~CPreferences();
		
		
		// --- field management ---
		
		void				RegisterField(
								void*		inField,
								void*		inFieldDefault,
								SInt32		inFieldSize,
								SInt16		inFieldID )
									{ RegisterPtrField( inField, inFieldDefault,
														inFieldSize, inFieldID ); }
		
		virtual void		RegisterPtrField(
								void*		inField,
								void*		inFieldDefault,
								SInt32		inFieldSize,
								SInt16		inFieldID );
		virtual void		RegisterHandleField(
								Handle		inField,
								Handle		inFieldDefault,
								SInt16		inFieldID );
		
		virtual Boolean		IsFieldIDUnique(
								SInt16		inFieldID ) const;
								
		// --- reading/writing prefs ---
		
		virtual OSErr		Read();
		virtual OSErr		Write();
		virtual OSErr		Reset();
		
		// --- state saving/restoring ---
		
		virtual OSErr		SaveState();
		virtual OSErr		RestoreState();
		
		// --- utility functions ---
		
		virtual Boolean		HasSavedState() const;
		
		virtual Boolean		FileExists() const;
		
	protected:
	
		// Protected functions
		
		virtual OSErr		ReadSelf();
		virtual OSErr		WriteSelf();
		virtual OSErr		ResetSelf();
		
		virtual OSErr		ReadField(
								SInt16	inFieldIndex );
		virtual OSErr		WriteField(
								SInt16	inFieldIndex );
		virtual OSErr		ResetField(
								SInt16	inFieldIndex );
		
		virtual OSErr		SaveStateSelf();
		virtual OSErr		RestoreStateSelf();
		
		virtual OSErr		SaveFieldState(
								SInt16	inFieldIndex );
		virtual OSErr		RestoreFieldState(
								SInt16	inFieldIndex );
		
		void				DeleteFieldQueue();
		void				DeleteSavedStateQueue();
		
	private:
		// Member Variables and Classes
		
		LPreferencesFile*					mPrefsFile;
		OSType								mCreator;
		OSType								mFileType;

		TArray<SPreferencesField*>			mFields;
		
		TArray<SPreferencesSavedField*>		mSavedState;	
	
		// Defensive programming. No copy constructor nor operator=
							CPreferences(const CPreferences&);
		CPreferences&			operator=(const CPreferences&);
};


// templated RegisterField function:

template<class T>
void RegisterField(
	CPreferences&		inPreferences,
	T&					inField,
	const T&			inFieldDefault,
	SInt16				inFieldID )
{
	// find out field data space.
	SInt32 fieldSize = sizeof(T);
	
	// allocate a pointer to store the default data.
	StPointerBlock fieldDefault( fieldSize );
	
	// store default data in the pointer.
	::BlockMoveData( &inFieldDefault, fieldDefault, fieldSize );
	ThrowIfMemError_();
	
	// call the object's routine to register the field.
	inPreferences.RegisterPtrField( &inField, fieldDefault.Release(), fieldSize, inFieldID );
}


#endif //__CPREFERENCES__
