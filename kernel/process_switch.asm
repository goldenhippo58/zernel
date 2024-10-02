global switch_to_process

switch_to_process:
    mov eax, [esp + 4]  ; Get new stack pointer
    mov esp, eax        ; Switch to new stack
    pop eax             ; Pop return address (entry point)
    jmp eax             ; Jump to the entry point
