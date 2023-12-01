// =================================================================================
//	UVirtualMemory.cp					©1999, Charles Lechasseur
// =================================================================================
//
// Utility functions for initializing and disposing of the virtual memory system.
//
// Call Initialize in your app's initialization or constructor to init the virtual
// memory system. if you wish, you can pass a default virtual memory manager that
// all virtual streams will use as default (see CVirtualMemoryManager.cp for details).
//
// Call Finalize at the end of your program (typically in your app's destructor).
//
// Call GetDefaultManager to get a pointer to the default virtual memory manager.

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


#include "UVirtualMemory.h"

#include "CVirtualMemoryManager.h"


// static members
CVirtualMemoryManager*	UVirtualMemory::sDefaultManager = NULL;


// ---------------------------------------------------------------------------------
//		¥ Initialize
// ---------------------------------------------------------------------------------
// call at the beginning of your program.
// if you pass a valid virtual memory manager, it will be considered as being owned
// by the virtual memory manager system and will be disposed of when Finalize is called.

void
UVirtualMemory::Initialize(
	CVirtualMemoryManager*	inDefaultManager )
{
	sDefaultManager = inDefaultManager;
}


// ---------------------------------------------------------------------------------
//		¥ Finalize
// ---------------------------------------------------------------------------------
// call at the end of your program.
// disposes of the default virtual memory manager if it was passed to Initialize.

void
UVirtualMemory::Finalize()
{
	if (sDefaultManager != NULL)
	{
		delete sDefaultManager;
		sDefaultManager = NULL;
	}
}


// ---------------------------------------------------------------------------------
//		¥ GetDefaultManager
// ---------------------------------------------------------------------------------
// returns a pointer to the default virtual memory manager.

CVirtualMemoryManager*
UVirtualMemory::GetDefaultManager()
{
	return sDefaultManager;
}