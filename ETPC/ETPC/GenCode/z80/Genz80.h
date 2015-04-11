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
#ifndef _GENz80_H_1
#define _GENz80_H_1

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <fstream>
#include <string>
#include <iostream>

#include "asmz80Coder.h"
//#include "PileRegTempz80.h"
#include "FonctionItem.h"
#include "InstructionETPB.h"


typedef enum NatureOpz80{NOz80_REG,NOz80_DVAL,NOz80_DADR}NatureOpz80;



class Genz80{
private:
    Collection* errListe;
    Collection* VariablesPublic;
    Collection* Types;
    Collection* Fonctions;

    char outputFileName[300];
    asmz80Coder ilCoder;
    //VirtStackz80 virtualStack;
    //PileRegTempz80* registerStack;

    int mNbRegMax;

    //size_opz80 getSize(CNoeud* bNoeud);

    int getNbRegArith(CNoeud* bNoeud,NatureOpz80 bNat);
    int getNbRegObject(CNoeud* bNoeud,NatureOpz80 bNat);

//  void codeArith(CNoeud* bNoeud,NatureOp bNat,Operandez80** opertr);
//  void codeObject(CNoeud* bNoeud,NatureOp bNat,Operandez80** opertr);

public:
    Genz80(void);
    Genz80(const char *oFileName);
    ~Genz80(void);

    void testGenerate();
    void generateCode();

    void setEnvironnement(Collection* BerrListe,
                            Collection* BVariablesPublic,
                            Collection* BTypes,
                            Collection* BFonctions);
};

#endif

