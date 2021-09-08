/* Disciplina: Computacao Concorrente */
/* Nome: Matheus Vinicius da Silva de Figueiredo */
/* DRE: 116023504 */
/* Laboratorio 6 - Atividade 5 */

import java.util.Random;

//classe compartilhada entre as threads
class S {
    private int[] v;
    private int pares;

    //construtor
    public S(int[] vet) 
    { 
       this.v = vet; 
       pares = 0;
    }
    //retorna true se a posicao i for par
    public boolean isEven(int i) 
    { 
      return this.v[i] % 2 == 0; 
    }
    public synchronized void incrementaPares(int p)
    {
      pares += p;
    }
    public synchronized int get()
    {
      return pares;
    }   
  }
  
  class T extends Thread {
     //identificador, posicoes no vetor e contador de pares
     private int id, start, end, pares;
     
     S s;
    
     //construtor
     public T(int tid, S s, int st, int en) 
     { 
        this.id = tid; 
        this.s = s;
        this.start = st;
        this.end = en;
        this.pares = 0;
     }
  
     //metodo main da thread, conta numeros pares entre as posicoes no vetor dadas
     public void run() 
     {
        System.out.println("Thread " + this.id + " iniciou.");
        for (int i=start; i<end; i++) {
           if(this.s.isEven(i))
               this.pares++;
        }
        this.s.incrementaPares(this.pares);
        System.out.println("Thread " + this.id + " terminou: " + this.pares + " pares."); 
     }
  }
  
  //classe da aplicacao
  class lab6 {
     //numeros de thread e tamanho do vetor
     static final int N = 4;
     static final int TAM = 100;
  
     public static void main (String[] args) {
        //reserva espaço para um vetor de threads
        Thread[] threads = new Thread[N];
  
        //cria vetor de inteiros com numeros aleatorios entre 0 e tamanho maximo
        int[] vetor = new Random().ints(TAM, 0, TAM).toArray();

        //cria uma instancia do recurso compartilhado entre as threads
        S s = new S(vetor);

        int incremento = TAM/N;
        //cria as threads da aplicacao
        for (int i=0; i<threads.length; i++) 
        {
           threads[i] = new T(i, s, incremento*i, incremento*(i+1));
        }
  
        //inicia as threads
        for (int i=0; i<threads.length; i++) 
        {
           threads[i].start();
        }
  
        //espera pelo termino de todas as threads
        for (int i=0; i<threads.length; i++) 
        {
           try { threads[i].join(); } catch (InterruptedException e) { return; }
        }
        //funcao de verificacao
        int ver = 0;
        for(int i=0; i<TAM; i++)
        {
            if(vetor[i] % 2 == 0)
               ver++;
        }
        if(ver == s.get())
         System.out.println("Numero de pares valido.");
        else
        {
         System.out.println("Numero de pares invalido.");
         return;
        }
        System.out.println("Pares: " + s.get() + "."); 
     }
  }
  