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
    VeriSem.cpp

    Ce fichier permet d'analyser la s?mantique. Il prend en entr?e des structures
    de donn?es qui ont ?t? g?n?r? par l'analyse syntaxique. Les fonctions inclus
    dans ce fichier vont permettre de v?rifier le sens de ces donn?es.

\*********************************************************************************/

#include "verisem.h"
#include "TypeItem.h"

//#define _DEBUGSEM

VeriSem::VeriSem(){}
VeriSem::~VeriSem(){}

bool VeriSem::TypeExiste(VariableItem* bVariable)
{
    ColIterator iter1;
    TypeItem* Type1;
    bool Found=false;

    if (bVariable->getType().Type!=TP_USER && bVariable->getType().Type!=TP_UNKNOWN)
    {
        Found=true;
    } else if (bVariable->getType().Type==TP_UNKNOWN)
    {
        Found=false;
    }
    else
    {

        Types->bindIterator(&iter1);
        while(iter1.elemExists() && !Found){
            Type1 = (TypeItem*)iter1.getNext();
            if (!strcmp(Type1->getName(),bVariable->getType().Expr)){
                Found = true;
            }
        }
    }
    if (!Found)
    {
        errListe->add("Type inconnu",bVariable->GetTagType());
    }
    return Found;
}

void VeriSem::setEnvironnement(Collection* BerrListe,
                            Collection* BVariablesPublic,
                            Collection* BTypes,
                            Collection* BFonctions)
{
    errListe=BerrListe;
    VariablesPublic=BVariablesPublic;
    Types=BTypes;
    Fonctions=BFonctions;
}



