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

#include "noeud.h"
//#include <iostream.h>

char *ImageOperator[]={
	// TO DO: à refaire car TypeOptor a changé
	"NOT",
	"!",
	"/",
	"*",
	"-",
	"+",
	"arAND",
	"arOR",
	"arXOR",
	"&",
	"<",
	"<=",
	"=",
	"<>",
	">",
	">=",
	"cmpOR",
	"cmpAND",
	"cmpXOR",
	"MOD"
};



CNoeud::CNoeud(void)
{
	mNature=NOEUD_UNKNOWN;
	mType.Type=TP_UNKNOWN;	// on ne connait pas le type de cet élément
	mType.Expr=NULL;		// '	'	'	'	'	'	'	'	'
	mFilsG=NULL;
	mFilsD=NULL;
	nbReg=-1;
	for(int i=0;i<30;i++)
		Successeur[i]=NULL;
}
CNoeud::CNoeud(TAG* bTAG)
{
	//CNoeud();
	mNature=NOEUD_UNKNOWN;
	mType.Type=TP_UNKNOWN;	// on ne connait pas le type de cet élément
	mType.Expr=NULL;		// '	'	'	'	'	'	'	'	'
	mFilsG=NULL;
	mFilsD=NULL;
	for(int i=0;i<30;i++)
		Successeur[i]=NULL;
	mTAG =bTAG;
	// faut faire des trucs du style... (pour la génération de code)
	//if (bTAG->Token ==
	mOperator=GiveOperatorType(bTAG->GetToken());
	mOperType=GetOperType(bTAG);
	if (mOperator!=OPTOR_UNKNOWN)
		mNature=NOEUD_OPERATOR;
	else
	{
		if (bTAG->GetToken()==TOKEN_NOMBRE || bTAG->GetToken()==TOKEN_STRINGCONSTANT
			|| bTAG->GetToken()==TOKEN_TRUE
			|| bTAG->GetToken()==TOKEN_FALSE)
		{
			mNature=NOEUD_OPERANDE_CTE;
			#ifdef _DEBUGARBRES
				printf("defini comme operande cte\n");
			#endif
		}
		else if (bTAG->GetToken()==TOKEN_IDENTIF)
		{
			mNature=NOEUD_OPERANDE_VARIABLE;
			#ifdef _DEBUGARBRES
				printf("apriori une variable\n");
			#endif
		}
	}
}

CNoeud::~CNoeud(void)
{
	DestroyArbre();
}

void CNoeud::DestroyArbre(){
	if (mFilsG)
	{
		mFilsG->DestroyArbre();
		delete mFilsG;
	}
	if (mFilsD)
	{
		mFilsD->DestroyArbre();
		delete mFilsD;
	}
	for(int i=0;i<30;i++)
		if (Successeur[i])
		{
			Successeur[i]->DestroyArbre();
			delete Successeur[i];
		}
}

char *CNoeud::ImageNoeud(){
	if (mOperator==OPTOR_MOINSUNAIRE)
		return "-U";
	if (!(mTAG->GetToken() ==TOKEN_IDENTIF || mTAG->GetToken() ==TOKEN_NOMBRE || mTAG->GetToken() ==TOKEN_STRINGCONSTANT))
		return ReservedWords[mTAG->GetToken()];
	else
		return mTAG->GetIdentif();
}



