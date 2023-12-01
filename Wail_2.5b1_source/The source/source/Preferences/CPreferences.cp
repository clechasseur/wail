// =================================================================================
//	CPreferences.cp					©1999, Charles Lechasseur
// =================================================================================
//
// Class to automatically handle reading and writing preferences to a file
// stored in the Preferences folder.
//
// The main idea for this class is to automate as much as possible.
// it's designed so that you shouldn't need to override it *at all*.
// however, this means that some setup work is needed.
//
// this class maintains a list of "fields". these are essentially the variables
// where preferences will be stored in your application code. note that these
// are NOT stored by this class - this class merely maintains a list of *pointers*
// to your fields. this means that those fields must either be global or last for
// the entire scope of existence of the object of this class.
//
// to use this class, first create an instance of this class to be shared by
// everyone in your application. next, you must register all the fields you're
// going to use in your app.
//
// to do this, you must call either the RegisterPtrField or RegisterHandleField
// public functions. the RegisterPtrField function takes four parameters:
//
//	1. a pointer to the variable where the field is going to be stored.
//	2. a pointer to memory where the field default value[1] is stored (see below).
//	   this memory must be allocated via NewPtr and is de-allocated by CPreferences.
//	3. the field's data size. pass sizeof(fieldType).
//	4. the field ID. each field must have a unique ID that is a 2-bytes signed integer.
//	   this is how CPreferences manages the fields inside the prefs file.
//
// the RegisterHandleField function takes three parameters:
//
//	1. a Handle where the field is going to be stored.
//	2. a Handle where the field default value[1] is stored (see below).
//	   this memory must be allocated via NewHandle and is de-allocated by CPreferences.
//	3. the field ID (see above).
//
// [1] the default value is the value to set the field to if the prefs file doesn't exist.
// please note that this is done automatically by this class whenever Read is called and
// the prefs file doesn't exist.
//
// since the way of registering pointer fields is not very practical, you can use the little
// inlined function RegisterField that is templated. you'd use it like this:
//
//	RegisterField(CPreferencesObject, fieldVar, fieldDefault, fieldID);
//
// (the function is declared outside the class since CW Pro 3 doesn't seem to support
//  templated class member functions. ?)
//
// after you have set up all the fields, you can use the class pretty seamlessly through
// its public functions:
//
//	Read will read in the prefs from the prefs file and store them in the fields vars.
//	Write will dump the content of the field vars to the prefs file on disk.
//	Reset will reset the field vars to default value.
//
//	SaveState will save the field vars's state. this is useful with...
//	RestoreState, that will restore the field vars from a previously saved state.
//
// why use those two last functions? with a Prefs dialog. at the beginning of the
// dialog, call SaveState. when a field changes in the dialog, change its field var
// directly. then, if the user agrees, call Write; if he doesn't, just call RestoreState
// and you're done!

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


#include "CPreferences.h"

#include "StOldResFile.h"


// constants
const ResType	resType_PtrPrefsField		= 'PFld';
const ResType	resType_HandlePrefsField	= 'HFld';


// ---------------------------------------------------------------------------
//	¥ CPreferences	[public]
// ---------------------------------------------------------------------------
// simply initializes the prefs file to point to the correct file.
//
// little note: the default value for inCreatePrefsFolder is false, matching
// LPreferencesFile's behavior - however this sounds very weird if you ask me. :P

CPreferences::CPreferences(
	ConstStr255Param	inPrefsFileName,
	OSType				inCreator,
	OSType				inFileType,
	Boolean				inCreatePrefsFolder )
{
	// store type and creator.
	mCreator = inCreator;
	mFileType = inFileType;

	// create the prefs file object.
	mPrefsFile = new LPreferencesFile( inPrefsFileName, inCreatePrefsFolder );
	ThrowIfNil_( mPrefsFile );
}


// ---------------------------------------------------------------------------
//	¥ ~CPreferences	[public]
// ---------------------------------------------------------------------------
// simply deletes the prefs file object. this closes it if needed.
// also deletes the field and saved state queue.

CPreferences::~CPreferences()
{
	if (mPrefsFile != nil)
		delete mPrefsFile;
		
	// delete field queue.
	DeleteFieldQueue();
	
	// delete saved state queue.
	DeleteSavedStateQueue();
}


