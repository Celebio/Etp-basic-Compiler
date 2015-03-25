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

#include "asm68kCoder.h"


using namespace std;

const char* ImageOp68k[]={
	"???",
	"MOVE",
	"ADD",
	"SUB",
	"MULS",
	"DIVS",
	"CMP",
	"NOT",
	"OPP",
	"BSR",
	"BRA",

	"BEQ",
	"BNE",
	"BGE",
	"BGT",
	"BLE",
	"BLT",

	"SEQ",
	"SNE",
	"SGE",
	"SGT",
	"SLE",
	"SLT",

	"RTS",
};



/**********************************************************
	CONSTRUCTEUR/DESTRUCTEUR
	*******************************************************************/
asm68kCoder::asm68kCoder(void){
	etiqNo=0;
	mStream=NULL;
	generatedCode=NULL;
	lastAdded=NULL;
}

asm68kCoder::~asm68kCoder(void){
}

/**********************************************************
	UTILE
	*******************************************************************/
InsOpEnum68k asm68kCoder::NodeToOp(CNoeud* bNoeud)
{
	switch(bNoeud->GetOperator())
	{
	case OPTOR_NOT:
		return NOT;
		break;
	case OPTOR_DIV:
		return DIV;
		break;
	case OPTOR_MULT:
		return MUL;
		break;
	case OPTOR_SUB:
		return SUB;
		break;
	case OPTOR_ADD:
		return ADD;
		break;
	case OPTOR_EQUAL:
		return BEQ;
		break;
	case OPTOR_DIFFERENT:
		return BNE;
		break;
	case OPTOR_SUP:
		return BGT;
		break;
	case OPTOR_SUPEQ:
		return BGE;
		break;
	case OPTOR_INF:
		return BLT;
		break;
	case OPTOR_INFEQ:
		return BLE;
		break;

	}

	return OPB;
}


/**********************************************************
	AFFICHAGE
	*******************************************************************/

void asm68kCoder::Afficher(reg_id bReg){
	if (bReg==SP_REG)
		//printf("A7");
		(*mStream) << "A7";
	else if (bReg>=A0 && bReg <=A7)
		//printf("A%i",bReg);
		(*mStream) << "A" << bReg;
	else if (bReg>=D0 && bReg <=D7)
		//printf("D%i",bReg-D0);
		(*mStream) << "D" << bReg-D0;
	else
		//printf("??");
		(*mStream) << "??";
}


void asm68kCoder::Afficher(Operande68k* bOperande){
	if (bOperande==NULL)
		return;

	if (bOperande->nat==OP_ENTIER){
		//printf("#%i",bOperande->val.valInt);
		(*mStream) << "#" << bOperande->val.valInt;
	}
	else if (bOperande->nat==OP_FLOAT){
		//printf("#%f",bOperande->val.valFloat);
		(*mStream) << "#" << bOperande->val.valFloat;
	}
	else if (bOperande->nat==OP_CHAINE){
		//printf("#%c%s%c",34,bOperande->val.valChaine,34);
		(*mStream) << "#\"" << bOperande->val.valChaine << "\"";
	}
	else if (bOperande->nat==OP_DIRECT){
		Afficher(bOperande->val.RegDirect);
	}
	else if (bOperande->nat==OP_INDIRECT){
		if (bOperande->PreDecr)
			//printf("-");
			(*mStream) << "-";
		if (bOperande->val.Indirect.Dep!=0)
			//printf("%i(",bOperande->val.Indirect.Dep);
			(*mStream) << bOperande->val.Indirect.Dep;
		else {
		}
			//printf("(");
			(*mStream) << "(";
			Afficher(bOperande->val.Indirect.RegBase);
			//printf(")");
			(*mStream) << ")";
			if (bOperande->PostIncr){
				//printf("+");
				(*mStream) << "+";
			}
	}
	else if (bOperande->nat==OP_INDEXE){
		Afficher(bOperande->val.Indexe.RegIndexe);
		//printf("%i(",bOperande->val.Indirect.Dep);
		(*mStream) << bOperande->val.Indirect.Dep << "(";
		Afficher(bOperande->val.Indexe.RegBase);
		//printf(")");
		(*mStream) << ")";
	}
	else if (bOperande->nat==OP_ETIQ){
		//printf("%s",bOperande->val.valEtiq);
		(*mStream) << bOperande->val.valEtiq;
	}
}

