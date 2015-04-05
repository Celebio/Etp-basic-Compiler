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
#ifndef _NOEUD_H_1
#define _NOEUD_H_1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "TAG.h"
#include "VarTypeType.h"
#include "ColItem.h"


typedef enum NatureNoeud{
    NOEUD_UNKNOWN,
    NOEUD_OPERATOR,
    NOEUD_OPERANDE_CTE,
    NOEUD_OPERANDE_VARIABLE,
    NOEUD_OPERANDE_FONCTION,
    NOEUD_OPERANDE_ARRAY
} NatureNoeud;


typedef enum natureOperator {NOTOP,OPBIN,OPUNG,OPUND} natureOperator;

// les op?rateurs dans l'ordre de priorit?
typedef enum TypeOptor{

    // c'est pas un operateur
    OPTOR_UNKNOWN,
    // operateur virtuel '('
    OPTOR_OPENPAR,
    OPTOR_OPENCRO,

    OPTOR_POINT,    // pour les trucs du genre masprite.x

    // les operateurs unaires
    OPTOR_EXCLAM,
    //OPTOR_NOT,         NOT doit etre apres INF,INFEQ,etc...
    OPTOR_MOINSUNAIRE,
    // op?rations arithm?tiques

    OPTOR_DIV,
    OPTOR_MULT,
    OPTOR_SUB,
    OPTOR_ADD,

    OPTOR_ARI_AND,
    OPTOR_ARI_OR,
    OPTOR_ARI_XOR,

    // op?rations sur les strings
    OPTOR_CONCAT,
    // comparateurs
    OPTOR_INF,
    OPTOR_INFEQ,
    OPTOR_EQUAL,
    OPTOR_DIFFERENT,
    OPTOR_SUP,
    OPTOR_SUPEQ,

    OPTOR_NOT,      // un autre operateur unaire de negation logique ou arithmetique

    // op?rateurs logiques
    OPTOR_CMP_AND, // ..AND pour le comparaison
    OPTOR_CMP_OR,
    OPTOR_CMP_XOR,

    // le modulo
    OPTOR_MOD
} TypeOptor;


extern TypeOptor GiveOperatorType(enumTokenType bToken);

class CNoeud :
    public ColItem
{
private:
    NatureNoeud mNature;
    CNoeud* mFilsG;
    CNoeud* mFilsD;
    CNoeud* Successeur[30]; // pour les arbres non-binaires

    CNoeud* mParent;
    TAG* mTAG;

    TypeOptor mOperator;
    VarTypeType mType;
    natureOperator mOperType;

    int nbReg;      // nombre de registre n?cessaire
    int valConstInt;
    float valConstFloat;
    bool valConstBool;
    char* valConstString;

    natureOperator GetOperType(TAG* bTag);
public:

    CNoeud(void);
    CNoeud(TAG* bTAG);
    ~CNoeud(void);


    CNoeud* AddFilsG(TAG* bTAG);
    CNoeud* AddFilsD(TAG* bTAG);

    void Afficher(const char *S,const char *SD,const char *SG);
    void Afficher();
    void Afficher(int indent);
    const char *ImageNoeud();

    void DestroyArbre();
    void SetFilsG(CNoeud* bNoeud);
    void SetFilsD(CNoeud* bNoeud);
    void SetType(VarTypeType bType);
    void SetAsFonction();   // identifiant est une fonction
    void SetAsArray();      // identifiant est un tableau
    void SetOperatorMoinsUnaire();
    void SetOperatorArith();    // cmp_and devient ari_and etc..
    void SetNbReg(int Nb);

    bool EstFeuille();
    bool EstCommutatif();
    bool EstConstant();
    void SimplifyConstantExpression();

    CNoeud* GetFilsG();
    CNoeud* GetFilsD();
    CNoeud** GetSuccPtr(int index);
    int GetSuccNmbr();
    NatureNoeud GetNature();
    VarTypeType GetType();
    TAG* GetTAG();
    TypeOptor GetOperator();
    int GetNbReg();
    natureOperator GetOperType();
};


#endif
