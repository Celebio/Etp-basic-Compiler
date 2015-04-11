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
/*********************************************************************************\
    verisyn.cpp

    Ce fichier permet d'analyser la syntaxe. Il prend en entr?e une suite de TAG
    et donne des erreurs lorsque la syntaxe n'est pas conforme ? l'ETP-Basic.
    Il permet ?galement de d?clarer les types et les variables en meme temps qu'il
    analyse ? travers des collections.

\*********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "verisyn.h"
#include "InstructionETPB.h"


#define _DEBUGARBRES


VeriSyn::VeriSyn(TAG* bListe,Collection* bErrListe)
{
    tagListe=bListe;
    errListe=bErrListe;
    readingPredef=false;
}
VeriSyn::~VeriSyn(void){}
void VeriSyn::AvanceNextLigne(TAG** bTAG)
{
    while ((*bTAG)->GetToken() != TOKEN_CRLF && ((*bTAG)->GetNext())!= NULL)
        (*bTAG)=(*bTAG)->GetNext();
}
bool VeriSyn::IsAType(TAG* bTag)
{
    if (bTag->GetToken() >= TOKEN_TYPEINTEGER  &&
        bTag->GetToken() <= TOKEN_TYPEBYTE)
        return true;
    else
        return false;
}
VarTypeType VeriSyn::GetVarType(TAG* bTag)
{
    VarTypeType RetVal;
    if (bTag == NULL)
    {
        RetVal.Type = TP_VOID;  // Ca sert uniquement pour construire un type VOID
        RetVal.Expr = NULL;
    }
    else
    {
        enumTokenType bToken = bTag->GetToken();
        if (bToken>=TOKEN_TYPEINTEGER && bToken<=TOKEN_TYPEBYTE)
            RetVal.Type = (VarTypeEnum)((int)TP_INTEGER + (bToken-TOKEN_TYPEINTEGER));
        else    // normalement c'est forc?ment un identifiant dans ce cas
        {
            RetVal.Type = TP_USER;
            RetVal.Expr = bTag->GetIdentif();
        }
    }
    return RetVal;
}
void VeriSyn::NetoyerArbreParentheses(CNoeud *CurNoeud)
{
    CNoeud* filsD;
    CNoeud* filsG;
    if (CurNoeud==NULL) return;

    filsD=CurNoeud->getRightChild();
    filsG=CurNoeud->getLeftChild();

    if (filsD)
    {
        if (filsD->getTag()->GetToken() == TOKEN_OPENPAR)
        {
            CurNoeud->setRightChild(filsD->getRightChild());
            delete filsD;
        }
        NetoyerArbreParentheses(CurNoeud->getRightChild());

    }
    if (filsG)
    {
        if (filsG->getTag()->GetToken() == TOKEN_OPENPAR)
        {
            CurNoeud->setLeftChild(filsG->getRightChild());
            delete filsG;
        }
        NetoyerArbreParentheses(CurNoeud->getLeftChild());
    }

    return;
}


bool VeriSyn::VerifyExpression(bool requis,
                      TAG* debTag,
                      TAG* finTag,
                      bool moinsPermis,
                      CNoeud** ArbreExpress)
{
#ifdef _DEBUGARBRES
    printf("verifying expression...\n");
#endif
    TAG* current=debTag;
    TAG* currentEnd;
    TAG* precCur=current;
    int mCtr,cCtr,argCtr;
    CNoeud* Arbre=*ArbreExpress;
    CNoeud* lastAdded=NULL;
    CNoeud* aux;
    CNoeud* courNoeud;
    CNoeud* precNoeud;
    //typeOperator crTokType;
    if (debTag==finTag && requis)
    {
        //*ArbreExpress=NULL;
        //return true;
        errListe->add("On attend une expression",precCur);
        return false;
    }
    while (current && current!=(finTag))
    {
#ifdef _DEBUGARBRES
        current->display();
#endif


        aux = new CNoeud(current);
        //crTokType=aux->getOperType();


        if (aux->getNature()==NODE_UNKNOWN)
        {
            errListe->add("Mot reserve non permis dans une expression",current);
            return false;
        }
        if (!Arbre)
        {
            if (current->GetToken()==TOKEN_MOINS)      // si le premier terme est un token_moins, ce token_moins est un operateur unaire droite
            {
                aux->setOperatorUnaryMinus();
            }
            if (aux->getOperType()==OPBIN || aux->getOperType()==OPUNG)
            {
                // on a commenc? par un op?rateur binaire ou unaire_gauche-> faux
                errListe->add("Inattendu ici ",current);
                return false;
            }
            else
            {
                Arbre = aux;
                if (current->GetToken()==TOKEN_OPENCRO)
                {
                    errListe->add("Crochet non permis ici",current);
                    return false;
                }
                if (current->GetToken()==TOKEN_OPENPAR)
                {   // une expression entre parentheses
                    lastAdded=Arbre;
                    currentEnd = current;
                    mCtr = 0;
                    cCtr = 0;
                    while (!(( currentEnd->GetToken() == TOKEN_CLOSEPAR && mCtr==1 && cCtr==0)  || currentEnd == finTag))
                    {
                        if (currentEnd->GetToken() == TOKEN_OPENPAR)           mCtr++;
                        else if (currentEnd->GetToken() == TOKEN_CLOSEPAR)     mCtr--;
                        else if (currentEnd->GetToken() == TOKEN_OPENCRO)      cCtr++;
                        else if (currentEnd->GetToken() == TOKEN_CLOSECRO)     cCtr--;
                        currentEnd = currentEnd->GetNext();
                    }
                    if (currentEnd==finTag)
                    {
                        errListe->add("Il manque un ')' ",current);
                        return false;
                    }
                    aux=lastAdded->getRightChild();
                    VerifyExpression(false,current->GetNext(),currentEnd,true,&aux);
                    #ifdef _DEBUGARBRES
                        printf("dondum openpar basta\n");
                    #endif
                    Arbre->setRightChild(aux);
                    current=currentEnd;
                }
            }
        }
        else
        {
            if (aux->getOperType()==OPBIN || aux->getOperType()==OPUNG)
            {
                courNoeud=Arbre;
                precNoeud=Arbre;
                // verifications pour ne pas avoir deux op?rateurs incorrectes ? la suite du genre +*
                // ? faire..

                // insertion du noeud de l'op?rateur ? la bonne place en fonction de sa priorit?
                while (courNoeud->getOperator()> getOperatorType(current->GetToken()))
                {
                    precNoeud=courNoeud;
                    courNoeud=courNoeud->getRightChild();
                }
                if (precNoeud==Arbre && courNoeud==Arbre)
                {
                    Arbre=aux;
                    aux->setLeftChild(courNoeud);
                }
                else
                {
                    aux->setLeftChild(courNoeud);
                    precNoeud->setRightChild(aux);
                }
            }
            if (aux->getOperType()==OPUND)
            {
                //if (!(GetOperatorType(lastAdded->getTag())==OPBIN || GetOperatorType(lastAdded->getTag())==OPUND))
                if (!(lastAdded->getOperType()==OPBIN || lastAdded->getOperType()==OPUND))
                {
                    errListe->add("Operateur mal place ",current);
                    return false;
                }
                if (lastAdded->getRightChild()==NULL)
                    lastAdded->setRightChild(aux);
                else
                {   // impossible normalement
                    errListe->add("Erreur interne ",current);
                    return false;
                }
            }
            else if (aux->getOperType()==NOTOP)
            {
                //if (lastAdded)
                if (lastAdded->getTag()->GetToken()!=TOKEN_OPENPAR && lastAdded->getOperType()!=OPBIN && lastAdded->getOperType()!=OPUND && lastAdded->getOperType()!=NOTOP)
                {
                    errListe->add("Inattendu: Pas d'operateur pour cet operande ",current);
                    return false;
                }

                // une expression entre parentheses
                if (current->GetToken()==TOKEN_OPENPAR && lastAdded->getOperType()!=NOTOP)
                {
                    if (lastAdded->getRightChild()==NULL)    // ajout de l'operateur virtuel
                    {
                        lastAdded->setRightChild(aux);
                        lastAdded=aux;
                    }
                    currentEnd = current;
                    mCtr = 0;
                    cCtr = 0;
                    while (!(( currentEnd->GetToken() == TOKEN_CLOSEPAR && mCtr==1 && cCtr==0)  || currentEnd == finTag))
                    {
                        if (currentEnd->GetToken() == TOKEN_OPENPAR)           mCtr++;
                        else if (currentEnd->GetToken() == TOKEN_CLOSEPAR)     mCtr--;
                        else if (currentEnd->GetToken() == TOKEN_OPENCRO)      cCtr++;
                        else if (currentEnd->GetToken() == TOKEN_CLOSECRO)     cCtr--;
                        currentEnd = currentEnd->GetNext();
                    }
                    if (currentEnd==finTag)
                    {
                        errListe->add("Il manque un ')' ",current);
                        return false;
                    }
                    aux=lastAdded->getRightChild();
                    VerifyExpression(false,current->GetNext(),currentEnd,true,&aux);
                    #ifdef _DEBUGARBRES
                        printf("dondum \n");
                    #endif
                    current=currentEnd;
                    if (lastAdded->getRightChild()==NULL)
                        lastAdded->setRightChild(aux);
                }
                // appel de fonction
                else if (current->GetToken()==TOKEN_OPENPAR && lastAdded->getOperType()==NOTOP)
                {
                    #ifdef _DEBUGARBRES
                        printf("appel de fonction\n");
                    #endif
                    currentEnd = current;
                    mCtr = 0;
                    cCtr = 0;
                    argCtr=0;
                    while (!( (currentEnd->GetToken() == TOKEN_CLOSEPAR && mCtr==1 && cCtr==0)  || currentEnd == finTag ))
                    {
                        if (currentEnd->GetToken() == TOKEN_OPENPAR)           mCtr++;
                        else if (currentEnd->GetToken() == TOKEN_CLOSEPAR)     mCtr--;
                        else if (currentEnd->GetToken() == TOKEN_OPENCRO)      cCtr++;
                        else if (currentEnd->GetToken() == TOKEN_CLOSECRO)     cCtr--;
                        currentEnd = currentEnd->GetNext();

                        if ((currentEnd->GetToken() == TOKEN_VIRGULE && mCtr==1 && cCtr==0) || (currentEnd->GetToken() == TOKEN_CLOSEPAR && mCtr==1 && cCtr==0))
                        {
                            #ifdef _DEBUGARBRES
                                printf("verification de l'expression pour l'argument %i\n",argCtr+1);
                            #endif
                            VerifyExpression(false,current->GetNext(),currentEnd,true,lastAdded->getSuccPtr(argCtr));
                            argCtr++;
                            current=currentEnd;
                        }
                    }
                    if (currentEnd==finTag)
                    {
                        errListe->add("Il manque un ')' ",current);
                        return false;
                    }
                    lastAdded->setRightChild(NULL);
                    lastAdded->setLeftChild(NULL);
                    //argCtr=0;
                    //while (lastAdded->Successeur[argCtr])
                    //{
                    //  lastAdded->Successeur[argCtr]->display();
                    //  argCtr++;
                    //}

                    lastAdded->setAsFunction();
                    delete aux;
                    current=currentEnd;
                }
                // appel d'un tableau
                else if (current->GetToken()==TOKEN_OPENCRO && lastAdded->getOperType()==NOTOP)
                {
                    #ifdef _DEBUGARBRES
                        printf("appel d'un tableau\n");
                    #endif
                    currentEnd = current;
                    mCtr = 0;
                    cCtr = 0;
                    argCtr=0;
                    while (!( (currentEnd->GetToken() == TOKEN_CLOSECRO && mCtr==0 && cCtr==1)  || currentEnd == finTag ))
                    {
                        if (currentEnd->GetToken() == TOKEN_OPENPAR)           mCtr++;
                        else if (currentEnd->GetToken() == TOKEN_CLOSEPAR)     mCtr--;
                        else if (currentEnd->GetToken() == TOKEN_OPENCRO)      cCtr++;
                        else if (currentEnd->GetToken() == TOKEN_CLOSECRO)     cCtr--;
                        currentEnd = currentEnd->GetNext();

                        if ((currentEnd->GetToken() == TOKEN_VIRGULE && mCtr==0 && cCtr==1) || (currentEnd->GetToken() == TOKEN_CLOSECRO && mCtr==0 && cCtr==1))
                        {
                            #ifdef _DEBUGARBRES
                                printf("verification de l'expression pour l'index %i\n",argCtr+1);
                            #endif
                            VerifyExpression(true,current->GetNext(),currentEnd,true,lastAdded->getSuccPtr(argCtr));
                            argCtr++;
                            current=currentEnd;
                        }
                    }
                    if (currentEnd==finTag)
                    {
                        errListe->add("Il manque un ']' ",current);
                        return false;
                    }
                    lastAdded->setRightChild(NULL);
                    lastAdded->setLeftChild(NULL);
                    lastAdded->setAsArray();
                    delete aux;
                    current=currentEnd;
                }
                else if (current->GetToken()==TOKEN_OPENCRO && lastAdded->getOperType()!=NOTOP)
                {
                    errListe->add("Crochet non permis ici ",current);
                    return false;
                }
                else
                {
                    if (lastAdded->getRightChild()==NULL)
                        lastAdded->setRightChild(aux);
                    else
                    {
                        // impossible normalement
                        errListe->add("Erreur interne1 ",current);
                        return false;
                    }
                }
            }

        }
        lastAdded = aux;

#ifdef _DEBUGARBRES
        if (Arbre)
            Arbre->display();
        printf("\n\n");
#endif
        precCur = current;
        current = current->GetNext();
    }// fin while

    //if (GetOperatorType(precCur)==OPBIN || GetOperatorType(precCur)==OPUND)
    //{
    //  errListe->add("Un manque un operande pour cet operateur",precCur);
    //  return false;
    //}
    NetoyerArbreParentheses(Arbre);     // il faut absolument mettre ca en dehors du while
#ifdef _DEBUGARBRES
    if (Arbre)
        Arbre->display();
    printf("\n");
#endif
    *ArbreExpress=Arbre;
    return true;
}


void VeriSyn::VerifyInstruction(TAG* debTag,
                       TAG* finTag,
                       FonctionItem* pFunc,
                       Collection* instrCollect)
{

    TAG* current=debTag;
    TAG* currentEnd;
    TAG* precCur=current;
    TAG* rpCour;
    enumETATInstr State=INST_00;
    int mCtr,cCtr;
    int ifCtr;
    TAG* debugCour;
    TAG* debExpression;
    TAG* aux;
    bool ifFound;
    InstructionETPB* instrCour;     // l'instruction que nous allons d?chiffrer

    // variables passageres pour les declarations
    VariableItem* tmpVar=NULL;
    DimElemItem* tmpDim=NULL;
    TypeItem* tmpType=NULL;
    FonctionItem* tmpFonc=NULL;

    while (current && current!=finTag)
    {
        switch (State)
        {
        case INST_00:
            debugCour = NULL;
            if (current->GetToken() == TOKEN_CRLF)
            {
                State = INST_00;
            }
            else if (current->GetToken() == TOKEN_DO)
            {
                debugCour = current;
                State = INST_DO01;
            }
            else if (current->GetToken() == TOKEN_IDENTIF)
                State = INST_IDENTIF;
            else if (current->GetToken() == TOKEN_LOCAL)
                State = INST_LOCAL00;
            else if (current->GetToken() == TOKEN_RETURN)
                State = INST_RET00;
            else if (current->GetToken() == TOKEN_FOR)
            {
                debugCour = current;
                State = INST_FOR00;
            }
            else if (current->GetToken() == TOKEN_IF)
            {
                debugCour = current;
                State = INST_IF00;
            }
            else if (current->GetToken() == TOKEN_PUBLIC)
            {
                errListe->add("Declaration publique dans un corps ",current);
                AvanceNextLigne(&current);
                State = INST_00;
            }
            else
            {
                errListe->add("Inattendu dans une fonction: ",current);
                AvanceNextLigne(&current);
                State = INST_00;
            }
            break;
        // -------------------------------------------------------------------------
        // GESTION DES DO... -------------------------------------------------------
        // -------------------------------------------------------------------------
        case INST_DO01:
            instrCour = new InstructionETPB(INS_STRUCT_DOLPWH);
            instrCollect->add(instrCour);
            if (current->GetToken() == TOKEN_CRLF)
            {
                State = INST_DO02;
                rpCour = current;
            }
            else if (current->GetToken() == TOKEN_WHILE)
                State = INST_DO05;
            else
            {
                errListe->add("Apres 'Do' on attend 'While' ou rien",current);
                AvanceNextLigne(&current);
                State = INST_00;
            }
            break;
        case INST_DO02:
            ifCtr = 0;
            ifFound = false;
            currentEnd = current;
            precCur = currentEnd;
            while (!(ifCtr==-1 || (currentEnd->GetToken()==TOKEN_LOOP && ifCtr==0) || currentEnd==finTag))
            {
                if (currentEnd->GetToken() == TOKEN_DO)
                    ifCtr++;
                else if (currentEnd->GetToken() == TOKEN_LOOP)
                {
                    ifCtr--;
                }
                precCur = currentEnd;
                currentEnd = currentEnd->GetNext();
            }
            if (currentEnd == finTag)
            {
                errListe->add("Il manque 'Loop' (Do) ",debugCour);
                State = INST_00;
                current = rpCour;
                break;
            }
            else
            {
                VerifyInstruction(current,currentEnd,pFunc,instrCour->getDoBody());
                State = INST_DO03;
            }
            current = currentEnd;
            break;
        case INST_DO03:
            if (current->GetToken() == TOKEN_WHILE)
                State = INST_DO04;
            else
            {
                errListe->add("On attend 'While'",current);
                AvanceNextLigne(&current);
                State = INST_00;
                break;
            }
            break;
        case INST_DO04:
            if (current->GetToken() == TOKEN_CRLF)
            {
                errListe->add("On attend une expression de condition apres 'While'",current);
                AvanceNextLigne(&current);
                State = INST_00;
                break;
            }
            // il faut avoir expression suivi de CRLF
            currentEnd = current;
            while (!(currentEnd->GetToken() == TOKEN_CRLF || currentEnd == finTag))
                currentEnd = currentEnd->GetNext();

            // signaler les erreurs dans l'expression
            VerifyExpression(true,current,currentEnd,true,instrCour->getDoExprConditionPtr());
            State = INST_00;
            current = currentEnd;
            rpCour = current;
            break;
        case INST_DO05:
            rpCour = current;
            instrCour->ChangeNatINS_STRUCT_DOWH();
            if (current->GetToken() == TOKEN_CRLF)
            {
                errListe->add("On attend une expression de condition apres 'While'",current);
                AvanceNextLigne(&current);
                State = INST_DO06;
                break;
            }
            // il faut avoir expression suivi de CRLF
            currentEnd = current;
            while (!(currentEnd->GetToken() == TOKEN_CRLF || currentEnd == finTag))
                currentEnd = currentEnd->GetNext();

            // signaler les erreurs dans l'expression
            VerifyExpression(true,current,currentEnd,true,instrCour->getDoExprConditionPtr());
            State = INST_DO06;
            current = currentEnd;
            break;
        case INST_DO06:
            ifCtr = 0;
            ifFound = false;
            currentEnd = current;
            precCur = currentEnd;
            while (!(ifCtr==-1 || (currentEnd->GetToken()==TOKEN_LOOP && ifCtr==0) || currentEnd==finTag))
            {
                if (currentEnd->GetToken() == TOKEN_DO)
                    ifCtr++;
                else if (currentEnd->GetToken() == TOKEN_LOOP)
                {
                    ifCtr--;
                }
                precCur = currentEnd;
                currentEnd = currentEnd->GetNext();
            }
            if (currentEnd == finTag)
            {
                errListe->add("Il manque 'Loop' (Do) ",debugCour);
                State = INST_00;
                current = rpCour;
                break;
            }
            else
            {
                VerifyInstruction(current,currentEnd,pFunc,instrCour->getDoBody());
                State = INST_DO07;
            }
            current = currentEnd;
            break;
        case INST_DO07:
            if (!(current->GetToken() == TOKEN_CRLF))
            {
                errListe->add("Inattendu apres 'Loop':",current);
                AvanceNextLigne(&current);
            }
            State = INST_00;
            break;


        // -------------------------------------------------------------------------
        // GESTION DES DECLARATIONS LOCALES ... ------------------------------------
        // -------------------------------------------------------------------------
        case INST_LOCAL00:
            /* comment? car pas besoin d'ajouter les déclarations en tant qu'instruction    */
            //instrCour = new InstructionETPB(INS_DECLARATION);
            //instrCollect->add(instrCour);
            if (current->GetToken() == TOKEN_IDENTIF)
            {
                tmpVar= new VariableItem();
                pFunc->getVariableList()->add(tmpVar);
                tmpVar->SetCar(VR_PUBLIC);
                tmpVar->SetFunc(NULL);
                tmpVar->setTagName(current);
                tmpVar->SetTagType(NULL);
                State = INST_LOCAL01;
            }
            else
            {
                errListe->add("On attend un identifiant:",current);
                AvanceNextLigne(&current);
                State = INST_00;
            }
            break;
        case INST_LOCAL01:
            if (current->GetToken() == TOKEN_AS)
                State = INST_LOCAL06;
            else if (current->GetToken() == TOKEN_OPENCRO)
                State = INST_LOCAL02;
            else
            {
                errListe->add("On attend 'as' ou '['",current);
                AvanceNextLigne(&current);
                State = INST_00;
            }
            break;
        case INST_LOCAL02:
            currentEnd = current;
            mCtr = 0;
            cCtr = 0;
            while (!(((currentEnd->GetToken() == TOKEN_CLOSECRO || currentEnd->GetToken() == TOKEN_VIRGULE) && mCtr==0 && cCtr==0)  || currentEnd == finTag))
            {
                if (currentEnd->GetToken() == TOKEN_OPENPAR)
                    mCtr++;
                else if (currentEnd->GetToken() == TOKEN_CLOSEPAR)
                    mCtr--;
                else if (currentEnd->GetToken() == TOKEN_OPENCRO)
                    cCtr++;
                else if (currentEnd->GetToken() == TOKEN_CLOSECRO)
                    cCtr--;
                currentEnd = currentEnd->GetNext();
            }
            if (current == currentEnd)
            {
                errListe->add("Vous devez sp?cifier la dimension",current);
                AvanceNextLigne(&current);
                State = INST_00;
                break;
            }
            if (currentEnd->GetToken()==TOKEN_CLOSECRO)
            {
                tmpDim = new DimElemItem();
                tmpDim->SetExprNull();
                VerifyExpression(true,current,currentEnd,false,tmpDim->GetExprPtr());
                tmpVar->GetDimListe()->add(tmpDim);
                State= INST_LOCAL05;
                current=currentEnd;
            }
            else if (currentEnd->GetToken()==TOKEN_VIRGULE)
            {
                tmpDim = new DimElemItem();
                tmpDim->SetExprNull();
                VerifyExpression(true,current,currentEnd,false,tmpDim->GetExprPtr());
                tmpVar->GetDimListe()->add(tmpDim);
                State= INST_LOCAL02;
                current=currentEnd;
            }
            else
            {
                errListe->add("On attend une expression de dimension",current);
                AvanceNextLigne(&current);
                State = INST_00;
            }
            break;
        case INST_LOCAL05:
            if (current->GetToken() == TOKEN_AS)
                State = INST_LOCAL06;
            else
            {
                errListe->add("On attend 'as' ",current);
                AvanceNextLigne(&current);
                State = INST_00;
                break;
            }
            break;
        case INST_LOCAL06:
            if (current->GetToken() == TOKEN_IDENTIF || IsAType(current))
            {
                tmpVar->setType(GetVarType(current));
                tmpVar->SetTagType(current);
                State = INST_LOCAL07;
            }
            else
            {
                errListe->add("On attend un type ",current);
                AvanceNextLigne(&current);
                State = INST_00;
            }
            break;
        case INST_LOCAL07:
            if (current->GetToken() == TOKEN_CRLF)
            {
                // On a d?clar? une variable locale
                tmpVar->SetCar(VR_LOCAL);
                AvanceNextLigne(&current);
                State = INST_00;
            }
            else
            {
                errListe->add("Inattendu ",current);
                AvanceNextLigne(&current);
                State = INST_00;
            }
            break;
        // -------------------------------------------------------------------------
        // GESTION DE RETURN... ------------------------------------------------------
        // -------------------------------------------------------------------------        case INST_FOR00:
        case INST_RET00:
            instrCour = new InstructionETPB(INS_RETURN);
            instrCollect->add(instrCour);
            currentEnd = current;
            // il faut avoir une expression puis CRLF
            while (!(currentEnd->GetToken() == TOKEN_CRLF || currentEnd == finTag))
                currentEnd = currentEnd->GetNext();
            // signaler les erreurs dans l'expression
            VerifyExpression(false,current,currentEnd,true,instrCour->getExprReturnPtr());
            State = INST_00;
            current = currentEnd;
            break;
        // -------------------------------------------------------------------------
        // GESTION DES FOR... ------------------------------------------------------
        // -------------------------------------------------------------------------        case INST_FOR00:
        case INST_FOR00:
            instrCour = new InstructionETPB(INS_STRUCT_FOR);
            instrCollect->add(instrCour);

            if (current->GetToken() == TOKEN_IDENTIF)
            {
                debExpression = current;   // c'est le premier GetToken() du exprAssigned
                currentEnd = current;
                // il faut avoir =, sans aller ? la line

                while (!(currentEnd->GetToken() == TOKEN_EQUAL || currentEnd->GetToken() == TOKEN_CRLF || currentEnd == finTag))
                    currentEnd = currentEnd->GetNext();
                if (currentEnd->GetToken() != TOKEN_EQUAL)
                {
                    errListe->add("On attend '=' ",current);
                    AvanceNextLigne(&current);
                    State = INST_FOR08;
                    break;
                }
                else
                {
                    // signaler les erreurs dans l'expression
                    VerifyExpression(true,current,currentEnd,true,instrCour->getForExprAssignedPtr());
                }
                current = currentEnd;
                rpCour = current;
                State = INST_FOR05;
                break;
            }
            else if (current->GetToken() == TOKEN_EQUAL)
            {
                errListe->add("On attend une variable a affecter ",current);
                AvanceNextLigne(&current);
                State = INST_FOR08;
                rpCour = current;
            }
            else
            {
                errListe->add("On attend une affectation ",current);
                AvanceNextLigne(&current);
                State = INST_FOR08;
                rpCour = current;
            }
            break;
        case INST_FOR05:
            // on a eu un "=" pour arriver ici

            // il faut avoir expr.TO, sans aller ? la line
            debExpression= current;
            currentEnd = current;
            while (!(currentEnd->GetToken() == TOKEN_TO || currentEnd->GetToken() == TOKEN_CRLF || currentEnd == finTag))
                currentEnd = currentEnd->GetNext();
            if (currentEnd->GetToken() != TOKEN_TO)
            {
                errListe->add("On attend 'To' ",current);
                AvanceNextLigne(&current);
                State = INST_FOR08;
            }
            else
            {
                // construire l'arbre de l'expression entre "=" et "TO"
                VerifyExpression(true,debExpression,currentEnd,true,instrCour->getForExprInitTreePtr());
                State = INST_FOR06;
            }
            current = currentEnd;
            rpCour = current;
            break;
        case INST_FOR06:
            currentEnd = current;
            debExpression= current;
            // il faut avoir Step, sans aller ? la line
            while (!(currentEnd->GetToken() == TOKEN_STEP || currentEnd->GetToken() == TOKEN_CRLF || currentEnd == finTag))
                currentEnd = currentEnd->GetNext();
            if (currentEnd == finTag)
            {
                errListe->add("On attend une expression pour la fin",current);
                AvanceNextLigne(&current);
                State = INST_FOR08;
            }
            else
            {
                // construire l'arbre de l'expression entre TO et STEP ou entre TO et CRLF
                VerifyExpression(true,debExpression,currentEnd,true,instrCour->getForExprToTreePtr());

                if (currentEnd->GetToken() == TOKEN_STEP)
                    State = INST_FOR07;
                else
                    State = INST_FOR08;
            }
            current = currentEnd;
            rpCour = current;
            break;
        case INST_FOR07:
            currentEnd = current;
            debExpression= current;
            // il faut aller ? la line
            while (!(currentEnd->GetToken() == TOKEN_CRLF || currentEnd == finTag))
                currentEnd = currentEnd->GetNext();
            if (currentEnd == finTag)
            {
                errListe->add("On attend une expression pour la fin",current);
                AvanceNextLigne(&current);
                State = INST_FOR08;
            }
            else
            {
                // construire l'arbre de l'expression entre STEP et CRLF
                VerifyExpression(true,debExpression,currentEnd,true,instrCour->getForExprStepTreePtr());
                State = INST_FOR08;
            }
            current = currentEnd;
            rpCour = current;
            break;
        case INST_FOR08:
            ifCtr = 0;
            ifFound = false;
            currentEnd = current;
            precCur = currentEnd;
            while (!(ifCtr==-1 || (currentEnd->GetToken()==TOKEN_NEXT && ifCtr==0) || currentEnd==finTag))
            {
                if (currentEnd->GetToken() == TOKEN_FOR)
                    ifCtr++;
                else if (currentEnd->GetToken() == TOKEN_NEXT)
                {
                    ifCtr--;
                }
                precCur = currentEnd;
                currentEnd = currentEnd->GetNext();
            }
            if (currentEnd == finTag)
            {
                errListe->add("Il manque 'Next' (For) ",debugCour);
                State = INST_00;
                current = rpCour;
                break;
            }
            else
            {
                VerifyInstruction(current,currentEnd,pFunc,instrCour->getForBody());
                State = INST_FOR09;
            }
            current = currentEnd;
            break;
        case INST_FOR09:
            if (current->GetToken() == TOKEN_CRLF)
                State = INST_00;
            else
            {
                errListe->add("Inattendu apres 'Next' ",current);
                AvanceNextLigne(&current);
                State = INST_00;
            }
            break;

        // -------------------------------------------------------------------------
        // GESTION DES IF... ------------------------------------------------------
        // -------------------------------------------------------------------------
        case INST_IF00:

            instrCour = new InstructionETPB(INS_IF);
            instrCollect->add(instrCour);

            currentEnd = current;
            // il faut avoir Then, sans aller ? la line

            while (!(currentEnd->GetToken() == TOKEN_THEN || currentEnd->GetToken() == TOKEN_CRLF || currentEnd == finTag))
                currentEnd = currentEnd->GetNext();
            if (currentEnd->GetToken() != TOKEN_THEN)
            {
                errListe->add("On attend 'Then' ",current);
                AvanceNextLigne(&current);
                State = INST_00;
            }
            else
            {
                // signaler les erreurs dans l'expression
                VerifyExpression(true,current,currentEnd,true,instrCour->getIfExprTreePtr());
                State = INST_IF01;
            }
            current = currentEnd;
            rpCour = current;
            break;
        case INST_IF01:
            if (current->GetToken() == TOKEN_CRLF)
            {
                State = INST_IF02;
            }
            else
            {
                errListe->add("Inattendu apres 'Then' ",current);
                AvanceNextLigne(&current);
                State = INST_IF02;
            }
            break;

        case INST_IF02:
            ifCtr = 0;
            ifFound = false;
            currentEnd = current;
            precCur = currentEnd;
            while (!(ifCtr==-1 || (currentEnd->GetToken()==TOKEN_ELSEIF && ifCtr==0) || (currentEnd->GetToken()==TOKEN_ELSE && ifCtr==0) || currentEnd==finTag))
            {
                if (currentEnd->GetToken() == TOKEN_IF && precCur->GetToken() != TOKEN_END)
                    ifFound = true;
                else if (currentEnd->GetToken() == TOKEN_CRLF)
                {
                    if (ifFound)
                        ifCtr++;
                    ifFound = false;
                }
                else if (currentEnd->GetToken() == TOKEN_END)
                {
                    if (currentEnd->GetNext() != finTag)
                    {
                        if (currentEnd->GetNext()->GetToken() == TOKEN_IF)
                        {
                            ifCtr--;
                            ifFound = false;
                        }
                    }
                }
                precCur = currentEnd;
                currentEnd = currentEnd->GetNext();
            }
            if (currentEnd == finTag)
            {
                errListe->add("Il manque 'End If'  ",debugCour);
                State = INST_00;
                current = rpCour;
            }
            else if (currentEnd->GetToken() == TOKEN_ELSEIF)
            {
                if (debugCour->GetToken()==TOKEN_IF)
                {
                    VerifyInstruction(current,currentEnd,pFunc,instrCour->getIfIfBody());
                }
                else if (debugCour->GetToken()==TOKEN_ELSEIF)
                {
                    Collection* CorpsT=new Collection();
                    VerifyInstruction(current,currentEnd,pFunc,CorpsT);
                    instrCour->getIfElseIfBody()->add(CorpsT);
                }
                State = INST_IF03;
                debugCour = currentEnd;
                current = currentEnd;
            }
            else if (currentEnd->GetToken() == TOKEN_ELSE)
            {
                if (debugCour->GetToken()==TOKEN_IF)
                {
                    VerifyInstruction(current,currentEnd,pFunc,instrCour->getIfIfBody());
                }
                else if (debugCour->GetToken()==TOKEN_ELSEIF)
                {
                    Collection* CorpsT=new Collection();
                    VerifyInstruction(current,currentEnd,pFunc,CorpsT);
                    instrCour->getIfElseIfBody()->add(CorpsT);
                }
                State = INST_IF04;
                debugCour = currentEnd;
                current = currentEnd;
            }
            else    // on a rencontr? End If
            {
                if (debugCour->GetToken()==TOKEN_IF)
                {
                    VerifyInstruction(current,precCur,pFunc,instrCour->getIfIfBody());
                }
                else if (debugCour->GetToken()==TOKEN_ELSEIF)
                {
                    Collection* CorpsT=new Collection();
                    VerifyInstruction(current,precCur,pFunc,CorpsT);
                    instrCour->getIfElseIfBody()->add(CorpsT);
                }
                State = INST_IF05;
                current = currentEnd;
            }
            break;
        case INST_IF03:
            currentEnd = current;
            // il faut avoir Then, sans aller ? la line

            while (!(currentEnd->GetToken() == TOKEN_THEN || currentEnd->GetToken() == TOKEN_CRLF || currentEnd == finTag))
                currentEnd = currentEnd->GetNext();
            if (currentEnd->GetToken() != TOKEN_THEN)
            {
                errListe->add("On attend 'Then' ",current);
                current = rpCour;
                State = INST_00;
                break;
            }
            else
            {
                // signaler les erreurs dans l'expression

                //VerifyExpression(true,current,currentEnd,true,instrCour->getIfExprTreePtr());
                CNoeud* exprElseIf=NULL;
                VerifyExpression(true,current,currentEnd,true,&exprElseIf);
                instrCour->getIfExprElseIf()->add(exprElseIf);
                State = INST_IF01;
            }
            current = currentEnd;
            rpCour = current;
            break;
        case INST_IF04:
            ifCtr = 0;
            ifFound = false;
            currentEnd = current;
            precCur = currentEnd;
            while (!(ifCtr==-1 || (currentEnd->GetToken()==TOKEN_ELSEIF && ifCtr==0) || (currentEnd->GetToken()==TOKEN_ELSE && ifCtr==0) || currentEnd==finTag))
            {
                if (currentEnd->GetToken() == TOKEN_IF && precCur->GetToken() != TOKEN_END)
                    ifFound = true;
                else if (currentEnd->GetToken() == TOKEN_CRLF)
                {
                    if (ifFound)
                        ifCtr++;
                    ifFound = false;
                }
                else if (currentEnd->GetToken() == TOKEN_END)
                {
                    if (currentEnd->GetNext() != finTag)
                    {
                        if (currentEnd->GetNext()->GetToken() == TOKEN_IF)
                        {
                            ifCtr--;
                            ifFound = false;
                        }
                    }
                }
                precCur = currentEnd;
                currentEnd = currentEnd->GetNext();
            }
            if (currentEnd == finTag)
            {
                errListe->add("Il manque 'End If'  ",debugCour);
                State = INST_00;
                current = rpCour;
            }
            else if (currentEnd->GetToken() == TOKEN_ELSEIF)
            {
                errListe->add("Impossible d'avoir 'ElseIf' suite ? 'Else' ",debugCour);
                State = INST_00;
                current = rpCour;
            }
            else if (currentEnd->GetToken() == TOKEN_ELSE)
            {
                errListe->add("Impossible d'avoir 'Else' suite ? 'Else' ",debugCour);
                State = INST_00;
                current = rpCour;
            }
            else    // on a rencontr? End If
            {
                VerifyInstruction(current,precCur,pFunc,instrCour->getIfElseBody());
                State = INST_IF05;
                current = currentEnd;
            }
            break;
        case INST_IF05:
            if (current->GetToken() == TOKEN_CRLF)
            {
                State = INST_00;
            }
            else
            {
                errListe->add("Inattendu apres 'End If': ",current);
                AvanceNextLigne(&current);
                State = INST_00;
            }
            break;
        // -------------------------------------------------------------------------
        // GESTION DES DECLARATIONS AUTRES ... ------------------------------------
        // -------------------------------------------------------------------------
        case INST_IDENTIF:

            if (current->GetToken() == TOKEN_CRLF)
            {   // appel de proc?dure sans argument

                instrCour = new InstructionETPB(INS_CALL);
                instrCollect->add(instrCour);
                VerifyExpression(true,precCur,current,true,instrCour->getExprFunctionCallPtr());
                State = INST_00;
                break;
            }
            if (current->GetToken() == TOKEN_OPENPAR){
                State = INST_IDOPENPAR;
                current=precCur;
                break;
            }
            currentEnd=current;
            while (currentEnd->GetToken()!=TOKEN_EQUAL && currentEnd !=finTag && currentEnd->GetToken()!=TOKEN_CRLF)
            {
                currentEnd=currentEnd->GetNext();
            }
            if (currentEnd==finTag || currentEnd->GetToken()==TOKEN_CRLF) // on n'a pas trouv? de TOKEN_EQUAL
            {
                // lecture des arguments sans parenthese
                // on va lui faire croire qu'il y a des parentheses
                // sauf s'il y a deja

                aux=new TAG;
                *aux=*current;
                aux->SetToken(TOKEN_OPENPAR);
                aux->SetNext(current);
                precCur->SetNext(aux);
                currentEnd=aux;
                while (currentEnd->GetToken() != TOKEN_CRLF)
                {
                    current=currentEnd;
                    currentEnd=currentEnd->GetNext();
                }
                aux=new TAG;
                *aux=*current;
                aux->SetToken(TOKEN_CLOSEPAR);
                current->SetNext(aux);
                aux->SetNext(currentEnd);
                current=precCur;
                State = INST_IDOPENPAR;
            }
            else        // on a trouv? un TOKEN_EQUAL
            {
                instrCour = new InstructionETPB(INS_AFFECTATION);
                instrCollect->add(instrCour);

                VerifyExpression(true,precCur,currentEnd,true,instrCour->getAssignmentExprAssignedPtr());
                current=currentEnd->GetNext();
                currentEnd=current;
                while (currentEnd !=finTag && currentEnd->GetToken()!=TOKEN_CRLF)
                {
                    currentEnd=currentEnd->GetNext();
                }
                VerifyExpression(true,current,currentEnd,true,instrCour->getAssignmentExprTreePtr());
                current=currentEnd;
                State = INST_00;
            }
            break;
        case INST_IDOPENPAR:
            if (current->GetToken() == TOKEN_CLOSEPAR && precCur->GetToken() == TOKEN_OPENPAR)
            {
                State = INST_IDCLOSEPAR;
                break;
            }
            currentEnd = current;
            mCtr = 0;
            cCtr = 0;
            while (!((currentEnd->GetToken() == TOKEN_CLOSEPAR && mCtr==1 && cCtr==0) || (currentEnd->GetToken() == TOKEN_VIRGULE && mCtr==0 && cCtr==0)  || currentEnd->GetToken() == TOKEN_CRLF))
            {
                if (currentEnd->GetToken() == TOKEN_OPENPAR)
                    mCtr++;
                else if (currentEnd->GetToken() == TOKEN_CLOSEPAR)
                    mCtr--;
                else if (currentEnd->GetToken() == TOKEN_OPENCRO)
                    cCtr++;
                else if (currentEnd->GetToken() == TOKEN_CLOSECRO)
                    cCtr--;
                currentEnd = currentEnd->GetNext();
            }
            if (currentEnd->GetToken() == TOKEN_CRLF)
            {
                errListe->add("Il manque un ')' ",current);
                AvanceNextLigne(&current);
                State = INST_00;
                break;
            }
            if (current == currentEnd)
            {
                errListe->add("Vous devez sp?cifier l'argument apres une , ",current);
                AvanceNextLigne(&current);
                State = INST_00;
                break;
            }
            // il faut v?rifier l'expression entre les parentheses
            instrCour = new InstructionETPB(INS_CALL);
            instrCollect->add(instrCour);
            if (!(VerifyExpression(true,precCur,currentEnd->GetNext(),true,instrCour->getExprFunctionCallPtr())))
            {
                AvanceNextLigne(&current);
                State = INST_00;
                break;
            }

            current = currentEnd;
            if (current->GetToken() == TOKEN_VIRGULE)
                State = INST_IDOPENPAR;
            else
                State = INST_IDCLOSEPAR;
            break;
        case INST_IDCLOSEPAR:
            if (current->GetToken() == TOKEN_CRLF)
            {
                State = INST_00;
            }
            else
            {
                errListe->add("Inattendu: ",current);
                AvanceNextLigne(&current);
                State = INST_00;
            }
            break;
        default:
            break;
        }   // fin switch
        precCur=current;
        current=current->GetNext();
    }

}

