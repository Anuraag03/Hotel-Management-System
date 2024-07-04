#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

//admin.c

int main(){
    char userInput;
    int size = 300;
    int shmId;
    int *shmPtr;
    key_t key = ftok("hotelmanager.c",1);
    shmId = shmget(key,size,IPC_CREAT|0666);
    if(shmId==-1){
        perror("Error in creating Shared Memory Id\n");
        exit(1);
    }
    
    shmPtr = shmat(shmId,NULL,0);
    if(*shmPtr == -1){
        perror("Error in attaching to Shared Memory\n");
        exit(1);
    }
    
    *shmPtr=0;
    //shmPtr[0]=0;

    
    while(1){ //keep taking input from user until 'Y' is recieved
        
        printf("Do you want to close the hotel? Enter Y for Yes and N for No : ");
        scanf(" %c",&userInput);
        
        if(userInput=='Y'||userInput=='y'){
            *shmPtr = 1;
            //shmPtr[0]=1;
            break;
        }
        else if(userInput=='N'||userInput=='n'){
            *shmPtr = 0;
            continue;
        }
        else{
            printf("Invalid Input: Enter 'Y' or 'N'. \n");
        }

    }
    

    if(shmdt(shmPtr)==-1){
        perror("Error in Detaching from Shared Memory\n");
        exit(1);
    }
    printf("Terminated\n");
    return 0;
}