void CNoeud::Afficher(char *S,char *SD,char *SG){
	char str1[300];
	char str2[300];
	char str3[300];
	char Ind[300];
	unsigned int i,j;
	for (i=0;i<strlen(ImageNoeud());i++)
		Ind[i]=32;
	Ind[i]=0;

	sprintf(str1,"%s%s     ",SD,Ind);
	sprintf(str2,"%s%s      ",SD,Ind);
	sprintf(str3,"%s%s     |",SD,Ind);
	if (mFilsD)
		mFilsD->Afficher(str1,str2,str3);

	for(j=0;j<30 && !Successeur[j];j++);

	if (mFilsD==NULL && mFilsG==NULL && j==30)
		printf("%s+-<%s >\n",S,ImageNoeud());
	else if (j<30)
	{
		if (mNature==NOEUD_OPERANDE_FONCTION)
			printf("%s+-<%s >-(\n",S,ImageNoeud());
		else if (mNature==NOEUD_OPERANDE_ARRAY)
			printf("%s+-<%s >-[\n",S,ImageNoeud());
		else
			printf("%s+-<%s >-?\n",S,ImageNoeud());

	}
	else
		printf("%s+-<%s >-|\n",S,ImageNoeud());


	sprintf(str1,"%s%s     ",SG,Ind);
	sprintf(str2,"%s%s     |",SG,Ind);
	sprintf(str3,"%s%s      ",SG,Ind);
	if (mFilsG)
		mFilsG->Afficher(str1,str2,str3);

	for(j=0;j<30;j++)
	{
		if (Successeur[j])
		{
			sprintf(str2,"%s%s     |",SG,Ind);
			sprintf(str3,"%s%s     |",SD,Ind);
			Successeur[j]->Afficher(str1,str2,str3);
		}
	}
}

void CNoeud::Afficher(){
	if (this)
		Afficher(""," "," ");

}

void CNoeud::Afficher(int indent){
	for(int i=0;i<indent;i++)
		printf(" ");
	printf("<%s>\n",ImageNoeud());
	if (mFilsG)
		mFilsG->Afficher(indent+5);
	if (mFilsD)
		mFilsD->Afficher(indent+5);
	for(int i=0;i<30;i++)
		if (Successeur[i])
		{
			Successeur[i]->Afficher();
		}
}

CNoeud* CNoeud::AddFilsD(TAG* bTAG){
	mFilsD = new CNoeud(bTAG);
	return mFilsD;
}
CNoeud* CNoeud::AddFilsG(TAG* bTAG){
	mFilsG = new CNoeud(bTAG);
	return mFilsG;
}

TypeOptor GiveOperatorType(enumTokenType bToken)
{
	TypeOptor retVal;
	if (bToken==TOKEN_OPENPAR)
		retVal = OPTOR_OPENPAR;
	else if (bToken==TOKEN_OPENCRO)
		retVal = OPTOR_OPENCRO;
	else if (bToken==TOKEN_POINT)
		retVal = OPTOR_POINT;
	else if (bToken==TOKEN_NOT)
		retVal = OPTOR_NOT;
	else if (bToken == TOKEN_EXCLAM)
		retVal = OPTOR_EXCLAM;
	else if (bToken == TOKEN_DIV)
		retVal = OPTOR_DIV;
	else if (bToken == TOKEN_MULT)
		retVal = OPTOR_MULT;
	else if (bToken == TOKEN_MOINS)
		retVal = OPTOR_SUB;
	else if (bToken == TOKEN_PLUS)
		retVal = OPTOR_ADD;
	else if (bToken == TOKEN_AND)
	{
		retVal = OPTOR_CMP_AND;
	}
	else if (bToken == TOKEN_OR)
	{
		retVal = OPTOR_CMP_OR;
	}
	else if (bToken == TOKEN_XOR)
	{
			retVal = OPTOR_CMP_XOR;
	}
	else if (bToken == TOKEN_CONCAT)								// &
		retVal = OPTOR_CONCAT;
	else if (bToken == TOKEN_INF)									// <
		retVal = OPTOR_INF;
	else if (bToken == TOKEN_INFEQ1 || bToken == TOKEN_INFEQ2)		// <=
		retVal = OPTOR_INFEQ;
	else if (bToken == TOKEN_EQUAL)									// =
		retVal = OPTOR_EQUAL;
	else if (bToken == TOKEN_DIFFERENT)								// <>
		retVal = OPTOR_DIFFERENT;
	else if (bToken == TOKEN_SUPEQUAL1 || bToken == TOKEN_SUPEQUAL2)// >=
		retVal = OPTOR_SUPEQ;
	else if (bToken == TOKEN_SUPERIEUR)									// <
		retVal = OPTOR_SUP;
	else if (bToken == TOKEN_MOD)									// Mod
		retVal = OPTOR_MOD;
	else
		retVal = OPTOR_UNKNOWN;
	return retVal;
}


