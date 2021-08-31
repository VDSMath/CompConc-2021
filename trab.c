/* Disciplina: Computacao Concorrente */
/* Nome: Matheus Vinicius da Silva de Figueiredo */
/* DRE: 116023504 */
/* Trabalho 1 */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include "timer.h"

#define VALORMAX 100

int tam;
int nthreads;
//tipo de ordenacao do vetor a ser ordenado - 0 = aleatorio, 1 = crescente, 2 = decrescente
int ordenacao;

int *vet;

typedef struct {
   int id;
   int ini;
   int fim;
   } t_Args;


void merge(int min, int meio, int max)
{

    // n1 e n2 sao metades criadas pelo merge sort
    int n1 = meio - min + 1;
    int n2 = max - meio;

    int *esquerda = malloc(n1 * sizeof(int));
    int *direita = malloc(n2 * sizeof(int));

    int i;
    int j;

    for (i = 0; i < n1; i++)
        esquerda[i] = vet[i + min];

    for (i = 0; i < n2; i++)
        direita[i] = vet[i + meio + 1];

    int k = min;

    i = j = 0;

    //unindo em ordem crescente
    while (i < n1 && j < n2) {
        if (esquerda[i] <= direita[j])
            vet[k++] = esquerda[i++];
        else
            vet[k++] = direita[j++];
    }

    //valores restantes
    while (i < n1)
        vet[k++] = esquerda[i++];

    while (j < n2)
        vet[k++] = direita[j++];

    free(esquerda);
    free(direita);
}

// merge sort 
void merge_sort(int min, int max)
{

    int meio = min + (max - min) / 2;

    if (min < max) 
    {
        merge_sort(min, meio);

        merge_sort(meio + 1, max);

        merge(min, meio, max);
    }
}

// merge multi thread
void *merge_sort123(void *arg)
{
    t_Args *args = arg;
    int ini;
    int fim;

    ini = args->ini;
    fim = args->fim;


    int meio = ini + (fim - ini) / 2;

    if (ini < fim) {
        merge_sort(ini, meio);
        merge_sort(meio + 1, fim);
        merge(ini, meio, fim);
    }

    return 0;
}


int main(int argc, char **argv)
{
	
    if(argc<4)
		{	
			printf("Digite: %s <tamanho do vetor> <numero de threads> <opcao de ordenacao>\n", argv[0]);
			return 1;
		}
		double tempI, tempF, tempD;
		GET_TIME(tempI);
		tam = atoi(argv[1]);
		nthreads = atoi(argv[2]);
		ordenacao = atoi(argv[3]);
		
    pthread_t tid[nthreads]; //identificadores de threads
		t_Args args[nthreads];

    // aloca memoria
    vet = malloc(sizeof(int) * tam);
    if (vet == NULL){
	  printf("Erro--malloc\n");
	  return 2;}

		//inicializa vetor
	  srand(time(NULL));
		for(int i=0; i<tam; i++)
		{
			switch(ordenacao)
			{
					case 0:
							vet[i] = rand() % VALORMAX;
							break;
					case 1:
							vet[i] = i;
							break;
					case 2:
							vet[i] = tam-i;
							break;
					default:
						puts("Metodo de ordenacao invalida(0,1 ou 2)");
						return 6;
		  }
		}	

    int tamThread = tam/nthreads;

    int inicio = 0;

		t_Args *arg;
    for (int i = 0; i < nthreads; i++, inicio += tamThread) {
        arg = &args[i];
        arg->id = i;

				arg->ini = inicio;
				arg->fim = inicio + tamThread - 1;
				if (i == (nthreads - 1))
						arg->fim = tam - 1;
    }

    for (int i = 0; i < nthreads; i++) {
        arg = &args[i];
        if(pthread_create(&tid[i], NULL, merge_sort123, arg)){
			  puts("Erro-- thread creation");
			  return 3;
		  }
    }

    for (int i = 0; i < nthreads; i++)
        pthread_join(tid[i], NULL);

    // merge final
		t_Args *argm = &args[0];
		for (int i = 1; i < nthreads; i++) 
		{
				t_Args *arg = &args[i];
				merge(argm->ini, arg->ini - 1, arg->fim);
		}
		
		//funcao de verificacao do resultado
		for (int i = 0; i < tam-2; i++)
		{
			if(i > i+i)
			{
				puts("Erro-- ordenacao invalida");
			  return 4;
			}
		}
		puts("Ordenacao valida");
		GET_TIME(tempF);
		tempD = tempF - tempI;
		printf("Tempo total de execucao: %f\n", tempD);

    return 0;
}
