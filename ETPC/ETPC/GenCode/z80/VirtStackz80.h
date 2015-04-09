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
#ifndef _VIRTSTACKZ80_H_1
#define _VIRTSTACKZ80_H_1

#include "VariableItem.h"



typedef enum regz80_id {A, B, C, D, E, F, H, L,IX ,UNDEFZ80REG} regz80_id;

#define SP IX

struct VirtStackElemz80{
    int depth;
    VariableItem* var;
    VirtStackElemz80* next;
};

class VirtStackz80{
private:
    VirtStackElemz80* VirtualStackBuffer;
    int mTempCtr;
public:
    VirtStackz80(void);
    ~VirtStackz80(void);

    void pushToStack(VariableItem* bVar);
    void pushToStack(int size);
    VariableItem* pop();

    void clearStack();

    int getStackPos(char* varName);
    void display();
};


#endif

