

#include "VirtStack.h"

VirtStack::VirtStack(){
    VirtualStackBuffer=NULL;
}
VirtStack::~VirtStack(){
}

void VirtStack::PushToStack(VariableItem* bVar){
    VirtStackElem* aux=new VirtStackElem;
    VirtStackElem* pCour;
    int prevSize,exSize;
    aux->deepth=0;
    aux->var=bVar;
    aux->next=VirtualStackBuffer;
    VirtualStackBuffer=aux;
    prevSize=aux->var->GetSize();
    pCour=aux->next;
    while(pCour)
    {
        pCour->deepth+=prevSize;
        exSize=pCour->var->GetSize();
        pCour=pCour->next;
    }
}
VariableItem* VirtStack::Pop(){
    VariableItem* Topest;
    Topest=VirtualStackBuffer->var;
    VirtStackElem* AEffacer=VirtualStackBuffer;
    VirtualStackBuffer= VirtualStackBuffer->next;
    delete AEffacer;

    return Topest;
}

void VirtStack::ClearStack(){

    VirtStackElem* pCour=VirtualStackBuffer;
    VirtStackElem* precCour=pCour;
    while (pCour){
        precCour=pCour;
        pCour=pCour->next;
        delete precCour;
    }
    VirtualStackBuffer=NULL;
}
int VirtStack::GetStackPos(char* VarNom)
{
    VirtStackElem* pCour=VirtualStackBuffer;
    while(pCour)
    {
        if (!strcmp(VarNom,pCour->var->GetTagNom()->GetIdentif()))
            return pCour->deepth;
        pCour=pCour->next;
    }
    return -77777;  // magic number pour dire qu'il n'a pas trouv? alors qu'il est cens?
                    // le trouver car les v?rifications sont faites dans verisem
}
void VirtStack::display(){
    VirtStackElem* pCour=VirtualStackBuffer;
    printf("Virtual Stack State:\n");
    while(pCour)
    {
        printf("deepth:%i  \t var_name:%s \t\tde size:%i\n",pCour->deepth,pCour->var->GetTagNom()->GetIdentif(),pCour->var->GetSize());
        pCour=pCour->next;
    }
    //VirtualStackBuffer=NULL;
}

