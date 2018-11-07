#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>

int randomizaNumEstudantes();
void criaThreadsEstudantes(pthread_t *, pthread_attr_t *, int *, int );
void *estudante (void *);
int pedeAjuda (int idEstudante);
void sendoAjudado(void);
void acordaMonitor(void);
void *monitor(void *param);

int numCadeiras,cadeirasOcupadas,estudanteAjudado;
pthread_mutex_t  cadeira_mutex;
sem_t  cadeiraAtendimento, monitorDormindo, estudanteSendoAjudado ;


int main (){

    srand(time(NULL));
    //inicia semafaros e mutexs
    sem_init(&cadeiraAtendimento, 0, 1);
    sem_init(&monitorDormindo, 0, 1);
    sem_init(&estudanteSendoAjudado, 0, 1);
    pthread_mutex_init(&cadeira_mutex,NULL);

    sem_wait(&monitorDormindo); //semafaro do assistente esta "fechado"
    sem_wait(&estudanteSendoAjudado); // o estudante esta esperando ser atendido

    pthread_t *estudantes;//thead de estudante
    pthread_attr_t *attrEstudantes; // atributos de estudante
    int *idEstudantes; //id para os estudantes

    pthread_t monitor_tid;
    pthread_attr_t monitor_attr;
	pthread_attr_init(&monitor_attr);
	pthread_create(&monitor_tid,&monitor_attr, monitor, NULL);

    int quantidade = randomizaNumEstudantes(); //randomiza a quantidade de estudantes
    printf("hey2\n");
    numCadeiras = quantidade/2;  //numero de cadeiras dividido por 2
    estudantes = (pthread_t *) malloc(sizeof(pthread_t)*quantidade); // aloca recurso para n estudantes
    attrEstudantes = (pthread_attr_t *) malloc (sizeof (pthread_attr_t)*quantidade); 
    idEstudantes = (int*) malloc(sizeof(int)*quantidade); //aloca recursos para n ids
    criaThreadsEstudantes(estudantes, attrEstudantes, idEstudantes, quantidade); //cria as threads de estudante
    pthread_join(monitor_tid, NULL);

    free(estudantes);
    free(attrEstudantes);
    free(idEstudantes);

}

int randomizaNumEstudantes() //randomiza numero de estudantes
{
    int quantidade;
    quantidade = 2; //(rand()%(38)) + 3;
    return quantidade;
}

void criaThreadsEstudantes(pthread_t *estudantes, pthread_attr_t *attrEstudantes, int *idEstudantes, int quantidade){

    for (int i = 0 ; i < quantidade; i++){
        idEstudantes[i] = i;
        pthread_attr_init(&attrEstudantes[i]);
        pthread_create(&estudantes[i], &attrEstudantes[i], estudante, &idEstudantes[i]); 
        printf("Estudante %d criado\n", i);   
    }

    for(int i = 0; i <quantidade; i++){ //espera a execução das threads
        pthread_join(estudantes[i], NULL);
    }

}

void *estudante (void *idEstudante){
    int *id = (int*) idEstudante;
    int contador=0; //contador de quantas vezes ajuda é pedido

    while(contador < 3){
        int x = (rand()%11)+1;
        printf("Oi eu sou o estudante %d e vou programar por %d segundos\n",*id, x );
        sleep(x);
        if ( pedeAjuda(*id) == 1){
            contador ++;
        }
        else{
            printf("Sou o estudante %d e nao fui atendido\n", *id);
        }
    }
    
}  

int pedeAjuda (int idEstudante){

    if (cadeirasOcupadas < numCadeiras){

        pthread_mutex_lock(&cadeira_mutex);
        cadeirasOcupadas ++;
        pthread_mutex_unlock(&cadeira_mutex);
        printf("numero de cadeiras ocupadas %d\n", cadeirasOcupadas);
        sem_wait(&cadeiraAtendimento);
        estudanteAjudado = idEstudante;
        acordaMonitor();
        sendoAjudado();
        printf("Eu sou o estudante %d e vou ser ajudado agora\n", idEstudante);
        sem_post(&cadeiraAtendimento);
        pthread_mutex_lock(&cadeira_mutex);
        cadeirasOcupadas--;
        pthread_mutex_unlock(&cadeira_mutex);

        return 1;

    }
    else{
        return 0;
    }
}

void *monitor(void *param){

        int temp = 0;

        while(1){
            if(cadeirasOcupadas == 0){ // se nao houver ngm para ser atendido ele dorme
                printf("Sou o monitor e nao ha ningem para ser atendido, vou dormir\n");
                sem_wait(&monitorDormindo);
            }
            else{
                temp = (rand() % 10 )+ 1; // tempo aleatorio de atendimento
                printf("Sou o assistente e vou ajudar o estudante %d por %d segundos\n", estudanteAjudado, temp);
                sleep(temp);
                printf("Sou o assistente e terminei de ajudar o estudante %d\n", estudanteAjudado);
                sem_post(&estudanteSendoAjudado);
            }
        }
}

void acordaMonitor(void){
	sem_post(&monitorDormindo);	
}

void sendoAjudado(void){
	sem_wait(&estudanteSendoAjudado);
}
