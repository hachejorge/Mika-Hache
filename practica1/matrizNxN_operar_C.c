#include "matriz_3x3.h"
uint8_t matrizNxN_operar_C(int A[N][N], int B[N][N], int C[N][N], int D[N][N], int Resultado[N][N]){
	int i,j,k, terminos_no_cero;
	int Resultado1[N][N], Transpuesta[N][N];
	
	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++){
				Resultado[i][j]=0;
			for(k = 0; k < N; k++){
				Resultado[i][j] += A[i][k] * B[k][j];
			}
		}
	}
	
	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			for(k = 0; k < N; k++){
				Resultado1[i][j] += C[i][k] * D[k][j];
			}
		}
	}
	
	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			Transpuesta[j][i] = Resultado1[i][j];
		}
	}
	
	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			Resultado[i][j] = Resultado[i][j] + Transpuesta[i][j];
		}
	}


	for(i = 0; i < N; i++){
			for(j = 0; j < N; j++){
				if(Resultado[i][j] == 0) { 
					terminos_no_cero--;
				}
			}
		}
	return terminos_no_cero;

}
