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
#ifndef _VERISEM_H_1
#define _VERISEM_H_1

#include "VariableItem.h"
#include "collection.h"
#include "FonctionItem.h"
#include "DimElemItem.h"
#include "InstructionETPB.h"
#include "Tokenizer.h"

class VeriSem 
{
private:
	Collection* errListe;
	Collection* VariablesPublic;
	Collection* Types;
	Collection* Fonctions;

	int UpdateVarSize(VariableItem* bVar,VarTypeType bType,bool UDasPtr);
	bool TypeExiste(VariableItem* bVariable);

	VarTypeType GetTypeExpression(CNoeud *expr,
								VarTypeType typeAttendu,
								FonctionItem* foncEnCours);

	void VerifSemInstr(Collection *bInstrListe,FonctionItem* foncEnCours);

	void VerifSemInstr(InstructionETPB *bInstr,
				   FonctionItem* foncEnCours);
public:
	VeriSem(void);
	~VeriSem(void);
	
	void VerifSem();
	void SetEnvironnement(Collection* BerrListe,
							Collection* BVariablesPublic,
							Collection* BTypes,
							Collection* BFonctions);
	
};

#endif
