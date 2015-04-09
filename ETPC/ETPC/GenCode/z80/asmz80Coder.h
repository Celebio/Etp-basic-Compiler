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
#ifndef _ASMZ80CODER_H_1
#define _ASMZ80CODER_H_1

#include <stdio.h>
#include <iostream>
#include <fstream>

#include "VirtStackz80.h"
#include "noeud.h"


typedef enum LigneNaturez80 {NAz80_ETIQ,NAz80_INST,NAz80_COMMENT,NAz80_UNKNOWN} LigneNaturez80;
typedef enum OperandeNaturez80 {OPz80_DIRECT,OPz80_INDIRECT,OPz80_INDEXE,OPz80_ENTIER,OPz80_FLOAT,OPz80_CHAINE,OPz80_ETIQ} OperandeNaturez80;
typedef enum size_opz80{ SZz80_UNKNOWN=-1,SZz80_B=1,SZz80_W=2,SZz80_L=4,SZz80_F=10 } size_opz80;

typedef enum InsOpEnumz80{
    z80_OPB,        // unknown opbin
    z80_LOAD,
    z80_ADD,
    z80_SUB,
    z80_JR,
    z80_RTS,
} InsOpEnumz80;

struct Operandez80 {
    OperandeNaturez80 nat;
    bool PreDecr;
    bool PostIncr;
    union {
        regz80_id directRegister;
        struct {
            int dep;
            regz80_id baseRegister;
        } indirect;
        struct {
            int dep;
            regz80_id baseRegister;
            regz80_id indexRegister;
        } indexed;
        int valInt;
        float valFloat;
        char* valString;
        char* valLabel;
    }val;
};

struct InstrILz80{
    InsOpEnumz80 opertr;
    size_opz80 Size;
    Operandez80* op1;
    Operandez80* op2;
};
struct LigneCodez80{
    LigneNaturez80 nat;
    union{
        InstrILz80* instr;
        char* label;
        char* comment;
    } val;
    LigneCodez80* next;
};



class asmz80Coder{
private:
    int nb_regmax;
    LigneCodez80* generatedCode;
    LigneCodez80* lastAdded;
    std::ofstream* mStream;

    void display(LigneCodez80* bLigne);
    void display(regz80_id bReg);
    void display(Operandez80* bOperande);

public:
    asmz80Coder();
    ~asmz80Coder();

    void display();
    void displayHeader();
    void displayFooter();
    void setStream(std::ofstream* bStream) {mStream= bStream; }

};


#endif

