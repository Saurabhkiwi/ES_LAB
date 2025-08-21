        AREA RESET, DATA, READONLY
        EXPORT __Vectors

__Vectors
        DCD 0x10001000          ; stack pointer
        DCD Reset_Handler       ; reset vector
        ALIGN

        AREA MYCODE, CODE, READONLY
ENTRY
        EXPORT Reset_Handler

Reset_Handler
        MOV R5,#10              
        LDR R0,=SRC             
        LDR R2,=target         
        LDR R7,[R2]            
        LDR R3,=result       
        MOV R4,#0               
        STR R4,[R3]             

loop
        LDR R1,[R0],#4          
        CMP R1,R7              
        BEQ found             

        SUBS R5,R5,#1           ; decrement counter
        BNE loop                ; if not finished, continue
        B stop                

found
        MOV R4,#1               ; found = 1
        STR R4,[R3]             
        B stop

stop
        B stop                

  
SRC     DCD 0xA,0xB,0xC,0xD,0xE,0xF,0x1,0x5,0x7,0x9
target  DCD 0x1                 
        AREA data1, DATA, READWRITE
result  DCD 0                   

        END
