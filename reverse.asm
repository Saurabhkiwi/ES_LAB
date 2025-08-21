        AREA  RESET, DATA, READONLY
        EXPORT __Vectors
__Vectors
        DCD 0x10001000          
        DCD Reset_Handler       
        ALIGN

        AREA mycode, CODE, READONLY
        ENTRY
        EXPORT Reset_Handler

Reset_Handler
        MOV     R2, #5             
        LDR     R0, =SRC            
        LDR     R1, =SRC+36  
		

BACK
		LDR 	R3, [R0]
		LDR 	R4, [R1]
		STR     R3, [R1] , #-4
		STR     R4, [R0] , #4        
        SUBS    R2,#1          
        BNE     BACK

STOP
        B       STOP

        AREA mydata, DATA, READWRITE

SRC      DCD     0x1234, 0xA13652, 0xF2, 0x56789ABC, 0x0FEDCBA9, 0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x55555555
	    

        END
