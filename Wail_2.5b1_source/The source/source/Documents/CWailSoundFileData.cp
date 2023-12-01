// =================================================================================
//	CWailSoundFileData.cp					©1999, Charles Lechasseur
// =================================================================================
//
// CWailSoundFileData is a class that is designed to hold information about
// the content of a sound file. it has no member functions except Constructors
// to build the object according to input data (either provide a LStream from
// which we can read the data, or another CWailSoundFileData to copy from) and
// a destructor to clean up any created pointers or handles the object might
// have allocated.
//
// this object will be used by the CWailDocWindow objects to hold information
// about their files.
//
// basically, a CWailSoundFileData will contain an array of sound classes.
// each sound class will hold its 5 sounds in LStream objects, and information
// about the various flags of the class, as well as the class ID.
//
// this file contains the declaration/definition of three other classes/structs as
// well: SMthonSoundHeader, which is the header of a Marathon sound file,
// SMthonSoundClass, which represents the exact way class data is stored in
// a Marathon sound file, and CWailSoundClass, which is the way class data is stored
// in this application. CWailSoundClass is a class with a constructor that can build
// the class from a SMthonSoundClass object. others are just struct. we provide
// inlined constructors for those structs that will take care of initializing the
// unimportant stuff, so we don't have to care about them.

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

#include "CWailSoundFileData.h"

#include "CWailProgressDialog.h"
#include "UWailPreferences.h"

#include "CWailSoundStream.h"


#pragma mark -- CWailSoundClass --

// ---------------------------------------------------------------------------------
//		¥ CWailSoundClass			Default Constructor
// ---------------------------------------------------------------------------------

CWailSoundClass::CWailSoundClass()
{
	// initialize an empty class.
	mClassID = classID_Unused;
	mVolume = volume_Soft;		// to match M00's standard for unused classes.
	mFlags = 0;
	
	// new flags are set to 0 to be backward-compatible.
	mChance = chance_Always;
	mLowPitch = 0;
	mHighPitch = 0;
	
	mNum8bitSounds = 0;
	int i;
	for (i = 0; i < 5; i++)
		m8bitSounds[i] = nil;
		
	mNum16bitSounds = 0;
	for (i = 0; i < 5; i++)
		m16bitSounds[i] = nil;
	mRemap8bit = false;
}


// ---------------------------------------------------------------------------------
//		¥ CWailSoundClass			Constructor from a SMthonSoundClass object
// ---------------------------------------------------------------------------------

CWailSoundClass::CWailSoundClass(
	const SMthonSoundClass& in8bitClass,
	const SMthonSoundClass& in16bitClass )
{
	// use the info from the SMthonSoundClass objects to initialize this class.
	//
	// in this case, both 8-bit and 16-bit sounds are supposed to have the same
	// parameters (class ID, volume, flags). with sound files created with the
	// Marathon Sound Editor or stripped with the M2/M° Sound Shrinker, it might
	// not be the case. this was my mistake, so I must fix it here. we'll read
	// info from the 8-bit version. this should be valid, except for sound files
	// that were stripped of their 8-bit sounds with the "Remove 8-bit Sounds"
	// command of the Sound Editor. so, if the class ID ends up being -1, we
	// will re-read the info from the 16-bit version... if they're the same, it
	// won't hurt too much anyway.
	
	// try to read from the 8-bit version.
	mClassID = in8bitClass.mClassID;
	mVolume = in8bitClass.mVolume;
	mFlags = in8bitClass.mFlags;
	mChance = in8bitClass.mChance;
	mLowPitch = in8bitClass.mLowPitch;
	mHighPitch = in8bitClass.mHighPitch;
	if (mClassID == -1) // maybe it's a mistake. check the 16-bit version.
	{
		mClassID = in16bitClass.mClassID;
		mVolume = in16bitClass.mVolume;
		mFlags = in16bitClass.mFlags;
		mChance = in16bitClass.mChance;
		mLowPitch = in16bitClass.mLowPitch;
		mHighPitch = in16bitClass.mHighPitch;
	}
	
	mNum8bitSounds = in8bitClass.mNumSounds;
	int i;
	for (i = 0; i < 5; i++)
		m8bitSounds[i] = nil;
		
	mNum16bitSounds = in16bitClass.mNumSounds;
	for (i = 0; i < 5; i++)
		m16bitSounds[i] = nil;
	// sounds must be stored by calling ReadSounds.
	
	// bugfix: if the classes specify impossible values for the first sound offset
	// (e.g. negative values), consider the class empty. case where this was necessary:
	// the sound file from "Marathon: RED" which had an unused class with seemingly broken values...
	if (in8bitClass.mFirstSoundOffset < 0)
		mNum8bitSounds = 0;
	if (in16bitClass.mFirstSoundOffset < 0)
		mNum16bitSounds = 0;
	
	// determine here if 8-bit sounds are remapped into the 16-bit slots:
	mRemap8bit = ((in8bitClass.mFirstSoundOffset == in16bitClass.mFirstSoundOffset)
					&& (in8bitClass.mClassID != classID_Unused)
					&& (in16bitClass.mClassID != classID_Unused));
	if (mRemap8bit)
		mNum16bitSounds = 0;
}


// ---------------------------------------------------------------------------------
//		¥ ~CWailSoundClass			Destructor
// ---------------------------------------------------------------------------------

CWailSoundClass::~CWailSoundClass()
{
	// delete the streams if they exist.
	int i;
	for (i = 0; i < mNum8bitSounds; i++)
		if (m8bitSounds[i] != nil)
			delete (m8bitSounds[i]);
	for (i = 0; i < mNum16bitSounds; i++)
		if (m16bitSounds[i] != nil)
			delete (m16bitSounds[i]);
}

