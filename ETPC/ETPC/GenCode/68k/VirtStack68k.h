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
#ifndef _VIRTSTACK_H_1
#define _VIRTSTACK_H_1

#include "VariableItem.h"



typedef enum reg_id {A0,A1,A2,A3,A4,A5,A6,A7, D0,D1,D2,D3,D4,D5,D6,D7 ,UNDEFREG} reg_id;

#define SP_REG A7

struct VirtStackElem68k{
    int deepth;
    VariableItem* var;
    VirtStackElem68k* next;
};

class VirtStack68k{
private:
    VirtStackElem68k* VirtualStackBuffer;
    int mTempCtr;
public:
    VirtStack68k(void);
    ~VirtStack68k(void);

    void PushToStack(VariableItem* bVar);
    void PushToStack(int taille);
    VariableItem* Pop();

    void ClearStack();

    int GetStackPos(char* VarNom);
    void Afficher();
};


#endif
