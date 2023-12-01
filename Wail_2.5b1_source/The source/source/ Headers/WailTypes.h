// =================================================================================
//	WailTypes.h					©1999, Charles Lechasseur
// =================================================================================
// constants for file types and creators used in Wail.

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

#ifndef __AIFF__
#include <AIFF.h>
#endif

// Standard file types

const OSType	fileType_Application				= 'APPL';
const OSType	fileType_Text						= 'TEXT';

// Wail types

const OSType	fileCreator_Wail					= 'W2il';

// Marathon sound files

const OSType	fileType_Marathon2Sound				= 'snd2';
const OSType	fileType_Marathon2Creator			= '52.4';

const OSType	fileType_MarathonInfinitySound		= 'snd°';
const OSType	fileCreator_MarathonInfinitySound	= '26.°';

// System-7 sound files

const OSType	fileType_System7Sound				= 'sfil';
const OSType	fileCreator_System7Sound			= 'movr';

// AIFF sound files

const OSType	fileType_AIFFSound					= AIFFID;	// both of those are
const OSType	fileType_AIFFCSound					= AIFCID;	// defines in AIFF.h

// Unknown creator (????)

const OSType	fileCreator_Unknown					= '????';