// ---------------------------------------------------------------------------------
//		¥ ReadSounds
// ---------------------------------------------------------------------------------
// provided with a valid LStream object that represents a Marathon sound file
// and two SMthonSoundClass objects to tell it where to look, this function will read in
// all sounds for this class and store them in the appropriate member variable.
//
// call this after constructing the object with the same SMthonSoundClass parameters.
//
// if inMapViewOfFile is true, we'll create sound streams by mapping views of the
// provided stream. this will save memory by making sounds be accessed directly from
// within the file. ownership of the provided stream is not assumed in this case,
// so if you use this method, the stream must be kept alive elsewhere.

void
CWailSoundClass::ReadSounds(
	LStream					*inMthonSoundFile,
	const SMthonSoundClass&	in8bitClass,
	const SMthonSoundClass& in16bitClass,
	bool					inMapViewOfFile /*= false*/ )
{
	// see if the stream is valid.
	ThrowIfNil_( inMthonSoundFile );
	
	int i = 0;
	
	// in a marathon sound file, sounds of the same class are always stored one after
	// the other. this is very useful since it means we can position the marker of the stream
	// and read all sounds. however we must figure out the sound sizes.
	//
	// strangely, the first sound's size is stored separately in mFirstSoundLength. other
	// sizes must be calculated by substracting the offset of the sound to the offset of
	// the next sound. one exception is of course the last sound, which has no next sound...
	// so we substract its offset to the total size of all sounds, which is known in
	// mTotalLength.
	
	// first we'll read 8-bit sounds if we have some.
	if (mNum8bitSounds > 0)
	{
		// position the marker.
		inMthonSoundFile->SetMarker( in8bitClass.mFirstSoundOffset, streamFrom_Start );
		
		for (i = 1; i <= in8bitClass.mNumSounds; i++)
		{
			// determine sound length.
			SInt32	soundLength;
			
			if (i == 1)	// first sound.
				soundLength = in8bitClass.mFirstSoundLength;
			else if (i == in8bitClass.mNumSounds) // last sound.
				soundLength = in8bitClass.mTotalLength - in8bitClass.mSoundOffset[i - 1];
			else		// another sound between the first and the last.
				soundLength = in8bitClass.mSoundOffset[i] - in8bitClass.mSoundOffset[i - 1];
				
			// create the sound stream, depending on the way user wants it.
			CWailSoundStream* theStream = nil;
				
			if (inMapViewOfFile)
			{
				// user wants to keep sound in file and access it from there using a stream view.
				theStream = new CWailSoundStream( inMthonSoundFile,
												  inMthonSoundFile->GetMarker(),
												  soundLength );
				// the stream moved the marker of the sound file stream so that it is
				// positioned at the beginning of the sound. move it back to the end
				// of the sound so that next sound is read properly.
				inMthonSoundFile->SetMarker( soundLength, streamFrom_Marker );
			}
			else
			{
				// user wants to read sound from file and store it independently.
				// create a handle to store the sound.
				Handle theHandle = ::NewHandle( soundLength );
				ThrowIfNil_( theHandle );
				
				{
					// since we're reading this from a Handle, we must lock the handle.
					StHandleLocker theHandleLocker( theHandle );
					
					// read the sound.
					inMthonSoundFile->ReadBlock( *theHandle, soundLength );
				}
				
				// store the handle in a stream.
				theStream = new CWailSoundStream( theHandle );
					// the stream now "owns" the handle and will dispose of it when destroyed.
			}
			
			// store this stream in our member variable.
			SignalIf_( theStream == nil );
			m8bitSounds[i - 1] = theStream;
		}
	}
	
	// next, we'll read 16-bit sounds if we have some (and they're not remapped).
	if ((mNum16bitSounds > 0) && (!mRemap8bit))
	{
		// position the marker.
		inMthonSoundFile->SetMarker( in16bitClass.mFirstSoundOffset, streamFrom_Start );
	
		for (i = 1; i <= in16bitClass.mNumSounds; i++)
		{
			// determine sound length.
			SInt32	soundLength;
			
			if (i == 1)	// first sound.
				soundLength = in16bitClass.mFirstSoundLength;
			else if (i == in16bitClass.mNumSounds) // last sound.
				soundLength = in16bitClass.mTotalLength - in16bitClass.mSoundOffset[i - 1];
			else		// another sound between the first and the last.
				soundLength = in16bitClass.mSoundOffset[i] - in16bitClass.mSoundOffset[i - 1];
				
			// create the sound stream, depending on the way user wants it.
			CWailSoundStream* theStream = nil;
				
			if (inMapViewOfFile)
			{
				// user wants to keep sound in file and access it from there using a stream view.
				theStream = new CWailSoundStream( inMthonSoundFile,
												  inMthonSoundFile->GetMarker(),
												  soundLength );
				// the stream moved the marker of the sound file stream so that it is
				// positioned at the beginning of the sound. move it back to the end
				// of the sound so that next sound is read properly.
				inMthonSoundFile->SetMarker( soundLength, streamFrom_Marker );
			}
			else
			{
				// user wants to read sound from file and store it independently.
				// create a handle to store the sound.
				Handle theHandle = ::NewHandle( soundLength );
				ThrowIfNil_( theHandle );
				
				{
					// since we're reading this from a Handle, we must lock the handle.
					StHandleLocker theHandleLocker( theHandle );
					
					// read the sound.
					inMthonSoundFile->ReadBlock( *theHandle, soundLength );
				}
				
				// store the handle in a stream.
				theStream = new CWailSoundStream( theHandle );
					// the stream now "owns" the handle and will dispose of it when destroyed.
			}
			
			// store this stream in our member variable.
			SignalIf_( theStream == nil );
			m16bitSounds[i - 1] = theStream;
		}
	}
}


// ---------------------------------------------------------------------------------
//		¥ AreAttributesSame
// ---------------------------------------------------------------------------------
// returns true if all attributes of the two classes are similar except for sounds.

