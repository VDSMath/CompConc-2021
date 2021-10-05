/* Disciplina: Computacao Concorrente */
/* Nome: Matheus Vinicius da Silva de Figueiredo */
/* DRE: 116023504 */
/* Laboratorio 9 */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_MAX 10

sem_t bufCheio, bufVazio; //semaforos
pthread_mutex_t mutex;
int buffer = 0; //recurso compartilhado

//funcoes executadas pelas threads
void *Produtor (void *arg)  //funcao para produtor, mais de um produtor pode acessar entre maximizacoes do buffer
{
	int *id = (int *) arg;
	sem_post(&bufVazio);
  while(1)
  {
		sem_wait(&bufVazio);
		pthread_mutex_lock(&mutex);
		if(buffer < BUFFER_MAX)
		{
			buffer++;
			printf("Prod%d: Buffer Incrementado - %d\n", *id, buffer);
			sem_post(&bufVazio); //permite qualquer produtor incrementar recurso
			pthread_mutex_unlock(&mutex);
		}
		else
		{
			pthread_mutex_unlock(&mutex); //aguarda consumo do recurso
			sem_post(&bufCheio);
		}
	}
	
  pthread_exit(NULL);
}
void *Consumidor (void *arg) //funcao para consumidor
{
	int *id = (int *) arg;
	while(1)
	{	
		pthread_mutex_lock(&mutex);
		if(buffer < BUFFER_MAX)
		{
			pthread_mutex_unlock(&mutex);
			sem_wait(&bufCheio); //espera buffer encher
		}
		else //consume buffer e libera producao
		{
			buffer = 0;
			printf("Cons%d: Buffer Esvaziado.\n", *id);
			sem_post(&bufVazio);
			pthread_mutex_unlock(&mutex);
		}		
	}
  
  pthread_exit(NULL);
}

//funcao principal do programa
int main(int argc, char* argv[]) 
{
  pthread_t *tid; //identificadores de threads
	int *ids;
	
	//quantidades de consumidores e produtores
  int nConsum = 3;
  int nProdut = 3;
  int nthreads = nConsum + nProdut;
  
  //alocacao de memoria
  tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
  if (tid == NULL)  {printf("Erro--malloc\n"); return 2;}
	ids = (int*) malloc(sizeof(int)*nthreads);
	if (ids == NULL)  {printf("Erro--malloc\n"); return 2;}
      
  //inicia os semaforos
  sem_init(&bufCheio, 0, 0);
  sem_init(&bufVazio, 0, 0);
  pthread_mutex_init(&mutex, NULL);
  
  //inicia threads
  int i,j;
  for(i = 0; i < nProdut; i++)
  {
		ids[i] = i+1;
		if(pthread_create(tid+i, NULL, Produtor, (void *) (ids+i))){
			puts("Erro-- thread creation");
			return 3;}  
  }
  for(j = 0; j < nConsum; j++)
  {
		ids[i+j] = j+1;
		if(pthread_create(tid+(j+i), NULL, Consumidor, (void *) (ids+(i+j)))){
			puts("Erro-- thread creation");
			return 3;}  
  }

  //espera termino das threads
  for(i = 0; i < nthreads; i++)
  {
      pthread_join(*(tid+i), NULL);
  }
  
  free(tid);
  
  return 0;
}

