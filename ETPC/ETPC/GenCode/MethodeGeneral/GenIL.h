#ifndef _GENIL_H_1
#define _GENIL_H_1

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "IL.h"
#include "functionitem.h"
#include "InstructionETPB.h"
#include "PileRegTemp.h"

typedef enum NatureOp{NO_REG,NO_DVAL,NO_DADR}NatureOp;


class GenIL{
private:
    Collection* errListe;
    Collection* VariablesPublic;
    Collection* Types;
    Collection* Fonctions;

    IL ilCoder;
    VirtStack virtualStack;
    PileRegTemp* registerStack;

    int mNbRegMax;

    int getNbRegArith(ASTNode* bNoeud,NatureOp bNat);
    int getNbRegObject(ASTNode* bNoeud,NatureOp bNat);

    void codeArith(ASTNode* bNoeud,NatureOp bNat,Operande** opertr);
    void codeObject(ASTNode* bNoeud,NatureOp bNat,Operande** opertr);

public:
    GenIL(void);
    ~GenIL(void);

    void testGenerate();
    void generateCode();

    void setEnvironnement(Collection* BerrListe,
                            Collection* BVariablesPublic,
                            Collection* BTypes,
                            Collection* BFonctions);
};

#endif
