.CODE

; extern void testcall();
testcall PROC
	vmmcall
	ret
testcall ENDP

END