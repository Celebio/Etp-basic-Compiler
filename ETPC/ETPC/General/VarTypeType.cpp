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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "VarTypeType.h"

VarTypeType::VarTypeType(void)
{
    Type=TP_UNKNOWN;
    Expr=NULL;
}
VarTypeType::VarTypeType(VarTypeEnum bTy)
{
    Type=bTy;
    Expr=NULL;
}


VarTypeType::~VarTypeType(void)
{
}

const char *VarTypeType::VarTypeTypeImage(){
    switch(Type)
    {
    case TP_INTEGER:
        return "Integer";
    case TP_FLOAT:
        return "Float";
    case TP_LONG:
        return "Long";
    case TP_STRING:
        return "String";
    case TP_BOOLEAN:
        return "Boolean";
    case TP_FILE:
        return "File";
    case TP_LIST:
        return "List";
    case TP_BYTE:
        return "Byte";
    case TP_USER:
        return Expr;
    case TP_VOID:
        return "Non-type";
    case TP_UNKNOWN:
        return "(Inconnu!)";
    default:
        break;
    }
    return "(Inconnu!)";
}

void VarTypeType::display()
{
    if (Type!=TP_USER)
    {
        switch (Type)
        {
        case TP_INTEGER:
            printf("TP_INTEGER");   break;
        case TP_FLOAT:
            printf("TP_FLOAT"); break;
        case TP_LONG:
            printf("TP_LONG");  break;
        case TP_STRING:
            printf("TP_STRING");    break;
        case TP_BOOLEAN:
            printf("TP_BOOLEAN");   break;
        case TP_FILE:
            printf("TP_FILE");  break;
        case TP_LIST:
            printf("TP_LIST");  break;
        case TP_BYTE:
            printf("TP_BYTE");  break;
        case TP_VOID:
            printf("TP_VOID");  break;
        case TP_UNKNOWN:
            printf("TP_UNKNOWN");   break;
        default:
            break;
        }
    }
    else
        printf("%s",Expr);
}