void VeriSyn::VerifSyntax()
{
    // etat automate
    enumETAT State = ST_DEHORS;

    // parcours
    TAG* current=tagListe;
    TAG* currentEnd;
    TAG* precCur;
    bool Found;
    int mCtr,cCtr;

    // variables temporaires pour les declarations
    VariableItem* tmpVar=NULL;
    DimElemItem* tmpDim=NULL;
    TypeItem* tmpType=NULL;
    FonctionItem* tmpFonc=NULL;
    /*
    Variable tmpVar;
    Collection<DimElem> tmpDimCollect;  // les dimensions d'une variable
    Collection<Variable> tmpVarCollect; // les variables d'un type
    Collection<InstructionType> tmpInstrCollect;    // la collection d'instruction
    DimElem tmpDim;
    TypeStruct tmpType;
    Fonction tmpFonc;
    Fonction* tmpFoncPtr;
    Fonction* pInFunc;
    */

    while (current)
    {

        readingPredef= current->GetisPredefFileTAG();      // est-ce que c'est un fichier de predef?
        switch (State)
        {
        case ST_INTRA:
            // on est ? l'int?rieur d'une proc?dure
            // on doit avoir une suite d'instruction

            // sauf si on est dans un fichier de predefinition, on est toujours a l'exterieur
            if (readingPredef){
                State = ST_DEHORS;
                break;
            }
            currentEnd = current;
            precCur = current;
            Found = false;
            while (currentEnd && !(currentEnd->GetToken()==TOKEN_PROCEDURE || currentEnd->GetToken()==TOKEN_FUNCTION || currentEnd->GetToken()==TOKEN_ENDOFMODULE))
            {
                precCur = currentEnd;
                currentEnd = currentEnd->GetNext();
            }
            //tmpInstrCollect.Clear();
            /*
            tmpInstrCollect.SetBuffer(NULL);
            VerifyInstruction(current,currentEnd,errListe,pInFunc,&tmpInstrCollect);
            pInFunc->instructionList=tmpInstrCollect.GetBuffer();
            */
            VerifyInstruction(current,currentEnd,tmpFonc,tmpFonc->getInstructionList());
            current = precCur;
            State = ST_DEHORS;
            break;
        case ST_DEHORS:
            if (current->GetToken() == TOKEN_PUBLIC)
                State = ST_P00;
            else if (current->GetToken() == TOKEN_TYPET)
            {
                State = ST_T00;
                // il faut v?rifier l'existance de End Type
                currentEnd = current;
                Found = false;
                while (currentEnd && !Found)
                {
                    if (currentEnd->GetNext())
                    {
                        if (currentEnd->GetNext()->GetNext())
                            if (currentEnd->GetToken()==TOKEN_END && currentEnd->GetNext()->GetToken()==TOKEN_TYPET)
                                Found = true;
                    }
                    currentEnd = currentEnd->GetNext();
                }
                if (!Found)
                {
                    errListe->add("'Type' sans 'End Type'",current);
                    AvanceNextLigne(&current);
                    State = ST_DEHORS;
                }
            }
            else if (current->GetToken() == TOKEN_CRLF)
            {
                State = ST_DEHORS;
            }
            else if (current->GetToken() == TOKEN_PROCEDURE)
                State = ST_PROC00;
            else if (current->GetToken() == TOKEN_FUNCTION)
                State = ST_FUNC00;
            else if (current->GetToken() == TOKEN_ENDOFMODULE) // ben.. c'est pas grave
                State = ST_DEHORS;
            else
            {
                errListe->add("Inattendu (dehors): ",current);
                AvanceNextLigne(&current);
                State = ST_DEHORS;
            }
            break;

        // -----------------------------------------------------------------------------------------------
        //  PUBLIC Declaration
        // -----------------------------------------------------------------------------------------------
        case ST_P00:
            if (current->GetToken() == TOKEN_IDENTIF)
            {

                tmpVar= new VariableItem();
                VariablesPublic.add(tmpVar);
                tmpVar->SetCar(VR_PUBLIC);
                tmpVar->SetFunc(NULL);
                tmpVar->setTagName(current);
                tmpVar->SetTagType(NULL);
                State = ST_P01;
            }
            else
            {
                errListe->add("On attend un identifiant",current);
                AvanceNextLigne(&current);
                State = ST_DEHORS;
            }
            break;
        case ST_P01:
            if (current->GetToken() == TOKEN_AS)
                State = ST_P06;
            else if (current->GetToken() == TOKEN_OPENCRO)
            {
                State = ST_P02;
            }
            else
            {
                errListe->add("On attend 'as' ou '['",current);
                AvanceNextLigne(&current);
                State = ST_DEHORS;
            }
            break;
        case ST_P02:
            currentEnd = current;
            mCtr = 0;
            cCtr = 0;
            while (!(((currentEnd->GetToken() == TOKEN_CLOSECRO || currentEnd->GetToken() == TOKEN_VIRGULE) && mCtr==0 && cCtr==0)  || currentEnd->GetNext() == NULL))
            {
                if (currentEnd->GetToken() == TOKEN_OPENPAR)
                    mCtr++;
                else if (currentEnd->GetToken() == TOKEN_CLOSEPAR)
                    mCtr--;
                else if (currentEnd->GetToken() == TOKEN_OPENCRO)
                    cCtr++;
                else if (currentEnd->GetToken() == TOKEN_CLOSECRO)
                    cCtr--;
                currentEnd = currentEnd->GetNext();
            }
            if (currentEnd->GetToken()==TOKEN_CLOSECRO)
            {
                tmpDim = new DimElemItem();
                tmpDim->SetExprNull();
                VerifyExpression(true,current,currentEnd,false,tmpDim->GetExprPtr());
                tmpVar->GetDimListe()->add(tmpDim);
                State= ST_P05;
                current=currentEnd;
            }
            else if (currentEnd->GetToken()==TOKEN_VIRGULE)
            {
                tmpDim = new DimElemItem();
                tmpDim->SetExprNull();
                VerifyExpression(true,current,currentEnd,false,tmpDim->GetExprPtr());
                tmpVar->GetDimListe()->add(tmpDim);
                State= ST_P02;
                current=currentEnd;
            }
            else
            {
                errListe->add("On attend une expression de dimension",current);
                AvanceNextLigne(&current);
                State = ST_DEHORS;
            }
            break;
        case ST_P05:
            if (current->GetToken() == TOKEN_AS)
                State = ST_P06;
            else
            {
                errListe->add("On attend 'as' ",current);
                AvanceNextLigne(&current);
                State = ST_DEHORS;
            }
            break;
        case ST_P06:
            if (current->GetToken() == TOKEN_IDENTIF || IsAType(current))
            {
                tmpVar->setType(GetVarType(current));
                tmpVar->SetTagType(current);
                State = ST_P07;
            }
            else
            {
                errListe->add("On attend un type ",current);
                AvanceNextLigne(&current);
                State = ST_DEHORS;
            }
            break;
        case ST_P07:
            if (current->GetToken() == TOKEN_CRLF)
            {
                // On a d?clar? une variable publique
                tmpVar->SetCar(VR_PUBLIC);

                AvanceNextLigne(&current);
                State = ST_DEHORS;
            }
            else
            {
                errListe->add("Inattendu ",current);
                AvanceNextLigne(&current);
                State = ST_DEHORS;
            }
            break;


        // -----------------------------------------------------------------------------------------------
        //  TYPE Declaration
        // -----------------------------------------------------------------------------------------------

        case ST_T00:
            if (current->GetToken() == TOKEN_IDENTIF)
            {
                tmpType= new TypeItem();
                Types.add(tmpType);
                tmpType->setTagName(current);
                State = ST_T01;
            }
            else
            {
                errListe->add("On attend un identifiant ",current);
                AvanceNextLigne(&current);
                State = ST_DEHORS;
            }
            break;
        case ST_T01:
            if (current->GetToken() == TOKEN_CRLF)
            {
                State = ST_T02;
            }
            else
            {
                errListe->add("Inattendu: ",current);
                AvanceNextLigne(&current);
                State = ST_DEHORS;
            }
            break;
        case ST_T02:
            if (current->GetToken() == TOKEN_IDENTIF)
            {
                tmpVar= new VariableItem();
                tmpType->getFieldList()->add(tmpVar);
                tmpVar->setTagName(current);
                State = ST_T03;
            }
            else if (current->GetToken() == TOKEN_END)
                State = ST_T10;
            else if (current->GetToken() == TOKEN_CRLF)
            {
                State = ST_T02;
            }
            else
            {
                errListe->add("Vous devez declarer les champs du type ",current);
                AvanceNextLigne(&current);
                State = ST_T02;
            }
            break;
        case ST_T03:
            if (current->GetToken() == TOKEN_OPENCRO)
                State = ST_T04;
            else if (current->GetToken() == TOKEN_AS)
                State = ST_T08;
            else
            {
                errListe->add("On attend 'as' ou '[' ",current);
                AvanceNextLigne(&current);

                State = ST_T02;
            }
            break;
        case ST_T04:
            currentEnd=current;
            mCtr=0;
            cCtr=0;
            while (!(((currentEnd->GetToken() == TOKEN_CLOSECRO || currentEnd->GetToken() == TOKEN_VIRGULE) && mCtr==0 && cCtr==0)  || currentEnd == NULL))
            {
                if (currentEnd->GetToken() == TOKEN_OPENPAR)
                    mCtr++;
                else if (currentEnd->GetToken() == TOKEN_CLOSEPAR)
                    mCtr--;
                else if (currentEnd->GetToken() == TOKEN_OPENCRO)
                    cCtr++;
                else if (currentEnd->GetToken() == TOKEN_CLOSECRO)
                    cCtr--;
                currentEnd = currentEnd->GetNext();
            }

            if (currentEnd->GetToken()==TOKEN_VIRGULE)
            {
                tmpDim=new DimElemItem();
                tmpDim->SetExprNull();
                VerifyExpression(true,current,currentEnd,false,tmpDim->GetExprPtr());
                tmpVar->GetDimListe()->add(tmpDim);
                State=ST_T04;
                current=currentEnd;
            }
            else if (currentEnd->GetToken()==TOKEN_CLOSECRO)
            {
                tmpDim=new DimElemItem();
                tmpDim->SetExprNull();
                VerifyExpression(true,current,currentEnd,false,tmpDim->GetExprPtr());
                tmpVar->GetDimListe()->add(tmpDim);
                State=ST_T07;
                current=currentEnd;
            }
            else
            {
                errListe->add("On attend une expression pour la dimension",current);
                AvanceNextLigne(&current);
                State = ST_T02;
            }
            break;
        case ST_T07:
            if (current->GetToken() == TOKEN_AS)
                State=ST_T08;
            else
            {
                errListe->add("On attend 'as' ",current);
                AvanceNextLigne(&current);
                State = ST_T02;
            }
            break;
        case ST_T08:
            if (current->GetToken() == TOKEN_IDENTIF || IsAType(current))
            {
                tmpVar->setType(GetVarType(current));
                tmpVar->SetTagType(current);
                State = ST_T09;
            }
            else
            {
                errListe->add("On attend un type ",current);
                AvanceNextLigne(&current);
                State = ST_T02;
            }
            break;
        case ST_T09:
            if (current->GetToken() == TOKEN_CRLF)
            {
                // On a d?clar? un champ du type
                tmpVar->SetCar(VR_MEMBER);
                tmpVar->SetFunc(NULL);
                State = ST_T02;
            }
            else
            {
                errListe->add("Inattendu ",current);
                AvanceNextLigne(&current);
                State = ST_T02;
            }
            break;
        case ST_T10:
            if (current->GetToken() == TOKEN_TYPET)
                State = ST_T11;
            else
            {
                errListe->add("Inattendu ",current);
                AvanceNextLigne(&current);
                State = ST_T02;
            }
            break;
        case ST_T11:
            if (current->GetToken() == TOKEN_CRLF)
            {
                // On a d?clar? un type
                State = ST_DEHORS;
            }
            else
            {
                errListe->add("Inattendu ",current);
                AvanceNextLigne(&current);
                State = ST_T02;
            }
            break;


        // -----------------------------------------------------------------------------------------------
        //  PROCEDURE
        // -----------------------------------------------------------------------------------------------

        case ST_PROC00:
            tmpFonc= new FonctionItem();
            tmpFonc->setLine(current->getLine());
            tmpFonc->setReturnType(TP_VOID);
            if (readingPredef){
                tmpFonc->setIsAssembler();
            }

            if (current->GetToken() == TOKEN_IDENTIF)
            {
                tmpFonc->setName(current->GetIdentif());
                State = ST_PROC01;
                Fonctions.add(tmpFonc);     // on l'insere tout de suite
            }
            else if (current->GetToken() == TOKEN_MAIN){
                tmpFonc->setName("_main");
                tmpFonc->setUsed();
                State = ST_PROC01;
                Fonctions.AddDebut(tmpFonc);        // on l'insere tout de suite
            }
            else
            {
                errListe->add("On attend un identifiant",current);
                AvanceNextLigne(&current);
                State = ST_INTRA;
            }
            //tmpFoncPtr = (Fonction*)foncCollect->add(tmpFonc);    //tmpFoncPtr pointe sur l'?l?ment qui vient d'etre inser?
            //pInFunc = tmpFoncPtr;
            break;
        case ST_PROC01:
            if (current->GetToken() == TOKEN_DEUXPOINTS)
                State=ST_PROC02;
            else if (current->GetToken() == TOKEN_OPENPAR)
            {
                State=ST_PROC03;
            }
            else if (current->GetToken() == TOKEN_CRLF)
            {
                // On a d?clar? une nouvelle procedure
                State = ST_INTRA;
            }
            else
            {
                errListe->add("On attend ':' ou '('",current);
                AvanceNextLigne(&current);
                State = ST_INTRA;
            }
            break;
        case ST_PROC02:
            if (current->GetToken() == TOKEN_CRLF)
            {
                // On a d?clar? une nouvelle procedure
                State = ST_INTRA;
            }
            else if (current->GetToken() == TOKEN_OPENPAR)
            {
                State=ST_PROC03;
            }
            else
            {
                errListe->add("Inattendu. On attend '(' ",current);
                AvanceNextLigne(&current);
                State = ST_INTRA;
            }
            break;
        case ST_PROC03:
            if (current->GetToken() == TOKEN_IDENTIF)
            {
                tmpVar=new VariableItem();
                tmpFonc->getArgumentList()->AddDebut(tmpVar);
                tmpVar->setTagName(current);
                State = ST_PROC04;
            }
            else if (current->GetToken() == TOKEN_CLOSEPAR)
            {
                State = ST_PROC08;
            }
            else
            {
                errListe->add("On attend une déclaration d'argument ou ')' ",current);
                AvanceNextLigne(&current);
                State = ST_INTRA;
            }
            break;
        case ST_PROC04:
            if (current->GetToken() == TOKEN_AS)
                State = ST_PROC05;
            else
            {
                errListe->add("On attend 'as' ",current);
                AvanceNextLigne(&current);
                State = ST_INTRA;
            }
            break;
        case ST_PROC05:
            if (current->GetToken() == TOKEN_IDENTIF || IsAType(current))
            {
                tmpVar->setType(GetVarType(current));
                tmpVar->SetTagType(current);
                tmpVar->SetCar(VR_ARGU);
                tmpVar->SetFunc(tmpFonc);
                // On a d?clar? un argument

                State = ST_PROC06;
            }
            else
            {
                errListe->add("On attend un type ",current);
                AvanceNextLigne(&current);
                State = ST_INTRA;
            }
            break;
        case ST_PROC06:
            if (current->GetToken() == TOKEN_VIRGULE)
                State = ST_PROC07;
            else if (current->GetToken() == TOKEN_CLOSEPAR)
                State = ST_PROC08;
            else
            {
                errListe->add("On attend ',' ou ')' ",current);
                AvanceNextLigne(&current);
                State = ST_INTRA;
            }
            break;
        case ST_PROC07:
            if (current->GetToken() == TOKEN_IDENTIF)
            {
                tmpVar=new VariableItem();
                tmpFonc->getArgumentList()->AddDebut(tmpVar);
                tmpVar->setTagName(current);
                State = ST_PROC04;
            }
            else
            {
                errListe->add("On attend une déclaration d'argument ",current);
                AvanceNextLigne(&current);
                State = ST_INTRA;
            }
            break;
        case ST_PROC08:
            if (current->GetToken() == TOKEN_CRLF)
            {
                // On a d?clar? une nouvelle procedure
                // la déclaration des arguments s'est pass? avec succ?s
                State = ST_INTRA;
            }
            else
            {
                errListe->add("Inattendu ",current);
                AvanceNextLigne(&current);
                State = ST_INTRA;
            }
            break;


        case ST_FUNC00:
            tmpFonc= new FonctionItem();
            tmpFonc->setLine(current->getLine());
            tmpFonc->setReturnType(TP_VOID);   // ... pour l'instant
            Fonctions.add(tmpFonc);     // on l'insere tout de suite
            if (readingPredef){
                tmpFonc->setIsAssembler();
            }
            if (current->GetToken() == TOKEN_IDENTIF)
            {
                tmpFonc->setName(current->GetIdentif());
                State = ST_FUNC01;
            }
            else
            {
                errListe->add("On attend un identifiant",current);
                AvanceNextLigne(&current);
                State = ST_INTRA;
            }
            //tmpFoncPtr = (Fonction*)foncCollect->add(tmpFonc);    //tmpFoncPtr pointe sur l'?l?ment qui vient d'etre inser?
            //pInFunc = tmpFoncPtr;
            break;
        case ST_FUNC01:
            if (current->GetToken() == TOKEN_DEUXPOINTS)
                State=ST_FUNC02;
            else if (current->GetToken() == TOKEN_OPENPAR)
                State=ST_FUNC03;
            else if (current->GetToken() == TOKEN_AS)
                State = ST_FUNC09;
            else
            {
                errListe->add("On attend ':' ou '(' ou 'as' ",current);
                AvanceNextLigne(&current);
                State = ST_INTRA;
            }
            break;
        case ST_FUNC02:
            if (current->GetToken() == TOKEN_AS)
                State = ST_FUNC09;
            else if (current->GetToken() == TOKEN_OPENPAR)
                State=ST_FUNC03;
            else
            {
                errListe->add("On attend ':' ou '(' ou 'as' ",current);
                AvanceNextLigne(&current);
                State = ST_INTRA;
            }
            break;
        case ST_FUNC03:
            if (current->GetToken() == TOKEN_IDENTIF)
            {
                tmpVar=new VariableItem();
                tmpFonc->getArgumentList()->AddDebut(tmpVar);
                tmpVar->setTagName(current);
                State = ST_FUNC04;
            }
            else if (current->GetToken() == TOKEN_CLOSEPAR)
            {
                State = ST_FUNC08;
            }
            else
            {
                errListe->add("On attend une déclaration d'argument ou ')' ",current);
                AvanceNextLigne(&current);
                State = ST_INTRA;
            }
            break;
        case ST_FUNC04:
            if (current->GetToken() == TOKEN_AS)
                State = ST_FUNC05;
            else
            {
                errListe->add("On attend 'as' ",current);
                AvanceNextLigne(&current);
                State = ST_INTRA;
            }
            break;
        case ST_FUNC05:
            if (current->GetToken() == TOKEN_IDENTIF || IsAType(current))
            {
                tmpVar->setType(GetVarType(current));
                tmpVar->SetTagType(current);
                tmpVar->SetCar(VR_ARGU);
                tmpVar->SetFunc(tmpFonc);
                // On a d?clar? un argument

                State = ST_FUNC06;
            }
            else
            {
                errListe->add("On attend un type ",current);
                AvanceNextLigne(&current);
                State = ST_INTRA;
            }
            break;
        case ST_FUNC06:
            if (current->GetToken() == TOKEN_VIRGULE)
                State = ST_FUNC07;
            else if (current->GetToken() == TOKEN_CLOSEPAR)
                State = ST_FUNC08;
            else
            {
                errListe->add("On attend ',' ou ')' ",current);
                AvanceNextLigne(&current);
                State = ST_INTRA;
            }
            break;
        case ST_FUNC07:
            if (current->GetToken() == TOKEN_IDENTIF)
            {
                tmpVar=new VariableItem();
                tmpFonc->getArgumentList()->AddDebut(tmpVar);
                tmpVar->setTagName(current);
                State = ST_FUNC04;
            }
            else
            {
                errListe->add("On attend une déclaration d'argument ",current);
                AvanceNextLigne(&current);
                State = ST_INTRA;
            }
            break;
        case ST_FUNC08:
            if (current->GetToken() == TOKEN_AS)
                State = ST_FUNC09;
            else
            {
                errListe->add("On attend 'as' ",current);
                AvanceNextLigne(&current);
                State = ST_INTRA;
            }
            break;
        case ST_FUNC09:
            if (current->GetToken() == TOKEN_IDENTIF || IsAType(current))
            {
                tmpFonc->setReturnType(GetVarType(current));
                State = ST_FUNC10;
            }
            else
            {
                errListe->add("On attend un type ",current);
                AvanceNextLigne(&current);
                State = ST_INTRA;
            }
            break;
        case ST_FUNC10:
            if (current->GetToken() == TOKEN_CRLF)
            {
                // il faut d?clarer la fonction
                State = ST_INTRA;
            }
            else
            {
                errListe->add("Inattendu ",current);
                AvanceNextLigne(&current);
                State = ST_INTRA;
            }
            break;
        default:
            break;

        }
        if (current)
        {
            precCur = current;
            current=current->GetNext();
        }
    }
    return;

}



