/* guarda para evitar inclusiones m?ltiples ("include guard") */
#ifndef matrizNxN
#define matrizNxN

#include <inttypes.h>

//el codigo en C debe ser independiente del tamaño de las matrices usando la definicion de "N"
//las versiones en ensamblador hacerlas para matrices de 3x3
#define N 3

/* *****************************************************************************
 * declaracion de funciones visibles en el exterior
 */

void matrizNxN_multiplicar_C(int A[N][N], int B[N][N], int Resultado[N][N]);

uint8_t matrizNxN_operar_C(int A[N][N], int B[N][N], int C[N][N], int D[N][N], int Resultado[N][N]);

//TODO
#if 0
// funciones a implementar en ARM y thumb para matrices de 3x3

//operar en ARM que llama al menos a matrizNxN_multiplicar_C implementada en C suponiendo que N = 3...
uint8_t matriz3x3_operar_ARM_C(int A[N][N], int B[N][N], int C[N][N], int D[N][N], int Resultado[N][N]);

//operar implementada integramente en ARM, se puede hacer con subfunciones o incrustado todo el codigo (pero que se entienda)
uint8_t matriz3x3_operar_ARM(int A[N][N], int B[N][N], int C[N][N], int D[N][N], int Resultado[N][N]);

//operar implementada en Thumb
uint8_t matriz3x3_operar_THB(int A[N][N], int B[N][N], int C[N][N], int D[N][N], int Resultado[N][N]);

#endif // 0

#endif /* matrizNxN */
