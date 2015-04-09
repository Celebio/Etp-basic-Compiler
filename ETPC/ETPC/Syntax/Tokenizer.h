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
#ifndef _TOKENIZER_H_1
#define _TOKENIZER_H_1

#include "TAG.h"
#include <stdlib.h>
#include <string.h>

typedef enum errorIsNumeric {
    ISINTEGER,
    ISFLOAT,
    ISNOTNUMERIC
} errorIsNumeric;

class Tokenizer
{

private:
    TAG *pListe;
    TAG *pListeCr;

    char *programBuffer;
    char *Fin;
    const char *CourFileName;
    errorIsNumeric IsNumeric(TAG* bTag);

    void AvanceTokenPtr(char **pText);
    TAG GetToken(char **Btext,int* Col);
    char GiveLowerCase(const char *bC);

    bool isPredefFile;
public:
    Tokenizer(void);

    ~Tokenizer(void);

    errorIsNumeric IsNumeric(char *ch);

    void Tokenize(char *pCour,char *pFin,const char *fileName);
    TAG* getTagListe() {return pListe;}
    void display(void);
    void SetIsPredefFile(bool predef) {isPredefFile=predef;}
};

#endif
