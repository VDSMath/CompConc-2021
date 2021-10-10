/* Disciplina: Computacao Concorrente */
/* Nome: Matheus Vinicius da Silva de Figueiredo */
/* DRE: 116023504 */
/* Trabalho 2 */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <semaphore.h>

#define BUFFERMAX 10 //numero maximo de blocos tamanho N no buffer

sem_t bufCheio, bufVazio; //semaforos
pthread_mutex_t mutex;
int *buffer; //recurso compartilhado pelos produtores/consumidores
int bufferQuant = 0; //contador de recursos disponibilizados no buffer pelo produtor
int done = 0; //booleana usada para fechar consumidores/escritores
FILE *fentrada, *fsaida; //arquivos de entrada e saida
int C,N;


/* Descricao das funcoes:
 * 
 * - comparador -  Funcao de comparacao entre dois inteiros usada na ordenacao feita pelos consumidores.
 * - Produtor - Funcao usada pela thread produtora, le o arquivo de entrada e armazena blocos no buffer
 * se o mesmo nao estiver cheio, entrando em espera se estiver
 * - Consumidor - Funcao usada pelas threads consumidoras, le o buffer se o mesmo nao estiver vazio,
 * entrando em espera se estiver. Realizada a ordenacao de acordo com a restricao do problema 
 * dos escritores (Apenas um escritor por vez) e escreve o resultado no arquivo de saida
 * 
 * Entrada/saida no buffer segue estrutura de pilha.
*/ 

int comparador(const void *a, const void *b)
{
  return ( *(int*)a - *(int*)b );
}
//funcoes executadas pelas threads
void *Produtor (void *arg)  //funcao para produtor
{
	int *quant = (int *) arg; //total de blocos de tamanho N no arquivo de entrada
  int i = 0;
  int j;
  sem_post(&bufVazio); //liberacao inicial do semaforo de exclusao
  while(1)
  {
    sem_wait(&bufVazio); //aguarda espaco para escrever no buffer
		pthread_mutex_lock(&mutex);
		if(i < *quant) //verifica se todos blocos do arquivo ja foram lidos
		{
			//le um bloco de tamanho n do arquivo de entrada
      bufferQuant++;
      for(j = 0; j < N; j++)
      {
        fscanf(fentrada, "%d", &buffer[(bufferQuant*N) + j]);
      }
			printf("Buffer Incrementado - %d bloco\n", bufferQuant);
			sem_post(&bufCheio); //permite execucao de um consumidor
      if(bufferQuant < BUFFERMAX) //se o buffer nao estiver cheio, permite outra execucao do produtor
        sem_post(&bufVazio);
			pthread_mutex_unlock(&mutex);
      i++;
		}
		else //se todos blocos forem lidos, sinaliza a booleana de finalizacao e termina execucao
		{
      done = 1;
			pthread_mutex_unlock(&mutex); //aguarda consumo do recurso
      break;
		}
	}
	printf("Produtor terminado.\n");
  pthread_exit(NULL);
}
void *Consumidor (void *arg) //funcao para consumidor, apenas um consumidor por vez pode alterar
														 //o arquivo de saida, conforme restricao dos escritores
{
	int *id = (int *) arg;
	
	//buffer temporario para ordenacao de um bloco tamanho N
  int *temp;
  temp = (int*) malloc(sizeof(int)*N);
  if (temp == NULL)  {printf("Erro--malloc\n");}
  
  int j;
	while(1)
	{	
		pthread_mutex_lock(&mutex);
		printf("Consumidor %d: %d blocos no buffer, Done: %d\n",*id, bufferQuant, done);
		if(bufferQuant > 0) //le um dos blocos do buffer se existerem um ou mais
		{
			//armazena valor do buffer no vetor temporario
      for(j = 0; j < N; j++)
      {
        temp[j] = buffer[(N*bufferQuant)+j];
      }
      qsort((void*)temp, N, sizeof(int), comparador); //ordena vetor temporario
      printf("Vetor temporario ordenado\n");
      //escreve vetor ordenado no arquivo de saida
      for(j = 0; j < N-1; j++)
      {
        fprintf(fsaida, "%d ", *(temp+j));
      }
      fprintf(fsaida, "%d\n", *(temp+j));
      bufferQuant--;
      
      sem_post(&bufVazio); //sinaliza vaga no buffer, liberando produtor se buffer previamente cheio
      pthread_mutex_unlock(&mutex);
		}
		else if(done) //se nao houverem blocos no buffer e a flag de termino foi ativada, termina thread
		{
			sem_post(&bufCheio); //sinaliza outros consumidores pausados
			pthread_mutex_unlock(&mutex);
      break;
		}
		else //se nao houverem blocos mas a flag nao foi ativada, entra em espera 
    {  
      printf("Buffer Vazio.\n");
			pthread_mutex_unlock(&mutex);
			sem_wait(&bufCheio); //espera buffer
		}		
	}
  printf("Consumidor %d terminado.\n", *id);
  pthread_exit(NULL);
}

//funcao principal do programa
int main(int argc, char* argv[]) 
{	
  pthread_t *tid; //identificadores de threads
  int *ids;
  int totalNums; //primeiro numero do arquivo de entrada
  char *entrada, *saida; //nomes dos arquivos

  if(argc<4)
  {
      printf("Digite: %s <numero de threads> <tamanho do bloco> <arquivo de entrada> <arquivo de saida>\n", argv[0]);
      return 1;
  }
  C = atoi(argv[1]);
  N = atoi(argv[2]);
  entrada = argv[3];
  saida = argv[4];
  
  //abre arquivos de entrada e saida
  if ((fentrada = fopen(entrada,"r")) == NULL){printf("Erro-file\n"); return(3);}
  if ((fsaida = fopen(saida,"w")) == NULL){printf("Erro-file\n"); return(3);}

  //le primeiro numero do arquivo
  fscanf(fentrada, "%d", &totalNums);

  //alocacao de memoria
  tid = (pthread_t*) malloc(sizeof(pthread_t)*(C+1));
  if (tid == NULL)  {printf("Erro--malloc\n"); return 2;}
  ids = (int*) malloc(sizeof(int)*(C+1));
  if (ids == NULL)  {printf("Erro--malloc\n"); return 2;}
  buffer = (int*) malloc(sizeof(int)*totalNums);
  if (buffer == NULL) {printf("Error--malloc\n"); return 2;}
      
  //inicia os semaforos
  sem_init(&bufCheio, 0, 0);
  sem_init(&bufVazio, 0, 0);
  pthread_mutex_init(&mutex, NULL);

  //inicia threads produtora e consumidora(s)
  int i = 0;
  ids[i] = totalNums/N;
  if(pthread_create(tid+i, NULL, Produtor, (void *) (ids+i))){
      puts("Erro-- thread creation");
      return 3;}  
  
  for(i = 1; i < C+1; i++)
  {
      ids[i] = i;
      if(pthread_create(tid+i, NULL, Consumidor, (void *) (ids+i))){
          puts("Erro-- thread creation");
          return 3;}  
  }
  
  //aguarda termino das threads
  for(i = 0; i < C+1; i++)
  {
      pthread_join(*(tid+i), NULL);
  }

  puts("Fim de execucao.");
  
  free(tid);
  fclose(fentrada);
  fclose(fsaida);

  return 0;
}

