.code

; extern void WHATS_A_GOOD_NAME(uint64_t* guest_vmcb_pa);
WHATS_A_GOOD_NAME PROC
	;mov rsp, rcx
	;
	;
	;mov rax, [rsp]
	;
	;vmload rax
	;vmrun rax
	;vmsave rax
WHATS_A_GOOD_NAME ENDP

end