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


#ifndef _VERISYN_H_1
//#define _VERISYN_H_1

//#include "TAG.h"
//#include "astnode.h"
//#include "Collection.h"

#include "VariableItem.h"
#include "DimElemItem.h"
#include "TypeItem.h"
#include "functionitem.h"



typedef enum enumETAT { ST_DEHORS,ST_INTRA,
            ST_P00,ST_P01,ST_P02,ST_P05,ST_P06,ST_P07,ST_P08,
            ST_T00,ST_T01,ST_T02,ST_T03,ST_T04,ST_T07,ST_T08,ST_T09,ST_T10,ST_T11,ST_T12,
            ST_PROC00,ST_PROC01,ST_PROC02,ST_PROC03,ST_PROC04,ST_PROC05,ST_PROC06,ST_PROC07,ST_PROC08,
            ST_FUNC00,ST_FUNC01,ST_FUNC02,ST_FUNC03,ST_FUNC04,ST_FUNC05,ST_FUNC06,ST_FUNC07,ST_FUNC08,
            ST_FUNC09,ST_FUNC10
            } enumETAT;
typedef enum enumETATInstr {INST_00,INST_IDENTIF,INST_IDOPENPAR,INST_IDCLOSEPAR,
            INST_AFFECT,INST_IDOPENCRO,INST_IDCLOSECRO,INST_IDDEBPAR,INST_IDPOINT,
            INST_RET00,
            INST_FOR00,INST_FOR01,INST_FOR02,INST_FOR03,INST_FOR04,INST_FOR05,
            INST_FOR06,INST_FOR07,INST_FOR08,INST_FOR09,
            INST_DO01,INST_DO02,INST_DO03,INST_DO04,INST_DO05,INST_DO06,INST_DO07,
            INST_LOCAL00,INST_LOCAL01,INST_LOCAL02,INST_LOCAL03,INST_LOCAL04,
            INST_LOCAL05,INST_LOCAL06,INST_LOCAL07,
            INST_IF00,INST_IF01,INST_IF02,INST_IF03,INST_IF04,INST_IF05,INST_IF06,
            INST_IF07,INST_IF08,INST_IF09,INST_IF10
            } enumETATInstr;

typedef enum enumETATExpr {EXST_00,EXST_01,EXST_02,EXST_03,EXST_04,EXST_05,
            EXST_06,EXST_07,EXST_08,EXST_09,EXST_10,EXST_11,EXST_12,EXST_13,
            EXST_14,EXST_15,EXST_16,EXST_17,EXST_FIN,EXST_FIN2} enumETATExpr;

class VeriSyn
{
private:
    TAG* tagListe;
    Collection* errListe;
    Collection VariablesPublic;
    Collection Types;
    Collection Fonctions;

    bool readingPredef;

    void AvanceNextLigne(TAG** bTAG);
    bool IsAType(TAG* bTag);
    VarTypeType GetVarType(TAG* bTag);
    void NetoyerArbreParentheses(ASTNode *CurNoeud);
    bool VerifyExpression(bool requis,
                      TAG* debTag,
                      TAG* finTag,
                      bool moinsPermis,
                      ASTNode** ArbreExpress);
    void VerifyInstruction(TAG* debTag,
                       TAG* finTag,
                       FunctionItem* pFunc,
                       Collection* instrCollect);

public:

    VeriSyn(TAG* bListe,Collection* bErrListe);
    ~VeriSyn(void);
    void VerifSyntax();

    Collection* GetVariablesPublicPtr(){return &VariablesPublic;}
    Collection* GetTypesPtr(){return &Types;}
    Collection* GetFonctionsPtr(){return &Fonctions;}
};


#endif

