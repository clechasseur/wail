// =================================================================================
//	CWailSoundFileData.h					©1999, Charles Lechasseur
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


// ---------------------------------------------------------------------------------
//	SMthonSoundHeader declaration
// ---------------------------------------------------------------------------------
// the SMthonSoundHeader is a struct that represents the header of a Marathon sound
// file. it will be used to determine the number of sound classes when reading, and
// must be known to fill it correctly when writing.

#pragma options align=mac68k	// Mthon structs are 68k-aligned.

struct SMthonSoundHeader
{
	SInt32		mVersion;		// file version. must be 1.
	OSType		mIdent;			// file identification. must be 'snd2'.
	
	SInt16		mNumSets;		// number of sound sets. must be 2 (8-bit and 16-bit).
	SInt16		mNumClasses;	// number of classes.
	
	SInt32		mUnknown;		// unknown. must be 0.
	UInt8		mReserved[244];	// reserved. must be 0.
	
	// we provide an inlined constructor that will initialize stuff that
	// we don't care about, so we only have to set what's interesting when
	// using the object.
				SMthonSoundHeader();
};
typedef struct SMthonSoundHeader SMthonSoundHeader;

#pragma options align=reset

inline
SMthonSoundHeader::SMthonSoundHeader()
{
	mVersion = 1;
	mIdent = 'snd2';
	
	mNumSets = 2;
	
	mUnknown = 0;
	int i;
	for (i = 0; i < 244; i++)
		mReserved[i] = 0;
}


// ---------------------------------------------------------------------------------
//	SMthonSoundClass declaration
// ---------------------------------------------------------------------------------
// the SMthonSoundClass is a struct that represents how the data is stored in a 
// real Marathon sound file. since I find this format much impratical for storing
// in memory, it won't be used by this program; but I must define it in order to
// read/write the info from/to the Marathon sound file itself.
// 
// this struct is derived from the SndDirEntryRecord record found in the Pascal
// source of my Marathon Sound Editor. that struct was itself derived from two sources:
// mostly from James Knight's info, and also from Bo Lindbergh's "m2sound.h" file
// which he kindly sent to me so I could figure out the sound flags.
//
// I'd like to thank them both for allowing me to write this program and the
// Marathon Sound Editor. </shameless plug>

const SInt16 classID_Unused				= -1;
const SInt16 classID_UnusedButInstalled	= -2;	// special flag used to tell shuttle
												// to install an empty class anyway.

const SInt16 volume_Soft				= 0;
const SInt16 volume_Medium				= 1;
const SInt16 volume_Loud				= 2;
const SInt16 volume_Volume3				= 3; // ZPC volume. don't know what it means.

const SInt16 flag_CannotBeRestarted		= 0x0001;
const SInt16 flag_DoesntSelfAbort		= 0x0002;
const SInt16 flag_ResistsPitchChange	= 0x0004;
const SInt16 flag_CannotChangePitch		= 0x0008;
const SInt16 flag_NoObstruct			= 0x0010;
const SInt16 flag_NoMediaObstruct		= 0x0020;
const SInt16 flag_IsAmbient				= 0x0040;

const SInt16 chance_10Percent			= 32768 * 9 / 10;	// all of these were extracted
const SInt16 chance_20Percent			= 32768 * 8 / 10;	// from the M2 source code.
const SInt16 chance_30Percent			= 32768 * 7 / 10;
const SInt16 chance_40Percent			= 32768 * 6 / 10;
const SInt16 chance_50Percent			= 32768 * 5 / 10;
const SInt16 chance_60Percent			= 32768 * 4 / 10;
const SInt16 chance_70Percent			= 32768 * 3 / 10;
const SInt16 chance_80Percent			= 32768 * 2 / 10;
const SInt16 chance_90Percent			= 32768 * 1 / 10;
const SInt16 chance_Always				= 0;

#pragma options align=mac68k	// Mthon structs are 68k-aligned.

struct SMthonSoundClass
{
	SInt16			mClassID;			// ID of this sound class. -1 if unused.
	SInt16			mVolume;			// volume of the sounds of this class.
	SInt16			mFlags;				// sound flags. see the constants above.
	
//	SInt32			mReserved1;			// old reserved fields are kept named
//	SInt32			mReserved2;			// like this to be backward compatible with
//	SInt16			mReserved3;			// the existing code.

	SInt16			mChance;			// chance of sound being played
	Fixed			mLowPitch;			// lowest possible sound pitch (1.0 is as recorded)
	Fixed			mHighPitch;			// highest possible sound pitch

	SInt16			mNumSounds;			// number of sounds in the class.
	
	SInt16			mReserved4;
	
	SInt32			mFirstSoundOffset;	// offset to the first sound in the file.
	SInt32			mFirstSoundLength;	// obvious.
	SInt32			mTotalLength;		// length of all sounds of this class.
	
	SInt32			mSoundOffset[5];	// offsets of each sound, relative to
										// mFirstSoundOffset. first offset is therefore 0.
										
	SInt32			mReserved5;
	SInt32			mReserved6;
	SInt32			mReserved7;			// all reserved must be 0.
	
	// we provide an inlined constructor that will initialize stuff that
	// we don't care about, so we only have to set what's interesting when
	// using the object.
					SMthonSoundClass();
};
typedef struct SMthonSoundClass SMthonSoundClass;

#pragma options align=reset

