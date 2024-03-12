.code

; extern void vmexit_handler(vcpu* vcpu);
extern vmexit_handler  : proc

; extern void WHATS_A_GOOD_NAME(uint64_t* guest_vmcb_pa);
WHATS_A_GOOD_NAME PROC
	mov rsp, rcx
	mov rax, [rsp]
	
	;		stack_frame guest_stack_frame;
	; -->	uint64_t guest_vmcb_pa; // host rsp
	;		uint64_t host_vmcb_pa;
	;		vcpu* self;
	;		struct shared* shared_data;
	
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
	sub rsp, 0C8h
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

	; uint128_t xmm0;
	; uint128_t xmm1;
	; uint128_t xmm2;
	; uint128_t xmm3;
	; uint128_t xmm4;
	; uint128_t xmm5;
	movaps xmmword ptr [rsp+68h], xmm0
	movaps xmmword ptr [rsp+78h], xmm1
	movaps xmmword ptr [rsp+88h], xmm2
	movaps xmmword ptr [rsp+98h], xmm3
	movaps xmmword ptr [rsp+0A8h], xmm4
	movaps xmmword ptr [rsp+0B8h], xmm5

	add rsp, 0C8h

	; 		stack_frame guest_stack_frame;
	; -->	uint64_t guest_vmcb_pa; // host rsp
	;		uint64_t host_vmcb_pa;
	;		vcpu* self;
	;		struct shared* shared_data;

	; call vmexit_handler(vcpu* vcpu); with self
	; rsp is at guest_stack_frame, sizeof(stack_frame) = 0C8h
	mov rcx, [rsp + 10h]
	call vmexit_handler

devirtualize:



WHATS_A_GOOD_NAME ENDP

end