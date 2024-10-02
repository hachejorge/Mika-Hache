;	Nekane Dï¿½az Montoya	  870795	
		;	Jorge Hernï¿½ndez Aznar 872838
		AREA codigo, CODE,READONLY
		EXPORT matriz3x3_operar_THB
		PRESERVE8 {TRUE}
		ENTRY
matriz3x3_operar_THB
			; PRÓLOGO
			ARM
			STMDB 	SP!,{R4-R12,LR}	   ; Se guardan los registros antiguos y el link register para volver
			SUB 	SP, SP, #36		   ; Se almacena espacio para las variables E, 36 bytes 

			ADR		R4, ini_thumb + 1
			BX		R4

			THUMB
ini_thumb
			LDR 	R4, [SP, #76] 	   ; R4 = @Resultado 
			MOV		R12, R4			

			; No hace falta inicializar Resultado ni E si solo vamos a hacer stores en memoria
			; Movemos las direccion de las matrices a high registers
			MOV 	R8, R0				; movemos @A a r8
			MOV 	R9, R1				; movemos @B a r9
			MOV 	R10, R2				; movemos @C a r10
			MOV 	R11, R3				; movemos @D a r11

			; multiplicar(A,B,Resultado) and multiplicar(C,D,E)
			
			; for (i = 2; i >= 0; i--)
			MOVS	R0, #2				; R6 = i = 2			
ini_f_i_m	
			; for (j = 2; j >= 0; j--)
			MOVS	R1, #2				; R7 = j = 2
ini_f_j_m
			; En r5 se va a guardar el resultado de cada Resultado[i][j]
			MOVS 	R5, #0
			; En r12 se va a guardar el valor de cada E[i][j]
			MOVS	R6, #0
			; for (k = 2; k >= 0; k--)
			MOVS	R2, #2
ini_f_k_m
			MOVS 	R7, #12
			; A(r3) = i(r0) * 12 + k(r2) * 4 + @A(r8)
			;MLA	R9, R0, R6, R0
			;LDR 	R9, [R9, R2, LSL #2]
			MOVS	R3, R0
			MULS 	R3, R7, R3
			; B(r4) = k(r8) * 12 + j(r7) * 4 + @B(r9) 
			;MLA		R11, R0, R2, R1
			;LDR		R11, [R11, R7, LSL #2]
			MOVS	R4, R2
			MULS	R4, R7, R4

			ADD 	R3, R3, R8
			LSLS	R7, R2, #2
			LDR		R3, [R3, R7]
			
			ADD		R4, R4, R9
			LSLS 	R7, R1, #2
			LDR 	R4, [R4, R7]

			; Resultado[i][j] += A[i][k] * B[k][j] + Resultado[i][j]
			;MLA		R5, R9, R11, R5
			MULS 	R4, R3, R4
			ADDS	R5, R5, R4

			MOVS 	R7, #12
			; C(r3) = i(r0) * 12 + k(r2) * 4 + @C(r10)
			;MLA		R9, R0, R6, R2
			;LDR 	R9, [R9, R2, LSL #2]
			MOVS	R3, R0
			MULS 	R3, R7, R3
			
			; D(r11) = k(r8) * 12 + j(r7) * 4 + @D(r3) 
			;MLA		R11, R0, R2, R3
			;LDR		R11, [R11, R7, LSL #2]
			MOVS	R4, R2
			MULS	R4, R7, R4
					
			ADD 	R3, R3, R10
			LSLS	R7, R2, #2
			LDR		R3, [R3, R7]
			
			ADD		R4, R4, R11
			LSLS 	R7, R1, #2
			LDR 	R4, [R4, R7]

			; E[i][j] += C[i][k] * D[k][j]	+ E[i][j]
			;MLA		R1, R9, R11, R1
			MULS 	R4, R3, R4
			ADDS	R6, R6, R4			

	   		SUBS 	R2, R2, #1	; R2--
			BPL		ini_f_k_m
			; fin_for_k
			MOVS 	R7, #12
			; Se guarda r5 con la suma total de Resultado
			;MLA	R11, R10, R6, R4
			;STR 	R5, [R11, R7, LSL #2]
			MOVS	R3, R0
			MULS	R3, R7, R3
			ADD		R3, R3, R12
			LSLS	R4, R1, #2
			STR		R5, [R3, R4]
			; Se guarda r12 con la suma total de E
			; En vez de guardar el resultado en E[i][j] se guarda en E[j][i] para que se almacena ya transpuesta 
			;MLA		R11, R0, R7, SP
			;STR 	R1, [R11, R6, LSL #2]
			MOVS	R3, R1
			MULS	R3, R7, R3
			ADD		R3, R3, SP
			LSLS	R4, R0, #2
			STR		R6, [R3, R4]

			SUBS 	R1, R1, #1			; R1--
			BPL		ini_f_j_m
			; fin_for_j			
			SUBS 	R0, R0, #1			; R0--	
			BPL		ini_f_i_m	
			; fin_for_i

			; end multiplicar(A,B,Resultado) and multiplicar(C,D,E)

			; Sumar(Resultado,E,Resultado) + Check terminos_no_cero

			; R12 = @Resultado 	SP = @E

			MOVS 	R0, #9				; R0 = terminos_no_cero
			MOVS	R1, #8				; R1 = i
			MOV		R4, R12				; R4 = @Resultado
			MOV		R5, SP

ini_f_suma	
			LSLS	R6, R1, #2	
			LDR 	R2, [R5, R6]		; R5 = E[i][j]
			LDR		R3, [R4, R6]		; R6 = Resultado[i][j]
			ADDS	R3, R3, R2				
			CMP 	R3, #0				; if (Resultado[i][j] == 0)
			BNE		end_if
			;IT EQ
			SUBS	R0, R0, #1			; terminos_no_cero--
end_if		
			STR 	R3, [R4, R6]

			SUBS 	R1, R1, #1
			BGE		ini_f_suma

end_f_suma	

			ADR 	R1, ini_arm
			BX		R1
			ARM
ini_arm			
			; EPÍLOGO
			ADD 	SP, SP, #36
			LDMIA	SP!,{R4-R12, LR}
			BX		LR 

		END