bool
CWailSoundClass::AreAttributesSame(
	const CWailSoundClass&	inClass ) const
{
	return (
			(mClassID == inClass.mClassID) &&
			(mVolume == inClass.mVolume) &&
			(mFlags == inClass.mFlags) &&
			(mChance == inClass.mChance) &&
			(mLowPitch == inClass.mLowPitch) &&
			(mHighPitch == inClass.mHighPitch)
		   );
}


// ---------------------------------------------------------------------------------
//		¥ Are8bitSoundsSame
// ---------------------------------------------------------------------------------
// returns true if all 8-bit sounds of the two classes are similar.

bool
CWailSoundClass::Are8bitSoundsSame(
	const CWailSoundClass&	inClass ) const
{
	Boolean isSame = ((&inClass != this) &&		// to avoid self-checking.
					  (inClass.mNum8bitSounds == mNum8bitSounds));
	
	SInt16 i = 0;
	while (isSame && (i < mNum8bitSounds))
	{
		// read both sounds.
		SInt32 ourLength = m8bitSounds[i]->GetLength();
		SInt32 theirLength = inClass.m8bitSounds[i]->GetLength();
		
		StPointerBlock ourSound( ourLength );
		m8bitSounds[i]->SetMarker( 0, streamFrom_Start );
		m8bitSounds[i]->ReadBlock( ourSound, ourLength );
		
		StPointerBlock theirSound( theirLength );
		inClass.m8bitSounds[i]->SetMarker( 0, streamFrom_Start );
		inClass.m8bitSounds[i]->ReadBlock( theirSound, theirLength );
										   
		// compare them
		isSame = isSame &&
				 (BlockCompare( ourSound, theirSound, ourLength, theirLength ) == 0);
		
		i++;
	}
	
	return isSame;
}


// ---------------------------------------------------------------------------------
//		¥ Are16bitSoundsSame
// ---------------------------------------------------------------------------------
// returns true if all 16-bit sounds of the two classes are similar.

bool
CWailSoundClass::Are16bitSoundsSame(
	const CWailSoundClass&	inClass ) const
{
	Boolean isSame = ((&inClass != this) &&		// to avoid self-checking.
					  (inClass.mNum16bitSounds == mNum16bitSounds) &&
					  (inClass.mRemap8bit == mRemap8bit));
	
	SInt16 i = 0;
	while (isSame && (i < mNum16bitSounds))
	{
		// read both sounds.
		SInt32 ourLength = m16bitSounds[i]->GetLength();
		SInt32 theirLength = inClass.m16bitSounds[i]->GetLength();
		
		StPointerBlock ourSound( ourLength );
		m16bitSounds[i]->SetMarker( 0, streamFrom_Start );
		m16bitSounds[i]->ReadBlock( ourSound, ourLength );
		
		StPointerBlock theirSound( theirLength );
		inClass.m16bitSounds[i]->SetMarker( 0, streamFrom_Start );
		inClass.m16bitSounds[i]->ReadBlock( theirSound, theirLength );
										   
		// compare them
		isSame = isSame &&
				 (BlockCompare( ourSound, theirSound, ourLength, theirLength ) == 0);
		
		i++;
	}
	
	return isSame;
}


// ---------------------------------------------------------------------------------
//		¥ operator ==
// ---------------------------------------------------------------------------------
// tests if two classes are the same. two classes are the same if all their attributes
// are the same and if all their sounds are the same (block-comparing them).

bool
CWailSoundClass::operator==(
	const CWailSoundClass&	inClass ) const
{
	return (AreAttributesSame( inClass ) &&
			Are8bitSoundsSame( inClass ) &&
			Are16bitSoundsSame( inClass ));
}


// ---------------------------------------------------------------------------------
//		¥ operator !=
// ---------------------------------------------------------------------------------
// tests if two classes are different. two classes are the same if all their attributes
// are the same and if all their sounds are the same (block-comparing them).

bool
CWailSoundClass::operator!=(
	const CWailSoundClass&	inClass ) const
{
	return (!AreAttributesSame( inClass ) ||
			!Are8bitSoundsSame( inClass ) ||
			!Are16bitSoundsSame( inClass ));
}


// ---------------------------------------------------------------------------------
//		¥ TurnMthonSoundIntoMacSound						[static]
// ---------------------------------------------------------------------------------
// static helper function to turn a Marathon sound into a mac sound by expanding
// the handle and add a sound header.
//
// the resulting mac sound is returned as a handle.

Handle
CWailSoundClass::TurnMthonSoundIntoMacSound(
	Handle 	inMthonSoundH )
{
	// let's create a handle of the appropriate size.
	Handle theMacSoundH = ::NewHandle( ::GetHandleSize( inMthonSoundH ) + (sizeof(SInt32) * 5) );
	ThrowIfNil_( theMacSoundH );
	
	// write the header to the new handle.
	::BlockMoveData( macSound_Header, *theMacSoundH, sizeof(SInt32) * 5 );
	
	// write the marathon sound to the new handle.
	::BlockMoveData( *inMthonSoundH, (*theMacSoundH) + (sizeof(SInt32) * 5), 
					::GetHandleSize( inMthonSoundH ) );
					
	// return the newly created mac sound.
	return theMacSoundH;
}


// ---------------------------------------------------------------------------------
//		¥ TurnMacSoundIntoMthonSound						[static]
// ---------------------------------------------------------------------------------
// static helper function to turn a mac sound into a Marathon sound by suppressing
// its header and reducing the handle size.
//
// the resulting marathon sound is returned as a handle.

Handle
CWailSoundClass::TurnMacSoundIntoMthonSound(
	Handle	inMacSoundH )
{
	// create a new handle of the appropriate size.
	Handle theMthonSoundH = ::NewHandle( ::GetHandleSize( inMacSoundH ) - (sizeof(SInt32) * 5) );
	ThrowIfNil_( theMthonSoundH );
				   
	// write the marathon sound to the new handle.
	::BlockMoveData( (*inMacSoundH) + (sizeof(SInt32) * 5), *theMthonSoundH, 
					::GetHandleSize( theMthonSoundH ) );
					
	// return the newly created marathon sound.
	return theMthonSoundH;
}


