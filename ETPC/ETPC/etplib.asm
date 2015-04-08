

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
