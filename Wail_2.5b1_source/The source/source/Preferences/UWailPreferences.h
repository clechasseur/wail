// =================================================================================
//	UWailPreferences.h					©1999, Charles Lechasseur
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

#include <UModalDialogs.h>

#include "CPreferences.h"

#include "CBroadcastingEditField.h"


// enums

enum ECompareSetting
{
	compareSetting_Together = 0,
	compareSetting_Separately
};


// UWailPreferences class
#pragma mark == UWailPreferences ==

class UWailPreferences
{
	public:
	
		// Initialization
	
		static void			Initialize();
		
		// cleanup
		
		static void			Finalize();
		
		// reading/writing/resetting
		
		static void			Read();
		static void			Write();
		static void			Reset();
		
		// editing prefs with a dialog
		
		static void			Edit(
								LCommander*	inSuper );
		
		// accessors
		
		static Boolean		WarnBeforeDeleting() { return sWarnBeforeDeleting; }
		static void			SetWarnBeforeDeleting(
								Boolean inWarnBeforeDeleting )
									{ sWarnBeforeDeleting = inWarnBeforeDeleting; }
									
		static Boolean		WarnBeforeRemapping() { return sWarnBeforeRemapping; }
		static void			SetWarnBeforeRemapping(
								Boolean inWarnBeforeRemapping )
									{ sWarnBeforeRemapping = inWarnBeforeRemapping; }
									
		static SInt16		CompareSettings() { return sCompareSettings; }
		static void			SetCompareSettings(
								SInt16	inCompareSettings )
									{ sCompareSettings = inCompareSettings; }
									
		static Boolean		IgnoreCaseInNameFind() { return sIgnoreCaseInNameFind; }
		static void			SetIgnoreCaseInNameFind(
								Boolean	inIgnoreCaseInNameFind )
									{ sIgnoreCaseInNameFind = inIgnoreCaseInNameFind; }
									
		static Boolean		ExactMatchInNameFind() { return sExactMatchInNameFind; }
		static void			SetExactMatchInNameFind(
								Boolean	inExactMatchInNameFind )
									{ sExactMatchInNameFind = inExactMatchInNameFind; }
									
		static Boolean		WarnBeforeDeletingClass() { return sWarnBeforeDeletingClass; }
		static void			SetWarnBeforeDeletingClass(
								Boolean inWarnBeforeDeletingClass )
									{ sWarnBeforeDeletingClass = inWarnBeforeDeletingClass; }
		
		static void			GetClassNamesFile(
								FSSpec& outSpec );
		static void			SetClassNamesFile(
								const FSSpec& inSpec );
									
		static SInt32		RAMToProtect() { return sRAMToProtect; }
		static void			SetRAMToProtect(
								SInt32 inRAMToProtect )
									{ sRAMToProtect = inRAMToProtect; }
		
	protected:
	
		// our prefs object
		
		static CPreferences*	sPreferences;
	
		// Editor prefs fields
		
		static Boolean			sWarnBeforeDeleting;
		static Boolean			sWarnBeforeRemapping;
		static SInt16			sCompareSettings;
		static Boolean			sIgnoreCaseInNameFind;
		static Boolean			sExactMatchInNameFind;
		static Boolean			sWarnBeforeDeletingClass;
		
		// Class Names prefs fields
		
		static FSSpec			sDeprecatedClassNamesFile;
		static AliasHandle		sClassNamesFileAlias;
		
		// Virtual Memory prefs fields
		
		static SInt32			sRAMToProtect;
		
	private:
		// can't create any object of this class.
								UWailPreferences();
		virtual					~UWailPreferences();
};


// StWailPrefsDialogHandler - a subclass of StDialogHandler to handle the prefs dialog in Wail
#pragma mark -
#pragma mark == StWailPrefsDialogHandler ==

class StWailPrefsDialogHandler : public StDialogHandler
{
	public:
	
		// Constructor
							StWailPrefsDialogHandler(
								LCommander		*inSuper);
		// Destructor
		virtual				~StWailPrefsDialogHandler();
		
		// dialog handling
		
		virtual void		ListenToMessage(
								MessageT		inMessage,
								void			*ioParam);
		
	protected:
	
		// initializations
		
		void				LinkMultiPanelViewToPageController();
		void				GetPanePointers();
		void				ListenToPanes();
		void				InitPanesAccordingToPrefs();
		
		// message handling
		
		Boolean				HandleMessage(
								MessageT		inMessage );
								
		// choosing the class names file
		
		void				ChooseClassNamesFile();
		void				SetClassNamesFile(
								const FSSpec&	inFile,
								bool			inUpdatePrefs = true );
	
	private:
	
		// member variables
		
		LControl*			mWarnBeforeDeletingCheckBox;
		LControl*			mWarnBeforeRemappingCheckBox;
		LControl*			mWarnBeforeDeletingClassCheckBox;
		LControl*			mCompareTogetherRadio;
		LControl*			mCompareSeparatelyRadio;
		
		LPane*				mClassNamesFileNameCaption;
		LControl*			mClassNamesFileChooseButton;
		
		CBroadcastingEditField*		mRAMToProtectEditField;
};