.CODE

; just a test so i can properly print the exit codes after vmexit, r14 is what holds the vcpu* so i just restore it after
; extern void testing_vmrun(uint64_t vmcb_pa);
testing_vmrun PROC
	push r14
	mov rax, rcx
	vmrun rax 
	pop r14
	ret
testing_vmrun ENDP

END