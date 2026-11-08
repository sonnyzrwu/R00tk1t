.code

; /*
;
; void disableWP_CR0();
;
; */
disableWP_CR0 PROC
	; This function will disable write protection in the CR0 register, allowing kernel memory to be modified.
	push rbx
	mov rbx, cr0
	and rbx, 0FFFEFFFFh ; Clear the WP bit (bit 16) in CR0
	mov cr0, rbx
	pop rbx
	ret
disableWP_CR0 ENDP

; /*
;
; void enableWP_CR0();
;
; */

enableWP_CR0 PROC
	; This function will enable write protection in the CR0 register, allowing kernel memory to be write protected.
	push rbx
	mov rbx, cr0
	or rbx, 000010000h
	mov cr0, rbx
	pop rbx
	ret
enableWP_CR0 ENDP
END