void asm68kCoder::AfficherLeDebut(){
	//printf(";Made with ETP\n\tinclude \"os.h\"\n\txdef _main\n\txdef _nostub\n\txdef _ti89\n\txdef _ti92plus\n\t\n\n");
	(*mStream) << ";Made with ETP\n\tinclude \"os.h\"\n\txdef _main\n\txdef _nostub\n\txdef _ti89\n\txdef _ti92plus\n\t\n\n";
}

void asm68kCoder::Afficher(){
	LigneCode68k* Cour=generatedCode;
	while (Cour){
		Afficher(Cour);
		Cour=Cour->next;
	}
}

void asm68kCoder::Afficher(LigneCode68k* bLigne){
	int sz;
	if (bLigne==NULL)
		return;

	switch(bLigne->nat){
		case NA_ETIQ:
			//printf("%s:",bLigne->val.etiq);
			(*mStream) << bLigne->val.etiq << ":";
			break;
		case NA_INST:
			//printf("\t");
			(*mStream) << "\t";
			//printf("%s",ImageOp68k[bLigne->val.instr->Op]);
			(*mStream) << ImageOp68k[bLigne->val.instr->Op];
			sz=bLigne->val.instr->Size;
			if (sz==SZ_UNKNOWN){
				//printf(".?");
				(*mStream) << ".?";
			}else if (sz==SZ_NA){
				//printf("");
				(*mStream) << "";
			}else if (sz==SZ_B){
				//printf(".B");
				(*mStream) << ".B";
			}else if (sz==SZ_W){
				//printf(".W");
				(*mStream) << ".W";
			}else if (sz==SZ_L){
				//printf(".L");
				(*mStream) << ".L";
			}else if (sz==SZ_F){
				//printf(".F");
				(*mStream) << ".F";
			}
			//printf("\t");
			(*mStream) << "\t";
			Afficher(bLigne->val.instr->op1);
			if (bLigne->val.instr->op2){
				//printf(",");
				(*mStream) << ",";
				Afficher(bLigne->val.instr->op2);
			}
			break;
		case NA_COMMENT:
			//printf("\t;%s",bLigne->val.comment);
			(*mStream) << "\t;" << bLigne->val.comment;
			break;
		case NA_UNKNOWN:
			break;
	}
	//printf("\n");
	(*mStream) << "\n";
}

/**********************************************************
	AJOUT D'INSTRUCTION
	*******************************************************************/

void asm68kCoder::Add(InsOpEnum68k bOp,Operande68k* bOp1,Operande68k* bOp2,size_op68k bSize){
	LigneCode68k* aux=new LigneCode68k();
	aux->nat=NA_INST;
	InstrIL* aux1=new InstrIL();
	aux1->Op=bOp;
	aux1->op1=new Operande68k(*bOp1);
	aux1->op2=new Operande68k(*bOp2);
	aux1->Size=bSize;
	aux->val.instr=aux1;

	if (generatedCode==NULL){
		generatedCode=aux;
		lastAdded=generatedCode;
	}
	else{
		lastAdded->next=aux;
		lastAdded=aux;
	}
#ifdef AFF_EACH_INST
	Afficher();
#endif
}
void asm68kCoder::Add(InsOpEnum68k bOp,Operande68k* bOp1,size_op68k bSize){
	LigneCode68k* aux=new LigneCode68k();
	aux->nat=NA_INST;
	InstrIL* aux1=new InstrIL();
	aux1->Op=bOp;
	aux1->op1=new Operande68k(*bOp1);
	aux1->op2=NULL;
	aux1->Size=bSize;
	aux->val.instr=aux1;

	if (generatedCode==NULL){
		generatedCode=aux;
		lastAdded=generatedCode;
	}
	else{
		lastAdded->next=aux;
		lastAdded=aux;
	}
#ifdef AFF_EACH_INST
	Afficher();
#endif
}
void asm68kCoder::Add(InsOpEnum68k bOp){
	LigneCode68k* aux=new LigneCode68k();
	aux->nat=NA_INST;
	InstrIL* aux1=new InstrIL();
	aux1->Op=bOp;
	aux1->op1=NULL;
	aux1->op2=NULL;
	aux1->Size=SZ_NA;
	aux->val.instr=aux1;

	if (generatedCode==NULL){
		generatedCode=aux;
		lastAdded=generatedCode;
	}
	else{
		lastAdded->next=aux;
		lastAdded=aux;
	}
#ifdef AFF_EACH_INST
	Afficher();
#endif
}
void asm68kCoder::Add(const char* bComment){
	LigneCode68k* aux=new LigneCode68k();
	aux->nat=NA_COMMENT;
	aux->val.comment=bComment;

	if (generatedCode==NULL){
		generatedCode=aux;
		lastAdded=generatedCode;
	}
	else{
		lastAdded->next=aux;
		lastAdded=aux;
	}
#ifdef AFF_EACH_INST
	Afficher();
#endif
}