VarTypeType VeriSem::GetTypeExpression(CNoeud *expr,
                                VarTypeType typeAttendu,
                                FonctionItem* foncEnCours){
    VarTypeType retVal(TP_UNKNOWN);
    VarTypeType unknownVal(TP_UNKNOWN);
    VarTypeType intVal(TP_INTEGER);
    VarTypeType longVal(TP_LONG);
    VarTypeType stringVal(TP_STRING);
    VarTypeType boolVal(TP_BOOLEAN);
    VarTypeType voidVal(TP_VOID);
    ColIterator pVarListe;
    ColIterator pFuncListe;
    ColIterator pDimListe;
    ColIterator pTypeListe;
    TypeItem* Type1;
    VariableItem* Var1;
    FonctionItem* Fonc1;
    //DimElemItem* Dim1;

    int i;
    char bufferErrDescr[300];
    TypeOptor bOp;
    if (expr==NULL)
    {
        return voidVal;
    }
#ifdef _DEBUGSEM
    printf("<%s>\n",expr->getNodeRepr());
#endif
    switch(expr->getNature())
    {
    case NODE_UNKNOWN:
        break;
    case NODE_OPERATOR:
        bOp=expr->getOperator();
        if (bOp==OPTOR_POINT)   // operateur d'acc?s
        {
            VarTypeType bType;
            bType=GetTypeExpression(expr->getLeftChild(),unknownVal,foncEnCours);
            #ifdef _DEBUGSEM

                printf("on dirait un ");
                bType.display();
                printf(" a gauche\n");
            #endif
            if (bType.Type==TP_USER)    // cest un user define type ? gauche du token_point
            {
                Types->bindIterator(&pTypeListe);
                while (pTypeListe.elemExists()){
                    Type1=(TypeItem*)pTypeListe.getNext();
                    if (!strcmp(Type1->getName(),bType.Expr)){
                        printf("les champs du type:\n");
                        Type1->getFieldList()->display();
                        printf("\n\n");
                        Type1->getFieldList()->bindIterator(&pVarListe);
                        while (pVarListe.elemExists()){
                            Var1=(VariableItem*)pVarListe.getNext();
                            if (!strcmp(Var1->getTagName()->GetIdentif(),expr->getRightChild()->getTag()->GetIdentif())){
                                #ifdef _DEBUGSEM
                                    printf("le champs %s existe dans le type %s\n",Var1->getTagName()->GetIdentif(),bType.Expr);
                                #endif
                                // verification eventuelle pour la size du tableau
                                Var1->GetDimListe()->bindIterator(&pDimListe);
                                i=0;
                                while (pDimListe.elemExists()){
                                    pDimListe.getNext();
                                    i++;
                                }
                                if (expr->getRightChild()->getSuccNmbr()!=i){
                                    sprintf(bufferErrDescr,"Erreur de dimension:le tableau a %i dimension(s) et on en fournit %i",i,expr->getRightChild()->getSuccNmbr());
                                    errListe->add(bufferErrDescr,expr->getRightChild()->getTag());
                                    break;
                                }
                                for (i=0;i<expr->getRightChild()->getSuccNmbr();i++)
                                {
                                    GetTypeExpression(*(expr->getRightChild()->getSuccPtr(i)),intVal,foncEnCours);
                                }
                                retVal=Var1->getType();
                                break;
                            }
                        }
                        if (retVal==unknownVal){
                            sprintf(bufferErrDescr,"Impossible de trouver le champs %s du type %s ",expr->getRightChild()->getTag()->GetIdentif(),bType.Expr);
                            errListe->add(bufferErrDescr,expr->getRightChild()->getTag());
                            break;
                        }
                        break;
                    }
                }
            }
            else if (bType.Type!=TP_UNKNOWN)
            {
                // traitement des types pr?d?finis avec champs..
                // possibilit? d'ajouter myString.champ
                // mais en attendant : ...
                sprintf(bufferErrDescr,"Impossible de trouver le champs %s ",expr->getRightChild()->getTag()->GetIdentif());
                errListe->add(bufferErrDescr,expr->getRightChild()->getTag());
                break;
            }
            break;
        }
        else if (bOp==OPTOR_EXCLAM)
        {

            VarTypeType bType;
            bType=GetTypeExpression(expr->getLeftChild(),unknownVal,foncEnCours);
            if (bType.Type!=TP_INTEGER && bType.Type!=TP_LONG)
            {
                errListe->add("On attend un Integer ou un Long",expr->getTag());
                break;
            }
            if (typeAttendu.Type==TP_INTEGER || typeAttendu.Type==TP_LONG)
            {
                retVal=typeAttendu;
            }
        }
        else if (bOp==OPTOR_NOT)
        {
            VarTypeType bType;
            bType=GetTypeExpression(expr->getRightChild(),typeAttendu,foncEnCours);
            if (bType.Type!=TP_BOOLEAN && bType.Type!=TP_INTEGER && bType.Type!=TP_LONG)
            {
                errListe->add("On attend un Boolean,Integer ou un Long",expr->getTag());
                break;
            }
            if (typeAttendu.Type==TP_BOOLEAN || typeAttendu.Type==TP_INTEGER || typeAttendu.Type==TP_LONG)
            {
                retVal=typeAttendu;
            }
        }
        else if (bOp==OPTOR_DIV || bOp==OPTOR_MULT || bOp==OPTOR_SUB || bOp==OPTOR_ADD)
        {
            //VarTypeType gType;
            //VarTypeType dType;
            GetTypeExpression(expr->getLeftChild(),typeAttendu,foncEnCours);
            GetTypeExpression(expr->getRightChild(),typeAttendu,foncEnCours);
            retVal=typeAttendu;
        }
        else if (bOp==OPTOR_MOINSUNAIRE){
            GetTypeExpression(expr->getRightChild(),typeAttendu,foncEnCours);
            retVal=typeAttendu;
        }
        else if (bOp==OPTOR_CONCAT)
        {
            retVal=stringVal;
            GetTypeExpression(expr->getLeftChild(),stringVal,foncEnCours);
            GetTypeExpression(expr->getRightChild(),stringVal,foncEnCours);
        }
        else if (bOp==OPTOR_INF || bOp==OPTOR_INFEQ || bOp==OPTOR_SUP
                    || bOp==OPTOR_SUPEQ || bOp==OPTOR_EQUAL || bOp==OPTOR_DIFFERENT
                    || bOp==OPTOR_CMP_AND || bOp==OPTOR_CMP_OR || bOp==OPTOR_CMP_XOR)
        {
            // operateurs de comparaison
            VarTypeType typeG;
            VarTypeType typeD;
            typeG=GetTypeExpression(expr->getLeftChild(),unknownVal,foncEnCours);
            typeD=GetTypeExpression(expr->getRightChild(),unknownVal,foncEnCours);
            /* long emporte sur integer     */
            if (typeG.Type==TP_LONG && typeD.Type==TP_INTEGER)
            {
                typeD=GetTypeExpression(expr->getRightChild(),longVal,foncEnCours);
            }
            else if (typeD.Type==TP_LONG && typeG.Type==TP_INTEGER)
            {
                typeG=GetTypeExpression(expr->getRightChild(),longVal,foncEnCours);
            }

            /* tout importe sur unknown */
            if (typeG!=unknownVal && typeD==unknownVal){
                typeD=GetTypeExpression(expr->getRightChild(),typeG,foncEnCours);
             } else if (typeG==unknownVal && typeD!=unknownVal){
                typeG=GetTypeExpression(expr->getLeftChild(),typeD,foncEnCours);
            }


            if (typeG!=typeD)
            {
                sprintf(bufferErrDescr,"Impossible de comparer deux elements de type different. A gauche il y a %s et a droite %s",typeG.VarTypeTypeImage(),typeD.VarTypeTypeImage());
                errListe->add(bufferErrDescr,expr->getTag());
                break;
            }

            if (bOp==OPTOR_INF || bOp==OPTOR_INFEQ || bOp==OPTOR_SUP || bOp==OPTOR_SUPEQ)
            {
                // les expressions doivent etre d'un type muni d'une relation d'ordre
                if (typeG.Type!=TP_INTEGER && typeG.Type!=TP_LONG && typeG.Type!=TP_FLOAT
                    && typeG.Type!=TP_BYTE)
                {
                    sprintf(bufferErrDescr,"Impossible de comparer deux elements de type %s",typeG.VarTypeTypeImage());
                    errListe->add(bufferErrDescr,expr->getTag());
                    break;
                }
                typeD=GetTypeExpression(expr->getRightChild(),typeG,foncEnCours);
                retVal=boolVal;
            }
            else if (bOp==OPTOR_EQUAL || bOp==OPTOR_DIFFERENT)
            {
                // il faut que typeG==typeD, deja v?rifi? pr?cedemment
                retVal=boolVal;
            }
            else if (bOp==OPTOR_CMP_AND || bOp==OPTOR_CMP_OR || bOp==OPTOR_CMP_XOR)
            {
                if (typeG.Type!=TP_BOOLEAN && typeG.Type!=TP_INTEGER && typeG.Type!=TP_LONG
                    && typeG.Type!=TP_BYTE)
                {
                    sprintf(bufferErrDescr,"Impossible de comparer deux elements de type %s",typeG.VarTypeTypeImage());
                    errListe->add(bufferErrDescr,expr->getTag());
                    break;
                }
                // il faut que typeG==typeD, deja v?rifi? pr?cedemment
                if (bOp==OPTOR_CMP_XOR && typeG.Type!=TP_BOOLEAN){
                    errListe->add("Impossible d'appliquer un XOR sur des booleens",expr->getTag());
                    break;
                }
                if (typeG.Type==TP_BYTE || typeG.Type==TP_INTEGER || typeG.Type==TP_LONG){
                    // C'est en fait un op?rateur arithm?tique
                    expr->setOperatorArith();
                    bOp=expr->getOperator();
                    retVal = typeG;
                } else {
                    retVal=boolVal;
                }
            }
        }


        //if (expr->getRightChild())

        break;
    case NODE_OPERAND_CONSTANT:
        #ifdef _DEBUGSEM
            printf("Donnee constante %i\n",1<<15);
        #endif
        if (expr->getTag()->GetToken()==TOKEN_NOMBRE)
        {
            Tokenizer bTok;
            if (bTok.IsNumeric(expr->getTag()->GetIdentif())==ISINTEGER)
            {
                if (typeAttendu.Type==TP_FLOAT){
                    retVal=typeAttendu;
                    break;
                }
                if (abs(atoi(expr->getTag()->GetIdentif()))<(1<<15))
                {
                    if (typeAttendu.Type==TP_INTEGER || typeAttendu.Type==TP_LONG)
                        retVal=typeAttendu;
                    else
                        retVal.Type=TP_INTEGER;
                }
                else
                {
                    retVal.Type=TP_LONG;
                }
            }
            else if (bTok.IsNumeric(expr->getTag()->GetIdentif())==ISFLOAT)
            {
                retVal.Type=TP_FLOAT;
            }
        }
        else if (expr->getTag()->GetToken()==TOKEN_STRINGCONSTANT)
        {
            retVal.Type=TP_STRING;
        }
        else if (expr->getTag()->GetToken()==TOKEN_TRUE || expr->getTag()->GetToken()==TOKEN_FALSE)
        {
            retVal.Type=TP_BOOLEAN;
        }
        break;
    case NODE_OPERAND_VARIABLE:
        #ifdef _DEBUGSEM
            printf("trouve une variable\n");
        #endif
        // EST-CE une variable LOCALE ???
        foncEnCours->getVariableList()->bindIterator(&pVarListe);
        while(pVarListe.elemExists()){
            Var1=(VariableItem*)pVarListe.getElem();
            if (!strcmp(Var1->getTagName()->GetIdentif(),expr->getTag()->GetIdentif())){
                #ifdef _DEBUGSEM
                    printf("c'est une variable locale \n");
                #endif
                retVal=Var1->getType();
                break;
            }
            pVarListe.getNext();
        }
        if (pVarListe.elemExists()){
            Var1=(VariableItem*)pVarListe.getElem();
            i=0;
            Var1->GetDimListe()->bindIterator(&pDimListe);

            while (pDimListe.elemExists()){
                i++;
                pDimListe.getNext();
            }
            if (expr->getSuccNmbr()!=i){
                sprintf(bufferErrDescr,"Erreur de dimension:le tableau a %i dimension(s) et on en fournit %i",i,expr->getSuccNmbr());
                errListe->add(bufferErrDescr,expr->getTag());
                break;
            }
            for (i=0;i<expr->getSuccNmbr();i++)
            {
                GetTypeExpression(*(expr->getSuccPtr(i)),intVal,foncEnCours);
            }
        }
        #ifdef _DEBUGSEM
        printf("c'est pas une locale\n");
        #endif


        // EST-CE un parametre de la fonction ???
        foncEnCours->getArgumentList()->bindIterator(&pVarListe);
        while (pVarListe.elemExists()){
            Var1=(VariableItem*)pVarListe.getElem();
            if (!strcmp(Var1->getTagName()->GetIdentif(),expr->getTag()->GetIdentif())){
                #ifdef _DEBUGSEM
                    printf("c'est un parametre de la fonction \n");
                #endif
                retVal=Var1->getType();
                break;
            }
            pVarListe.getNext();
        }
        if (pVarListe.elemExists()){
            Var1->GetDimListe()->bindIterator(&pDimListe);
            i=0;
            while (pDimListe.elemExists()){
                i++;
                pDimListe.getNext();
            }
            if (expr->getSuccNmbr()!=i)
            {
                sprintf(bufferErrDescr,"Erreur de dimension:le tableau a %i dimension(s) et on en fournit %i",i,expr->getSuccNmbr());
                errListe->add(bufferErrDescr,expr->getTag());
                break;
            }
            for (i=0;i<expr->getSuccNmbr();i++)
            {
                GetTypeExpression(*(expr->getSuccPtr(i)),intVal,foncEnCours);
            }
        }
        #ifdef _DEBUGSEM
            printf("c'est pas un parametre de la fonction \n");
        #endif



        // EST-CE une variable PUBLIQUE ???
        VariablesPublic->bindIterator(&pVarListe);
        while (pVarListe.elemExists()){
            Var1=(VariableItem*)pVarListe.getElem();
            if (!strcmp(Var1->getTagName()->GetIdentif(),expr->getTag()->GetIdentif())){
                #ifdef _DEBUGSEM
                    printf("c'est une variable publique \n");
                #endif
                retVal=Var1->getType();
                break;
            }
            pVarListe.getNext();
        }
        if (pVarListe.elemExists()){
            Var1->GetDimListe()->bindIterator(&pDimListe);
            i=0;
            while (pDimListe.elemExists()){
                i++;
                pDimListe.getNext();
            }
            if (expr->getSuccNmbr()!=i)
            {
                sprintf(bufferErrDescr,"Erreur de dimension:le tableau a %i dimension(s) et on en fournit %i",i,expr->getSuccNmbr());
                errListe->add(bufferErrDescr,expr->getTag());
                break;
            }
            for (i=0;i<expr->getSuccNmbr();i++)
            {
                GetTypeExpression(*(expr->getSuccPtr(i)),intVal,foncEnCours);
            }
        }
        //break;
        #ifdef _DEBUGSEM
            printf("c'est pas un public \n");
        #endif

        break;
    case NODE_OPERAND_FUNCTION:
        #ifdef _DEBUGSEM
            printf("utilise comme fonction\n");
        #endif
        Fonctions->bindIterator(&pFuncListe);
        while (pFuncListe.elemExists()){
            Fonc1=(FonctionItem*)pFuncListe.getElem();
            if (!strcmp(Fonc1->getName(),expr->getTag()->GetIdentif())){
                #ifdef _DEBUGSEM
                    printf("C'est bien declared..\n");
                #endif
                retVal=Fonc1->getReturnType();
                Fonc1->setUsed();
                break;
            }
            pFuncListe.getNext();
        }
        if (!pFuncListe.elemExists()){
            break;
        }
        Fonc1->getArgumentList()->bindIterator(&pVarListe);
        i=0;
        while (pVarListe.elemExists()){
            i++;
            pVarListe.getNext();
        }
        #ifdef _DEBUGSEM
            printf("utilisateur a entre %i arguments et la fonction en attend %i\n",expr->getSuccNmbr(),i);
        #endif
        if (expr->getSuccNmbr()!=i)
        {
            sprintf(bufferErrDescr,"Nombre d'arguments incorrect:il y a %i argument(s) et la fonction en attend %i",expr->getSuccNmbr(),i);
            errListe->add(bufferErrDescr,expr->getTag());
            break;
        }
        Fonc1->getArgumentList()->bindIterator(&pVarListe);

        // il faut verifier ? l'envers, car la liste des arguments est invers?e
        for (i=0;i<expr->getSuccNmbr();i++)
        {
            Var1=(VariableItem*)pVarListe.getElem();
            GetTypeExpression(*(expr->getSuccPtr(expr->getSuccNmbr()-1-i)),Var1->getType(),foncEnCours);
            pVarListe.getNext();
        }
        break;
    case NODE_OPERAND_ARRAY:
        #ifdef _DEBUGSEM
            printf("Trouve en tant que array\n");
        #endif
        // EST-CE un tableau LOCAL ???
        foncEnCours->getVariableList()->bindIterator(&pVarListe);
        while(pVarListe.elemExists()){
            Var1=(VariableItem*)pVarListe.getElem();
            if (!strcmp(Var1->getTagName()->GetIdentif(),expr->getTag()->GetIdentif())){
                #ifdef _DEBUGSEM
                    printf("c'est un tableau locale \n");
                #endif
                retVal=Var1->getType();
                break;
            }
            pVarListe.getNext();
        }
        if (!pVarListe.elemExists()){
            #ifdef _DEBUGSEM
                printf("c'est pas un tableau local\n");
            #endif
            // EST-CE un tableau PUBLIQUE ???
            VariablesPublic->bindIterator(&pVarListe);
            while (pVarListe.elemExists()){
                Var1=(VariableItem*)pVarListe.getElem();
                if (!strcmp(Var1->getTagName()->GetIdentif(),expr->getTag()->GetIdentif())){
                    #ifdef _DEBUGSEM
                        printf("c'est un tableau publique \n");
                    #endif
                    retVal=Var1->getType();
                    break;
                }
                pVarListe.getNext();
            }
            #ifdef _DEBUGSEM
                if (!pVarListe.elemExists()){
                    printf("c'est pas un tableau public \n");
                }
            #endif
        }
        if (pVarListe.elemExists()){
            Var1->GetDimListe()->bindIterator(&pDimListe);
            i=0;
            while (pDimListe.elemExists()){
                i++;
                pDimListe.getNext();
            }
            if (expr->getSuccNmbr()!=i)
            {
                sprintf(bufferErrDescr,"Erreur de dimension:le tableau a %i dimension(s) et on en fournit %i",i,expr->getSuccNmbr());
                errListe->add(bufferErrDescr,expr->getTag());
                break;
            }
            for (i=0;i<expr->getSuccNmbr();i++)
            {
                GetTypeExpression(*(expr->getSuccPtr(i)),intVal,foncEnCours);
            }
        }
        break;


    }   // fin du switch

#ifdef _DEBUGSEM
    printf("donc le type de <%s> serait:",expr->getNodeRepr());
    retVal.display();
    printf("\n");
#endif
    if (retVal.Type==TP_UNKNOWN)        // element non declar?
    {
        if (expr->getNature()==NODE_OPERAND_FUNCTION)
            errListe->add("Fonction non declare",expr->getTag());
        else if (expr->getNature()==NODE_OPERAND_VARIABLE)
            errListe->add("Variable non declare",expr->getTag());
        else if (expr->getNature()==NODE_OPERAND_ARRAY)
            errListe->add("Tableau non declare",expr->getTag());
        else if (expr->getNature()==NODE_OPERATOR)
            ;
        else
            errListe->add("Element non declare",expr->getTag());
        expr->setType(retVal);
        return retVal;
    }
    if ((typeAttendu.Type)!=TP_UNKNOWN)
    {
        if (typeAttendu!=retVal)        // vive la surcharge!!
        //if (typeAttendu.Type!=retVal.Type || (typeAttendu.Type==retVal.Type && retVal.Type==TP_USER && strcmp(retVal.Expr,typeAttendu.Expr)))
        {
            // conversion possible?
            // sinon:
            sprintf(bufferErrDescr,"Type incorrect. Conversion impossible de %s en %s",retVal.VarTypeTypeImage(),typeAttendu.VarTypeTypeImage());
            if (expr->getNature() == NODE_OPERATOR)
                errListe->add(bufferErrDescr,expr->getRightChild()->getTag());
            else
                errListe->add(bufferErrDescr,expr->getTag());
        }
        //expr->setType(retVal);
        //return retVal;
    }
    expr->setType(retVal);
    return retVal;

}
void VeriSem::VerifSemInstr(Collection *bInstrListe,FonctionItem* foncEnCours){
    ColIterator iter1;
    InstructionETPB* bInst;
    bInstrListe->bindIterator(&iter1);
    while (iter1.elemExists()){
        bInst=(InstructionETPB*)iter1.getNext();
        VerifSemInstr(bInst,foncEnCours);
    }
}

