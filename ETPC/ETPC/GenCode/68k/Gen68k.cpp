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


#include "Gen68k.h"

#include <fstream>
#include <string.h>
#include <cstdlib>


#define max(a,b) (a>b?a:b)
//#define max3(a,b,c)  ( a>b ? (max(a,c)) : (max(b,c)) )
#define max3(a,b,c) (max(a,max(b,c)))       // Geyelin optimisation

Gen68k::Gen68k(void){
    registerStack=new PileRegTemp68k(&ilCoder,&Stack);
}
Gen68k::Gen68k(const char *oFileName){
    sprintf(outputFileName,"%s%s",oFileName,".asm");
    registerStack=new PileRegTemp68k(&ilCoder,&Stack);
}

Gen68k::~Gen68k(void){
}


void Gen68k::TestGenerate(){
    //ilCoder.add(LOAD,ilCoder.createOp(2,3),ilCoder.createOp(5),SZ_W);
    ilCoder.display();
}


void Gen68k::SetEnvironnement(Collection* BerrListe,
                            Collection* BVariablesPublic,
                            Collection* BTypes,
                            Collection* BFonctions)
{
    errListe=BerrListe;
    VariablesPublic=BVariablesPublic;
    Types=BTypes;
    Fonctions=BFonctions;
}

int Gen68k::NbRegObjet(CNoeud* bNoeud, NatureOp bNat){
    int val;
    if (bNoeud->getNature()==NOEUD_OPERANDE_VARIABLE){
        if (bNat==NO_REG){
            val=1;
        }
        else if (bNat==NO_DVAL || bNat==NO_DADR){
            val=0;
        }
    }
    else{
        printf("Cas inconnu dans NbRegObjet\n");
    }
    bNoeud->setNbReg(val);
    return val;
}

int Gen68k::NbRegBool(CNoeud* bNoeud){
    bNoeud->display();
    int val=-1;
    TypeOptor operaTOR = bNoeud->getOperator();

    if (operaTOR == OPTOR_CMP_AND || operaTOR == OPTOR_CMP_OR){
        int n1 = NbRegBool(bNoeud->getLeftChild());
        int n2 = NbRegBool(bNoeud->getRightChild());
        val = max3(1,n1,n2);
    }
    else if (operaTOR == OPTOR_NOT){
        val = NbRegBool(bNoeud->getRightChild());
    }
    else if (operaTOR == OPTOR_UNKNOWN){
        if ((bNoeud->getNature()== NOEUD_OPERANDE_CTE && bNoeud->getTag()->GetToken() ==TOKEN_TRUE)
            || (bNoeud->getNature()== NOEUD_OPERANDE_CTE && bNoeud->getTag()->GetToken() ==TOKEN_FALSE)){
            val=1;  // TO DO : ? voir
        }
        // TO DO : autre objets...
    }
    else if (operaTOR == OPTOR_INF || operaTOR == OPTOR_INFEQ || operaTOR == OPTOR_EQUAL ||
        operaTOR == OPTOR_DIFFERENT || operaTOR == OPTOR_SUP || operaTOR == OPTOR_SUPEQ ) {
        int n1 = NbRegArith(bNoeud->getLeftChild(),NO_REG);
        int n2 = NbRegArith(bNoeud->getRightChild(),NO_DVAL);
        if (n1!=n2){
            val = max(n1,n2);
        } else {
            val = n1+1;
        }
    }
    else{
        printf("je suis la nbregbool\n");
    }
    bNoeud->setNbReg(val);
    return val;
}

