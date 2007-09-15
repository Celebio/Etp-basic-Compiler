#ifndef _GENIL_H_1
#define _GENIL_H_1

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "IL.h"
#include "FonctionItem.h"
#include "InstructionETPB.h"
#include "PileRegTemp.h"

typedef enum NatureOp{NO_REG,NO_DVAL,NO_DADR};


class GenIL{
private:
	Collection* errListe;
	Collection* VariablesPublic;
	Collection* Types;
	Collection* Fonctions;

	IL ILcoder;
	VirtStack Stack;
	PileRegTemp* RegPile;

	int mNbRegMax;

	int NbRegArith(CNoeud* bNoeud,NatureOp bNat);
	int NbRegObjet(CNoeud* bNoeud,NatureOp bNat);
	
	void CodeArith(CNoeud* bNoeud,NatureOp bNat,Operande** Op);
	void CodeObjet(CNoeud* bNoeud,NatureOp bNat,Operande** Op);

public:
	GenIL(void);
	~GenIL(void);
	
	void TestGenerate();
	void GenerCode();

	void SetEnvironnement(Collection* BerrListe,
							Collection* BVariablesPublic,
							Collection* BTypes,
							Collection* BFonctions);
};

#endif