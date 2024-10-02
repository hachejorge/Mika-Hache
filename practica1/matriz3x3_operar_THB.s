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

			ADR		R4, ini_thumb + 1
			BX		R4

			THUMB
ini_thumb
			LDR 	R4, [SP, #40] 	   ; R12 = @Resultado , #40
			MOV		R12, R4			

			; No hace falta inicializar Resultado ni E si solo vamos a hacer stores en memoria
			; Movemos las direccion de las matrices a high registers
			MOV 	R8, R0				; movemos @A a r8
			MOV 	R9, R1				; movemos @B a r9
			MOV 	R10, R2				; movemos @C a r10
			MOV 	R11, R3				; movemos @D a r11

			; multiplicar(A,B,Resultado) and multiplicar(C,D,E)
			MOVS	R0, #0
			MOV		R14, R0		; R14 = terminos_no_cero
						
			; for (i = 2; i >= 0; i--)
			MOVS	R0, #2				; R0 = i = 2			
ini_f_i_m	
			; for (j = 2; j >= 0; j--)
			MOVS	R1, #2				; R1 = j = 2
ini_f_j_m
			; En r5 se va a guardar el resultado de cada Resultado[i][j]
			MOVS 	R5, #0
			; En r6 se va a guardar el valor de cada E[i][j]
			MOVS	R6, #0

			; for (k = 2; k >= 0; k--)
			MOVS	R2, #2	 			; R2 = k = 2
ini_f_k_m
			MOVS 	R7, #12
			; A(r3) = i(r0) * 12 + k(r2) * 4 + @A(r8)
			MOVS	R3, R0
			MULS 	R3, R7, R3			; r3 = i(r0, r3)*12(r7)
			; B(r4) = k(r8) * 12 + j(r7) * 4 + @B(r9) 
			MOVS	R4, R2
			MULS	R4, R7, R4			; r4 = k(r2, r4)*12(r7)


			ADD 	R3, R3, R8			; r3 = i(r3)*12(r7) + @A(r8)
			LSLS	R7, R2, #2			; r7 = 4 * k(r2)
			LDR		R3, [R3, R7]		; guarda lo que hay en la direccion de A apuntada en r3

			ADD		R4, R4, R9			; r4 = k(r2, r4)*12(r7)
			LSLS 	R7, R1, #2			; r7 = 4 * j(r1)
			LDR 	R4, [R4, R7]		; guarda lo que hay en la direccion de B apuntada en r4

			; Resultado[i][j] += A[i][k] * B[k][j] + Resultado[i][j]
			MULS 	R4, R3, R4			; r4 = A[i][j](r3) * B[i][j](r4)
			ADDS	R5, R5, R4			; r5 = r4 + Resultado[i][j]

			MOVS 	R7, #12
			; C(r3) = j(r1) * 12 + k(r2) * 4 + @C(r10)
			MOVS	R3, R1				 
			MULS 	R3, R7, R3			; r3 = i(r3, r0) * 12(r7)
			
			; D(r11) = k(r2) * 12 + i(r0) * 4 + @D(r11) 
			MOVS	R4, R2
			MULS	R4, R7, R4			; r4 = k(r4, r2) * 12(r7)
					
			ADD 	R3, R3, R10			; i(r3, r0) * 12(r7) + @C (r10)
			LSLS	R7, R2, #2			; r7 = 4 * k(r2)
			LDR		R3, [R3, R7]		; guarda lo que hay en la direccion de C apuntada en r3
			
			ADD		R4, R4, R11			; k(r4, r2) * 12(r7) + @D
			LSLS 	R7, R0, #2			; r7 = 4 * j(r1)
			LDR 	R4, [R4, R7]		; guarda lo que hay en la direccion de D apuntada en r4

			; E[i][j] += C[i][k] * D[k][j]	+ E[i][j]
			MULS 	R4, R3, R4			; r4 = C[i][j](r3) * D[i][j](r4)
			ADDS	R6, R6, R4			; r6 = r4 + E[i][j]

	   		SUBS 	R2, R2, #1	; R2--
			BPL		ini_f_k_m
			; fin_for_k
			MOVS 	R2, #1				; R2 = #1
			MOVS 	R7, #12
			
			ADDS	R5, R5, R6 			; Resultado[i][j] + E[i][j]
			CMP 	R5, #0				; if (Resultado[i][j] != 0){
			BEQ		es_cero
			ADD		R14, R14, R2		; terminos_no_cero++}

es_cero		MULS    R7, R0, R7			; R7 = i * 12 (porque cada fila tiene 12 bytes, 3 enteros de 4 bytes)
    		ADD     R7, R7, R12			; R7 = i * 12 + @Resultado (R12)
    		LSLS    R4, R1, #2			; R4 = j * 4 (columna multiplicada por 4 bytes por elemento)
   										; R7 = i * 12 + @Resultado + j * 4
    		STR     R5, [R7, R4]		; Almacena el valor en la dirección calculada
			
			SUBS 	R1, R1, #1			; R1--
			BPL		ini_f_j_m
			; fin_for_j			
			SUBS 	R0, R0, #1			; R0--	
			BPL		ini_f_i_m	
			; fin_for_i

			MOV 	R0, R14				; R0 = terminos_no_cero

			ADR 	R1, ini_arm
			BX		R1
			ARM
ini_arm			
			; EPÍLOGO
			LDMIA	SP!,{R4-R12, LR}
			BX		LR 

		END