int Gen68k::NbRegArith(CNoeud* bNoeud,NatureOp bNat){
    NatureNoeud NatureArbre=bNoeud->getNature();
    int val=-1;
    int n1,n2,nMax;
    // A = Litteral
    if (NatureArbre==NOEUD_OPERANDE_CTE){
        if (bNat==NO_REG){
            val=1;
        }
        else{
            val=0;
        }
    }
    // A = objet
    else if (NatureArbre==NOEUD_OPERANDE_VARIABLE)
    {
        val=NbRegObjet(bNoeud,bNat);
    }
    // A = - A1 ou A = conversion(A1)
    else if (NatureArbre==NOEUD_OPERATOR && bNoeud->getOperator()==OPTOR_MOINSUNAIRE)
    {
        val=max(1,NbRegArith(bNoeud,bNat));
    }
    //else if (NatureArbre==NOEUD_OPERATOR && bNoeud->getOperType()==OPBIN

    else if (NatureArbre==NOEUD_OPERATOR && bNoeud->getOperType()==OPBIN)
    {
        n1=NbRegArith(bNoeud->getLeftChild(),NO_REG);
        n2=NbRegArith(bNoeud->getRightChild(),NO_DVAL);
        if (n1==n2){
            val=n1+1;
        }else{
            val=max(n1,n2);
        }
    }
    // A = f(A1,A2, ... , An)
    else if (NatureArbre==NOEUD_OPERANDE_FONCTION)
    {
        nMax=1;
        for (int i=0;i<bNoeud->getSuccNmbr();i++){
            n1=NbRegArith(*(bNoeud->getSuccPtr(i)),NO_REG);
            if (n1>nMax){
                nMax=n1;
            }
        }
        val=nMax;
    }

    bNoeud->setNbReg(val);
    return val;
}
void Gen68k::CodeObjet(CNoeud* bNoeud,NatureOp bNat,Operande68k** opertr){
    size_op68k Size=GetSize(bNoeud);

    if (bNoeud->getNature()==NOEUD_OPERANDE_VARIABLE){
        if (bNat==NO_REG){
            ilCoder.add(MOVE,ilCoder.createOp(Stack.GetStackPos(bNoeud->getTag()->GetIdentif()),SP_REG),registerStack->front(),Size);
            *opertr=registerStack->front();
        }
        else if (bNat==NO_DVAL || bNat==NO_DADR){

            *opertr=ilCoder.createOp(Stack.GetStackPos(bNoeud->getTag()->GetIdentif()),SP_REG);
        }
    }
    else{
        printf("Cas inconnu dans CodeObjet\n");
    }
}

size_op68k Gen68k::GetSize(CNoeud* bNoeud){
    size_op68k Size=SZ_UNKNOWN;

    if (bNoeud->getType().Type==TP_INTEGER) Size=SZ_W;
    if (bNoeud->getType().Type==TP_FLOAT) Size=SZ_F;
    if (bNoeud->getType().Type==TP_LONG) Size=SZ_L;
    if (bNoeud->getType().Type==TP_BOOLEAN) Size=SZ_W;  // et pas Size=SZ_B;
    if (bNoeud->getType().Type==TP_BYTE) Size=SZ_W;     // et pas Size=SZ_B;
    return Size;
}

