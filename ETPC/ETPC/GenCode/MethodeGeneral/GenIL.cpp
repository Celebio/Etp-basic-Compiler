

#include "GenIL.h"

GenIL::GenIL(void){
    registerStack=new PileRegTemp(&ilCoder,&Stack);
}
GenIL::~GenIL(void){
}

#define max(a,b) (a>b?a:b)

void GenIL::TestGenerate(){
    ilCoder.add(LOAD,ilCoder.createOp(2,3),ilCoder.createOp(5),SZ_W);
    ilCoder.display();
}


void GenIL::SetEnvironnement(Collection* BerrListe,
                            Collection* BVariablesPublic,
                            Collection* BTypes,
                            Collection* BFonctions)
{
    errListe=BerrListe;
    VariablesPublic=BVariablesPublic;
    Types=BTypes;
    Fonctions=BFonctions;
}

int GenIL::NbRegObjet(CNoeud* bNoeud,NatureOp bNat){
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

int GenIL::NbRegArith(CNoeud* bNoeud,NatureOp bNat){
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
void GenIL::CodeObjet(CNoeud* bNoeud,NatureOp bNat,Operande** opertr){
    size_op Size=SZ_UNKNOWN;

    if (bNoeud->getType().Type==TP_INTEGER) Size=SZ_W;
    if (bNoeud->getType().Type==TP_FLOAT) Size=SZ_F;
    if (bNoeud->getType().Type==TP_LONG) Size=SZ_L;
    if (bNoeud->getType().Type==TP_BOOLEAN) Size=SZ_B;
    if (bNoeud->getType().Type==TP_BYTE) Size=SZ_B;

    if (bNoeud->getNature()==NOEUD_OPERANDE_VARIABLE){
        if (bNat==NO_REG){
            ilCoder.add(LOAD,ilCoder.createOp(Stack.GetStackPos(bNoeud->getTag()->GetIdentif()),SP_REG),registerStack->front(),Size);
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

void GenIL::CodeArith(CNoeud* bNoeud,NatureOp bNat,Operande** opertr){
    NatureNoeud NatureArbre=bNoeud->getNature();
    size_op Size=SZ_UNKNOWN;
    Operande* Op1;
    Operande* Op2;

    if (bNoeud->getType().Type==TP_INTEGER) Size=SZ_W;
    if (bNoeud->getType().Type==TP_FLOAT) Size=SZ_F;
    if (bNoeud->getType().Type==TP_LONG) Size=SZ_L;
    if (bNoeud->getType().Type==TP_BOOLEAN) Size=SZ_B;
    if (bNoeud->getType().Type==TP_BYTE) Size=SZ_B;


    // A = Litteral
    if (NatureArbre==NOEUD_OPERANDE_CTE){
        if (bNat==NO_REG){
            if (bNoeud->getTag()->GetToken()==TOKEN_NOMBRE)
            {
                ilCoder.add(LOAD,ilCoder.createOpVal(atoi(bNoeud->getTag()->GetIdentif())),registerStack->front(),Size);

            }
            else if (bNoeud->getTag()->GetToken()==TOKEN_STRINGCONSTANT)
            {
                // pas encore g?r?
            }
            else if (bNoeud->getTag()->GetToken()==TOKEN_TRUE)
            {
                ilCoder.add(LOAD,ilCoder.createOpVal(1),registerStack->front(),Size);

            }
            else if (bNoeud->getTag()->GetToken()==TOKEN_FALSE)
            {
                ilCoder.add(LOAD,ilCoder.createOpVal(0),registerStack->front(),Size);
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
    //else if (NatureArbre==NOEUD_OPERATOR && bNoeud->getOperType()==OPBIN

    else if (NatureArbre==NOEUD_OPERATOR && bNoeud->getOperType()==OPBIN)
    {
        int n1,n2;
        n1=bNoeud->getLeftChild()->getNbReg();
        n2=bNoeud->getRightChild()->getNbReg();
        Operande* T;

        if (n1>=mNbRegMax && n2 >=mNbRegMax){
            CodeArith(bNoeud->getRightChild(),NO_REG,&Op2);
            T=registerStack->allocateTemp(Size);
            ilCoder.add(STORE,Op2,T,Size);
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
            registerStack->Echange();
            CodeArith(bNoeud->getRightChild(),NO_DVAL,&Op2);
            registerStack->allocate(Op2);
            CodeArith(bNoeud->getLeftChild(),NO_REG,&Op1);
            registerStack->freeOperand(Op2);
            registerStack->Echange();
            ilCoder.add(ilCoder.nodeToOp(bNoeud),Op2,Op1,Size);
            *opertr=Op1;
        }
    }
    // A = f(A1,A2, ... , An)
    else if (NatureArbre==NOEUD_OPERANDE_FONCTION)
    {

        for (int i=0;i<bNoeud->getSuccNmbr();i++){

        }
    }
}


void GenIL::GenerCode(){
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
        Stack.ClearStack();
        ilCoder.addLabel(Fonc1->GetNom());
        Fonc1->GetArguListe()->bindIterator(&iter2);
        while (iter2.elemExists()){
            Var1=(VariableItem*)iter2.getElem();
            Stack.PushToStack(Var1);
            iter2.getNext();
        }
        somme=0;
        Fonc1->GetVarListe()->bindIterator(&iter2);
        while (iter2.elemExists()){
            Var1=(VariableItem*)iter2.getElem();
            Stack.PushToStack(Var1);
            somme+=Var1->GetSize();
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
                NbRegArith(Instr1->GetAffectExprArbre(),NO_REG);
                CodeArith(Instr1->GetAffectExprArbre(),NO_REG,&ExprArbrAff);

                break;
            }
        }

        ilCoder.display();


        Stack.ClearStack();

    }// prochaine fonction

}

