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

class FonctionItem;

typedef enum VarPortEnum {
	VR_PUBLIC,
	VR_LOCAL,
	VR_MEMBER,	// pour les variables d'un type
	VR_ARGU		// argument d'une fonction ou procédure
};


class VariableItem:
	public ColItem
{
private:
	VarPortEnum Car;	// LOCAL/PUBLIC
	TAG* bTAGNom;			// le TAG dont le champ Identif contient le nom de la variable
	Collection DimListe;	// Liste des expressions des dimensions
	VarTypeType Type;	// Integer,Float,User-defined,etc..
	TAG* bTAGType;		// le TAG dont le champ Identif donne définit le type
	FonctionItem* Func;		// La fonction qui la contient (pour les variables locales)
	int Size;			// taille en octet
	bool Pointed;
public:
	// CONSTRUCTEUR/DESTRUCTEUR
	VariableItem(void);
	~VariableItem(void);

	// reimplemantation des fonctions:
	void Detruir();
	void Afficher();

	// Get
	VarPortEnum GetCar() {return Car;}
	TAG* GetTagNom() {return bTAGNom;}
	Collection* GetDimListe() {return &DimListe;}
	VarTypeType GetType() {return Type;}
	TAG* GetTagType() {return bTAGType;}
	FonctionItem* GetFunc() {return Func;}
	int GetSize() {return Size;}
	bool GetPointed() {return Pointed;}

	// Set
	void SetCar(VarPortEnum bCar) {Car=bCar;}
	void SetTagNom(TAG* nTAGNom) {bTAGNom=nTAGNom;}
	void SetDimListe(Collection* bListe) {if (bListe) DimListe=*bListe;}
	void SetType(VarTypeType bType) {Type=bType;}
	void SetTagType(TAG* nTAGType) {bTAGType=nTAGType;}
	void SetFunc(FonctionItem* bFunc) {Func=bFunc;}
	void SetSize(int bSize) {Size=bSize;}
	void SetPointed(bool ptd) {Pointed=ptd;}
};

#endif
