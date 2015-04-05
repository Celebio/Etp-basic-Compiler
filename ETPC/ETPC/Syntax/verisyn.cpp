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

    filsD=CurNoeud->GetFilsD();
    filsG=CurNoeud->GetFilsG();

    if (filsD)
    {
        if (filsD->GetTAG()->GetToken() == TOKEN_OPENPAR)
        {
            CurNoeud->SetFilsD(filsD->GetFilsD());
            delete filsD;
        }
        NetoyerArbreParentheses(CurNoeud->GetFilsD());

    }
    if (filsG)
    {
        if (filsG->GetTAG()->GetToken() == TOKEN_OPENPAR)
        {
            CurNoeud->SetFilsG(filsG->GetFilsD());
            delete filsG;
        }
        NetoyerArbreParentheses(CurNoeud->GetFilsG());
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
    TAG* Cour=debTag;
    TAG* CourEnd;
    TAG* precCour=Cour;
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
        errListe->Add("On attend une expression",precCour);
        return false;
    }
    while (Cour && Cour!=(finTag))
    {
#ifdef _DEBUGARBRES
        Cour->Afficher();
#endif


        aux = new CNoeud(Cour);
        //crTokType=aux->GetOperType();


        if (aux->GetNature()==NOEUD_UNKNOWN)
        {
            errListe->Add("Mot reserve non permis dans une expression",Cour);
            return false;
        }
        if (!Arbre)
        {
            if (Cour->GetToken()==TOKEN_MOINS)      // si le premier terme est un token_moins, ce token_moins est un operateur unaire droite
            {
                aux->SetOperatorMoinsUnaire();
            }
            if (aux->GetOperType()==OPBIN || aux->GetOperType()==OPUNG)
            {
                // on a commenc? par un op?rateur binaire ou unaire_gauche-> faux
                errListe->Add("Inattendu ici ",Cour);
                return false;
            }
            else
            {
                Arbre = aux;
                if (Cour->GetToken()==TOKEN_OPENCRO)
                {
                    errListe->Add("Crochet non permis ici",Cour);
                    return false;
                }
                if (Cour->GetToken()==TOKEN_OPENPAR)
                {   // une expression entre parentheses
                    lastAdded=Arbre;
                    CourEnd = Cour;
                    mCtr = 0;
                    cCtr = 0;
                    while (!(( CourEnd->GetToken() == TOKEN_CLOSEPAR && mCtr==1 && cCtr==0)  || CourEnd == finTag))
                    {
                        if (CourEnd->GetToken() == TOKEN_OPENPAR)           mCtr++;
                        else if (CourEnd->GetToken() == TOKEN_CLOSEPAR)     mCtr--;
                        else if (CourEnd->GetToken() == TOKEN_OPENCRO)      cCtr++;
                        else if (CourEnd->GetToken() == TOKEN_CLOSECRO)     cCtr--;
                        CourEnd = CourEnd->GetNext();
                    }
                    if (CourEnd==finTag)
                    {
                        errListe->Add("Il manque un ')' ",Cour);
                        return false;
                    }
                    aux=lastAdded->GetFilsD();
                    VerifyExpression(false,Cour->GetNext(),CourEnd,true,&aux);
                    #ifdef _DEBUGARBRES
                        printf("dondum openpar basta\n");
                    #endif
                    Arbre->SetFilsD(aux);
                    Cour=CourEnd;
                }
            }
        }
        else
        {
            if (aux->GetOperType()==OPBIN || aux->GetOperType()==OPUNG)
            {
                courNoeud=Arbre;
                precNoeud=Arbre;
                // verifications pour ne pas avoir deux op?rateurs incorrectes ? la suite du genre +*
                // ? faire..

                // insertion du noeud de l'op?rateur ? la bonne place en fonction de sa priorit?
                while (courNoeud->GetOperator()> GiveOperatorType(Cour->GetToken()))
                {
                    precNoeud=courNoeud;
                    courNoeud=courNoeud->GetFilsD();
                }
                if (precNoeud==Arbre && courNoeud==Arbre)
                {
                    Arbre=aux;
                    aux->SetFilsG(courNoeud);
                }
                else
                {
                    aux->SetFilsG(courNoeud);
                    precNoeud->SetFilsD(aux);
                }
            }
            if (aux->GetOperType()==OPUND)
            {
                //if (!(GetOperatorType(lastAdded->GetTAG())==OPBIN || GetOperatorType(lastAdded->GetTAG())==OPUND))
                if (!(lastAdded->GetOperType()==OPBIN || lastAdded->GetOperType()==OPUND))
                {
                    errListe->Add("Operateur mal place ",Cour);
                    return false;
                }
                if (lastAdded->GetFilsD()==NULL)
                    lastAdded->SetFilsD(aux);
                else
                {   // impossible normalement
                    errListe->Add("Erreur interne ",Cour);
                    return false;
                }
            }
            else if (aux->GetOperType()==NOTOP)
            {
                //if (lastAdded)
                if (lastAdded->GetTAG()->GetToken()!=TOKEN_OPENPAR && lastAdded->GetOperType()!=OPBIN && lastAdded->GetOperType()!=OPUND && lastAdded->GetOperType()!=NOTOP)
                {
                    errListe->Add("Inattendu: Pas d'operateur pour cet operande ",Cour);
                    return false;
                }

                // une expression entre parentheses
                if (Cour->GetToken()==TOKEN_OPENPAR && lastAdded->GetOperType()!=NOTOP)
                {
                    if (lastAdded->GetFilsD()==NULL)    // ajout de l'operateur virtuel
                    {
                        lastAdded->SetFilsD(aux);
                        lastAdded=aux;
                    }
                    CourEnd = Cour;
                    mCtr = 0;
                    cCtr = 0;
                    while (!(( CourEnd->GetToken() == TOKEN_CLOSEPAR && mCtr==1 && cCtr==0)  || CourEnd == finTag))
                    {
                        if (CourEnd->GetToken() == TOKEN_OPENPAR)           mCtr++;
                        else if (CourEnd->GetToken() == TOKEN_CLOSEPAR)     mCtr--;
                        else if (CourEnd->GetToken() == TOKEN_OPENCRO)      cCtr++;
                        else if (CourEnd->GetToken() == TOKEN_CLOSECRO)     cCtr--;
                        CourEnd = CourEnd->GetNext();
                    }
                    if (CourEnd==finTag)
                    {
                        errListe->Add("Il manque un ')' ",Cour);
                        return false;
                    }
                    aux=lastAdded->GetFilsD();
                    VerifyExpression(false,Cour->GetNext(),CourEnd,true,&aux);
                    #ifdef _DEBUGARBRES
                        printf("dondum \n");
                    #endif
                    Cour=CourEnd;
                    if (lastAdded->GetFilsD()==NULL)
                        lastAdded->SetFilsD(aux);
                }
                // appel de fonction
                else if (Cour->GetToken()==TOKEN_OPENPAR && lastAdded->GetOperType()==NOTOP)
                {
                    #ifdef _DEBUGARBRES
                        printf("appel de fonction\n");
                    #endif
                    CourEnd = Cour;
                    mCtr = 0;
                    cCtr = 0;
                    argCtr=0;
                    while (!( (CourEnd->GetToken() == TOKEN_CLOSEPAR && mCtr==1 && cCtr==0)  || CourEnd == finTag ))
                    {
                        if (CourEnd->GetToken() == TOKEN_OPENPAR)           mCtr++;
                        else if (CourEnd->GetToken() == TOKEN_CLOSEPAR)     mCtr--;
                        else if (CourEnd->GetToken() == TOKEN_OPENCRO)      cCtr++;
                        else if (CourEnd->GetToken() == TOKEN_CLOSECRO)     cCtr--;
                        CourEnd = CourEnd->GetNext();

                        if ((CourEnd->GetToken() == TOKEN_VIRGULE && mCtr==1 && cCtr==0) || (CourEnd->GetToken() == TOKEN_CLOSEPAR && mCtr==1 && cCtr==0))
                        {
                            #ifdef _DEBUGARBRES
                                printf("verification de l'expression pour l'argument %i\n",argCtr+1);
                            #endif
                            VerifyExpression(false,Cour->GetNext(),CourEnd,true,lastAdded->GetSuccPtr(argCtr));
                            argCtr++;
                            Cour=CourEnd;
                        }
                    }
                    if (CourEnd==finTag)
                    {
                        errListe->Add("Il manque un ')' ",Cour);
                        return false;
                    }
                    lastAdded->SetFilsD(NULL);
                    lastAdded->SetFilsG(NULL);
                    //argCtr=0;
                    //while (lastAdded->Successeur[argCtr])
                    //{
                    //  lastAdded->Successeur[argCtr]->Afficher();
                    //  argCtr++;
                    //}

                    lastAdded->SetAsFonction();
                    delete aux;
                    Cour=CourEnd;
                }
                // appel d'un tableau
                else if (Cour->GetToken()==TOKEN_OPENCRO && lastAdded->GetOperType()==NOTOP)
                {
                    #ifdef _DEBUGARBRES
                        printf("appel d'un tableau\n");
                    #endif
                    CourEnd = Cour;
                    mCtr = 0;
                    cCtr = 0;
                    argCtr=0;
                    while (!( (CourEnd->GetToken() == TOKEN_CLOSECRO && mCtr==0 && cCtr==1)  || CourEnd == finTag ))
                    {
                        if (CourEnd->GetToken() == TOKEN_OPENPAR)           mCtr++;
                        else if (CourEnd->GetToken() == TOKEN_CLOSEPAR)     mCtr--;
                        else if (CourEnd->GetToken() == TOKEN_OPENCRO)      cCtr++;
                        else if (CourEnd->GetToken() == TOKEN_CLOSECRO)     cCtr--;
                        CourEnd = CourEnd->GetNext();

                        if ((CourEnd->GetToken() == TOKEN_VIRGULE && mCtr==0 && cCtr==1) || (CourEnd->GetToken() == TOKEN_CLOSECRO && mCtr==0 && cCtr==1))
                        {
                            #ifdef _DEBUGARBRES
                                printf("verification de l'expression pour l'index %i\n",argCtr+1);
                            #endif
                            VerifyExpression(true,Cour->GetNext(),CourEnd,true,lastAdded->GetSuccPtr(argCtr));
                            argCtr++;
                            Cour=CourEnd;
                        }
                    }
                    if (CourEnd==finTag)
                    {
                        errListe->Add("Il manque un ']' ",Cour);
                        return false;
                    }
                    lastAdded->SetFilsD(NULL);
                    lastAdded->SetFilsG(NULL);
                    lastAdded->SetAsArray();
                    delete aux;
                    Cour=CourEnd;
                }
                else if (Cour->GetToken()==TOKEN_OPENCRO && lastAdded->GetOperType()!=NOTOP)
                {
                    errListe->Add("Crochet non permis ici ",Cour);
                    return false;
                }
                else
                {
                    if (lastAdded->GetFilsD()==NULL)
                        lastAdded->SetFilsD(aux);
                    else
                    {
                        // impossible normalement
                        errListe->Add("Erreur interne1 ",Cour);
                        return false;
                    }
                }
            }

        }
        lastAdded = aux;

#ifdef _DEBUGARBRES
        if (Arbre)
            Arbre->Afficher();
        printf("\n\n");
#endif
        precCour = Cour;
        Cour = Cour->GetNext();
    }// fin while

    //if (GetOperatorType(precCour)==OPBIN || GetOperatorType(precCour)==OPUND)
    //{
    //  errListe->Add("Un manque un operande pour cet operateur",precCour);
    //  return false;
    //}
    NetoyerArbreParentheses(Arbre);     // il faut absolument mettre ca en dehors du while
