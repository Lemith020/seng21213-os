[org 0x7c00]
[bits 16]

    ; ---- E820 Memory Map Detection (real mode) ----
    mov di, 0x8000          ; E820 map save wena memory address eka
    xor ebx, ebx
    xor bp, bp              ; entry count
    mov edx, 0x534D4150     ; "SMAP" signature

.e820_loop:
    mov eax, 0xE820
    mov ecx, 24
    int 0x15
    jc .e820_done           ; carry flag set nam, list eka iwarai

    cmp eax, 0x534D4150
    jne .e820_done

    inc bp
    add di, 24
    test ebx, ebx
    jz .e820_done
    jmp .e820_loop

.e820_done:
    mov [0x7C00 - 2], bp    ; entry count eka save karanawa

    ; --- Existing bootloader continuation ---
    cli
    ; (rest of your bootloader / protected mode switch code continues here)