// ---------------------------------------------------------------------------
//		¥ RoundChance										[static]
// ---------------------------------------------------------------------------
// rounds up the given value for the "Chance" field of a sound class to the
// nearest defined value.

SInt16
CWailSoundClass::RoundChance(
	SInt16	inChance )
{
	SInt16 theRoundedChance = chance_Always;

	// define a handy macro to find the midpoint between two integers.
	// macros might be ugly, but here they might be useful.
#define _Midpoint(x,y) (((x) + (y)) / 2)

	if (inChance > _Midpoint(chance_10Percent, chance_20Percent))
		theRoundedChance = chance_10Percent;
	else if ((inChance <= _Midpoint(chance_10Percent, chance_20Percent)) &&
			 (inChance > _Midpoint(chance_20Percent, chance_30Percent)))
		theRoundedChance = chance_20Percent;
	else if ((inChance <= _Midpoint(chance_20Percent, chance_30Percent)) &&
			 (inChance > _Midpoint(chance_30Percent, chance_40Percent)))
		theRoundedChance = chance_30Percent;
	else if ((inChance <= _Midpoint(chance_30Percent, chance_40Percent)) &&
			 (inChance > _Midpoint(chance_40Percent, chance_50Percent)))
		theRoundedChance = chance_40Percent;
	else if ((inChance <= _Midpoint(chance_40Percent, chance_50Percent)) &&
			 (inChance > _Midpoint(chance_50Percent, chance_60Percent)))
		theRoundedChance = chance_50Percent;
	else if ((inChance <= _Midpoint(chance_50Percent, chance_60Percent)) &&
			 (inChance > _Midpoint(chance_60Percent, chance_70Percent)))
		theRoundedChance = chance_60Percent;
	else if ((inChance <= _Midpoint(chance_60Percent, chance_70Percent)) &&
			 (inChance > _Midpoint(chance_70Percent, chance_80Percent)))
		theRoundedChance = chance_70Percent;
	else if ((inChance <= _Midpoint(chance_70Percent, chance_80Percent)) &&
			 (inChance > _Midpoint(chance_80Percent, chance_90Percent)))
		theRoundedChance = chance_80Percent;
	else if ((inChance <= _Midpoint(chance_80Percent, chance_90Percent)) &&
			 (inChance > _Midpoint(chance_90Percent, chance_Always)))
		theRoundedChance = chance_90Percent;
	else if (inChance <= _Midpoint(chance_90Percent, chance_Always))
		theRoundedChance = chance_Always;
	
	// no need to propagate our macro.
#undef _Midpoint

	return theRoundedChance;
}


#pragma mark -- CWailSoundFileData --

// ---------------------------------------------------------------------------
//	¥ CWailSoundFileData						Default Constructor
// ---------------------------------------------------------------------------

CWailSoundFileData::CWailSoundFileData()
	: mSoundClasses(),
	  mDemoLayout( FALSE ),
	  mViewedStream( NULL )
{
	// do nothing, since we don't have any data.
}


// ---------------------------------------------------------------------------
//	¥ CWailSoundFileData						Stream Constructor
// ---------------------------------------------------------------------------
// in this case, the LStream object passed to this constructor must point to
// a valid Marathon Sound File. most likely you will want to pass a LFileStream
// object, but you might also read the entire file in a LHandleStream if you
// have tons of RAM. (i don't, so you probably won't see it in this source :)
//
// if inMapViewOfFile is true, it means we must use stream views to load the
// sounds directly from the provided stream. in this case, WE ASSUME OWNERSHIP
// of the provided stream and use it to create stream views for our sounds.

CWailSoundFileData::CWailSoundFileData(
	LStream		*inMthonSoundFile,
	bool		inMapViewOfFile /*= false*/ )
	: mSoundClasses(),
	  mDemoLayout( FALSE ),
	  mViewedStream( NULL )
{
	ThrowIfNil_( inMthonSoundFile );

	LoadFromFile( inMthonSoundFile, inMapViewOfFile );
}


// ---------------------------------------------------------------------------
//	¥ ~CWailSoundFileData						Destructor
// ---------------------------------------------------------------------------

CWailSoundFileData::~CWailSoundFileData()
{
	Clear();
	
	// delete viewed stream if we have one.
	if (mViewedStream != NULL)
	{
		delete mViewedStream;
		mViewedStream = NULL;
	}
}


// ---------------------------------------------------------------------------
//		¥ LoadFromFile
// ---------------------------------------------------------------------------
// provided with a valid LStream object that contains an existing Marathon
// Sounds file, this function will read its data and save it in this class.
//
// if inMapViewOfFile is true, it means we must use stream views to load the
// sounds directly from the provided stream. in this case, WE ASSUME OWNERSHIP
// of the provided stream and use it to create stream views for our sounds.

