		;	Nekane Díaz Montoya	  870795	
		;	Jorge Hernández Aznar 872838
		AREA codigo, CODE,READONLY
		EXPORT matriz3x3_operar_ARM
		PRESERVE8 {TRUE}

		ENTRY
matriz3x3_operar_ARM
			; PRÓLOGO
			STMDB 	SP!,{R4-R12,LR}	   ; Se guardan los registros antiguos y el link register para volver
			SUB 	SP, SP, #72		   ; Se almacena espacio para las variables E y F, 36 bytes respectivamente

			MOV 	R6, R2		 ; r6 = @C
			MOV 	R7, R3  	 ; r7 = @D
			LDR 	R4,[SP,#112] ; R4 = @Resultado 
			ADD 	R5, SP, #40	 ; R5 = @E
			
			MOV 	R8, #0		 ; i = r8
			MOV 	R9, #0 		 ; R9 = 0 CTE

ini_for		CMP 	R8, #9
			BGE		end_for

		    STR 	R9, [R4, R8, LSL #2]
			STR 	R9, [R5, R8, LSL #2]

			ADD 	R8, R8, #1
			B ini_for

end_for		
			; multiplicar(A,B,Resultado)
			MOV 	R10, #12
			; for (i = 2; i >= 0; i--)
			MOV		R6, #2		; R6 = i = 2			
ini_f_i_m	
			; for (j = 2; j >= 0; j--)
			MOV		R7, #2		; R7 = j = 2
ini_f_j_m
			; for (k = 2; k >= 0; k--)
			MOV		R8, #2
ini_f_k_m
			; A(r9) = i(r6) * 12 + k(r8) * 4 + @A(r0)
			MLA		R9, R10, R6, R0
			LDR 	R9, [R9, R8, LSL #2]
			; B(r11) = k(r8) * 12 + j(r7) * 4 + @B [r1] 
			MLA		R11, R10, R8, R1
			LDR		R11, [R11, R7, LSL #2]
			; Resultado
			MLA		R12, R10, R6, R4
			LDR 	R12, [R12, R7, LSL #2]

			MLA		R12, R9, R11, R12
			; GUARDAR R12 EN RESULTADO
			MLA		R11, R10, R6, R4
			STR 	R12, [R11, R7, LSL #2]

	   		SUB 	R8, R8, #1	; R8--
			CMP		R8, #0
			BGE		ini_f_k_m
			; BPL		ini_f_k_m
			; fin_for_k
			SUB 	R7, R7, #1	; R7--
			CMP 	R7, #0
			BGE		ini_f_j_m
			; BPL		ini_f_j_m
			; fin_for_j			
			SUB 	R6, R6, #1	; R6--
			CMP 	R6, #0
			BGE		ini_f_i_m	
			; BPL		ini_f_i_m	
			; fin_for_i

			; end-multiplicar(A,B,Resultado)

			MOV 	R0, R6	 ; R0 = @C
			MOV 	R1, R7	 ; R1 = @D
			MOV 	R2, R4	 ; R2 = @E

		    ; llamada a multiplicar(C,D,E)	

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