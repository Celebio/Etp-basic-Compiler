
#include "IL.h"



char* ImageOp[]={
	"???",
	"LOAD",
	"STORE",
	"ADD",
	"SUB",
	"MUL",
	"DIV",
	"NOT",
	"OPP"
};



/**********************************************************
	CONSTRUCTEUR/DESTRUCTEUR
	*******************************************************************/
IL::IL(void){
	generatedCode=NULL;
	lastAdded=NULL;
}

IL::~IL(void){
}

/**********************************************************
	UTILE
	*******************************************************************/
InsOpEnum IL::NodeToOp(CNoeud* bNoeud)
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
	}
	return OPB;
}


/**********************************************************
	AFFICHAGE
	*******************************************************************/

void IL::Afficher(reg_id bReg){
	if (bReg==SP_REG)
		printf("SP");
	else
		printf("R%i",bReg);
}
void IL::Afficher(Operande* bOperande){
	if (bOperande==NULL)
		return;

	if (bOperande->nat==OP_ENTIER){
		printf("#%i",bOperande->val.valInt);
	}
	else if (bOperande->nat==OP_FLOAT){
		printf("#%f",bOperande->val.valFloat);
	}
	else if (bOperande->nat==OP_CHAINE){
		printf("#%c%s%c",34,bOperande->val.valChaine,34);
	}
	else if (bOperande->nat==OP_DIRECT){
		Afficher(bOperande->val.RegDirect);
	}
	else if (bOperande->nat==OP_INDIRECT){
		printf("%i(",bOperande->val.Indirect.Dep);
		Afficher(bOperande->val.Indirect.RegBase);
		printf(")");
	}
	else if (bOperande->nat==OP_INDEXE){
		Afficher(bOperande->val.Indexe.RegIndexe);
		printf("%i(",bOperande->val.Indirect.Dep);
		Afficher(bOperande->val.Indexe.RegBase);
		printf(")");
	}
}

void IL::Afficher(){
	LigneCode* Cour=generatedCode;
	while (Cour){
		Afficher(Cour);
		Cour=Cour->next;
	}
}

void IL::Afficher(LigneCode* bLigne){
	int sz;
	if (bLigne==NULL)
		return;

	switch(bLigne->nat){
		case NA_ETIQ:
			printf("%s:",bLigne->val.etiq);
			break;
		case NA_INST:
			printf("\t");
			printf("%s",ImageOp[bLigne->val.instr->Op]);
			printf(".");
			sz=bLigne->val.instr->Size;
			if (sz==SZ_UNKNOWN){
				printf("?");
			}else if (sz==SZ_B){
				printf("B");
			}else if (sz==SZ_W){
				printf("W");
			}else if (sz==SZ_L){
				printf("L");
			}else if (sz==SZ_F){
				printf("F");
			}
			printf("\t");
			Afficher(bLigne->val.instr->op1);
			printf(",");
			Afficher(bLigne->val.instr->op2);
			break;
		case NA_COMMENT:
			printf("\t//%s",bLigne->val.comment);
			break;
		case NA_UNKNOWN:
			break;
	}
	printf("\n");
}

/**********************************************************
	AJOUT D'INSTRUCTION
	*******************************************************************/

void IL::Add(InsOpEnum bOp,Operande* bOp1,Operande* bOp2,size_op bSize){
	LigneCode* aux=new LigneCode();
	aux->nat=NA_INST;
	InstrIL* aux1=new InstrIL();
	aux1->Op=bOp;
	aux1->op1=new Operande(*bOp1);
	aux1->op2=new Operande(*bOp2);
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
}
void IL::Add(InsOpEnum bOp,Operande* bOp1,size_op bSize){
	LigneCode* aux=new LigneCode();
	aux->nat=NA_INST;
	InstrIL* aux1=new InstrIL();
	aux1->Op=bOp;
	aux1->op1=new Operande(*bOp1);
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
}
void IL::Add(InsOpEnum bOp){
	LigneCode* aux=new LigneCode();
	aux->nat=NA_INST;
	InstrIL* aux1=new InstrIL();
	aux1->Op=bOp;
	aux1->op1=NULL;
	aux1->op2=NULL;
	aux1->Size=SZ_UNKNOWN;
	aux->val.instr=aux1;
	
	if (generatedCode==NULL){
		generatedCode=aux;
		lastAdded=generatedCode;
	}
	else{
		lastAdded->next=aux;
		lastAdded=aux;
	}
}
void IL::Add(char* bComment){
	LigneCode* aux=new LigneCode();
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
}

void IL::AddEtiq(char* bEtiq){
	LigneCode* aux=new LigneCode();
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
}


/**********************************************************
	CREATION OPERANDE
	*******************************************************************/
Operande* IL::createOp(reg_id bRegDirect){
	Operande* aux=new Operande();
	aux->nat=OP_DIRECT;
	aux->val.RegDirect=bRegDirect;
	return aux;
}
Operande* IL::createOp(int bDep,reg_id bRegBase){
	Operande* aux=new Operande();
	aux->nat=OP_INDIRECT;
	aux->val.Indirect.Dep=bDep;
	aux->val.Indirect.RegBase=bRegBase;
	return aux;
}
Operande* IL::createOp(int bDep,reg_id bRegBase,reg_id bRegIndexe){
	Operande* aux=new Operande();
	aux->nat=OP_INDEXE;
	aux->val.Indexe.Dep=bDep;
	aux->val.Indexe.RegBase=bRegBase;
	aux->val.Indexe.RegIndexe=bRegIndexe;
	return aux;
}
Operande* IL::createOpVal(int bvalInt){
	Operande* aux=new Operande();
	aux->nat=OP_ENTIER;
	aux->val.valInt=bvalInt;
	return aux;
}
Operande* IL::createOpFloat(float bvalFloat){
	Operande* aux=new Operande();
	aux->nat=OP_FLOAT;
	aux->val.valFloat=bvalFloat;
	return aux;
}
Operande* IL::createOpChaine(char* bvalChaine){
	Operande* aux=new Operande();
	aux->nat=OP_CHAINE;
	aux->val.valChaine=bvalChaine;
	return aux;
}
Operande* IL::createOpEtiq(char* bvalEtiq){
	Operande* aux=new Operande();
	aux->nat=OP_ETIQ;
	aux->val.valEtiq=bvalEtiq;
	return aux;
}

