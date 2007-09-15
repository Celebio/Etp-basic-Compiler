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
#ifndef _DIMELEMITEM_H_1
#define _DIMELEMITEM_H_1

#include "noeud.h"
#include "ColItem.h"

class DimElemItem :
	public ColItem
{
private:
	int ConstVal;
	CNoeud* expr;
public:

	DimElemItem(void);
	~DimElemItem(void);

	// les obligés
	void Detruir();
	void Afficher();

	int GetConstVal(){return ConstVal;}
	CNoeud* GetExpr(){return expr;}
	CNoeud** GetExprPtr(){return &expr;}

	void SetExprNull(){expr=NULL;}
};

#endif
