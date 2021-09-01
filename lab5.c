/* Disciplina: Computacao Concorrente */
/* Nome: Matheus Vinicius da Silva de Figueiredo */
/* DRE: 116023504 */
/* Laboratorio 5 */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include "timer.h"

#define VALORMAX 10

int *vet;
int *ret;
int n;

int contador;
pthread_mutex_t mutex;
pthread_cond_t cond_bar;

//funcao de sincronizacao das threads
void barreira()
{
	pthread_mutex_lock(&mutex);
	contador--;
	if(contador > 0)
	{
		pthread_cond_wait(&cond_bar, &mutex);
	} else {
			contador = n;
			pthread_cond_broadcast(&cond_bar);
		} 
		pthread_mutex_unlock(&mutex);
	
}
//funcao executada pelas threads
void *tarefa (void *arg) 
{
	int *id = (int *) arg;
	printf("Iniciando thread %d\n", *id);
	int *sum = malloc(sizeof(int));
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			*sum += vet[j];
		}
		printf("Soma da iteracao %d da thread %d e %d\n", i, *id, *sum);
		barreira();
		printf("Valor correspondente da thread %d randomizado\n", *id);
		vet[*id] = rand() % VALORMAX;
		barreira();
	}
	ret[*id] = *sum;
	pthread_exit(NULL);
}


//funcao principal do programa
int main(int argc, char* argv[]) 
{
	pthread_t *tid; //identificadores de threads
	int *ids;

	if(argc<2)
	{
	  printf("Digite: %s <tamanho do vetor/numero de threads>\n", argv[0]);
	  return 1;
	}
	n = atoi(argv[1]);
	if(n < 1)
	{
		puts("Digite n maior que 1.");
		return 1;
	}

	//alocacao de memoria
	vet = (int *) malloc(sizeof(int) * n);
	if (vet == NULL){
	  printf("Erro--malloc\n");
	  return 2;}
	ret = (int *) malloc(sizeof(int) * n);
	if (ret == NULL){
	  printf("Erro--malloc\n");
	  return 2;}

	//inicializa vetor
	srand(time(NULL));
	for(int i=0; i<n; i++)
	{
		vet[i] = rand() % VALORMAX;
	}	
	
	//alocacao de memoria
	tid = (pthread_t*) malloc(sizeof(pthread_t)*n);
	if (tid == NULL)  {
	  printf("Erro--malloc\n");
	  return 2;}
	ids = (int*) malloc(sizeof(int)*n);
	if (ids == NULL)  {
	  printf("Erro--malloc\n");
	  return 2;}
	  
	pthread_mutex_init(&mutex, NULL);
  pthread_cond_init (&cond_bar, NULL);
  
	contador = n;
	for(int i = 0; i < n; i++)
	{
		ids[i] = i;
		if(pthread_create(tid+i, NULL, tarefa, (void *) (ids+i))){
			puts("Erro-- thread creation");
			return 3;
		}
	}
			
	int sum = 0;
	int equal = 1; //variavel para verificao dos valores
	pthread_join(*(tid+0), NULL);
	printf("Soma da thread 1 e %d\n", ret[0]);
	sum = ret[0];
	
	int nsum = 0;
	for(int i = 1; i < n; i++)
	{
		pthread_join(*(tid+i), NULL);
		printf("Soma da thread %d e %d\n", i+1, ret[i]);
		nsum = ret[i];
		if(nsum != sum)
			equal = 0;
	}	
	 
	//verifica resultados
	if(!equal)
	{
		puts("Erro-- saida invalida");
		return 4;
	}
	puts("Valores Validos");
	
	free(vet);
	free(ret);
	free(tid);
	
	return 0;
}
