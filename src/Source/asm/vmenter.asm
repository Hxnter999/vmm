.code

testcall proc
	vmmcall
	ret
testcall endp

; extern bool vmexit_handler(vcpu* vcpu);
extern vmexit_handler  : proc

; extern void vmenter(uint64_t* guest_vmcb_pa);
vmenter PROC
	mov rsp, rcx
	sub rsp, 100h ; needed at first cause the loop always adds 100h from the start

vmrun_loop:
    add rsp, 100h ; have to do it after the conditional jump since it will overwrite the zero flag

	; rsp -> guest_vmcb_pa
	mov rax, [rsp]
	vmload rax
	vmrun rax
	;int 3
	vmsave rax
	
	; rsp -> guest_vmcb_pa
	; every push gets it closer to stack_contents

	; cant push xmm directly so we simulate a push by subtracting and manually moving
	; rsp -> xmm0
	sub rsp, 100h

	movaps xmmword ptr [rsp], xmm0
	movaps xmmword ptr [rsp+10h], xmm1
	movaps xmmword ptr [rsp+20h], xmm2
	movaps xmmword ptr [rsp+30h], xmm3
	movaps xmmword ptr [rsp+40h], xmm4
	movaps xmmword ptr [rsp+50h], xmm5
	movaps xmmword ptr [rsp+60h], xmm6
	movaps xmmword ptr [rsp+70h], xmm7
	movaps xmmword ptr [rsp+80h], xmm8
	movaps xmmword ptr [rsp+90h], xmm9
	movaps xmmword ptr [rsp+0A0h], xmm10
	movaps xmmword ptr [rsp+0B0h], xmm11
	movaps xmmword ptr [rsp+0C0h], xmm12
	movaps xmmword ptr [rsp+0D0h], xmm13
	movaps xmmword ptr [rsp+0E0h], xmm14
	movaps xmmword ptr [rsp+0F0h], xmm15

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


    ; rdtsc
    ; shl rdx, 32
    ; or rdx, rax

	; rsp -> stack_frame
	mov rcx, [rsp + 180h] ; sizeof(stack_contents) + sizeof(uint64_t) * 2
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
	movaps xmm6, xmmword ptr [rsp+60h]
	movaps xmm7, xmmword ptr [rsp+70h]
	movaps xmm8, xmmword ptr [rsp+80h]
	movaps xmm9, xmmword ptr [rsp+90h]
	movaps xmm10, xmmword ptr [rsp+0A0h]
	movaps xmm11, xmmword ptr [rsp+0B0h]
	movaps xmm12, xmmword ptr [rsp+0C0h]
	movaps xmm13, xmmword ptr [rsp+0D0h]
	movaps xmm14, xmmword ptr [rsp+0E0h]
	movaps xmm15, xmmword ptr [rsp+0F0h]

	jnz vmrun_loop

devirtualize:
	; rsp -> guest_rip
	add rsp, 118h ; 100h for xmm + sizeof(uint64_t) * 3

	mov rax, [rsp]

	; rsp + 8 -> guest_rsp
	mov rsp, [rsp+8]
	
	jmp rax

vmenter ENDP

end