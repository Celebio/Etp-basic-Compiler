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
#ifndef _GEN68K_H_1
#define _GEN68K_H_1

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <fstream>
#include <string>
#include <iostream>

#include "asm68kCoder.h"
#include "FonctionItem.h"
#include "InstructionETPB.h"
#include "PileRegTemp68k.h"


typedef enum NatureOp { NO_REG, NO_DVAL, NO_DADR } NatureOp;



class Gen68k{
private:
    Collection* errListe;
    Collection* VariablesPublic;
    Collection* Types;
    Collection* Fonctions;

    char outputFileName[300];

    asm68kCoder ilCoder;
    VirtStack68k Stack;
    PileRegTemp68k* registerStack;

    int mNbRegMax;

    size_op68k GetSize(CNoeud* bNoeud);

    int NbRegArith(CNoeud* bNoeud,NatureOp bNat);
    int NbRegObjet(CNoeud* bNoeud,NatureOp bNat);
    int NbRegBool(CNoeud* bNoeud);



    void CodeArith(CNoeud* bNoeud,NatureOp bNat,Operande68k** opertr);
    void CodeObjet(CNoeud* bNoeud,NatureOp bNat,Operande68k** opertr);
    void CodeBool(CNoeud* bNoeud,bool AvecSaut,bool ValSaut,Operande68k* etiqSaut,bool DansReg,bool ValReg);

    void CodeInstr(InstructionETPB* bInstr);
    void CodeInstr(Collection* bInstrSuite);

public:
    Gen68k(void);
    Gen68k(const char *oFileName);
    ~Gen68k(void);

    void TestGenerate();
    void GenerCode();

    void SetEnvironnement(Collection* BerrListe,
                            Collection* BVariablesPublic,
                            Collection* BTypes,
                            Collection* BFonctions);

};

#endif

