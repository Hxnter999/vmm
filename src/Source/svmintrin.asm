.CODE

; extern void testing_vmrun(uint64_t vmcb_pa);
testing_vmrun PROC
	;push r14
	;mov rax, rcx
	;vmrun rax 
	;pop r14
	;ret
testing_vmrun ENDP

; extern void testcall();
testcall PROC
	vmmcall
	ret
testcall ENDP

END