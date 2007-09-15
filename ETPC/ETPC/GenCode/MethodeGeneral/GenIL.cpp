

#include "GenIL.h"

GenIL::GenIL(void){
	RegPile=new PileRegTemp(&ILcoder,&Stack);
}
GenIL::~GenIL(void){
}

#define max(a,b) (a>b?a:b)

void GenIL::TestGenerate(){
	ILcoder.Add(LOAD,ILcoder.createOp(2,3),ILcoder.createOp(5),SZ_W);
	ILcoder.Afficher();
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
	if (bNoeud->GetNature()==NOEUD_OPERANDE_VARIABLE){
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
	bNoeud->SetNbReg(val);
	return val;
}

int GenIL::NbRegArith(CNoeud* bNoeud,NatureOp bNat){
	NatureNoeud NatureArbre=bNoeud->GetNature();
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
	else if (NatureArbre==NOEUD_OPERATOR && bNoeud->GetOperator()==OPTOR_MOINSUNAIRE)
	{
		val=max(1,NbRegArith(bNoeud,bNat));
	}
	//else if (NatureArbre==NOEUD_OPERATOR && bNoeud->GetOperType()==OPBIN
	
	else if (NatureArbre==NOEUD_OPERATOR && bNoeud->GetOperType()==OPBIN)
	{
		n1=NbRegArith(bNoeud->GetFilsG(),NO_REG);
		n2=NbRegArith(bNoeud->GetFilsD(),NO_DVAL);
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
		for (int i=0;i<bNoeud->GetSuccNmbr();i++){
			n1=NbRegArith(*(bNoeud->GetSuccPtr(i)),NO_REG);
			if (n1>nMax){
				nMax=n1;
			}
		}
		val=nMax;
	}
	
	bNoeud->SetNbReg(val);
	return val;
}
void GenIL::CodeObjet(CNoeud* bNoeud,NatureOp bNat,Operande** Op){
	size_op Size=SZ_UNKNOWN;
	
	if (bNoeud->GetType().Type==TP_INTEGER) Size=SZ_W;
	if (bNoeud->GetType().Type==TP_FLOAT) Size=SZ_F;
	if (bNoeud->GetType().Type==TP_LONG) Size=SZ_L;
	if (bNoeud->GetType().Type==TP_BOOLEAN) Size=SZ_B;
	if (bNoeud->GetType().Type==TP_BYTE) Size=SZ_B;
		
	if (bNoeud->GetNature()==NOEUD_OPERANDE_VARIABLE){
		if (bNat==NO_REG){
			ILcoder.Add(LOAD,ILcoder.createOp(Stack.GetStackPos(bNoeud->GetTAG()->GetIdentif()),SP_REG),RegPile->Sommet(),Size);
			*Op=RegPile->Sommet();
		}
		else if (bNat==NO_DVAL || bNat==NO_DADR){

			*Op=ILcoder.createOp(Stack.GetStackPos(bNoeud->GetTAG()->GetIdentif()),SP_REG);
		}
	}
	else{
		printf("Cas inconnu dans CodeObjet\n");
	}
}

void GenIL::CodeArith(CNoeud* bNoeud,NatureOp bNat,Operande** Op){
	NatureNoeud NatureArbre=bNoeud->GetNature();
	size_op Size=SZ_UNKNOWN;
	Operande* Op1;
	Operande* Op2;
	
	if (bNoeud->GetType().Type==TP_INTEGER) Size=SZ_W;
	if (bNoeud->GetType().Type==TP_FLOAT) Size=SZ_F;
	if (bNoeud->GetType().Type==TP_LONG) Size=SZ_L;
	if (bNoeud->GetType().Type==TP_BOOLEAN) Size=SZ_B;
	if (bNoeud->GetType().Type==TP_BYTE) Size=SZ_B;
	
		
	// A = Litteral
	if (NatureArbre==NOEUD_OPERANDE_CTE){
		if (bNat==NO_REG){
			if (bNoeud->GetTAG()->GetToken()==TOKEN_NOMBRE)
			{
				ILcoder.Add(LOAD,ILcoder.createOpVal(atoi(bNoeud->GetTAG()->GetIdentif())),RegPile->Sommet(),Size);
				
			}
			else if (bNoeud->GetTAG()->GetToken()==TOKEN_STRINGCONSTANT)
			{
				// pas encore géré
			}
			else if (bNoeud->GetTAG()->GetToken()==TOKEN_TRUE)
			{
				ILcoder.Add(LOAD,ILcoder.createOpVal(1),RegPile->Sommet(),Size);

			}
			else if (bNoeud->GetTAG()->GetToken()==TOKEN_FALSE)
			{
				ILcoder.Add(LOAD,ILcoder.createOpVal(0),RegPile->Sommet(),Size);
			}
			else
			{
				printf("ERREUR interne!!!\n");
			}
			*Op=RegPile->Sommet();
		}
		else{
			if (bNoeud->GetTAG()->GetToken()==TOKEN_NOMBRE)
			{
				*Op=ILcoder.createOpVal(atoi(bNoeud->GetTAG()->GetIdentif()));
			}
			else if (bNoeud->GetTAG()->GetToken()==TOKEN_STRINGCONSTANT)
			{
				// pas encore géré
			}
			else if (bNoeud->GetTAG()->GetToken()==TOKEN_TRUE)
			{
				*Op=ILcoder.createOpVal(1);
			}
			else if (bNoeud->GetTAG()->GetToken()==TOKEN_FALSE)
			{
				*Op=ILcoder.createOpVal(0);
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
		CodeObjet(bNoeud,bNat,Op);
	}
	// A = - A1 ou A = conversion(A1)
	else if (NatureArbre==NOEUD_OPERATOR && bNoeud->GetOperator()==OPTOR_MOINSUNAIRE)
	{
		CodeArith(bNoeud->GetFilsD(),NO_DVAL,&Op1);
		ILcoder.Add(OPP,Op1,RegPile->Sommet(),Size);
		*Op=RegPile->Sommet();
	}
	//else if (NatureArbre==NOEUD_OPERATOR && bNoeud->GetOperType()==OPBIN
	
	else if (NatureArbre==NOEUD_OPERATOR && bNoeud->GetOperType()==OPBIN)
	{
		int n1,n2;
		n1=bNoeud->GetFilsG()->GetNbReg();
		n2=bNoeud->GetFilsD()->GetNbReg();
		Operande* T;

		if (n1>=mNbRegMax && n2 >=mNbRegMax){
			CodeArith(bNoeud->GetFilsD(),NO_REG,&Op2);
			T=RegPile->AllouerTemp(Size);
			ILcoder.Add(STORE,Op2,T,Size);
			CodeArith(bNoeud->GetFilsG(),NO_REG,&Op1);
			RegPile->LibererTemp(T,Size);
			ILcoder.Add(ILcoder.NodeToOp(bNoeud),T,Op1,Size);
			delete T;
			*Op=Op1;
		}
		else if (n1>=n2 && n2 < mNbRegMax)
		{
			CodeArith(bNoeud->GetFilsG(),NO_REG,&Op1);
			RegPile->Allouer(Op1);
			CodeArith(bNoeud->GetFilsD(),NO_DVAL,&Op2);
			RegPile->Liberer(Op1);
			ILcoder.Add(ILcoder.NodeToOp(bNoeud),Op2,Op1,Size);
			*Op=Op1;
		}
		else if (n1<n2 && n1 < mNbRegMax)
		{
			RegPile->Echange();
			CodeArith(bNoeud->GetFilsD(),NO_DVAL,&Op2);
			RegPile->Allouer(Op2);
			CodeArith(bNoeud->GetFilsG(),NO_REG,&Op1);
			RegPile->Liberer(Op2);
			RegPile->Echange();
			ILcoder.Add(ILcoder.NodeToOp(bNoeud),Op2,Op1,Size);
			*Op=Op1;
		}
	}
	// A = f(A1,A2, ... , An)
	else if (NatureArbre==NOEUD_OPERANDE_FONCTION)
	{
		
		for (int i=0;i<bNoeud->GetSuccNmbr();i++){
		
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
		ILcoder.AddEtiq(Fonc1->GetNom());
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
		Stack.Afficher();
		if (somme)
			ILcoder.Add(SUB,ILcoder.createOpVal(somme),ILcoder.createOp(SP_REG),SZ_L);
		
		Fonc1->GetInstrListe()->bindIterator(&iter2);
		while(iter2.elemExists()){
			Instr1=(InstructionETPB*)iter2.getNext();
			RegPile->Init(mNbRegMax);
			switch(Instr1->getNat()){
			case INS_AFFECTATION:
				Operande* ExprArbrAff=RegPile->Sommet();
				NbRegArith(Instr1->GetAffectExprArbre(),NO_REG);
				CodeArith(Instr1->GetAffectExprArbre(),NO_REG,&ExprArbrAff);

				break;
			}
		}
		
		ILcoder.Afficher();


		Stack.ClearStack();

	}// prochaine fonction

}