// ---------------------------------------------------------------------------------
//		¥ RegisterPtrField	[public]
// ---------------------------------------------------------------------------------
// adds the given pointer field to our field queue.
//
// we assume ownership of inFieldDefault; it must have been allocated via NewPtr.
//
// call the templated RegisterPtrField function for ease of use in calling this function.

void
CPreferences::RegisterPtrField(
	void*		inField,
	void*		inFieldDefault,
	SInt32		inFieldSize,
	SInt16		inFieldID )
{
	// can't have two fields that have the same ID.
	if (IsFieldIDUnique( inFieldID ))
	{
		// create a struct to store the data and save it in the queue.
		SPreferencesField* theField = new SPreferencesField;
		theField->mFieldP = inField;
		theField->mFieldDefault = inFieldDefault;
		theField->mFieldSize = inFieldSize;
		theField->mFieldID = inFieldID;
		
		mFields.AddItem( theField );
	}
	else
	{
		// hopefully, two fields with similar IDs is a programmer error.
		// we'll signal here and get rid of the inFieldDefault variable.
		SignalStringLiteral_( "Duplicate Preferences Field ID" );
		
		::DisposePtr( static_cast<Ptr>(inFieldDefault) );
	}
}


// ---------------------------------------------------------------------------------
//		¥ RegisterHandleField	[public]
// ---------------------------------------------------------------------------------
// adds the given Handle field to our field queue.
//
// we assume ownership of inFieldDefault; it must have been allocated via NewHandle.

void
CPreferences::RegisterHandleField(
	Handle		inField,
	Handle		inFieldDefault,
	SInt16		inFieldID )
{
	// can't have two fields that have the same ID.
	if (IsFieldIDUnique( inFieldID ))
	{
		// create a struct to store the data and save it in the queue.
		SPreferencesField* theField = new SPreferencesField;
		theField->mFieldP = inField;
		theField->mFieldDefault = inFieldDefault;
		theField->mFieldSize = 0;
		theField->mFieldID = inFieldID;
		
		mFields.AddItem( theField );
	}
	else
	{
		// hopefully, two fields with similar IDs is a programmer error.
		// we'll signal here and get rid of the inFieldDefault variable.
		SignalStringLiteral_( "Duplicate Preferences Field ID" );
		
		::DisposeHandle( static_cast<Handle>(inFieldDefault) );
	}
}


// ---------------------------------------------------------------------------------
//		¥ IsFieldIDUnique	[public]
// ---------------------------------------------------------------------------------
// checks if the given field ID is unique.
// field IDs must be unique for this class to work.

Boolean
CPreferences::IsFieldIDUnique(
	SInt16		inFieldID ) const
{
	// check in the entire queue. if we find a field that bears this ID, we stop.
	Boolean isUnique = true;
	TArrayIterator<SPreferencesField*> iterator( mFields );
	SPreferencesField* theField;
	while (iterator.Next( theField ))
	{
		isUnique = theField->mFieldID != inFieldID ;
		if (isUnique)
			break;
	}
		
	return isUnique;
}


// ---------------------------------------------------------------------------------
//		¥ Read	[public]
// ---------------------------------------------------------------------------------
// if the prefs file exists, reads the prefs from it.
// if it doesn't exist, resets the prefs to default (without writing them to disk).

OSErr
CPreferences::Read()
{
	// if file exists, read from it.
	if (FileExists())
		return ReadSelf();
	else
		// file doesn't exist. just reset.
		Reset();
		
	return noErr;
}


// ---------------------------------------------------------------------------------
//		¥ Write	[public]
// ---------------------------------------------------------------------------------
// writes the prefs back to the prefs file on disk.

OSErr
CPreferences::Write()
{
	return WriteSelf();
}


// ---------------------------------------------------------------------------------
//		¥ Reset	[public]
// ---------------------------------------------------------------------------------
// reset prefs to default.

OSErr
CPreferences::Reset()
{
	return ResetSelf();
}
		

// ---------------------------------------------------------------------------------
//		¥ SaveState	[public]
// ---------------------------------------------------------------------------------
// save the state.

OSErr
CPreferences::SaveState()
{
	// if there's already a saved state, delete it first.
	if (HasSavedState())
		DeleteSavedStateQueue();
		
	return SaveStateSelf();
}


