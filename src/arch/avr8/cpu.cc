// EPOS-- AVR8_CPU Implementation

#include <arch/avr8/cpu.h>

__BEGIN_SYS

void AVR8::Context::load() const volatile
{
        ASM(
	
	    // Discard Return Address
	    "   pop   r1		;  	\n" 
	    "   pop   r1		;  	\n"	
	
	    // Move "this" pointer to X
	    "   mov   r27,r25           ;       \n"  
            "   mov   r26,r24           ;       \n" 
	    
	    // load context
	    "   ld    r1,X+             ; sreg	\n"
	    "   ld    r0,X+             ;	\n" 
	    "   ld    r2,X+             ;	\n" 
	    "   ld    r3,X+             ;	\n" 
            "   ld    r4,X+             ;	\n" 
	    "   ld    r5,X+             ;	\n" 
	    "   ld    r6,X+             ;	\n" 
	    "   ld    r7,X+             ; 	\n"
	    "   ld    r8,X+             ;	\n" 
	    "   ld    r9,X+             ;	\n" 
	    "   ld    r10,X+            ;	\n" 
            "   ld    r11,X+            ;	\n" 
	    "   ld    r12,X+            ;	\n" 
	    "   ld    r13,X+            ;	\n" 
	    "   ld    r14,X+            ; 	\n"
	    "   ld    r15,X+            ;	\n" 
	    "   ld    r16,X+            ;	\n" 
	    "   ld    r17,X+            ;	\n" 
            "   ld    r18,X+            ;	\n" 
	    "   ld    r19,X+            ;	\n" 
	    "   ld    r20,X+            ;	\n" 
	    "   ld    r21,X+            ; 	\n"
	    "   ld    r22,X+            ;	\n" 
	    "   ld    r23,X+            ;	\n" 
	    //"   ld    r24,X+            ;	\n" 
            //"   ld    r25,X+            ;	\n" 
	    //"   ld    r26,X+            ;	\n" 
	    //"   ld    r27,X+            ;	\n" 	    	    					    
	    "   ld    r28,X+            ;	\n" 
            "   ld    r29,X+            ;	\n" 
	    "   ld    r30,X+            ;	\n" 
	    "   ld    r31,X+            ;	\n" 
	    
	    "   ld    r25,X+            ; PC	\n"
	    "   ld    r24,X+		;	\n"	    
	    
	    // Put PC back on the Stack
	    "   push  r24		;	\n"
	    "   push  r25		;       \n"
	    
	    // Save SREG and clear r1
	    "   out   0x3f,r1           ; sreg  \n"
	    "   clr   r1		; r1=0  \n" 


            //"   sei                             \n"	
	);
}

void AVR8::Context::save() volatile
{

        ASM(
	    //"   cli                             \n"
	    "   in    r1,0x3f           ; sreg  \n"
	    
	    // Move "this" pointer to X
	    "   mov   r27,r25           ;       \n"  
            "   mov   r26,r24           ;       \n" 
	    
	    // Read Return Address into r25:r24 
	    "   pop   r25		;  	\n" // PC - Return Address
	    "   pop   r24		;  	\n"
	    "   push  r24		;	\n"
	    "	push  r25		; 	\n"

	    // save context
	    "   st    X+,r1             ; sreg	\n"
	    "   st    X+,r0             ;	\n" 
	    "   st    X+,r2             ;	\n" 
	    "   st    X+,r3             ;	\n" 
            "   st    X+,r4             ;	\n" 
	    "   st    X+,r5             ;	\n" 
	    "   st    X+,r6             ;	\n" 
	    "   st    X+,r7             ; 	\n"
	    "   st    X+,r8             ;	\n" 
	    "   st    X+,r9             ;	\n" 
	    "   st    X+,r10            ;	\n" 
            "   st    X+,r11            ;	\n" 
	    "   st    X+,r12            ;	\n" 
	    "   st    X+,r13            ;	\n" 
	    "   st    X+,r14            ; 	\n"
	    "   st    X+,r15            ;	\n" 
	    "   st    X+,r16            ;	\n" 
	    "   st    X+,r17            ;	\n" 
            "   st    X+,r18            ;	\n" 
	    "   st    X+,r19            ;	\n" 
	    "   st    X+,r20            ;	\n" 
	    "   st    X+,r21            ; 	\n"
	    "   st    X+,r22            ;	\n" 
	    "   st    X+,r23            ;	\n" 
	    //"   st    X+,r24            ;	\n" 
            //"   st    X+,r25            ;	\n" 
	    //"   st    X+,r26            ;	\n" 
	    //"   st    X+,r27            ;	\n" 	    	    					    
	    "   st    X+,r28            ;	\n" 
            "   st    X+,r29            ;	\n" 
	    "   st    X+,r30            ;	\n" 
	    "   st    X+,r31            ;	\n" 	   
	    "   st    X+,r25            ; PC	\n" 
	    "   st    X+,r24            ;	\n"  
	    
	    // Clear r1
	    "   clr   r1		; r1=0  \n" 	    
	    
	);
}


