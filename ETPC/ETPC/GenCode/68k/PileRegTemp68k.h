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
#define TAILLE_PILED (D7-D1+1)
#define TAILLE_PILEA (A6-A0+1)

class PileRegTemp68k{
private:
    reg_id PileD[TAILLE_PILED];
    reg_id PileA[TAILLE_PILEA];

    void DepilerD(void);
    void EmpilerD(reg_id R);
    int nb_reg;
    asm68kCoder* mIL;
    VirtStack68k* mStack;
    int mTempCtr;
public:
    PileRegTemp68k(void);
    PileRegTemp68k(asm68kCoder* bIL,VirtStack68k* bStack);
    ~PileRegTemp68k(void);

    void Init();
    void EchangeD(void);

    Operande68k* Sommet();

    void Allouer(Operande68k* M);
    void Liberer(Operande68k* M);

    Operande68k* AllouerTemp(int taille);
    void LibererTemp(Operande68k* T,int taille);
};

#endif