// ---------------------------------------------------------------------------------
//		¥ RestoreState	[public]
// ---------------------------------------------------------------------------------
// restores the previously saved state.

OSErr
CPreferences::RestoreState()
{
	// calling this if we don't have a saved state is probably a programmer error.
	if (!HasSavedState())
	{
		SignalStringLiteral_( "No state to restore" );
		return noErr;
	}
	
	// restore saved state, then delete it.
	OSErr err = RestoreStateSelf();
	DeleteSavedStateQueue();
	return err;
}
		

// ---------------------------------------------------------------------------------
//		¥ HasSavedState	[public]
// ---------------------------------------------------------------------------------
// returns true if a saved state currently exists.
// note that restoring a saved state doesn't delete it.

Boolean
CPreferences::HasSavedState() const
{
	return (mSavedState.GetCount() > 0);
}
		

// ---------------------------------------------------------------------------------
//		¥ FileExists	[public]
// ---------------------------------------------------------------------------------
// returns true if the prefs file exists on disk.

Boolean
CPreferences::FileExists() const
{
	if (mPrefsFile == NULL)
		// prefs file object doesn't exist, so we can't know.
		return false;
	else
	{
		// check if file exists on disk.
		FSSpec theSpec;
		mPrefsFile->GetSpecifier( theSpec );
		OSErr err = ::FSMakeFSSpec( theSpec.vRefNum, theSpec.parID, theSpec.name, &theSpec );
		
		return (err == noErr);	// FSMakeFSSpec returns fnfErr if file doesn't exist.
	}
}


// ---------------------------------------------------------------------------------
//		¥ ReadSelf	[protected]
// ---------------------------------------------------------------------------------
// reads the prefs from the prefs file.

OSErr
CPreferences::ReadSelf()
{
	OSErr err = noErr;
	
	// if file object doesn't exist, big problem.
	if (mPrefsFile == NULL)
		return fnfErr;
	
	// save old resource file.
	StOldResFile oldResFile;

	try
	{
		// open the file. it must exist on disk prior to calling this.
		mPrefsFile->OpenResourceFork( fsRdPerm );
		
		// read all fields.
		SInt32 i;
		for (i = 1; i <= mFields.GetCount(); i++)
			ThrowIfOSErr_( ReadField( i ) );
			
		// close file.
		mPrefsFile->CloseResourceFork();
	}
	
	catch (ExceptionCode catchedErr)
	{
		// got an error. try closing the file, then return the error.
		try
		{
			mPrefsFile->CloseResourceFork();
		}
		
		catch (...) { }	// don't care if it worked or not.
		
		err = (OSErr) catchedErr;
	}
	
	return err;
}


// ---------------------------------------------------------------------------------
//		¥ WriteSelf	[protected]
// ---------------------------------------------------------------------------------
// writes all the fields in the prefs file.

OSErr
CPreferences::WriteSelf()
{
	OSErr err = noErr;
	
	// if file object doesn't exist, big problem.
	if (mPrefsFile == NULL)
		return fnfErr;
	
	// save old resource file.
	StOldResFile oldResFile;

	try
	{
		// open the file. create it as necessary.
		mPrefsFile->OpenOrCreateResourceFork( fsRdWrPerm, mCreator, mFileType, smSystemScript );
		
		// write all fields.
		SInt32 i;
		for (i = 1; i <= mFields.GetCount(); i++)
			ThrowIfOSErr_( WriteField( i ) );
			
		// close file.
		mPrefsFile->CloseResourceFork();
	}
	
	catch (ExceptionCode catchedErr)
	{
		// got an error. try closing the file, then return the error.
		try
		{
			mPrefsFile->CloseResourceFork();
		}
		
		catch (...) { }	// don't care if it worked or not.
		
		err = (OSErr) catchedErr;
	}
	
	return err;
}


// ---------------------------------------------------------------------------------
//		¥ ResetSelf	[protected]
// ---------------------------------------------------------------------------------
// resets all field to their default value.

OSErr
CPreferences::ResetSelf()
{
	try
	{
		// reset all fields.
		SInt32 i;
		for (i = 1; i <= mFields.GetCount(); i++)
			ThrowIfOSErr_( ResetField( i ) );
	}
	
	catch (ExceptionCode catchedErr)
	{
		// got an error. return it.
		return (OSErr) catchedErr;
	}
	
	return noErr;
}


