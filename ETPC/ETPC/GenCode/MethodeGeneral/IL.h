#ifndef _IL_H_1
#define _IL_H_1

#include <stdio.h>
#include "VirtStack.h"
#include "astnode.h"

typedef enum LigneNature {NA_ETIQ,NA_INST,NA_COMMENT,NA_UNKNOWN};
typedef enum OperandeNature {OP_DIRECT,OP_INDIRECT,OP_INDEXE,OP_INTEGER,OP_FLOAT,OP_STRING,OP_LABEL};
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
        reg_id directRegister;
        struct {
            int dep;
            reg_id baseRegister;
        } indirect;
        struct {
            int dep;
            reg_id baseRegister;
            reg_id indexRegister;
        } indexed;
        int valInt;
        float valFloat;
        char* valString;
        char* valLabel;
    }val;
};

struct InstrIL{
    InsOpEnum opertr;
    size_op size;
    Operande* op1;
    Operande* op2;
};

struct LigneCode{
    LigneNature nat;
    union{
        InstrIL* instr;
        char* label;
        char* comment;
    } val;
    LigneCode* next;
};


class IL{
private:
    int nb_regmax;
    LigneCode* generatedCode;
    LigneCode* lastAdded;
    void display(LigneCode* bLigne);
    void display(reg_id bReg);
    void display(Operande* bOperande);

public:
    IL(void);
    ~IL(void);

    void display();

    InsOpEnum nodeToOp(ASTNode* bNoeud);

    Operande* createOp(reg_id bdirectRegister);
    Operande* createOp(int bdep,reg_id bRegBase);
    Operande* createOp(int bdep,reg_id bRegBase,reg_id bRegIndexe);
    Operande* createOpVal(int bvalInt);
    Operande* createOpFloat(float bvalFloat);
    Operande* createOpString(char* bvalChaine);
    Operande* createOpLabel(char* bvalEtiq);



    void add(InsOpEnum bOp,Operande* bOp1,Operande* bOp2,size_op bSize);
    void add(InsOpEnum bOp,Operande* bOp1,size_op bSize);
    void add(InsOpEnum bOp);
    void add(const char* bComment);
    void addLabel(const char* bEtiq);

};

#endif
