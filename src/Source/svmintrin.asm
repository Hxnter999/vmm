.code

; extern bool vmexit_handler(vcpu* vcpu);
extern vmexit_handler  : proc

; extern void testcall();
testcall proc
	vmmcall
	ret
testcall endp

; extern void WHATS_A_GOOD_NAME(vcpu* _vcpu);
WHATS_A_GOOD_NAME PROC
	; stack grows downwards so our shit will populate the stack frame
	mov rax, [rcx] ; pa of vmcb is at rcx
	add rcx, 10h
	mov rsp, rcx; set RSP to guest_stack_frame 
	
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

	;	--> guest_stack_frame;
	mov [rsp], rcx
	mov [rsp+8], rdx
	mov [rsp+10h], rbx
	mov [rsp+18h], rsi
	mov [rsp+20h], rdi
	mov [rsp+28h], r8
	mov [rsp+30h], r9
	mov [rsp+38h], r10
	mov [rsp+40h], r11
	mov [rsp+48h], r12
	mov [rsp+50h], r13
	mov [rsp+58h], r14
	mov [rsp+60h], r15

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
	sub rsp, 10h ; --> guest_vmcb_pa
	mov rcx, rsp
	call vmexit_handler
	add rsp, 10h ; --> guest_stack_frame

	;reset registers

	mov rcx, [rsp]
	mov rdx, [rsp+8]
	mov rbx, [rsp+10h]
	mov rsi, [rsp+18h]
	mov rdi, [rsp+20h]
	mov r8,  [rsp+28h]
	mov r9,  [rsp+30h]
	mov r10, [rsp+38h]
	mov r11, [rsp+40h]
	mov r12, [rsp+48h]
	mov r13, [rsp+50h]
	mov r14, [rsp+58h]
	mov r15, [rsp+60h]

	movaps xmm0, xmmword ptr [rsp-70h]
	movaps xmm1, xmmword ptr [rsp-80h]
	movaps xmm2, xmmword ptr [rsp-90h]
	movaps xmm3, xmmword ptr [rsp-0A0h]
	movaps xmm4, xmmword ptr [rsp-0B0h]
	movaps xmm5, xmmword ptr [rsp-0C0h]

	; check return address
	test al, al

	jnz vmrun_loop ; if returns 1 (non-zero) loop

devirtualize:
	


	ret

WHATS_A_GOOD_NAME ENDP

end