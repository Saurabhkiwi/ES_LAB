AREA RESET, DATA, READONLY
EXPORT __Vectors
__Vectors
    DCD 0x10001000       
    DCD Reset_Handler      
ALIGN

AREA mycode, CODE, READONLY
ENTRY
EXPORT Reset_Handler
Reset_Handler

    ;--- Initialize ---
    LDR r0, =list        
    LDR r1, =result      
    MOV r2, #10            
    MOV r3, #0             

copy_loop
    LDR r4, [r0, r3, LSL #2] 
    STR r4, [r1, r3, LSL #2] 
    ADD r3, #1
    CMP r3, r2
    BLT copy_loop           ; copy all elements

    ;--- Selection Sort ---
    MOV r3, #0              ; i = 0

outer_loop
    CMP r3, r2
    BGE sort_done           ; if i >= n, done

    MOV r4, r3              ; min_index = i
    ADD r5, r3, #1          ; j = i + 1

inner_loop
    CMP r5, r2
    BGE swap_min            ; if j >= n, swap min

    LDR r6, [r1, r4, LSL #2] ; result[min_index]
    LDR r7, [r1, r5, LSL #2] ; result[j]
    CMP r7, r6
    BGE no_update
    MOV r4, r5              ; update min_index = j

no_update
    ADD r5, #1
    B inner_loop

swap_min
    CMP r4, r3
    BEQ skip_swap           ; if min_index == i, no swap

    LDR r6, [r1, r3, LSL #2] ; result[i]
    LDR r7, [r1, r4, LSL #2] ; result[min_index]
    STR r7, [r1, r3, LSL #2] ; swap
    STR r6, [r1, r4, LSL #2]

skip_swap
    ADD r3, #1
    B outer_loop

sort_done
STOP  B STOP                    

list    DCD 0x10,0x05,0x33,0x24,0x56,0x77,0x21,0x04,0x87,0x01
AREA data1, DATA, READWRITE
result  DCD 0,0,0,0,0,0,0,0,0,0

END
