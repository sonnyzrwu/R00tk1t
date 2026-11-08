.code

; /*
;
; VOID disableWP_CR0();
;
; */
disableWP_CR0 PROC
	; This function will disable write protection in the CR0 register, allowing kernel memory to be modified.
	push rbx
	mov rbx, cr0
	and rbx, 0FFFEFFFFh ; Clear the WP bit (bit 16) in CR0
	mov cr0, rbx
	pop rbx
disableWP_CR0 ENDP

END
