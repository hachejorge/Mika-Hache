// fuentes Proyecto Hardware 2024
// version 0.1
//	Nekane Díaz Montoya	  870795	
//	Jorge Hernández Aznar 872838


#include "matriz_3x3.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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


// Dada una matriz A y devuelve en la matriz Transpuesta, el restultado de transporner A
void matrizNxN_transponer(int A[N][N], int Transpuesta[N][N]){
	int i,j;
	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			Transpuesta[j][i] = A[i][j];
		}
	}
}

// Dada una matriz A y otra B devuelve en Resultado la suma de estas
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
// Dada una matriz A y otra B devuelve en Resultado la multiplicación de estas	
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

// función equivalente a matrizNxN_operar_C pero con un diseño monolítico equivalente al implementado en la versión ARM
uint8_t matrizNxN_operar_C_equivalenteARM(int A[N][N], int B[N][N], int C[N][N], int D[N][N], int Resultado[N][N]){
	uint8_t terminos_no_cero = 9;
	int suma;
	int i,j,k;
	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			suma = 0;
			for(k = 0; k < N; k++){
				suma += A[i][k] * B[k][j] + C[j][k] * D[k][i];
			}
			Resultado[i][j] = suma;
			if (suma != 0) {
				terminos_no_cero--;
			}
		}
	}
	return terminos_no_cero;
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

// Compara los resultados de operar una matriz 3x3 en distintas formas, C, Arm y Thumb y devuelve false(0) en caso de que 
// todas las funciones devuelvan el mismo número de terminos no cero , y true(1) en caso contrario 	
uint8_t matrizNxN_verificar(int A[N][N], int B[N][N], int C[N][N], int D[N][N], int Resultado[N][N]){
	uint8_t terminos_no_cero_C;
	uint8_t terminos_no_cero_ARM_C;
	uint8_t terminos_no_cero_ARM;
	uint8_t terminos_no_cero_THB;
	uint8_t	terminos_no_cero_C_equivalenteARM;
	uint8_t resultado;
	
	terminos_no_cero_C = matrizNxN_operar_C(A, B, C, D, Resultado);
	terminos_no_cero_ARM_C = matriz3x3_operar_ARM_C(A, B, C, D, Resultado);
	terminos_no_cero_ARM = matriz3x3_operar_ARM(A, B, C, D, Resultado);
	terminos_no_cero_THB = matriz3x3_operar_THB(A, B, C, D, Resultado);
	terminos_no_cero_C_equivalenteARM = matrizNxN_operar_C_equivalenteARM(A, B, C, D, Resultado);
	
	resultado = !((terminos_no_cero_C != terminos_no_cero_ARM_C) || (terminos_no_cero_ARM_C != terminos_no_cero_ARM) || (terminos_no_cero_ARM != terminos_no_cero_THB) || (terminos_no_cero_THB != terminos_no_cero_C_equivalenteARM));

	return resultado;
}

// Compara todas las componentes de las matrices rC, rCArm,	rArm, rThumb y rCOptimizado y devuelve fales(0) si solo si todas la matrices son iguales, 
// y devuelve true(1) en caso contrario
uint8_t matrizNxN_compararResultados(int rC[N][N], int rCArm[N][N], int rArm[N][N], int rThumb[N][N], int rCOptimizado[N][N]){

	uint8_t resultado = 0;
	int i, j;
	for(i = 0; i < N && resultado == 1; i++){
		for(j = 0; j < N && resultado == 1; j++){
			if( rC[i][j] != rCArm[i][j] || rCArm[i][j] != rArm[i][j] || rArm[i][j] != rThumb[i][j] || rThumb[i][j] != rCOptimizado[i][j]){
				 resultado = 1;
			}
		}
	}
	return resultado;
}

// Devuelve un número aleatorio en el rango (a,b)
int randInt(int min, int max) {
	return min + rand() % (max - min +1);
}

// Asigna valores aleatorios a las matrices A, B, C y D
int leerMatrices(int A[N][N], int B[N][N], int C[N][N], int D[N][N]){
	int i, j;
    for(i = 0; i < N; i++ ){
    	for(j = 0; j < N; j++) {   
			A[i][j] = randInt(1,10);
            B[i][j] = randInt(1,10);
			C[i][j] = randInt(1,10);
			D[i][j] = randInt(1,10);  
    	}  
    }
	return 0;
}

// Compara los resultados de operar una matriz 3x3 en distintas formas, C, Arm y Thumb y devuelve false(0) en caso de que 
// todas las funciones calculen el mismo resultado, y true(1) en caso contrario
uint8_t verificar_resultados(){
	int A[N][N], B[N][N], C[N][N], D[N][N];
	int rC[N][N], rCArm[N][N], rArm[N][N], rThumb[N][N], rCOptimizado[N][N];
	// Se incializan las matrices A, B, C y D de forma aleatoria
	leerMatrices(A,B,C,D);
	// Se calculan sus resultados en distintas versiones
	matrizNxN_operar_C(A, B, C, D, rC);
	matriz3x3_operar_ARM_C(A, B, C, D, rCArm);
	matriz3x3_operar_ARM(A, B, C, D, rArm);
	matriz3x3_operar_THB(A, B, C, D, rThumb);
	matrizNxN_operar_C_equivalenteARM(A, B, C, D, rCOptimizado);
	// Se devuelve el resultado de comparar los resultados de distintas versiones
	return matrizNxN_compararResultados(rC, rCArm, rArm, rThumb, rCOptimizado);
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

	// Verificar resultados con terminos no cero original
	error = matrizNxN_verificar(Test_A, Test_B, Test_C, Test_D, Resultado_E);

	// Nueva función de verificación validando la igualdad de los resultados
	//error = verificar_resultados();	

	while(1); //no hay S.O., no se retorna
	
}