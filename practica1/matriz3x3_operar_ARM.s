		;	Nekane Díaz Montoya	  870795	
		;	Jorge Hernández Aznar 872838
		AREA codigo, CODE,READONLY
		EXPORT matriz3x3_operar_ARM
		PRESERVE8 {TRUE}

		ENTRY
matriz3x3_operar_ARM
			; PRÓLOGO
			STMDB 	SP!,{R4-R12,LR}	   ; Se guardan los registros antiguos y el link register para volver
			SUB 	SP, SP, #36		   ; Se almacena espacio para las variables E, 36 bytes 

			LDR 	R4,[SP,#76] ; R4 = @Resultado 
			
			; No hace falta inicializar Resultado ni E si solo vamos a hacer stores en memoria
end_for		
			; multiplicar(A,B,Resultado)
			MOV 	R10, #12
			; for (i = 2; i >= 0; i--)
			MOV		R6, #2		; R6 = i = 2			
ini_f_i_m1	
			; for (j = 2; j >= 0; j--)
			MOV		R7, #2		; R7 = j = 2
ini_f_j_m1
			; En r5 se va a guardar el resultado de cada resultado[i][j]
			MOV 	R5, #0
			; for (k = 2; k >= 0; k--)
			MOV		R8, #2
ini_f_k_m1
			; A(r9) = i(r6) * 12 + k(r8) * 4 + @A(r0)
			MLA		R9, R10, R6, R0
			LDR 	R9, [R9, R8, LSL #2]
			; B(r11) = k(r8) * 12 + j(r7) * 4 + @B [r1] 
			MLA		R11, R10, R8, R1
			LDR		R11, [R11, R7, LSL #2]

			; Resultado[i][j] += A[i][k] * B[k][j]
			MLA		R5, R9, R11, R5
			
	   		SUB 	R8, R8, #1	; R8--
			CMP		R8, #0
			BGE		ini_f_k_m1
			; BPL		ini_f_k_m
			; fin_for_k
			; Se guarda r5 con la suma total
			MLA		R11, R10, R6, R4
			STR 	R5, [R11, R7, LSL #2]

			SUB 	R7, R7, #1	; R7--
			CMP 	R7, #0
			BGE		ini_f_j_m1
			; BPL		ini_f_j_m
			; fin_for_j			
			SUB 	R6, R6, #1	; R6--
			CMP 	R6, #0
			BGE		ini_f_i_m1	
			; BPL		ini_f_i_m	
			; fin_for_i

			; end-multiplicar(A,B,Resultado)


		    ; llamada a multiplicar(C,D,E)	
			
			ADD 	R1, SP, #0	 ; R1 = @E

			; for (i = 2; i >= 0; i--)
			MOV		R6, #2		; R6 = i = 2			
ini_f_i_m2	
			; for (j = 2; j >= 0; j--)
			MOV		R7, #2		; R7 = j = 2
ini_f_j_m2  
			; En R0 se acumulará el valor de 
			MOV 	R5, #0
			; for (k = 2; k >= 0; k--)
			MOV		R8, #2
ini_f_k_m2
			; C(r9) = i(r6) * 12 + k(r8) * 4 + @C(r2)
			MLA		R9, R10, R6, R2
			LDR 	R9, [R9, R8, LSL #2]
			; D(r11) = k(r8) * 12 + j(r7) * 4 + @D(r3) 
			MLA		R11, R10, R8, R3
			LDR		R11, [R11, R7, LSL #2]
			
			; Se obtiene el nuevo valor a guardar 
			MLA		R5, R9, R11, R5

	   		SUB 	R8, R8, #1	; R8--
			CMP		R8, #0
			BGE		ini_f_k_m2
			; BPL		ini_f_k_m
			; fin_for_k
			; Se guarda r5 con la suma total
			; En vez de guardar el resultado en Resultado[i][j] se guarda en Resultado[j][i] para que se almacena ya transpuesta 
			MLA		R11, R10, R7, R1
			STR 	R5, [R11, R6, LSL #2]

			SUB 	R7, R7, #1	; R7--
			CMP 	R7, #0
			BGE		ini_f_j_m2
			; BPL		ini_f_j_m
			; fin_for_j			
			SUB 	R6, R6, #1	; R6--
			CMP 	R6, #0
			BGE		ini_f_i_m2	
			; BPL		ini_f_i_m	
			; fin_for_i

			; end-multiplicar(C,D,E)

			; Sumar(Resultado,E,Resultado) + Check terminos_no_cero

			; R10 = @Resultado 	R1 = @E

			MOV 	R0, #9	; R0 = terminos_no_cero
			MOV 	R2, #0	; R2 = i

ini_f_suma		
			LDR 	R5, [R1, R2, LSL #2] 	; R5 = E[i][j]
			LDR		R6, [R4, R2, LSL #2]	; R6 = Resultado[i][j]
			ADD		R6, R5, R6				
			CMP 	R3, #0					; if (Resultado[i][j] == 0)
			SUBEQ	R0, R0, #1			    ; 		terminos_no_cero--
			STR 	R6, [R4, R2, LSL #2]

			ADD 	R2, R2, #1
			CMP		R2, #9
			BLT		ini_f_suma

end_f_suma				
			; EPÍLOGO
			ADD 	SP, SP, #36
			LDMIA	SP!,{R4-R12,LR}
			BX		LR 

		END