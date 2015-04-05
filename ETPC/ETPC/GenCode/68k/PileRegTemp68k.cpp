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

#include "PileRegTemp68k.h"


PileRegTemp68k::PileRegTemp68k(){
}
PileRegTemp68k::~PileRegTemp68k(){
}
PileRegTemp68k::PileRegTemp68k(asm68kCoder* bIL,VirtStack68k* bStack){
    mTempCtr=0;
    mIL=bIL;
    mStack=bStack;
}

void PileRegTemp68k::DepilerD(void){
    int i;
    for (i=0;i<TAILLE_PILED-1;i++){
        PileD[i]=PileD[i+1];
    }
    PileD[TAILLE_PILED-1]=UNDEFREG;
}
void PileRegTemp68k::EmpilerD(reg_id R){
    int i;
    for (i=TAILLE_PILED;i>0;i--){
        PileD[i]=PileD[i-1];
    }
    PileD[0]=R;
}


void PileRegTemp68k::Init(){
    int i;
    for(i=0;i<TAILLE_PILED;i++){
        PileD[i]=(reg_id)(i+D1);
    }
    for(;i<TAILLE_PILED;i++){
        PileD[i]=UNDEFREG;
    }
}
void PileRegTemp68k::EchangeD(void){
    reg_id aux=PileD[0];
    PileD[0]=PileD[1];
    PileD[1]=aux;
}

Operande68k* PileRegTemp68k::Sommet(){
    return mIL->createOp(PileD[0]);
}

void PileRegTemp68k::Allouer(Operande68k* M){
    if (M->nat==OP_DIRECT){
        DepilerD();
    }
    else if (M->nat==OP_INDIRECT){
        if (M->val.Indirect.RegBase!=SP_REG){
            DepilerD();
        }
    }
    else if (M->nat==OP_INDEXE){
        DepilerD();
    }
    else{
    }
}
void PileRegTemp68k::Liberer(Operande68k* M){
    if (M->nat==OP_DIRECT){
        EmpilerD(M->val.RegDirect);
    }
    else if (M->nat==OP_INDIRECT){
        if (M->val.Indirect.RegBase!=SP_REG){
            EmpilerD(M->val.Indirect.RegBase);
        }
    }
    else if (M->nat==OP_INDEXE){
        EmpilerD(M->val.Indexe.RegIndexe);
    }
    else{
    }
}

Operande68k* PileRegTemp68k::AllouerTemp(int taille){
    Operande68k* retVal;
    VariableItem* StackVar=new VariableItem();
    TAG *tagNomVar=new TAG();
    char buffer[200];
    sprintf(buffer,"_systVar%i",mTempCtr++);
    tagNomVar->SetIdentif(buffer);
    StackVar->SetTagNom(tagNomVar);
    StackVar->SetSize(taille);
    retVal=mIL->createOp(0,SP_REG);
    mIL->Add("Creation de temporaire");
    mIL->Add(SUB,mIL->createOpVal(taille),mIL->createOp(SP_REG),SZ_L);
    mStack->PushToStack(StackVar);
    return retVal;
}
void PileRegTemp68k::LibererTemp(Operande68k* T,int taille){
    VariableItem* AEffacer;
    AEffacer=mStack->Pop();
    mIL->Add("Liberation de temporaire");
    mIL->Add(ADD,mIL->createOpVal(taille),mIL->createOp(SP_REG),SZ_L);

    delete AEffacer->GetTagNom();
}

