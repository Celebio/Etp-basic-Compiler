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
#ifndef _FONCTIONITEM_H_1
#define _FONCTIONITEM_H_1

#include "collection.h"
#include "VarTypeType.h"
#include "asm68kCoder.h"

class FonctionItem :
	public ColItem
{
private:
	const char* Nom;
	int Ligne;
	Collection ArguListe;	// liste des arguments
	Collection VarListe;	// liste des variables locales
	VarTypeType RetType;
	Collection InstrListe;	// la suite d'instruction
	bool IsAssembler;
	bool Used;
	asm68kCoder* asmCode;

public:
	FonctionItem(void);
	~FonctionItem(void);


	void Detruir();
	void Afficher();

	// Get
	const char* GetNom() {return Nom;}
	int GetLigne() {return Ligne;}
	Collection* GetArguListe() {return &ArguListe;}
	Collection* GetVarListe() {return &VarListe;}
	VarTypeType GetRetType() {return RetType;}
	Collection* GetInstrListe() {return &InstrListe;}
	bool GetUsed() {return Used;}
	bool GetIsAssembler() {return IsAssembler;}
	asm68kCoder* GetasmCode() {return asmCode;}

	// Set
	void SetLigne(int bLigne){Ligne=bLigne;}
	void SetRetType(VarTypeType bType){RetType=bType;}
	void SetNom(const char* bNom){Nom=bNom;}
	void SetUsed(){Used=true;}
	void SetIsAssembler(){IsAssembler=true;}
	void SetasmCode(asm68kCoder* bCode){asmCode=bCode;}

};

#endif


