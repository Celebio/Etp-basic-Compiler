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
#include "TAG.h"


const char *ReservedWords[] = {
    "Return",
    "MAIN",
    "as",
    "Procedure",
    "Function",
    "Local",
    "Public",
    "If",
    "Then",
    "ElseIf",
    "Else",
    "Quit",
    "For",
    "To",
    "Step",
    "Next",
    "Exit",
    "Do",
    "Loop",
    "While",
    "Type",
    "End",
    "Lbl",
    "GoTo",
    "Integer",
    "Float",
    "Long",
    "String",
    "Boolean",
    "File",
    "List",
    "Byte",
    "And",
    "Or",
    "Xor",
    "Not",
    "True",
    "False",
    "[",
    "]",
    "(",
    ")",
    ",",
    ".",
    "<",
    "<=",
    "=<",
    "=",
    "<>",
    ">",
    ">=",
    "=>",
    ":",
    "Mod",
    "+",
    "-",
    "*",
    "/",
    "&",    // concat
    "!",
    "'",    // commentaire .. tout ce qui suit cest juste pour avoir un tableau de meme taille que le tableau de token
    " ",    // espace
    " ",    // END OF MODULE
    " ",    //crlf
    " ",    //identif
    " ",    // nombre
    " ",    // stringconstant
    " "};   //unknown (normalement ca ne sert plus)



TAG::TAG(void)
{
    isPredefFileTAG=false;
}

TAG::~TAG(void)
{
}

void TAG::display(void)
{
const char* bT[] = {"TK_RETURN",
                "TK_MAIN",
                "TK_AS",
                "TK_PROCEDURE",
                "TK_FUNCTION",
                "TK_LOCAL",
                "TK_PUBLIC",
                "TK_IF",
                "TK_THEN",
                "TK_ELSEIF",
                "TK_ELSE",
                "TK_QUIT",
                "TK_FOR",
                "TK_TO",
                "TK_STEP",
                "TK_NEXT",
                "TK_EXITFOR",
                "TK_DO",
                "TK_LOOP",
                "TK_WHILE",
                "TK_TYPE",
                "TK_END",
                "TK_LBL",
                "TK_GOTO",
                "TK_TYPEINTEGER",
                "TK_TYPEFLOAT",
                "TK_TYPELONG",
                "TK_TYPESTRING",
                "TK_TYPEBOOLEAN",
                "TK_TYPEFILE",
                "TK_TYPELIST",
                "TK_TYPEBYTE",
                "TK_AND",
                "TK_OR",
                "TK_XOR",
                "TK_NOT",
                "TK_TRUE",
                "TK_FALSE",
                "TK_OPENCRO",
                "TK_CLOSECRO",
                "TK_OPENPAR",
                "TK_CLOSEPAR",
                "TK_VIRGULE",
                "TK_POINT",
                "TK_INF",
                "TK_INFEQ1",
                "TK_INFEQ2",
                "TK_EQUAL",
                "TK_DIFFERENT",
                "TK_SUPERIEUR",
                "TK_SUPEQUAL1",
                "TK_SUPEQUAL2",
                "TK_DEUXPOINTS",
                "TK_MOD",
                "TK_PLUS",
                "TK_MOINS",
                "TK_MULT",
                "TK_DIV",
                "TK_CONCAT",
                "TK_EXCLAM",
                "TK_COMMENTAIRE",
                "TK_ESPACE",
                "TK_ENDOFMODULE",
                "TK_CRLF",
                "TK_IDENTIF",
                "TK_NOMBRE",
                "TK_STRINGCONSTANT",
                "TK_UNKNOWN"};
    if (Token!=TOKEN_IDENTIF && Token!=TOKEN_NOMBRE && Token!=TOKEN_STRINGCONSTANT)
    {
        printf("  %s  (%s)  col: %i \n",bT[Token],ReservedWords[Token],Col);
    }
    else
    {
        printf("  %s  (%s)  col: %i \n",bT[Token],Identif,Col);
    }
        if (isPredefFileTAG) {
            printf("PREDEF\n");
        }

}
void TAG::SetIdentif(char* bIdentif){
    if (!bIdentif){
        Identif[0]=0;
        return;
    }
    strcpy(Identif,bIdentif);
}
