.code

__read_cs proc
	mov ax, cs
	ret
__read_cs endp

__read_ss proc
	mov ax, ss
	ret
__read_ss endp

__read_ds proc
	mov ax, ds
	ret
__read_ds endp

__read_es proc
	mov ax, es
	ret
__read_es endp

__read_fs proc
	mov ax, fs
	ret
__read_fs endp

__read_gs proc
	mov ax, gs
	ret
__read_gs endp

__read_tr proc
	str ax
	ret
__read_tr endp

__read_ldtr proc
	sldt ax
	ret
__read_ldtr endp

__write_ds proc
    mov ds, cx
	ret
__write_ds endp

__write_es proc
	mov es, cx
	ret
__write_es endp

__write_fs proc
	mov fs, cx
	ret
__write_fs endp

__write_gs proc
	mov gs, cx
	ret
__write_gs endp

__write_tr proc
	ltr cx
	ret
__write_tr endp

__write_ldtr proc
	lldt cx
	ret
__write_ldtr endp

end