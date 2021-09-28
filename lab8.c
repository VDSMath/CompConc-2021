/* Disciplina: Computacao Concorrente */
/* Nome: Matheus Vinicius da Silva de Figueiredo */
/* DRE: 116023504 */
/* Laboratorio 8 */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <semaphore.h>

sem_t condt2, condt3;     //semaforos
int cont = 0; //contador auxiliar

//struct passada para as threads, identificador e texto a ser imprimido
typedef struct {
   int id;
   char *texto;
} t_Args;

//funcoes executadas pelas threads
void *A (void *arg)  //primeira funcao a ser terminada
{
  t_Args *args = (t_Args *) arg;
  
  printf("%s\n", args->texto);
  sem_post(&condt2); //sinaliza T2 ou T3
  
  pthread_exit(NULL);
}
void *B (void *arg) //segunda e terceira funcoes a serem terminadas pela condicao de espera
{
  t_Args *args = (t_Args *) arg;
  sem_wait(&condt2); //espera T1
  printf("%s\n", args->texto);
  cont++;
  if(cont >= 2)
    sem_post(&condt3); //permite que T4 execute se T2 e T3 estiverem finalizadas
  else
    sem_post(&condt2); //permite a restante de T2 e T3
  
  pthread_exit(NULL);
}
void *C (void *arg) //ultima funcao a ser terminada
{
   t_Args *args = (t_Args *) arg;

  sem_wait(&condt3); //espera T2 e T3
  printf("%s\n", args->texto);
  
  pthread_exit(NULL);
}

//funcao principal do programa
int main(int argc, char* argv[]) 
{
  pthread_t *tid; //identificadores de threads
  t_Args *args;
  int nthreads = 4;
  
  //aloca memoria
  tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
  if (tid == NULL)  {printf("Erro--malloc\n"); return 2;}
  args = (t_Args*) malloc(sizeof(t_Args)*nthreads);
  if (args == NULL)  {printf("Erro--malloc\n"); return 2;}
      
  char **frases;
  frases = (char**) malloc(sizeof(char)*nthreads*80);
  frases[0] = "Fique a vontade.";
  frases[1] = "Seja bem-vindo!";
  frases[2] = "Volte sempre!";
  frases[3] = "Sente-se por favor.";

  //inicia os semaforos
  sem_init(&condt2, 0, 0);
  sem_init(&condt3, 0, 0);
     
  for(int i = 0; i < nthreads; i++)
  {
    (args+i)->id = i;
    (args+i)->texto = frases[i];
    switch(i)
    {
		case 0:
			if(pthread_create(tid+i, NULL, B, (void*) (args+i))){
				puts("Erro-- thread creation");
				return 3;}
				break;
		case 1:
			if(pthread_create(tid+i, NULL, A, (void*) (args+i))){
				puts("Erro-- thread creation");
				return 3;}
				break;
		case 2:
			if(pthread_create(tid+i, NULL, C, (void*) (args+i))){
				puts("Erro-- thread creation");
				return 3;}
				break;
		case 3:
			if(pthread_create(tid+i, NULL, B, (void*) (args+i))){
				puts("Erro-- thread creation");
				return 3;}
				break;
		
	}  
  }

  //espera termino das threads
  for(int i = 0; i < nthreads; i++)
  {
      pthread_join(*(tid+i), NULL);
  }
  
  free(args);
  free(tid);
  
  return 0;
}
