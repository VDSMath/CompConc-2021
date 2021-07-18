/* Disciplina: Computacao Concorrente */
/* Nome: Matheus Vinicius da Silva de Figueiredo */
/* DRE: 116023504/
/* Laboratorio 1 - Atividade 5*/

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

#define NTHREADS  2 //threads criadas
#define TAMVETOR 10000
#define VALORINICIAL 10

//struct passada para as threads, posicao inicial no vetor e o vetor proprio modificado
typedef struct {
   int ini, *vet;
} t_Args;

//funcao executada pelas threads
void *VetorQuadrado (void *arg) 
{
  t_Args *args = (t_Args *) arg;
  int fim = args->ini + (TAMVETOR/2)-1;
  printf("--Thread de %d a %d\n", args->ini, fim);
  int j;
  for(j = args->ini; j <= fim; j++)
  {
      args->vet[j] *= args->vet[j];
  }
  free(args);
  pthread_exit(NULL);
}

//funcao principal do programa
int main() 
{
  pthread_t tid_sistema[NTHREADS]; //identificadores das threads no sistema
  int i; //variavel auxiliar
  //posicao inicial para as threads
  int pri = 0; 
  int seg = TAMVETOR/2; 
  //vetor modificado pelas threads
  int *vetor = (int*)malloc((TAMVETOR)*sizeof(int));
  t_Args *arg;
  
  //inicializacao do vetor, valores simples
  for(i = 0; i < TAMVETOR; i++)
  {
      vetor[i] = VALORINICIAL; 
  }
  
  //cria a thread 1
  arg = malloc(sizeof(t_Args));
  if (arg == NULL) {
    printf("--ERRO: malloc()\n"); exit(-1);
  }
  arg->ini = pri; 
  arg->vet = vetor; 
  if (pthread_create(&tid_sistema[0], NULL, VetorQuadrado, (void*) arg)) {
  printf("--ERRO: pthread_create()\n"); exit(-1);
  }
  arg = malloc(sizeof(t_Args));
  if (arg == NULL) {
    printf("--ERRO: malloc()\n"); exit(-1);
  }
  arg->ini = seg; 
  arg->vet = vetor;
  
  //cria a thread 2
  if (pthread_create(&tid_sistema[1], NULL, VetorQuadrado, (void*) arg)) {
  printf("--ERRO: pthread_create()\n"); exit(-1);
  }

  //espera as threads terminarem
  for (i=0; i<NTHREADS; i++) 
  {
    if (pthread_join(tid_sistema[i], NULL)) 
    {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
    } 
  }
  printf("--Threads finalizadas\n");
  
  //verifica se valores foram modificados
  for(i = 0; i < TAMVETOR; i++)
  {
      if(vetor[i] != VALORINICIAL*VALORINICIAL)
      {
        printf("--Valor invalido");
        exit(-2);  
      } 
  }
  
  printf("--Valores validos\n");

  printf("--Thread principal finalizada\n");
  pthread_exit(NULL);
}
