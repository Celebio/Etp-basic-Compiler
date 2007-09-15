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

#include "collection.h"
#include "errorC.h"
#include "string.h"

/* CONSTRUCTEURS, DESTRUCTEURS...
***********************************************************************/
Collection::Collection(){
	buffer=NULL;
}
Collection::~Collection(){
	Clear();
}

void Collection::SetBuffer(ArrayElem *tBuff){
	buffer=tBuff;
	// mise a jour de last
	last=buffer;
	if (last)
	{
		while (last->next)
		{
			last = last->next;
		}
	}
}

bool Collection::estVide(){
	if (buffer==NULL)
		return true;
	return false;
}

/* LIBERATION DE LA MEMOIRE
***********************************************************************/


void Collection::Clear(){
	ArrayElem *Cour=buffer;
	ArrayElem *precCour=Cour;
	while (Cour)
	{
		precCour=Cour;
		Cour=Cour->next;
		precCour->Elem->Detruir();
		delete precCour;
	}
	buffer=NULL;
	last = NULL;
	return;
}


/* MANIPULATION DE LA COLLECTION
***********************************************************************/
ColItem* Collection::Add(ColItem* bElem)
{
	if (!buffer)
	{
		buffer= new ArrayElem();
		buffer->next = NULL;
		buffer->Elem = bElem;
		last = buffer;
	}
	else
	{
		last->next = new ArrayElem();
		last = last->next;
		last->Elem = bElem;
		last->next = NULL;
	}
	return bElem;
}
ColItem* Collection::AddDebut(ColItem* bElem)
// pareil que Add, mais ajoute en debut de liste
// sert uniquement pour avoir une liste dans l'autre sens pour les arguments d'une fonction
// ne pas utiliser AddDebut et Add pour une meme instance de Collection, last n'est pas mis à jour
{
	if (!buffer)
	{
		buffer= new ArrayElem();
		buffer->next = NULL;
		buffer->Elem = bElem;
		last = buffer;
	}
	else
	{
		ArrayElem* aux = new ArrayElem();
		aux->Elem = bElem;
		aux->next = buffer;
		buffer = aux;
	}
	return bElem;
}
void Collection::Add(char* bDesc,TAG* bTag)
{
	errorC *bErr=new errorC();
	strcpy(bErr->Descr,bDesc);
	bErr->Tag = bTag;
	Add(bErr);
}
void Collection::Afficher()
{
	ArrayElem *Cour=buffer;
	while (Cour)
	{
		Cour->Elem->Afficher();
		Cour=Cour->next;
	}
}


/* ITERATEURS
***********************************************************************/
void Collection::bindIterator(ColIterator* bIter){
	bIter->initHead(buffer);
}
