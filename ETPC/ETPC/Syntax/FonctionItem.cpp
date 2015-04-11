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

#include "FonctionItem.h"
#include <stdio.h>

FonctionItem::FonctionItem(void)
{
    assembler=false;
    used=false;
}
FonctionItem::~FonctionItem(void)
{
}
void FonctionItem::destroy()
{
}
void FonctionItem::display()
{
    printf("Fonction:line:%i  nom:%s, arguments:\n",line,name);
    printf("Arguments:\n");
    argumentList.display();
    printf("Variables locales:\n");
    variableList.display();

    if (returnType.Type != TP_VOID)
    {
        printf("retourne ");
        returnType.display();
    }
    printf("\n");
    printf("Instructions:\n");
    instructionList.display();
}

