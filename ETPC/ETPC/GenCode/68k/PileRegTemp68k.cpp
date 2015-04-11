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

void PileRegTemp68k::popD(void){
    int i;
    for (i=0;i<D_STACK_SIZE-1;i++){
        stackForD[i]=stackForD[i+1];
    }
    stackForD[D_STACK_SIZE-1]=UNDEFREG;
}
void PileRegTemp68k::pushD(reg_id R){
    int i;
    for (i=D_STACK_SIZE;i>0;i--){
        stackForD[i]=stackForD[i-1];
    }
    stackForD[0]=R;
}


void PileRegTemp68k::init(){
    int i;
    for(i=0;i<D_STACK_SIZE;i++){
        stackForD[i]=(reg_id)(i+D1);
    }
    for(;i<D_STACK_SIZE;i++){
        stackForD[i]=UNDEFREG;
    }
}
void PileRegTemp68k::switchD(void){
    reg_id aux=stackForD[0];
    stackForD[0]=stackForD[1];
    stackForD[1]=aux;
}

Operande68k* PileRegTemp68k::front(){
    return mIL->createOp(stackForD[0]);
}

void PileRegTemp68k::allocate(Operande68k* M){
    if (M->nat==OP_DIRECT){
        popD();
    }
    else if (M->nat==OP_INDIRECT){
        if (M->val.indirect.baseRegister!=SP_REG){
            popD();
        }
    }
    else if (M->nat==OP_INDEXE){
        popD();
    }
    else{
    }
}
void PileRegTemp68k::freeOperand(Operande68k* M){
    if (M->nat==OP_DIRECT){
        pushD(M->val.directRegister);
    }
    else if (M->nat==OP_INDIRECT){
        if (M->val.indirect.baseRegister!=SP_REG){
            pushD(M->val.indirect.baseRegister);
        }
    }
    else if (M->nat==OP_INDEXE){
        pushD(M->val.indexed.indexRegister);
    }
    else{
    }
}

Operande68k* PileRegTemp68k::allocateTemp(int size){
    Operande68k* retVal;
    VariableItem* StackVar=new VariableItem();
    TAG *tagNomVar=new TAG();
    char buffer[200];
    sprintf(buffer,"_systVar%i",mTempCtr++);
    tagNomVar->SetIdentif(buffer);
    StackVar->setTagName(tagNomVar);
    StackVar->SetSize(size);
    retVal=mIL->createOp(0,SP_REG);
    mIL->add("Creation de temporaire");
    mIL->add(SUB,mIL->createOpVal(size),mIL->createOp(SP_REG),SZ_L);
    mStack->pushToStack(StackVar);
    return retVal;
}
void PileRegTemp68k::freeTemp(Operande68k* T,int size){
    VariableItem* AEffacer;
    AEffacer=mStack->pop();
    mIL->add("Liberation de temporaire");
    mIL->add(ADD,mIL->createOpVal(size),mIL->createOp(SP_REG),SZ_L);

    delete AEffacer->getTagName();
}

