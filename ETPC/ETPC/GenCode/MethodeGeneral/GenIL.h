#ifndef _GENIL_H_1
#define _GENIL_H_1

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "IL.h"
#include "FonctionItem.h"
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
    VirtStack Stack;
    PileRegTemp* registerStack;

    int mNbRegMax;

    int getNbRegArith(CNoeud* bNoeud,NatureOp bNat);
    int getNbRegObject(CNoeud* bNoeud,NatureOp bNat);

    void codeArith(CNoeud* bNoeud,NatureOp bNat,Operande** opertr);
    void codeObject(CNoeud* bNoeud,NatureOp bNat,Operande** opertr);

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
