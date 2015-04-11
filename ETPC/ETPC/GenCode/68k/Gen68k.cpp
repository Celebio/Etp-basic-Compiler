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
#define max3(a,b,c) (max(a,max(b,c)))

Gen68k::Gen68k(void){
    registerStack = new PileRegTemp68k(&ilCoder,&Stack);
}
Gen68k::Gen68k(const char *oFileName){
    sprintf(outputFileName,"%s%s",oFileName,".asm");
    registerStack= new PileRegTemp68k(&ilCoder,&Stack);
}

Gen68k::~Gen68k(void){
}


void Gen68k::testGenerate(){
    //ilCoder.add(LOAD,ilCoder.createOp(2,3),ilCoder.createOp(5),SZ_W);
    ilCoder.display();
}


void Gen68k::setEnvironnement(Collection* BerrListe,
                            Collection* BVariablesPublic,
                            Collection* BTypes,
                            Collection* BFonctions)
{
    errListe=BerrListe;
    VariablesPublic=BVariablesPublic;
    Types=BTypes;
    Fonctions=BFonctions;
}

int Gen68k::getNbRegObject(CNoeud* bNoeud, NatureOp bNat){
    int val;
    if (bNoeud->getNature()==NODE_OPERAND_VARIABLE){
        if (bNat==NO_REG){
            val=1;
        }
        else if (bNat==NO_DVAL || bNat==NO_DADR){
            val=0;
        }
    }
    else{
        printf("Cas inconnu dans getNbRegObject\n");
    }
    bNoeud->setNbReg(val);
    return val;
}

