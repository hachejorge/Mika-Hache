;	Nekane D�az Montoya	  870795	
		;	Jorge Hern�ndez Aznar 872838
		AREA codigo, CODE,READONLY
		EXPORT matriz3x3_operar_THB
		PRESERVE8 {TRUE}
		THUMB
		ENTRY
matriz3x3_operar_THB
			; PR�LOGO
			STMDB 	SP!,{R4-R7,LR}	   ; Se guardan los registros antiguos y el link register para volver
			SUB 	SP, SP, #36		   ; Se almacena espacio para las variables E, 36 bytes 

			LDR 	R4, [SP,#76] ; R4 = @Resultado 
			
			; No hace falta inicializar Resultado ni E si solo vamos a hacer stores en memoria
			; Movemos las direccion de las matrices a high registers
			MOV 	R8, R0				; movemos @A a r8
			MOV 	R9, R1 				; movemos @B a r9
			MOV 	R10, R2 			; movemos @C a r10
			MOV 	R11, R3 			; movemos @D a r11

			; multiplicar(A,B,Resultado) and multiplicar(C,D,E)
			MOVS 	R0, #12
			; for (i = 2; i >= 0; i--)
			MOVS	R6, #2		; R6 = i = 2			
ini_f_i_m	
			; for (j = 2; j >= 0; j--)
			MOVS	R7, #2		; R7 = j = 2
ini_f_j_m
			; En r5 se va a guardar el resultado de cada Resultado[i][j]
			MOVS 	R5, #0
			; En r12 se va a guardar el valor de cada E[i][j]
			MOVS	R1, #0
			; for (k = 2; k >= 0; k--)
			MOVS	R2, #2
ini_f_k_m
			; A(r9) = i(r6) * 12 + k(r8) * 4 + @A(r0)
			;MLA		R9, R0, R6, R0
			;LDR 	R9, [R9, R2, LSL #2]
			MOVS	R3, R6
			MULS 	R3, R0, R3
			ADD 	R3, R3, R8
			LDR		R3, [R3, R2, LSL #2]
			
			; B(r11) = k(r8) * 12 + j(r7) * 4 + @B [r1] 
			MLA		R11, R0, R2, R1
			LDR		R11, [R11, R7, LSL #2]
			; Resultado[i][j] += A[i][k] * B[k][j] + Resultado[i][j]
			MLA		R5, R9, R11, R5

			; C(r9) = i(r6) * 12 + k(r8) * 4 + @C(r2)
			MLA		R9, R0, R6, R2
			LDR 	R9, [R9, R2, LSL #2]
			; D(r11) = k(r8) * 12 + j(r7) * 4 + @D(r3) 
			MLA		R11, R0, R2, R3
			LDR		R11, [R11, R7, LSL #2]
			; E[i][j] += C[i][k] * D[k][j]	+ E[i][j]
			MLA		R1, R9, R11, R1
			
	   		SUBS 	R2, R2, #1	; R8--
			BPL		ini_f_k_m
			; fin_for_k
			; Se guarda r5 con la suma total de Resultado
			MLA		R11, R0, R6, R4
			STR 	R5, [R11, R7, LSL #2]
			; Se guarda r12 con la suma total de E
			; En vez de guardar el resultado en E[i][j] se guarda en E[j][i] para que se almacena ya transpuesta 
			MLA		R11, R0, R7, SP
			STR 	R1, [R11, R6, LSL #2]

			SUBS 	R7, R7, #1	; R7--
			BPL		ini_f_j_m
			; fin_for_j			
			SUBS 	R6, R6, #1	; R6--	
			BPL		ini_f_i_m	
			; fin_for_i

			; end multiplicar(A,B,Resultado) and multiplicar(C,D,E)

			; Sumar(Resultado,E,Resultado) + Check terminos_no_cero

			; R4 = @Resultado 	SP = @E

			MOV 	R0, #9	; R0 = terminos_no_cero
			MOV 	R2, #9	; R2 = i

ini_f_suma		
			LDR 	R5, [SP, R2, LSL #2] 	; R5 = E[i][j]
			LDR		R6, [R4, R2, LSL #2]	; R6 = Resultado[i][j]
			ADD		R6, R5, R6				
			CMP 	R3, #0					; if (Resultado[i][j] == 0)
			SUBEQ	R0, R0, #1			    ; 		terminos_no_cero--
			STR 	R6, [R4, R2, LSL #2]

			SUBS 	R2, R2, #1
			BNE		ini_f_suma

end_f_suma				
			; EP�LOGO
			ADD 	SP, SP, #36
			LDMIA	SP!,{R4-R7,LR}
			BX		LR 

		END