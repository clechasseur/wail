// =================================================================================
//	UWailActions.h					©1999, Charles Lechasseur
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

#include <LAction.h>

#include "CWailDocWindow.h"


#pragma mark -- CAddSoundAction --

// Action for Adding a sound to a sound list. this can be done via Paste or Import.

class CAddSoundAction: public LAction
{
	public:
	// Public Functions

		//Constructor
							CAddSoundAction(
								LStream*			inSound,
								SInt32				inClassNumber,
								Boolean				inIs8bit,
								CWailDocWindow*		inWindow );
		//Destructor
		virtual				~CAddSoundAction();
		
	protected:
	
	// LAction-overridden protected functions
	
		virtual void		RedoSelf();
		virtual void		UndoSelf();
		
	private:
	// Member Variables and Classes

		LStream*			mSound;				// the sound we're adding.
		SInt32				mClassNumber;		// what class does this sound belong to?
		Boolean				mIs8bit;			// is this sound 8-bit?
		
		CWailDocWindow*		mWindow;			// parent window of that action.
	
	// Private Functions
		// Defensive programming. No  operator= or copy constructor
							CAddSoundAction(const CAddSoundAction& inCAddSoundAction);
		CAddSoundAction&			operator=(const CAddSoundAction&);
};


#pragma mark -- CRemoveSoundAction --

// Action for Removing a sound from a sound list. this can be done via Cut, Clear or Delete.

class CRemoveSoundAction: public LAction
{
	public:
	// Public Functions

		//Constructor
							CRemoveSoundAction(
								SInt32				inClassNumber,
								SInt32				inSoundNumber,
								Boolean				inIs8bit,
								CWailDocWindow*		inWindow );
		//Destructor
		virtual				~CRemoveSoundAction();
		
	protected:
	
	// LAction-overridden protected functions
	
		virtual void		RedoSelf();
		virtual void		UndoSelf();
		
	private:
	// Member Variables and Classes

		LStream*			mSound;				// the sound we're removing. (we must keep it
												// in RAM if we're to be able to Undo)
		SInt32				mClassNumber;		// what class does this sound belong to?
		SInt32				mSoundNumber;		// what is the number of that sound?
		Boolean				mIs8bit;			// is this sound 8-bit?
		
		CWailDocWindow*		mWindow;			// parent window of that action.
	
	// Private Functions
		// Defensive programming. No  operator= or copy constructor
							CRemoveSoundAction(const CRemoveSoundAction& inCRemoveSoundAction);
		CRemoveSoundAction&			operator=(const CRemoveSoundAction&);
};


#pragma mark -- CChangeFlagAction --

// Action for changing a class flag.

class CChangeFlagAction: public LAction
{
	public:
	// Public Functions

		//Constructor
							CChangeFlagAction(
								LControl*			inFlagCheckBox,
								Boolean				inWasChecked,
								SInt32				inClassNumber,
								CWailDocWindow*		inWindow );
		
	protected:
	
	// LAction-overridden protected functions
	
		virtual void		RedoSelf();
		virtual void		UndoSelf();
		
	private:
	// Member Variables and Classes

		LControl*			mFlagCheckBox;		// flag checkbox we're changing.
		Boolean				mWasChecked;		// was the flag checked when we were created?
		SInt32				mClassNumber;		// class who owns that flag.
		
		CWailDocWindow*		mWindow;			// parent window of that action.
	
	// Private Functions
		// Defensive programming. No  operator= or copy constructor
							CChangeFlagAction(const CChangeFlagAction& inCChangeFlagAction);
		CChangeFlagAction&			operator=(const CChangeFlagAction&);
};


#pragma mark -- CChangeVolumeAction --

// Action for changing a class' volume setting.

class CChangeVolumeAction: public LAction
{
	public:
	// Public Functions

		//Constructor
							CChangeVolumeAction(
								LControl*			inOldVolumeControl,
								LControl*			inNewVolumeControl,
								SInt32				inClassNumber,
								CWailDocWindow*		inWindow );
		
	protected:
	
	// LAction-overridden protected functions
	
		virtual void		RedoSelf();
		virtual void		UndoSelf();
		
	private:
	// Member Variables and Classes

		LControl*			mOldVolumeControl;	// control of the old volume setting.
		LControl*			mNewVolumeControl;	// control of the new volume setting.
		
		SInt32				mClassNumber;		// class who owns that volume setting.
		
		CWailDocWindow*		mWindow;			// parent window of that action.
	
	// Private Functions
		// Defensive programming. No  operator= or copy constructor
							CChangeVolumeAction(const CChangeVolumeAction& inCChangeVolumeAction);
		CChangeVolumeAction&		operator=(const CChangeVolumeAction&);
};


#pragma mark -- CDontRemap8bitAction --

// Action for removing the Remap 8-bit check from the window.

class CDontRemap8bitAction: public LAction
{
	public:
	// Public Functions

