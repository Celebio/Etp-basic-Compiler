
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
InsOpEnum IL::nodeToOp(CNoeud* bNoeud)
{
    switch(bNoeud->getOperator())
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

void IL::display(reg_id bReg){
    if (bReg==SP_REG)
        printf("SP");
    else
        printf("R%i",bReg);
}
void IL::display(Operande* bOperande){
    if (bOperande==NULL)
        return;

    if (bOperande->nat==OP_INTEGER){
        printf("#%i",bOperande->val.valInt);
    }
    else if (bOperande->nat==OP_FLOAT){
        printf("#%f",bOperande->val.valFloat);
    }
    else if (bOperande->nat==OP_STRING){
        printf("#%c%s%c",34,bOperande->val.valString,34);
    }
    else if (bOperande->nat==OP_DIRECT){
        display(bOperande->val.directRegister);
    }
    else if (bOperande->nat==OP_INDIRECT){
        printf("%i(",bOperande->val.indirect.dep);
        display(bOperande->val.indirect.baseRegister);
        printf(")");
    }
    else if (bOperande->nat==OP_INDEXE){
        display(bOperande->val.indexed.indexRegister);
        printf("%i(",bOperande->val.indirect.dep);
        display(bOperande->val.indexed.baseRegister);
        printf(")");
    }
}

void IL::display(){
    LigneCode* Cour=generatedCode;
    while (Cour){
        display(Cour);
        Cour=Cour->next;
    }
}

void IL::display(LigneCode* bLigne){
    int sz;
    if (bLigne==NULL)
        return;

    switch(bLigne->nat){
        case NA_ETIQ:
            printf("%s:",bLigne->val.label);
            break;
        case NA_INST:
            printf("\t");
            printf("%s",ImageOp[bLigne->val.instr->opertr]);
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
            display(bLigne->val.instr->op1);
            printf(",");
            display(bLigne->val.instr->op2);
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

void IL::add(InsOpEnum bOp,Operande* bOp1,Operande* bOp2,size_op bSize){
    LigneCode* aux=new LigneCode();
    aux->nat=NA_INST;
    InstrIL* aux1=new InstrIL();
    aux1->opertr=bOp;
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
void IL::add(InsOpEnum bOp,Operande* bOp1,size_op bSize){
    LigneCode* aux=new LigneCode();
    aux->nat=NA_INST;
    InstrIL* aux1=new InstrIL();
    aux1->opertr=bOp;
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
void IL::add(InsOpEnum bOp){
    LigneCode* aux=new LigneCode();
    aux->nat=NA_INST;
    InstrIL* aux1=new InstrIL();
    aux1->opertr=bOp;
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
void IL::add(const char* bComment){
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

void IL::addLabel(const char* bEtiq){
    LigneCode* aux=new LigneCode();
    aux->nat=NA_ETIQ;
    aux->val.label=bEtiq;

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
Operande* IL::createOp(reg_id bdirectRegister){
    Operande* aux=new Operande();
    aux->nat=OP_DIRECT;
    aux->val.directRegister=bdirectRegister;
    return aux;
}
Operande* IL::createOp(int bdep,reg_id bRegBase){
    Operande* aux=new Operande();
    aux->nat=OP_INDIRECT;
    aux->val.indirect.dep=bdep;
    aux->val.indirect.baseRegister=bRegBase;
    return aux;
}
Operande* IL::createOp(int bdep,reg_id bRegBase,reg_id bRegIndexe){
    Operande* aux=new Operande();
    aux->nat=OP_INDEXE;
    aux->val.indexed.dep=bdep;
    aux->val.indexed.baseRegister=bRegBase;
    aux->val.indexed.indexRegister=bRegIndexe;
    return aux;
}
Operande* IL::createOpVal(int bvalInt){
    Operande* aux=new Operande();
    aux->nat=OP_INTEGER;
    aux->val.valInt=bvalInt;
    return aux;
}
Operande* IL::createOpFloat(float bvalFloat){
    Operande* aux=new Operande();
    aux->nat=OP_FLOAT;
    aux->val.valFloat=bvalFloat;
    return aux;
}
Operande* IL::createOpString(char* bvalChaine){
    Operande* aux=new Operande();
    aux->nat=OP_STRING;
    aux->val.valString=bvalChaine;
    return aux;
}
Operande* IL::createOpLabel(char* bvalEtiq){
    Operande* aux=new Operande();
    aux->nat=OP_LABEL;
    aux->val.valLabel=bvalEtiq;
    return aux;
}

