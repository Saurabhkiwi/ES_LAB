	AREA RESET, DATA, READONLY
	EXPORT __Vectors
__Vectors
    DCD 0x40001000        ; initial stack pointer
    DCD Reset_Handler     ; reset vector
	ALIGN
	AREA mycode, CODE, READONLY
ENTRY
	EXPORT Reset_Handler
Reset_Handler
	MOV r4, #0            
    MOV r1, #10           ; number of elements
    LDR r0, =list
    LDR r2, =result

copy_loop
    LDR r3, [r0, r4]
    STR r3, [r2, r4]
    ADD r4, #4
    SUBS r1, #1
    BNE copy_loop

    LDR r0, =result
    MOV r3, #9            ; outer loop counter = n-1 passes

outer_loop
    MOV r4, r3            ; number of comparisons in this pass
    MOV r5, r0            ; pointer to start of array

inner_loop
    LDR r6, [r5], #4      ; load result[i] and increment pointer
    LDR r7, [r5]          ; load result[i+1]
    CMP r7, r6
    STRLS r6, [r5]        ; swap if needed
    STRLS r7, [r5, #-4]
    SUBS r4, #1
    BNE inner_loop

    SUBS r3, #1           ; decrement outer loop counter
    BNE outer_loop
STOP B STOP

list    DCD 0x10,0x05,0x33,0x24,0x56,0x77,0x21,0x04,0x87,0x01
	AREA mydata, DATA, READWRITE
result  DCD 0,0,0,0,0,0,0,0,0,0
	END


    