void VeriSem::VerifSemInstr(InstructionETPB *bInstr,FonctionItem* foncEnCours){
    Collection* listElseIfExpr;
    Collection* listElseIfCorps;
    ColIterator iterElseIfExpr;
    ColIterator iterElseIfCorps;
    VarTypeType intVal(TP_INTEGER);
    VarTypeType longVal(TP_LONG);
    VarTypeType byteVal(TP_BYTE);
    VarTypeType typeAux,typePro;
    CNoeud *forExprTreestep = NULL;

    switch(bInstr->getNat())
    {
    case INS_UNKNOWN:
        break;
    case INS_DECLARATION:
        //printf("Declaration\n");
        break;
    case INS_AFFECTATION:
        typePro.Type=TP_UNKNOWN;
        typePro.Expr=NULL;
        typeAux=GetTypeExpression(bInstr->getAssignmentExprAssigned(),typePro,foncEnCours);
        if (bInstr->getAssignmentExprTree()){
            GetTypeExpression(bInstr->getAssignmentExprTree(),typeAux,foncEnCours);
            bInstr->getAssignmentExprTree()->simplifyConstantExpression();
        }
        else
        {
            errListe->add("Expression d'assignement vide",bInstr->getAssignmentExprAssigned()->getTag());
            break;
        }
        break;
    case INS_STRUCT_FOR:
        //printf("For\n");
        typePro.Type=TP_UNKNOWN;
        typePro.Expr=NULL;
        typeAux=GetTypeExpression(bInstr->getForExprAssigned(),typePro,foncEnCours);
        if (typeAux != intVal && typeAux != longVal && typeAux != byteVal){
            errListe->add("L'iterateur doit etre Integer ou Long",bInstr->getForExprAssigned()->getTag());
            break;
        }

        GetTypeExpression(bInstr->getForExprInitTree(),typeAux,foncEnCours);
        GetTypeExpression(bInstr->getForExprToTree(),typeAux,foncEnCours);
        GetTypeExpression(bInstr->getForExprStepTree(),typeAux,foncEnCours);
        bInstr->getForExprInitTree()->simplifyConstantExpression();
        bInstr->getForExprToTree()->simplifyConstantExpression();
        forExprTreestep = bInstr->getForExprStepTree();
        if (forExprTreestep){
            forExprTreestep->simplifyConstantExpression();
        }
        VerifSemInstr(bInstr->getForBody(),foncEnCours);
        break;
    case INS_STRUCT_DOLPWH:
        typePro.Type=TP_BOOLEAN;
        typePro.Expr=NULL;
        GetTypeExpression(bInstr->getDoExprCondition(),typePro,foncEnCours);
        bInstr->getDoExprCondition()->simplifyConstantExpression();
        VerifSemInstr(bInstr->getDoBody(),foncEnCours);
        break;
    case INS_STRUCT_DOWH:
        typePro.Type=TP_BOOLEAN;
        typePro.Expr=NULL;
        GetTypeExpression(bInstr->getDoExprCondition(),typePro,foncEnCours);
        bInstr->getDoExprCondition()->simplifyConstantExpression();
        VerifSemInstr(bInstr->getDoBody(),foncEnCours);
        break;
    case INS_IF:
        typePro.Type=TP_BOOLEAN;
        typePro.Expr=NULL;
        GetTypeExpression(bInstr->getIfExprTree(),typePro,foncEnCours);
        bInstr->getIfExprTree()->simplifyConstantExpression();

        VerifSemInstr(bInstr->getIfIfBody(),foncEnCours);

        listElseIfExpr=bInstr->getIfExprElseIf();
        listElseIfCorps=bInstr->getIfElseIfBody();

        listElseIfExpr->bindIterator(&iterElseIfExpr);
        listElseIfCorps->bindIterator(&iterElseIfCorps);
        while (iterElseIfCorps.elemExists())
        {
            VerifSemInstr((Collection *)iterElseIfCorps.getNext(),foncEnCours);
            GetTypeExpression((CNoeud*)iterElseIfExpr.getNext(),typePro,foncEnCours);
        }
        if (iterElseIfExpr.elemExists())
        {
            printf("ERREUR!!!!");           // car normalement ils s'annulent en meme temps
        }
        VerifSemInstr(bInstr->getIfElseBody(),foncEnCours);
        break;
    case INS_CALL:
        typePro.Type=TP_VOID;
        typePro.Expr=NULL;
        GetTypeExpression(bInstr->getExprFunctionCall(),typePro,foncEnCours);
        break;
    case INS_RETURN:
        GetTypeExpression(bInstr->getExprReturn(),foncEnCours->getReturnType(),foncEnCours);
        bInstr->getExprReturn()->simplifyConstantExpression();
        break;
    }
}

