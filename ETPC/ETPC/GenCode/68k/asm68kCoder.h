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
#ifndef _ASM68KCODER_H_1
#define _ASM68KCODER_H_1

#include <stdio.h>
#include <iostream>
#include <fstream>


#include "VirtStack68k.h"
#include "astnode.h"

typedef enum LineNature68k {NA_ETIQ,NA_INST,NA_COMMENT,NA_UNKNOWN} LineNature68k;
typedef enum OperandNature68k {OP_DIRECT,OP_INDIRECT,OP_INDEXE,OP_INTEGER,OP_FLOAT,OP_STRING,OP_LABEL} OperandNature68k;
typedef enum size_op68k{ SZ_UNKNOWN=-1,SZ_NA=-2,SZ_B=1,SZ_W=2,SZ_L=4,SZ_F=10 } size_op68k;



typedef enum InsOpEnum68k{
    OPB,        // unknown opbin
    MOVE,
    ADD,
    SUB,
    MUL,
    DIV,
    CMP,
    NOT,
    OPP,
    BSR,
    BRA,

    BEQ,
    BNE,
    BGE,
    BGT,
    BLE,
    BLT,

    SEQ,
    SNE,
    SGE,
    SGT,
    SLE,
    SLT,

    RTS,
} InsOpEnum68k;

struct Operande68k {
    OperandNature68k nat;
    bool PreDecr;
    bool PostIncr;
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
    InsOpEnum68k opertr;
    size_op68k size;
    Operande68k* op1;
    Operande68k* op2;
};

struct LineCode68k{
    LineNature68k nat;
    union{
        InstrIL* instr;
        const char* label;
        const char* comment;
    } val;
    LineCode68k* next;
};


class asm68kCoder{
private:
    int nb_regmax;
    int etiqNo;

    LineCode68k* generatedCode;
    LineCode68k* lastAdded;
    std::ofstream* mStream;
    std::ifstream* iStream;

    void display(LineCode68k* bLigne);
    void display(reg_id bReg);
    void display(Operande68k* bOperande);

public:
    asm68kCoder(void);
    ~asm68kCoder(void);

    void display();
    void displayHeader();
    void displayFooter();

    void setStream(std::ofstream* bStream) {mStream= bStream; }

    InsOpEnum68k nodeToOp(ASTNode* bNoeud);

    Operande68k* createOp(reg_id bdirectRegister);
    Operande68k* createOp(int bdep,reg_id bRegBase);
    Operande68k* createOp(reg_id bRegBase,bool Pred,bool Posti);
    Operande68k* createOp(int bdep,reg_id bRegBase,reg_id bRegIndexe);
    Operande68k* createOpVal(int bvalInt);
    Operande68k* createOpFloat(float bvalFloat);
    Operande68k* createOpString(char* bvalChaine);
    Operande68k* createOpLabel(char* bvalEtiq);
    Operande68k* createOpLabel();


    void add(InsOpEnum68k bOp,Operande68k* bOp1,Operande68k* bOp2,size_op68k bSize);
    void add(InsOpEnum68k bOp,Operande68k* bOp1,size_op68k bSize);
    void add(InsOpEnum68k bOp);
    void add(const char* bComment);
    void addLabel(const char* bEtiq);


    InsOpEnum68k getOppBra(InsOpEnum68k bOp);
};

#endif
