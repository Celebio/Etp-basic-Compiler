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

const char* imageOp68k[]={
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
InsOpEnum68k asm68kCoder::nodeToOp(CNoeud* bNoeud)
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
    default:
        break;
    }

    return OPB;
}


/**********************************************************
    AFFICHAGE
    *******************************************************************/

void asm68kCoder::display(reg_id bReg){
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


void asm68kCoder::display(Operande68k* bOperande){
    if (bOperande==NULL)
        return;

    if (bOperande->nat==OP_INTEGER){
        (*mStream) << "#" << bOperande->val.valInt;
    }
    else if (bOperande->nat==OP_FLOAT){
        (*mStream) << "#" << bOperande->val.valFloat;
    }
    else if (bOperande->nat==OP_STRING){
        (*mStream) << "#\"" << bOperande->val.valString << "\"";
    }
    else if (bOperande->nat==OP_DIRECT){
        display(bOperande->val.directRegister);
    }
    else if (bOperande->nat==OP_INDIRECT){
        if (bOperande->PreDecr)
            (*mStream) << "-";
        if (bOperande->val.indirect.dep!=0)
            (*mStream) << bOperande->val.indirect.dep;
        else {
        }
            (*mStream) << "(";
            display(bOperande->val.indirect.baseRegister);
            (*mStream) << ")";
            if (bOperande->PostIncr){
                (*mStream) << "+";
            }
    }
    else if (bOperande->nat==OP_INDEXE){
        display(bOperande->val.indexed.indexRegister);
        (*mStream) << bOperande->val.indirect.dep << "(";
        display(bOperande->val.indexed.baseRegister);
        (*mStream) << ")";
    }
    else if (bOperande->nat==OP_LABEL){
        (*mStream) << bOperande->val.valLabel;
    }
}

void asm68kCoder::displayHeader(){
    (*mStream) << ";Made with ETP\n\tinclude \"os.h\"\n\txdef _main\n\txdef _nostub\n\txdef _ti89\n\txdef _ti92plus\n\t\n\n";
}

void asm68kCoder::displayFooter(){
    iStream = new ifstream("etplib.asm");
    string line;
    if (iStream->is_open())
    {
        while ( getline (*iStream,line) )
        {
          *mStream << line << '\n';
        }
        iStream->close();
    }
    delete iStream;
}

void asm68kCoder::display(){
    LineCode68k* Cour=generatedCode;
    while (Cour){
        display(Cour);
        Cour=Cour->next;
    }
}

void asm68kCoder::display(LineCode68k* bLigne){
    int sz;
    if (bLigne==NULL)
        return;

    switch(bLigne->nat){
        case NA_ETIQ:
            (*mStream) << bLigne->val.label << ":";
            break;
        case NA_INST:
            (*mStream) << "\t";
            (*mStream) << imageOp68k[bLigne->val.instr->opertr];
            sz=bLigne->val.instr->Size;
            if (sz==SZ_UNKNOWN){
                (*mStream) << ".?";
            }else if (sz==SZ_NA){
                (*mStream) << "";
            }else if (sz==SZ_B){
                (*mStream) << ".B";
            }else if (sz==SZ_W){
                (*mStream) << ".W";
            }else if (sz==SZ_L){
                (*mStream) << ".L";
            }else if (sz==SZ_F){
                (*mStream) << ".F";
            }
            (*mStream) << "\t";
            display(bLigne->val.instr->op1);
            if (bLigne->val.instr->op2){
                (*mStream) << ",";
                display(bLigne->val.instr->op2);
            }
            break;
        case NA_COMMENT:
            (*mStream) << "\t;" << bLigne->val.comment;
            break;
        case NA_UNKNOWN:
            break;
    }
    (*mStream) << "\n";
}

/**********************************************************
    AJOUT D'INSTRUCTION
    *******************************************************************/

void asm68kCoder::add(InsOpEnum68k bOp,Operande68k* bOp1,Operande68k* bOp2,size_op68k bSize){
    LineCode68k* aux=new LineCode68k();
    aux->nat=NA_INST;
    InstrIL* aux1=new InstrIL();
    aux1->opertr=bOp;
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
    display();
#endif
}
void asm68kCoder::add(InsOpEnum68k bOp,Operande68k* bOp1,size_op68k bSize){
    LineCode68k* aux=new LineCode68k();
    aux->nat=NA_INST;
    InstrIL* aux1=new InstrIL();
    aux1->opertr=bOp;
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
    display();
#endif
}
void asm68kCoder::add(InsOpEnum68k bOp){
    LineCode68k* aux=new LineCode68k();
    aux->nat=NA_INST;
    InstrIL* aux1=new InstrIL();
    aux1->opertr=bOp;
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
    display();
#endif
}
void asm68kCoder::add(const char* bComment){
    LineCode68k* aux=new LineCode68k();
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
    display();
#endif
}

void asm68kCoder::addLabel(const char* bEtiq){
    LineCode68k* aux=new LineCode68k();
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
#ifdef AFF_EACH_INST
    display();
#endif
}


/**********************************************************
    CREATION OPERANDE
    *******************************************************************/
Operande68k* asm68kCoder::createOp(reg_id bdirectRegister){
    Operande68k* aux=new Operande68k();
    aux->nat=OP_DIRECT;
    aux->PostIncr=false;
    aux->PreDecr=false;
    aux->val.directRegister=bdirectRegister;
    return aux;
}
Operande68k* asm68kCoder::createOp(int bdep,reg_id bRegBase){
    Operande68k* aux=new Operande68k();
    aux->nat=OP_INDIRECT;
    aux->PostIncr=false;
    aux->PreDecr=false;
    aux->val.indirect.dep=bdep;
    aux->val.indirect.baseRegister=bRegBase;
    return aux;
}
Operande68k* asm68kCoder::createOp(reg_id bRegBase,bool Pred,bool Posti){
    Operande68k* aux=new Operande68k();
    aux->nat=OP_INDIRECT;
    aux->PostIncr=Pred;
    aux->PreDecr=Posti;
    aux->val.indirect.dep=0;
    aux->val.indirect.baseRegister=bRegBase;
    return aux;
}
Operande68k* asm68kCoder::createOp(int bdep,reg_id bRegBase,reg_id bRegIndexe){
    Operande68k* aux=new Operande68k();
    aux->nat=OP_INDEXE;
    aux->PostIncr=false;
    aux->PreDecr=false;
    aux->val.indexed.dep=bdep;
    aux->val.indexed.baseRegister=bRegBase;
    aux->val.indexed.indexRegister=bRegIndexe;
    return aux;
}
Operande68k* asm68kCoder::createOpVal(int bvalInt){
    Operande68k* aux=new Operande68k();
    aux->nat=OP_INTEGER;
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
Operande68k* asm68kCoder::createOpString(char* bvalChaine){
    Operande68k* aux=new Operande68k();
    aux->nat=OP_STRING;
    aux->PostIncr=false;
    aux->PreDecr=false;
    aux->val.valString=bvalChaine;
    return aux;
}
Operande68k* asm68kCoder::createOpLabel(char* bvalEtiq){
    Operande68k* aux=new Operande68k();
    aux->nat=OP_LABEL;
    aux->PostIncr=false;
    aux->PreDecr=false;
    aux->val.valLabel=bvalEtiq;
    return aux;
}

Operande68k* asm68kCoder::createOpLabel(){
    Operande68k* aux=new Operande68k();
    // g?n?ration automatique des ?tiquettes distinctes
    // to do: a transformer en string de stdlib
    char* newString = (char*)malloc(300);
    sprintf(newString,"%s%i",".sysetiq",etiqNo);
    etiqNo++;

    aux->nat=OP_LABEL;
    aux->PostIncr=false;
    aux->PreDecr=false;
    aux->val.valLabel=newString;
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
