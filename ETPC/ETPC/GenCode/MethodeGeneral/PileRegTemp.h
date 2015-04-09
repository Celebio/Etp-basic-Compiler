#ifndef _PILEREGTEMP_H_1
#define _PILEREGTEMP_H_1

#include "IL.h"
#define TAILLE_PILE 160

class PileRegTemp{
private:
    reg_id Pile[TAILLE_PILE];

    void depiler(void);
    void Empiler(reg_id R);
    int nb_reg;
    IL* mIL;
    VirtStack* mStack;
    int mTempCtr;
public:
    PileRegTemp(void);
    PileRegTemp(IL* bIL,VirtStack* bStack);
    ~PileRegTemp(void);

    void init(int nbReg);
    void Echange(void);

    Operande* front();

    void allocate(Operande* M);
    void freeOperand(Operande* M);

    Operande* allocateTemp(int size);
    void freeTemp(Operande* T,int size);
};

#endif
