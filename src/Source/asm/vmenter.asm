.code

; extern bool vmexit_handler(vcpu* vcpu);
extern vmexit_handler  : proc

; extern void vmenter(uint64_t* guest_vmcb_pa);
vmenter PROC
	mov [rcx+10h], rsp ; preserve stack pointer
	mov rsp, rcx
	
vmrun_loop:
	mov rax, [rsp]
	vmload rax
	vmrun rax
	vmsave rax
	
	; rsp -> guest_vmcb_pa
	; every push gets it closer to stack_contents

	; cant push xmm directly so we simulate a push by subtracting and manually moving
	sub rsp, 60h

	movaps xmmword ptr [rsp], xmm0
	movaps xmmword ptr [rsp+10h], xmm1
	movaps xmmword ptr [rsp+20h], xmm2
	movaps xmmword ptr [rsp+30h], xmm3
	movaps xmmword ptr [rsp+40h], xmm4
	movaps xmmword ptr [rsp+50h], xmm5

	; rsp -> xmm0
	push r15
	push r14
	push r13
	push r12
	push r11
	push r10
	push r9
	push r8
	push rdi
	push rsi
	push rbx
	push rdx
	push rcx
	push rax

	; rsp -> stack_contents
	mov rcx, [rsp + 0D8h] ; sizeof(stack_contents) + sizeof(uint64_t)
	call vmexit_handler
	test al, al

	pop rax
	pop rcx
	pop rdx
	pop rbx
	pop rsi
	pop rdi
	pop r8
	pop r9
	pop r10
	pop r11
	pop r12
	pop r13
	pop r14
	pop r15

	; rsp -> xmm0
	movaps xmm0, xmmword ptr [rsp]
	movaps xmm1, xmmword ptr [rsp+10h]
	movaps xmm2, xmmword ptr [rsp+20h]
	movaps xmm3, xmmword ptr [rsp+30h]
	movaps xmm4, xmmword ptr [rsp+40h]
	movaps xmm5, xmmword ptr [rsp+50h]
	add rsp, 60h

	; rsp -> guest_vmcb_pa

	jnz vmrun_loop

devirtualize:
	ret

vmenter ENDP

end