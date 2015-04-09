;Made with ETP
	include "os.h"
	xdef _main
	xdef _nostub
	xdef _ti89
	xdef _ti92plus
	

_main:
	SUB.L	#8,A7
	MOVE.W	#12,D1
	MOVE.W	D1,4(A7)
	MOVE.W	#3,D1
	MOVE.W	D1,2(A7)
	MOVE.W	#32,D1
	MULS.W	#5,D1
	MOVE.W	4(A7),D2
	DIVS.W	2(A7),D2
	ADD.W	D2,D1
	MOVE.W	D1,6(A7)
	MOVE.W	#1,D1
	MOVE.W	D1,(A7)
	;Creation de temporaire
	SUB.L	#2,A7
	MOVE.W	#5,D1
	MOVE.W	D1,(A7)
.sysetiq0:
	MOVE.W	2(A7),D1
	CMP.W	(A7),D1
	BGT	.sysetiq1
	BSR	clearscreen
	MOVE.W	2(A7),D1
	MULS.W	8(A7),D1
	MOVE.W	D1,-(A7)
	BSR	dispint
	ADD.L	#2,A7
	BSR	afficheuntruc
	BSR	waitkey
	ADD.W	#1,2(A7)
	BRA	.sysetiq0
.sysetiq1:
	;Liberation de temporaire
	ADD.L	#2,A7
	ADD.L	#8,A7
	RTS	


clearscreen:
    MOVE.L $C8,a0
    MOVE.L ClrScr*4(a0),a0
    JSR (a0)
    RTS

dispint:
    MOVE.W 4(a7),d0
    LEA -30(a7),a7
    MOVE.L a7,a1
    PEA.L (a1)
    MOVE.W d0,-(a7)
    PEA.L dispintformat(pc)
    PEA.L (a1)
    MOVE.L $C8,a0
    MOVE.L sprintf*4(a0),a0
    JSR (a0)
    LEA 10(a7),a7
    MOVE.L (a7)+,a1
    MOVE.W #1,-(a7)
    PEA.L (a1)
    MOVE.W #5,-(a7)
    MOVE.W #40,-(a7)
    MOVE.L $C8,a0
    MOVE.L DrawStr*4(a0),a0
    JSR (a0)
    LEA 10(a7),a7
    LEA 30(a7),a7
    RTS


afficheuntruc:
    MOVE.L $C8,a5
    MOVE.W #1,-(a7)
    PEA.L texte(pc)
    MOVE.W #25,-(a7)
    MOVE.W #40,-(a7)
    MOVE.L DrawStr*4(a5),a0
    JSR (a0)
    LEA 10(a7),a7
    RTS

waitkey:
    MOVE.L $C8,a0
    MOVE.L ngetchx*4(a0),a0
    JSR (a0)
    RTS


dispintformat: dc.b "val = %i",0
texte: dc.b "Qq chose!",0
