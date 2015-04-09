# Makefile pour Pingu by Onur et FpgForce
# 2005

# Compiler
CC = g++
# Link editor
LD = g++

# Compiler flags
CFLAGS = -c -O3 -std=c++11
# Object flags
OFLAGS = -o
# Includes
INC = -IGeneral -ISyntax -IVerif -IGenCode/68k -IGenCode/z80
# Link Editor flags
LDFLAGS = -o

# Objects list
OLIST = Obj/ColItem.o Obj/ColIterator.o Obj/collection.o Obj/Compiler.o Obj/errorC.o Obj/LectureFichier.o Obj/noeud.o Obj/TAG.o Obj/VarTypeType.o Obj/DimElemItem.o Obj/FonctionItem.o Obj/InstructionETPB.o Obj/Tokenizer.o Obj/TypeItem.o Obj/VariableItem.o Obj/verisyn.o Obj/verisem.o Obj/asm68kCoder.o Obj/Gen68k.o  Obj/PileRegTemp68k.o Obj/VirtStack68k.o Obj/Genz80.o Obj/VirtStackz80.o Obj/asmz80Coder.o
# Binary name
EXENAME = etpc


$(EXENAME): $(OLIST)
	$(LD) $(LDFLAGS) $(EXENAME) $(OLIST)

Obj/ColItem.o: General/ColItem.cpp General/ColItem.h
	$(CC) $(CFLAGS) General/ColItem.cpp $(INC) $(OFLAGS) Obj/ColItem.o

Obj/ColIterator.o: General/ColIterator.cpp General/ColIterator.h Obj/ColItem.o
	$(CC) $(CFLAGS) General/ColIterator.cpp $(INC) $(OFLAGS) Obj/ColIterator.o

Obj/collection.o: General/collection.cpp General/collection.h Obj/errorC.o Obj/TAG.o Obj/ColIterator.o
	$(CC) $(CFLAGS) General/collection.cpp $(INC) $(OFLAGS) Obj/collection.o

Obj/Compiler.o: General/Compiler.cpp General/Compiler.h Obj/verisyn.o Obj/Tokenizer.o Obj/verisem.o Obj/Gen68k.o Obj/Genz80.o
	$(CC) $(CFLAGS) General/Compiler.cpp $(INC) $(OFLAGS) Obj/Compiler.o

Obj/errorC.o: General/errorC.cpp General/errorC.h Obj/TAG.o Obj/ColItem.o
	$(CC) $(CFLAGS) General/errorC.cpp $(INC) $(OFLAGS) Obj/errorC.o

Obj/LectureFichier.o: General/LectureFichier.cpp General/LectureFichier.h Obj/Compiler.o
	$(CC) $(CFLAGS) General/LectureFichier.cpp $(INC) $(OFLAGS) Obj/LectureFichier.o

Obj/noeud.o: General/Noeud.cpp General/noeud.h Obj/TAG.o Obj/VarTypeType.o Obj/ColItem.o
	$(CC) $(CFLAGS) General/Noeud.cpp $(INC) $(OFLAGS) Obj/noeud.o

Obj/TAG.o: General/TAG.cpp General/TAG.h
	$(CC) $(CFLAGS) General/TAG.cpp $(INC) $(OFLAGS) Obj/TAG.o

Obj/VarTypeType.o: General/VarTypeType.cpp General/VarTypeType.h
	$(CC) $(CFLAGS) General/VarTypeType.cpp $(INC) $(OFLAGS) Obj/VarTypeType.o




Obj/DimElemItem.o: Syntax/DimElemItem.cpp Syntax/DimElemItem.h
	$(CC) $(CFLAGS) Syntax/DimElemItem.cpp $(INC) $(OFLAGS) Obj/DimElemItem.o

Obj/FonctionItem.o: Syntax/FonctionItem.cpp Syntax/FonctionItem.h Obj/asm68kCoder.o Obj/VarTypeType.o Obj/collection.o
	$(CC) $(CFLAGS) Syntax/FonctionItem.cpp $(INC) $(OFLAGS) Obj/FonctionItem.o