void Gen68k::CodeArith(CNoeud* bNoeud,NatureOp bNat,Operande68k** opertr){
    NatureNoeud NatureArbre=bNoeud->getNature();
    size_op68k Size=GetSize(bNoeud);
    Operande68k* Op1;
    Operande68k* Op2;
    //printf("entree dans codearith avec bNoeud:\n");
    //bNoeud->display();

    // A = Litteral
    if (NatureArbre==NOEUD_OPERANDE_CTE){
        if (bNat==NO_REG){
            if (bNoeud->getTag()->GetToken()==TOKEN_NOMBRE)
            {
                ilCoder.add(MOVE,ilCoder.createOpVal(atoi(bNoeud->getTag()->GetIdentif())),registerStack->front(),Size);

            }
            else if (bNoeud->getTag()->GetToken()==TOKEN_STRINGCONSTANT)
            {
                // pas encore g?r?
            }
            else if (bNoeud->getTag()->GetToken()==TOKEN_TRUE)
            {
                ilCoder.add(MOVE,ilCoder.createOpVal(1),registerStack->front(),Size);

            }
            else if (bNoeud->getTag()->GetToken()==TOKEN_FALSE)
            {
                ilCoder.add(MOVE,ilCoder.createOpVal(0),registerStack->front(),Size);
            }
            else
            {
                printf("ERREUR interne2!!!\n");
            }
            *opertr=registerStack->front();
        }
        else{
            if (bNoeud->getTag()->GetToken()==TOKEN_NOMBRE)
            {
                *opertr=ilCoder.createOpVal(atoi(bNoeud->getTag()->GetIdentif()));
            }
            else if (bNoeud->getTag()->GetToken()==TOKEN_STRINGCONSTANT)
            {
                // pas encore g?r?
            }
            else if (bNoeud->getTag()->GetToken()==TOKEN_TRUE)
            {
                *opertr=ilCoder.createOpVal(1);
            }
            else if (bNoeud->getTag()->GetToken()==TOKEN_FALSE)
            {
                *opertr=ilCoder.createOpVal(0);
            }
            else
            {
                printf("ERREUR interne1!!!\n");
            }
        }
    }
    // A = objet
    else if (NatureArbre==NOEUD_OPERANDE_VARIABLE)
    {
        CodeObjet(bNoeud,bNat,opertr);
    }
    // A = - A1 ou A = conversion(A1)
    else if (NatureArbre==NOEUD_OPERATOR && bNoeud->getOperator()==OPTOR_MOINSUNAIRE)
    {
        CodeArith(bNoeud->getRightChild(),NO_DVAL,&Op1);
        ilCoder.add(OPP,Op1,registerStack->front(),Size);
        *opertr=registerStack->front();
    }
    // A = A1 opbin A2 et op_bin commutatif et A1 est litteral ou objet
    // et A2 n'est ni litteral ni objet
    else if (NatureArbre==NOEUD_OPERATOR && bNoeud->getOperType()==OPBIN &&
            bNoeud->isCommutative() &&
             (bNoeud->getLeftChild()->getNature()==NOEUD_OPERANDE_CTE ||
             bNoeud->getLeftChild()->getNature()==NOEUD_OPERANDE_VARIABLE)
             &&
            (bNoeud->getRightChild()->getNature()!=NOEUD_OPERANDE_CTE &&
             bNoeud->getRightChild()->getNature()!=NOEUD_OPERANDE_VARIABLE))
    {
        CNoeud* aux=bNoeud->getRightChild();
        bNoeud->setRightChild(bNoeud->getLeftChild());
        bNoeud->setLeftChild(aux);
        CodeArith(bNoeud, bNat, opertr);
    }

    else if (NatureArbre==NOEUD_OPERATOR && bNoeud->getOperType()==OPBIN)
    {
        int n1,n2;
        n1=bNoeud->getLeftChild()->getNbReg();
        n2=bNoeud->getRightChild()->getNbReg();
        Operande68k* T;

        if (n1>=mNbRegMax && n2 >=mNbRegMax){
            CodeArith(bNoeud->getRightChild(),NO_REG,&Op2);
            T=registerStack->allocateTemp(Size);
            ilCoder.add(MOVE,Op2,T,Size);
            CodeArith(bNoeud->getLeftChild(),NO_REG,&Op1);
            registerStack->freeTemp(T,Size);
            ilCoder.add(ilCoder.nodeToOp(bNoeud),T,Op1,Size);
            delete T;
            *opertr=Op1;
        }
        else if (n1>=n2 && n2 < mNbRegMax)
        {
            CodeArith(bNoeud->getLeftChild(),NO_REG,&Op1);
            registerStack->allocate(Op1);
            CodeArith(bNoeud->getRightChild(),NO_DVAL,&Op2);
            registerStack->freeOperand(Op1);
            ilCoder.add(ilCoder.nodeToOp(bNoeud),Op2,Op1,Size);
            *opertr=Op1;
        }
        else if (n1<n2 && n1 < mNbRegMax)
        {
            registerStack->switchD();
            CodeArith(bNoeud->getRightChild(),NO_DVAL,&Op2);
            registerStack->allocate(Op2);
            CodeArith(bNoeud->getLeftChild(),NO_REG,&Op1);
            registerStack->freeOperand(Op2);
            registerStack->switchD();
            ilCoder.add(ilCoder.nodeToOp(bNoeud),Op2,Op1,Size);
            *opertr=Op1;
        }
    }
    // A = f(A1,A2, ... , An)
    else if (NatureArbre==NOEUD_OPERANDE_FONCTION)
    {
        int somme=0;        // la somme des tailles de ce qui est pouss? dans la pile
        size_op68k SizeOfArgument;
        for (int i=bNoeud->getSuccNmbr()-1;i>=0;i--){
            SizeOfArgument=GetSize(*bNoeud->getSuccPtr(i));
            somme+=(int)SizeOfArgument;
            CodeArith(*bNoeud->getSuccPtr(i),NO_REG,&Op2);
            ilCoder.add(MOVE,Op2,ilCoder.createOp(A7,false,true),SizeOfArgument);
            Stack.PushToStack(SizeOfArgument);
        }
        ilCoder.add(BSR,ilCoder.createOpLabel(bNoeud->getTag()->GetIdentif()),SZ_NA);
        ilCoder.add(ADD,ilCoder.createOpVal(somme),ilCoder.createOp(SP_REG),SZ_L);

        //Stack.display();
        for (int i=bNoeud->getSuccNmbr()-1;i>=0;i--){
            Stack.Pop();
            //Stack.display();
        }
        ilCoder.add(MOVE,ilCoder.createOp(D0),registerStack->front(),Size);
        *opertr=registerStack->front();
    }
}