// ---------------------------------------------------------------------------------
//		¥ ReadField	[protected]
// ---------------------------------------------------------------------------------
// reads a field from the prefs file.
// this function assumes that the currently open resource fork is the prefs file.

OSErr
CPreferences::ReadField(
	SInt16	inFieldIndex )
{
	OSErr err = noErr;

	try
	{
		// temporarily disable debug throw notifications. since we'll reset the field in the catch
		// block anyway if it fails, no need to alert the programmer.
		StDisableDebugThrow_();
	
		// get field info.
		SPreferencesField* theFieldP = mFields[inFieldIndex];
		ThrowIfNULL_( theFieldP );
		
		// get that resource from the prefs file.
		StResource theResource( (theFieldP->mFieldSize > 0
								 ? resType_PtrPrefsField
								 : resType_HandlePrefsField),
								theFieldP->mFieldID,
								true,
								true );
		
		// get its size.
		SInt32 theResourceSize = ::GetResourceSizeOnDisk( theResource );
		ThrowIfResError_();
		
		// it must be the same size as the field in RAM
		// (unless the field is a Handle field, which can be of variable size).
		ThrowIfNot_( (theResourceSize == theFieldP->mFieldSize) || (theFieldP->mFieldSize == 0) );
		
		// copy it in our field.
		if (theFieldP->mFieldSize > 0)
		{
			// a pointer field.
			::BlockMoveData( *theResource, theFieldP->mFieldP, theResourceSize );
			ThrowIfMemError_();
		}
		else
		{
			// a handle field.
			StHandleLocker theResourceLocker( theResource );
			ThrowIfOSErr_( ::PtrToXHand( *theResource,
										 static_cast<Handle>(theFieldP->mFieldP),
										 theResourceSize ) );
		}
	}
	
	catch (...)
	{
		// got an error. try resetting this field to default values.
		err = ResetField( inFieldIndex );
	}
	
	return err;
}


// ---------------------------------------------------------------------------------
//		¥ WriteField	[protected]
// ---------------------------------------------------------------------------------
// writes a field to the prefs file.
// this function assumes that the currently open resource fork is the prefs file.

OSErr
CPreferences::WriteField(
	SInt16	inFieldIndex )
{
	OSErr err = noErr;

	try
	{
		// get field info.
		SPreferencesField* theFieldP = mFields[inFieldIndex];
		ThrowIfNULL_( theFieldP );
		
		// get existing resource from the prefs file.
		Handle theOldResource = ::Get1Resource( (theFieldP->mFieldSize > 0
												 ? resType_PtrPrefsField
												 : resType_HandlePrefsField),
												theFieldP->mFieldID );
		
		// if field exists, we must remove it.
		if (theOldResource != nil)
		{
			::RemoveResource( theOldResource );
			theOldResource = nil;	// no longer valid after this point.
			ThrowIfResError_();
			
			::UpdateResFile( ::CurResFile() );
			ThrowIfResError_();
		}
		
		// create a handle to write the resource.
		StHandleBlock theHandle;
		
		// write the field to it.
		if (theFieldP->mFieldSize > 0)
		{
			// a pointer field.
			Handle theTempHandle = nil;
			ThrowIfOSErr_( ::PtrToHand( theFieldP->mFieldP,
							 			&theTempHandle,
							 			theFieldP->mFieldSize ) );
			theHandle.Adopt( theTempHandle );
		}
		else
		{
			// a handle field.
			Handle theTempHandle = static_cast<Handle>(theFieldP->mFieldP);
			ThrowIfOSErr_( ::HandToHand( &theTempHandle ) );
			theHandle.Adopt( theTempHandle );
		}
		
		// add this handle as a resource.
		::AddResource( theHandle,
					   (theFieldP->mFieldSize > 0
					    ? resType_PtrPrefsField
					    : resType_HandlePrefsField),
					   theFieldP->mFieldID,
					   "\p" );
		ThrowIfResError_();
		// need a new wrapper to dispose of handle now that it's a resource.
		StResource theResource( theHandle.Release() );
		
		// write resource to disk.
		::WriteResource( theResource );
		ThrowIfResError_();
	}
	
	catch (ExceptionCode catchedErr)
	{
		// got an error. we must return it.
		err = (OSErr) catchedErr;
	}
	
	return err;
}


