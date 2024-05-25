.code

; uint64_t __vmmcall(hypercall_t& request)
?__vmmcall@@YA_KAEAUhypercall_t@@@Z proc
	mov r8, [rcx+8]
	mov r9, [rcx+10h]
	mov r10, [rcx+18h]
	mov r11, [rcx+20h]
	mov rcx, [rcx]
	vmmcall
	ret
?__vmmcall@@YA_KAEAUhypercall_t@@@Z endp

end