void Gen68k::CodeBool(CNoeud* bNoeud,bool AvecSaut,bool ValSaut,Operande68k* etiqSaut,bool DansReg,bool ValReg){
    //printf("Entree dans COdeBOOl\n");
    //printf("expression booleenne:\n");
    //bNoeud->display();

    size_op68k Size=GetSize(bNoeud);
    Operande68k* T;

    Operande68k* Etiq = ilCoder.createOpLabel();
    Operande68k* Etiq1 = ilCoder.createOpLabel();
    //Operande68k* Etiq2 = ilCoder.createOpLabel();

    TypeOptor operaTOR = bNoeud->getOperator();

    if (operaTOR == OPTOR_CMP_AND){
        if (AvecSaut==true){
            if (ValSaut==true){
                CodeBool(bNoeud->getLeftChild(),true,false,Etiq1,DansReg,ValReg);
                CodeBool(bNoeud->getRightChild(),true,true,etiqSaut,DansReg,ValReg);
                ilCoder.addLabel(Etiq1->val.valLabel);
            }
            else {  // ValSaut==false
                CodeBool(bNoeud->getLeftChild(),true,false,etiqSaut,DansReg,ValReg);
                CodeBool(bNoeud->getRightChild(),true,false,etiqSaut,DansReg,ValReg);
            }
        }
        else {      // AvecSaut==false
            CodeBool(bNoeud->getLeftChild(),true,false,Etiq1,true,ValReg);
            CodeBool(bNoeud->getRightChild(),false,false,NULL,true,ValReg);  // le deuxieme false n'a aucun impact normalement
            ilCoder.addLabel(Etiq1->val.valLabel);
        }
    }
    else if (operaTOR == OPTOR_CMP_OR){
        if (AvecSaut==true){
            if (ValSaut==true){
                CodeBool(bNoeud->getLeftChild(),true,true,etiqSaut,DansReg,ValReg);
                CodeBool(bNoeud->getRightChild(),true,true,etiqSaut,DansReg,ValReg);
            }
            else {  // ValSaut==false
                CodeBool(bNoeud->getLeftChild(),true,true,Etiq1,DansReg,ValReg);
                CodeBool(bNoeud->getRightChild(),true,false,etiqSaut,DansReg,ValReg);
                ilCoder.addLabel(Etiq1->val.valLabel);
            }
        }
        else {      // AvecSaut==false
            CodeBool(bNoeud->getLeftChild(),true,true,Etiq1,true,ValReg);
            CodeBool(bNoeud->getRightChild(),false,false,NULL,true,ValReg);  // le deuxieme false n'a aucun impact normalement
            ilCoder.addLabel(Etiq1->val.valLabel);
        }
    }
    else if (operaTOR == OPTOR_NOT){
        CodeBool(bNoeud->getRightChild(),AvecSaut,!ValSaut,etiqSaut,DansReg,!ValReg);
    }
    else if (operaTOR == OPTOR_UNKNOWN){
        if (bNoeud->getNature()== NOEUD_OPERANDE_CTE && bNoeud->getTag()->GetToken() ==TOKEN_TRUE){
            Operande68k* ValRegOp;
            ValRegOp = ilCoder.createOpVal( ValReg ? 1 : 0 );
            if (AvecSaut==true){
                ilCoder.add(MOVE,ValRegOp,registerStack->front(),SZ_B);
                ilCoder.add(BRA,etiqSaut,SZ_NA);
            }
            else {  // AvecSaut==false
                ilCoder.add(MOVE,ValRegOp,registerStack->front(),SZ_B);
            }
        } else if (bNoeud->getNature()== NOEUD_OPERANDE_CTE && bNoeud->getTag()->GetToken() ==TOKEN_FALSE){
            Operande68k* ValRegOp;
            ValRegOp = ilCoder.createOpVal( ValReg ? 0 : 1 );
            if (AvecSaut==true){
                ilCoder.add(MOVE,ValRegOp,registerStack->front(),SZ_B);
                ilCoder.add(BRA,etiqSaut,SZ_NA);
            }
            else {  // AvecSaut==false
                ilCoder.add(MOVE,ValRegOp,registerStack->front(),SZ_B);
            }
        }
    }
    else if (operaTOR == OPTOR_INF || operaTOR == OPTOR_INFEQ || operaTOR == OPTOR_EQUAL ||
        operaTOR == OPTOR_DIFFERENT || operaTOR == OPTOR_SUP || operaTOR == OPTOR_SUPEQ ) {
        int n1 = NbRegArith(bNoeud->getLeftChild(),NO_REG);
        int n2 = NbRegArith(bNoeud->getRightChild(),NO_DVAL);
        Operande68k* Op2;
        Operande68k* Op1;
        if (n1 >= mNbRegMax && n2 >= mNbRegMax) {
            CodeArith(bNoeud->getRightChild(),NO_REG,&Op2);
            T=registerStack->allocateTemp(Size);
            ilCoder.add(MOVE,Op2,T,Size);
            CodeArith(bNoeud->getLeftChild(),NO_REG,&Op1);
            registerStack->freeTemp(T,Size);
            ilCoder.add(CMP,T,Op1,GetSize(bNoeud->getRightChild()));
        }
        else if (n1>=n2 && n2 < mNbRegMax)
        {
            CodeArith(bNoeud->getLeftChild(),NO_REG,&Op1);
            registerStack->allocate(Op1);
            CodeArith(bNoeud->getRightChild(),NO_DVAL,&Op2);
            registerStack->freeOperand(Op1);
            ilCoder.add(CMP,Op2,Op1,GetSize(bNoeud->getRightChild()));
        }
        else if (n1<n2 && n1 < mNbRegMax)
        {
            registerStack->switchD();
            CodeArith(bNoeud->getRightChild(),NO_DVAL,&Op2);
            registerStack->allocate(Op2);
            CodeArith(bNoeud->getLeftChild(),NO_REG,&Op1);
            registerStack->freeOperand(Op2);
            registerStack->switchD();
            ilCoder.add(CMP,Op2,Op1,GetSize(bNoeud->getRightChild()));
        }

        if (AvecSaut){
            // Scc
            /*
            if (ValReg==true){
                ilCoder.add((InsOpEnum68k)(ilCoder.nodeToOp(bNoeud)+(SEQ-BEQ)),Op1,Size);
            } else {        // sinon l'oppos?
                ilCoder.add((InsOpEnum68k)(ilCoder.getOppBra(ilCoder.nodeToOp(bNoeud))+(SEQ-BEQ)),Op1,Size);
            }*/
            // Bcc
            if (ValSaut==true){
                ilCoder.add(ilCoder.nodeToOp(bNoeud),etiqSaut,SZ_NA);
            } else {        // sinon l'oppos?
                ilCoder.add(ilCoder.getOppBra(ilCoder.nodeToOp(bNoeud)),etiqSaut,SZ_NA);
            }

        } else {
            // Scc
            /*
            if (ValReg==true){
                ilCoder.add((InsOpEnum68k)(ilCoder.nodeToOp(bNoeud)+(SEQ-BEQ)),Op1,Size);
            } else {        // sinon l'oppos?
                ilCoder.add((InsOpEnum68k)(ilCoder.getOppBra(ilCoder.nodeToOp(bNoeud))+(SEQ-BEQ)),Op1,Size);
            }*/
        }
    }
    else{
        printf("je suis la\n");
    }

}


