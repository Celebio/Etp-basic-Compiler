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


#include "VirtStack68k.h"

VirtStack68k::VirtStack68k(){
    VirtualStackBuffer=NULL;
    mTempCtr=0;
}
VirtStack68k::~VirtStack68k(){
}

void VirtStack68k::PushToStack(VariableItem* bVar){
    VirtStackElem68k* aux=new VirtStackElem68k;
    VirtStackElem68k* pCour;
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
void VirtStack68k::PushToStack(int taille){
    VariableItem* StackVar=new VariableItem();
    TAG *tagNomVar=new TAG();
    char* buffer= new char[200];
    sprintf(buffer,"_systArgVar%i",mTempCtr++);
    tagNomVar->SetIdentif(buffer);
    StackVar->SetTagNom(tagNomVar);
    StackVar->SetSize(taille);
    PushToStack(StackVar);
}
VariableItem* VirtStack68k::Pop(){
    VariableItem* Topest;
    Topest=VirtualStackBuffer->var;
    VirtStackElem68k* AEffacer=VirtualStackBuffer;
    VirtualStackBuffer= VirtualStackBuffer->next;
    delete AEffacer;

    // mise ? jour du champs deepth
    //--------------------------------------------
    VirtStackElem68k* pCour=VirtualStackBuffer;
    int prevSize=0;
    pCour=VirtualStackBuffer;
    while(pCour)
    {
        pCour->deepth=prevSize;
        prevSize+=pCour->var->GetSize();
        pCour=pCour->next;
    }
    //---------------------------------------------
    return Topest;
}

void VirtStack68k::ClearStack(){

    VirtStackElem68k* pCour=VirtualStackBuffer;
    VirtStackElem68k* precCour=pCour;
    while (pCour){
        precCour=pCour;
        pCour=pCour->next;
        delete precCour;
    }
    VirtualStackBuffer=NULL;
}
int VirtStack68k::GetStackPos(char* VarNom)
{
    VirtStackElem68k* pCour=VirtualStackBuffer;
    while(pCour)
    {
        if (!strcmp(VarNom,pCour->var->GetTagNom()->GetIdentif()))
            return pCour->deepth;
        pCour=pCour->next;
    }
    return -77777;  // magic number pour dire qu'il n'a pas trouv? alors qu'il est cens?
                    // le trouver car les v?rifications sont faites dans verisem
}
void VirtStack68k::display(){
    VirtStackElem68k* pCour=VirtualStackBuffer;
    printf("Virtual Stack State:\n");
    while(pCour)
    {
        printf("deepth:%i  \t var_name:%s \t\tde taille:%i\n",pCour->deepth,pCour->var->GetTagNom()->GetIdentif(),pCour->var->GetSize());
        pCour=pCour->next;
    }
    //VirtualStackBuffer=NULL;
}

