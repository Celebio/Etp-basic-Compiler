
#include "PileRegTemp.h"


PileRegTemp::PileRegTemp(){
}
PileRegTemp::~PileRegTemp(){
}
PileRegTemp::PileRegTemp(IL* bIL,VirtStack* bStack){
    mTempCtr=0;
    mIL=bIL;
    mStack=bStack;
}

void PileRegTemp::depiler(void){
    int i;
    for (i=0;i<TAILLE_PILE-1;i++){
        Pile[i]=Pile[i+1];
    }
    Pile[TAILLE_PILE-1]=-2;
}
void PileRegTemp::Empiler(reg_id R){
    int i;
    for (i=TAILLE_PILE;i>0;i--){
        Pile[i]=Pile[i-1];
    }
    Pile[0]=R;
}


void PileRegTemp::init(int nbReg){
    int i;
    for(i=0;i<nbReg;i++){
        Pile[i]=i;
    }
    for(;i<TAILLE_PILE;i++){
        Pile[i]=-2;
    }
}
void PileRegTemp::Echange(void){
    reg_id aux=Pile[0];
    Pile[0]=Pile[1];
    Pile[1]=aux;
}

Operande* PileRegTemp::front(){
    return mIL->createOp(Pile[0]);
}

void PileRegTemp::allocate(Operande* M){
    if (M->nat==OP_DIRECT){
        depiler();
    }
    else if (M->nat==OP_INDIRECT){
        if (M->val.indirect.baseRegister!=SP_REG){
            depiler();
        }
    }
    else if (M->nat==OP_INDEXE){
        depiler();
    }
    else{
    }
}
void PileRegTemp::freeOperand(Operande* M){
    if (M->nat==OP_DIRECT){
        Empiler(M->val.directRegister);
    }
    else if (M->nat==OP_INDIRECT){
        if (M->val.indirect.baseRegister!=SP_REG){
            Empiler(M->val.indirect.baseRegister);
        }
    }
    else if (M->nat==OP_INDEXE){
        Empiler(M->val.indexed.indexRegister);
    }
    else{
    }
}

Operande* PileRegTemp::allocateTemp(int size){
    Operande* retVal;
    VariableItem* StackVar=new VariableItem();
    TAG *tagNomVar=new TAG();
    char buffer[200];
    sprintf(buffer,"_systVar%i",mTempCtr++);
    tagNomVar->SetIdentif(buffer);
    StackVar->SetTagNom(tagNomVar);
    StackVar->SetSize(size);
    retVal=mIL->createOp(0,SP_REG);
    mIL->add("Creation de temporaire");
    mIL->add(SUB,mIL->createOpVal(size),mIL->createOp(SP_REG),SZ_L);
    mStack->pushToStack(StackVar);
    return retVal;
}
void PileRegTemp::freeTemp(Operande* T,int size){
    VariableItem* AEffacer;
    AEffacer=mStack->pop();
    mIL->add("Liberation de temporaire");
    mIL->add(ADD,mIL->createOpVal(size),mIL->createOp(SP_REG),SZ_L);

    delete AEffacer->GetTagNom();
}

