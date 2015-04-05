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

#ifndef _COLLECTION_H_1
#define _COLLECTION_H_1

//#include "tokenizer.h"
//#include "structures.h"
//#include "Noeud.h"
#include <stdio.h>
#include "TAG.h"
#include "ColIterator.h"
/*
Types->bindIterator(&iter1);
while(iter1.elemExists()){
        Type1 = (TypeItem*)iter1.getNext();

}

*/
//Types->bindIterator(&iter1);
//for (Type1 = (TypeItem*)iter1.getNext();iter1.elemExists();Type1 = (TypeItem*)iter1.getNext())

// for_each(ColItem*, ColItemDerivedType, ColIterator*, Collection*)
/*
#define for_each(itemName, itemType, iteratorName, collectionName)          \
        collectionName->bindIterator(&iteratorName);                        \
        for (itemName = (itemType*)iteratorName.getElem();iteratorName.elemExists();itemName = (itemType*)iteratorName.getNext())                   \
*/



/* DEFINITION DE LA CLASSE COLLECTION GENERIQUE
***********************************************************************/

class Collection:
    public ColItem
{

private:
    ArrayElem *buffer;
    ArrayElem *last;
    ArrayElem *iter1;
    ArrayElem *iter2;

    void SetBuffer(ArrayElem* tBuff);
public:
    // constructeurs
    Collection();
    ~Collection();

    //fonctions
    ColItem* Add(ColItem* bElem);
    ColItem* AddDebut(ColItem* bElem);
    void Add(const char* bDesc,TAG* bTag);
    void Clear();
    void Afficher();
    bool estVide();

    // iterators:
    void bindIterator(ColIterator* bIter);

};




#endif