void
CWailSoundFileData::LoadFromFile(
	LStream 	*inMthonSoundFile,
	bool		inMapViewOfFile /*= false*/ )
{
	ThrowIfNil_( inMthonSoundFile );
	
	// if user told us to map views of the file, save stream to view.
	if (inMapViewOfFile)
		SetViewedStream( inMthonSoundFile );
	
	// clear any already-existing data.
	Clear();

	// now read the new data.
	// we must first read the file header, to know exactly how many classes it contains.
	SMthonSoundHeader theHeader;
	inMthonSoundFile->SetMarker( 0, streamFrom_Start );
	inMthonSoundFile->ReadBlock( &theHeader, sizeof(SMthonSoundHeader) );
	
	// patch: the M2 Demo Sounds file wasn't layed out exactly like we thought...
	// it has mNumClasses set to 0, and mNumSets set to the number of sound classes.
	// it doesn't seem to have 16-bit sounds either. so what we'll do is we'll switch
	// it around here...
	if ((theHeader.mNumClasses == 0) && (theHeader.mNumSets > 0))
	{
		SInt16 numClasses = theHeader.mNumSets;
		theHeader.mNumSets = 1;
		theHeader.mNumClasses = numClasses;
		
		// remember that this data uses the M2 Demo layout so that we can save using
		// the same layout later on.
		mDemoLayout = TRUE;
	}
	else
	{
		// we're not using the M2 Demo Sounds file layout.
		mDemoLayout = FALSE;
	}
	
	// create a progress dialog.
	CWailProgressDialog theProgressDialog( (SInt32) theHeader.mNumClasses,
										   progressString_LoadingSoundData,
										   nil );
	
	// now, read each class one by one and store it. in order to create a CWailSoundClass
	// object, we must read two SMthonSoundClass objects from the file, one for 8-bit sounds
	// and one for 16-bit sounds.
	mSoundClasses.AdjustAllocation( theHeader.mNumClasses );
	for (SInt16 i = 1; i <= theHeader.mNumClasses; i++)
	{
		SMthonSoundClass	the8bitClass,
							the16bitClass;
	
		// only try to read 8-bit sounds if we have an 8-bit sound set.
		if (theHeader.mNumSets >= 1)
		{
			// position the marker for the 8-bit class.
			inMthonSoundFile->SetMarker( sizeof(SMthonSoundHeader)				// we skip the header,
										 + sizeof(SMthonSoundClass) * (i - 1),	// and all the 8-bit
										 										// classes before
										 										// this one.
										 streamFrom_Start );
			
			// read the 8-bit class.
			inMthonSoundFile->ReadBlock( &the8bitClass, sizeof(SMthonSoundClass) );
		}
		else
		{
			// no 8-bit set (not very likely, but hey)
			the8bitClass.mClassID = classID_Unused;
			the8bitClass.mVolume = volume_Soft;
			the8bitClass.mFlags = 0;
			
			the8bitClass.mNumSounds = 0;
			
			the8bitClass.mFirstSoundOffset = 0;
			the8bitClass.mFirstSoundLength = 0;
			the8bitClass.mTotalLength = 0;
			
			for (int j = 0; j < 5; ++j)
				the8bitClass.mSoundOffset[j] = 0;
		}
		
		// only try to read 16-bit sounds if we have a 16-bit sound set.
		if (theHeader.mNumSets >= 2)
		{
			// position the marker for the 16-bit class.
			inMthonSoundFile->SetMarker( sizeof(SMthonSoundHeader)				// we skip the header,
										 + sizeof(SMthonSoundClass) * theHeader.mNumClasses
																				// all 8-bit classes,
										 + sizeof(SMthonSoundClass) * (i - 1),	// and all the 16-bit
										 										// classes before
										 										// this one.
										 
										 streamFrom_Start );
			
			// read the 16-bit class.
			inMthonSoundFile->ReadBlock( &the16bitClass, sizeof(SMthonSoundClass) );
		}
		else
		{
			// no 16-bit set (true in the M2 Demo Sounds file, at least)
			the16bitClass.mClassID = classID_Unused;
			the16bitClass.mVolume = volume_Soft;
			the16bitClass.mFlags = 0;
			
			the16bitClass.mNumSounds = 0;
			
			the16bitClass.mFirstSoundOffset = 0;
			the16bitClass.mFirstSoundLength = 0;
			the16bitClass.mTotalLength = 0;
			
			for (int j = 0; j < 5; ++j)
				the16bitClass.mSoundOffset[j] = 0;
		}
		
		// create a new CWailSoundClass object and read all its sounds.
		CWailSoundClass *theSoundClass = new CWailSoundClass( the8bitClass, the16bitClass );
		theSoundClass->ReadSounds( inMthonSoundFile,
								   the8bitClass,
								   the16bitClass,
								   inMapViewOfFile );
		
		// store it in our array of classes.
		mSoundClasses.AddItem( theSoundClass );
		
		// increment the progress bar.
		theProgressDialog.Increment();
	}
}


// ---------------------------------------------------------------------------------
//		¥ SaveToFile
// ---------------------------------------------------------------------------------
// provided with a valid LStream object that points to a newly created file,
// SaveToFile will save the content of the SoundFileData object to the file, in Marathon
// format. this is what to call when the user selects "Save" or "Save As".

