.code

; extern bool vmexit_handler(vcpu* vcpu);
extern vmexit_handler  : proc

; extern void WHATS_A_GOOD_NAME(vcpu* _vcpu);
WHATS_A_GOOD_NAME PROC
    sub rcx, 8
	mov rsp, rcx; set RSP to guest_stack_frame 
	; stack grows downwards so our shit will populate the stack frame
	mov rax, [rcx] ; pa of vmcb is at rcx
	
vmrun_loop:
	int 3
	vmload rax
	vmrun rax
	vmsave rax

	; uint64_t rcx;
	; uint64_t rdx;
	; uint64_t rbx;
	; uint64_t rsi;
	; uint64_t rdi;
	; uint64_t r8;
	; uint64_t r9;
	; uint64_t r10;
	; uint64_t r11;
	; uint64_t r12;
	; uint64_t r13;
	; uint64_t r14;
	; uint64_t r15;

	;	-->	stack_frame guest_stack_frame;
	mov [rsp], rcx
	mov [rsp-8], rdx
	mov [rsp-10h], rbx
	mov [rsp-18h], rsi
	mov [rsp-20h], rdi
	mov [rsp-28h], r8
	mov [rsp-30h], r9
	mov [rsp-38h], r10
	mov [rsp-40h], r11
	mov [rsp-48h], r12
	mov [rsp-50h], r13
	mov [rsp-58h], r14
	mov [rsp-60h], r15

	; M128A xmm0;
	; M128A xmm1;
	; M128A xmm2;
	; M128A xmm3;
	; M128A xmm4;
	; M128A xmm5;
	movaps xmmword ptr [rsp-70h], xmm0
	movaps xmmword ptr [rsp-80h], xmm1
	movaps xmmword ptr [rsp-90h], xmm2
	movaps xmmword ptr [rsp-0A0h], xmm3
	movaps xmmword ptr [rsp-0B0h], xmm4
	movaps xmmword ptr [rsp-0C0h], xmm5


	; call vmexit_handler(vcpu* vcpu); 
	mov rcx, rsp + 8

	sub rsp, 0D0h; sizeof(stack_frame)

	call vmexit_handler

	test rax, rax

	;reset registers

	jnz vmrun_loop ; if returns 1 (non-zero) loop

devirtualize:
	


WHATS_A_GOOD_NAME ENDP

end