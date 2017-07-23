# Makefile pour Pingu by Onur et FpgForce
# 2005 (->2017 ;))

# compiler
CC = g++
# Link editor
LD = g++

# compiler flags
CFLAGS = -c -O3 -std=c++11 --coverage
# Object flags
OFLAGS = -o
# Includes
INC = -IGeneral -ISyntax -IVerif -IGenCode/68k -IGenCode/z80
# Link Editor flags
LDFLAGS = --coverage -o

# Objects list
OLIST = Obj/ColItem.o Obj/ColIterator.o Obj/collection.o Obj/compiler.o Obj/errorC.o Obj/filereader.o Obj/astnode.o Obj/TAG.o Obj/VarTypeType.o Obj/DimElemItem.o Obj/functionitem.o Obj/InstructionETPB.o Obj/tokenizer.o Obj/TypeItem.o Obj/VariableItem.o Obj/verisyn.o Obj/verisem.o Obj/asm68kCoder.o Obj/Gen68k.o  Obj/tempRegStack68k.o Obj/VirtStack68k.o Obj/Genz80.o Obj/VirtStackz80.o Obj/asmz80Coder.o
# Binary name
EXENAME = etpc


$(EXENAME): $(OLIST)
	$(LD) $(LDFLAGS) $(EXENAME) $(OLIST)

Obj/ColItem.o: General/ColItem.cpp General/colitem.h
	$(CC) $(CFLAGS) General/ColItem.cpp $(INC) $(OFLAGS) Obj/ColItem.o

Obj/ColIterator.o: General/ColIterator.cpp General/ColIterator.h Obj/ColItem.o
	$(CC) $(CFLAGS) General/ColIterator.cpp $(INC) $(OFLAGS) Obj/ColIterator.o

Obj/collection.o: General/collection.cpp General/collection.h Obj/errorC.o Obj/TAG.o Obj/ColIterator.o
	$(CC) $(CFLAGS) General/collection.cpp $(INC) $(OFLAGS) Obj/collection.o

Obj/compiler.o: General/compiler.cpp General/compiler.h Obj/verisyn.o Obj/tokenizer.o Obj/verisem.o Obj/Gen68k.o Obj/Genz80.o
	$(CC) $(CFLAGS) General/compiler.cpp $(INC) $(OFLAGS) Obj/compiler.o

Obj/errorC.o: General/errorC.cpp General/errorC.h Obj/TAG.o Obj/ColItem.o
	$(CC) $(CFLAGS) General/errorC.cpp $(INC) $(OFLAGS) Obj/errorC.o

Obj/filereader.o: General/filereader.cpp General/filereader.h Obj/compiler.o
	$(CC) $(CFLAGS) General/filereader.cpp $(INC) $(OFLAGS) Obj/filereader.o

Obj/astnode.o: General/astnode.cpp General/astnode.h Obj/TAG.o Obj/VarTypeType.o Obj/ColItem.o
	$(CC) $(CFLAGS) General/astnode.cpp $(INC) $(OFLAGS) Obj/astnode.o

Obj/TAG.o: General/TAG.cpp General/TAG.h
	$(CC) $(CFLAGS) General/TAG.cpp $(INC) $(OFLAGS) Obj/TAG.o

Obj/VarTypeType.o: General/VarTypeType.cpp General/VarTypeType.h
	$(CC) $(CFLAGS) General/VarTypeType.cpp $(INC) $(OFLAGS) Obj/VarTypeType.o




Obj/DimElemItem.o: Syntax/DimElemItem.cpp Syntax/DimElemItem.h
	$(CC) $(CFLAGS) Syntax/DimElemItem.cpp $(INC) $(OFLAGS) Obj/DimElemItem.o

Obj/functionitem.o: Syntax/functionitem.cpp Syntax/functionitem.h Obj/asm68kCoder.o Obj/VarTypeType.o Obj/collection.o
	$(CC) $(CFLAGS) Syntax/functionitem.cpp $(INC) $(OFLAGS) Obj/functionitem.o