void
CWailSoundFileData::SaveToFile(
	LStream		*inMthonSoundFile )
{
	ThrowIfNil_( inMthonSoundFile );
	
	// set the stream's marker to the beginning of the stream.
	inMthonSoundFile->SetMarker( 0, streamFrom_Start );
	
	// ok. first, we have to write the sound file header.
	SMthonSoundHeader theHeader;
	if (mDemoLayout)
	{
		// we're using the M2 Demo Sounds file layout, which has mNumClasses set to 0
		// and mNumSets set to the number of classes.
		theHeader.mNumSets = mSoundClasses.GetCount();
		theHeader.mNumClasses = 0;
	}
	else
	{
		// we're using the standard layout, set number of classes in mNumClasses.
		theHeader.mNumClasses = mSoundClasses.GetCount();
	}
		// everything else is set in SMthonSoundHeader's inlined constructor.
	inMthonSoundFile->WriteBlock( &theHeader, sizeof(SMthonSoundHeader) );
	
	// now, switch header layout around if we set it up using the demo layout,
	// so that the rest of this function is easier to write.
	if (mDemoLayout)
	{
		theHeader.mNumClasses = theHeader.mNumSets;
		theHeader.mNumSets = 1;
	}
	
	// create a progress dialog.
	CWailProgressDialog theProgressDialog( theHeader.mNumClasses * 6,
										   progressString_SavingSoundData,
										   nil );
	
	// ok. now we have to make up the class header list. this is more tricky since we need to figure
	// out in advance the offset of the first sound of each class in the final file.
	// we will create an array of SMthonSoundClass objects and figure out each offset one by one.
	// figuring relative offsets is easy, but we need to start counting the first sound offsets.
	// we will create a variable called theCurrentOffset that will start right after the class header
	// list and keep track of where sounds will be written in the final file. this variable will
	// start right after the sound file header and class header list. each time we create a new
	// class header, we will add that class header's totalLength to theCurrentOffset.
	SInt32 theCurrentOffset = (
		/* skip sound file header: */ sizeof(SMthonSoundHeader) +
		/* skip class headers: */ ( sizeof(SMthonSoundClass) * theHeader.mNumClasses * theHeader.mNumSets )
							 );
	
	TArray<SMthonSoundClass*> theClassHeaderArray;	// we define our array. it's currently empty.
	
	SInt16 i;
	
	// create 8-bit class headers.
	if (theHeader.mNumSets >= 1)
	{
		theClassHeaderArray.AdjustAllocation( theHeader.mNumClasses );
		for (i = 0; i < theHeader.mNumClasses; i++)
		{
			// get the class info from our data.
			CWailSoundClass *theWailClass;
			mSoundClasses.FetchItemAt( i + 1, theWailClass );
			
			// create a new SMthonSoundClass struct.
			SMthonSoundClass *theMthonClass = new SMthonSoundClass;
			
			// copy trivial info.
			theMthonClass->mClassID = theWailClass->mClassID;
			theMthonClass->mVolume = theWailClass->mVolume;
			theMthonClass->mFlags = theWailClass->mFlags;
			
			theMthonClass->mChance = theWailClass->mChance;
			theMthonClass->mLowPitch = theWailClass->mLowPitch;
			theMthonClass->mHighPitch = theWailClass->mHighPitch;
			
			theMthonClass->mNumSounds = theWailClass->mNum8bitSounds;
			
			// set the first sound's offset to the current offset.
			if (theMthonClass->mNumSounds > 0)
				theMthonClass->mFirstSoundOffset = theCurrentOffset;
			else
				theMthonClass->mFirstSoundOffset = 0;
				
			theMthonClass->mSoundOffset[0] = 0; // this one is always 0. (go figure. :)
			
			// set the first sound's length.
			if (theMthonClass->mNumSounds > 0)
				theMthonClass->mFirstSoundLength = theWailClass->m8bitSounds[0]->GetLength();
			else
				theMthonClass->mFirstSoundLength = 0;
			
			// create a temporary variable to calculate the total length taken by sounds of
			// the class.
			SInt32 theTotalLength = theMthonClass->mFirstSoundLength;
			
			// calculate the relative offset of each sound, according to their
			// length, and also calculate the total length.
			SInt16 j;
			for (j = 1; j < theMthonClass->mNumSounds; j++)
			{
				// here's one damn easy way of calculating the offsets: the relative offset
				// of a sound is always equal to theTotalLength :) [think about it.]
				// we just need to set the sound's relative offset, then add the sound's size
				// to theTotalLength.
				theMthonClass->mSoundOffset[j] = theTotalLength;
				theTotalLength += theWailClass->m8bitSounds[j]->GetLength();
			}
			
			// fill up the array with 0, if there are slots left
			for (; j < 5; j++)
				theMthonClass->mSoundOffset[j] = 0;
			
			// now that we know the total length of all sounds for sure, set it.
			theMthonClass->mTotalLength = theTotalLength;
			
			// we now have the complete SMthonSoundClass struct filled up.
			// add it to our array.
			theClassHeaderArray.AddItem( theMthonClass );
			
			// add the total length to the current offset to work up the file.
			theCurrentOffset += theTotalLength;
			
			// increment the progress bar.
			theProgressDialog.Increment();
		}
	}
	
	// now, create 16-bit class headers.
	if (theHeader.mNumSets >= 2)
	{
		theClassHeaderArray.AdjustAllocation( theHeader.mNumClasses );
		for (i = 0; i < theHeader.mNumClasses; i++)
		{
			// get the class info from our data.
			CWailSoundClass *theWailClass;
			mSoundClasses.FetchItemAt( i + 1, theWailClass );
			
			// create a new SMthonSoundClass struct.
			SMthonSoundClass *theMthonClass = new SMthonSoundClass;
			
			// copy trivial info.
			theMthonClass->mClassID = theWailClass->mClassID;
			theMthonClass->mVolume = theWailClass->mVolume;
			theMthonClass->mFlags = theWailClass->mFlags;
			
			// here, it depends on whether the class is remapping 8-bit sounds or not.
			SInt32 theTotalLength;
			if (!theWailClass->mRemap8bit)
			{
				theMthonClass->mNumSounds = theWailClass->mNum16bitSounds;
				
				// set the first sound's offset to the current offset.
				if (theMthonClass->mNumSounds > 0)
					theMthonClass->mFirstSoundOffset = theCurrentOffset;
				else
					theMthonClass->mFirstSoundOffset = 0;
					
				theMthonClass->mSoundOffset[0] = 0; // this one is always 0. (go figure. :)
				
				// set the first sound's length.
				if (theMthonClass->mNumSounds > 0)
					theMthonClass->mFirstSoundLength = theWailClass->m16bitSounds[0]->GetLength();
				else
					theMthonClass->mFirstSoundLength = 0;
				
				// create a temporary variable to calculate the total length taken by sounds of
				// the class.
				theTotalLength = theMthonClass->mFirstSoundLength;
				
				// calculate the relative offset of each sound, according to their
				// length, and also calculate the total length.
				SInt16 j;
				for (j = 1; j < theMthonClass->mNumSounds; j++)
				{
					// here's one damn easy way of calculating the offsets: the relative offset
					// of a sound is always equal to theTotalLength :) [think about it.]
					// we just need to set the sound's relative offset, then add the sound's size
					// to theTotalLength.
					theMthonClass->mSoundOffset[j] = theTotalLength;
					theTotalLength += theWailClass->m16bitSounds[j]->GetLength();
				}
				
				// fill up the array with 0, if there are slots left
				for (; j < 5; j++)
					theMthonClass->mSoundOffset[j] = 0;
				
				// now that we know the total length of all sounds for sure, set it.
				theMthonClass->mTotalLength = theTotalLength;
				
				// we now have the complete SMthonSoundClass struct filled up.
				// add it to our array.
				theClassHeaderArray.AddItem( theMthonClass );
				
				// add the total length to the current offset to work up the file.
				theCurrentOffset += theTotalLength;
			}
			else
			{
				// the class is simply remapping 8-bit sounds, so we must fetch the
				// 8-bit class and copy its fields.
				SMthonSoundClass* the8bitClass;
				ThrowIf_( !theClassHeaderArray.FetchItemAt( i + 1, the8bitClass ) );
					// we won't need to free up this, since it's in the array already.
			
				theMthonClass->mNumSounds = the8bitClass->mNumSounds;
				theMthonClass->mFirstSoundOffset = the8bitClass->mFirstSoundOffset;
				theMthonClass->mFirstSoundLength = the8bitClass->mFirstSoundLength;
				SInt16 j;
				for (j = 0; j < 5; j++)
				{
					theMthonClass->mSoundOffset[j] = the8bitClass->mSoundOffset[j];
				}
				
				theMthonClass->mTotalLength = the8bitClass->mTotalLength;
				
				// we now have the complete SMthonSoundClass struct filled up.
				// add it to our array.
				theClassHeaderArray.AddItem( theMthonClass );
				
				// this class didn't add to the total length of the file, so don't touch the offset.
			}
			
			// increment the progress bar.
			theProgressDialog.Increment();
		}
	}
	
	// now that we have all the class headers, we can write them to the file.
	for (i = 1; i <= theClassHeaderArray.GetCount(); ++i)
	{
		// get the class header from the array.
		SMthonSoundClass *theClassHeader;
		theClassHeaderArray.FetchItemAt( i, theClassHeader );
	
		// write the class header to the stream.
		inMthonSoundFile->WriteBlock( theClassHeader, sizeof(SMthonSoundClass) );
		
		// increment the progress bar.
		theProgressDialog.Increment();
	}
	
	// now, all we must add are the sounds themselves.
	// start with 8-bit sounds.
	if (theHeader.mNumSets >= 1)
	{
		for (i = 0; i < theHeader.mNumClasses; i++)
		{
			// get the class from our data.
			CWailSoundClass *theWailClass;
			mSoundClasses.FetchItemAt( i + 1, theWailClass );
			
			// write all the sounds of that class.
			SInt16 j;
			for (j = 0; j < theWailClass->mNum8bitSounds; ++j)
			{
				// get the sound from the class data.
				Ptr theSoundP = ::NewPtr( theWailClass->m8bitSounds[j]->GetLength() );
				ThrowIfNil_( theSoundP );
				theWailClass->m8bitSounds[j]->SetMarker( 0, streamFrom_Start );
				theWailClass->m8bitSounds[j]->ReadBlock( theSoundP,
														 theWailClass->m8bitSounds[j]->GetLength() );
				
				// write it to the file.
				inMthonSoundFile->WriteBlock( theSoundP,
											  theWailClass->m8bitSounds[j]->GetLength() );
											  
				// dispose of the buffer.
				::DisposePtr( theSoundP );
			}
			
			// increment the progress bar.
			theProgressDialog.Increment();
		}
	}
	
	// now, 16-bit sounds.
	for (i = 0; i < theHeader.mNumClasses; i++)
	{
		// get the class from our data.
		CWailSoundClass *theWailClass;
		mSoundClasses.FetchItemAt( i + 1, theWailClass );
		
		// we only write 16-bit sounds if they're not remapped.
		if (!theWailClass->mRemap8bit)
		{
			// write all the sounds of that class.
			SInt16 j;
			for (j = 0; j < theWailClass->mNum16bitSounds; j++)
			{
				// get the sound from the class data.
				Ptr theSoundP = ::NewPtr( theWailClass->m16bitSounds[j]->GetLength() );
				ThrowIfNil_( theSoundP );
				theWailClass->m16bitSounds[j]->SetMarker( 0, streamFrom_Start );
				theWailClass->m16bitSounds[j]->ReadBlock( theSoundP,
														 theWailClass->m16bitSounds[j]->GetLength() );
				// write it to the file.
				inMthonSoundFile->WriteBlock( theSoundP,
											  theWailClass->m16bitSounds[j]->GetLength() );
											  
				// dispose of the buffer.
				::DisposePtr( theSoundP );	
			}
		}
		
		// increment the progress bar.
		theProgressDialog.Increment();
	}
	
	// clear the class header array.
	{
		TArrayIterator<SMthonSoundClass*> theIterator( theClassHeaderArray );
		SMthonSoundClass* theMthonClass = nil;
		while (theIterator.Next( theMthonClass ))
			delete theMthonClass;
			
		theClassHeaderArray.RemoveItemsAt( 0x7FFFFFFF, LArray::index_First );
	}
	
	// hide progress dialog since we're done writing.
	theProgressDialog.Hide();
	
	// we're done writing the data to the file.
}