// ---------------------------------------------------------------------------------
//		¥ ResetField	[protected]
// ---------------------------------------------------------------------------------
// resets a field to its default value.

OSErr
CPreferences::ResetField(
	SInt16	inFieldIndex )
{
	OSErr err = noErr;

	try
	{
		// get field info.
		SPreferencesField* theFieldP = mFields[inFieldIndex];
		ThrowIfNULL_( theFieldP );
		
		if (theFieldP->mFieldSize > 0)
		{
			// a pointer field.
			::BlockMoveData( theFieldP->mFieldDefault,
							 theFieldP->mFieldP,
							 theFieldP->mFieldSize );
			ThrowIfMemError_();
		}
		else
		{
			// a handle field.
			Handle theDefaultH = static_cast<Handle>(theFieldP->mFieldDefault);
			ThrowIfNil_( theDefaultH );
			StHandleLocker theDefaultLocker( theDefaultH );
			ThrowIfOSErr_( ::PtrToXHand( *theDefaultH,
										 static_cast<Handle>(theFieldP->mFieldP),
										 ::GetHandleSize( theDefaultH ) ) );
		}
	}
	
	catch (ExceptionCode catchedErr)
	{
		// got an error, return it.
		err = (OSErr) catchedErr;
	}
	
	return err; 
}


// ---------------------------------------------------------------------------------
//		¥ SaveStateSelf	[protected]
// ---------------------------------------------------------------------------------
// saves the state of all fields.
// this function assumes that there is currently no saved state.

OSErr
CPreferences::SaveStateSelf()
{
	try
	{
		// save states of all fields, in order.
		SInt32 i;
		for (i = 1; i <= mFields.GetCount(); i++)
			ThrowIfOSErr_( SaveFieldState( i ) );
	}
	
	catch (ExceptionCode catchedErr)
	{
		// got an error. return it.
		return (OSErr) catchedErr;
	}
	
	return noErr;
}


// ---------------------------------------------------------------------------------
//		¥ RestoreStateSelf	[protected]
// ---------------------------------------------------------------------------------
// restores the state of all fields.
// this function assumes that there is currently a saved state.
// it does not delete the saved state after restoring it.

OSErr
CPreferences::RestoreStateSelf()
{
	try
	{
		// restore states of all fields, in order.
		SInt32 i;
		for (i = 1; i <= mFields.GetCount(); i++)
			ThrowIfOSErr_( RestoreFieldState( i ) );
	}
	
	catch (ExceptionCode catchedErr)
	{
		// got an error. return it.
		return (OSErr) catchedErr;
	}
	
	return noErr;
}


// ---------------------------------------------------------------------------------
//		¥ SaveFieldState	[protected]
// ---------------------------------------------------------------------------------
// saves the state of the given field.
// note: this functions assumes that the saved state queue has (inFieldIndex - 1) elements.
// please save states starting from the first to the last field so this works fine.

OSErr
CPreferences::SaveFieldState(
	SInt16	inFieldIndex )
{
	SignalIfNot_( mSavedState.GetCount() == (inFieldIndex - 1) );

	OSErr err = noErr;

	try
	{
		// get info about field to save.
		SPreferencesField* theFieldP = mFields[inFieldIndex];
		ThrowIfNULL_( theFieldP );
	
		// create a new saved state queue element.
		StDeleter<SPreferencesSavedField> theSavedField( new SPreferencesSavedField );
		ThrowIfNil_( theSavedField );
		theSavedField->mFieldState = nil;	// so that the catch block doesn't try deleting it.
		
		if (theFieldP->mFieldSize > 0)
		{
			// a pointer field.
			// allocate memory for the saved field state.
			StPointerBlock theFieldState( theFieldP->mFieldSize );
			ThrowIfNil_( theFieldState );
			
			// copy the current field's state into the newly allocated memory.
			::BlockMoveData( theFieldP->mFieldP,
							 theFieldState,
							 theFieldP->mFieldSize );
			ThrowIfMemError_();
			
			// save field state into the queue element.
			theSavedField->mFieldState = theFieldState.Release();
			
			// save it size too.
			theSavedField->mFieldSize = theFieldP->mFieldSize;
		}
		else
		{
			// a Handle field.
			// allocate memory for the saved field state.
			Handle theFieldState = static_cast<Handle>(theFieldP->mFieldP);
			ThrowIfOSErr_( ::HandToHand( &theFieldState ) );
			
			// save field state into the queue element.
			theSavedField->mFieldState = theFieldState;
			
			// set saved field state size to 0 to remember it's a Handle.
			theSavedField->mFieldSize = 0;
		}
		
		// add element to the queue.
		mSavedState.AddItem( theSavedField.Release() );
	}
	
	catch (ExceptionCode catchedErr)
	{
		// got an error. gotta return it.
		err = (OSErr) catchedErr;
	}
	
	return err;
}


