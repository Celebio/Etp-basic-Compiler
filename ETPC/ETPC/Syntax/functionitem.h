/*
    This file is part of ETP-Basic Compiler, a cross compiler for TI by Onur Celebi 2005-2007.

    ETP-Basic Compiler is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ETP-Basic Compiler is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _FUNCTIONITEM_H_1
#define _FUNCTIONITEM_H_1

#include "collection.h"
#include "VarTypeType.h"
#include "asm68kCoder.h"

class FunctionItem :
    public ColItem
{
private:
    const char* name;
    int line;
    Collection argumentList;
    Collection variableList;    // liste des variables locales
    VarTypeType returnType;
    Collection instructionList;  // la suite d'instruction
    bool assembler;
    bool used;
    asm68kCoder* asmCode;

public:
    FunctionItem(void);
    ~FunctionItem(void);


    void destroy();
    void display();

    // Get
    const char* getName() {return name;}
    int getLine() {return line;}
    Collection* getArgumentList() {return &argumentList;}
    Collection* getVariableList() {return &variableList;}
    VarTypeType getReturnType() {return returnType;}
    Collection* getInstructionList() {return &instructionList;}
    bool isUsed() {return used;}
    bool isAssembler() {return assembler;}
    asm68kCoder* getAsmCode() {return asmCode;}

    // Set
    void setLine(int bLigne){line=bLigne;}
    void setReturnType(VarTypeType bType){returnType=bType;}
    void setName(const char* bNom){name=bNom;}
    void setUsed(){used=true;}
    void setIsAssembler(){assembler=true;}
    void setAsmCode(asm68kCoder* bCode){asmCode=bCode;}

};

#endif