		//Constructor
							CDontRemap8bitAction(
								LControl*			inRemap8bitCheckBox,
								SInt32				inClassNumber,
								CWailDocWindow*		inWindow );
		
	protected:
	
	// LAction-overridden protected functions
	
		virtual void		RedoSelf();
		virtual void		UndoSelf();
		
	private:
	// Member Variables and Classes
	
		LControl*			mRemap8bitCheckBox;	// the check box of the Remap 8-bit option.
		
		SInt32				mClassNumber;		// class who owns that remap 8-bit action.

		CWailDocWindow*		mWindow;			// parent window of that action.
	
	// Private Functions
		// Defensive programming. No  operator= or copy constructor
							CDontRemap8bitAction(const CDontRemap8bitAction&);
		CDontRemap8bitAction&		operator=(const CDontRemap8bitAction&);
};


#pragma mark -- CRemap8bitAction --

// Action for checking the Remap 8-bit check in the window.

class CRemap8bitAction: public LAction
{
	public:
	// Public Functions

		//Constructor
							CRemap8bitAction(
								LControl*			inRemap8bitCheckBox,
								SInt32				inClassNumber,
								CWailDocWindow*		inWindow );
		//Destructor
		virtual				~CRemap8bitAction();
		
	protected:
	
	// LAction-overridden protected functions
	
		virtual void		RedoSelf();
		virtual void		UndoSelf();
		
	private:
	// Member Variables and Classes
	
		LControl*			mRemap8bitCheckBox;	// the check box of the Remap 8-bit option.
		
		SInt32				mClassNumber;		// class who owns that remap 8-bit action.

		LStream*			m16bitSounds[5];	// saved 16-bit sounds.
		SInt16				mNum16bitSounds;	// number of saved sounds.

		CWailDocWindow*		mWindow;			// parent window of that action.
	
	// Private Functions
		// Defensive programming. No  operator= or copy constructor
							CRemap8bitAction(const CRemap8bitAction&);
		CRemap8bitAction&		operator=(const CRemap8bitAction&);
};


#pragma mark -- CChangeChanceAction --

// Action for changing a class' Chance value.

class CChangeChanceAction: public LAction
{
	public:
	// Public Functions

		//Constructor
							CChangeChanceAction(
								SInt16				inOldValue,
								SInt16				inNewValue,
								SInt32				inClassNumber,
								CWailDocWindow*		inWindow );
		
	protected:
	
	// LAction-overridden protected functions
	
		virtual void		RedoSelf();
		virtual void		UndoSelf();
		
	private:
	// Member Variables and Classes

		SInt16				mOldValue;			// old Chance value.
		SInt16				mNewValue;			// new Chance value.
		SInt32				mClassNumber;		// class whose Chance value changed.
		
		CWailDocWindow*		mWindow;			// parent window of that action.
	
	// Private Functions
		// Defensive programming. No  operator= or copy constructor
								CChangeChanceAction(const CChangeChanceAction& );
		CChangeChanceAction&	operator=(const CChangeChanceAction&);
};


#pragma mark -- CAddClassesAction --

// Action for Adding (a) sound class(es) to a sound file.

class CAddClassesAction: public LAction
{
	public:
	// Public Functions

		//Constructor
							CAddClassesAction(
								SInt32				inHowMany,
								CWailDocWindow*		inWindow );
		
	protected:
	
	// LAction-overridden protected functions
	
		virtual void		RedoSelf();
		virtual void		UndoSelf();
		
	private:
	// Member Variables and Classes

		SInt32				mHowMany;			// how many classes are we supposed to add?
		
		CWailDocWindow*		mWindow;			// parent window of that action.
	
	// Private Functions
		// Defensive programming. No  operator= or copy constructor
							CAddClassesAction( const CAddClassesAction& );
		CAddClassesAction&			operator=( const CAddClassesAction& );
};


#pragma mark -- CRemoveClassesAction --

// Action for Removing (a) sound class(es) from a sound file.

class CRemoveClassesAction: public LAction
{
	public:
	// Public Functions

		//Constructor
							CRemoveClassesAction(
								SInt32				inFirstIndex,
								SInt32				inHowMany,
								CWailDocWindow*		inWindow );
		//Destructor
		virtual				~CRemoveClassesAction();
		
	protected:
	
	// LAction-overridden protected functions
	
		virtual void		RedoSelf();
		virtual void		UndoSelf();
		
	private:
	// Member Variables and Classes

		TArray<CWailSoundClass*>	mClasses;	// the classes we have removed. we keep them
												// alive in case we have to Undo.
												
		SInt32						mFirstIndex;// index of the first class to remove (0-based).
		SInt32						mHowMany;	// how many classes are we supposed to remove?
		
		CWailDocWindow*				mWindow;	// parent window of that action.
	
	// Private Functions
		// Defensive programming. No  operator= or copy constructor
							CRemoveClassesAction( const CRemoveClassesAction& );
		CRemoveClassesAction&			operator=( const CRemoveClassesAction& );
};
