/* Disciplina: Computacao Concorrente */
/* Nome: Matheus Vinicius da Silva de Figueiredo */
/* DRE: 116023504 */
/* Laboratorio 4 - Atividade 4*/

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

int x = 0;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;

//struct passada para as threads, posicao identificador e texto a ser imprimido
typedef struct {
   int id;
   char *texto;
} t_Args;

//funcoes executadas pelas threads
void *A (void *arg)  //primeira funcao a ser terminada
{
  t_Args *args = (t_Args *) arg;
  
  pthread_mutex_lock(&x_mutex);
  printf("%s\n", args->texto, args->id);
  x++;
  pthread_cond_signal(&x_cond);
  pthread_mutex_unlock(&x_mutex); 
  
  pthread_exit(NULL);
}
void *B (void *arg) //segunda e terceira funcoes a serem terminadas pela condicao de espera
{
  t_Args *args = (t_Args *) arg;
  pthread_mutex_lock(&x_mutex);
  while (x < 1 ) { 
     pthread_cond_wait(&x_cond, &x_mutex);
  }
  printf("%s\n", args->texto, args->id);
  x++;
  pthread_cond_signal(&x_cond);
  pthread_mutex_unlock(&x_mutex); 
  
  pthread_exit(NULL);
}
void *C (void *arg) //ultima funcao a ser terminada
{
   t_Args *args = (t_Args *) arg;

  pthread_mutex_lock(&x_mutex);
  while (x < 3 ) { 
     pthread_cond_wait(&x_cond, &x_mutex);
  }
  printf("%s\n", args->texto, args->id);
  x++;
  pthread_mutex_unlock(&x_mutex); 
  
  pthread_exit(NULL);
}

//funcao principal do programa
int main(int argc, char* argv[]) 
{
  pthread_t *tid; //identificadores de threads
  t_Args *args;
  int nthreads = 4;
  
  tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
  if (tid == NULL)  {
      printf("Erro--malloc\n");
      return 2;}
  args = (t_Args*) malloc(sizeof(t_Args)*nthreads);
  if (args == NULL)  {
      printf("Erro--malloc\n");
      return 2;}
      
  char **frases;
  frases = (char**) malloc(sizeof(char)*nthreads*80);
  frases[0] = "Fique a vontade.";
  frases[1] = "Seja bem-vindo!";
  frases[2] = "Volte sempre!";
  frases[3] = "Sente-se por favor.";
    
  pthread_mutex_init(&x_mutex, NULL);
  pthread_cond_init (&x_cond, NULL);
     
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

  
  for(int i = 0; i < nthreads; i++)
  {
      pthread_join(*(tid+i), NULL);
  }
  
  free(args);
  free(tid);
  
  return 0;
}