inline
SMthonSoundClass::SMthonSoundClass()
{
	// to be backward-compatible, new fields must be set to 0.
	mChance = chance_Always;
	mLowPitch = 0;
	mHighPitch = 0;

	mReserved4 = 0;
	mReserved5 = 0;
	mReserved6 = 0;
	mReserved7 = 0;
	
	// safekeeping:
	mSoundOffset[0] = 0;	// this one is always 0, so intialize it here for safety.
}


// ---------------------------------------------------------------------------------
//  CWailSoundClass declaration
// ---------------------------------------------------------------------------------
// CWailSoundClass is a class that will store the data about a sound class in this
// application. it is a little more practical for editing than the Marathon way of
// storing it. sounds are kept in LStream objects, so they can be stored either in
// memory (with LDataStream or LHandleStream objects) or in files (with LFileStream
// objects).
//
// objects can be created directly from two SMthonSoundClass objects (one for 8-bit
// sounds, one for 16-bit sounds). you can then call ReadSounds, passing a valid
// LStream pointing to a Marathon sound file, to read in all the sounds.

// constant representing the header of a mac sound:

const SInt32 macSound_Header[5] = {0x00010001,0x00050000,0x00A00001,0x80510000,0x00000014};

class CWailSoundClass
{
	public:
	// member variables.
	
		SInt16		mClassID;			// ID of the class.
		SInt16		mVolume;			// volume of the sounds.
		SInt16		mFlags;				// sound flags.
		
		SInt16		mChance;			// new flags.
		Fixed		mLowPitch;			// they were found in Anvil's backup
		Fixed		mHighPitch;			// sound window in 1.0.3b4.
		
		SInt16		mNum8bitSounds;		// number of 8-bit sounds in the class.
		LStream		*m8bitSounds[5];	// 8-bit sounds.
		
		SInt16		mNum16bitSounds;	// number of 16-bit sounds in the class.
		LStream		*m16bitSounds[5];	// 16-bit sounds.
		Boolean		mRemap8bit;			// 8-bit remapping support.
										// if true, mNum16bitSounds MUST be 0.
	
	// Public Functions
		//Default Constructor
							CWailSoundClass();
							
		//Constructor from a SMthonSoundClass object
							CWailSoundClass(
								const SMthonSoundClass& in8bitClass,
								const SMthonSoundClass& in16bitClass );
		
		//Destructor
		virtual				~CWailSoundClass();
		
		void				ReadSounds(
								LStream					*inMthonSoundFile,
								const SMthonSoundClass&	in8bitClass,
								const SMthonSoundClass& in16bitClass,
								bool					inMapViewOfFile = false );
									
		// comparing classes
		
		virtual bool		AreAttributesSame(
								const CWailSoundClass&	inClass ) const;
		virtual bool		Are8bitSoundsSame(
								const CWailSoundClass&	inClass ) const;
		virtual bool		Are16bitSoundsSame(
								const CWailSoundClass&	inClass ) const;
		
		bool				operator==(
								const CWailSoundClass&	inClass ) const;
		bool				operator!=(
								const CWailSoundClass&	inClass ) const;
									
		// static helper functions
		
		static Handle		TurnMthonSoundIntoMacSound(
								Handle 					inMthonSoundH );
		static Handle		TurnMacSoundIntoMthonSound(
								Handle					inMacSoundH );
								
		static SInt16		RoundChance(
								SInt16					inChance );
		
	private:
		// Defensive programming. No copy constructor or operator=
							CWailSoundClass( const CWailSoundClass& inOriginal );
		CWailSoundClass&	operator=( const CWailSoundClass& );
};


// ---------------------------------------------------------------------------------
//  CWailSoundFileData declaration
// ---------------------------------------------------------------------------------

class CWailSoundFileData
{
	public:
	// Public Functions
		//Default Constructor
								CWailSoundFileData();
							
		//Stream Constructor
								CWailSoundFileData(
									LStream *inMthonSoundFile,
									bool	inMapViewOfFile = false );
		
		//Destructor
		virtual					~CWailSoundFileData();
		
		// loading/saving
		
		void					LoadFromFile(
									LStream *inMthonSoundFile,
									bool	inMapViewOfFile = false );
		void					SaveToFile(
									LStream	*inMthonSoundFile );
									
		// Clearing
									
		void					Clear();
		void					ClearViewedStream();
									
		// compare files feature:
		
		void					CompareAndKeepOnlyDiffs(
									const CWailSoundFileData& inData );
		
	// Public member variables.
		
		TArray<CWailSoundClass*>	mSoundClasses;	// array of sound classes.
		Boolean						mDemoLayout;	// whether we're using the layout of
													// the M2 Demo Sounds file
													// (no 16-bit sounds allowed).
		
	protected:
	
		// internal handling of the compare routine
		
		void					CompareAndKeepOnlyDiffsTogether(
									const CWailSoundFileData& inData );
		void					CompareAndKeepOnlyDiffsSeparately(
									const CWailSoundFileData& inData );
									
		// Getting/setting the viewed stream
		
		LStream*				GetViewedStream() { return mViewedStream; }
		void					SetViewedStream(
									LStream*	inStream );
		
	private:
	
	// Private member variables.
	
		LStream*				mViewedStream;
	
		// Defensive programming. No copy constructor or operator=
								CWailSoundFileData( const CWailSoundFileData& );
		CWailSoundFileData&		operator=( const CWailSoundFileData& );
};
