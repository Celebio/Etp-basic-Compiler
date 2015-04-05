#ifndef _IL_H_1
#define _IL_H_1

#include <stdio.h>
#include "VirtStack.h"
#include "noeud.h"

typedef enum LigneNature {NA_ETIQ,NA_INST,NA_COMMENT,NA_UNKNOWN};
typedef enum OperandeNature {OP_DIRECT,OP_INDIRECT,OP_INDEXE,OP_ENTIER,OP_FLOAT,OP_CHAINE,OP_ETIQ};
typedef enum size_op{ SZ_UNKNOWN=-1,SZ_B=1,SZ_W=2,SZ_L=4,SZ_F=10 };




typedef enum InsOpEnum{
    OPB,        // unknown opbin
    LOAD,
    STORE,
    ADD,
    SUB,
    MUL,
    DIV,
    NOT,
    OPP
};

struct Operande {
    OperandeNature nat;
    union {
        reg_id RegDirect;
        struct {
            int Dep;
            reg_id RegBase;
        } Indirect;
        struct {
            int Dep;
            reg_id RegBase;
            reg_id RegIndexe;
        } Indexe;
        int valInt;
        float valFloat;
        char* valChaine;
        char* valEtiq;
    }val;
};

struct InstrIL{
    InsOpEnum Op;
    size_op Size;
    Operande* op1;
    Operande* op2;
};

struct LigneCode{
    LigneNature nat;
    union{
        InstrIL* instr;
        char* etiq;
        char* comment;
    } val;
    LigneCode* next;
};


class IL{
private:
    int nb_regmax;
    LigneCode* generatedCode;
    LigneCode* lastAdded;
    void Afficher(LigneCode* bLigne);
    void Afficher(reg_id bReg);
    void Afficher(Operande* bOperande);

public:
    IL(void);
    ~IL(void);

    void Afficher();

    InsOpEnum IL::NodeToOp(CNoeud* bNoeud);

    Operande* createOp(reg_id bRegDirect);
    Operande* createOp(int bDep,reg_id bRegBase);
    Operande* createOp(int bDep,reg_id bRegBase,reg_id bRegIndexe);
    Operande* createOpVal(int bvalInt);
    Operande* createOpFloat(float bvalFloat);
    Operande* createOpChaine(char* bvalChaine);
    Operande* createOpEtiq(char* bvalEtiq);



    void Add(InsOpEnum bOp,Operande* bOp1,Operande* bOp2,size_op bSize);
    void Add(InsOpEnum bOp,Operande* bOp1,size_op bSize);
    void Add(InsOpEnum bOp);
    void Add(const char* bComment);
    void AddEtiq(const char* bEtiq);

};

#endif
