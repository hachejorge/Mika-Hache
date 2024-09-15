		PRESERVE8

		AREA codigo, CODE
		EXPORT matriz3x3_operar_ARM_C
		IMPORT matrizNxN_multiplicar_C

		ENTRY
matriz3x3_operar_ARM_C
			STMDB 	SP!,{R4-R11,LR}	   ; Se guardan los registros antiguos y el link register para volver
			SUB 	SP, SP, #72		   ; Se almacena espacio para las variables E y F, 36 bytes respectivamente

			mov r6, r2	; r6 = @C
			mov r7, r3  ; r7 = @D
			
			LDR 	R2,[SP,#108] ; R2 = @Resultado 
			ADD 	R4, SP, #36	 ; R4 = @E
			MOV 	R8, #0		 ; i = r8
			MOV 	R9, #0 		 ; R9 = 0 CTE

ini_for		CMP 	R8, #9
			BGE		end_for

		    STR 	R9, [R2, R8, LSL #2]
			STR 	R9, [R4, R8,	LSL #2]

			ADD 	R8, R8, #1
			B ini_for

end_for					
			MOV 	R8, R0	 ; R8 = @A
			MOV 	R9, R1	 ; R9 = @B
			MOV 	R10, R2	 ; R10 = @Resultado

			; llamada a multiplicar 
			BL		matrizNxN_multiplicar_C

			MOV 	R0, R6	 ; R6 = @C
			MOV 	R1, R7	 ; R7 = @D
			MOV 	R2, SP	 ; SP = @E

			BL		matrizNxN_multiplicar_C	

			



AQUI		b AQUI
		END