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
#ifndef _INSTRUCTIONETPB_H_1
#define _INSTRUCTIONETPB_H_1

#include "collection.h"
#include "VariableItem.h"
#include "astnode.h"


typedef enum InstrTypeEnum {
    INS_DECLARATION,
    INS_AFFECTATION,
    INS_STRUCT_FOR,
    INS_STRUCT_DOWH,
    INS_STRUCT_DOLPWH,
    INS_IF,
    INS_CALL,
    INS_RETURN,
    INS_UNKNOWN
} InstrTypeEnum;


class InstructionETPB :
    public ColItem
{
private:
    InstrTypeEnum nat;
    union {
        struct {
            ASTNode* exprAssigned;
            ASTNode* exprTree;
        } instrAssignment;

        struct {
            ASTNode* exprTree;
            Collection* ifBody;
            Collection* exprElseIf;
            Collection* elseBody;
            Collection* elseIfBody;
        } instrIfCondition;

        struct {
            ASTNode* exprAssigned;
            ASTNode* exprInitTree;
            ASTNode* exprToTree;
            ASTNode* exprStepTree;
            Collection* body;
        } instrFor;

        struct {
            ASTNode* exprCondition;
            Collection* body;
        } instrDo;

        VariableItem* declaredVariable;
        ASTNode* exprFunctionCall;
        ASTNode* exprReturn;
    } val;

    void printProblem();
public:

    InstructionETPB(void);
    InstructionETPB(InstrTypeEnum bNat);
    ~InstructionETPB(void);

    void destroy();
    void display();

    void ChangeNatINS_STRUCT_DOWH()
    {
        if (nat==INS_STRUCT_DOLPWH)
            nat=INS_STRUCT_DOWH;
        else
        {
            printProblem();
        }
    }

    // get, set
    InstrTypeEnum getNat(){ return nat;}

    /* AFFECTATION  */
    ASTNode* getAssignmentExprAssigned(){
        if (nat==INS_AFFECTATION) return val.instrAssignment.exprAssigned;
        /*...else */printProblem(); return NULL;
    }
    void setAssignmentExprAssigned(ASTNode* bNoeud){
        val.instrAssignment.exprAssigned=bNoeud;
    }
    ASTNode** getAssignmentExprAssignedPtr(){
        if (nat==INS_AFFECTATION) return &(val.instrAssignment.exprAssigned);
        /*...else */printProblem(); return NULL;
    }
    ASTNode* getAssignmentExprTree(){
        if (nat==INS_AFFECTATION) return val.instrAssignment.exprTree;
        /*...else */printProblem(); return NULL;
    }
    ASTNode** getAssignmentExprTreePtr(){
        if (nat==INS_AFFECTATION) return &(val.instrAssignment.exprTree);
        /*...else */printProblem(); return NULL;
    }

    /* IF   */
    ASTNode* getIfExprTree(){
        if (nat==INS_IF) return val.instrIfCondition.exprTree;
        /*...else */printProblem(); return NULL;
    }
    ASTNode** getIfExprTreePtr(){
        if (nat==INS_IF) return &(val.instrIfCondition.exprTree);
        /*...else */printProblem(); return NULL;
    }
    Collection* getIfIfBody(){
        if (nat==INS_IF) return val.instrIfCondition.ifBody;
        /*...else */printProblem(); return NULL;
    }
    Collection* getIfExprElseIf(){
        if (nat==INS_IF) return val.instrIfCondition.exprElseIf;
        /*...else */printProblem(); return NULL;
    }
    Collection* getIfElseBody(){
        if (nat==INS_IF) return val.instrIfCondition.elseBody;
        /*...else */printProblem(); return NULL;
    }
    Collection* getIfElseIfBody(){
        if (nat==INS_IF) return val.instrIfCondition.elseIfBody;
        /*...else */printProblem(); return NULL;
    }

    /* FOR  */
    ASTNode* getForExprAssigned(){
        if (nat==INS_STRUCT_FOR) return val.instrFor.exprAssigned;
        /*...else */printProblem(); return NULL;
    }
    void setForExprAssigned(ASTNode* bNoeud){
        val.instrFor.exprAssigned=bNoeud;
    }
    ASTNode** getForExprAssignedPtr(){
        if (nat==INS_STRUCT_FOR) return &(val.instrFor.exprAssigned);
        /*...else */printProblem(); return NULL;
    }

    ASTNode* getForExprInitTree(){
        if (nat==INS_STRUCT_FOR) return val.instrFor.exprInitTree;
        /*...else */printProblem(); return NULL;
    }
    void setForExprInitTree(ASTNode* bNoeud){
        val.instrFor.exprInitTree=bNoeud;
    }
    ASTNode** getForExprInitTreePtr(){
        if (nat==INS_STRUCT_FOR) return &(val.instrFor.exprInitTree);
        /*...else */printProblem(); return NULL;
    }
    ASTNode* getForExprToTree(){
        if (nat==INS_STRUCT_FOR) return val.instrFor.exprToTree;
        /*...else */printProblem(); return NULL;
    }
    void setForExprToTree(ASTNode* bNoeud)
    {
        val.instrFor.exprToTree=bNoeud;
    }
    ASTNode** getForExprToTreePtr(){
        if (nat==INS_STRUCT_FOR) return &(val.instrFor.exprToTree);
        /*...else */printProblem(); return NULL;
    }
    ASTNode* getForExprStepTree(){
        if (nat==INS_STRUCT_FOR) return val.instrFor.exprStepTree;
        /*...else */printProblem(); return NULL;
    }
    ASTNode** getForExprStepTreePtr(){
        if (nat==INS_STRUCT_FOR) return &(val.instrFor.exprStepTree);
        /*...else */printProblem(); return NULL;
    }
    Collection* getForBody(){
        if (nat==INS_STRUCT_FOR) return val.instrFor.body;
        /*...else */printProblem(); return NULL;
    }

    /*  DO  */
    ASTNode* getDoExprCondition(){
        if (nat==INS_STRUCT_DOWH || nat==INS_STRUCT_DOLPWH) return val.instrDo.exprCondition;
        /*...else */printProblem(); return NULL;
    }
    ASTNode** getDoExprConditionPtr(){
        if (nat==INS_STRUCT_DOWH || nat==INS_STRUCT_DOLPWH) return &(val.instrDo.exprCondition);
        /*...else */printProblem(); return NULL;
    }

    Collection* getDoBody(){
        if (nat==INS_STRUCT_DOWH || nat==INS_STRUCT_DOLPWH) return val.instrDo.body;
        /*...else */printProblem(); return NULL;
    }

    VariableItem* getDeclaredVariable(){
        if (nat==INS_DECLARATION) return val.declaredVariable;
        /*...else */printProblem(); return NULL;
    }
    ASTNode* getExprFunctionCall(){
        if (nat==INS_CALL) return val.exprFunctionCall;
        /*...else */printProblem(); return NULL;
    }
    ASTNode** getExprFunctionCallPtr(){
        if (nat==INS_CALL) return &(val.exprFunctionCall);
        /*...else */printProblem(); return NULL;
    }
    ASTNode* getExprReturn(){
        if (nat==INS_RETURN) return val.exprReturn;
        /*...else */printProblem(); return NULL;
    }
    ASTNode** getExprReturnPtr(){
        if (nat==INS_RETURN) return &(val.exprReturn);
        /*...else */printProblem(); return NULL;
    }
    void setExprReturn(ASTNode* bExpr){
        val.exprReturn=bExpr;
    }

};

#endif
