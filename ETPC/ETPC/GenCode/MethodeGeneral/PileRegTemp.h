#ifndef _PILEREGTEMP_H_1
#define _PILEREGTEMP_H_1

#include "IL.h"
#define TAILLE_PILE 160

class PileRegTemp{
private:
    reg_id Pile[TAILLE_PILE];

    void Depiler(void);
    void Empiler(reg_id R);
    int nb_reg;
    IL* mIL;
    VirtStack* mStack;
    int mTempCtr;
public:
    PileRegTemp(void);
    PileRegTemp(IL* bIL,VirtStack* bStack);
    ~PileRegTemp(void);

    void Init(int nbReg);
    void Echange(void);

    Operande* Sommet();

    void Allouer(Operande* M);
    void Liberer(Operande* M);

    Operande* AllouerTemp(int taille);
    void LibererTemp(Operande* T,int taille);
};

#endif
