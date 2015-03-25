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
#include "noeud.h"

typedef enum LigneNature68k {NA_ETIQ,NA_INST,NA_COMMENT,NA_UNKNOWN} LigneNature68k;
typedef enum OperandeNature68k {OP_DIRECT,OP_INDIRECT,OP_INDEXE,OP_ENTIER,OP_FLOAT,OP_CHAINE,OP_ETIQ} OperandeNature68k;
typedef enum size_op68k{ SZ_UNKNOWN=-1,SZ_NA=-2,SZ_B=1,SZ_W=2,SZ_L=4,SZ_F=10 } size_op68k;



typedef enum InsOpEnum68k{
	OPB,		// unknown opbin
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
	OperandeNature68k nat;
	bool PreDecr;
	bool PostIncr;
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
	InsOpEnum68k Op;
	size_op68k Size;
	Operande68k* op1;
	Operande68k* op2;
};

struct LigneCode68k{
	LigneNature68k nat;
	union{
		InstrIL* instr;
		char* etiq;
		const char* comment;
	} val;
	LigneCode68k* next;
};


class asm68kCoder{
private:
	int nb_regmax;
	int etiqNo;

	LigneCode68k* generatedCode;
	LigneCode68k* lastAdded;
	std::ofstream* mStream;

	void Afficher(LigneCode68k* bLigne);
	void Afficher(reg_id bReg);
	void Afficher(Operande68k* bOperande);

public:
	asm68kCoder(void);
	~asm68kCoder(void);

	void Afficher();
	void AfficherLeDebut();

	void SetStream(std::ofstream* bStream) {mStream= bStream; }

	InsOpEnum68k NodeToOp(CNoeud* bNoeud);

	Operande68k* createOp(reg_id bRegDirect);
	Operande68k* createOp(int bDep,reg_id bRegBase);
	Operande68k* createOp(reg_id bRegBase,bool Pred,bool Posti);
	Operande68k* createOp(int bDep,reg_id bRegBase,reg_id bRegIndexe);
	Operande68k* createOpVal(int bvalInt);
	Operande68k* createOpFloat(float bvalFloat);
	Operande68k* createOpChaine(char* bvalChaine);
	Operande68k* createOpEtiq(char* bvalEtiq);
	Operande68k* createOpEtiq();


	void Add(InsOpEnum68k bOp,Operande68k* bOp1,Operande68k* bOp2,size_op68k bSize);
	void Add(InsOpEnum68k bOp,Operande68k* bOp1,size_op68k bSize);
	void Add(InsOpEnum68k bOp);
	void Add(const char* bComment);
	void AddEtiq(char* bEtiq);


	InsOpEnum68k getOppBra(InsOpEnum68k bOp);
};

#endif