void AVR8::switch_context(Context * volatile * o, Context * volatile n) {


        ASM(
	
	    // Read sreg
	    "   in    r1,0x3f           ; sreg  \n"	
	
	    // Move "* o" pointer to X
	    "   mov   r27,r25           ;       \n"  
            "   mov   r26,r24     	;	\n"
	    // Load "o" pointer into r25:r24
	    "   ld    r25,X+		;	\n"
	    "	ld    r24,X		;	\n"
	    // Move "o" pointer to X
	    "   mov   r27,r25           ;       \n"  
            "   mov   r26,r24     	;	\n"
	    
	    	    
	    // Read Return Address into r25:r24 
	    "   pop   r25		;  	\n" // PC - Return Address
	    "   pop   r24		;  	\n"

	    // save context
	    "   st    X+,r1             ; sreg	\n"
	    "   st    X+,r0             ;	\n" 
	    "   st    X+,r2             ;	\n" 
	    "   st    X+,r3             ;	\n" 
            "   st    X+,r4             ;	\n" 
	    "   st    X+,r5             ;	\n" 
	    "   st    X+,r6             ;	\n" 
	    "   st    X+,r7             ; 	\n"
	    "   st    X+,r8             ;	\n" 
	    "   st    X+,r9             ;	\n" 
	    "   st    X+,r10            ;	\n" 
            "   st    X+,r11            ;	\n" 
	    "   st    X+,r12            ;	\n" 
	    "   st    X+,r13            ;	\n" 
	    "   st    X+,r14            ; 	\n"
	    "   st    X+,r15            ;	\n" 
	    "   st    X+,r16            ;	\n" 
	    "   st    X+,r17            ;	\n" 
            "   st    X+,r18            ;	\n" 
	    "   st    X+,r19            ;	\n" 
	    "   st    X+,r20            ;	\n" 
	    "   st    X+,r21            ; 	\n"
	    "   st    X+,r22            ;	\n" 
	    "   st    X+,r23            ;	\n" 
	    //"   st    X+,r24            ;	\n" 
            //"   st    X+,r25            ;	\n" 
	    //"   st    X+,r26            ;	\n" 
	    //"   st    X+,r27            ;	\n" 	    	    					    
	    "   st    X+,r28            ;	\n" 
            "   st    X+,r29            ;	\n" 
	    "   st    X+,r30            ;	\n" 
	    "   st    X+,r31            ;	\n" 	   
	    "   st    X+,r25            ; PC	\n" 
	    "   st    X+,r24            ;	\n"  
	    	    
	    // Move "n" pointer to X
	    "   mov   r27,r23           ;       \n"  
            "   mov   r26,r22     	;	\n"
	    
	    // load context
	    "   ld    r1,X+             ; sreg	\n"
	    "   ld    r0,X+             ;	\n" 
	    "   ld    r2,X+             ;	\n" 
	    "   ld    r3,X+             ;	\n" 
            "   ld    r4,X+             ;	\n" 
	    "   ld    r5,X+             ;	\n" 
	    "   ld    r6,X+             ;	\n" 
	    "   ld    r7,X+             ; 	\n"
	    "   ld    r8,X+             ;	\n" 
	    "   ld    r9,X+             ;	\n" 
	    "   ld    r10,X+            ;	\n" 
            "   ld    r11,X+            ;	\n" 
	    "   ld    r12,X+            ;	\n" 
	    "   ld    r13,X+            ;	\n" 
	    "   ld    r14,X+            ; 	\n"
	    "   ld    r15,X+            ;	\n" 
	    "   ld    r16,X+            ;	\n" 
	    "   ld    r17,X+            ;	\n" 
            "   ld    r18,X+            ;	\n" 
	    "   ld    r19,X+            ;	\n" 
	    "   ld    r20,X+            ;	\n" 
	    "   ld    r21,X+            ; 	\n"
	    "   ld    r22,X+            ;	\n" 
	    "   ld    r23,X+            ;	\n" 
	    //"   ld    r24,X+            ;	\n" 
            //"   ld    r25,X+            ;	\n" 
	    //"   ld    r26,X+            ;	\n" 
	    //"   ld    r27,X+            ;	\n" 	    	    					    
	    "   ld    r28,X+            ;	\n" 
            "   ld    r29,X+            ;	\n" 
	    "   ld    r30,X+            ;	\n" 
	    "   ld    r31,X+            ;	\n" 
	    
	    "   ld    r25,X+            ; PC	\n"
	    "   ld    r24,X+		;	\n"	    
	    
	    // Put PC back on the Stack
	    "   push  r24		;	\n"
	    "   push  r25		;       \n"
	    
	    // Save SREG and clear r1
	    "   out   0x3f,r1           ; sreg  \n"
	    "   clr   r1		; r1=0  \n" 
	    
            //"   sei                             \n"	
	);


}




__END_SYS
