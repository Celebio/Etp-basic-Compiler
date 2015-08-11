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
#ifndef _COMPILER_H_
#define _COMPILER_H_


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>


#include "tokenizer.h"
#include "verisyn.h"
#include "verisem.h"
#include "Gen68k.h"
#include "Genz80.h"


#ifdef _WIN32
    #define get_os_name() "Windows"
    #define OS 1


#else
    #include <sys/utsname.h>
    #define OS 2
    static inline const char *get_os_name(void)
    {
        static struct utsname osname;
        uname(&osname);
        return osname.sysname;
    }
#endif


typedef enum End_Etape {EN_VERIF_SYN,EN_VERIF_SEM, EN_GENER} End_Etape;


namespace ETPC{
// #ifdef True
// public class Compiler{
// #else
class Compiler{
//#endif
private:
    End_Etape fin;

    Tokenizer mTokenizer;
    TAG* liste;

    VeriSyn* Syntax;
    VeriSem* Semantique;
    Collection errListe;
    bool mVerbose;
    bool mNoLogo;
    bool mHelp;
    bool logoDisplayed;
    int moduleCtr;

    Collection* mVarPublic;
    Collection* mUDTypes;
    Collection* mFonctions;


    void DisplayLogo();

public:

    Compiler(void);
    Compiler(End_Etape bFin);

    ~Compiler(void);

    void DisplayHelp();
    void DisplayTokens();
    void DisplayErrors();
    void DisplayVarPublic();
    void DisplayUDTypes();
    void DisplayFonctions();

    void AddSourceFile(const char* fileName);

    void Tokenize();
    void VerifSyntax();
    void VerifSemantik();
    void Gener68k(const char* outputFileName);
    void Generz80(const char* outputFileName);

    void SetVerbose(bool bVerbose) {mVerbose =bVerbose;}
    void SetHelp(bool bHelp) {mHelp =bHelp;}
    void SetNoLogo(bool bNoLogo) {mNoLogo =bNoLogo;}
};

}

#endif