void CNoeud::SetFilsD(CNoeud *bNoeud){
	mFilsD=bNoeud;
}
void CNoeud::SetFilsG(CNoeud *bNoeud){
	mFilsG=bNoeud;
}
void CNoeud::SetAsFonction(){
	mNature=NOEUD_OPERANDE_FONCTION;
	#ifdef _DEBUGARBRES
		printf("finalement function\n");
	#endif
}
void CNoeud::SetAsArray(){
	mNature=NOEUD_OPERANDE_ARRAY;
	#ifdef _DEBUGARBRES
		printf("finalement array\n");
	#endif
}
void CNoeud::SetOperatorMoinsUnaire(){
	mOperator=OPTOR_MOINSUNAIRE;
	mOperType=OPUND;
}
void CNoeud::SetOperatorArith(){
	if (mOperator==OPTOR_CMP_AND){
		mOperator=OPTOR_ARI_AND;
	}
	else if (mOperator==OPTOR_CMP_OR){
		mOperator=OPTOR_ARI_OR;
	}
	else if (mOperator==OPTOR_CMP_XOR){
		mOperator=OPTOR_ARI_XOR;
	}
}
void CNoeud::SetType(VarTypeType bType){
	if (bType.Type!=TP_USER)
		bType.Expr=NULL;
	mType=bType;
}
void CNoeud::SetNbReg(int Nb){
	nbReg=Nb;
}

bool CNoeud::EstFeuille(){
	return (mFilsG==NULL && mFilsD==NULL);
};

int CNoeud::GetNbReg(){
	return nbReg;
}

CNoeud* CNoeud::GetFilsG(){
	return mFilsG;
}
CNoeud* CNoeud::GetFilsD(){
	return mFilsD;
}
CNoeud** CNoeud::GetSuccPtr(int index){
	return &Successeur[index];
}
int CNoeud::GetSuccNmbr(){
	int i;
	for(i=0;Successeur[i]!=NULL;i++);
	return i;
}
NatureNoeud CNoeud::GetNature(){
	return mNature;
}
VarTypeType CNoeud::GetType(){
	return mType;
}
TAG* CNoeud::GetTAG(){
	return mTAG;
}
TypeOptor CNoeud::GetOperator(){
	return mOperator;
}

natureOperator CNoeud::GetOperType(){
	return mOperType;
}
bool CNoeud::EstCommutatif(){
	if (mOperator== OPTOR_MULT || mOperator== OPTOR_ADD || mOperator== OPTOR_CONCAT ||
		mOperator== OPTOR_EQUAL || mOperator== OPTOR_DIFFERENT || mOperator== OPTOR_CMP_AND ||
		mOperator== OPTOR_CMP_OR)
		return true;
	return false;
}
natureOperator CNoeud::GetOperType(TAG* bTag)
{
	enumTokenType bToken = bTag->GetToken();
	if ((bToken >= TOKEN_PLUS && bToken <= TOKEN_CONCAT) ||
		(bToken >= TOKEN_INF && bToken <= TOKEN_SUPEQUAL2) ||
		bToken == TOKEN_POINT || bToken == TOKEN_MOD ||
		bToken == TOKEN_OR || bToken == TOKEN_XOR || bToken == TOKEN_AND)
		return OPBIN;
	else if (bToken==TOKEN_NOT)
		return OPUND;
	else if (bToken==TOKEN_EXCLAM)
		return OPUNG;
	else
		return NOTOP;
}

bool CNoeud::EstConstant(){
	if (mNature==NOEUD_OPERANDE_CTE){
		return true;
	}
	else if (mNature==NOEUD_OPERATOR){
		if (mFilsG && mFilsD){
			return (mFilsG->EstConstant() && mFilsD->EstConstant());
		}
		else if (mFilsG){
			return mFilsG->EstConstant();
		}
		else if (mFilsD){
			return mFilsD->EstConstant();
		}
		else {
			return false;	// un operateur sans fils, normalement impossible
		}
	}
	return false;
}

