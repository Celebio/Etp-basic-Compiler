#ifndef _VIRTSTACK_H_1
#define _VIRTSTACK_H_1

#include "VariableItem.h"

#define SP_REG -1           // le registre SP a l'index -1
typedef int reg_id;

struct VirtStackElem{
    int depth;
    VariableItem* var;
    VirtStackElem* next;
};

class VirtStack{
private:
    VirtStackElem* VirtualStackBuffer;
public:
    VirtStack(void);
    ~VirtStack(void);

    void pushToStack(VariableItem* bVar);
    VariableItem* pop();

    void clearStack();

    int getStackPos(char* varName);
    void display();
};


#endif
