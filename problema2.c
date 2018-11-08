#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

int randomizaNumEstudantes();
void *estudante (void *param);
void *monitor(void *param);
void criaThreadsEstudantes(pthread_t *estudantes, pthread_attr_t *attrEstudantes, int *idEstudantes, int quantidade);
int numero_estudantes_atendidos = 0;
int cad_ocup = 0;
int num_cadeiras;
int cadeira_monitor;
int quantidade=0;

pthread_t monitor_tid; 
pthread_attr_t attrMonitor;

sem_t cadeiraAtendimento, soneca_monitor, ajudando_estudante;
pthread_mutex_t cad_mutex, estudante_mutex; 

void acorda_monitor(void){
	sem_post(&soneca_monitor);	
}

void ajuda_estudante(void){
	sem_wait(&ajudando_estudante);
}


 
int main (){

    srand(time(NULL));
    sem_init(&cadeiraAtendimento, 0, 1);
	sem_init(&soneca_monitor, 0, 1);
	sem_init(&ajudando_estudante, 0, 1);
	pthread_mutex_init(&cad_mutex, NULL);
	pthread_mutex_init(&estudante_mutex, NULL);

	sem_wait(&soneca_monitor); //faz o monitor tirar uma soneca
	sem_wait(&ajudando_estudante); //faz o estudante esperar

    int i, *idEstudantes;
    
    pthread_attr_init(&attrMonitor);
	pthread_create(&monitor_tid,&attrMonitor, monitor, NULL);
    quantidade = randomizaNumEstudantes(); //randomiza a quantidade de estudantes
    printf("Quantidade de estudantes %d\n", quantidade);
    num_cadeiras = quantidade/2;  //numero de cadeiras dividido por 2
    printf("Numero de cadeiras é %d\n", num_cadeiras);

    pthread_t *estudantes;
    pthread_attr_t *attrEstudantes;
    
    estudantes = (pthread_t *) malloc(sizeof(pthread_t)*quantidade); // cria um ponteiro com n estudantes
    attrEstudantes = (pthread_attr_t *) malloc (sizeof (pthread_attr_t)*quantidade); 
    idEstudantes = (int*) malloc(sizeof(int)*quantidade);
    criaThreadsEstudantes(estudantes, attrEstudantes, idEstudantes, quantidade); //cria as threads de estudante
    pthread_join(monitor_tid, NULL);


	free(estudantes);
	free(attrEstudantes);
	free(idEstudantes);

}

int randomizaNumEstudantes()
{
    int quantidade;
    quantidade = (rand()%(38)) + 3;
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

void *vai_programar(int idEstudantes){
    int tempo_prog = rand() % 100000 + 100000;
    printf("Sou o estudante %d. Vou programar por %d tempo!\n", idEstudantes, tempo_prog);
    usleep(tempo_prog);
}

int pede_ajuda(int idEstudantes){
    if(cad_ocup < num_cadeiras){
        pthread_mutex_lock(&cad_mutex);
        cad_ocup ++;
        printf("numero de cadeiras ocupadas %d\n", cad_ocup);
        pthread_mutex_unlock(&cad_mutex);
        sem_wait(&cadeiraAtendimento);
		cadeira_monitor = idEstudantes;
		printf("O estudante %d vai ser Atendido.\n", idEstudantes);
        //sem_post(&soneca_monitor); //implemtado na função acorda_monitor();
        //sem_wait(&ajudando_estudante);
		acorda_monitor();
		ajuda_estudante();
		cadeira_monitor = -1;
        //printf("Acorda monitor!\n");
		//printf("O estudante %d foi Atendido.\n", idEstudantes);
		sem_post(&cadeiraAtendimento);
		pthread_mutex_lock(&cad_mutex);
		cad_ocup--;
		pthread_mutex_unlock(&cad_mutex);
		return 1;
	}
	else{
		return 0;
    }
}

void *estudante (void *param){
    int *estudante_num = (int *) param;
    int cont_ajuda = 0;

    printf("Sou o estudante número %d. \n", *estudante_num);

    while(cont_ajuda <3){

        vai_programar(*estudante_num);
        if(pede_ajuda(*estudante_num) == 1){
            cont_ajuda ++;
            printf("Sou o estudante num. %d. Fui atendido %d vezes.\n", *estudante_num, cont_ajuda);
        }
        else{
            printf("Sou o estudante num. %d. Volto depois.\n", *estudante_num);
        }
    }    
    numero_estudantes_atendidos++;
    
    if(numero_estudantes_atendidos == quantidade){
        printf("\n\nTodos os estudantes foram atendidos por 3 vezes!\n");
        printf("Total de estudantes Atendidos %d!\n", numero_estudantes_atendidos);
        pthread_cancel(monitor_tid);
    }
    pthread_exit(0);
}

void *monitor(void *param){

	while(1){
		if(cad_ocup == 0){ // se nao houver ngm para ser atendido ele dorme
        printf("O monitor vai tirar uma soneca!.\n");
		sem_wait(&soneca_monitor);
        }
        else{
        int ajuda_temp = rand() % 100000 + 100000; // tempo aleatorio de atendimento
		//printf("O monitor vair atender o estudante num %d.\n",cadeira_monitor);

		usleep(ajuda_temp);
		printf("O estudante %d foi atendido por %d tempo.\n",cadeira_monitor, ajuda_temp);
		sem_post(&ajudando_estudante);
        }
	}
	pthread_exit(0);
}