// =================================================================================
//	CWailDocWindow.h					©1999, Charles Lechasseur
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

#include "CTrackableWindow.h"
#include <LListener.h>

#include "CWailSoundFileData.h"


// constants related to the window - panes & messages.

#pragma mark -- CWailDocWindow constants --

// Resource IDs
const	ResIDT		rPPob_SoundWindow				= 1000;

// pane IDs and message types of panes
const	PaneIDT		pane_ClassList					= 'Clas';
const	MessageT	msg_ClassListSingleClick		= 'Cla1';
const	MessageT	msg_ClassListDoubleClick		= 'Cla2';

const	PaneIDT		pane_8bitSoundsList				= '8bit';
const	MessageT	msg_8bitSoundsListDoubleClick	= '8bt2';
const	PaneIDT		pane_16bitSoundsList			= '16bt';
const	MessageT	msg_16bitSoundsListDoubleClick	= '16b2';

const	MessageT	msg_SoundsListDeleteKey			= 'Sl!k';
const	MessageT	msg_SoundsListEnterKey			= 'SlÂk';

const	PaneIDT		pane_Flag1CheckBox				= 'Flg1';
const	MessageT	msg_Flag1CheckBox				= 'Flg1';
const	PaneIDT		pane_Flag2CheckBox				= 'Flg2';
const	MessageT	msg_Flag2CheckBox				= 'Flg2';
const	PaneIDT		pane_Flag3CheckBox				= 'Flg3';
const	MessageT	msg_Flag3CheckBox				= 'Flg3';
const	PaneIDT		pane_Flag4CheckBox				= 'Flg4';
const	MessageT	msg_Flag4CheckBox				= 'Flg4';
const	PaneIDT		pane_Flag5CheckBox				= 'Flg5';
const	MessageT	msg_Flag5CheckBox				= 'Flg5';
const	PaneIDT		pane_Flag6CheckBox				= 'Flg6';
const	MessageT	msg_Flag6CheckBox				= 'Flg6';
const	PaneIDT		pane_Flag7CheckBox				= 'Flg7';
const	MessageT	msg_Flag7CheckBox				= 'Flg7';

const	PaneIDT		pane_ClassIDEditField			= 'ClId';
const	MessageT	msg_ClassIDEditFieldChanged		= 'ClId';
const	MessageT	msg_ClassIDEditFieldEnterPressed= 'ClIÂ';
const	MessageT	msg_ClassIDEditFieldLostTarget	= 'ClIv';
const	PaneIDT		pane_ClassNumberCaption			= 'ClNm';

const	PaneIDT		pane_ChancePopupMenu			= 'Chnc';
const	MessageT	msg_ChancePopupMenuChanged		= 'Chnc';
const	PaneIDT		pane_LowPitchEditField			= 'LowP';
const	MessageT	msg_LowPitchEditFieldChanged	= 'LowP';
const	PaneIDT		pane_HighPitchEditField			= 'HiP ';
const	MessageT	msg_HighPitchEditFieldChanged	= 'HiP ';

const	PaneIDT		pane_SoftVolumeRadioButton		= 'Soft';
const	MessageT	msg_SoftVolumeRadioButton		= 'Soft';
const	PaneIDT		pane_MediumVolumeRadioButton	= 'Medi';
const	MessageT	msg_MediumVolumeRadioButton		= 'Medi';
const	PaneIDT		pane_LoudVolumeRadioButton		= 'Loud';
const	MessageT	msg_LoudVolumeRadioButton		= 'Loud';
const	PaneIDT		pane_Volume3RadioButton			= 'Vol3';
const	MessageT	msg_Volume3RadioButton			= 'Vol3';

const	PaneIDT		pane_Remap8bitCheckBox			= 'Rmap';
const	MessageT	msg_Remap8bitCheckBox			= 'Rmap';

// drag messages for sound lists

const	MessageT	msg_Send8bitDrag				= 'SDg8';
const	MessageT	msg_Receive8bitDrag				= 'RDg8';

const	MessageT	msg_Send16bitDrag				= 'SDgA';
const	MessageT	msg_Receive16bitDrag			= 'RDgA';

// popup menu values for the chance popup menu

const	SInt32		popupValue_10Percent			= 1;
const	SInt32		popupValue_20Percent			= 2;
const	SInt32		popupValue_30Percent			= 3;
const	SInt32		popupValue_40Percent			= 4;
const	SInt32		popupValue_50Percent			= 5;
const	SInt32		popupValue_60Percent			= 6;
const	SInt32		popupValue_70Percent			= 7;
const	SInt32		popupValue_80Percent			= 8;
const	SInt32		popupValue_90Percent			= 9;
const	SInt32		popupValue_Always				= 10;


