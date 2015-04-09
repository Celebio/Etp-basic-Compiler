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
#ifndef _TAG_H_1
#define _TAG_H_1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TABSIZE 4       // le nombre d'espaces dans un TAB

extern const char *ReservedWords[];     // les chaines de caractere correspondants au type enumer? suivant:
typedef enum enumTokenType {TOKEN_RETURN,
    TOKEN_MAIN,
    TOKEN_AS,
    TOKEN_PROCEDURE,
    TOKEN_FUNCTION,
    TOKEN_LOCAL,
    TOKEN_PUBLIC,
    TOKEN_IF,
    TOKEN_THEN,
    TOKEN_ELSEIF,
    TOKEN_ELSE,
    TOKEN_QUIT,
    TOKEN_FOR,
    TOKEN_TO,
    TOKEN_STEP,
    TOKEN_NEXT,
    TOKEN_EXIT,
    TOKEN_DO,
    TOKEN_LOOP,
    TOKEN_WHILE,
    TOKEN_TYPET,
    TOKEN_END,
    TOKEN_LBL,
    TOKEN_GOTO,
    TOKEN_TYPEINTEGER,
    TOKEN_TYPEFLOAT,
    TOKEN_TYPELONG,
    TOKEN_TYPESTRING,
    TOKEN_TYPEBOOLEAN,
    TOKEN_TYPEFILE,
    TOKEN_TYPELIST,
    TOKEN_TYPEBYTE,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_XOR,
    TOKEN_NOT,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_OPENCRO,
    TOKEN_CLOSECRO,
    TOKEN_OPENPAR,
    TOKEN_CLOSEPAR,
    TOKEN_VIRGULE,
    TOKEN_POINT,
    TOKEN_INF,
    TOKEN_INFEQ1,
    TOKEN_INFEQ2,
    TOKEN_EQUAL,
    TOKEN_DIFFERENT,
    TOKEN_SUPERIEUR,
    TOKEN_SUPEQUAL1,
    TOKEN_SUPEQUAL2,
    TOKEN_DEUXPOINTS,
    TOKEN_MOD,
    TOKEN_PLUS,
    TOKEN_MOINS,
    TOKEN_MULT,
    TOKEN_DIV,
    TOKEN_CONCAT,
    TOKEN_EXCLAM,
    TOKEN_COMMENT,
    TOKEN_ESPACE,
    TOKEN_ENDOFMODULE,
    TOKEN_CRLF,
    TOKEN_IDENTIF,
    TOKEN_NOMBRE,
    TOKEN_STRINGCONSTANT,
    TOKEN_UNKNOWN} enumTokenType;

class TAG
{
private:
    enumTokenType Token;
    char Identif[200];
    int Col;
    int Ligne;
    const char *fileName;
    bool isPredefFileTAG;
    TAG* Next;
public:
    TAG(void);
    ~TAG(void);

    // Get
    enumTokenType GetToken() {return Token;}
    char* GetIdentif() {return Identif;}
    int GetCol() {return Col;}
    int GetLigne() {return Ligne;}
    const char* GetFileName() {return fileName;}
    bool GetisPredefFileTAG() {return isPredefFileTAG;}
    TAG* GetNext() {return Next;}

    // Set
    void SetToken(enumTokenType bToken) {Token=bToken;}
    void SetIdentif(char* bIdentif);
    void SetCol(int bCol) {Col=bCol;}
    void SetLigne(int bLigne) {Ligne=bLigne;}
    void SetFileName(const char* bFileName) {fileName=bFileName; }
    void SetisPredefFileTAG(bool bPredef) {isPredefFileTAG=bPredef;}
    void SetNext(TAG* bNext) {Next=bNext;}

    // Display
    void display();
};


#endif

