[BITS 32]
global context_switch

context_switch:
    push ebp
    push ebx
    push esi
    push edi

    mov ecx, [esp + 20]
    test ecx, ecx
    jz .load_new
    mov [ecx], esp

.load_new:
    mov edx, [esp + 24]
    mov esp, edx

    pop edi
    pop esi
    pop ebx
    pop ebp
    ret
