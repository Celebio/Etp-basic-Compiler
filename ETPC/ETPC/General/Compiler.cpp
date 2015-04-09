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


#include "Compiler.h"


namespace ETPC{

Compiler::Compiler(){
    mVerbose=true;
    mHelp=false;
    mNoLogo=false;
    logoDisplayed=false;
    moduleCtr=0;
}
Compiler::Compiler(End_Etape bFin){
    fin =bFin;
    mVerbose=true;
    mHelp=false;
    mNoLogo=false;
    logoDisplayed=false;
    moduleCtr=0;
}
Compiler::~Compiler(){
    delete Syntax;
    delete Semantique;
    delete mVarPublic;
    delete mUDTypes;
    delete mFonctions;
}


void Compiler::DisplayLogo(){
    if (mNoLogo) return;
    printf("Compilateur ETP-Basic \nCopyright (C) Onur CELEBI 2004/2006\n");
    printf("Vous etes sous %s \n",get_os_name());
    printf("Version de test.Compile le %s a %s\n\n",__DATE__,__TIME__);
    logoDisplayed=true;
}
void Compiler::DisplayHelp(){
    printf("etpc [options] [etp files]\n");
    printf("Options du compilateur:\n");
    printf("\t-v\tVerbose, affichage des informations au fur et a mesure de la compilation\n");
    printf("\t-h\tAffiche l'aide\n");
    printf("\t-nologo\tN'affiche pas le logo de depart\n");
}
void Compiler::DisplayTokens(){
    mTokenizer.display();
}
void Compiler::DisplayErrors(){
    if (!errListe.estVide()){
        printf("Erreurs:\n--------------------------------------\n");
        errListe.display();
    }
}
void Compiler::DisplayVarPublic(){
    printf("variables publiques crees:\n");
    printf("----------------------------------------------\n");
    mVarPublic->display();
}
void Compiler::DisplayUDTypes(){
    printf("types crees:\n");
    printf("----------------------------------------------\n");
    mUDTypes->display();
}
void Compiler::DisplayFonctions(){
    printf("fonctions et leurs corps:\n");
    printf("----------------------------------------------\n");
    mFonctions->display();
}
void Compiler::AddSourceFile(const char* fileName){

    FILE *fp;
    char* TextString;
    int FileSize;
    if (!logoDisplayed) {
        DisplayLogo();
    }
    bool readingPredef= false;      // est-ce que c'est un fichier de predef?
    if (strlen(fileName) >5){
        if ( strcmp(&(fileName[strlen(fileName)-5]) , ".elib") ==0){
            readingPredef= true;
        }
    }
    mTokenizer.SetIsPredefFile(readingPredef);

    fp=fopen(fileName,"rb");
    if (mVerbose) printf("Ouverture du fichier nomme %s \n",fileName);
    if (fp)
    {
        fseek (fp, 0, SEEK_END);
        FileSize = ftell (fp); rewind (fp);

        TextString = (char *)malloc(FileSize+2);
        char *p=TextString;
        char *pCour=TextString;
        if (mVerbose) printf("FileSize: %i \n",FileSize);
        for (int k=0;k<FileSize;k++)
        {
            *p=(char)(getc(fp)&0x00FF);
            p++;
        }
        *p = 0; p++;
        *p = 0;

        mTokenizer.Tokenize(pCour,p-2,fileName);
        moduleCtr++;
    }
    else
    {
        printf("Impossible d'ouvrir le fichier %s\n",fileName);
    }
}

void Compiler::VerifSyntax(){
    liste=mTokenizer.getTagListe();
    Syntax=new VeriSyn(liste,&errListe);
    Syntax->VerifSyntax();
    mVarPublic = Syntax->GetVariablesPublicPtr();
    mUDTypes = Syntax->GetTypesPtr();
    mFonctions = Syntax->GetFonctionsPtr();

}
void Compiler::VerifSemantik(){
    Semantique = new VeriSem();
    Semantique->setEnvironnement(&errListe,mVarPublic,mUDTypes,mFonctions);
    Semantique->VerifSem();
}

void Compiler::Gener68k(const char* outputFileName){
    if (!errListe.estVide()) return;
    Gen68k generator(outputFileName);
    //generator.testGenerate();
    generator.setEnvironnement(&errListe,mVarPublic,mUDTypes,mFonctions);
    generator.generateCode();
}
void Compiler::Generz80(const char* outputFileName){
    if (!errListe.estVide()) return;
    Genz80 generz80(outputFileName);
    generz80.setEnvironnement(&errListe,mVarPublic,mUDTypes,mFonctions);
    //generz80.testGenerate();
    generz80.generateCode();
}

}
