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

#ifndef _VARTYPETYPE_H_1
#define _VARTYPETYPE_H_1

#include <string.h>

typedef enum VarTypeEnum {
    TP_INTEGER,
    TP_FLOAT,
    TP_LONG,
    TP_STRING,
    TP_BOOLEAN,
    TP_FILE,
    TP_LIST,
    TP_BYTE,
    TP_USER,        // user-defined type, dans ce cas, regarder le champs Expr
    TP_VOID,        // le type retourn? par une proc?dure
    TP_UNKNOWN
} VarTypeEnum;



class VarTypeType
{
public:
    VarTypeEnum Type;
    char* Expr;     //donne le nom si c'est user-defined type
public:
    VarTypeType(void);
    VarTypeType(VarTypeEnum bTy);
    ~VarTypeType(void);
    const char *VarTypeTypeImage();
    void display();

    bool operator ==(VarTypeType &b){
        if (Type==b.Type)
        {
            if (Type!=TP_USER)
                return true;
            else
                return !(strcmp(Expr,b.Expr));
        }
        else
            return false;
    }
    bool operator !=(VarTypeType &b){
        return !(*this==b);
    }
};


#endif