Obj/InstructionETPB.o: Syntax/InstructionETPB.cpp Syntax/InstructionETPB.h Obj/VariableItem.o Obj/noeud.o Obj/collection.o
	$(CC) $(CFLAGS) Syntax/InstructionETPB.cpp $(INC) $(OFLAGS) Obj/InstructionETPB.o


Obj/Tokenizer.o: Syntax/Tokenizer.cpp Syntax/Tokenizer.h Obj/TAG.o
	$(CC) $(CFLAGS) Syntax/Tokenizer.cpp $(INC) $(OFLAGS) Obj/Tokenizer.o

Obj/TypeItem.o: Syntax/TypeItem.cpp Syntax/TypeItem.h Obj/TAG.o
	$(CC) $(CFLAGS) Syntax/TypeItem.cpp $(INC) $(OFLAGS) Obj/TypeItem.o

Obj/VariableItem.o: Syntax/VariableItem.cpp Syntax/VariableItem.h Obj/TAG.o Obj/VarTypeType.o Obj/collection.o
	$(CC) $(CFLAGS) Syntax/VariableItem.cpp $(INC) $(OFLAGS) Obj/VariableItem.o

Obj/verisyn.o: Syntax/VariableItem.cpp Syntax/verisyn.h Obj/InstructionETPB.o Obj/VariableItem.o Obj/DimElemItem.o Obj/TypeItem.o Obj/FonctionItem.o
	$(CC) $(CFLAGS) Syntax/verisyn.cpp $(INC) $(OFLAGS) Obj/verisyn.o

Obj/verisem.o: Verif/verisem.cpp Verif/verisem.h Obj/TypeItem.o Obj/VariableItem.o Obj/collection.o Obj/FonctionItem.o Obj/DimElemItem.o Obj/InstructionETPB.o Obj/Tokenizer.o
	$(CC) $(CFLAGS) Verif/verisem.cpp $(INC) $(OFLAGS) Obj/verisem.o

Obj/asm68kCoder.o: GenCode/68k/asm68kCoder.cpp GenCode/68k/asm68kCoder.h Obj/VirtStack68k.o Obj/noeud.o
	$(CC) $(CFLAGS) GenCode/68k/asm68kCoder.cpp $(INC) $(OFLAGS) Obj/asm68kCoder.o

Obj/Gen68k.o: GenCode/68k/Gen68k.cpp GenCode/68k/Gen68k.h Obj/asm68kCoder.o Obj/FonctionItem.o Obj/InstructionETPB.o Obj/PileRegTemp68k.o
	$(CC) $(CFLAGS) GenCode/68k/Gen68k.cpp $(INC) $(OFLAGS) Obj/Gen68k.o

Obj/PileRegTemp68k.o: GenCode/68k/PileRegTemp68k.cpp GenCode/68k/PileRegTemp68k.h Obj/asm68kCoder.o
	$(CC) $(CFLAGS) GenCode/68k/PileRegTemp68k.cpp $(INC) $(OFLAGS) Obj/PileRegTemp68k.o

Obj/VirtStack68k.o: GenCode/68k/VirtStack68k.cpp GenCode/68k/VirtStack68k.h Obj/VariableItem.o
	$(CC) $(CFLAGS) GenCode/68k/VirtStack68k.cpp $(INC) $(OFLAGS) Obj/VirtStack68k.o






Obj/Genz80.o: GenCode/z80/Genz80.cpp GenCode/z80/Genz80.h Obj/asmz80Coder.o Obj/FonctionItem.o Obj/InstructionETPB.o
	$(CC) $(CFLAGS) GenCode/z80/Genz80.cpp $(INC) $(OFLAGS) Obj/Genz80.o


Obj/VirtStackz80.o: GenCode/z80/VirtStackz80.cpp GenCode/z80/VirtStackz80.h Obj/VariableItem.o
	$(CC) $(CFLAGS) GenCode/z80/VirtStackz80.cpp $(INC) $(OFLAGS) Obj/VirtStackz80.o

Obj/asmz80Coder.o: GenCode/z80/asmz80Coder.cpp GenCode/z80/asmz80Coder.h Obj/VirtStackz80.o Obj/noeud.o
	$(CC) $(CFLAGS) GenCode/z80/asmz80Coder.cpp $(INC) $(OFLAGS) Obj/asmz80Coder.o



clean:
	\rm Obj/*
