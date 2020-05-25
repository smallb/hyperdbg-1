PUBLIC AsmGeneralDetourHook
EXTERN ExAllocatePoolWithTagOrig:QWORD
EXTERN DebuggerEventHiddenHookGeneralDetourEventHandler:PROC

.code _text

;------------------------------------------------------------------------

AsmGeneralDetourHook PROC PUBLIC

SaveTheRegisters:
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8        
    push rdi
    push rsi
    push rbp
    push rbp	; rsp
    push rbx
    push rdx
    push rcx
    push rax	

	mov rcx, rsp		    ; Fast call argument to PGUEST_REGS
    mov rdx, [rsp +080h]    ; Fast call argument (second) - CalledFrom
    sub rdx, 5              ; as we used (call $ + 5) so we subtract it by 5 
	sub	rsp, 28h		; Free some space for Shadow Section

	call	DebuggerEventHiddenHookGeneralDetourEventHandler

	add	rsp, 28h		; Restore the state

    mov  [rsp +080h], rax ; the return address of the above function is where we should continue

RestoreTheRegisters:
	pop rax
    pop rcx
    pop rdx
    pop rbx
    pop rbp		; rsp
    pop rbp
    pop rsi
    pop rdi 
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15
    ret ; jump back to the trampoline
    
AsmGeneralDetourHook ENDP 

;------------------------------------------------------------------------

END                     
