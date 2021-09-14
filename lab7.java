/* Disciplina: Computacao Concorrente */
/* Nome: Matheus Vinicius da Silva de Figueiredo */
/* DRE: 116023504 */
/* Programa baseado no disponibilizado pela professora. */

import java.util.concurrent.atomic.AtomicInteger;


class LE {
  private int leit, escr;  
  
  // Construtor
  LE() { 
     this.leit = 0; 
     this.escr = 0; 
  } 
  
  // Entrada para leitores
  public synchronized void EntraLeitor (int id) {
    try { 
      if (this.escr > 0) {
         wait();  //bloqueia pela condicao logica da aplicacao 
      }
      this.leit++; 
    } catch (InterruptedException e) { }
  }
  
  // Saida para leitores
  public synchronized void SaiLeitor (int id) {
     this.leit--; 
     if (this.leit == 0) 
           this.notify();
  }
  
  // Entrada para escritores
  public synchronized void EntraEscritor (int id) {
    try { 
      if ((this.leit > 0) || (this.escr > 0)) {
         wait();  //bloqueia pela condicao logica da aplicacao 
      }
      this.escr++;
    } catch (InterruptedException e) { }
  }
  
  // Saida para escritores
  public synchronized void SaiEscritor (int id) {
     this.escr--;
     notify();
  }
}

class Leitor extends Thread {
  int id; 
  int delay;
  LE monitor; //objeto monitor para coordenar a lógica de execução das threads

  Leitor (int id, int delayTime, LE m) {
    this.id = id;
    this.delay = delayTime;
    this.monitor = m;
  }

  // Método executado pela thread
  public void run () {
    boolean isPrime;
    try {
        this.monitor.EntraLeitor(this.id);

        // Verifica se a variavel compartilhada é prima
        isPrime = true;
        for(int divisor = 2; divisor <= lab7.var.get() / 2; divisor++) {
            if (lab7.var.get() % divisor == 0) {
                isPrime = false;
            }
        }
        if(isPrime)
          System.out.println ("Thread L " + this.id + ": Variavel com valor " + lab7.var.get() + " e prima.");
        else
          System.out.println ("Thread L " + this.id + ": Variavel com valor " + lab7.var.get() + " nao e prima.");   
      
        this.monitor.SaiLeitor(this.id);
        
      sleep(this.delay);
    } catch (InterruptedException e) { return; }
  }
}

class Escritor extends Thread {
  int id; 
  int delay; 
  LE monitor; //objeto monitor para coordenar a lógica de execução das threads

  Escritor (int id, int delayTime, LE m) {
    this.id = id;
    this.delay = delayTime;
    this.monitor = m;
  }

  // Método executado pela thread
  public void run () {
    try {
        this.monitor.EntraEscritor(this.id); 
        System.out.println ("Thread E " + this.id + ": Valor inicial: " + lab7.var.get());
        
        // Escreve novo valor na váriavel compartilhada
        lab7.var.set(this.id);

        System.out.println ("Thread E " + this.id + ": Novo valor da variavel: " + lab7.var.get());

        this.monitor.SaiEscritor(this.id); 
        sleep(this.delay);
      
    } catch (InterruptedException e) { return; }
  }
}


class LeitorEscritor extends Thread {
  int id; 
  int delay; 
  LE monitor; //objeto monitor para coordenar a lógica de execução das threads

  // Construtor
  LeitorEscritor (int id, int delayTime, LE m) {
    this.id = id;
    this.delay = delayTime;
    this.monitor = m;
  }

  // Método executado pela thread
  public void run () {
    boolean isPar = true;
    try {
        this.monitor.EntraEscritor(this.id); 
        System.out.println ("Thread LE " + this.id + ": Valor inicial: " + lab7.var.get());
        
        // Verifica se variável compartilhada é par
        if(lab7.var.get() % 2 != 0)
          isPar = false;  

        if(isPar)
          System.out.println ("Thread LE " + this.id + ": Variavel com valor " + lab7.var.get() + " e par.");
        else
          System.out.println ("Thread LE " + this.id + ": Variavel com valor " + lab7.var.get() + " e impar.");

        // Dobra valor atual da váriavel compartilhada
        lab7.var.set(lab7.var.get()*2) ;
      
        System.out.println ("Thread LE " + this.id + ": Novo valor da variavel: " + lab7.var.get());
     
        this.monitor.SaiEscritor(this.delay); 
        sleep(this.delay);
      
    } catch (InterruptedException e) { return; }
  }
}

class lab7 {
  static final int L = 4;
  static final int E = 6;
  static final int LE = 7;

  // Váriavel compartilhada pelas threads
  public static AtomicInteger var = new AtomicInteger(0);

  public static void main (String[] args) {
    int i;
    LE monitor = new LE();            // Monitor (objeto compartilhado entre leitores e escritores)
    Leitor[] l = new Leitor[L];       // Threads leitoras
    Escritor[] e = new Escritor[E];   // Threads escritoras
    LeitorEscritor[] le = new LeitorEscritor[LE]; // Threads leitoras e escritoras

    
    for (i=0; i<L; i++) {
       l[i] = new Leitor(i+1, (i+1)*500, monitor);
       l[i].start(); 
    }
    for (i=0; i<E; i++) {
       e[i] = new Escritor(i+1, (i+1)*500, monitor);
       e[i].start(); 
    }
    for (i=0; i<LE; i++) {
      le[i] = new LeitorEscritor(i+1, (i+1)*500, monitor);
      le[i].start(); 
    }

    for (i=0; i<l.length; i++) {
        try { l[i].join(); } catch (InterruptedException ex) { return; }
    }
    for (i=0; i<e.length; i++) {
        try { e[i].join(); } catch (InterruptedException ex) { return; }
    }
    for (i=0; i<le.length; i++) {
        try { le[i].join(); } catch (InterruptedException ex) { return; }
    }

   System.out.println("Valor final da variavel: " + var.get());
  }
}
