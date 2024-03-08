.CODE

; void __sgdt(void* gdtr);
__sgdt PROC
	sgdt [ecx]
	ret
__sgdt ENDP

END