void VeriSem::VerifSem(){

    char errMsgBuf[500];
    VarTypeType fooType;
    ColIterator iter1;
    ColIterator iter2;
    ColIterator iter3;
    TypeItem* Type1;
    TypeItem* Type2;
    VariableItem* Var1;
    VariableItem* Var2;
    FonctionItem* Fonc1;

    // Verification des types, des declarations circulaires

    // double déclaration des types et existance du type de chaque champs de chaque type
    Types->bindIterator(&iter1);
    while(iter1.elemExists()){
        iter2.initHead(iter1.getCour()->next);
        Type1 = (TypeItem*)iter1.getNext();
        //Type1->display();
        while(iter2.elemExists()){
            Type2 = (TypeItem*)iter2.getNext();
            //Type2->display();
            if (!strcmp(Type2->getName(),Type1->getName())){
                sprintf(errMsgBuf,"%s est declare plusieurs fois",Type2->getName());
                errListe->add(errMsgBuf,Type2->getTagName());
            }
        }
        // existence des types des champs
        Type1->getFieldList()->bindIterator(&iter2);
        while (iter2.elemExists()){
            Var1 = (VariableItem*)iter2.getNext();
            //Var1->display();
            TypeExiste(Var1);
        }
        fooType.Type=TP_USER;
        fooType.Expr=Type1->getName();
        UpdateVarSize(NULL,fooType,false);
    }




    // Verification de double definition des variables publiques
    // ainsi que l'existence des types
    VariablesPublic->bindIterator(&iter1);
    while (iter1.elemExists()){
        iter2.initHead(iter1.getCour()->next);
        Var1=(VariableItem*)iter1.getNext();
        while (iter2.elemExists()){
            Var2=(VariableItem*)iter2.getNext();
            if (!strcmp(Var1->getTagName()->GetIdentif(),Var2->getTagName()->GetIdentif())){
                sprintf(errMsgBuf,"%s est declare plusieurs fois",Var2->getTagName()->GetIdentif());
                errListe->add(errMsgBuf,Var2->getTagName());
            }
        }
        TypeExiste(Var1);
    }

    Fonctions->bindIterator(&iter1);
    while (iter1.elemExists()){
        Fonc1=(FonctionItem*)iter1.getNext();
        // Verification de double definition des parametres
        Fonc1->getArgumentList()->bindIterator(&iter2);
        while (iter2.elemExists()){
            iter3.initHead(iter2.getCour()->next);
            Var1=(VariableItem*)iter2.getNext();
            while (iter3.elemExists()){
                Var2=(VariableItem*)iter3.getNext();
                if (!strcmp(Var1->getTagName()->GetIdentif(),Var2->getTagName()->GetIdentif())){
                    sprintf(errMsgBuf,"%s est declare plusieurs fois",Var2->getTagName()->GetIdentif());
                    errListe->add(errMsgBuf,Var2->getTagName());
                }
            }
            if(TypeExiste(Var1))
            {
                // les parametres sont updat?s avec UDasPtr = true parce que le passage en argument
                // d'un user-defined type doit etre de type pointeur
                UpdateVarSize(Var1,Var1->getType(),true);
            }
        }

        // Verification de double definition des variables locales
        Fonc1->getVariableList()->bindIterator(&iter2);
        while (iter2.elemExists()){
            iter3.initHead(iter2.getCour()->next);
            Var1=(VariableItem*)iter2.getNext();
            while (iter3.elemExists()){
                Var2=(VariableItem*)iter3.getNext();
                if (!strcmp(Var1->getTagName()->GetIdentif(),Var2->getTagName()->GetIdentif())){
                    sprintf(errMsgBuf,"%s est declare plusieurs fois",Var2->getTagName()->GetIdentif());
                    errListe->add(errMsgBuf,Var2->getTagName());
                }
            }
            // regarder les similitudes avec les arguments
            Fonc1->getArgumentList()->bindIterator(&iter3);
            while (iter3.elemExists()){
                Var2=(VariableItem*)iter3.getNext();
                if (!strcmp(Var1->getTagName()->GetIdentif(),Var2->getTagName()->GetIdentif())){
                    sprintf(errMsgBuf,"%s est declare plusieurs fois",Var2->getTagName()->GetIdentif());
                    errListe->add(errMsgBuf,Var2->getTagName());
                }
            }

            if(TypeExiste(Var1))
            {
                // on a UDasPtr = false pour pouvoir cr?er un nouvel element local
                UpdateVarSize(Var1,Var1->getType(),false);
            }
        }

        // verification des expressions contenus dans les instructions
        VerifSemInstr(Fonc1->getInstructionList(),Fonc1);

    }

    /*
    while (FuncListe)
    {

        // verification des expressions contenus dans les instructions
        instructionList=FuncListe->Elem->instructionList;
        while (instructionList)
        {
            VerifSemInstr(instructionList->Elem,FuncListe->Elem,errListe);
            instructionList=instructionList->next;
        }
        FuncListe=FuncListe->next;
    }
    */
}