Obj/InstructionETPB.o: Syntax/InstructionETPB.cpp Syntax/InstructionETPB.h Obj/VariableItem.o Obj/astnode.o Obj/collection.o
	$(CC) $(CFLAGS) Syntax/InstructionETPB.cpp $(INC) $(OFLAGS) Obj/InstructionETPB.o


Obj/tokenizer.o: Syntax/tokenizer.cpp Syntax/tokenizer.h Obj/TAG.o
	$(CC) $(CFLAGS) Syntax/tokenizer.cpp $(INC) $(OFLAGS) Obj/tokenizer.o

Obj/TypeItem.o: Syntax/TypeItem.cpp Syntax/TypeItem.h Obj/TAG.o
	$(CC) $(CFLAGS) Syntax/TypeItem.cpp $(INC) $(OFLAGS) Obj/TypeItem.o

Obj/VariableItem.o: Syntax/VariableItem.cpp Syntax/VariableItem.h Obj/TAG.o Obj/VarTypeType.o Obj/collection.o
	$(CC) $(CFLAGS) Syntax/VariableItem.cpp $(INC) $(OFLAGS) Obj/VariableItem.o

Obj/verisyn.o: Syntax/VariableItem.cpp Syntax/verisyn.h Obj/InstructionETPB.o Obj/VariableItem.o Obj/DimElemItem.o Obj/TypeItem.o Obj/functionitem.o
	$(CC) $(CFLAGS) Syntax/verisyn.cpp $(INC) $(OFLAGS) Obj/verisyn.o

Obj/verisem.o: Verif/verisem.cpp Verif/verisem.h Obj/TypeItem.o Obj/VariableItem.o Obj/collection.o Obj/functionitem.o Obj/DimElemItem.o Obj/InstructionETPB.o Obj/tokenizer.o
	$(CC) $(CFLAGS) Verif/verisem.cpp $(INC) $(OFLAGS) Obj/verisem.o

Obj/asm68kCoder.o: GenCode/68k/asm68kCoder.cpp GenCode/68k/asm68kCoder.h Obj/VirtStack68k.o Obj/astnode.o
	$(CC) $(CFLAGS) GenCode/68k/asm68kCoder.cpp $(INC) $(OFLAGS) Obj/asm68kCoder.o

Obj/Gen68k.o: GenCode/68k/Gen68k.cpp GenCode/68k/Gen68k.h Obj/asm68kCoder.o Obj/functionitem.o Obj/InstructionETPB.o Obj/tempRegStack68k.o
	$(CC) $(CFLAGS) GenCode/68k/Gen68k.cpp $(INC) $(OFLAGS) Obj/Gen68k.o

Obj/tempRegStack68k.o: GenCode/68k/tempRegStack68k.cpp GenCode/68k/tempRegStack68k.h Obj/asm68kCoder.o
	$(CC) $(CFLAGS) GenCode/68k/tempRegStack68k.cpp $(INC) $(OFLAGS) Obj/tempRegStack68k.o

Obj/VirtStack68k.o: GenCode/68k/VirtStack68k.cpp GenCode/68k/VirtStack68k.h Obj/VariableItem.o
	$(CC) $(CFLAGS) GenCode/68k/VirtStack68k.cpp $(INC) $(OFLAGS) Obj/VirtStack68k.o






Obj/Genz80.o: GenCode/z80/Genz80.cpp GenCode/z80/Genz80.h Obj/asmz80Coder.o Obj/functionitem.o Obj/InstructionETPB.o
	$(CC) $(CFLAGS) GenCode/z80/Genz80.cpp $(INC) $(OFLAGS) Obj/Genz80.o


Obj/VirtStackz80.o: GenCode/z80/VirtStackz80.cpp GenCode/z80/VirtStackz80.h Obj/VariableItem.o
	$(CC) $(CFLAGS) GenCode/z80/VirtStackz80.cpp $(INC) $(OFLAGS) Obj/VirtStackz80.o

Obj/asmz80Coder.o: GenCode/z80/asmz80Coder.cpp GenCode/z80/asmz80Coder.h Obj/VirtStackz80.o Obj/astnode.o
	$(CC) $(CFLAGS) GenCode/z80/asmz80Coder.cpp $(INC) $(OFLAGS) Obj/asmz80Coder.o



clean:
	\rm Obj/*