// ---------------------------------------------------------------------------
//		¥ Clear
// ---------------------------------------------------------------------------
// clears all sound classes from this data.

void
CWailSoundFileData::Clear()
{
	// destroy all classes in the array.
	TArrayIterator<CWailSoundClass*> theIterator( mSoundClasses );
	CWailSoundClass* theClass = nil;
	while (theIterator.Next( theClass ))
		delete theClass;
		
	// clear the array.
	mSoundClasses.RemoveItemsAt( 0x7FFFFFFF, LArray::index_First );
}


// ---------------------------------------------------------------------------
//		¥ Function_name_here
// ---------------------------------------------------------------------------
// destroys the stream used to access our sounds via stream views, if any.
// if we do have such a stream, it is cleared and all our data is also cleared.

void
CWailSoundFileData::ClearViewedStream()
{
	if (mViewedStream != nil)
	{
		// clear all our data, since it is based on this stream, it will
		// no longer be valid after the stream is freed.
		Clear();
		
		// now delete the viewed stream.
		delete mViewedStream;
		mViewedStream = nil;
	}
}


// ---------------------------------------------------------------------------------
//		¥ CompareAndKeepOnlyDiffs
// ---------------------------------------------------------------------------------
// compares our data with the given data. remove from our data any class that
// is similar to the given class of the other data, and replaces it with
// an empty class.
//
// branches to the two protected functions to do the job, according to prefs.