void Gen68k::CodeInstr(Collection* bInstrSuite){
    ColIterator iter2;
    bInstrSuite->bindIterator(&iter2);
    InstructionETPB* Instr1;

    while(iter2.elemExists()){
        Instr1=(InstructionETPB*)iter2.getNext();
        CodeInstr(Instr1);
    }
}
void Gen68k::CodeInstr(InstructionETPB* bInstr){



    registerStack->init();
    switch(bInstr->getNat()){
    case INS_DECLARATION:
        break;
    case INS_UNKNOWN:
        break;
    case INS_AFFECTATION:
        Operande68k* ExprArbrAff;   //=registerStack->front();
        Operande68k* ExprArbrAffected;
        int nG,nD;

        nG=NbRegArith(bInstr->GetAffectExprAssigned(),NO_REG);
        nD=NbRegArith(bInstr->GetAffectExprArbre(),NO_REG);

        if (nD >= nG){
            CodeArith(bInstr->GetAffectExprArbre(),NO_REG,&ExprArbrAff);
            registerStack->allocate(ExprArbrAff);
            CodeArith(bInstr->GetAffectExprAssigned(),NO_DADR,&ExprArbrAffected);
            registerStack->freeOperand(ExprArbrAff);
            ilCoder.add(MOVE,ExprArbrAff,ExprArbrAffected,GetSize(bInstr->GetAffectExprAssigned()));
        }
        else{
            CodeArith(bInstr->GetAffectExprAssigned(),NO_DADR,&ExprArbrAffected);
            registerStack->allocate(ExprArbrAffected);
            CodeArith(bInstr->GetAffectExprArbre(),NO_REG,&ExprArbrAff);
            registerStack->freeOperand(ExprArbrAffected);
            ilCoder.add(MOVE,ExprArbrAff,ExprArbrAffected,GetSize(bInstr->GetAffectExprAssigned()));
        }
        break;
    case INS_RETURN:
        Operande68k* OpResReturn;
        if (bInstr->GetReturnExpr()){
            CodeArith(bInstr->GetReturnExpr(),NO_REG,&OpResReturn);
            ilCoder.add(MOVE,OpResReturn,ilCoder.createOp(D0),GetSize(bInstr->GetReturnExpr()));
        }
        ilCoder.add(RTS);
        break;
    case INS_CALL:
        int somme;
        CNoeud* CallNoeud;

        somme=0;        // la somme des tailles de ce qui est pouss? dans la pile
        size_op68k SizeOfArgument;
        CallNoeud=bInstr->GetCallExpr();
        Operande68k* Op2;
        for (int i=CallNoeud->getSuccNmbr()-1;i>=0;i--){
            SizeOfArgument=GetSize(*CallNoeud->getSuccPtr(i));
            somme+=(int)SizeOfArgument;
            CodeArith(*CallNoeud->getSuccPtr(i),NO_REG,&Op2);
            ilCoder.add(MOVE,Op2,ilCoder.createOp(A7,false,true),SizeOfArgument);
            Stack.PushToStack(SizeOfArgument);
        }
        ilCoder.add(BSR,ilCoder.createOpLabel(CallNoeud->getTag()->GetIdentif()),SZ_NA);
        ilCoder.add(ADD,ilCoder.createOpVal(somme),ilCoder.createOp(SP_REG),SZ_L);
        //Stack.display();
        for (int i=CallNoeud->getSuccNmbr()-1;i>=0;i--){
            Stack.Pop();
            //Stack.display();
        }
        break;
    case INS_STRUCT_FOR:
        CNoeud* initExpr;
        CNoeud* finExpr;
        CNoeud* stepExpr;
        CNoeud* varExpr;
        Operande68k* EtiqForDebTest;
        Operande68k* EtiqForFin;

        size_op68k SizeVarIter;
        Operande68k* TexpTO;
        Collection* CorpsFor;

        initExpr = bInstr->GetFORExprArbreINIT();
        finExpr = bInstr->GetFORExprArbreTO();
        stepExpr = bInstr->GetFORExprArbreSTEP();
        varExpr = bInstr->GetFORExprAssigned();
        CorpsFor = bInstr->GetFORCorps();
        SizeVarIter =GetSize(varExpr);
        EtiqForDebTest = ilCoder.createOpLabel();
        EtiqForFin = ilCoder.createOpLabel();

        // ---------------------------------------------------
        // Affectation initiale
        // ---------------------------------------------------
        nG=NbRegArith(varExpr,NO_REG);
        nD=NbRegArith(initExpr,NO_REG);

        if (nD >= nG){
            CodeArith(initExpr,NO_REG,&ExprArbrAff);
            registerStack->allocate(ExprArbrAff);
            CodeArith(varExpr,NO_DADR,&ExprArbrAffected);
            registerStack->freeOperand(ExprArbrAff);
            ilCoder.add(MOVE,ExprArbrAff,ExprArbrAffected,SizeVarIter);
        }
        else{
            CodeArith(varExpr,NO_DADR,&ExprArbrAffected);
            registerStack->allocate(ExprArbrAffected);
            CodeArith(initExpr,NO_REG,&ExprArbrAff);
            registerStack->freeOperand(ExprArbrAffected);
            ilCoder.add(MOVE,ExprArbrAff,ExprArbrAffected,SizeVarIter);
        }

        // ---------------------------------------------------
        // Evaluation de l'expression d'arriv?e
        // ---------------------------------------------------

        // temporaire sur la pile qui va contenir la valeur de l'expression d'arriv?e TO
        TexpTO=registerStack->allocateTemp(SizeVarIter);
        CodeArith(finExpr,NO_REG,&ExprArbrAff);
        //registerStack->allocate(ExprArbrAff);
        ilCoder.add(MOVE,ExprArbrAff,TexpTO,SizeVarIter);
        //registerStack->freeOperand(ExprArbrAff);


        // ---------------------------------------------------
        // Test de la condition d'arriv?
        // ---------------------------------------------------
        ilCoder.addLabel(EtiqForDebTest->val.valLabel);

        // cas 1: pas de step
        if (stepExpr == NULL){
            CodeArith(varExpr,NO_REG,&ExprArbrAffected);
            ilCoder.add(CMP,TexpTO,ExprArbrAffected,SizeVarIter);
            ilCoder.add(BGT,EtiqForFin,SZ_NA);
            CodeInstr(CorpsFor);
            // iterateur = iterateur+1 sous entendu
            CodeArith(varExpr,NO_DADR,&ExprArbrAffected);
            ilCoder.add(ADD,ilCoder.createOpVal(1),ExprArbrAffected,SizeVarIter);
            ilCoder.add(BRA,EtiqForDebTest,SZ_NA);
        }
        // cas 2: step constante
        else if (stepExpr->isConstant()){
            int constStepVal;
            constStepVal = atoi(stepExpr->getTag()->GetIdentif());
            CodeArith(varExpr,NO_REG,&ExprArbrAffected);
            ilCoder.add(CMP,TexpTO,ExprArbrAffected,SizeVarIter);
            if (constStepVal < 0){
                ilCoder.add(BLT,EtiqForFin,SZ_NA);
            }
            else {
                ilCoder.add(BGT,EtiqForFin,SZ_NA);
            }
            CodeInstr(CorpsFor);
            // iterateur = iterateur+cte sous entendu
            CodeArith(varExpr,NO_DADR,&ExprArbrAffected);
            ilCoder.add(ADD,ilCoder.createOpVal(constStepVal),ExprArbrAffected,SizeVarIter);
            ilCoder.add(BRA,EtiqForDebTest,SZ_NA);

        }
        // cas 3: step non-constante
        else {
            //Operande68k* EtiqNegStep;
            //Operande68k* EtiqForDebCorps;
            //EtiqForFin = ilCoder.createOpLabel();

        }
        ilCoder.addLabel(EtiqForFin->val.valLabel);
        registerStack->freeTemp(TexpTO,SizeVarIter);   // on libere le temporaire qui contient l'expression de fin
        break;
    case INS_STRUCT_DOLPWH:     // do ... loop while (condition)
        Collection* CorpsDo;
        CNoeud* exprDo;
        Operande68k* EtiqDebut;
        CorpsDo = bInstr->GetDOCorps();
        exprDo = bInstr->GetDOExprCondition();
        EtiqDebut = ilCoder.createOpLabel();
        ilCoder.addLabel(EtiqDebut->val.valLabel);
        CodeInstr(CorpsDo);
        CodeBool(exprDo,true,true,EtiqDebut,false,false);
        break;
    case INS_STRUCT_DOWH:       // do while (condition) .... loop
        Collection* CorpsWhile;
        CNoeud* exprWhile;
        Operande68k* EtiqWhileDebut;
        Operande68k* EtiqWhileFin;

        CorpsWhile = bInstr->GetDOCorps();
        exprWhile = bInstr->GetDOExprCondition();
        EtiqWhileDebut = ilCoder.createOpLabel();
        EtiqWhileFin = ilCoder.createOpLabel();      // juste avant le test de la condition

        ilCoder.add(BRA,EtiqWhileFin,SZ_NA);        // on va directement voir le test
        ilCoder.addLabel(EtiqWhileDebut->val.valLabel);
        CodeInstr(CorpsWhile);
        ilCoder.addLabel(EtiqWhileFin->val.valLabel);
        CodeBool(exprWhile,true,true,EtiqWhileDebut,false,false);

        break;
    case INS_IF:
        CNoeud* exprIf;
        exprIf = bInstr->GetIFExprArbre();
        //printf("CodeInstr IF\n");
        //printf("Expression du If:\n");
        //exprIf->display();

        Operande68k* EtiqFinCorpsIF = ilCoder.createOpLabel();
        Operande68k* EtiqFinLocalCorpsElseIf;
        Operande68k* EtiqDebutElse=NULL;    // utilis? s'il y a des elseIf et un else
        Operande68k* EtiqFinBloc;


        Collection* InstrCorpsIf = bInstr->GetIFIfCorps();
        Collection* InstrCorpsElse = bInstr->GetIFElseCorps();
        Collection* listExprElseIf = bInstr->GetIFExprElseIf();
        Collection* listCorpsElseIf = bInstr->GetIFElseIfCorps();

        ColIterator iterExprElseIf;
        ColIterator iterCorpsElseIf;

        listExprElseIf->bindIterator(&iterExprElseIf);
        listCorpsElseIf->bindIterator(&iterCorpsElseIf);
        Collection* InstrCorpsElseIf;
        CNoeud* exprElseIf;

        // -------------------------------------------------------
        // Codage de expression de IF
        // -------------------------------------------------------
        CodeBool(exprIf,true,false,EtiqFinCorpsIF,false,false); // le dernier false n'a aucun impact normalement

        // -------------------------------------------------------
        // Codage de corps de IF
        // -------------------------------------------------------
        CodeInstr(InstrCorpsIf);
        if (!InstrCorpsElse->estVide() || !listExprElseIf->estVide() ){
            EtiqFinBloc = ilCoder.createOpLabel();
            ilCoder.add(BRA,EtiqFinBloc,SZ_NA);     // on ajoute un saut ? tout ? la fin s'il y a un else ou des elseif
        }
        // -------------------------------------------------------
        //      label fin corps If
        // -------------------------------------------------------
        ilCoder.addLabel(EtiqFinCorpsIF->val.valLabel);


        // -------------------------------------------------------
        // Codage des ELSE IF
        // -------------------------------------------------------
        while(iterExprElseIf.elemExists()){
            exprElseIf=(CNoeud*)iterExprElseIf.getNext();
            InstrCorpsElseIf=(Collection*)iterCorpsElseIf.getNext();
            if (iterExprElseIf.elemExists()) {  // le suivant existe
                EtiqFinLocalCorpsElseIf = ilCoder.createOpLabel();   // donc nouvelle etiquette
            } else {        // dernier element
                EtiqDebutElse = ilCoder.createOpLabel();
                EtiqFinLocalCorpsElseIf=EtiqDebutElse;  // on utilise l'etiquette de fin de tout le bloc IF

            }

            // -------------------------------------------------------
            // Codage de expression de ElseIF
            // -------------------------------------------------------
            CodeBool(exprElseIf,true,false,EtiqFinLocalCorpsElseIf,false,false);

            // -------------------------------------------------------
            // Codage de corps de ElseIF
            // -------------------------------------------------------
            CodeInstr(InstrCorpsElseIf);

            if (!InstrCorpsElse->estVide()){            // il existe un Else, il ne faut pas executer son corps, donc saut ? la fin
                ilCoder.add(BRA,EtiqFinBloc,SZ_NA);
            }
            if (iterExprElseIf.elemExists()) {  // le suivant ElseIf existe, on avait cr?e une nouvelle ?tiquette
                ilCoder.addLabel(EtiqFinLocalCorpsElseIf->val.valLabel);  // label fin corps ElseIf
            }
        }

        // -------------------------------------------------------
        // Codage de corps de ELSE
        // -------------------------------------------------------
        if (EtiqDebutElse) {    // un elseif a cr?e une ?tiquette debut Else
            ilCoder.addLabel(EtiqDebutElse->val.valLabel);    // label fin corps ElseIf
        }
        CodeInstr(InstrCorpsElse);

        // -------------------------------------------------------
        //      label fin Global
        // -------------------------------------------------------
        if (!InstrCorpsElse->estVide() || !listExprElseIf->estVide()){
            ilCoder.addLabel(EtiqFinBloc->val.valLabel);  // label fin corps Else
        }


        break;

    }
}