int VeriSem::UpdateVarSize(VariableItem* bVar,VarTypeType bType,bool UDasPtr){
    // si UDasPtr vaut true, les user-defined types est de type pointeur donc de size 4
    int somme;
    int retSize;
    if (bVar) bVar->SetPointed(false);
    switch(bType.Type)
    {
    case TP_INTEGER:
        retSize=2;
        break;
    case TP_FLOAT:
        retSize= 10;    // a voir selon les architectures
        break;
    case TP_LONG:
        retSize= 4;
        break;
    case TP_STRING:
        retSize= 4; // pointeur
        if (bVar) bVar->SetPointed(true);
        break;
    case TP_BOOLEAN:
        retSize= 2; // et pas retSize= 1;
        break;
    case TP_FILE:
        retSize= 4; // pointeur
        if (bVar) bVar->SetPointed(true);
        break;
    case TP_LIST:
        retSize= 4; // pointeur
        if (bVar) bVar->SetPointed(true);
        break;
    case TP_BYTE:
        retSize= 2; // et pas retSize= 1;
        break;
    case TP_USER:
        if (UDasPtr)
        {
            retSize= 4;
            if (bVar) bVar->SetPointed(true);
        }
        else
        {
            somme=0;
            ColIterator iter1;
            ColIterator iter2;
            TypeItem* Type1;
            VariableItem* Var1;

            Types->bindIterator(&iter1);
            while(iter1.elemExists()){
                Type1 = (TypeItem*)iter1.getNext();
                if (!strcmp(Type1->getName(),bType.Expr)){
                    Type1->getFieldList()->bindIterator(&iter2);
                    while (iter2.elemExists()){
                        Var1=(VariableItem*)iter2.getNext();
                        somme += UpdateVarSize(Var1,Var1->getType(),true);
                    }
                }
            }
            retSize= somme;
        }
        break;
    case TP_VOID:
        retSize= 0;
        break;
    case TP_UNKNOWN:
        retSize= 0;
        break;
    }
    if (bVar) bVar->SetSize(retSize);
    return retSize;
}




