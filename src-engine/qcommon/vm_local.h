//////////////////////////////////////////////////////////////////////////////////////
//
//  This file is part of OWEngine source code.
//  Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.
//  Copyright (C) 2016 Dusan Jocic <dusanjocic@msn.com>
//
//  This file is part of the OWEngine single player GPL Source Code.
//
//  OWengine Source Code is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OWEngine Source Code is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with OWEngine Source Code. If not, see <http://www.gnu.org/licenses/>.
//
//  In addition, the OWEngine Source Code is also subject to certain additional terms.
//  You should have received a copy of these additional terms immediately following
//  the terms and conditions of the GNU General Public License which accompanied the
//  OWEngine Source Code. If not, please request a copy in writing from id Software
//  at the address below.
//
//  If you have questions concerning this license or the applicable additional terms,
//  you may contact in writing id Software LLC, c/o ZeniMax Media Inc.,
//  Suite 120, Rockville, Maryland 20850 USA.
//
// -------------------------------------------------------------------------
//  File name:   vm_local.h
//  Version:     v1.00
//  Created:
//  Compilers:
//  Description:
// -------------------------------------------------------------------------
//  History:
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __VM_LOCAL_H__
#define __VM_LOCAL_H__

#include "q_shared.h"
#include "qcommon.h"

typedef enum
{
    OP_UNDEF,
    
    OP_IGNORE,
    
    OP_BREAK,
    
    OP_ENTER,
    OP_LEAVE,
    OP_CALL,
    OP_PUSH,
    OP_POP,
    
    OP_CONST,
    OP_LOCAL,
    
    OP_JUMP,
    
    //-------------------
    
    OP_EQ,
    OP_NE,
    
    OP_LTI,
    OP_LEI,
    OP_GTI,
    OP_GEI,
    
    OP_LTU,
    OP_LEU,
    OP_GTU,
    OP_GEU,
    
    OP_EQF,
    OP_NEF,
    
    OP_LTF,
    OP_LEF,
    OP_GTF,
    OP_GEF,
    
    //-------------------
    
    OP_LOAD1,
    OP_LOAD2,
    OP_LOAD4,
    OP_STORE1,
    OP_STORE2,
    OP_STORE4,              // *(stack[top-1]) = stack[top]
    OP_ARG,
    
    OP_BLOCK_COPY,
    
    //-------------------
    
    OP_SEX8,
    OP_SEX16,
    
    OP_NEGI,
    OP_ADD,
    OP_SUB,
    OP_DIVI,
    OP_DIVU,
    OP_MODI,
    OP_MODU,
    OP_MULI,
    OP_MULU,
    
    OP_BAND,
    OP_BOR,
    OP_BXOR,
    OP_BCOM,
    
    OP_LSH,
    OP_RSHI,
    OP_RSHU,
    
    OP_NEGF,
    OP_ADDF,
    OP_SUBF,
    OP_DIVF,
    OP_MULF,
    
    OP_CVIF,
    OP_CVFI
} opcode_t;

typedef intptr_t vmptr_t;

typedef struct vmSymbol_s
{
    struct vmSymbol_s*   next;
    int symValue;
    int profileCount;
    char symName[1];        // variable sized
} vmSymbol_t;

#define VM_OFFSET_PROGRAM_STACK     0
#define VM_OFFSET_SYSTEM_CALL       sizeof(intptr_t)

struct vm_s
{
    // DO NOT MOVE OR CHANGE THESE WITHOUT CHANGING THE VM_OFFSET_* DEFINES
    // USED BY THE ASM CODE
    intptr_t programStack;               // the vm may be recursively entered
    intptr_t ( *systemCall )( intptr_t* parms );
    
    //------------------------------------
    
    char name[MAX_QPATH];
    
    // for dynamic linked modules
    void*        dllHandle;
    intptr_t ( * entryPoint )( intptr_t callNum, ... );
    
    // for interpreted modules
    bool currentlyInterpreting;
    
    bool compiled;
    byte*        codeBase;
    int codeLength;
    
    int*         instructionPointers;
    int instructionPointersLength;
    
    byte*        dataBase;
    int dataMask;
    
    int stackBottom;                // if programStack < stackBottom, error
    
    int numSymbols;
    struct vmSymbol_s*   symbols;
    
    int callLevel;                  // for debug indenting
    int breakFunction;              // increment breakCount on function entry to this
    int breakCount;
};


extern vm_t*    currentVM;
extern int vm_debugLevel;

void VM_Compile( vm_t* vm, vmHeader_t* header );
intptr_t VM_CallCompiled( vm_t* vm, intptr_t* args );

void VM_PrepareInterpreter( vm_t* vm, vmHeader_t* header );
intptr_t VM_CallInterpreted( vm_t* vm, intptr_t* args );

vmSymbol_t* VM_ValueToFunctionSymbol( vm_t* vm, int value );
int VM_SymbolToValue( vm_t* vm, const char* symbol );
const char* VM_ValueToSymbol( vm_t* vm, int value );
void VM_LogSyscalls( intptr_t* args );

#endif // !__VM_LOCAL_H__