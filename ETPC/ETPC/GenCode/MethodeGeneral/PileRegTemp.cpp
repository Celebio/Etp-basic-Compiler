
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

void PileRegTemp::Depiler(void){
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


void PileRegTemp::Init(int nbReg){
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

Operande* PileRegTemp::Sommet(){
    return mIL->createOp(Pile[0]);
}

void PileRegTemp::Allouer(Operande* M){
    if (M->nat==OP_DIRECT){
        Depiler();
    }
    else if (M->nat==OP_INDIRECT){
        if (M->val.Indirect.RegBase!=SP_REG){
            Depiler();
        }
    }
    else if (M->nat==OP_INDEXE){
        Depiler();
    }
    else{
    }
}
void PileRegTemp::Liberer(Operande* M){
    if (M->nat==OP_DIRECT){
        Empiler(M->val.RegDirect);
    }
    else if (M->nat==OP_INDIRECT){
        if (M->val.Indirect.RegBase!=SP_REG){
            Empiler(M->val.Indirect.RegBase);
        }
    }
    else if (M->nat==OP_INDEXE){
        Empiler(M->val.Indexe.RegIndexe);
    }
    else{
    }
}

Operande* PileRegTemp::AllouerTemp(int taille){
    Operande* retVal;
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
void PileRegTemp::LibererTemp(Operande* T,int taille){
    VariableItem* AEffacer;
    AEffacer=mStack->Pop();
    mIL->Add("Liberation de temporaire");
    mIL->Add(ADD,mIL->createOpVal(taille),mIL->createOp(SP_REG),SZ_L);

    delete AEffacer->GetTagNom();
}

