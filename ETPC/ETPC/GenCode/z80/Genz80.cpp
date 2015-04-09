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


#include "Genz80.h"

#include <fstream>
#include <string.h>
#include <cstdlib>


Genz80::Genz80(void){
}
Genz80::Genz80(const char *oFileName){
    sprintf(outputFileName,"%s%s",oFileName,".z80");
}

Genz80::~Genz80(void){
}

#define max(a,b) (a>b?a:b)

void Genz80::testGenerate(){
    //ilCoder.add(LOAD,ilCoder.createOp(2,3),ilCoder.createOp(5),SZ_W);
    //ilCoder.display();
}


void Genz80::setEnvironnement(Collection* BerrListe,
                            Collection* BVariablesPublic,
                            Collection* BTypes,
                            Collection* BFonctions)
{
    errListe=BerrListe;
    VariablesPublic=BVariablesPublic;
    Types=BTypes;
    Fonctions=BFonctions;
}

int Genz80::getNbRegObject(CNoeud* bNoeud,NatureOpz80 bNat){
    return 0;
}

int Genz80::getNbRegArith(CNoeud* bNoeud,NatureOpz80 bNat){
    return 0;
}



/*
size_opz80 Genz80::getSize(CNoeud* bNoeud){

}
*/



void Genz80::generateCode(){
    std::ofstream file(outputFileName);
    ilCoder.setStream(&file);
    ilCoder.displayHeader();
    ilCoder.displayFooter();
}

