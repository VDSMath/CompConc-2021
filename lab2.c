/* Disciplina: Computacao Concorrente */
/* Nome: Matheus Vinicius da Silva de Figueiredo */
/* DRE: 116023504 */
/* Laboratorio 2 */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include "timer.h"

#define VALORINICIAL 3

float *mat1;
float *mat2;
float *saida;
int nthreads;

//struct passada para as threads
typedef struct {
   int id;
   int tam;
} t_Args;

//funcao executada pelas threads, multiplica duas matrizes quadradas
void *tarefa (void *arg) 
{
  t_Args *args = (t_Args *) arg;
  
  for(int i = args->id; i < args->tam; i+=nthreads)
  {
      for(int j = 0; j < args->tam; j++)
      {
          for(int k = 0; k < args->tam; k++)
          {
            saida[i*(args->tam) + j] += mat1[i*(args->tam) + k] * mat2[k*(args->tam) + j];
          }
      }
  }
  pthread_exit(NULL);
}

//funcao principal do programa
int main(int argc, char* argv[]) 
{
  //variaveis para calculo de tempo de execucao
  double iniI, iniF, iniD;
  double multI, multF, multD;
  double finI, finF, finD;

  GET_TIME(iniI);

  int tam; //tamanho da entrada
  pthread_t *tid; //identificadores de threads
  t_Args *args;
  
  if(argc<3)
  {
      printf("Digite: %s <dimensao da matriz> <numero de threads>\n", argv[0]);
      return 1;
  }
  tam = atoi(argv[1]);
  nthreads = atoi(argv[2]);
  if (nthreads > tam)
    nthreads = tam;
    
  printf("tam: %d, nthreads: %d\n", tam, nthreads);
  
  //alocacao de memoria para matrizes
  mat1 = (float *) malloc(sizeof(float) * tam * tam);
  if (mat1 == NULL){
      printf("Erro--malloc\n");
      return 2;}
  mat2 = (float *) malloc(sizeof(float) * tam * tam);
  if (mat2 == NULL)  {
      printf("Erro--malloc\n");
      return 2;}
  
  saida = (float *) malloc(sizeof(float) * tam * tam);
  if (saida == NULL)  {
      printf("Erro--malloc\n");
      return 2;}
  
  //inicializacao das matrizes
  for(int i =0; i<tam; i++)
  {
      for(int j=0; j<tam; j++)
      {
          mat1[i*tam+j] = VALORINICIAL;
          mat2[i*tam+j] = VALORINICIAL;
          saida[i*tam+j] = 0;
      }
  }
  
  //alocacao de memoria para threads
  tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
  if (tid == NULL)  {
      printf("Erro--malloc\n");
      return 2;}
  args = (t_Args*) malloc(sizeof(t_Args)*nthreads);
  if (args == NULL)  {
      printf("Erro--malloc\n");
      return 2;}
  GET_TIME(iniF);
  GET_TIME(multI);
  
  //criacao das threads
  for(int i = 0; i < nthreads; i++)
  {
      (args+i)->id = i;
      (args+i)->tam = tam;
      if(pthread_create(tid+i, NULL, tarefa, (void*) (args+i))){
          puts("Erro-- thread creation");
          return 3;
      }
  }
  
  //aguarda finalizacao das threads
  for(int i = 0; i < nthreads; i++)
  {
      pthread_join(*(tid+i), NULL);
  }
  
  GET_TIME(multF);
  GET_TIME(finI);
  
  //verifica resultados
  for(int i = 0; i < nthreads; i++)
  {
      for(int j = 0; j < nthreads; j++)
      {
        if(saida[i*tam + j] != VALORINICIAL*VALORINICIAL*tam)
        {
            puts("Erro-- saida invalida");
            return 4;
        }
      }
  }
  printf("Valores validos.\n");
  
  //libera memoria
  free(mat1);
  free(mat2);
  free(saida);
  free(args);
  free(tid);

  GET_TIME(finF);
  iniD = iniF - iniI;
  multD = multF - multI;
  finD = finF - finI;

  printf("Tempo inicializacao: %lf\n", iniD);
  printf("Tempo multiplicacao: %lf\n", multD);
  printf("Tempo finalizacao: %lf\n", finD);
  printf("Tempo total: %lf\n", iniD + multD + finD);
  
  return 0;
}