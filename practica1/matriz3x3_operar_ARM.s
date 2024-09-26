		;	Nekane Díaz Montoya	  870795	
		;	Jorge Hernández Aznar 872838
		AREA codigo, CODE,READONLY
		EXPORT matriz3x3_operar_ARM
		PRESERVE8 {TRUE}

		ENTRY
matriz3x3_operar_ARM
			; PRÓLOGO
			STMDB 	SP!,{R4-R12,LR}	   ; Se guardan los registros antiguos y el link register para volver
			MOV 	FP, SP		
			LDR 	R4,[FP,#40] 	   ; R4 = @Resultado 
		
			; multiplicar(A,B,Resultado) and multiplicar(C,D,E)
			MOV 	R10, #12
			MOV 	R14, #9
			; for (i = 2; i >= 0; i--)
			MOV		R6, #2		; R6 = i = 2			
ini_f_i_m	
			; for (j = 2; j >= 0; j--)
			MOV		R7, #2		; R7 = j = 2
ini_f_j_m
			; En r5 se va a guardar el resultado de cada Resultado[i][j]
			MOV 	R5, #0
			; En r12 se va a guardar el valor de cada E[i][j]
			MOV		R12, #0
			; for (k = 2; k >= 0; k--)
			MOV		R8, #2
ini_f_k_m
			; A(r9) = i(r6) * 12 + k(r8) * 4 + @A(r0)
			MLA		R9, R10, R6, R0
			LDR 	R9, [R9, R8, LSL #2]
			; B(r11) = k(r8) * 12 + j(r7) * 4 + @B [r1] 
			MLA		R11, R10, R8, R1
			LDR		R11, [R11, R7, LSL #2]
			; Resultado[i][j] += A[i][k] * B[k][j] + Resultado[i][j]
			MLA		R5, R9, R11, R5

			; C(r9) = j(r7) * 12 + k(r8) * 4 + @C(r2)
			MLA		R9, R10, R7, R2
			LDR 	R9, [R9, R8, LSL #2]
			; D(r11) = k(r8) * 12 + i(r6) * 4 + @D(r3) 
			MLA		R11, R10, R8, R3
			LDR		R11, [R11, R6, LSL #2]
			; E[i][j] += C[j][k] * D[k][i]	+ E[i][j]
			MLA		R12, R9, R11, R12
			
	   		SUBS 	R8, R8, #1	; R8--
			BPL		ini_f_k_m
			; fin_for_k
			
			ADD		R5, R5, R12 	; RESULTADO[i][j] + E[i][j]
			CMP 	R5, #0					; if (Resultado[i][j] == 0)
			SUBEQ	R14, R14, #1			    ; terminos_no_cero--

			MLA		R11, R10, R6, R4
			STR 	R5, [R11, R7, LSL #2]

			SUBS 	R7, R7, #1	; R7--
			BPL		ini_f_j_m
			; fin_for_j			
			SUBS 	R6, R6, #1	; R6--	
			BPL		ini_f_i_m	
			; fin_for_i

			; end multiplicar(A,B,Resultado) and multiplicar(C,D,E)

			MOV 	R0, R14
					
			; EPÍLOGO
			LDMIA	SP!,{R4-R12,LR}
			BX		LR 

		END