void asm68kCoder::AddEtiq(char* bEtiq){
	LigneCode68k* aux=new LigneCode68k();
	aux->nat=NA_ETIQ;
	aux->val.etiq=bEtiq;

	if (generatedCode==NULL){
		generatedCode=aux;
		lastAdded=generatedCode;
	}
	else{
		lastAdded->next=aux;
		lastAdded=aux;
	}
#ifdef AFF_EACH_INST
	Afficher();
#endif
}


/**********************************************************
	CREATION OPERANDE
	*******************************************************************/
Operande68k* asm68kCoder::createOp(reg_id bRegDirect){
	Operande68k* aux=new Operande68k();
	aux->nat=OP_DIRECT;
	aux->PostIncr=false;
	aux->PreDecr=false;
	aux->val.RegDirect=bRegDirect;
	return aux;
}
Operande68k* asm68kCoder::createOp(int bDep,reg_id bRegBase){
	Operande68k* aux=new Operande68k();
	aux->nat=OP_INDIRECT;
	aux->PostIncr=false;
	aux->PreDecr=false;
	aux->val.Indirect.Dep=bDep;
	aux->val.Indirect.RegBase=bRegBase;
	return aux;
}
Operande68k* asm68kCoder::createOp(reg_id bRegBase,bool Pred,bool Posti){
	Operande68k* aux=new Operande68k();
	aux->nat=OP_INDIRECT;
	aux->PostIncr=Pred;
	aux->PreDecr=Posti;
	aux->val.Indirect.Dep=0;
	aux->val.Indirect.RegBase=bRegBase;
	return aux;
}
Operande68k* asm68kCoder::createOp(int bDep,reg_id bRegBase,reg_id bRegIndexe){
	Operande68k* aux=new Operande68k();
	aux->nat=OP_INDEXE;
	aux->PostIncr=false;
	aux->PreDecr=false;
	aux->val.Indexe.Dep=bDep;
	aux->val.Indexe.RegBase=bRegBase;
	aux->val.Indexe.RegIndexe=bRegIndexe;
	return aux;
}
Operande68k* asm68kCoder::createOpVal(int bvalInt){
	Operande68k* aux=new Operande68k();
	aux->nat=OP_ENTIER;
	aux->PostIncr=false;
	aux->PreDecr=false;
	aux->val.valInt=bvalInt;
	return aux;
}
Operande68k* asm68kCoder::createOpFloat(float bvalFloat){
	Operande68k* aux=new Operande68k();
	aux->nat=OP_FLOAT;
	aux->PostIncr=false;
	aux->PreDecr=false;
	aux->val.valFloat=bvalFloat;
	return aux;
}
Operande68k* asm68kCoder::createOpChaine(char* bvalChaine){
	Operande68k* aux=new Operande68k();
	aux->nat=OP_CHAINE;
	aux->PostIncr=false;
	aux->PreDecr=false;
	aux->val.valChaine=bvalChaine;
	return aux;
}
Operande68k* asm68kCoder::createOpEtiq(char* bvalEtiq){
	Operande68k* aux=new Operande68k();
	aux->nat=OP_ETIQ;
	aux->PostIncr=false;
	aux->PreDecr=false;
	aux->val.valEtiq=bvalEtiq;
	return aux;
}

Operande68k* asm68kCoder::createOpEtiq(){
	Operande68k* aux=new Operande68k();
	// g?n?ration automatique des ?tiquettes distinctes
	// to do: a transformer en string de stdlib
	char* newString = (char*)malloc(300);
	sprintf(newString,"%s%i",".sysetiq",etiqNo);
	etiqNo++;

	aux->nat=OP_ETIQ;
	aux->PostIncr=false;
	aux->PreDecr=false;
	aux->val.valEtiq=newString;
	return aux;
}

InsOpEnum68k asm68kCoder::getOppBra(InsOpEnum68k bOp){
	if (bOp == BEQ) return BNE;
	if (bOp == BNE) return BEQ;
	if (bOp == BGE) return BLT;
	if (bOp == BGT) return BLE;
	if (bOp == BLE) return BGT;
	if (bOp == BLT) return BGE;
	return OPB;
}
