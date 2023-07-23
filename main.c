#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// VARIÁVEIS COM VALORES ALTERÁVEIS:*------
// MAX_THREADS = Quantidade de threads que podem ser usadas
// TEMPOREQ = Tempo entre a leitura de uma requisição e outra
// NUMREQ = Quantidade de requisições a serem calculadas
#define MAX_THREADS 3
#define TEMPOREQ 100
#define NUMREQ 30
// ---------------------------------*------

// Inicializa as funções basicas do pragrama
void *dispatcher(void *arg);
long double calcPI(int i, int tempoEspera);
void *execThread(void *arg);
void espera(float segundos);
int criaInput();
int criaArquivosOutput();

// Inicializa um ponteiro para o arquivo de entrada e saida de dados
FILE *output;

// Cria as threads necessárias:
pthread_t thread_dispatcher;
pthread_t workerThreads[MAX_THREADS];

// Mutex e Cond
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t dataAvailableCondition = PTHREAD_COND_INITIALIZER;
int existeDados = 0;
int usandoWorkerThread=0;
char sharedMemory[100];

// contador da quantidade de linhas lidas
int qtdLinhas = 0 ;
int ordemThread = 1;

// Struct que aramazena as informações da requisicao
struct requsicao {
  char *caminho;
  int numReq;
  int numThreads;
  int tempoReq;
};

// Struct que armazena os valores lidos do arquivo input.txt
struct itensArquivoTxt {
  int i;
  int tempoEspera;
};

int main() {
  // verifica se o número de threads é valido:
  if(MAX_THREADS <=0) {
    printf("*****************************************\n");
    printf("Número de Threads para execução inválida!\n");
    printf("*****************************************\n");
    return 1;
  }

  if(NUMREQ <= 0) {
    printf("*********************************************\n");
    printf("Número de Requisições para execução inválida!\n");
    printf("*********************************************\n");
    return 1;
  }

  if(TEMPOREQ <= 0) {
    printf("********************************************\n");
    printf("Tempo de leitura entre requisições inválido!\n");
    printf("********************************************\n");
    return 1;
  }

  // Cria o arquivo de entrada de requisições
  printf("Criando arquivo de requisições...\n");
  criaInput();

  // seta uma seed para o tempo
  srand(time(NULL));

  // cria a struct que leva a requisicao geral para a dispatcher:
  struct requsicao requsicao;

  // Cria um arquivo "output.txt" para guardar a saída do resultado das threads
  output = fopen("output.txt", "w");
  if (output == NULL) {
    printf("Erro ao abrir o arquivo de saída.\n");
    return 1;
  }

  // Configura a struct requsicao
  requsicao.caminho = "input.txt";
  requsicao.numReq = NUMREQ;
  requsicao.numThreads = MAX_THREADS;
  requsicao.tempoReq = TEMPOREQ;

  printf("Iniciando execução...\n");
  
  // cria a thread dispatcher
  pthread_create(&thread_dispatcher, NULL, (void *)dispatcher, &requsicao);

  // Cria as threads trabalhadoras:
  for(int i=0 ; i<requsicao.numThreads ; i++) {
    pthread_create(&workerThreads[i], NULL, (void *)execThread, NULL);
  }

  pthread_join(thread_dispatcher, NULL);

  // distribui os resultados do arquivo output para os arquivos de cada thread
  printf("Distribuindo arquivos de saída...\n");
  criaArquivosOutput();

  printf("Fim do programa...\n");
  return 0;
}

long double calcPI(int i, int tempoEspera) {
  long double pi = 0.0; // guarda o valor de pi
  double denominador;
  long double valor;
  int k;

  for (k = 0; k < 100; k++) {
    valor = (1.0 / pow(16, k)) * ((4.0 / (8 * k + 1)) - (2.0 / (8 * k + 4)) - (1.0 / (8 * k + 5)) - (1.0 / (8 * k + 6)));
    pi += valor;
  }

  long double multiplicador = pow(10, i);
  pi *= multiplicador;
  pi = roundl(pi) / multiplicador;
  
  //  espera o tempoespera
  espera((tempoEspera / 1000));

  //printf("valor: %Lf\n",pi);
  
  return pi;
}

void *execThread(void *arg) {
  while(1) {
    pthread_mutex_lock(&mutex);
    while (!existeDados) {
      if(usandoWorkerThread = 0) {
        pthread_mutex_unlock(&mutex);
        pthread_exit(NULL);
      }
      pthread_cond_wait(&dataAvailableCondition,&mutex); // Aguarda por dados disponíveis
    }

     // Guarda na struct o valor de i e tempo espera
    struct itensArquivoTxt ItensLinha;
    int n = sscanf(sharedMemory, "%d;%d", &ItensLinha.i, &ItensLinha.tempoEspera);
    
    // Pega o TID da thread atual:
    pthread_t tid = pthread_self();
    //printf("TID: %li\n", tid);
    int i = ItensLinha.i;
    int tempoEspera = ItensLinha.tempoEspera;
    
    
    // Realiza o cálculo de PI
    long double PI = calcPI( i , tempoEspera );

    fprintf(output, "%li | %.*Lf | %i | %i | %i \n", tid , i , PI , ordemThread , ItensLinha.i , ItensLinha.tempoEspera);
    
    //printf("Linha [%i] lida: %i ; %i ; ordem: %i\n", qtdLinhas , ItensLinha.i , ItensLinha.tempoEspera , ordemThread);
    ordemThread++;
    existeDados = 0; // Indica que os dados foram lidos
    pthread_mutex_unlock(&mutex);
    printf("Realizando cálculos...\n");
  }  
  pthread_exit(NULL);
}

