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
#define max3(a,b,c)  ( a>b ? (max(a,c)) : (max(b,c)) )


Gen68k::Gen68k(void){
	RegPile=new PileRegTemp68k(&ILcoder,&Stack);
}
Gen68k::Gen68k(char *oFileName){
	sprintf(outputFileName,"%s%s",oFileName,".asm");
	RegPile=new PileRegTemp68k(&ILcoder,&Stack);
}

Gen68k::~Gen68k(void){
}


void Gen68k::TestGenerate(){
	//ILcoder.Add(LOAD,ILcoder.createOp(2,3),ILcoder.createOp(5),SZ_W);
	ILcoder.Afficher();
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

int Gen68k::NbRegObjet(CNoeud* bNoeud,NatureOp bNat){
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

int Gen68k::NbRegBool(CNoeud* bNoeud){
	bNoeud->Afficher();
	int val=-1;
	TypeOptor operaTOR = bNoeud->GetOperator();

	if (operaTOR == OPTOR_CMP_AND || operaTOR == OPTOR_CMP_OR){
		int n1 = NbRegBool(bNoeud->GetFilsG());
		int n2 = NbRegBool(bNoeud->GetFilsD());
		val = max3(1,n1,n2);
	}
	else if (operaTOR == OPTOR_NOT){
		val = NbRegBool(bNoeud->GetFilsD());
	}
	else if (operaTOR == OPTOR_UNKNOWN){
		if (bNoeud->GetNature()== NOEUD_OPERANDE_CTE && bNoeud->GetTAG()->GetToken() ==TOKEN_TRUE 
			|| bNoeud->GetNature()== NOEUD_OPERANDE_CTE && bNoeud->GetTAG()->GetToken() ==TOKEN_FALSE){
			val=1;	// TO DO : à voir
		}
		// TO DO : autre objets...
	}
	else if (operaTOR == OPTOR_INF || operaTOR == OPTOR_INFEQ || operaTOR == OPTOR_EQUAL || 
		operaTOR == OPTOR_DIFFERENT || operaTOR == OPTOR_SUP || operaTOR == OPTOR_SUPEQ ) {
		int n1 = NbRegArith(bNoeud->GetFilsG(),NO_REG);
		int n2 = NbRegArith(bNoeud->GetFilsD(),NO_DVAL);
		if (n1!=n2){
			val = max(n1,n2);
		} else {
			val = n1+1;
		}
	}
	else{
		operaTOR;
		printf("je suis la nbregbool\n");
	}
	bNoeud->SetNbReg(val);
	return val;
}

int Gen68k::NbRegArith(CNoeud* bNoeud,NatureOp bNat){
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
void Gen68k::CodeObjet(CNoeud* bNoeud,NatureOp bNat,Operande68k** Op){
	size_op68k Size=GetSize(bNoeud);

	if (bNoeud->GetNature()==NOEUD_OPERANDE_VARIABLE){
		if (bNat==NO_REG){
			ILcoder.Add(MOVE,ILcoder.createOp(Stack.GetStackPos(bNoeud->GetTAG()->GetIdentif()),SP_REG),RegPile->Sommet(),Size);
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

size_op68k Gen68k::GetSize(CNoeud* bNoeud){
	size_op68k Size=SZ_UNKNOWN;

	if (bNoeud->GetType().Type==TP_INTEGER) Size=SZ_W;
	if (bNoeud->GetType().Type==TP_FLOAT) Size=SZ_F;
	if (bNoeud->GetType().Type==TP_LONG) Size=SZ_L;
	if (bNoeud->GetType().Type==TP_BOOLEAN) Size=SZ_W;	// et pas Size=SZ_B;
	if (bNoeud->GetType().Type==TP_BYTE) Size=SZ_W;		// et pas Size=SZ_B;
	return Size;
}

void Gen68k::CodeArith(CNoeud* bNoeud,NatureOp bNat,Operande68k** Op){
	NatureNoeud NatureArbre=bNoeud->GetNature();
	size_op68k Size=GetSize(bNoeud);
	Operande68k* Op1;
	Operande68k* Op2;
	//printf("entree dans codearith avec bNoeud:\n");
	//bNoeud->Afficher();

	// A = Litteral
	if (NatureArbre==NOEUD_OPERANDE_CTE){
		if (bNat==NO_REG){
			if (bNoeud->GetTAG()->GetToken()==TOKEN_NOMBRE)
			{
				ILcoder.Add(MOVE,ILcoder.createOpVal(atoi(bNoeud->GetTAG()->GetIdentif())),RegPile->Sommet(),Size);
				
			}
			else if (bNoeud->GetTAG()->GetToken()==TOKEN_STRINGCONSTANT)
			{
				// pas encore géré
			}
			else if (bNoeud->GetTAG()->GetToken()==TOKEN_TRUE)
			{
				ILcoder.Add(MOVE,ILcoder.createOpVal(1),RegPile->Sommet(),Size);

			}
			else if (bNoeud->GetTAG()->GetToken()==TOKEN_FALSE)
			{
				ILcoder.Add(MOVE,ILcoder.createOpVal(0),RegPile->Sommet(),Size);
			}
			else
			{
				printf("ERREUR interne2!!!\n");
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
				printf("ERREUR interne1!!!\n");
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
	// A = A1 opbin A2 et op_bin commutatif et A1 est litteral ou objet
	// et A2 n'est ni litteral ni objet
	else if (NatureArbre==NOEUD_OPERATOR && bNoeud->GetOperType()==OPBIN &&
			bNoeud->EstCommutatif() && 
			 (bNoeud->GetFilsG()->GetNature()==NOEUD_OPERANDE_CTE ||
			 bNoeud->GetFilsG()->GetNature()==NOEUD_OPERANDE_VARIABLE)
			 &&
			(bNoeud->GetFilsD()->GetNature()!=NOEUD_OPERANDE_CTE &&
			 bNoeud->GetFilsD()->GetNature()!=NOEUD_OPERANDE_VARIABLE))
	{
		CNoeud* aux=bNoeud->GetFilsD();
		bNoeud->SetFilsD(bNoeud->GetFilsG());
		bNoeud->SetFilsG(aux);
		CodeArith(bNoeud, bNat, Op);
	}
	
	else if (NatureArbre==NOEUD_OPERATOR && bNoeud->GetOperType()==OPBIN)
	{
		int n1,n2;
		n1=bNoeud->GetFilsG()->GetNbReg();
		n2=bNoeud->GetFilsD()->GetNbReg();
		Operande68k* T;

		if (n1>=mNbRegMax && n2 >=mNbRegMax){
			CodeArith(bNoeud->GetFilsD(),NO_REG,&Op2);
			T=RegPile->AllouerTemp(Size);
			ILcoder.Add(MOVE,Op2,T,Size);
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
			RegPile->EchangeD();
			CodeArith(bNoeud->GetFilsD(),NO_DVAL,&Op2);
			RegPile->Allouer(Op2);
			CodeArith(bNoeud->GetFilsG(),NO_REG,&Op1);
			RegPile->Liberer(Op2);
			RegPile->EchangeD();
			ILcoder.Add(ILcoder.NodeToOp(bNoeud),Op2,Op1,Size);
			*Op=Op1;
		}
	}
	// A = f(A1,A2, ... , An)
	else if (NatureArbre==NOEUD_OPERANDE_FONCTION)
	{
		int somme=0;		// la somme des tailles de ce qui est poussé dans la pile
		size_op68k SizeOfArgument;
		for (int i=bNoeud->GetSuccNmbr()-1;i>=0;i--){
			SizeOfArgument=GetSize(*bNoeud->GetSuccPtr(i));
			somme+=(int)SizeOfArgument;
			CodeArith(*bNoeud->GetSuccPtr(i),NO_REG,&Op2);
			ILcoder.Add(MOVE,Op2,ILcoder.createOp(A7,false,true),SizeOfArgument);
			Stack.PushToStack(SizeOfArgument);
		}
		ILcoder.Add(BSR,ILcoder.createOpEtiq(bNoeud->GetTAG()->GetIdentif()),SZ_NA);
		ILcoder.Add(ADD,ILcoder.createOpVal(somme),ILcoder.createOp(SP_REG),SZ_L);
		
		//Stack.Afficher();
		for (int i=bNoeud->GetSuccNmbr()-1;i>=0;i--){
			Stack.Pop();
			//Stack.Afficher();
		}
		ILcoder.Add(MOVE,ILcoder.createOp(D0),RegPile->Sommet(),Size);
		*Op=RegPile->Sommet();
	}
}

void Gen68k::CodeBool(CNoeud* bNoeud,bool AvecSaut,bool ValSaut,Operande68k* etiqSaut,bool DansReg,bool ValReg){
	//printf("Entree dans COdeBOOl\n");
	//printf("expression booleenne:\n");
	//bNoeud->Afficher();
	
	size_op68k Size=GetSize(bNoeud);
	Operande68k* T;

	Operande68k* Etiq = ILcoder.createOpEtiq();
	Operande68k* Etiq1 = ILcoder.createOpEtiq();
	//Operande68k* Etiq2 = ILcoder.createOpEtiq();

	bNoeud;
	TypeOptor operaTOR = bNoeud->GetOperator();

	if (operaTOR == OPTOR_CMP_AND){
		if (AvecSaut==true){
			if (ValSaut==true){
				CodeBool(bNoeud->GetFilsG(),true,false,Etiq1,DansReg,ValReg);
				CodeBool(bNoeud->GetFilsD(),true,true,etiqSaut,DansReg,ValReg);
				ILcoder.AddEtiq(Etiq1->val.valEtiq);
			}
			else {	// ValSaut==false
				CodeBool(bNoeud->GetFilsG(),true,false,etiqSaut,DansReg,ValReg);
				CodeBool(bNoeud->GetFilsD(),true,false,etiqSaut,DansReg,ValReg);
			}
		}
		else {		// AvecSaut==false
			CodeBool(bNoeud->GetFilsG(),true,false,Etiq1,true,ValReg);
			CodeBool(bNoeud->GetFilsD(),false,false,NULL,true,ValReg);	// le deuxieme false n'a aucun impact normalement
			ILcoder.AddEtiq(Etiq1->val.valEtiq);
		}
	}
	else if (operaTOR == OPTOR_CMP_OR){
		if (AvecSaut==true){
			if (ValSaut==true){
				CodeBool(bNoeud->GetFilsG(),true,true,etiqSaut,DansReg,ValReg);
				CodeBool(bNoeud->GetFilsD(),true,true,etiqSaut,DansReg,ValReg);
			}
			else {	// ValSaut==false
				CodeBool(bNoeud->GetFilsG(),true,true,Etiq1,DansReg,ValReg);
				CodeBool(bNoeud->GetFilsD(),true,false,etiqSaut,DansReg,ValReg);
				ILcoder.AddEtiq(Etiq1->val.valEtiq);
			}
		}
		else {		// AvecSaut==false
			CodeBool(bNoeud->GetFilsG(),true,true,Etiq1,true,ValReg);
			CodeBool(bNoeud->GetFilsD(),false,false,NULL,true,ValReg);	// le deuxieme false n'a aucun impact normalement
			ILcoder.AddEtiq(Etiq1->val.valEtiq);
		}
	}
	else if (operaTOR == OPTOR_NOT){
		CodeBool(bNoeud->GetFilsD(),AvecSaut,!ValSaut,etiqSaut,DansReg,!ValReg);
	}
	else if (operaTOR == OPTOR_UNKNOWN){
		bNoeud;
		if (bNoeud->GetNature()== NOEUD_OPERANDE_CTE && bNoeud->GetTAG()->GetToken() ==TOKEN_TRUE){
			Operande68k* ValRegOp;
			ValRegOp = ILcoder.createOpVal( ValReg ? 1 : 0 );
			if (AvecSaut==true){
				ILcoder.Add(MOVE,ValRegOp,RegPile->Sommet(),SZ_B);
				ILcoder.Add(BRA,etiqSaut,SZ_NA);
			}
			else {	// AvecSaut==false
				ILcoder.Add(MOVE,ValRegOp,RegPile->Sommet(),SZ_B);
			}
		} else if (bNoeud->GetNature()== NOEUD_OPERANDE_CTE && bNoeud->GetTAG()->GetToken() ==TOKEN_FALSE){
			Operande68k* ValRegOp;
			ValRegOp = ILcoder.createOpVal( ValReg ? 0 : 1 );
			if (AvecSaut==true){
				ILcoder.Add(MOVE,ValRegOp,RegPile->Sommet(),SZ_B);
				ILcoder.Add(BRA,etiqSaut,SZ_NA);
			}
			else {	// AvecSaut==false
				ILcoder.Add(MOVE,ValRegOp,RegPile->Sommet(),SZ_B);
			}
		}
	}
	else if (operaTOR == OPTOR_INF || operaTOR == OPTOR_INFEQ || operaTOR == OPTOR_EQUAL || 
		operaTOR == OPTOR_DIFFERENT || operaTOR == OPTOR_SUP || operaTOR == OPTOR_SUPEQ ) {
		int n1 = NbRegArith(bNoeud->GetFilsG(),NO_REG);
		int n2 = NbRegArith(bNoeud->GetFilsD(),NO_DVAL);
		Operande68k* Op2;
		Operande68k* Op1;
		if (n1 >= mNbRegMax && n2 >= mNbRegMax) {
			CodeArith(bNoeud->GetFilsD(),NO_REG,&Op2);
			T=RegPile->AllouerTemp(Size);
			ILcoder.Add(MOVE,Op2,T,Size);
			CodeArith(bNoeud->GetFilsG(),NO_REG,&Op1);
			RegPile->LibererTemp(T,Size);
			ILcoder.Add(CMP,T,Op1,GetSize(bNoeud->GetFilsD()));
		}
		else if (n1>=n2 && n2 < mNbRegMax)
		{
			CodeArith(bNoeud->GetFilsG(),NO_REG,&Op1);
			RegPile->Allouer(Op1);
			CodeArith(bNoeud->GetFilsD(),NO_DVAL,&Op2);
			RegPile->Liberer(Op1);
			ILcoder.Add(CMP,Op2,Op1,GetSize(bNoeud->GetFilsD()));
		}
		else if (n1<n2 && n1 < mNbRegMax)
		{
			RegPile->EchangeD();
			CodeArith(bNoeud->GetFilsD(),NO_DVAL,&Op2);
			RegPile->Allouer(Op2);
			CodeArith(bNoeud->GetFilsG(),NO_REG,&Op1);
			RegPile->Liberer(Op2);
			RegPile->EchangeD();
			ILcoder.Add(CMP,Op2,Op1,GetSize(bNoeud->GetFilsD()));
		}
		
		if (AvecSaut){
			// Scc
			/*
			if (ValReg==true){
				ILcoder.Add((InsOpEnum68k)(ILcoder.NodeToOp(bNoeud)+(SEQ-BEQ)),Op1,Size);
			} else {		// sinon l'opposé
				ILcoder.Add((InsOpEnum68k)(ILcoder.getOppBra(ILcoder.NodeToOp(bNoeud))+(SEQ-BEQ)),Op1,Size);
			}*/
			// Bcc
			if (ValSaut==true){
				ILcoder.Add(ILcoder.NodeToOp(bNoeud),etiqSaut,SZ_NA);
			} else {		// sinon l'opposé
				ILcoder.Add(ILcoder.getOppBra(ILcoder.NodeToOp(bNoeud)),etiqSaut,SZ_NA);
			}

		} else {
			// Scc
			/*
			if (ValReg==true){
				ILcoder.Add((InsOpEnum68k)(ILcoder.NodeToOp(bNoeud)+(SEQ-BEQ)),Op1,Size);
			} else {		// sinon l'opposé
				ILcoder.Add((InsOpEnum68k)(ILcoder.getOppBra(ILcoder.NodeToOp(bNoeud))+(SEQ-BEQ)),Op1,Size);
			}*/
		}
	}
	else{
		operaTOR;
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

	

	RegPile->Init();
	switch(bInstr->getNat()){
	case INS_AFFECTATION:
		Operande68k* ExprArbrAff;	//=RegPile->Sommet();
		Operande68k* ExprArbrAffected;
		int nG,nD;
		
		nG=NbRegArith(bInstr->GetAffectExprAssigned(),NO_REG);
		nD=NbRegArith(bInstr->GetAffectExprArbre(),NO_REG);
		
		if (nD >= nG){
			CodeArith(bInstr->GetAffectExprArbre(),NO_REG,&ExprArbrAff);
			RegPile->Allouer(ExprArbrAff);
			CodeArith(bInstr->GetAffectExprAssigned(),NO_DADR,&ExprArbrAffected);
			RegPile->Liberer(ExprArbrAff);
			ILcoder.Add(MOVE,ExprArbrAff,ExprArbrAffected,GetSize(bInstr->GetAffectExprAssigned()));
		}
		else{
			CodeArith(bInstr->GetAffectExprAssigned(),NO_DADR,&ExprArbrAffected);
			RegPile->Allouer(ExprArbrAffected);
			CodeArith(bInstr->GetAffectExprArbre(),NO_REG,&ExprArbrAff);
			RegPile->Liberer(ExprArbrAffected);
			ILcoder.Add(MOVE,ExprArbrAff,ExprArbrAffected,GetSize(bInstr->GetAffectExprAssigned()));							
		}
		break;
	case INS_RETURN:
		Operande68k* OpResReturn;
		if (bInstr->GetReturnExpr()){
			CodeArith(bInstr->GetReturnExpr(),NO_REG,&OpResReturn);
			ILcoder.Add(MOVE,OpResReturn,ILcoder.createOp(D0),GetSize(bInstr->GetReturnExpr()));
		}
		ILcoder.Add(RTS);
		break;
	case INS_CALL:
		int somme;
		CNoeud* CallNoeud;

		somme=0;		// la somme des tailles de ce qui est poussé dans la pile
		size_op68k SizeOfArgument;
		CallNoeud=bInstr->GetCallExpr();
		Operande68k* Op2;
		for (int i=CallNoeud->GetSuccNmbr()-1;i>=0;i--){
			SizeOfArgument=GetSize(*CallNoeud->GetSuccPtr(i));
			somme+=(int)SizeOfArgument;
			CodeArith(*CallNoeud->GetSuccPtr(i),NO_REG,&Op2);
			ILcoder.Add(MOVE,Op2,ILcoder.createOp(A7,false,true),SizeOfArgument);
			Stack.PushToStack(SizeOfArgument);
		}
		ILcoder.Add(BSR,ILcoder.createOpEtiq(CallNoeud->GetTAG()->GetIdentif()),SZ_NA);
		ILcoder.Add(ADD,ILcoder.createOpVal(somme),ILcoder.createOp(SP_REG),SZ_L);
		//Stack.Afficher();
		for (int i=CallNoeud->GetSuccNmbr()-1;i>=0;i--){
			Stack.Pop();
			//Stack.Afficher();
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
		EtiqForDebTest = ILcoder.createOpEtiq();
		EtiqForFin = ILcoder.createOpEtiq();

		// ---------------------------------------------------
		// Affectation initiale
		// ---------------------------------------------------
		nG=NbRegArith(varExpr,NO_REG);
		nD=NbRegArith(initExpr,NO_REG);
		
		if (nD >= nG){
			CodeArith(initExpr,NO_REG,&ExprArbrAff);
			RegPile->Allouer(ExprArbrAff);
			CodeArith(varExpr,NO_DADR,&ExprArbrAffected);
			RegPile->Liberer(ExprArbrAff);
			ILcoder.Add(MOVE,ExprArbrAff,ExprArbrAffected,SizeVarIter);
		}
		else{
			CodeArith(varExpr,NO_DADR,&ExprArbrAffected);
			RegPile->Allouer(ExprArbrAffected);
			CodeArith(initExpr,NO_REG,&ExprArbrAff);
			RegPile->Liberer(ExprArbrAffected);
			ILcoder.Add(MOVE,ExprArbrAff,ExprArbrAffected,SizeVarIter);							
		}

		// ---------------------------------------------------
		// Evaluation de l'expression d'arrivée
		// ---------------------------------------------------
		
		// temporaire sur la pile qui va contenir la valeur de l'expression d'arrivée TO
		TexpTO=RegPile->AllouerTemp(SizeVarIter);
		CodeArith(finExpr,NO_REG,&ExprArbrAff);
		//RegPile->Allouer(ExprArbrAff);
		ILcoder.Add(MOVE,ExprArbrAff,TexpTO,SizeVarIter);
		//RegPile->Liberer(ExprArbrAff);


		// ---------------------------------------------------
		// Test de la condition d'arrivé
		// ---------------------------------------------------
		ILcoder.AddEtiq(EtiqForDebTest->val.valEtiq);
		
		// cas 1: pas de step
		if (stepExpr == NULL){
			CodeArith(varExpr,NO_REG,&ExprArbrAffected);
			ILcoder.Add(CMP,TexpTO,ExprArbrAffected,SizeVarIter);
			ILcoder.Add(BGT,EtiqForFin,SZ_NA);
			CodeInstr(CorpsFor);
			// iterateur = iterateur+1 sous entendu
			CodeArith(varExpr,NO_DADR,&ExprArbrAffected);
			ILcoder.Add(ADD,ILcoder.createOpVal(1),ExprArbrAffected,SizeVarIter);
			ILcoder.Add(BRA,EtiqForDebTest,SZ_NA);
		}
		// cas 2: step constante
		else if (stepExpr->EstConstant()){
			int constStepVal;
			constStepVal = atoi(stepExpr->GetTAG()->GetIdentif());
			CodeArith(varExpr,NO_REG,&ExprArbrAffected);
			ILcoder.Add(CMP,TexpTO,ExprArbrAffected,SizeVarIter);
			if (constStepVal < 0){
				ILcoder.Add(BLT,EtiqForFin,SZ_NA);
			}
			else {
				ILcoder.Add(BGT,EtiqForFin,SZ_NA);
			}
			CodeInstr(CorpsFor);
			// iterateur = iterateur+cte sous entendu
			CodeArith(varExpr,NO_DADR,&ExprArbrAffected);
			ILcoder.Add(ADD,ILcoder.createOpVal(constStepVal),ExprArbrAffected,SizeVarIter);				
			ILcoder.Add(BRA,EtiqForDebTest,SZ_NA);

		}
		// cas 3: step non-constante
		else {
			//Operande68k* EtiqNegStep;
			//Operande68k* EtiqForDebCorps;
			//EtiqForFin = ILcoder.createOpEtiq();
			
		}
		ILcoder.AddEtiq(EtiqForFin->val.valEtiq);
		RegPile->LibererTemp(TexpTO,SizeVarIter);	// on libere le temporaire qui contient l'expression de fin
		break;
	case INS_STRUCT_DOLPWH:		// do ... loop while (condition)
		Collection* CorpsDo;
		CNoeud* exprDo;
		Operande68k* EtiqDebut;
		CorpsDo = bInstr->GetDOCorps();
		exprDo = bInstr->GetDOExprCondition();
		EtiqDebut = ILcoder.createOpEtiq();
		ILcoder.AddEtiq(EtiqDebut->val.valEtiq);
		CodeInstr(CorpsDo);
		CodeBool(exprDo,true,true,EtiqDebut,false,false);
		break;
	case INS_STRUCT_DOWH:		// do while (condition) .... loop
		Collection* CorpsWhile;
		CNoeud* exprWhile;
		Operande68k* EtiqWhileDebut;
		Operande68k* EtiqWhileFin;

		CorpsWhile = bInstr->GetDOCorps();
		exprWhile = bInstr->GetDOExprCondition();
		EtiqWhileDebut = ILcoder.createOpEtiq();
		EtiqWhileFin = ILcoder.createOpEtiq();		// juste avant le test de la condition

		ILcoder.Add(BRA,EtiqWhileFin,SZ_NA);		// on va directement voir le test
		ILcoder.AddEtiq(EtiqWhileDebut->val.valEtiq);
		CodeInstr(CorpsWhile);
		ILcoder.AddEtiq(EtiqWhileFin->val.valEtiq);
		CodeBool(exprWhile,true,true,EtiqWhileDebut,false,false);
		
		break;
	case INS_IF:
		CNoeud* exprIf;
		exprIf = bInstr->GetIFExprArbre();
		//printf("CodeInstr IF\n");
		//printf("Expression du If:\n");
		//exprIf->Afficher();

		Operande68k* EtiqFinCorpsIF = ILcoder.createOpEtiq();
		Operande68k* EtiqFinLocalCorpsElseIf;
		Operande68k* EtiqDebutElse=NULL;	// utilisé s'il y a des elseIf et un else
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
		CodeBool(exprIf,true,false,EtiqFinCorpsIF,false,false);	// le dernier false n'a aucun impact normalement

		// -------------------------------------------------------
		// Codage de corps de IF
		// -------------------------------------------------------
		CodeInstr(InstrCorpsIf);
		if (!InstrCorpsElse->estVide() || !listExprElseIf->estVide() ){
			EtiqFinBloc = ILcoder.createOpEtiq();
			ILcoder.Add(BRA,EtiqFinBloc,SZ_NA);		// on ajoute un saut à tout à la fin s'il y a un else ou des elseif
		}
		// -------------------------------------------------------
		//		etiq fin corps If
		// -------------------------------------------------------
		ILcoder.AddEtiq(EtiqFinCorpsIF->val.valEtiq);	


		// -------------------------------------------------------
		// Codage des ELSE IF
		// -------------------------------------------------------
		while(iterExprElseIf.elemExists()){
			exprElseIf=(CNoeud*)iterExprElseIf.getNext();
			InstrCorpsElseIf=(Collection*)iterCorpsElseIf.getNext();
			if (iterExprElseIf.elemExists()) {	// le suivant existe
				EtiqFinLocalCorpsElseIf = ILcoder.createOpEtiq();	// donc nouvelle etiquette
			} else {		// dernier element
				EtiqDebutElse = ILcoder.createOpEtiq();
				EtiqFinLocalCorpsElseIf=EtiqDebutElse;	// on utilise l'etiquette de fin de tout le bloc IF

			}

			// -------------------------------------------------------
			// Codage de expression de ElseIF
			// -------------------------------------------------------		
			CodeBool(exprElseIf,true,false,EtiqFinLocalCorpsElseIf,false,false);

			// -------------------------------------------------------
			// Codage de corps de ElseIF
			// -------------------------------------------------------	
			CodeInstr(InstrCorpsElseIf);

			if (!InstrCorpsElse->estVide()){			// il existe un Else, il ne faut pas executer son corps, donc saut à la fin
				ILcoder.Add(BRA,EtiqFinBloc,SZ_NA);
			}
			if (iterExprElseIf.elemExists()) {	// le suivant ElseIf existe, on avait crée une nouvelle étiquette
				ILcoder.AddEtiq(EtiqFinLocalCorpsElseIf->val.valEtiq);	// etiq fin corps ElseIf
			}
		}
		
		// -------------------------------------------------------
		// Codage de corps de ELSE
		// -------------------------------------------------------
		if (EtiqDebutElse) {	// un elseif a crée une étiquette debut Else
			ILcoder.AddEtiq(EtiqDebutElse->val.valEtiq);	// etiq fin corps ElseIf
		}
		CodeInstr(InstrCorpsElse);
		
		// -------------------------------------------------------
		//		etiq fin Global
		// -------------------------------------------------------
		if (!InstrCorpsElse->estVide() || !listExprElseIf->estVide()){
			ILcoder.AddEtiq(EtiqFinBloc->val.valEtiq);	// etiq fin corps Else
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
	ILcoder.SetStream(&file);
	
	mNbRegMax=8;
	ILcoder.AfficherLeDebut();

	
	int tailleVarLocales;
	Fonctions->bindIterator(&iter1);
	while(iter1.elemExists()){
		Fonc1=(FonctionItem*)iter1.getNext();
		if (Fonc1->GetUsed()){
			
			
			if (Fonc1->GetIsAssembler() ){
				
			}
			else {
				ILcoder.AddEtiq(Fonc1->GetNom());
				Stack.ClearStack();
				Fonc1->GetArguListe()->bindIterator(&iter2);
				while (iter2.elemExists()){
					Var1=(VariableItem*)iter2.getElem();
					// used? unused?
					Stack.PushToStack(Var1);
					iter2.getNext();
				}
				Stack.PushToStack(4);	// l'adresse de retour de la fonction
				//Stack.Afficher();
				
				tailleVarLocales=0;
				Fonc1->GetVarListe()->bindIterator(&iter2);
				while (iter2.elemExists()){
					Var1=(VariableItem*)iter2.getElem();
					Stack.PushToStack(Var1);
					tailleVarLocales+=Var1->GetSize();
					iter2.getNext();
				}
				if (tailleVarLocales)
					ILcoder.Add(SUB,ILcoder.createOpVal(tailleVarLocales),ILcoder.createOp(SP_REG),SZ_L);
				
				Stack.Afficher();

				Collection* InstrListe = Fonc1->GetInstrListe();
				CodeInstr(InstrListe);

				
				Stack.ClearStack();
				if (tailleVarLocales)
					ILcoder.Add(ADD,ILcoder.createOpVal(tailleVarLocales),ILcoder.createOp(SP_REG),SZ_L);
				ILcoder.Add(RTS);
			}
		}
	}// prochaine fonction

	ILcoder.Afficher();
	//ILcoder.Afficher("kjdhg");
}

