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
		val.Declared =NULL;
		break;
	case INS_AFFECTATION:
		val.affect.ExprAssigned =NULL;
		val.affect.ExprArbre =NULL;
		break;
	case INS_STRUCT_FOR:
		val.structFOR.ExprAssigned =NULL;
		val.structFOR.ExprArbreINIT =NULL;
		val.structFOR.ExprArbreTO =NULL;
		val.structFOR.ExprArbreSTEP =NULL;
		val.structFOR.Corps =new Collection();
		break;
	case INS_STRUCT_DOLPWH:
		val.structDO.Corps =new Collection();
		val.structDO.ExprCondition =NULL;
		break;
	case INS_STRUCT_DOWH:
		val.structDO.ExprCondition =NULL;
		val.structDO.Corps =new Collection();
		break;
	case INS_IF:
		val.conditionIF.ExprArbre =NULL;

		val.conditionIF.IfCorps =new Collection();
		val.conditionIF.ElseCorps =new Collection();
		val.conditionIF.ElseIfCorps = new Collection();
		val.conditionIF.ExprElseIf = new Collection();
		break;
	case INS_CALL:
		val.CallExpr =NULL;
		break;
	case INS_RETURN:
		val.ReturnExpr =NULL;
		break;
	default:
		break;
	}
}
InstructionETPB::~InstructionETPB(void){}


void InstructionETPB::Detruir()
{

}
void InstructionETPB::Afficher()
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
		val.Declared->Afficher();
		break;
	case INS_AFFECTATION:
		printf("Affectation\n");
		printf("Expression assigned...\n");
		val.affect.ExprAssigned->Afficher();
		printf("Expression arbre...\n");
		val.affect.ExprArbre->Afficher();
		break;
	case INS_STRUCT_FOR:
		printf("For\n");
		printf("Expression assigned...\n");
		if (val.structFOR.ExprAssigned)
			val.structFOR.ExprAssigned->Afficher();
		printf("Expression valeur initiale...\n");
		if (val.structFOR.ExprArbreINIT)
			val.structFOR.ExprArbreINIT->Afficher();
		printf("Expression valeur de fin...\n");
		if (val.structFOR.ExprArbreTO)
			val.structFOR.ExprArbreTO->Afficher();
		printf("Expression valeur de pas...\n");
		if (val.structFOR.ExprArbreSTEP)
			val.structFOR.ExprArbreSTEP->Afficher();
		printf("Corps de la boucle:\n");
		if (val.structFOR.Corps)
			val.structFOR.Corps->Afficher();
		printf("\n....Next\n");
		break;
	case INS_STRUCT_DOLPWH:
		printf("Do\n");
		printf("Corps de la boucle:\n");
		val.structDO.Corps->Afficher();
		printf("LOOP WHILE Condition...\n");
		val.structDO.ExprCondition->Afficher();
		break;
	case INS_STRUCT_DOWH:
		printf("Do\n");
		printf("WHILE Condition...\n");
		val.structDO.ExprCondition->Afficher();
		printf("Corps de la boucle:\n");
		val.structDO.Corps->Afficher();
		printf("LOOP\n");
		break;
	case INS_IF:
		printf("If\n");
		printf("Expression de la condition principale:\n");
		val.conditionIF.ExprArbre->Afficher();

		printf("Corps If:\n");
		val.conditionIF.IfCorps->Afficher();
		listElseIfExpr=val.conditionIF.ExprElseIf;
		listElseIfCorps=val.conditionIF.ElseIfCorps;

		//listElseIfExpr->iteratorInit1();
		//listElseIfCorps->iteratorInit1();
		listElseIfExpr->bindIterator(&iterElseIfExpr);
		listElseIfCorps->bindIterator(&iterElseIfCorps);
		while (iterElseIfCorps.elemExists())
		{
			printf("ElseIf..condition:\n");
			iterElseIfExpr.getNext()->Afficher();
			printf("Corps du ElseIf:\n");
			iterElseIfCorps.getNext()->Afficher();
		}
		if (iterElseIfExpr.elemExists())
		{
			printf("ERREUR!!!!");			// car normalement ils s'annulent en meme temps
		}

		/*
		while (listElseIfExpr)	// normalement ils s'annulent en meme temps
		{
			printf("ElseIf..condition:\n");
			(*(listElseIfExpr->Elem))->Afficher();
			listElseIfExpr=listElseIfExpr->next;
		}

		while (listElseIfCorps)	// normalement ils s'annulent en meme temps
		{
			printf("Corps du ElseIf:\n");
			afficher(*(listElseIfCorps->Elem));
			listElseIfCorps=listElseIfCorps->next;
		}
		*/
		/*
		while (listElseIfExpr && listElseIfCorps)	// normalement ils s'annulent en meme temps
		{
			printf("ElseIf..condition:\n");
			listElseIfExpr->Elem->Afficher();
			listElseIfExpr=listElseIfExpr->next;
			printf("Corps du ElseIf:\n");
			afficher(*(listElseIfCorps->Elem));
			listElseIfCorps=listElseIfCorps->next;
		}
		if (listElseIfExpr || listElseIfCorps)
		{
			printf("ERREUR!!!!");			// car normalement ils s'annulent en meme temps
			if (listElseIfExpr)
				printf("trop d'expression\n");
			if (listElseIfCorps)
				printf("trop de corps\n");
		}*/
		printf("Corps ELSE:\n");
		val.conditionIF.ElseCorps->Afficher();
		break;
	case INS_CALL:
		printf("Call\n");
		val.CallExpr->Afficher();
		break;
	case INS_RETURN:
		printf("Return\n");
		val.ReturnExpr->Afficher();
		break;
	default:
		break;
	}

}

void InstructionETPB::printProblem()
{
	printf("!!! Probleme interne, corruption de la nature d'Instruction\n");
}