// ---------------------------------------------------------------------------------
//		¥ RestoreFieldState	[protected]
// ---------------------------------------------------------------------------------
// restores a field's state.
// does not remove the saved state from the saved state queue.

OSErr
CPreferences::RestoreFieldState(
	SInt16	inFieldIndex )
{
	try
	{
		// get saved field.
		SPreferencesSavedField* theSavedFieldP = mSavedState[inFieldIndex];
		ThrowIfNULL_( theSavedFieldP );
		
		// get info about field to restore.
		SPreferencesField* theFieldP = mFields[inFieldIndex];
		ThrowIfNULL_( theFieldP );
		
		// it must be the same size as the field in the field queue bearing
		// the same index. otherwise, you saved fields in the wrong order.
		ThrowIfNot_( theSavedFieldP->mFieldSize == theFieldP->mFieldSize );
		
		// copy the saved field's state into the field.
		if (theSavedFieldP->mFieldSize > 0)
		{
			// a pointer field.
			::BlockMoveData( theSavedFieldP->mFieldState,
							 theFieldP->mFieldP,
							 theSavedFieldP->mFieldSize );
			ThrowIfMemError_();
		}
		else
		{
			// a Handle field.
			Handle theSavedStateH = static_cast<Handle>(theSavedFieldP->mFieldState);
			ThrowIfNil_( theSavedStateH );
			StHandleLocker theSavedStateLocker( theSavedStateH );
			ThrowIfOSErr_( ::PtrToXHand( *theSavedStateH,
										 static_cast<Handle>(theFieldP->mFieldP),
										 ::GetHandleSize( theSavedStateH ) ) );
		}
	}
	
	catch (ExceptionCode catchedErr)
	{
		// got an error. gotta return it.
		return (OSErr) catchedErr;
	}
	
	return noErr;
}


// ---------------------------------------------------------------------------------
//		¥ DeleteFieldQueue	[protected]
// ---------------------------------------------------------------------------------
// deletes the field queue.

void
CPreferences::DeleteFieldQueue()
{
	// iterate fields, delete them.
	TArrayIterator<SPreferencesField*> iterator( mFields );
	SPreferencesField* theField;
	while (iterator.Next( theField ))
	{
		// dispose of field's default value.
		if (theField->mFieldSize > 0)
			::DisposePtr( static_cast<Ptr>(theField->mFieldDefault) );
		else
			::DisposeHandle( static_cast<Handle>(theField->mFieldDefault) );
		
		// delete field itself.
		delete theField;
	}
	
	// clear array.
	mFields.RemoveItemsAt( 0x7FFFFFFF, LArray::index_First );
}


// ---------------------------------------------------------------------------------
//		¥ DeleteSavedStateQueue	[protected]
// ---------------------------------------------------------------------------------
// deletes the saved state field queue.

void
CPreferences::DeleteSavedStateQueue()
{
	// iterate saved fields, delete them.
	TArrayIterator<SPreferencesSavedField*> iterator( mSavedState );
	SPreferencesSavedField* theSavedField;
	while (iterator.Next( theSavedField ))
	{
		// dispose of saved field state.
		if (theSavedField->mFieldSize > 0)
			::DisposePtr( static_cast<Ptr>(theSavedField->mFieldState) );
		else
			::DisposeHandle( static_cast<Handle>(theSavedField->mFieldState) );
		
		// delete saved field itself.
		delete theSavedField;
	}
	
	// clear array.
	mSavedState.RemoveItemsAt( 0x7FFFFFFF, LArray::index_First );
}