// related constants.

#pragma mark -- Related constants --

// modal dialogs constants
const	ResIDT		rPPob_AddClassesDialog			= 1002;
const	PaneIDT		pane_HowManyClassesEditField	= 'NumC';

const	ResIDT		rPPob_FindClassByIDDialog		= 1003;
const	PaneIDT		pane_WhatClassIDEditField		= 'ClId';

const	ResIDT		rPPob_FindClassByNumberDialog	= 1008;
const	PaneIDT		pane_WhatClassNumberEditField	= 'Cl#N';

const	ResIDT		rPPob_WarnDelete16bitSoundsDialog = 1005;

const	ResIDT		rPPob_WarnDeleteSoundDialog		= 1006;

const	ResIDT		rPPob_WarnDeleteClassDialog		= 1009;

// alert IDs
const	ResIDT		rALRT_TooManyClassesAlert		= 10001;

// STR# IDs and indexes
const	ResIDT		STRx_ShuttleStrings				= 201;
const	SInt16		str_ShuttleDefaultName			= 1;

const	ResIDT		STRx_SoundStrings				= 202;
const	SInt16		str_SoundFileDefaultName		= 1;
const	SInt16		str_AIFFFileNameFooter			= 2;


// ---------------------------------------------------------------------------------
//	CWailDocWindow class declaration
// ---------------------------------------------------------------------------------

#pragma mark -- CWailDocWindow class declaration --

class CWailDocWindow: public CTrackableWindow,
					  public LListener
{
	public:
	// Public Enums and Constants
		enum { class_ID = 'winW' };
		
		enum EControlStateEffect { state_NoEffect, state_Disable, state_Enable };
	
	// Public Functions

		//Default Constructor
							CWailDocWindow();
		
		//The two next constructors are part of LWindow. I don't know what they do :)
							CWailDocWindow(
								ResIDT				inWINDid,
								UInt32				inAttributes,
								LCommander			*inSuperCommander);
								
							CWailDocWindow(
								LCommander			*inSuperCommander,
								const Rect			&inGlobalBounds,
								ConstStringPtr		inTitle,
								SInt16				inProcID,
								UInt32				inAttributes,
								WindowPtr			inBehind );
		
		// Stream Constructor 
							CWailDocWindow(
								LStream 			*inStream );
		// Copy Constructor
							CWailDocWindow(
								const CWailDocWindow& inCWailDocWindow );
		//Destructor
		virtual				~CWailDocWindow();
		
		//Messaging
		virtual void		ListenToMessage(
								MessageT			inMessage,
								void				*ioParam);
								
		//Command handling
		virtual Boolean		ObeyCommand(
								CommandT			inCommand,
								void				*ioParam);
		virtual void		FindCommandStatus(
								CommandT			inCommand,
								Boolean				&outEnabled,
								Boolean				&outUsesMark,
								UInt16				&outMark,
								Str255				outName);
								
		//Selecting classes
		void				SelectClass(
								SInt32				inWhichClass,
								Boolean				inDoRefreshClassList = false );
								
		// Pane setting
		void				FillAllClassPanes();
		void				ResetAllClassPanes();
		
		void				FillClassList();
		void				Fill8bitSoundsList();
		void				Fill16bitSoundsList();
		
		void				ClearClassList();
		void				Clear8bitSoundsList();
		void				Clear16bitSoundsList();
								
		//Data accessors
		void				SetSoundFileData(
								CWailSoundFileData	*inData );
		CWailSoundFileData*	GetSoundFileData();
		void				DeleteSoundFileData();
		
		SInt32				GetCurrentClass() const
									{ return mCurrentClass; }
		
		void				SetDirty(
								Boolean				inDirty );
		Boolean				IsDirty() const { return mDirty; }
		
		void				SetIsPerformingAction(
								Boolean				inIsPerformingAction )
									{ mIsPerformingAction = inIsPerformingAction; }
		Boolean				IsPerformingAction() const { return mIsPerformingAction; }
		
		void				SetIsRemapping(
								Boolean				inIsRemapping )
									{ mIsRemapping = inIsRemapping; }
		Boolean				IsRemapping() const { return mIsRemapping; }
		
		void				SetIsChangingClass(
								Boolean				inIsChangingClass )
									{ mIsChangingClass = inIsChangingClass; }
		Boolean				IsChangingClass() const { return mIsChangingClass; }
		
		SInt16				GetChance() const;
		void				SetChance(
								SInt16				inChance,
								EControlStateEffect	inStateEffect,
								bool				inBroadcastChange );
								
	protected:
	//Protected member functions
	
		//Init
		void			InitWailDocWindow();
		virtual void	FinishCreateSelf();
							
		// Message handling
		void			HandleFlagChange(
							MessageT		 inMessage,
							CWailSoundClass* inClass );
		void			HandleVolumeChange(
							MessageT		 inMessage,
							CWailSoundClass* inClass );
		void			HandleRemapping(
							CWailSoundClass* inClass );
							
		void			HandleDragSend(
							const SMouseDownEvent&	inMouseDown,
							Boolean					inIs8bit );
		void			HandleDragReceive(
							Handle&					inSoundHandle,
							Boolean					inIs8bit );
							
		// File Menu Command handlers
		
		Boolean			CompareWithAnotherFile();
		Boolean			CompareWithAnotherWindow();
		
		// Class Menu Command handlers
		Boolean			AddClass();
		Boolean			AddClasses();
		Boolean			RemoveClass();
		Boolean			FindClassByID();
		Boolean			FindClassByName();
		Boolean			FindClassByNumber();
		
		// Sound Command handlers
		Boolean			CutCopyClearSound( CommandT inCommand, Boolean inIs8bit );
		Boolean			PasteSound( Boolean inIs8bit );
		Boolean			ImportSys7Sound( Boolean inIs8bit );
		Boolean			ExportSys7Sound( Boolean inIs8bit );
		Boolean			ImportAiffSound( Boolean inIs8bit );
		Boolean			ExportAiffSound( Boolean inIs8bit );
		Boolean			PlaySound( Boolean inLoop, Boolean inIs8bit );
		
		// Undoable actions
		void			ActionAddSound(
							LStream*		inSound,
							Boolean			inIs8bit );
		void			ActionRemoveSound(
							SInt32			inWhichSound,
							Boolean			inIs8bit );
							
		void			ActionAddClasses(
							SInt32			inHowMany );
		void			ActionRemoveClass();
							
		void			ActionChangeFlag(
							MessageT		inFlagMessage );
		void			ActionChangeVolume(
							SInt16			inOldVolume,
							SInt16			inNewVolume );
		void			ActionDontRemap8bit();
		void			ActionRemap8bit();
		void			ActionChangeChance(
							SInt16			inOldValue,
							SInt16			inNewValue );
		
	private:
	// Private variables.
	
		Boolean					mDirty;				// are we dirty? (ahh, dirt.)
		Boolean					mIsChangingClass;	// if we're changing class, we must not
													// listen to messages from controls.
		Boolean					mIsPerformingAction;// if we're performing actions, we must
													// not create any more actions.
		Boolean					mIsRemapping;		// when we're remapping, we must not
													// listen to Remapping messages.
	
		CWailSoundFileData		*mSoundFileData;	// the data of the sound file.
													// (or "that which shall be edited")
													
		SInt32					mCurrentClass;		// the currently selected sound class.
													// we keep this to avoid always having to
													// access the list Pane object.
													// since lists start at 0, -1 means no selection.
	
	// Private Functions
		// Defensive programming. No  operator=
		CWailDocWindow&			operator=(const CWailDocWindow&);
};