int Gen68k::getNbRegBool(CNoeud* bNoeud){
    bNoeud->display();
    int val=-1;
    TypeOptor operaTOR = bNoeud->getOperator();

    if (operaTOR == OPTOR_CMP_AND || operaTOR == OPTOR_CMP_OR){
        int n1 = getNbRegBool(bNoeud->getLeftChild());
        int n2 = getNbRegBool(bNoeud->getRightChild());
        val = max3(1,n1,n2);
    }
    else if (operaTOR == OPTOR_NOT){
        val = getNbRegBool(bNoeud->getRightChild());
    }
    else if (operaTOR == OPTOR_UNKNOWN){
        if ((bNoeud->getNature()== NODE_OPERAND_CONSTANT && bNoeud->getTag()->GetToken() ==TOKEN_TRUE)
            || (bNoeud->getNature()== NODE_OPERAND_CONSTANT && bNoeud->getTag()->GetToken() ==TOKEN_FALSE)){
            val=1;  // TO DO : ? voir
        }
        // TO DO : autre objets...
    }
    else if (operaTOR == OPTOR_INF || operaTOR == OPTOR_INFEQ || operaTOR == OPTOR_EQUAL ||
        operaTOR == OPTOR_DIFFERENT || operaTOR == OPTOR_SUP || operaTOR == OPTOR_SUPEQ ) {
        int n1 = getNbRegArith(bNoeud->getLeftChild(),NO_REG);
        int n2 = getNbRegArith(bNoeud->getRightChild(),NO_DVAL);
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

int Gen68k::getNbRegArith(CNoeud* bNoeud,NatureOp bNat){
    NodeNature NatureArbre=bNoeud->getNature();
    int val=-1;
    int n1,n2,nMax;
    // A = Litteral
    if (NatureArbre==NODE_OPERAND_CONSTANT){
        if (bNat==NO_REG){
            val=1;
        }
        else{
            val=0;
        }
    }
    // A = objet
    else if (NatureArbre==NODE_OPERAND_VARIABLE)
    {
        val=getNbRegObject(bNoeud,bNat);
    }
    // A = - A1 ou A = conversion(A1)
    else if (NatureArbre==NODE_OPERATOR && bNoeud->getOperator()==OPTOR_MOINSUNAIRE)
    {
        val=max(1,getNbRegArith(bNoeud,bNat));
    }
    //else if (NatureArbre==NODE_OPERATOR && bNoeud->getOperType()==OPBIN

    else if (NatureArbre==NODE_OPERATOR && bNoeud->getOperType()==OPBIN)
    {
        n1=getNbRegArith(bNoeud->getLeftChild(),NO_REG);
        n2=getNbRegArith(bNoeud->getRightChild(),NO_DVAL);
        if (n1==n2){
            val=n1+1;
        }else{
            val=max(n1,n2);
        }
    }
    // A = f(A1,A2, ... , An)
    else if (NatureArbre==NODE_OPERAND_FUNCTION)
    {
        nMax=1;
        for (int i=0;i<bNoeud->getSuccNmbr();i++){
            n1=getNbRegArith(*(bNoeud->getSuccPtr(i)),NO_REG);
            if (n1>nMax){
                nMax=n1;
            }
        }
        val=nMax;
    }

    bNoeud->setNbReg(val);
    return val;
}
void Gen68k::codeObject(CNoeud* bNoeud,NatureOp bNat,Operande68k** opertr){
    size_op68k size=getSize(bNoeud);

    if (bNoeud->getNature()==NODE_OPERAND_VARIABLE){
        if (bNat==NO_REG){
            ilCoder.add(MOVE,ilCoder.createOp(Stack.getStackPos(bNoeud->getTag()->GetIdentif()),SP_REG),registerStack->front(),size);
            *opertr=registerStack->front();
        }
        else if (bNat==NO_DVAL || bNat==NO_DADR){

            *opertr=ilCoder.createOp(Stack.getStackPos(bNoeud->getTag()->GetIdentif()),SP_REG);
        }
    }
    else{
        printf("Cas inconnu dans codeObject\n");
    }
}

size_op68k Gen68k::getSize(CNoeud* bNoeud){
    size_op68k size=SZ_UNKNOWN;

    if (bNoeud->getType().Type==TP_INTEGER) size=SZ_W;
    if (bNoeud->getType().Type==TP_FLOAT) size=SZ_F;
    if (bNoeud->getType().Type==TP_LONG) size=SZ_L;
    if (bNoeud->getType().Type==TP_BOOLEAN) size=SZ_W;  // et pas size=SZ_B;
    if (bNoeud->getType().Type==TP_BYTE) size=SZ_W;     // et pas size=SZ_B;
    return size;
}

void Gen68k::codeArith(CNoeud* bNoeud,NatureOp bNat,Operande68k** opertr){
    NodeNature NatureArbre=bNoeud->getNature();
    size_op68k size=getSize(bNoeud);
    Operande68k* Op1;
    Operande68k* Op2;
    //printf("entree dans codearith avec bNoeud:\n");
    //bNoeud->display();

    // A = Litteral
    if (NatureArbre==NODE_OPERAND_CONSTANT){
        if (bNat==NO_REG){
            if (bNoeud->getTag()->GetToken()==TOKEN_NOMBRE)
            {
                ilCoder.add(MOVE,ilCoder.createOpVal(atoi(bNoeud->getTag()->GetIdentif())),registerStack->front(),size);

            }
            else if (bNoeud->getTag()->GetToken()==TOKEN_STRINGCONSTANT)
            {
                // pas encore g?r?
            }
            else if (bNoeud->getTag()->GetToken()==TOKEN_TRUE)
            {
                ilCoder.add(MOVE,ilCoder.createOpVal(1),registerStack->front(),size);

            }
            else if (bNoeud->getTag()->GetToken()==TOKEN_FALSE)
            {
                ilCoder.add(MOVE,ilCoder.createOpVal(0),registerStack->front(),size);
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
    else if (NatureArbre==NODE_OPERAND_VARIABLE)
    {
        codeObject(bNoeud,bNat,opertr);
    }
    // A = - A1 ou A = conversion(A1)
    else if (NatureArbre==NODE_OPERATOR && bNoeud->getOperator()==OPTOR_MOINSUNAIRE)
    {
        codeArith(bNoeud->getRightChild(),NO_DVAL,&Op1);
        ilCoder.add(OPP,Op1,registerStack->front(),size);
        *opertr=registerStack->front();
    }
    // A = A1 opbin A2 et op_bin commutatif et A1 est litteral ou objet
    // et A2 n'est ni litteral ni objet
    else if (NatureArbre==NODE_OPERATOR && bNoeud->getOperType()==OPBIN &&
            bNoeud->isCommutative() &&
             (bNoeud->getLeftChild()->getNature()==NODE_OPERAND_CONSTANT ||
             bNoeud->getLeftChild()->getNature()==NODE_OPERAND_VARIABLE)
             &&
            (bNoeud->getRightChild()->getNature()!=NODE_OPERAND_CONSTANT &&
             bNoeud->getRightChild()->getNature()!=NODE_OPERAND_VARIABLE))
    {
        CNoeud* aux=bNoeud->getRightChild();
        bNoeud->setRightChild(bNoeud->getLeftChild());
        bNoeud->setLeftChild(aux);
        codeArith(bNoeud, bNat, opertr);
    }

    else if (NatureArbre==NODE_OPERATOR && bNoeud->getOperType()==OPBIN)
    {
        int n1,n2;
        n1=bNoeud->getLeftChild()->getNbReg();
        n2=bNoeud->getRightChild()->getNbReg();
        Operande68k* T;

        if (n1>=mNbRegMax && n2 >=mNbRegMax){
            codeArith(bNoeud->getRightChild(),NO_REG,&Op2);
            T=registerStack->allocateTemp(size);
            ilCoder.add(MOVE,Op2,T,size);
            codeArith(bNoeud->getLeftChild(),NO_REG,&Op1);
            registerStack->freeTemp(T,size);
            ilCoder.add(ilCoder.nodeToOp(bNoeud),T,Op1,size);
            delete T;
            *opertr=Op1;
        }
        else if (n1>=n2 && n2 < mNbRegMax)
        {
            codeArith(bNoeud->getLeftChild(),NO_REG,&Op1);
            registerStack->allocate(Op1);
            codeArith(bNoeud->getRightChild(),NO_DVAL,&Op2);
            registerStack->freeOperand(Op1);
            ilCoder.add(ilCoder.nodeToOp(bNoeud),Op2,Op1,size);
            *opertr=Op1;
        }
        else if (n1<n2 && n1 < mNbRegMax)
        {
            registerStack->switchD();
            codeArith(bNoeud->getRightChild(),NO_DVAL,&Op2);
            registerStack->allocate(Op2);
            codeArith(bNoeud->getLeftChild(),NO_REG,&Op1);
            registerStack->freeOperand(Op2);
            registerStack->switchD();
            ilCoder.add(ilCoder.nodeToOp(bNoeud),Op2,Op1,size);
            *opertr=Op1;
        }
    }
    // A = f(A1,A2, ... , An)
    else if (NatureArbre==NODE_OPERAND_FUNCTION)
    {
        int somme=0;        // la somme des tailles de ce qui est pouss? dans la pile
        size_op68k SizeOfArgument;
        for (int i=bNoeud->getSuccNmbr()-1;i>=0;i--){
            SizeOfArgument=getSize(*bNoeud->getSuccPtr(i));
            somme+=(int)SizeOfArgument;
            codeArith(*bNoeud->getSuccPtr(i),NO_REG,&Op2);
            ilCoder.add(MOVE,Op2,ilCoder.createOp(A7,false,true),SizeOfArgument);
            Stack.pushToStack(SizeOfArgument);
        }
        ilCoder.add(BSR,ilCoder.createOpLabel(bNoeud->getTag()->GetIdentif()),SZ_NA);
        ilCoder.add(ADD,ilCoder.createOpVal(somme),ilCoder.createOp(SP_REG),SZ_L);

        //Stack.display();
        for (int i=bNoeud->getSuccNmbr()-1;i>=0;i--){
            Stack.pop();
            //Stack.display();
        }
        ilCoder.add(MOVE,ilCoder.createOp(D0),registerStack->front(),size);
        *opertr=registerStack->front();
    }
}

void Gen68k::codeBool(CNoeud* bNoeud,bool AvecSaut,bool ValSaut,Operande68k* etiqSaut,bool DansReg,bool ValReg){
    //printf("Entree dans COdeBOOl\n");
    //printf("expression booleenne:\n");
    //bNoeud->display();

    size_op68k size=getSize(bNoeud);
    Operande68k* T;

    Operande68k* Etiq = ilCoder.createOpLabel();
    Operande68k* Etiq1 = ilCoder.createOpLabel();
    //Operande68k* Etiq2 = ilCoder.createOpLabel();

    TypeOptor operaTOR = bNoeud->getOperator();

    if (operaTOR == OPTOR_CMP_AND){
        if (AvecSaut==true){
            if (ValSaut==true){
                codeBool(bNoeud->getLeftChild(),true,false,Etiq1,DansReg,ValReg);
                codeBool(bNoeud->getRightChild(),true,true,etiqSaut,DansReg,ValReg);
                ilCoder.addLabel(Etiq1->val.valLabel);
            }
            else {  // ValSaut==false
                codeBool(bNoeud->getLeftChild(),true,false,etiqSaut,DansReg,ValReg);
                codeBool(bNoeud->getRightChild(),true,false,etiqSaut,DansReg,ValReg);
            }
        }
        else {      // AvecSaut==false
            codeBool(bNoeud->getLeftChild(),true,false,Etiq1,true,ValReg);
            codeBool(bNoeud->getRightChild(),false,false,NULL,true,ValReg);  // le deuxieme false n'a aucun impact normalement
            ilCoder.addLabel(Etiq1->val.valLabel);
        }
    }
    else if (operaTOR == OPTOR_CMP_OR){
        if (AvecSaut==true){
            if (ValSaut==true){
                codeBool(bNoeud->getLeftChild(),true,true,etiqSaut,DansReg,ValReg);
                codeBool(bNoeud->getRightChild(),true,true,etiqSaut,DansReg,ValReg);
            }
            else {  // ValSaut==false
                codeBool(bNoeud->getLeftChild(),true,true,Etiq1,DansReg,ValReg);
                codeBool(bNoeud->getRightChild(),true,false,etiqSaut,DansReg,ValReg);
                ilCoder.addLabel(Etiq1->val.valLabel);
            }
        }
        else {      // AvecSaut==false
            codeBool(bNoeud->getLeftChild(),true,true,Etiq1,true,ValReg);
            codeBool(bNoeud->getRightChild(),false,false,NULL,true,ValReg);  // le deuxieme false n'a aucun impact normalement
            ilCoder.addLabel(Etiq1->val.valLabel);
        }
    }
    else if (operaTOR == OPTOR_NOT){
        codeBool(bNoeud->getRightChild(),AvecSaut,!ValSaut,etiqSaut,DansReg,!ValReg);
    }
    else if (operaTOR == OPTOR_UNKNOWN){
        if (bNoeud->getNature()== NODE_OPERAND_CONSTANT && bNoeud->getTag()->GetToken() ==TOKEN_TRUE){
            Operande68k* ValRegOp;
            ValRegOp = ilCoder.createOpVal( ValReg ? 1 : 0 );
            if (AvecSaut==true){
                ilCoder.add(MOVE,ValRegOp,registerStack->front(),SZ_B);
                ilCoder.add(BRA,etiqSaut,SZ_NA);
            }
            else {  // AvecSaut==false
                ilCoder.add(MOVE,ValRegOp,registerStack->front(),SZ_B);
            }
        } else if (bNoeud->getNature()== NODE_OPERAND_CONSTANT && bNoeud->getTag()->GetToken() ==TOKEN_FALSE){
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
        int n1 = getNbRegArith(bNoeud->getLeftChild(),NO_REG);
        int n2 = getNbRegArith(bNoeud->getRightChild(),NO_DVAL);
        Operande68k* Op2;
        Operande68k* Op1;
        if (n1 >= mNbRegMax && n2 >= mNbRegMax) {
            codeArith(bNoeud->getRightChild(),NO_REG,&Op2);
            T=registerStack->allocateTemp(size);
            ilCoder.add(MOVE,Op2,T,size);
            codeArith(bNoeud->getLeftChild(),NO_REG,&Op1);
            registerStack->freeTemp(T,size);
            ilCoder.add(CMP,T,Op1,getSize(bNoeud->getRightChild()));
        }
        else if (n1>=n2 && n2 < mNbRegMax)
        {
            codeArith(bNoeud->getLeftChild(),NO_REG,&Op1);
            registerStack->allocate(Op1);
            codeArith(bNoeud->getRightChild(),NO_DVAL,&Op2);
            registerStack->freeOperand(Op1);
            ilCoder.add(CMP,Op2,Op1,getSize(bNoeud->getRightChild()));
        }
        else if (n1<n2 && n1 < mNbRegMax)
        {
            registerStack->switchD();
            codeArith(bNoeud->getRightChild(),NO_DVAL,&Op2);
            registerStack->allocate(Op2);
            codeArith(bNoeud->getLeftChild(),NO_REG,&Op1);
            registerStack->freeOperand(Op2);
            registerStack->switchD();
            ilCoder.add(CMP,Op2,Op1,getSize(bNoeud->getRightChild()));
        }

        if (AvecSaut){
            // Scc
            /*
            if (ValReg==true){
                ilCoder.add((InsOpEnum68k)(ilCoder.nodeToOp(bNoeud)+(SEQ-BEQ)),Op1,size);
            } else {        // sinon l'oppos?
                ilCoder.add((InsOpEnum68k)(ilCoder.getOppBra(ilCoder.nodeToOp(bNoeud))+(SEQ-BEQ)),Op1,size);
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
                ilCoder.add((InsOpEnum68k)(ilCoder.nodeToOp(bNoeud)+(SEQ-BEQ)),Op1,size);
            } else {        // sinon l'oppos?
                ilCoder.add((InsOpEnum68k)(ilCoder.getOppBra(ilCoder.nodeToOp(bNoeud))+(SEQ-BEQ)),Op1,size);
            }*/
        }
    }
    else{
        printf("je suis la\n");
    }

}


void Gen68k::codeInstr(Collection* bInstrSuite){
    ColIterator iter2;
    bInstrSuite->bindIterator(&iter2);
    InstructionETPB* Instr1;

    while(iter2.elemExists()){
        Instr1=(InstructionETPB*)iter2.getNext();
        codeInstr(Instr1);
    }
}
void Gen68k::codeInstr(InstructionETPB* bInstr){



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

        nG=getNbRegArith(bInstr->getAssignmentExprAssigned(),NO_REG);
        nD=getNbRegArith(bInstr->getAssignmentExprTree(),NO_REG);

        if (nD >= nG){
            codeArith(bInstr->getAssignmentExprTree(),NO_REG,&ExprArbrAff);
            registerStack->allocate(ExprArbrAff);
            codeArith(bInstr->getAssignmentExprAssigned(),NO_DADR,&ExprArbrAffected);
            registerStack->freeOperand(ExprArbrAff);
            ilCoder.add(MOVE,ExprArbrAff,ExprArbrAffected,getSize(bInstr->getAssignmentExprAssigned()));
        }
        else{
            codeArith(bInstr->getAssignmentExprAssigned(),NO_DADR,&ExprArbrAffected);
            registerStack->allocate(ExprArbrAffected);
            codeArith(bInstr->getAssignmentExprTree(),NO_REG,&ExprArbrAff);
            registerStack->freeOperand(ExprArbrAffected);
            ilCoder.add(MOVE,ExprArbrAff,ExprArbrAffected,getSize(bInstr->getAssignmentExprAssigned()));
        }
        break;
    case INS_RETURN:
        Operande68k* OpResReturn;
        if (bInstr->getExprReturn()){
            codeArith(bInstr->getExprReturn(),NO_REG,&OpResReturn);
            ilCoder.add(MOVE,OpResReturn,ilCoder.createOp(D0),getSize(bInstr->getExprReturn()));
        }
        ilCoder.add(RTS);
        break;
    case INS_CALL:
        int somme;
        CNoeud* CallNoeud;

        somme=0;        // la somme des tailles de ce qui est pouss? dans la pile
        size_op68k SizeOfArgument;
        CallNoeud=bInstr->getExprFunctionCall();
        Operande68k* Op2;
        for (int i=CallNoeud->getSuccNmbr()-1;i>=0;i--){
            SizeOfArgument=getSize(*CallNoeud->getSuccPtr(i));
            somme+=(int)SizeOfArgument;
            codeArith(*CallNoeud->getSuccPtr(i),NO_REG,&Op2);
            ilCoder.add(MOVE,Op2,ilCoder.createOp(A7,false,true),SizeOfArgument);
            Stack.pushToStack(SizeOfArgument);
        }
        ilCoder.add(BSR,ilCoder.createOpLabel(CallNoeud->getTag()->GetIdentif()),SZ_NA);
        if (somme){
            ilCoder.add(ADD,ilCoder.createOpVal(somme),ilCoder.createOp(SP_REG),SZ_L);
        }
        //Stack.display();
        for (int i=CallNoeud->getSuccNmbr()-1;i>=0;i--){
            Stack.pop();
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

        initExpr = bInstr->getForExprInitTree();
        finExpr = bInstr->getForExprToTree();
        stepExpr = bInstr->getForExprStepTree();
        varExpr = bInstr->getForExprAssigned();
        CorpsFor = bInstr->getForBody();
        SizeVarIter =getSize(varExpr);
        EtiqForDebTest = ilCoder.createOpLabel();
        EtiqForFin = ilCoder.createOpLabel();

        // ---------------------------------------------------
        // Affectation initiale
        // ---------------------------------------------------
        nG=getNbRegArith(varExpr,NO_REG);
        nD=getNbRegArith(initExpr,NO_REG);

        if (nD >= nG){
            codeArith(initExpr,NO_REG,&ExprArbrAff);
            registerStack->allocate(ExprArbrAff);
            codeArith(varExpr,NO_DADR,&ExprArbrAffected);
            registerStack->freeOperand(ExprArbrAff);
            ilCoder.add(MOVE,ExprArbrAff,ExprArbrAffected,SizeVarIter);
        }
        else{
            codeArith(varExpr,NO_DADR,&ExprArbrAffected);
            registerStack->allocate(ExprArbrAffected);
            codeArith(initExpr,NO_REG,&ExprArbrAff);
            registerStack->freeOperand(ExprArbrAffected);
            ilCoder.add(MOVE,ExprArbrAff,ExprArbrAffected,SizeVarIter);
        }

        // ---------------------------------------------------
        // Evaluation de l'expression d'arriv?e
        // ---------------------------------------------------

        // temporaire sur la pile qui va contenir la valeur de l'expression d'arriv?e TO
        TexpTO=registerStack->allocateTemp(SizeVarIter);
        codeArith(finExpr,NO_REG,&ExprArbrAff);
        //registerStack->allocate(ExprArbrAff);
        ilCoder.add(MOVE,ExprArbrAff,TexpTO,SizeVarIter);
        //registerStack->freeOperand(ExprArbrAff);


        // ---------------------------------------------------
        // Test de la condition d'arriv?
        // ---------------------------------------------------
        ilCoder.addLabel(EtiqForDebTest->val.valLabel);

        // cas 1: pas de step
        if (stepExpr == NULL){
            codeArith(varExpr,NO_REG,&ExprArbrAffected);
            ilCoder.add(CMP,TexpTO,ExprArbrAffected,SizeVarIter);
            ilCoder.add(BGT,EtiqForFin,SZ_NA);
            codeInstr(CorpsFor);
            // iterateur = iterateur+1 sous entendu
            codeArith(varExpr,NO_DADR,&ExprArbrAffected);
            ilCoder.add(ADD,ilCoder.createOpVal(1),ExprArbrAffected,SizeVarIter);
            ilCoder.add(BRA,EtiqForDebTest,SZ_NA);
        }
        // cas 2: step constante
        else if (stepExpr->isConstant()){
            int constStepVal;
            constStepVal = atoi(stepExpr->getTag()->GetIdentif());
            codeArith(varExpr,NO_REG,&ExprArbrAffected);
            ilCoder.add(CMP,TexpTO,ExprArbrAffected,SizeVarIter);
            if (constStepVal < 0){
                ilCoder.add(BLT,EtiqForFin,SZ_NA);
            }
            else {
                ilCoder.add(BGT,EtiqForFin,SZ_NA);
            }
            codeInstr(CorpsFor);
            // iterateur = iterateur+cte sous entendu
            codeArith(varExpr,NO_DADR,&ExprArbrAffected);
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
        CorpsDo = bInstr->getDoBody();
        exprDo = bInstr->getDoExprCondition();
        EtiqDebut = ilCoder.createOpLabel();
        ilCoder.addLabel(EtiqDebut->val.valLabel);
        codeInstr(CorpsDo);
        codeBool(exprDo,true,true,EtiqDebut,false,false);
        break;
    case INS_STRUCT_DOWH:       // do while (condition) .... loop
        Collection* CorpsWhile;
        CNoeud* exprWhile;
        Operande68k* EtiqWhileDebut;
        Operande68k* EtiqWhileFin;

        CorpsWhile = bInstr->getDoBody();
        exprWhile = bInstr->getDoExprCondition();
        EtiqWhileDebut = ilCoder.createOpLabel();
        EtiqWhileFin = ilCoder.createOpLabel();      // juste avant le test de la condition

        ilCoder.add(BRA,EtiqWhileFin,SZ_NA);        // on va directement voir le test
        ilCoder.addLabel(EtiqWhileDebut->val.valLabel);
        codeInstr(CorpsWhile);
        ilCoder.addLabel(EtiqWhileFin->val.valLabel);
        codeBool(exprWhile,true,true,EtiqWhileDebut,false,false);

        break;
    case INS_IF:
        CNoeud* exprIf;
        exprIf = bInstr->getIfExprTree();
        //printf("codeInstr IF\n");
        //printf("Expression du If:\n");
        //exprIf->display();

        Operande68k* EtiqFinCorpsIF = ilCoder.createOpLabel();
        Operande68k* EtiqFinLocalCorpsElseIf;
        Operande68k* EtiqDebutElse=NULL;    // utilis? s'il y a des elseIf et un else
        Operande68k* EtiqFinBloc;


        Collection* InstrCorpsIf = bInstr->getIfIfBody();
        Collection* InstrCorpsElse = bInstr->getIfElseBody();
        Collection* listExprElseIf = bInstr->getIfExprElseIf();
        Collection* listCorpsElseIf = bInstr->getIfElseIfBody();

        ColIterator iterExprElseIf;
        ColIterator iterCorpsElseIf;

        listExprElseIf->bindIterator(&iterExprElseIf);
        listCorpsElseIf->bindIterator(&iterCorpsElseIf);
        Collection* InstrCorpsElseIf;
        CNoeud* exprElseIf;

        // -------------------------------------------------------
        // Codage de expression de IF
        // -------------------------------------------------------
        codeBool(exprIf,true,false,EtiqFinCorpsIF,false,false); // le dernier false n'a aucun impact normalement

        // -------------------------------------------------------
        // Codage de corps de IF
        // -------------------------------------------------------
        codeInstr(InstrCorpsIf);
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
            codeBool(exprElseIf,true,false,EtiqFinLocalCorpsElseIf,false,false);

            // -------------------------------------------------------
            // Codage de corps de ElseIF
            // -------------------------------------------------------
            codeInstr(InstrCorpsElseIf);

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
        codeInstr(InstrCorpsElse);

        // -------------------------------------------------------
        //      label fin Global
        // -------------------------------------------------------
        if (!InstrCorpsElse->estVide() || !listExprElseIf->estVide()){
            ilCoder.addLabel(EtiqFinBloc->val.valLabel);  // label fin corps Else
        }


        break;

    }
}


void Gen68k::generateCode(){
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
        if (Fonc1->isUsed()){


            if (Fonc1->isAssembler() ){

            }
            else {
                ilCoder.addLabel(Fonc1->getName());
                Stack.clearStack();
                Fonc1->getArgumentList()->bindIterator(&iter2);
                while (iter2.elemExists()){
                    Var1=(VariableItem*)iter2.getElem();
                    // used? unused?
                    Stack.pushToStack(Var1);
                    iter2.getNext();
                }
                Stack.pushToStack(4);   // l'adresse de retour de la fonction
                //Stack.display();

                tailleVarLocales=0;
                Fonc1->getVariableList()->bindIterator(&iter2);
                while (iter2.elemExists()){
                    Var1=(VariableItem*)iter2.getElem();
                    Stack.pushToStack(Var1);
                    tailleVarLocales+=Var1->getSize();
                    iter2.getNext();
                }
                if (tailleVarLocales)
                    ilCoder.add(SUB,ilCoder.createOpVal(tailleVarLocales),ilCoder.createOp(SP_REG),SZ_L);

                Stack.display();

                Collection* instructionList = Fonc1->getInstructionList();
                codeInstr(instructionList);


                Stack.clearStack();
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

