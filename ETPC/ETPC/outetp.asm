;Made with ETP
	include "os.h"
	xdef _main
	xdef _nostub
	xdef _ti89
	xdef _ti92plus
	

_main:
	SUB.L	#4,A7
	MOVE.W	#15,D1
	MOVE.W	D1,2(A7)
	MOVE.W	#0,D1
	MOVE.W	D1,(A7)
	BSR	clearscreen
	MOVE.W	#1337,D1
	MOVE.W	D1,-(A7)
	MOVE.W	#5,D1
	MOVE.W	D1,-(A7)
	MOVE.W	#5,D1
	MOVE.W	D1,-(A7)
	BSR	dispintxy
	ADD.L	#6,A7
	BRA	.sysetiq3
.sysetiq0:
	MOVE.W	#666,D1
	MOVE.W	D1,-(A7)
	MOVE.W	#5,D1
	MOVE.W	D1,-(A7)
	MOVE.W	#5,D1
	MOVE.W	D1,-(A7)
	BSR	dispintxy
	ADD.L	#6,A7
.sysetiq3:
	BSR	waitkey
	ADD.L	#4,A7
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

dispintxy:
    MOVE.W 8(a7),d0
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
    MOVE.W 36(a7),d1
    MOVE.W 34(a7),d2
    MOVE.W #1,-(a7)
    PEA.L (a1)
    MOVE.W d1,-(a7)
    MOVE.W d2,-(a7)
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
