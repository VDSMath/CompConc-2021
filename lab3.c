/* Disciplina: Computacao Concorrente */
/* Nome: Matheus Vinicius da Silva de Figueiredo */
/* DRE: 116023504 */
/* Laboratorio 3 */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include "timer.h"

#define VALORMAX 1000 //valor maximo no vetor
#define NUMEXECUCOES 20 //numero de execucoes para medicao de tempo

float *vet;
float *mat2;
float *saida;
int nthreads;

//struct passada para as threads, posicao inicial e final no vetor
typedef struct {
   int start;
   int finish;
} t_Args;
//struct de retorno das threads
typedef struct {
   float max;
   float min;
} t_Ret;

//funcao executada pelas threads
void *tarefa (void *arg) 
{
	t_Args *args = (t_Args *) arg;
	t_Ret *rets = (t_Ret *) malloc(sizeof(t_Ret));

	rets->max = 0;
	rets->min = (float)VALORMAX;
	for(long long int i=(args->start); i<(args->finish); i++)
	{
		if(vet[i] > rets->max)
			rets->max = vet[i];
		if(vet[i] < rets->min)
			rets->min = vet[i];
	}
	pthread_exit((void *) rets);
}

//funcao principal do programa
int main(int argc, char* argv[]) 
{
	double iniI, iniF, iniD;
	double seqI, seqF, seqD;
	double seqT = 0;
	double concI, concF, concD;
	double concT = 0;

	long long int tam; //tamanho da entrada
	pthread_t *tid; //identificadores de threads
	t_Args *args;

	GET_TIME(iniI);
	if(argc<3)
	{
	  printf("Digite: %s <tamanho do vetor> <numero de threads>\n", argv[0]);
	  return 1;
	}
	tam = atoll(argv[1]);
	nthreads = atoi(argv[2]);

	printf("tam: %lld, nthreads: %d\n", tam, nthreads);
	//alocacao de memoria
	srand(time(NULL));
	vet = (float *) malloc(sizeof(float) * tam);
	if (vet == NULL){
	  printf("Erro--malloc\n");
	  return 2;}

	//inicializa vetor com floats aleatorios entre 0 e VALORMAX
	for(long long int i=0; i<tam; i++)
	{
		vet[i] = (float)rand()/RAND_MAX * VALORMAX;
	}	
	GET_TIME(iniF);
	
	//sequencial
	float max;
	float min;
	for(int e = 0; e < NUMEXECUCOES; e++)
	{
		GET_TIME(seqI);	
		max = 0;
		min = (float)VALORMAX;
		for(long long int i=0; i<tam; i++)
		{
			if(vet[i] > max)
				max = vet[i];
			if(vet[i] < min)
				min = vet[i];
		}
		GET_TIME(seqF);
		seqD = seqF - seqI;
		seqT += seqD;
	}
	
	//concorrente
	tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
	if (tid == NULL)  {
	  printf("Erro--malloc\n");
	  return 2;}
	args = (t_Args*) malloc(sizeof(t_Args)*nthreads);
	if (args == NULL)  {
	  printf("Erro--malloc\n");
	  return 2;}
	
	float cMax;
	float cMin;
	for(int e = 0; e < NUMEXECUCOES; e++)
	{
		GET_TIME(concI); 
		for(int i = 0; i < nthreads; i++)
		{
		  (args+i)->start = i*(tam/nthreads);
		  (args+i)->finish = (i+1)*(tam/nthreads);
		  if(pthread_create(tid+i, NULL, tarefa, (void*) (args+i))){
			  puts("Erro-- thread creation");
			  return 3;
		  }
		}
		
		cMax = 0;
		cMin = (float)VALORMAX;
		
		t_Ret *ret = (t_Ret *) malloc(sizeof(t_Ret));
		for(int i = 0; i < nthreads; i++)
		{
		  pthread_join(*(tid+i), (void**) &ret);
		  if(ret->max > cMax)
				cMax = ret->max;
		  if(ret->min < cMin)
				cMin = ret->min;
		}
		GET_TIME(concF);
		concD = concF - concI;
		concT += concD;
	}
	
	 
	//verifica resultados
	if(cMax != max || cMin != min)
	{
		puts("Erro-- saida invalida");
		return 4;
	}
	printf("Valores validos - max: %f, min: %f\n", max, min);

	free(vet);
	free(args);
	free(tid);

	iniD = iniF - iniI;
	concD = concF - concI;

	printf("Tempo de inicializacao: %lf\n", iniD);
	printf("Tempo sequencial - Total:%lf, Medio:%lf\n", seqT, seqT/NUMEXECUCOES);
	printf("Tempo concorrente - Total:%lf, Medio:%lf\n", concT, concT/NUMEXECUCOES);
	
	return 0;
}
