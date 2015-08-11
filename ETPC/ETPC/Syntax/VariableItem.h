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
#ifndef _VARIABLEITEM_H_1
#define _VARIABLEITEM_H_1


#include "collection.h"
#include "VarTypeType.h"
#include "TAG.h"

class FunctionItem;

typedef enum VarPortEnum {
    VR_PUBLIC,
    VR_LOCAL,
    VR_MEMBER,  // pour les variables d'un type
    VR_ARGU     // argument d'une fonction ou proc?dure
} VarPortEnum;


class VariableItem:
    public ColItem
{
private:
    VarPortEnum locality;    // LOCAL/PUBLIC
    TAG* bTagName;           // le TAG dont le champ Identif contient le nom de la variable
    Collection DimListe;    // Liste des expressions des dimensions
    VarTypeType Type;   // Integer,Float,User-defined,etc..
    TAG* bTAGType;      // le TAG dont le champ Identif donne d?finit le type
    FunctionItem* Func;     // La fonction qui la contient (pour les variables locales)
    int size;           // size en octet
    bool Pointed;
public:
    // CONSTRUCTEUR/DESTRUCTEUR
    VariableItem(void);
    ~VariableItem(void);

    // reimplemantation des fonctions:
    void destroy();
    void display();

    // Get
    VarPortEnum GetLocality() {return locality;}
    TAG* getTagName() {return bTagName;}
    Collection* getDimList() {return &DimListe;}
    VarTypeType getType() {return Type;}
    TAG* getTagType() {return bTAGType;}
    FunctionItem* GetFunc() {return Func;}
    int getSize() {return size;}
    bool GetPointed() {return Pointed;}

    // Set
    void setLocality(VarPortEnum bCar) {locality=bCar;}
    void setTagName(TAG* nTAGNom) {bTagName=nTAGNom;}
    void setDimListe(Collection* bListe) {if (bListe) DimListe=*bListe;}
    void setType(VarTypeType bType) {Type=bType;}
    void setTagType(TAG* nTAGType) {bTAGType=nTAGType;}
    void SetFunc(FunctionItem* bFunc) {Func=bFunc;}
    void SetSize(int bSize) {size=bSize;}
    void SetPointed(bool ptd) {Pointed=ptd;}
};

#endif
