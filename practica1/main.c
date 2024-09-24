// fuentes Proyecto Hardware 2024
// version 0.1
//	Nekane Díaz Montoya	  870795	
//	Jorge Hernández Aznar 872838


#include "matriz_3x3.h"

    // Inicializaciï¿½n de matrices usando el tipo de datos Matriz3x3
static int Test_A[N][N] = {
		{1, 0, 0},
		{0, 1, 0},
		{0, 0, 1}
	};

static int Test_B[N][N] = {
		{9, 8, 7},
		{6, 5, 4},
		{3, 2, 1}
	};


/* *****************************************************************************
 * declaraciÃ³n funciones internas 
 */
		
// funcion que ejecuta las distintas versiones de la implementacion en C, ARM y Thumb y verifica que dan el mismo resultado.
// recibe las matrices con las que operar
// devuelve si todas las versiones coinciden en el numero de terminos_no_cero o no y el resultado de la opoeraciï¿½n.
uint8_t matrizNxN_verificar(int A[N][N], int B[N][N], int C[N][N], int D[N][N], int Resultado[N][N]);


void matrizNxN_transponer(int A[N][N], int Transpuesta[N][N]){
	int i,j;
	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			Transpuesta[j][i] = A[i][j];
		}
	}
}

void matrizNxN_sumar(int A[N][N], int B[N][N], int Resultado[N][N] ){
	int i,j;
	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			Resultado[i][j] = A[i][j] + B[i][j];
		}
	}
}		
		

/* *****************************************************************************
 * IMPLEMENTACIONES 
 */
		
void matrizNxN_multiplicar_C(int A[N][N], int B[N][N], int Resultado[N][N]){
	int i,j,k;
	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			for(k = 0; k < N; k++){
				Resultado[i][j] += A[i][k] * B[k][j];
			}
		}
	}
}

//funcion que calcula Resultado = A*B + transpuesta(C*D) y devuelva el numero de terminos distintos de cero en el Resultado
//ayudandose de funcion matrizNxN_multiplicar_C que calcula A*B de NxN
uint8_t matrizNxN_operar_C(int A[N][N], int B[N][N], int C[N][N], int D[N][N], int Resultado[N][N]){
	uint8_t terminos_no_cero;
	int E[N][N],F[N][N];
	int i,j;
	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			E[i][j] = 0;
			Resultado[i][j] = 0;
		}
	}	

	matrizNxN_multiplicar_C(A, B, Resultado);
	matrizNxN_multiplicar_C(C, D, E);
	matrizNxN_transponer(E,F);
	matrizNxN_sumar(Resultado,F,Resultado);

	terminos_no_cero = 9;

	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			if(Resultado[i][j] == 0) { 
				terminos_no_cero--;
			}
		}
	}
	return terminos_no_cero;
}
		
uint8_t matrizNxN_verificar(int A[N][N], int B[N][N], int C[N][N], int D[N][N], int Resultado[N][N]){
	uint8_t terminos_no_cero_C;
	uint8_t terminos_no_cero_ARM_C;
	uint8_t terminos_no_cero_ARM;
	uint8_t terminos_no_cero_THB;
	uint8_t resultado;
	
	terminos_no_cero_C = matrizNxN_operar_C(A, B, C, D, Resultado);
	terminos_no_cero_ARM_C = matriz3x3_operar_ARM_C(A, B, C, D, Resultado);
	terminos_no_cero_ARM = matriz3x3_operar_ARM(A, B, C, D, Resultado);
	terminos_no_cero_THB = matriz3x3_operar_THB(A, B, C, D, Resultado);
		
	//TODO llamar al resto de implementaciones en ensamblador
	
	//TODO resultado = (terminos_no_cero_C ! = al resto....
	
	resultado = (terminos_no_cero_C != terminos_no_cero_ARM_C) && (terminos_no_cero_ARM_C != terminos_no_cero_ARM);

	return resultado;
}

// MAIN 
int main (void) {
	int Resultado_E[N][N];
	int error;

	int Test_C[N][N]  = {
		{1, 0, 2},
		{0, 1, 2},
		{2, 0, 1}
	};
	     
	int Test_D[N][N]  = {
		{2, 1, 0},
		{1, 2, 0},
		{0, 0, 2}
	};

	error = matrizNxN_verificar(Test_A, Test_B, Test_C, Test_D, Resultado_E);
	
	while(1); //no hay S.O., no se retorna
	
}

/**
 *@}
 **/
