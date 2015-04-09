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

#include <stdio.h>

#include "InstructionETPB.h"

InstructionETPB::InstructionETPB(void){
    nat=INS_UNKNOWN;
}
InstructionETPB::InstructionETPB(InstrTypeEnum bNat){
    nat=bNat;
    switch(nat)
    {
    case INS_DECLARATION:
        val.declaredVariable =NULL;
        break;
    case INS_AFFECTATION:
        val.instrAssignment.exprAssigned =NULL;
        val.instrAssignment.exprTree =NULL;
        break;
    case INS_STRUCT_FOR:
        val.instrFor.exprAssigned =NULL;
        val.instrFor.exprInitTree =NULL;
        val.instrFor.exprToTree =NULL;
        val.instrFor.exprStepTree =NULL;
        val.instrFor.body =new Collection();
        break;
    case INS_STRUCT_DOLPWH:
        val.instrDo.body =new Collection();
        val.instrDo.exprCondition =NULL;
        break;
    case INS_STRUCT_DOWH:
        val.instrDo.exprCondition =NULL;
        val.instrDo.body =new Collection();
        break;
    case INS_IF:
        val.instrIfCondition.exprTree =NULL;

        val.instrIfCondition.ifBody =new Collection();
        val.instrIfCondition.elseBody =new Collection();
        val.instrIfCondition.elseIfBody = new Collection();
        val.instrIfCondition.exprElseIf = new Collection();
        break;
    case INS_CALL:
        val.exprFunctionCall =NULL;
        break;
    case INS_RETURN:
        val.exprReturn =NULL;
        break;
    default:
        break;
    }
}
InstructionETPB::~InstructionETPB(void){}


void InstructionETPB::destroy()
{

}
void InstructionETPB::display()
{
    Collection* listElseIfExpr;
    Collection* listElseIfCorps;
    ColIterator iterElseIfExpr;
    ColIterator iterElseIfCorps;
    printf("INSTRUCTION----");
    switch(nat)
    {
    case INS_DECLARATION:
        printf("Declaration\n");
        val.declaredVariable->display();
        break;
    case INS_AFFECTATION:
        printf("Affectation\n");
        printf("Expression assigned...\n");
        val.instrAssignment.exprAssigned->display();
        printf("Expression arbre...\n");
        val.instrAssignment.exprTree->display();
        break;
    case INS_STRUCT_FOR:
        printf("For\n");
        printf("Expression assigned...\n");
        if (val.instrFor.exprAssigned)
            val.instrFor.exprAssigned->display();
        printf("Expression valeur initiale...\n");
        if (val.instrFor.exprInitTree)
            val.instrFor.exprInitTree->display();
        printf("Expression valeur de fin...\n");
        if (val.instrFor.exprToTree)
            val.instrFor.exprToTree->display();
        printf("Expression valeur de pas...\n");
        if (val.instrFor.exprStepTree)
            val.instrFor.exprStepTree->display();
        printf("body de la boucle:\n");
        if (val.instrFor.body)
            val.instrFor.body->display();
        printf("\n....Next\n");
        break;
    case INS_STRUCT_DOLPWH:
        printf("Do\n");
        printf("body de la boucle:\n");
        val.instrDo.body->display();
        printf("LOOP WHILE Condition...\n");
        val.instrDo.exprCondition->display();
        break;
    case INS_STRUCT_DOWH:
        printf("Do\n");
        printf("WHILE Condition...\n");
        val.instrDo.exprCondition->display();
        printf("body de la boucle:\n");
        val.instrDo.body->display();
        printf("LOOP\n");
        break;
    case INS_IF:
        printf("If\n");
        printf("Expression de la condition principale:\n");
        val.instrIfCondition.exprTree->display();

        printf("body If:\n");
        val.instrIfCondition.ifBody->display();
        listElseIfExpr=val.instrIfCondition.exprElseIf;
        listElseIfCorps=val.instrIfCondition.elseIfBody;

        //listElseIfExpr->iteratorInit1();
        //listElseIfCorps->iteratorInit1();
        listElseIfExpr->bindIterator(&iterElseIfExpr);
        listElseIfCorps->bindIterator(&iterElseIfCorps);
        while (iterElseIfCorps.elemExists())
        {
            printf("ElseIf..condition:\n");
            iterElseIfExpr.getNext()->display();
            printf("body du ElseIf:\n");
            iterElseIfCorps.getNext()->display();
        }
        if (iterElseIfExpr.elemExists())
        {
            printf("ERREUR!!!!");           // car normalement ils s'annulent en meme temps
        }

        /*
        while (listElseIfExpr)  // normalement ils s'annulent en meme temps
        {
            printf("ElseIf..condition:\n");
            (*(listElseIfExpr->Elem))->display();
            listElseIfExpr=listElseIfExpr->next;
        }

        while (listElseIfCorps) // normalement ils s'annulent en meme temps
        {
            printf("body du ElseIf:\n");
            afficher(*(listElseIfCorps->Elem));
            listElseIfCorps=listElseIfCorps->next;
        }
        */
        /*
        while (listElseIfExpr && listElseIfCorps)   // normalement ils s'annulent en meme temps
        {
            printf("ElseIf..condition:\n");
            listElseIfExpr->Elem->display();
            listElseIfExpr=listElseIfExpr->next;
            printf("body du ElseIf:\n");
            afficher(*(listElseIfCorps->Elem));
            listElseIfCorps=listElseIfCorps->next;
        }
        if (listElseIfExpr || listElseIfCorps)
        {
            printf("ERREUR!!!!");           // car normalement ils s'annulent en meme temps
            if (listElseIfExpr)
                printf("trop d'expression\n");
            if (listElseIfCorps)
                printf("trop de corps\n");
        }*/
        printf("body ELSE:\n");
        val.instrIfCondition.elseBody->display();
        break;
    case INS_CALL:
        printf("Call\n");
        val.exprFunctionCall->display();
        break;
    case INS_RETURN:
        printf("Return\n");
        val.exprReturn->display();
        break;
    default:
        break;
    }

}

void InstructionETPB::printProblem()
{
    printf("!!! Probleme interne, corruption de la nature d'Instruction\n");
}