void *dispatcher(void *arg) {
  FILE *arquivo;
  struct requsicao *requisicao = (struct requsicao *)arg;
  
  // Abre o arquivo em modo de leitura
  arquivo = fopen(requisicao->caminho, "r");

  if (arquivo == NULL) {
    printf("Erro ao abrir o arquivo.\n");
    return NULL;
  }

  // Lê cada linha do arquivo até encontrar o final do arquivo (EOF)
  while (fgets(sharedMemory, sizeof(sharedMemory), arquivo) != NULL) {
    
    // Usando MUTEX para garantir a comunicação entre threads
    usandoWorkerThread = 1;
    pthread_mutex_lock(&mutex);
    existeDados = 1; // Sinaliza que há dados disponíveis
    pthread_cond_signal(&dataAvailableCondition); // Notifica a thread trabalhadora
    pthread_mutex_unlock(&mutex);

    // Espera um determinado temporeq
    espera(3);
    
    ++qtdLinhas;
    
    // A fim de garantir que todas as thread sejam executadas sem que a thread dispatcher encerre
    // Eu espero um tempo a mais para garantir a execução completa
    if(qtdLinhas == NUMREQ) {
      // Espera um tempo a mais antes de encerrar a thread:
      unsigned int retTime = time(0) + 3;
      while (time(0) < retTime);
    }    
  }
  // Fecha o arquivo de input
  fclose(arquivo);

  existeDados = 0; // Indica que não há mais dados disponíveis
  
  pthread_cond_signal(&dataAvailableCondition); // Notifica a thread trabalhadora para encerrar
  
  pthread_mutex_unlock(&mutex);

  // Fecha o arquivo output:
  fclose(output);

  pthread_exit(NULL);
  
  return 0;
}

void espera(float segundos) {
    float tempo_espera = (float)clock() / CLOCKS_PER_SEC + segundos;

    while ((float)clock() / CLOCKS_PER_SEC < tempo_espera) {
        // Espera até o tempo específico ser alcançado
    }
}

int criaInput() {
  srand(time(NULL));
  
  FILE* arquivoInput = fopen("input.txt", "w");
  if(arquivoInput == NULL) {
    printf("Erro ao abrir 'input.txt'\n...\n");
    return 0;
  }

  for(int j=0 ; j < NUMREQ ; j++ ) {
    // Gera o valor de i:
    int i = 10 + rand() % (90 + 1);

    // Gera o valor de tempoEspera:
    int tempoEspera = 500 + rand() % (1000 + 1);

    if(j == NUMREQ-1) {
      fprintf(arquivoInput , "%i;%i" , i , tempoEspera);
    } else {
      fprintf(arquivoInput , "%i;%i\n" , i , tempoEspera);
    }

    //printf("%i;%i\n" , i , tempoEspera);
  }

  fclose(arquivoInput);
  
}

int criaArquivosOutput(){
  // Abre o arquivo de saida em modo de leitura
  char linha[300*NUMREQ];
  FILE* arquivo = fopen("output.txt", "r");
  if (arquivo == NULL) {
    printf("Erro ao abrir o arquivo 'output.txt'.\n");
    return 1;
  }

  // variáveis que armazenam o TID , valor de PI , ordem de execucao , i , tempoespera
  char tid[1000];
  char pi[1001];
  char ordemSaida[10];
  char valorI[10];
  char valorTempoEspera[20];

  int qtdeLinhasLidas = 1;

  FILE* arquivoSaidaEspecifico;
  
  // Lê cada linha do arquivo até encontrar o final do arquivo (EOF)
  while (fgets(linha, sizeof(linha), arquivo) != NULL) {
    // Cria o texto para abrir/criar um arquivo de thread e
    int n = sscanf(linha, "%s | %s | %s | %s | %s \n", tid, pi , ordemSaida , valorI , valorTempoEspera);
    strcat(tid , ".txt");
    
    arquivoSaidaEspecifico = fopen( tid , "a");
    
    if(arquivoSaidaEspecifico == NULL) {
      printf("Erro ao abrir o arquivo de saída %s\n", tid);
      return 0;
    }

    fprintf(arquivoSaidaEspecifico, "Linha da requisicao: %s ; %s\n", valorI , valorTempoEspera);
    fprintf(arquivoSaidaEspecifico , "> Ordem da Requisicao: %s ; PI: %s\n" , ordemSaida , pi);
    fprintf(arquivoSaidaEspecifico , "--------------------\n");
    fclose(arquivoSaidaEspecifico);
    
    qtdeLinhasLidas++;
  }
  
  fclose(arquivo);
}