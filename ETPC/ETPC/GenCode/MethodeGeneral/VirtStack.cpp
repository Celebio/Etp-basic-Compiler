

#include "VirtStack.h"

VirtStack::VirtStack(){
    VirtualStackBuffer=NULL;
}
VirtStack::~VirtStack(){
}

void VirtStack::pushToStack(VariableItem* bVar){
    VirtStackElem* aux=new VirtStackElem;
    VirtStackElem* pCour;
    int prevSize,exSize;
    aux->depth=0;
    aux->var=bVar;
    aux->next=VirtualStackBuffer;
    VirtualStackBuffer=aux;
    prevSize=aux->var->getSize();
    pCour=aux->next;
    while(pCour)
    {
        pCour->depth+=prevSize;
        exSize=pCour->var->getSize();
        pCour=pCour->next;
    }
}
VariableItem* VirtStack::pop(){
    VariableItem* Topest;
    Topest=VirtualStackBuffer->var;
    VirtStackElem* AEffacer=VirtualStackBuffer;
    VirtualStackBuffer= VirtualStackBuffer->next;
    delete AEffacer;

    return Topest;
}

void VirtStack::clearStack(){

    VirtStackElem* pCour=VirtualStackBuffer;
    VirtStackElem* precCur=pCour;
    while (pCour){
        precCur=pCour;
        pCour=pCour->next;
        delete precCur;
    }
    VirtualStackBuffer=NULL;
}
int VirtStack::getStackPos(char* varName)
{
    VirtStackElem* pCour=VirtualStackBuffer;
    while(pCour)
    {
        if (!strcmp(varName,pCour->var->getTagName()->GetIdentif()))
            return pCour->depth;
        pCour=pCour->next;
    }
    return -77777;  // magic number pour dire qu'il n'a pas trouv? alors qu'il est cens?
                    // le trouver car les v?rifications sont faites dans verisem
}
void VirtStack::display(){
    VirtStackElem* pCour=VirtualStackBuffer;
    printf("Virtual virtualStack State:\n");
    while(pCour)
    {
        printf("depth:%i  \t var_name:%s \t\tde size:%i\n",pCour->depth,pCour->var->getTagName()->GetIdentif(),pCour->var->getSize());
        pCour=pCour->next;
    }
    //VirtualStackBuffer=NULL;
}

