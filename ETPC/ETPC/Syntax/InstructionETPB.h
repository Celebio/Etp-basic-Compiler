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
#include "noeud.h"


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
            CNoeud* ExprAssigned;               // expression de la chose affect?
            CNoeud* ExprArbre;                  // l'arbre de l'expression de l'affectation
        } affect;

        struct {
            CNoeud* ExprArbre;                                      // l'expression de la condition du IF
            Collection* IfCorps;                    // le corps de ce qu'il faut executer pour le IF
            Collection* ExprElseIf;                         // les expressions des elseIf
            Collection* ElseCorps;                  // le corps de ce qu'il faut executer pour le else
            Collection* ElseIfCorps;    // les corps des diff?rents bloc des endif
        } conditionIF;

        struct {
            CNoeud* ExprAssigned;                   // expression de la chose affect?
            CNoeud* ExprArbreINIT;                  // l'arbre de l'expression de l'affectation
            CNoeud* ExprArbreTO;                    // l'expression de la limite "To ..."
            CNoeud* ExprArbreSTEP;                  // l'expression de la limite "To ..."
            Collection* Corps;      // le corps de ce qu'il faut executer dans la boucle
        } structFOR;

        struct {
            CNoeud* ExprCondition;                  // l'arbre de la condition
            Collection* Corps;      // le corps de ce qu'il faut executer dans la boucle
        } structDO;

        VariableItem* Declared;     // la variable d?clar?e
        CNoeud* CallExpr;       // l'expression de l'appel de procedure
        CNoeud* ReturnExpr;     // l'expression retourn?e
    } val;

    void printProblem();
public:

    InstructionETPB(void);
    InstructionETPB(InstrTypeEnum bNat);
    ~InstructionETPB(void);

    // les oblig?s
    void Detruir();
    void Afficher();

    // divers
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
    CNoeud* GetAffectExprAssigned(){
        if (nat==INS_AFFECTATION) return val.affect.ExprAssigned;
        /*...else */printProblem(); return NULL;
    }
    void SetAffectExprAssigned(CNoeud* bNoeud){
        val.affect.ExprAssigned=bNoeud;
    }
    CNoeud** GetAffectExprAssignedPtr(){
        if (nat==INS_AFFECTATION) return &(val.affect.ExprAssigned);
        /*...else */printProblem(); return NULL;
    }
    CNoeud* GetAffectExprArbre(){
        if (nat==INS_AFFECTATION) return val.affect.ExprArbre;
        /*...else */printProblem(); return NULL;
    }
    CNoeud** GetAffectExprArbrePtr(){
        if (nat==INS_AFFECTATION) return &(val.affect.ExprArbre);
        /*...else */printProblem(); return NULL;
    }

    /* IF   */
    CNoeud* GetIFExprArbre(){
        if (nat==INS_IF) return val.conditionIF.ExprArbre;
        /*...else */printProblem(); return NULL;
    }
    CNoeud** GetIFExprArbrePtr(){
        if (nat==INS_IF) return &(val.conditionIF.ExprArbre);
        /*...else */printProblem(); return NULL;
    }
    Collection* GetIFIfCorps(){
        if (nat==INS_IF) return val.conditionIF.IfCorps;
        /*...else */printProblem(); return NULL;
    }
    Collection* GetIFExprElseIf(){
        if (nat==INS_IF) return val.conditionIF.ExprElseIf;
        /*...else */printProblem(); return NULL;
    }
    Collection* GetIFElseCorps(){
        if (nat==INS_IF) return val.conditionIF.ElseCorps;
        /*...else */printProblem(); return NULL;
    }
    Collection* GetIFElseIfCorps(){
        if (nat==INS_IF) return val.conditionIF.ElseIfCorps;
        /*...else */printProblem(); return NULL;
    }

    /* FOR  */
    CNoeud* GetFORExprAssigned(){
        if (nat==INS_STRUCT_FOR) return val.structFOR.ExprAssigned;
        /*...else */printProblem(); return NULL;
    }
    void SetFORExprAssigned(CNoeud* bNoeud){
        val.structFOR.ExprAssigned=bNoeud;
    }
    CNoeud** GetFORExprAssignedPtr(){
        if (nat==INS_STRUCT_FOR) return &(val.structFOR.ExprAssigned);
        /*...else */printProblem(); return NULL;
    }

    CNoeud* GetFORExprArbreINIT(){
        if (nat==INS_STRUCT_FOR) return val.structFOR.ExprArbreINIT;
        /*...else */printProblem(); return NULL;
    }
    void SetFORExprArbreINIT(CNoeud* bNoeud){
        val.structFOR.ExprArbreINIT=bNoeud;
    }
    CNoeud** GetFORExprArbreINITPtr(){
        if (nat==INS_STRUCT_FOR) return &(val.structFOR.ExprArbreINIT);
        /*...else */printProblem(); return NULL;
    }
    CNoeud* GetFORExprArbreTO(){
        if (nat==INS_STRUCT_FOR) return val.structFOR.ExprArbreTO;
        /*...else */printProblem(); return NULL;
    }
    void SetFORExprArbreTO(CNoeud* bNoeud)
    {
        val.structFOR.ExprArbreTO=bNoeud;
    }
    CNoeud** GetFORExprArbreTOPtr(){
        if (nat==INS_STRUCT_FOR) return &(val.structFOR.ExprArbreTO);
        /*...else */printProblem(); return NULL;
    }
    CNoeud* GetFORExprArbreSTEP(){
        if (nat==INS_STRUCT_FOR) return val.structFOR.ExprArbreSTEP;
        /*...else */printProblem(); return NULL;
    }
    CNoeud** GetFORExprArbreSTEPPtr(){
        if (nat==INS_STRUCT_FOR) return &(val.structFOR.ExprArbreSTEP);
        /*...else */printProblem(); return NULL;
    }
    Collection* GetFORCorps(){
        if (nat==INS_STRUCT_FOR) return val.structFOR.Corps;
        /*...else */printProblem(); return NULL;
    }

    /*  DO  */
    CNoeud* GetDOExprCondition(){
        if (nat==INS_STRUCT_DOWH || nat==INS_STRUCT_DOLPWH) return val.structDO.ExprCondition;
        /*...else */printProblem(); return NULL;
    }
    CNoeud** GetDOExprConditionPtr(){
        if (nat==INS_STRUCT_DOWH || nat==INS_STRUCT_DOLPWH) return &(val.structDO.ExprCondition);
        /*...else */printProblem(); return NULL;
    }

    Collection* GetDOCorps(){
        if (nat==INS_STRUCT_DOWH || nat==INS_STRUCT_DOLPWH) return val.structDO.Corps;
        /*...else */printProblem(); return NULL;
    }

    VariableItem* GetDeclDeclared(){
        if (nat==INS_DECLARATION) return val.Declared;
        /*...else */printProblem(); return NULL;
    }
    CNoeud* GetCallExpr(){
        if (nat==INS_CALL) return val.CallExpr;
        /*...else */printProblem(); return NULL;
    }
    CNoeud** GetCallExprPtr(){
        if (nat==INS_CALL) return &(val.CallExpr);
        /*...else */printProblem(); return NULL;
    }
    CNoeud* GetReturnExpr(){
        if (nat==INS_RETURN) return val.ReturnExpr;
        /*...else */printProblem(); return NULL;
    }
    CNoeud** GetReturnExprPtr(){
        if (nat==INS_RETURN) return &(val.ReturnExpr);
        /*...else */printProblem(); return NULL;
    }
    void SetReturnExpr(CNoeud* bExpr){
        val.ReturnExpr=bExpr;
    }

};

#endif
