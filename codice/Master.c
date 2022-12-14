#include "header.h"

int main() {
    struct struct_merce *vettore_di_merci;
    struct struct_merce *indirizzo_attachment_merce;
    struct struct_porto *indirizzo_attachment_porto;
    int message_queue_id;
    int shared_memory_id_porto;
    int navi;
    int porti;
    int i;
    int j;
    int fd;
    char **args;
    pid_t pid_processi;
    sem_t *semaforo_master;
    int *value;
    j = 0;
    srand(time(NULL));
    

    semaforo_master = sem_open(semaforo_nome, O_CREAT, 0644, 1);

    indirizzo_attachment_porto = NULL;
    shared_memory_id_porto = memoria_condivisa_creazione(SHM_KEY_PORTO, sizeof(struct struct_porto));
    indirizzo_attachment_porto = (struct struct_porto*)shmat(shared_memory_id_porto, NULL, 0);

    vettore_di_merci = generatore_array_merci();

    printf("\n\n\n");

    /*creazione processi porto*/
    for(i = 0; i < NO_PORTI; i++){  
        sem_wait(semaforo_master);
        switch (pid_processi = fork()){
            case -1:
                fprintf(stderr, "Errore nella fork() del Porto");
                exit(EXIT_FAILURE);
                break;    
            case 0:
                execvp("./Porti", args);
                exit(EXIT_SUCCESS);
                break;            
            default:
                /*inserisco l'array di strutture contenente la merce nella memoria condivisa*/
                indirizzo_attachment_merce = NULL;
                shared_memory_id_merce = memoria_condivisa_creazione(SHM_KEY_MERCE, sizeof(struct struct_merce)*NUMERO_TOTALE_MERCI);
                indirizzo_attachment_merce = (struct struct_merce*)shmat(shared_memory_id_merce, NULL, 0);
                indirizzo_attachment_merce[0] = vettore_di_merci[j];
                j++;
                /*waitpid(pid_processi, NULL, WUNTRACED);*/
                break;
        } 
    } j = 0;

    /*creazione processi nave*/
    for(i = 0; i < NO_NAVI; i++){
        sem_wait(semaforo_master);
        switch (pid_processi = fork()){
            case -1:
                fprintf(stderr, "Errore nella fork() della Nave");
                exit(EXIT_FAILURE);
                break;    
            case 0:
                execvp("./Navi", args);
                exit(EXIT_SUCCESS);
                break;            
            default:
                /*inserisco l'array di strutture contenente la merce nella memoria condivisa*/
                indirizzo_attachment_merce = NULL;
                shared_memory_id_merce = memoria_condivisa_creazione(SHM_KEY_MERCE, sizeof(struct struct_merce)*NUMERO_TOTALE_MERCI);
                indirizzo_attachment_merce = (struct struct_merce*)shmat(shared_memory_id_merce, NULL, 0);
                indirizzo_attachment_merce[0] = vettore_di_merci[j];
                j++;
                /*waitpid(pid_processi, NULL, WUNTRACED);*/
                break;
        }
    }

    sem_unlink(semaforo_nome);
    sem_close(semaforo_master);
    memoria_condivisa_deallocazione(shared_memory_id_porto);
    memoria_condivisa_deallocazione(shared_memory_id_merce);
    return 0;
}                                                       