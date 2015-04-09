

#include "GenIL.h"

GenIL::GenIL(void){
    registerStack=new PileRegTemp(&ilCoder,&Stack);
}
GenIL::~GenIL(void){
}

#define max(a,b) (a>b?a:b)

void GenIL::testGenerate(){
    ilCoder.add(LOAD,ilCoder.createOp(2,3),ilCoder.createOp(5),SZ_W);
    ilCoder.display();
}


void GenIL::setEnvironnement(Collection* BerrListe,
                            Collection* BVariablesPublic,
                            Collection* BTypes,
                            Collection* BFonctions)
{
    errListe=BerrListe;
    VariablesPublic=BVariablesPublic;
    Types=BTypes;
    Fonctions=BFonctions;
}

int GenIL::getNbRegObject(CNoeud* bNoeud,NatureOp bNat){
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

int GenIL::getNbRegArith(CNoeud* bNoeud,NatureOp bNat){
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
void GenIL::codeObject(CNoeud* bNoeud,NatureOp bNat,Operande** opertr){
    size_op size=SZ_UNKNOWN;

    if (bNoeud->getType().Type==TP_INTEGER) size=SZ_W;
    if (bNoeud->getType().Type==TP_FLOAT) size=SZ_F;
    if (bNoeud->getType().Type==TP_LONG) size=SZ_L;
    if (bNoeud->getType().Type==TP_BOOLEAN) size=SZ_B;
    if (bNoeud->getType().Type==TP_BYTE) size=SZ_B;

    if (bNoeud->getNature()==NODE_OPERAND_VARIABLE){
        if (bNat==NO_REG){
            ilCoder.add(LOAD,ilCoder.createOp(Stack.getStackPos(bNoeud->getTag()->GetIdentif()),SP_REG),registerStack->front(),size);
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

void GenIL::codeArith(CNoeud* bNoeud,NatureOp bNat,Operande** opertr){
    NodeNature NatureArbre=bNoeud->getNature();
    size_op size=SZ_UNKNOWN;
    Operande* Op1;
    Operande* Op2;

    if (bNoeud->getType().Type==TP_INTEGER) size=SZ_W;
    if (bNoeud->getType().Type==TP_FLOAT) size=SZ_F;
    if (bNoeud->getType().Type==TP_LONG) size=SZ_L;
    if (bNoeud->getType().Type==TP_BOOLEAN) size=SZ_B;
    if (bNoeud->getType().Type==TP_BYTE) size=SZ_B;


    // A = Litteral
    if (NatureArbre==NODE_OPERAND_CONSTANT){
        if (bNat==NO_REG){
            if (bNoeud->getTag()->GetToken()==TOKEN_NOMBRE)
            {
                ilCoder.add(LOAD,ilCoder.createOpVal(atoi(bNoeud->getTag()->GetIdentif())),registerStack->front(),size);

            }
            else if (bNoeud->getTag()->GetToken()==TOKEN_STRINGCONSTANT)
            {
                // pas encore g?r?
            }
            else if (bNoeud->getTag()->GetToken()==TOKEN_TRUE)
            {
                ilCoder.add(LOAD,ilCoder.createOpVal(1),registerStack->front(),size);

            }
            else if (bNoeud->getTag()->GetToken()==TOKEN_FALSE)
            {
                ilCoder.add(LOAD,ilCoder.createOpVal(0),registerStack->front(),size);
            }
            else
            {
                printf("ERREUR interne!!!\n");
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
                printf("ERREUR interne!!!\n");
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
    //else if (NatureArbre==NODE_OPERATOR && bNoeud->getOperType()==OPBIN

    else if (NatureArbre==NODE_OPERATOR && bNoeud->getOperType()==OPBIN)
    {
        int n1,n2;
        n1=bNoeud->getLeftChild()->getNbReg();
        n2=bNoeud->getRightChild()->getNbReg();
        Operande* T;

        if (n1>=mNbRegMax && n2 >=mNbRegMax){
            codeArith(bNoeud->getRightChild(),NO_REG,&Op2);
            T=registerStack->allocateTemp(size);
            ilCoder.add(STORE,Op2,T,size);
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
            registerStack->Echange();
            codeArith(bNoeud->getRightChild(),NO_DVAL,&Op2);
            registerStack->allocate(Op2);
            codeArith(bNoeud->getLeftChild(),NO_REG,&Op1);
            registerStack->freeOperand(Op2);
            registerStack->Echange();
            ilCoder.add(ilCoder.nodeToOp(bNoeud),Op2,Op1,size);
            *opertr=Op1;
        }
    }
    // A = f(A1,A2, ... , An)
    else if (NatureArbre==NODE_OPERAND_FUNCTION)
    {

        for (int i=0;i<bNoeud->getSuccNmbr();i++){

        }
    }
}


void GenIL::generateCode(){
    ColIterator iter1;
    ColIterator iter2;
    FonctionItem* Fonc1;
    VariableItem* Var1;
    InstructionETPB* Instr1;

    mNbRegMax=4;

    int somme;
    Fonctions->bindIterator(&iter1);
    while(iter1.elemExists()){
        Fonc1=(FonctionItem*)iter1.getNext();
        Stack.clearStack();
        ilCoder.addLabel(Fonc1->GetNom());
        Fonc1->GetArguListe()->bindIterator(&iter2);
        while (iter2.elemExists()){
            Var1=(VariableItem*)iter2.getElem();
            Stack.pushToStack(Var1);
            iter2.getNext();
        }
        somme=0;
        Fonc1->GetVarListe()->bindIterator(&iter2);
        while (iter2.elemExists()){
            Var1=(VariableItem*)iter2.getElem();
            Stack.pushToStack(Var1);
            somme+=Var1->getSize();
            iter2.getNext();
        }
        Stack.display();
        if (somme)
            ilCoder.add(SUB,ilCoder.createOpVal(somme),ilCoder.createOp(SP_REG),SZ_L);

        Fonc1->GetInstrListe()->bindIterator(&iter2);
        while(iter2.elemExists()){
            Instr1=(InstructionETPB*)iter2.getNext();
            registerStack->init(mNbRegMax);
            switch(Instr1->getNat()){
            case INS_AFFECTATION:
                Operande* ExprArbrAff=registerStack->front();
                getNbRegArith(Instr1->GetAffectExprArbre(),NO_REG);
                codeArith(Instr1->GetAffectExprArbre(),NO_REG,&ExprArbrAff);

                break;
            }
        }

        ilCoder.display();


        Stack.clearStack();

    }// prochaine fonction

}