void
CWailSoundFileData::CompareAndKeepOnlyDiffs(
	const CWailSoundFileData& inData )
{
	switch (UWailPreferences::CompareSettings())
	{
		case compareSetting_Together:
			CompareAndKeepOnlyDiffsTogether( inData );
			break;
			
		case compareSetting_Separately:
			CompareAndKeepOnlyDiffsSeparately( inData );
			break;
			
		default:	// programmer error or corruption?
			SignalStringLiteral_( "Invalid compare setting value" );
			break;
	}
}


// ---------------------------------------------------------------------------------
//		¥ CompareAndKeepOnlyDiffsTogether
// ---------------------------------------------------------------------------------
// compares our data with the given data. remove from our data any class that
// is similar to the given class of the other data, and replaces it with
// an empty class.
//
// classes are compared as a whole, not 8-bit and 16-bit sounds independently.

void
CWailSoundFileData::CompareAndKeepOnlyDiffsTogether(
	const CWailSoundFileData& inData )
{
	// if we don't have the same number of classes, compare up to the max number
	// of classes of the smallest data.
	SInt32 numClasses = inData.mSoundClasses.GetCount();
	if (mSoundClasses.GetCount() < numClasses)
		numClasses = mSoundClasses.GetCount();
		
	// create a progress dialog.
	CWailProgressDialog theProgressDialog( numClasses,
										   progressString_ComparingSoundData,
										   nil );
		
	// big compare and eliminate thingy.
	SInt32 i;
	for (i = 1; i <= numClasses; i++)
	{
		// fetch both classes.
		CWailSoundClass* ourClass = mSoundClasses[i];
		const CWailSoundClass* theirClass = inData.mSoundClasses[i];
	
		// are they the same?
		if (*ourClass == *theirClass)
		{
			// yes. delete ours and replace it with a blank one.
			delete ourClass;
			mSoundClasses[i] = new CWailSoundClass();
		}
	
		// increment progress dialog.
		theProgressDialog.Increment();
	}
}


// ---------------------------------------------------------------------------------
//		¥ CompareAndKeepOnlyDiffsSeparately
// ---------------------------------------------------------------------------------
// compares our data with the given data. remove from our data any class that
// is similar to the given class of the other data, and replaces it with
// an empty class.
//
// classes are compared this way: first, we compare everything for sounds (called "attributes").
// if they're the same, we go deeper and compare 8-bit and 16-bit sounds separately.
// if they end up being both the same, the whole class is dropped.

void
CWailSoundFileData::CompareAndKeepOnlyDiffsSeparately(
	const CWailSoundFileData& inData )
{
	// if we don't have the same number of classes, compare up to the max number
	// of classes of the smallest data.
	SInt32 numClasses = inData.mSoundClasses.GetCount();
	if (mSoundClasses.GetCount() < numClasses)
		numClasses = mSoundClasses.GetCount();
		
	// create a progress dialog.
	CWailProgressDialog theProgressDialog( numClasses,
										   progressString_ComparingSoundData,
										   nil );
		
	// big compare and eliminate thingy.
	SInt32 i;
	for (i = 1; i <= numClasses; i++)
	{
		// fetch both classes.
		CWailSoundClass* ourClass = mSoundClasses[i];
		const CWailSoundClass* theirClass = inData.mSoundClasses[i];
	
		// check attributes first.
		if (ourClass->AreAttributesSame( *theirClass ))
		{
			SInt16 numDifferences = 0;
			
			// check 8-bit sounds.
			if (ourClass->Are8bitSoundsSame( *theirClass ))
			{
				// we must delete 8-bit sounds from our class.
				SInt16 i;
				for (i = 0; i < ourClass->mNum8bitSounds; i++)
					delete ourClass->m8bitSounds[i];
				ourClass->mNum8bitSounds = 0;
				
				numDifferences++;
			}
			
			// check 16-bit sounds.
			if (ourClass->Are16bitSoundsSame( *theirClass ))
			{
				// we must delete 16-bit sounds from our class.
				SInt16 i;
				for (i = 0; i < ourClass->mNum16bitSounds; i++)
					delete ourClass->m16bitSounds[i];
				ourClass->mNum16bitSounds = 0;
				
				numDifferences++;
			}
			
			// if both are same, then delete the class altogether and replace it with a blank one.
			if (numDifferences == 2)
			{
				delete ourClass;
				mSoundClasses[i] = new CWailSoundClass();
			}
		}
	
		// increment progress dialog.
		theProgressDialog.Increment();
	}
}


// ---------------------------------------------------------------------------
//		¥ SetViewedStream
// ---------------------------------------------------------------------------
// sets the stream used to create stream views for our sounds. if set, we own
// this stream and will destroy it when done.

void
CWailSoundFileData::SetViewedStream(
	LStream*	inStream )
{
	// protect against self-assignation.
	if (inStream != mViewedStream)
	{
		// delete old stream.
		delete mViewedStream;
		
		// save new one.
		mViewedStream = inStream;
	}
}