#pragma mark -- StRemapping class declaration --

// A stack-based class to set a Wail doc window's "Is remapping" flag.

class StRemapping
{
public:

	// Constructor
	
							StRemapping(
								CWailDocWindow&		inWindow );
							StRemapping(
								CWailDocWindow&		inWindow,
								Boolean				inNewState );
	
	// Destructor
	
							~StRemapping();
							
private:

	// Member variables
	
	CWailDocWindow&			mWindow;
	Boolean					mSavedState;
};


#pragma mark -- StPerformingAction class declaration --

// A stack-based class to set a Wail doc window's "Is performing action" flag.

class StPerformingAction
{
public:

	// Constructor
	
							StPerformingAction(
								CWailDocWindow&		inWindow );
							StPerformingAction(
								CWailDocWindow&		inWindow,
								Boolean				inNewState );
	
	// Destructor
	
							~StPerformingAction();
							
private:

	// Member variables
	
	CWailDocWindow&			mWindow;
	Boolean					mSavedState;
};


#pragma mark -- StChangingClass class declaration --

// A stack-based class to set a Wail doc window's "Is changing class" flag.

class StChangingClass
{
public:

	// Constructor
	
							StChangingClass(
								CWailDocWindow&		inWindow );
							StChangingClass(
								CWailDocWindow&		inWindow,
								Boolean				inNewState );
	
	// Destructor
	
							~StChangingClass();
							
private:

	// Member variables
	
	CWailDocWindow&			mWindow;
	Boolean					mSavedState;
};
