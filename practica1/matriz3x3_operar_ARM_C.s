		PRESERVE8

		AREA codigo, CODE
		EXPORT matriz3x3_operar_ARM_C
		IMPORT matrizNxN_multiplicar_C

		ENTRY
matriz3x3_operar_ARM_C
			; PRÓLOGO
			STMDB 	SP!,{R4-R12,LR}	   ; Se guardan los registros antiguos y el link register para volver
			SUB 	SP, SP, #72		   ; Se almacena espacio para las variables E y F, 36 bytes respectivamente

			MOV 	R6, R2		 ; r6 = @C
			MOV 	R7, R3  	 ; r7 = @D
			LDR 	R2,[SP,#112] ; R2 = @Resultado 
			ADD 	R4, SP, #40	 ; R4 = @E
			

			MOV 	R8, #0		 ; i = r8
			MOV 	R9, #0 		 ; R9 = 0 CTE

ini_for		CMP 	R8, #9
			BGE		end_for

		    STR 	R9, [R2, R8, LSL #2]
			STR 	R9, [R4, R8, LSL #2]

			ADD 	R8, R8, #1
			B ini_for

end_for					
			MOV 	R8, R0	 ; R8 = @A
			MOV 	R9, R1	 ; R9 = @B
			MOV 	R10, R2	 ; R10 = @Resultado

			; llamada a multiplicar(A,B,Resultado)
			BL		matrizNxN_multiplicar_C

			MOV 	R0, R6	 ; R0 = @C
			MOV 	R1, R7	 ; R1 = @D
			MOV 	R2, R4	 ; R2 = @E

			BL		matrizNxN_multiplicar_C	

			; Trasponer (E, F) 
			MOV 	R5, SP	 ; R5 = @F
			MOV		R11, #12	
			
			; R0 = i = 0
			MOV 	R0, #0

trans_i_ini	; i < 3	
		    CMP 	R0, #3
			BGE		trans_i_end
			; R1 = j = 0
			MOV 	R1, #0

trans_j_ini ; j < 3
			CMP 	R1, #3
			BGE 	trans_j_end

			; @E = E + j*4 + i*12
			MUL 	R12, R11 ,R0 
			ADD 	R2, R12, R1, LSL #2
			LDR 	R3, [R4, R2]

			; @F = F + i*4 + j*12
			MUL		R12, R11, R1
			ADD 	R2, R12, R0, LSL #2
			STR 	R3, [R5, R2]

		   	ADD 	R1, R1, #1	; i++
			B		trans_j_ini
trans_j_end
			ADD 	R0, R0, #1	; j++
			B 		trans_i_ini
trans_i_end

			; Sumar(Resultado,F,Resultado) + Check terminos_no_cero

			; R10 = @Resultado 	R5 = @F

			MOV 	R0, #9	; R0 = terminos_no_cero
			MOV 	R1, #0	; R1 = #0	
			MOV 	R2, #0	; R2 = i

ini_f_suma	CMP 	R2, #9	
			BGE		end_f_suma

			LDR 	R3, [R5, R2, LSL #2] 	; R3 = F[i][j]
			LDR		R4, [R10, R2, LSL #2]	; R4 = Resultado[i][j]
			ADD		R3, R4, R3				
			CMP 	R3, R1					; if (Resultado[i][j] == 0)
			SUBEQ	R0, R0, #1			    ; 		terminos_no_cero--
			STR 	R3, [R10, R2, LSL #2]

			ADD 	R2, R2, #1
			B ini_f_suma

end_f_suma				
			; EPÍLOGO
			ADD 	SP, SP, #72
			LDMIA	SP!,{R4-R12,LR}
			BX		LR 

		END