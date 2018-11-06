#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int randomizaNumEstudantes();


int main (){

    srand(time(NULL));
    pthread_t *estudantes;
    pthread_attr_t *attrEstudantes;
    int *idEstudantes;

    pthread_t monitor;
    pthread_attr_t attrMonitor;

    int quantidade = randomizaNumEstudantes;

    estudantes = (pthread_t *) malloc(sizeof(pthread_t)*quantidade); // cria um ponteiro com n estudantes
    attrEstudantes = (pthread_attr_t *) malloc (sizeof (pthread_attr_t)*quantidade); 
    idEstudantes = (int*) malloc(sizeof(int)*quantidade);

    


}

int randomizaNumEstudantes()
{
    int quantidade;
    quantidade = (rand()%(37)) + 3;
    return quantidade;
}

void criaThreadsEstudantes(pthread_t *estudantes, pthread_attr_t *attrEstudantes, int *idEstudantes, int quantidade){

    for (int i = 0 ; i < quantidade; i++){
        idEstudantes[i] = i;
        pthread_attr_init(&attrEstudantes[i]);
        pthread_create(&estudantes[i], &attrEstudantes[i], estudante, &idEstudantes[i]);    
    }

}

void estudante (){
    
}