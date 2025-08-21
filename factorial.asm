        AREA RESET, DATA, READONLY
        EXPORT __Vectors
__Vectors
        DCD 0x10001000        ; initial SP
        DCD Reset_Handler     ; reset vector
        ALIGN

        AREA mycode, CODE, READONLY
        ENTRY
        EXPORT Reset_Handler

Reset_Handler
        LDR R1, =5            ; input number n = 5
        BL fact               ; call factorial
        LDR R12, =0x10001000  ; memory location to store result
        STR R0, [R12]         ; store factorial result
STOP    B STOP                ; infinite loop (end)
fact
        CMP R1, #1            ; if (n <= 1)
        BLE base_case         ; branch to base case

        PUSH {R1, LR}         ; save n and return address
        SUB R1, R1, #1        ; n-1
        BL fact               ; recursive call fact(n-1)

        POP {R2, LR}          ; restore original n into R2
        MUL R0, R0, R2        ; R0 = (fact(n-1)) * n
        BX LR                 ; return

base_case
        MOV R0, #1            ; return 1
        BX LR
