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


#include "VirtStackz80.h"

VirtStackz80::VirtStackz80(){
	VirtualStackBuffer=NULL;
	mTempCtr=0;
}
VirtStackz80::~VirtStackz80(){
}

void VirtStackz80::PushToStack(VariableItem* bVar){
	VirtStackElemz80* aux=new VirtStackElemz80;
	VirtStackElemz80* pCour;
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
void VirtStackz80::PushToStack(int taille){
	VariableItem* StackVar=new VariableItem();
	TAG *tagNomVar=new TAG();
	char* buffer= new char[200];
	sprintf(buffer,"_systArgVar%i",mTempCtr++);
	tagNomVar->SetIdentif(buffer);
	StackVar->SetTagNom(tagNomVar);
	StackVar->SetSize(taille);
	PushToStack(StackVar);
}
VariableItem* VirtStackz80::Pop(){
	VariableItem* Topest;
	Topest=VirtualStackBuffer->var;
	VirtStackElemz80* AEffacer=VirtualStackBuffer;
	VirtualStackBuffer= VirtualStackBuffer->next;
	delete AEffacer;

	// mise � jour du champs deepth
	//--------------------------------------------
	VirtStackElemz80* pCour=VirtualStackBuffer;
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

void VirtStackz80::ClearStack(){
	
	VirtStackElemz80* pCour=VirtualStackBuffer;
	VirtStackElemz80* precCour=pCour;
	while (pCour){
		precCour=pCour;
		pCour=pCour->next;
		delete precCour;
	}
	VirtualStackBuffer=NULL;
}
int VirtStackz80::GetStackPos(char* VarNom)
{
	VirtStackElemz80* pCour=VirtualStackBuffer;
	while(pCour)
	{
		if (!strcmp(VarNom,pCour->var->GetTagNom()->GetIdentif()))
			return pCour->deepth;
		pCour=pCour->next;
	}
	return -77777;	// magic number pour dire qu'il n'a pas trouv� alors qu'il est cens�
					// le trouver car les v�rifications sont faites dans verisem
}
void VirtStackz80::Afficher(){
	VirtStackElemz80* pCour=VirtualStackBuffer;
	printf("Virtual Stack State:\n");
	while(pCour)
	{
		printf("deepth:%i  \t var_name:%s \t\tde taille:%i\n",pCour->deepth,pCour->var->GetTagNom()->GetIdentif(),pCour->var->GetSize());
		pCour=pCour->next;
	}
	//VirtualStackBuffer=NULL;
}

