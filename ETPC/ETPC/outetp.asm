;Made with ETP
	include "os.h"
	xdef _main
	xdef _nostub
	xdef _ti89
	xdef _ti92plus
	

_main:
	SUB.L	#10,A7
	MOVE.W	#32,D1
	MULS.W	#5,D1
	MOVE.W	6(A7),D2
	DIVS.W	4(A7),D2
	ADD.W	D2,D1
	MOVE.W	D1,8(A7)
	ADD.L	#10,A7
	RTS	
