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

/***************** E T P B A S I C - C O M P I L E R **************\
    Author: Onur CELEBI
    2004/2007
\******************************************************************/
/******************************************************\
    LectureFichier.cpp

    ouvre le fichier TEST et appelle les diff?rentes
    fonctions, tokeniser, verificateur syntaxique...

\*******************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#include "LectureFichier.h"
#include "Compiler.h"






typedef enum OptReadStateType {INPUT_FILE, OUTPUT_FILE } OptReadStateType;


int main(int argc, const char *argv[])
{
    int i;
    ETPC::Compiler ETPBCompiler(EN_GENER);
    int srcFileCtr=0;


    const char* defOutputFileName="outetp";
    const char* outputFile=defOutputFileName;
    OptReadStateType State=INPUT_FILE;

    for (i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            if (State==INPUT_FILE){
                ETPBCompiler.AddSourceFile(argv[i]);
                srcFileCtr++;
            } else {
                outputFile=argv[i];
            }
        }
        else {

            if (!strcmp(argv[i]+1,"v")){
                ETPBCompiler.SetVerbose(true);
            } else if (!strcmp(argv[i]+1,"h")){
                ETPBCompiler.SetHelp(true);
            } else if (!strcmp(argv[i]+1,"nologo")){
                ETPBCompiler.SetNoLogo(true);
            } else {
                printf("Option inconnue:%s\n",argv[i]);
            }
        }
    }

    if (!srcFileCtr)
    {
        printf("Pas de fichier en entree\n");
        Quit();
    }
    // {{{ Fetching predefined functions and UD-types
    ETPBCompiler.AddSourceFile("etplib.elib");
    // }}}

    //ETPBCompiler.DisplayTokens();
    ETPBCompiler.VerifSyntax();
    ETPBCompiler.VerifSemantik();

    //ETPBCompiler.DisplayFonctions();
    ETPBCompiler.DisplayErrors();


    ETPBCompiler.Gener68k(outputFile);





    Quit();

}

void Quit(){
    //exit(1);
    #ifdef _WIN32
        char rien[30];
        printf("Appuyez sur Entree pour terminer..");
        scanf("%c",rien);
    #endif
    exit(1);
}


// void AfficheContenu(void *BText)
// {
//  printf("Affichage du contenu... \n");
//  printf((const char *)BText);
//  printf("\n");
// }