void Gen68k::GenerCode(){
    ColIterator iter1;
    ColIterator iter2;
    FonctionItem* Fonc1;
    VariableItem* Var1;

    std::ofstream file(outputFileName);
    ilCoder.setStream(&file);

    mNbRegMax=8;
    ilCoder.displayHeader();


    int tailleVarLocales;
    Fonctions->bindIterator(&iter1);
    while(iter1.elemExists()){
        Fonc1=(FonctionItem*)iter1.getNext();
        if (Fonc1->GetUsed()){


            if (Fonc1->GetIsAssembler() ){

            }
            else {
                ilCoder.addLabel(Fonc1->GetNom());
                Stack.ClearStack();
                Fonc1->GetArguListe()->bindIterator(&iter2);
                while (iter2.elemExists()){
                    Var1=(VariableItem*)iter2.getElem();
                    // used? unused?
                    Stack.PushToStack(Var1);
                    iter2.getNext();
                }
                Stack.PushToStack(4);   // l'adresse de retour de la fonction
                //Stack.display();

                tailleVarLocales=0;
                Fonc1->GetVarListe()->bindIterator(&iter2);
                while (iter2.elemExists()){
                    Var1=(VariableItem*)iter2.getElem();
                    Stack.PushToStack(Var1);
                    tailleVarLocales+=Var1->GetSize();
                    iter2.getNext();
                }
                if (tailleVarLocales)
                    ilCoder.add(SUB,ilCoder.createOpVal(tailleVarLocales),ilCoder.createOp(SP_REG),SZ_L);

                Stack.display();

                Collection* InstrListe = Fonc1->GetInstrListe();
                CodeInstr(InstrListe);


                Stack.ClearStack();
                if (tailleVarLocales)
                    ilCoder.add(ADD,ilCoder.createOpVal(tailleVarLocales),ilCoder.createOp(SP_REG),SZ_L);
                ilCoder.add(RTS);
            }
        }
    }// prochaine fonction

    ilCoder.display();
    ilCoder.displayFooter();
    //ilCoder.display("kjdhg");
}

