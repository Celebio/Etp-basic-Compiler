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
#ifndef _PILEREGTEMP_H_1
#define _PILEREGTEMP_H_1

#include "asm68kCoder.h"
#define D_STACK_SIZE (D7-D1+1)
#define A_STACK_SIZE (A6-A0+1)

class TempRegStack68k{
private:
    reg_id stackForD[D_STACK_SIZE];
    reg_id stackForA[A_STACK_SIZE];

    void popD(void);
    void pushD(reg_id R);
    int nb_reg;
    asm68kCoder* mIL;
    VirtStack68k* mStack;
    int mTempCtr;
public:
    TempRegStack68k(void);
    TempRegStack68k(asm68kCoder* bIL,VirtStack68k* bStack);
    ~TempRegStack68k(void);

    void init();
    void switchD(void);

    Operande68k* front();

    void allocate(Operande68k* M);
    void freeOperand(Operande68k* M);

    Operande68k* allocateTemp(int size);
    void freeTemp(Operande68k* T,int size);
};

#endif

