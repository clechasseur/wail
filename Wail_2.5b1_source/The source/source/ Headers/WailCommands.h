// =================================================================================
//	WailCommands					©1999, Charles Lechasseur
// =================================================================================
// constants for Wail menu commands.

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


// menu commands for sound files:

const CommandT	cmd_MakeShuttleFromFile		= '+Shu';
const CommandT	cmd_CompareWithFile			= '??Fi';
const CommandT	cmd_CompareWithWindow		= '??Wi';

// menu commands for the class list:

const CommandT	cmd_AddClass				= '+Cla';
const CommandT	cmd_AddClasses				= '+#Cl';
const CommandT	cmd_RemoveClass				= '-Cla';
// -
const CommandT	cmd_FindClassByID			= '?ID_';
const CommandT	cmd_FindClassByName			= '?Nam';
const CommandT	cmd_FindClassByNumber		= '?#Nu';

// menu commands for the sound lists:

const CommandT	cmd_ImportSys7Sound			= '+s7s';
const CommandT	cmd_ExportSys7Sound			= '-s7s';
// -
const CommandT	cmd_ImportAiffSound			= '+aif';
const CommandT	cmd_ExportAiffSound			= '-aif';
// -
const CommandT	cmd_PlaySound				= 'PSnd';
const CommandT	cmd_LoopSound				= 'LSnd';
const CommandT	cmd_StopSound				= '.Snd';
// -
const CommandT 	cmd_DeleteSound				= '-Snd';

// additional commands that we might need to handle (even though not in any menus):

const CommandT	cmd_ImportSys7_8bitSound	= '+7_8';
const CommandT	cmd_ImportSys7_16bitSound	= '+716';
const CommandT	cmd_ExportSys7_8bitSound	= '-7_8';
const CommandT	cmd_ExportSys7_16bitSound	= '-716';
// -
const CommandT	cmd_ImportAiff8bitSound		= '+a_8';
const CommandT	cmd_ImportAiff16bitSound	= '+a16';
const CommandT	cmd_ExportAiff8bitSound		= '-a_8';
const CommandT	cmd_ExportAiff16bitSound	= '-a16';
// -
const CommandT	cmd_Play8bitSound			= 'PS8b';
const CommandT	cmd_Play16bitSound			= 'PS16';
const CommandT	cmd_Loop8bitSound			= 'LS8b';
const CommandT	cmd_Loop16bitSound			= 'LS16';

const CommandT	cmd_Cut8bitSound			= 'Cx8b';
const CommandT	cmd_Cut16bitSound			= 'Cx16';
const CommandT	cmd_Copy8bitSound			= 'Cc8b';
const CommandT	cmd_Copy16bitSound			= 'Cc16';
const CommandT	cmd_Paste8bitSound			= 'Cv8b';
const CommandT	cmd_Paste16bitSound			= 'Cv16';
const CommandT	cmd_Clear8bitSound			= 'C-8b';
const CommandT	cmd_Clear16bitSound			= 'C-16';