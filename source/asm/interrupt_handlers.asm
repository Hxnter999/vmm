PUSH_GPRS macro reg:req
	mov guest_context.$rax[reg], rax
	mov guest_context.$rcx[reg], rcx
	mov guest_context.$rdx[reg], rdx
	mov guest_context.$rbx[reg], rbx
	mov guest_context.$rbp[reg], rbp
	mov guest_context.$rsi[reg], rsi
	mov guest_context.$rdi[reg], rdi
	mov guest_context.$r8 [reg],  r8
	mov guest_context.$r9 [reg],  r9
	mov guest_context.$r10[reg], r10
	mov guest_context.$r11[reg], r11
	mov guest_context.$r12[reg], r12
	mov guest_context.$r13[reg], r13
	mov guest_context.$r14[reg], r14
	mov guest_context.$r15[reg], r15
endm

POP_GPRS macro reg:req
	mov rax, guest_context.$rax[reg]
	mov rcx, guest_context.$rcx[reg]
	mov rdx, guest_context.$rdx[reg]
	mov rbx, guest_context.$rbx[reg]
	mov rbp, guest_context.$rbp[reg]
	mov rsi, guest_context.$rsi[reg]
	mov rdi, guest_context.$rdi[reg]
	mov r8,  guest_context.$r8[reg]
	mov r9,  guest_context.$r9[reg]
	mov r10, guest_context.$r10[reg]
	mov r11, guest_context.$r11[reg]
	mov r12, guest_context.$r12[reg]
	mov r13, guest_context.$r13[reg]
	mov r14, guest_context.$r14[reg]
	mov r15, guest_context.$r15[reg]
endm

.code

extern shared_exception_handler : proc


INTERRUPT_HANDLER macro InterruptNumber:req, InterruptName:req
InterruptName proc
        push    0
        push    InterruptNumber
        jmp near ptr asm_shared_exception_handler ; force a near jump
InterruptName endp
endm

INTERRUPT_HANDLER_WITH_CODE macro InterruptNumber:req, InterruptName:req
InterruptName proc
        nop
        nop
        push    InterruptNumber
        jmp near ptr asm_shared_exception_handler
InterruptName endp
endm

asm_shared_exception_handler proc
	sub rsp, 78h

	PUSH_GPRS rsp

	mov rcx, rsp

	sub rsp, 20h
	call shared_exception_handler
	add rsp, 20h

	POP_GPRS rsp

	add rsp, 78h

	add rsp, 10h ; "pop" the interrupt number and the errorcode
	iretq
asm_shared_exception_handler endp

INTERRUPT_HANDLER 0, interrupt_handler_0
INTERRUPT_HANDLER 1, interrupt_handler_1
INTERRUPT_HANDLER 2, interrupt_handler_2
INTERRUPT_HANDLER 3, interrupt_handler_3
INTERRUPT_HANDLER 4, interrupt_handler_4
INTERRUPT_HANDLER 5, interrupt_handler_5
INTERRUPT_HANDLER 6, interrupt_handler_6
INTERRUPT_HANDLER 7, interrupt_handler_7

INTERRUPT_HANDLER_WITH_CODE 8, interrupt_handler_8
INTERRUPT_HANDLER_WITH_CODE 10, interrupt_handler_10
INTERRUPT_HANDLER_WITH_CODE 11, interrupt_handler_11
INTERRUPT_HANDLER_WITH_CODE 12, interrupt_handler_12
INTERRUPT_HANDLER_WITH_CODE 13, interrupt_handler_13
INTERRUPT_HANDLER_WITH_CODE 14, interrupt_handler_14

INTERRUPT_HANDLER 16, interrupt_handler_16

INTERRUPT_HANDLER_WITH_CODE 17, interrupt_handler_17

INTERRUPT_HANDLER 18, interrupt_handler_18
INTERRUPT_HANDLER 19, interrupt_handler_19
INTERRUPT_HANDLER 20, interrupt_handler_20

INTERRUPT_HANDLER_WITH_CODE 30, interrupt_handler_30

end