void CNoeud::SimplifyConstantExpression(){
	if (!EstConstant()) return;
	printf("ok on va simplifier\n");
	this->Afficher();
	VarTypeType unknownVal(TP_UNKNOWN);
	VarTypeType intVal(TP_INTEGER);
	VarTypeType longVal(TP_LONG);
	VarTypeType byteVal(TP_BYTE);
	VarTypeType floatVal(TP_FLOAT);
	VarTypeType stringVal(TP_STRING);
	VarTypeType boolVal(TP_BOOLEAN);
	VarTypeType voidVal(TP_VOID);
	char *courIdentif1;
	char *courIdentif2;
	switch(mNature)
	{
	case NOEUD_OPERATOR:
		if (mOperType==OPBIN){
			mFilsG->SimplifyConstantExpression();
			mFilsD->SimplifyConstantExpression();
			
			if (mFilsG->EstConstant() && mFilsD->EstConstant()){
				this->GetType();
				courIdentif1=mFilsG->GetTAG()->GetIdentif();
				courIdentif2=mFilsD->GetTAG()->GetIdentif();
				if (mType==intVal || mType==longVal || mType==byteVal){
					if (mOperator==OPTOR_ADD){
						*this = *mFilsG;
						int i1=atoi(courIdentif1);
						int i2=atoi(courIdentif2);
						sprintf(courIdentif1,"%i",i1+i2);
						delete mFilsG;
						delete mFilsD;
					}
					else if (mOperator==OPTOR_SUB){
						*this = *mFilsG;
						int i1=atoi(courIdentif1);
						int i2=atoi(courIdentif2);
						sprintf(courIdentif1,"%i",i1-i2);
						delete mFilsG;
						delete mFilsD;
					}
					else if (mOperator==OPTOR_MULT){
						*this = *mFilsG;
						int i1=atoi(courIdentif1);
						int i2=atoi(courIdentif2);
						sprintf(courIdentif1,"%i",i1*i2);
						delete mFilsG;
						delete mFilsD;
					}
					else if (mOperator==OPTOR_DIV){
						*this = *mFilsG;
						int i1=atoi(courIdentif1);
						int i2=atoi(courIdentif2);
						sprintf(courIdentif1,"%i",i1/i2);
						delete mFilsG;
						delete mFilsD;
					}
				}
				else if (mType==floatVal || mType==longVal || mType==byteVal){
					if (mOperator==OPTOR_ADD){
						*this = *mFilsG;
						double i1=atof(courIdentif1);
						double i2=atof(courIdentif2);
						sprintf(courIdentif1,"%f",i1+i2);
						delete mFilsG;
						delete mFilsD;
					}
					else if (mOperator==OPTOR_SUB){
						*this = *mFilsG;
						double i1=atof(courIdentif1);
						double i2=atof(courIdentif2);
						sprintf(courIdentif1,"%f",i1-i2);
						delete mFilsG;
						delete mFilsD;
					}
					else if (mOperator==OPTOR_MULT){
						*this = *mFilsG;
						double i1=atof(courIdentif1);
						double i2=atof(courIdentif2);
						sprintf(courIdentif1,"%f",i1*i2);
						delete mFilsG;
						delete mFilsD;
					}
					else if (mOperator==OPTOR_DIV){
						*this = *mFilsG;
						double i1=atof(courIdentif1);
						double i2=atof(courIdentif2);
						sprintf(courIdentif1,"%f",i1/i2);
						delete mFilsG;
						delete mFilsD;
					}
				}
			}
			
		}
		else if (mOperType==OPUND){
			if (mOperator==OPTOR_MOINSUNAIRE){
				if (mFilsD->EstConstant()){
					courIdentif1=mFilsD->GetTAG()->GetIdentif();
					if (mType == intVal || mType==longVal || mType==byteVal){
						sprintf(courIdentif1,"%i",-atoi(courIdentif1));
					}
					else if (mType == floatVal) {
						sprintf(courIdentif1,"%f",-atof(courIdentif1));
					}
					*this = *mFilsD;
					//mTAG->SetIdentif(courIdentif1);
					delete mFilsD;
				}
			}
		}
		break;
	case NOEUD_OPERANDE_CTE:
		//rien à faire
		break;
	case NOEUD_OPERANDE_VARIABLE:
		//rien à faire
		break;
	case NOEUD_OPERANDE_FONCTION:
		//rien à faire
		break;
	case NOEUD_OPERANDE_ARRAY:
		//rien à faire
		break;
	}
	printf("apres simplification:\n");
	this->Afficher();
	printf("\n\n");
}

