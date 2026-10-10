.code

ReadGdtrAsm PROC
    sgdt tbyte ptr [rcx]
    ret
ReadGdtrAsm ENDP

END