#ifdef _DEBUGARBRES
    if (Arbre)
        Arbre->Afficher();
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

    TAG* Cour=debTag;
    TAG* CourEnd;
    TAG* precCour=Cour;
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

    while (Cour && Cour!=finTag)
    {
        switch (State)
        {
        case INST_00:
            debugCour = NULL;
            if (Cour->GetToken() == TOKEN_CRLF)
            {
                State = INST_00;
            }
            else if (Cour->GetToken() == TOKEN_DO)
            {
                debugCour = Cour;
                State = INST_DO01;
            }
            else if (Cour->GetToken() == TOKEN_IDENTIF)
                State = INST_IDENTIF;
            else if (Cour->GetToken() == TOKEN_LOCAL)
                State = INST_LOCAL00;
            else if (Cour->GetToken() == TOKEN_RETURN)
                State = INST_RET00;
            else if (Cour->GetToken() == TOKEN_FOR)
            {
                debugCour = Cour;
                State = INST_FOR00;
            }
            else if (Cour->GetToken() == TOKEN_IF)
            {
                debugCour = Cour;
                State = INST_IF00;
            }
            else if (Cour->GetToken() == TOKEN_PUBLIC)
            {
                errListe->Add("Declaration publique dans un corps ",Cour);
                AvanceNextLigne(&Cour);
                State = INST_00;
            }
            else
            {
                errListe->Add("Inattendu dans une fonction: ",Cour);
                AvanceNextLigne(&Cour);
                State = INST_00;
            }
            break;
        // -------------------------------------------------------------------------
        // GESTION DES DO... -------------------------------------------------------
        // -------------------------------------------------------------------------
        case INST_DO01:
            instrCour = new InstructionETPB(INS_STRUCT_DOLPWH);
            instrCollect->Add(instrCour);
            if (Cour->GetToken() == TOKEN_CRLF)
            {
                State = INST_DO02;
                rpCour = Cour;
            }
            else if (Cour->GetToken() == TOKEN_WHILE)
                State = INST_DO05;
            else
            {
                errListe->Add("Apres 'Do' on attend 'While' ou rien",Cour);
                AvanceNextLigne(&Cour);
                State = INST_00;
            }
            break;
        case INST_DO02:
            ifCtr = 0;
            ifFound = false;
            CourEnd = Cour;
            precCour = CourEnd;
            while (!(ifCtr==-1 || (CourEnd->GetToken()==TOKEN_LOOP && ifCtr==0) || CourEnd==finTag))
            {
                if (CourEnd->GetToken() == TOKEN_DO)
                    ifCtr++;
                else if (CourEnd->GetToken() == TOKEN_LOOP)
                {
                    ifCtr--;
                }
                precCour = CourEnd;
                CourEnd = CourEnd->GetNext();
            }
            if (CourEnd == finTag)
            {
                errListe->Add("Il manque 'Loop' (Do) ",debugCour);
                State = INST_00;
                Cour = rpCour;
                break;
            }
            else
            {
                VerifyInstruction(Cour,CourEnd,pFunc,instrCour->GetDOCorps());
                State = INST_DO03;
            }
            Cour = CourEnd;
            break;
        case INST_DO03:
            if (Cour->GetToken() == TOKEN_WHILE)
                State = INST_DO04;
            else
            {
                errListe->Add("On attend 'While'",Cour);
                AvanceNextLigne(&Cour);
                State = INST_00;
                break;
            }
            break;
        case INST_DO04:
            if (Cour->GetToken() == TOKEN_CRLF)
            {
                errListe->Add("On attend une expression de condition apres 'While'",Cour);
                AvanceNextLigne(&Cour);
                State = INST_00;
                break;
            }
            // il faut avoir expression suivi de CRLF
            CourEnd = Cour;
            while (!(CourEnd->GetToken() == TOKEN_CRLF || CourEnd == finTag))
                CourEnd = CourEnd->GetNext();

            // signaler les erreurs dans l'expression
            VerifyExpression(true,Cour,CourEnd,true,instrCour->GetDOExprConditionPtr());
            State = INST_00;
            Cour = CourEnd;
            rpCour = Cour;
            break;
        case INST_DO05:
            rpCour = Cour;
            instrCour->ChangeNatINS_STRUCT_DOWH();
            if (Cour->GetToken() == TOKEN_CRLF)
            {
                errListe->Add("On attend une expression de condition apres 'While'",Cour);
                AvanceNextLigne(&Cour);
                State = INST_DO06;
                break;
            }
            // il faut avoir expression suivi de CRLF
            CourEnd = Cour;
            while (!(CourEnd->GetToken() == TOKEN_CRLF || CourEnd == finTag))
                CourEnd = CourEnd->GetNext();

            // signaler les erreurs dans l'expression
            VerifyExpression(true,Cour,CourEnd,true,instrCour->GetDOExprConditionPtr());
            State = INST_DO06;
            Cour = CourEnd;
            break;
        case INST_DO06:
            ifCtr = 0;
            ifFound = false;
            CourEnd = Cour;
            precCour = CourEnd;
            while (!(ifCtr==-1 || (CourEnd->GetToken()==TOKEN_LOOP && ifCtr==0) || CourEnd==finTag))
            {
                if (CourEnd->GetToken() == TOKEN_DO)
                    ifCtr++;
                else if (CourEnd->GetToken() == TOKEN_LOOP)
                {
                    ifCtr--;
                }
                precCour = CourEnd;
                CourEnd = CourEnd->GetNext();
            }
            if (CourEnd == finTag)
            {
                errListe->Add("Il manque 'Loop' (Do) ",debugCour);
                State = INST_00;
                Cour = rpCour;
                break;
            }
            else
            {
                VerifyInstruction(Cour,CourEnd,pFunc,instrCour->GetDOCorps());
                State = INST_DO07;
            }
            Cour = CourEnd;
            break;
        case INST_DO07:
            if (!(Cour->GetToken() == TOKEN_CRLF))
            {
                errListe->Add("Inattendu apres 'Loop':",Cour);
                AvanceNextLigne(&Cour);
            }
            State = INST_00;
            break;


        // -------------------------------------------------------------------------
        // GESTION DES DECLARATIONS LOCALES ... ------------------------------------
        // -------------------------------------------------------------------------
        case INST_LOCAL00:
            /* comment? car pas besoin d'ajouter les déclarations en tant qu'instruction    */
            //instrCour = new InstructionETPB(INS_DECLARATION);
            //instrCollect->Add(instrCour);
            if (Cour->GetToken() == TOKEN_IDENTIF)
            {
                tmpVar= new VariableItem();
                pFunc->GetVarListe()->Add(tmpVar);
                tmpVar->SetCar(VR_PUBLIC);
                tmpVar->SetFunc(NULL);
                tmpVar->SetTagNom(Cour);
                tmpVar->SetTagType(NULL);
                State = INST_LOCAL01;
            }
            else
            {
                errListe->Add("On attend un identifiant:",Cour);
                AvanceNextLigne(&Cour);
                State = INST_00;
            }
            break;
        case INST_LOCAL01:
            if (Cour->GetToken() == TOKEN_AS)
                State = INST_LOCAL06;
            else if (Cour->GetToken() == TOKEN_OPENCRO)
                State = INST_LOCAL02;
            else
            {
                errListe->Add("On attend 'as' ou '['",Cour);
                AvanceNextLigne(&Cour);
                State = INST_00;
            }
            break;
        case INST_LOCAL02:
            CourEnd = Cour;
            mCtr = 0;
            cCtr = 0;
            while (!(((CourEnd->GetToken() == TOKEN_CLOSECRO || CourEnd->GetToken() == TOKEN_VIRGULE) && mCtr==0 && cCtr==0)  || CourEnd == finTag))
            {
                if (CourEnd->GetToken() == TOKEN_OPENPAR)
                    mCtr++;
                else if (CourEnd->GetToken() == TOKEN_CLOSEPAR)
                    mCtr--;
                else if (CourEnd->GetToken() == TOKEN_OPENCRO)
                    cCtr++;
                else if (CourEnd->GetToken() == TOKEN_CLOSECRO)
                    cCtr--;
                CourEnd = CourEnd->GetNext();
            }
            if (Cour == CourEnd)
            {
                errListe->Add("Vous devez sp?cifier la dimension",Cour);
                AvanceNextLigne(&Cour);
                State = INST_00;
                break;
            }
            if (CourEnd->GetToken()==TOKEN_CLOSECRO)
            {
                tmpDim = new DimElemItem();
                tmpDim->SetExprNull();
                VerifyExpression(true,Cour,CourEnd,false,tmpDim->GetExprPtr());
                tmpVar->GetDimListe()->Add(tmpDim);
                State= INST_LOCAL05;
                Cour=CourEnd;
            }
            else if (CourEnd->GetToken()==TOKEN_VIRGULE)
            {
                tmpDim = new DimElemItem();
                tmpDim->SetExprNull();
                VerifyExpression(true,Cour,CourEnd,false,tmpDim->GetExprPtr());
                tmpVar->GetDimListe()->Add(tmpDim);
                State= INST_LOCAL02;
                Cour=CourEnd;
            }
            else
            {
                errListe->Add("On attend une expression de dimension",Cour);
                AvanceNextLigne(&Cour);
                State = INST_00;
            }
            break;
        case INST_LOCAL05:
            if (Cour->GetToken() == TOKEN_AS)
                State = INST_LOCAL06;
            else
            {
                errListe->Add("On attend 'as' ",Cour);
                AvanceNextLigne(&Cour);
                State = INST_00;
                break;
            }
            break;
        case INST_LOCAL06:
            if (Cour->GetToken() == TOKEN_IDENTIF || IsAType(Cour))
            {
                tmpVar->SetType(GetVarType(Cour));
                tmpVar->SetTagType(Cour);
                State = INST_LOCAL07;
            }
            else
            {
                errListe->Add("On attend un type ",Cour);
                AvanceNextLigne(&Cour);
                State = INST_00;
            }
            break;
        case INST_LOCAL07:
            if (Cour->GetToken() == TOKEN_CRLF)
            {
                // On a d?clar? une variable locale
                tmpVar->SetCar(VR_LOCAL);
                AvanceNextLigne(&Cour);
                State = INST_00;
            }
            else
            {
                errListe->Add("Inattendu ",Cour);
                AvanceNextLigne(&Cour);
                State = INST_00;
            }
            break;
        // -------------------------------------------------------------------------
        // GESTION DE RETURN... ------------------------------------------------------
        // -------------------------------------------------------------------------        case INST_FOR00:
        case INST_RET00:
            instrCour = new InstructionETPB(INS_RETURN);
            instrCollect->Add(instrCour);
            CourEnd = Cour;
            // il faut avoir une expression puis CRLF
            while (!(CourEnd->GetToken() == TOKEN_CRLF || CourEnd == finTag))
                CourEnd = CourEnd->GetNext();
            // signaler les erreurs dans l'expression
            VerifyExpression(false,Cour,CourEnd,true,instrCour->GetReturnExprPtr());
            State = INST_00;
            Cour = CourEnd;
            break;
        // -------------------------------------------------------------------------
        // GESTION DES FOR... ------------------------------------------------------
        // -------------------------------------------------------------------------        case INST_FOR00:
        case INST_FOR00:
            instrCour = new InstructionETPB(INS_STRUCT_FOR);
            instrCollect->Add(instrCour);

            if (Cour->GetToken() == TOKEN_IDENTIF)
            {
                debExpression = Cour;   // c'est le premier GetToken() du ExprAssigned
                CourEnd = Cour;
                // il faut avoir =, sans aller ? la ligne

                while (!(CourEnd->GetToken() == TOKEN_EQUAL || CourEnd->GetToken() == TOKEN_CRLF || CourEnd == finTag))
                    CourEnd = CourEnd->GetNext();
                if (CourEnd->GetToken() != TOKEN_EQUAL)
                {
                    errListe->Add("On attend '=' ",Cour);
                    AvanceNextLigne(&Cour);
                    State = INST_FOR08;
                    break;
                }
                else
                {
                    // signaler les erreurs dans l'expression
                    VerifyExpression(true,Cour,CourEnd,true,instrCour->GetFORExprAssignedPtr());
                }
                Cour = CourEnd;
                rpCour = Cour;
                State = INST_FOR05;
                break;
            }
            else if (Cour->GetToken() == TOKEN_EQUAL)
            {
                errListe->Add("On attend une variable a affecter ",Cour);
                AvanceNextLigne(&Cour);
                State = INST_FOR08;
                rpCour = Cour;
            }
            else
            {
                errListe->Add("On attend une affectation ",Cour);
                AvanceNextLigne(&Cour);
                State = INST_FOR08;
                rpCour = Cour;
            }
            break;
        case INST_FOR05:
            // on a eu un "=" pour arriver ici

            // il faut avoir expr.TO, sans aller ? la ligne
            debExpression= Cour;
            CourEnd = Cour;
            while (!(CourEnd->GetToken() == TOKEN_TO || CourEnd->GetToken() == TOKEN_CRLF || CourEnd == finTag))
                CourEnd = CourEnd->GetNext();
            if (CourEnd->GetToken() != TOKEN_TO)
            {
                errListe->Add("On attend 'To' ",Cour);
                AvanceNextLigne(&Cour);
                State = INST_FOR08;
            }
            else
            {
                // construire l'arbre de l'expression entre "=" et "TO"
                VerifyExpression(true,debExpression,CourEnd,true,instrCour->GetFORExprArbreINITPtr());
                State = INST_FOR06;
            }
            Cour = CourEnd;
            rpCour = Cour;
            break;
        case INST_FOR06:
            CourEnd = Cour;
            debExpression= Cour;
            // il faut avoir Step, sans aller ? la ligne
            while (!(CourEnd->GetToken() == TOKEN_STEP || CourEnd->GetToken() == TOKEN_CRLF || CourEnd == finTag))
                CourEnd = CourEnd->GetNext();
            if (CourEnd == finTag)
            {
                errListe->Add("On attend une expression pour la fin",Cour);
                AvanceNextLigne(&Cour);
                State = INST_FOR08;
            }
            else
            {
                // construire l'arbre de l'expression entre TO et STEP ou entre TO et CRLF
                VerifyExpression(true,debExpression,CourEnd,true,instrCour->GetFORExprArbreTOPtr());

                if (CourEnd->GetToken() == TOKEN_STEP)
                    State = INST_FOR07;
                else
                    State = INST_FOR08;
            }
            Cour = CourEnd;
            rpCour = Cour;
            break;
        case INST_FOR07:
            CourEnd = Cour;
            debExpression= Cour;
            // il faut aller ? la ligne
            while (!(CourEnd->GetToken() == TOKEN_CRLF || CourEnd == finTag))
                CourEnd = CourEnd->GetNext();
            if (CourEnd == finTag)
            {
                errListe->Add("On attend une expression pour la fin",Cour);
                AvanceNextLigne(&Cour);
                State = INST_FOR08;
            }
            else
            {
                // construire l'arbre de l'expression entre STEP et CRLF
                VerifyExpression(true,debExpression,CourEnd,true,instrCour->GetFORExprArbreSTEPPtr());
                State = INST_FOR08;
            }
            Cour = CourEnd;
            rpCour = Cour;
            break;
        case INST_FOR08:
            ifCtr = 0;
            ifFound = false;
            CourEnd = Cour;
            precCour = CourEnd;
            while (!(ifCtr==-1 || (CourEnd->GetToken()==TOKEN_NEXT && ifCtr==0) || CourEnd==finTag))
            {
                if (CourEnd->GetToken() == TOKEN_FOR)
                    ifCtr++;
                else if (CourEnd->GetToken() == TOKEN_NEXT)
                {
                    ifCtr--;
                }
                precCour = CourEnd;
                CourEnd = CourEnd->GetNext();
            }
            if (CourEnd == finTag)
            {
                errListe->Add("Il manque 'Next' (For) ",debugCour);
                State = INST_00;
                Cour = rpCour;
                break;
            }
            else
            {
                VerifyInstruction(Cour,CourEnd,pFunc,instrCour->GetFORCorps());
                State = INST_FOR09;
            }
            Cour = CourEnd;
            break;
        case INST_FOR09:
            if (Cour->GetToken() == TOKEN_CRLF)
                State = INST_00;
            else
            {
                errListe->Add("Inattendu apres 'Next' ",Cour);
                AvanceNextLigne(&Cour);
                State = INST_00;
            }
            break;

        // -------------------------------------------------------------------------
        // GESTION DES IF... ------------------------------------------------------
        // -------------------------------------------------------------------------
        case INST_IF00:

            instrCour = new InstructionETPB(INS_IF);
            instrCollect->Add(instrCour);

            CourEnd = Cour;
            // il faut avoir Then, sans aller ? la ligne

            while (!(CourEnd->GetToken() == TOKEN_THEN || CourEnd->GetToken() == TOKEN_CRLF || CourEnd == finTag))
                CourEnd = CourEnd->GetNext();
            if (CourEnd->GetToken() != TOKEN_THEN)
            {
                errListe->Add("On attend 'Then' ",Cour);
                AvanceNextLigne(&Cour);
                State = INST_00;
            }
            else
            {
                // signaler les erreurs dans l'expression
                VerifyExpression(true,Cour,CourEnd,true,instrCour->GetIFExprArbrePtr());
                State = INST_IF01;
            }
            Cour = CourEnd;
            rpCour = Cour;
            break;
        case INST_IF01:
            if (Cour->GetToken() == TOKEN_CRLF)
            {
                State = INST_IF02;
            }
            else
            {
                errListe->Add("Inattendu apres 'Then' ",Cour);
                AvanceNextLigne(&Cour);
                State = INST_IF02;
            }
            break;

        case INST_IF02:
            ifCtr = 0;
            ifFound = false;
            CourEnd = Cour;
            precCour = CourEnd;
            while (!(ifCtr==-1 || (CourEnd->GetToken()==TOKEN_ELSEIF && ifCtr==0) || (CourEnd->GetToken()==TOKEN_ELSE && ifCtr==0) || CourEnd==finTag))
            {
                if (CourEnd->GetToken() == TOKEN_IF && precCour->GetToken() != TOKEN_END)
                    ifFound = true;
                else if (CourEnd->GetToken() == TOKEN_CRLF)
                {
                    if (ifFound)
                        ifCtr++;
                    ifFound = false;
                }
                else if (CourEnd->GetToken() == TOKEN_END)
                {
                    if (CourEnd->GetNext() != finTag)
                    {
                        if (CourEnd->GetNext()->GetToken() == TOKEN_IF)
                        {
                            ifCtr--;
                            ifFound = false;
                        }
                    }
                }
                precCour = CourEnd;
                CourEnd = CourEnd->GetNext();
            }
            if (CourEnd == finTag)
            {
                errListe->Add("Il manque 'End If'  ",debugCour);
                State = INST_00;
                Cour = rpCour;
            }
            else if (CourEnd->GetToken() == TOKEN_ELSEIF)
            {
                if (debugCour->GetToken()==TOKEN_IF)
                {
                    VerifyInstruction(Cour,CourEnd,pFunc,instrCour->GetIFIfCorps());
                }
                else if (debugCour->GetToken()==TOKEN_ELSEIF)
                {
                    Collection* CorpsT=new Collection();
                    VerifyInstruction(Cour,CourEnd,pFunc,CorpsT);
                    instrCour->GetIFElseIfCorps()->Add(CorpsT);
                }
                State = INST_IF03;
                debugCour = CourEnd;
                Cour = CourEnd;
            }
            else if (CourEnd->GetToken() == TOKEN_ELSE)
            {
                if (debugCour->GetToken()==TOKEN_IF)
                {
                    VerifyInstruction(Cour,CourEnd,pFunc,instrCour->GetIFIfCorps());
                }
                else if (debugCour->GetToken()==TOKEN_ELSEIF)
                {
                    Collection* CorpsT=new Collection();
                    VerifyInstruction(Cour,CourEnd,pFunc,CorpsT);
                    instrCour->GetIFElseIfCorps()->Add(CorpsT);
                }
                State = INST_IF04;
                debugCour = CourEnd;
                Cour = CourEnd;
            }
            else    // on a rencontr? End If
            {
                if (debugCour->GetToken()==TOKEN_IF)
                {
                    VerifyInstruction(Cour,precCour,pFunc,instrCour->GetIFIfCorps());
                }
                else if (debugCour->GetToken()==TOKEN_ELSEIF)
                {
                    Collection* CorpsT=new Collection();
                    VerifyInstruction(Cour,precCour,pFunc,CorpsT);
                    instrCour->GetIFElseIfCorps()->Add(CorpsT);
                }
                State = INST_IF05;
                Cour = CourEnd;
            }
            break;
        case INST_IF03:
            CourEnd = Cour;
            // il faut avoir Then, sans aller ? la ligne

            while (!(CourEnd->GetToken() == TOKEN_THEN || CourEnd->GetToken() == TOKEN_CRLF || CourEnd == finTag))
                CourEnd = CourEnd->GetNext();
            if (CourEnd->GetToken() != TOKEN_THEN)
            {
                errListe->Add("On attend 'Then' ",Cour);
                Cour = rpCour;
                State = INST_00;
                break;
            }
            else
            {
                // signaler les erreurs dans l'expression

                //VerifyExpression(true,Cour,CourEnd,true,instrCour->GetIFExprArbrePtr());
                CNoeud* exprElseIf=NULL;
                VerifyExpression(true,Cour,CourEnd,true,&exprElseIf);
                instrCour->GetIFExprElseIf()->Add(exprElseIf);
                State = INST_IF01;
            }
            Cour = CourEnd;
            rpCour = Cour;
            break;
        case INST_IF04:
            ifCtr = 0;
            ifFound = false;
            CourEnd = Cour;
            precCour = CourEnd;
            while (!(ifCtr==-1 || (CourEnd->GetToken()==TOKEN_ELSEIF && ifCtr==0) || (CourEnd->GetToken()==TOKEN_ELSE && ifCtr==0) || CourEnd==finTag))
            {
                if (CourEnd->GetToken() == TOKEN_IF && precCour->GetToken() != TOKEN_END)
                    ifFound = true;
                else if (CourEnd->GetToken() == TOKEN_CRLF)
                {
                    if (ifFound)
                        ifCtr++;
                    ifFound = false;
                }
                else if (CourEnd->GetToken() == TOKEN_END)
                {
                    if (CourEnd->GetNext() != finTag)
                    {
                        if (CourEnd->GetNext()->GetToken() == TOKEN_IF)
                        {
                            ifCtr--;
                            ifFound = false;
                        }
                    }
                }
                precCour = CourEnd;
                CourEnd = CourEnd->GetNext();
            }
            if (CourEnd == finTag)
            {
                errListe->Add("Il manque 'End If'  ",debugCour);
                State = INST_00;
                Cour = rpCour;
            }
            else if (CourEnd->GetToken() == TOKEN_ELSEIF)
            {
                errListe->Add("Impossible d'avoir 'ElseIf' suite ? 'Else' ",debugCour);
                State = INST_00;
                Cour = rpCour;
            }
            else if (CourEnd->GetToken() == TOKEN_ELSE)
            {
                errListe->Add("Impossible d'avoir 'Else' suite ? 'Else' ",debugCour);
                State = INST_00;
                Cour = rpCour;
            }
            else    // on a rencontr? End If
            {
                VerifyInstruction(Cour,precCour,pFunc,instrCour->GetIFElseCorps());
                State = INST_IF05;
                Cour = CourEnd;
            }
            break;
        case INST_IF05:
            if (Cour->GetToken() == TOKEN_CRLF)
            {
                State = INST_00;
            }
            else
            {
                errListe->Add("Inattendu apres 'End If': ",Cour);
                AvanceNextLigne(&Cour);
                State = INST_00;
            }
            break;
        // -------------------------------------------------------------------------
        // GESTION DES DECLARATIONS AUTRES ... ------------------------------------
        // -------------------------------------------------------------------------
        case INST_IDENTIF:

            if (Cour->GetToken() == TOKEN_CRLF)
            {   // appel de proc?dure sans argument

                instrCour = new InstructionETPB(INS_CALL);
                instrCollect->Add(instrCour);
                VerifyExpression(true,precCour,Cour,true,instrCour->GetCallExprPtr());
                State = INST_00;
                break;
            }
            if (Cour->GetToken() == TOKEN_OPENPAR){
                State = INST_IDOPENPAR;
                Cour=precCour;
                break;
            }
            CourEnd=Cour;
            while (CourEnd->GetToken()!=TOKEN_EQUAL && CourEnd !=finTag && CourEnd->GetToken()!=TOKEN_CRLF)
            {
                CourEnd=CourEnd->GetNext();
            }
            if (CourEnd==finTag || CourEnd->GetToken()==TOKEN_CRLF) // on n'a pas trouv? de TOKEN_EQUAL
            {
                // lecture des arguments sans parenthese
                // on va lui faire croire qu'il y a des parentheses
                // sauf s'il y a deja

                aux=new TAG;
                *aux=*Cour;
                aux->SetToken(TOKEN_OPENPAR);
                aux->SetNext(Cour);
                precCour->SetNext(aux);
                CourEnd=aux;
                while (CourEnd->GetToken() != TOKEN_CRLF)
                {
                    Cour=CourEnd;
                    CourEnd=CourEnd->GetNext();
                }
                aux=new TAG;
                *aux=*Cour;
                aux->SetToken(TOKEN_CLOSEPAR);
                Cour->SetNext(aux);
                aux->SetNext(CourEnd);
                Cour=precCour;
                State = INST_IDOPENPAR;
            }
            else        // on a trouv? un TOKEN_EQUAL
            {
                instrCour = new InstructionETPB(INS_AFFECTATION);
                instrCollect->Add(instrCour);

                VerifyExpression(true,precCour,CourEnd,true,instrCour->GetAffectExprAssignedPtr());
                Cour=CourEnd->GetNext();
                CourEnd=Cour;
                while (CourEnd !=finTag && CourEnd->GetToken()!=TOKEN_CRLF)
                {
                    CourEnd=CourEnd->GetNext();
                }
                VerifyExpression(true,Cour,CourEnd,true,instrCour->GetAffectExprArbrePtr());
                Cour=CourEnd;
                State = INST_00;
            }
            break;
        case INST_IDOPENPAR:
            if (Cour->GetToken() == TOKEN_CLOSEPAR && precCour->GetToken() == TOKEN_OPENPAR)
            {
                State = INST_IDCLOSEPAR;
                break;
            }
            CourEnd = Cour;
            mCtr = 0;
            cCtr = 0;
            while (!((CourEnd->GetToken() == TOKEN_CLOSEPAR && mCtr==1 && cCtr==0) || (CourEnd->GetToken() == TOKEN_VIRGULE && mCtr==0 && cCtr==0)  || CourEnd->GetToken() == TOKEN_CRLF))
            {
                if (CourEnd->GetToken() == TOKEN_OPENPAR)
                    mCtr++;
                else if (CourEnd->GetToken() == TOKEN_CLOSEPAR)
                    mCtr--;
                else if (CourEnd->GetToken() == TOKEN_OPENCRO)
                    cCtr++;
                else if (CourEnd->GetToken() == TOKEN_CLOSECRO)
                    cCtr--;
                CourEnd = CourEnd->GetNext();
            }
            if (CourEnd->GetToken() == TOKEN_CRLF)
            {
                errListe->Add("Il manque un ')' ",Cour);
                AvanceNextLigne(&Cour);
                State = INST_00;
                break;
            }
            if (Cour == CourEnd)
            {
                errListe->Add("Vous devez sp?cifier l'argument apres une , ",Cour);
                AvanceNextLigne(&Cour);
                State = INST_00;
                break;
            }
            // il faut v?rifier l'expression entre les parentheses
            instrCour = new InstructionETPB(INS_CALL);
            instrCollect->Add(instrCour);
            if (!(VerifyExpression(true,precCour,CourEnd->GetNext(),true,instrCour->GetCallExprPtr())))
            {
                AvanceNextLigne(&Cour);
                State = INST_00;
                break;
            }

            Cour = CourEnd;
            if (Cour->GetToken() == TOKEN_VIRGULE)
                State = INST_IDOPENPAR;
            else
                State = INST_IDCLOSEPAR;
            break;
        case INST_IDCLOSEPAR:
            if (Cour->GetToken() == TOKEN_CRLF)
            {
                State = INST_00;
            }
            else
            {
                errListe->Add("Inattendu: ",Cour);
                AvanceNextLigne(&Cour);
                State = INST_00;
            }
            break;
        default:
            break;
        }   // fin switch
        precCour=Cour;
        Cour=Cour->GetNext();
    }

}

