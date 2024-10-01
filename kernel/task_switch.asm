global switch_task

switch_task:
    ; Save current task's state
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15

    ; Save current RSP
    mov [rdi], rsp

    ; Load new task's RSP
    mov rsp, rsi

    ; Restore new task's state
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx

    ; Return to new task
    ret