void VeriSyn::VerifSyntax()
{
    // etat automate
    enumETAT State = ST_DEHORS;

    // parcours
    TAG* Cour=tagListe;
    TAG* CourEnd;
    TAG* precCour;
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

    while (Cour)
    {

        readingPredef= Cour->GetisPredefFileTAG();      // est-ce que c'est un fichier de predef?
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
            CourEnd = Cour;
            precCour = Cour;
            Found = false;
            while (CourEnd && !(CourEnd->GetToken()==TOKEN_PROCEDURE || CourEnd->GetToken()==TOKEN_FUNCTION || CourEnd->GetToken()==TOKEN_ENDOFMODULE))
            {
                precCour = CourEnd;
                CourEnd = CourEnd->GetNext();
            }
            //tmpInstrCollect.Clear();
            /*
            tmpInstrCollect.SetBuffer(NULL);
            VerifyInstruction(Cour,CourEnd,errListe,pInFunc,&tmpInstrCollect);
            pInFunc->InstrListe=tmpInstrCollect.GetBuffer();
            */
            VerifyInstruction(Cour,CourEnd,tmpFonc,tmpFonc->GetInstrListe());
            Cour = precCour;
            State = ST_DEHORS;
            break;
        case ST_DEHORS:
            if (Cour->GetToken() == TOKEN_PUBLIC)
                State = ST_P00;
            else if (Cour->GetToken() == TOKEN_TYPET)
            {
                State = ST_T00;
                // il faut v?rifier l'existance de End Type
                CourEnd = Cour;
                Found = false;
                while (CourEnd && !Found)
                {
                    if (CourEnd->GetNext())
                    {
                        if (CourEnd->GetNext()->GetNext())
                            if (CourEnd->GetToken()==TOKEN_END && CourEnd->GetNext()->GetToken()==TOKEN_TYPET)
                                Found = true;
                    }
                    CourEnd = CourEnd->GetNext();
                }
                if (!Found)
                {
                    errListe->Add("'Type' sans 'End Type'",Cour);
                    AvanceNextLigne(&Cour);
                    State = ST_DEHORS;
                }
            }
            else if (Cour->GetToken() == TOKEN_CRLF)
            {
                State = ST_DEHORS;
            }
            else if (Cour->GetToken() == TOKEN_PROCEDURE)
                State = ST_PROC00;
            else if (Cour->GetToken() == TOKEN_FUNCTION)
                State = ST_FUNC00;
            else if (Cour->GetToken() == TOKEN_ENDOFMODULE) // ben.. c'est pas grave
                State = ST_DEHORS;
            else
            {
                errListe->Add("Inattendu (dehors): ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_DEHORS;
            }
            break;

        // -----------------------------------------------------------------------------------------------
        //  PUBLIC Declaration
        // -----------------------------------------------------------------------------------------------
        case ST_P00:
            if (Cour->GetToken() == TOKEN_IDENTIF)
            {

                tmpVar= new VariableItem();
                VariablesPublic.Add(tmpVar);
                tmpVar->SetCar(VR_PUBLIC);
                tmpVar->SetFunc(NULL);
                tmpVar->SetTagNom(Cour);
                tmpVar->SetTagType(NULL);
                State = ST_P01;
            }
            else
            {
                errListe->Add("On attend un identifiant",Cour);
                AvanceNextLigne(&Cour);
                State = ST_DEHORS;
            }
            break;
        case ST_P01:
            if (Cour->GetToken() == TOKEN_AS)
                State = ST_P06;
            else if (Cour->GetToken() == TOKEN_OPENCRO)
            {
                State = ST_P02;
            }
            else
            {
                errListe->Add("On attend 'as' ou '['",Cour);
                AvanceNextLigne(&Cour);
                State = ST_DEHORS;
            }
            break;
        case ST_P02:
            CourEnd = Cour;
            mCtr = 0;
            cCtr = 0;
            while (!(((CourEnd->GetToken() == TOKEN_CLOSECRO || CourEnd->GetToken() == TOKEN_VIRGULE) && mCtr==0 && cCtr==0)  || CourEnd->GetNext() == NULL))
            {
                if (CourEnd->GetToken() == TOKEN_OPENPAR)
                    mCtr++;
                else if (CourEnd->GetToken() == TOKEN_CLOSEPAR)
                    mCtr--;
                else if (CourEnd->GetToken() == TOKEN_OPENCRO)
                    cCtr++;
                else if (CourEnd->GetToken() == TOKEN_CLOSECRO)
                    cCtr--;
                CourEnd = CourEnd->GetNext();
            }
            if (CourEnd->GetToken()==TOKEN_CLOSECRO)
            {
                tmpDim = new DimElemItem();
                tmpDim->SetExprNull();
                VerifyExpression(true,Cour,CourEnd,false,tmpDim->GetExprPtr());
                tmpVar->GetDimListe()->Add(tmpDim);
                State= ST_P05;
                Cour=CourEnd;
            }
            else if (CourEnd->GetToken()==TOKEN_VIRGULE)
            {
                tmpDim = new DimElemItem();
                tmpDim->SetExprNull();
                VerifyExpression(true,Cour,CourEnd,false,tmpDim->GetExprPtr());
                tmpVar->GetDimListe()->Add(tmpDim);
                State= ST_P02;
                Cour=CourEnd;
            }
            else
            {
                errListe->Add("On attend une expression de dimension",Cour);
                AvanceNextLigne(&Cour);
                State = ST_DEHORS;
            }
            break;
        case ST_P05:
            if (Cour->GetToken() == TOKEN_AS)
                State = ST_P06;
            else
            {
                errListe->Add("On attend 'as' ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_DEHORS;
            }
            break;
        case ST_P06:
            if (Cour->GetToken() == TOKEN_IDENTIF || IsAType(Cour))
            {
                tmpVar->SetType(GetVarType(Cour));
                tmpVar->SetTagType(Cour);
                State = ST_P07;
            }
            else
            {
                errListe->Add("On attend un type ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_DEHORS;
            }
            break;
        case ST_P07:
            if (Cour->GetToken() == TOKEN_CRLF)
            {
                // On a d?clar? une variable publique
                tmpVar->SetCar(VR_PUBLIC);

                AvanceNextLigne(&Cour);
                State = ST_DEHORS;
            }
            else
            {
                errListe->Add("Inattendu ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_DEHORS;
            }
            break;


        // -----------------------------------------------------------------------------------------------
        //  TYPE Declaration
        // -----------------------------------------------------------------------------------------------

        case ST_T00:
            if (Cour->GetToken() == TOKEN_IDENTIF)
            {
                tmpType= new TypeItem();
                Types.Add(tmpType);
                tmpType->SetTagNom(Cour);
                State = ST_T01;
            }
            else
            {
                errListe->Add("On attend un identifiant ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_DEHORS;
            }
            break;
        case ST_T01:
            if (Cour->GetToken() == TOKEN_CRLF)
            {
                State = ST_T02;
            }
            else
            {
                errListe->Add("Inattendu: ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_DEHORS;
            }
            break;
        case ST_T02:
            if (Cour->GetToken() == TOKEN_IDENTIF)
            {
                tmpVar= new VariableItem();
                tmpType->GetChampListe()->Add(tmpVar);
                tmpVar->SetTagNom(Cour);
                State = ST_T03;
            }
            else if (Cour->GetToken() == TOKEN_END)
                State = ST_T10;
            else if (Cour->GetToken() == TOKEN_CRLF)
            {
                State = ST_T02;
            }
            else
            {
                errListe->Add("Vous devez declarer les champs du type ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_T02;
            }
            break;
        case ST_T03:
            if (Cour->GetToken() == TOKEN_OPENCRO)
                State = ST_T04;
            else if (Cour->GetToken() == TOKEN_AS)
                State = ST_T08;
            else
            {
                errListe->Add("On attend 'as' ou '[' ",Cour);
                AvanceNextLigne(&Cour);

                State = ST_T02;
            }
            break;
        case ST_T04:
            CourEnd=Cour;
            mCtr=0;
            cCtr=0;
            while (!(((CourEnd->GetToken() == TOKEN_CLOSECRO || CourEnd->GetToken() == TOKEN_VIRGULE) && mCtr==0 && cCtr==0)  || CourEnd == NULL))
            {
                if (CourEnd->GetToken() == TOKEN_OPENPAR)
                    mCtr++;
                else if (CourEnd->GetToken() == TOKEN_CLOSEPAR)
                    mCtr--;
                else if (CourEnd->GetToken() == TOKEN_OPENCRO)
                    cCtr++;
                else if (CourEnd->GetToken() == TOKEN_CLOSECRO)
                    cCtr--;
                CourEnd = CourEnd->GetNext();
            }

            if (CourEnd->GetToken()==TOKEN_VIRGULE)
            {
                tmpDim=new DimElemItem();
                tmpDim->SetExprNull();
                VerifyExpression(true,Cour,CourEnd,false,tmpDim->GetExprPtr());
                tmpVar->GetDimListe()->Add(tmpDim);
                State=ST_T04;
                Cour=CourEnd;
            }
            else if (CourEnd->GetToken()==TOKEN_CLOSECRO)
            {
                tmpDim=new DimElemItem();
                tmpDim->SetExprNull();
                VerifyExpression(true,Cour,CourEnd,false,tmpDim->GetExprPtr());
                tmpVar->GetDimListe()->Add(tmpDim);
                State=ST_T07;
                Cour=CourEnd;
            }
            else
            {
                errListe->Add("On attend une expression pour la dimension",Cour);
                AvanceNextLigne(&Cour);
                State = ST_T02;
            }
            break;
        case ST_T07:
            if (Cour->GetToken() == TOKEN_AS)
                State=ST_T08;
            else
            {
                errListe->Add("On attend 'as' ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_T02;
            }
            break;
        case ST_T08:
            if (Cour->GetToken() == TOKEN_IDENTIF || IsAType(Cour))
            {
                tmpVar->SetType(GetVarType(Cour));
                tmpVar->SetTagType(Cour);
                State = ST_T09;
            }
            else
            {
                errListe->Add("On attend un type ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_T02;
            }
            break;
        case ST_T09:
            if (Cour->GetToken() == TOKEN_CRLF)
            {
                // On a d?clar? un champ du type
                tmpVar->SetCar(VR_MEMBER);
                tmpVar->SetFunc(NULL);
                State = ST_T02;
            }
            else
            {
                errListe->Add("Inattendu ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_T02;
            }
            break;
        case ST_T10:
            if (Cour->GetToken() == TOKEN_TYPET)
                State = ST_T11;
            else
            {
                errListe->Add("Inattendu ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_T02;
            }
            break;
        case ST_T11:
            if (Cour->GetToken() == TOKEN_CRLF)
            {
                // On a d?clar? un type
                State = ST_DEHORS;
            }
            else
            {
                errListe->Add("Inattendu ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_T02;
            }
            break;


        // -----------------------------------------------------------------------------------------------
        //  PROCEDURE
        // -----------------------------------------------------------------------------------------------

        case ST_PROC00:
            tmpFonc= new FonctionItem();
            tmpFonc->SetLigne(Cour->GetLigne());
            tmpFonc->SetRetType(TP_VOID);
            if (readingPredef){
                tmpFonc->SetIsAssembler();
            }

            if (Cour->GetToken() == TOKEN_IDENTIF)
            {
                tmpFonc->SetNom(Cour->GetIdentif());
                State = ST_PROC01;
                Fonctions.Add(tmpFonc);     // on l'insere tout de suite
            }
            else if (Cour->GetToken() == TOKEN_MAIN){
                tmpFonc->SetNom("_main");
                tmpFonc->SetUsed();
                State = ST_PROC01;
                Fonctions.AddDebut(tmpFonc);        // on l'insere tout de suite
            }
            else
            {
                errListe->Add("On attend un identifiant",Cour);
                AvanceNextLigne(&Cour);
                State = ST_INTRA;
            }
            //tmpFoncPtr = (Fonction*)foncCollect->Add(tmpFonc);    //tmpFoncPtr pointe sur l'?l?ment qui vient d'etre inser?
            //pInFunc = tmpFoncPtr;
            break;
        case ST_PROC01:
            if (Cour->GetToken() == TOKEN_DEUXPOINTS)
                State=ST_PROC02;
            else if (Cour->GetToken() == TOKEN_OPENPAR)
            {
                State=ST_PROC03;
            }
            else if (Cour->GetToken() == TOKEN_CRLF)
            {
                // On a d?clar? une nouvelle procedure
                State = ST_INTRA;
            }
            else
            {
                errListe->Add("On attend ':' ou '('",Cour);
                AvanceNextLigne(&Cour);
                State = ST_INTRA;
            }
            break;
        case ST_PROC02:
            if (Cour->GetToken() == TOKEN_CRLF)
            {
                // On a d?clar? une nouvelle procedure
                State = ST_INTRA;
            }
            else if (Cour->GetToken() == TOKEN_OPENPAR)
            {
                State=ST_PROC03;
            }
            else
            {
                errListe->Add("Inattendu. On attend '(' ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_INTRA;
            }
            break;
        case ST_PROC03:
            if (Cour->GetToken() == TOKEN_IDENTIF)
            {
                tmpVar=new VariableItem();
                tmpFonc->GetArguListe()->AddDebut(tmpVar);
                tmpVar->SetTagNom(Cour);
                State = ST_PROC04;
            }
            else if (Cour->GetToken() == TOKEN_CLOSEPAR)
            {
                State = ST_PROC08;
            }
            else
            {
                errListe->Add("On attend une déclaration d'argument ou ')' ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_INTRA;
            }
            break;
        case ST_PROC04:
            if (Cour->GetToken() == TOKEN_AS)
                State = ST_PROC05;
            else
            {
                errListe->Add("On attend 'as' ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_INTRA;
            }
            break;
        case ST_PROC05:
            if (Cour->GetToken() == TOKEN_IDENTIF || IsAType(Cour))
            {
                tmpVar->SetType(GetVarType(Cour));
                tmpVar->SetTagType(Cour);
                tmpVar->SetCar(VR_ARGU);
                tmpVar->SetFunc(tmpFonc);
                // On a d?clar? un argument

                State = ST_PROC06;
            }
            else
            {
                errListe->Add("On attend un type ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_INTRA;
            }
            break;
        case ST_PROC06:
            if (Cour->GetToken() == TOKEN_VIRGULE)
                State = ST_PROC07;
            else if (Cour->GetToken() == TOKEN_CLOSEPAR)
                State = ST_PROC08;
            else
            {
                errListe->Add("On attend ',' ou ')' ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_INTRA;
            }
            break;
        case ST_PROC07:
            if (Cour->GetToken() == TOKEN_IDENTIF)
            {
                tmpVar=new VariableItem();
                tmpFonc->GetArguListe()->AddDebut(tmpVar);
                tmpVar->SetTagNom(Cour);
                State = ST_PROC04;
            }
            else
            {
                errListe->Add("On attend une déclaration d'argument ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_INTRA;
            }
            break;
        case ST_PROC08:
            if (Cour->GetToken() == TOKEN_CRLF)
            {
                // On a d?clar? une nouvelle procedure
                // la déclaration des arguments s'est pass? avec succ?s
                State = ST_INTRA;
            }
            else
            {
                errListe->Add("Inattendu ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_INTRA;
            }
            break;


        case ST_FUNC00:
            tmpFonc= new FonctionItem();
            tmpFonc->SetLigne(Cour->GetLigne());
            tmpFonc->SetRetType(TP_VOID);   // ... pour l'instant
            Fonctions.Add(tmpFonc);     // on l'insere tout de suite
            if (readingPredef){
                tmpFonc->SetIsAssembler();
            }
            if (Cour->GetToken() == TOKEN_IDENTIF)
            {
                tmpFonc->SetNom(Cour->GetIdentif());
                State = ST_FUNC01;
            }
            else
            {
                errListe->Add("On attend un identifiant",Cour);
                AvanceNextLigne(&Cour);
                State = ST_INTRA;
            }
            //tmpFoncPtr = (Fonction*)foncCollect->Add(tmpFonc);    //tmpFoncPtr pointe sur l'?l?ment qui vient d'etre inser?
            //pInFunc = tmpFoncPtr;
            break;
        case ST_FUNC01:
            if (Cour->GetToken() == TOKEN_DEUXPOINTS)
                State=ST_FUNC02;
            else if (Cour->GetToken() == TOKEN_OPENPAR)
                State=ST_FUNC03;
            else if (Cour->GetToken() == TOKEN_AS)
                State = ST_FUNC09;
            else
            {
                errListe->Add("On attend ':' ou '(' ou 'as' ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_INTRA;
            }
            break;
        case ST_FUNC02:
            if (Cour->GetToken() == TOKEN_AS)
                State = ST_FUNC09;
            else if (Cour->GetToken() == TOKEN_OPENPAR)
                State=ST_FUNC03;
            else
            {
                errListe->Add("On attend ':' ou '(' ou 'as' ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_INTRA;
            }
            break;
        case ST_FUNC03:
            if (Cour->GetToken() == TOKEN_IDENTIF)
            {
                tmpVar=new VariableItem();
                tmpFonc->GetArguListe()->AddDebut(tmpVar);
                tmpVar->SetTagNom(Cour);
                State = ST_FUNC04;
            }
            else if (Cour->GetToken() == TOKEN_CLOSEPAR)
            {
                State = ST_FUNC08;
            }
            else
            {
                errListe->Add("On attend une déclaration d'argument ou ')' ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_INTRA;
            }
            break;
        case ST_FUNC04:
            if (Cour->GetToken() == TOKEN_AS)
                State = ST_FUNC05;
            else
            {
                errListe->Add("On attend 'as' ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_INTRA;
            }
            break;
        case ST_FUNC05:
            if (Cour->GetToken() == TOKEN_IDENTIF || IsAType(Cour))
            {
                tmpVar->SetType(GetVarType(Cour));
                tmpVar->SetTagType(Cour);
                tmpVar->SetCar(VR_ARGU);
                tmpVar->SetFunc(tmpFonc);
                // On a d?clar? un argument

                State = ST_FUNC06;
            }
            else
            {
                errListe->Add("On attend un type ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_INTRA;
            }
            break;
        case ST_FUNC06:
            if (Cour->GetToken() == TOKEN_VIRGULE)
                State = ST_FUNC07;
            else if (Cour->GetToken() == TOKEN_CLOSEPAR)
                State = ST_FUNC08;
            else
            {
                errListe->Add("On attend ',' ou ')' ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_INTRA;
            }
            break;
        case ST_FUNC07:
            if (Cour->GetToken() == TOKEN_IDENTIF)
            {
                tmpVar=new VariableItem();
                tmpFonc->GetArguListe()->AddDebut(tmpVar);
                tmpVar->SetTagNom(Cour);
                State = ST_FUNC04;
            }
            else
            {
                errListe->Add("On attend une déclaration d'argument ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_INTRA;
            }
            break;
        case ST_FUNC08:
            if (Cour->GetToken() == TOKEN_AS)
                State = ST_FUNC09;
            else
            {
                errListe->Add("On attend 'as' ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_INTRA;
            }
            break;
        case ST_FUNC09:
            if (Cour->GetToken() == TOKEN_IDENTIF || IsAType(Cour))
            {
                tmpFonc->SetRetType(GetVarType(Cour));
                State = ST_FUNC10;
            }
            else
            {
                errListe->Add("On attend un type ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_INTRA;
            }
            break;
        case ST_FUNC10:
            if (Cour->GetToken() == TOKEN_CRLF)
            {
                // il faut d?clarer la fonction
                State = ST_INTRA;
            }
            else
            {
                errListe->Add("Inattendu ",Cour);
                AvanceNextLigne(&Cour);
                State = ST_INTRA;
            }
            break;
        default:
            break;

        }
        if (Cour)
        {
            precCour = Cour;
            Cour=Cour->GetNext();
        }
    }
    return;

}



