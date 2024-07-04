#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX_ITEMS 100
//waiter.c
#define MAX_CUSTOMERS 5
#define MAX_ORDERS 50
void Menu(int costs[]){

    FILE *file = fopen("menu.txt", "r");
    
    
    if (file == NULL) {
        printf("Error opening file!\n");
        
    }
    
    
    char items[MAX_ITEMS][100]; 
    
    
    
    int item_count = 1; 
    while (fscanf(file, "%*d. %99[^0-9] %d INR\n", items[item_count], &costs[item_count]) == 2) {
        
        item_count++;
    }
   
    fclose(file);
}
int main(){
    int totalTables;
    int size = 300;
    int shmId2;
    int *shmPtr2;
    int shmflagId;
    int *shmflagPtr;
    int shmId3;
    int *shmPtr3;
    int waiterId;
    int bill = 0;
    int stas;
    int arraySize;
    int costs[MAX_ITEMS];
    key_t key2 = ftok("waiter.c",2);
    key_t key3 = ftok("table.c",3);

    shmflagId = shmget(key2,size,IPC_CREAT|0666);
    if(shmflagId==-1){
        perror("Error in creating Shared Memory Id");
        exit(1);
    }
    
    shmflagPtr = shmat(shmflagId,NULL,0);
    if(*shmflagPtr == -1){
        perror("Error in attaching to Shared Memory");
        exit(1);
    }
    
    //*shmflagPtr == 0 not attached to waiter, 1 attached to waiter;
    //shmflagPtr[1] = waiterId;
    //shmflagPtr[2] = totalTables
    //shmflagPtr[3] = admin instruction 0 == open , 1 == close

    printf("Enter Waiter ID: ");
    scanf("%d",&waiterId);

    shmflagPtr[1]=waiterId;

    shmId2 = shmget(key2+waiterId,size,IPC_CREAT|0666);
    if(shmId2==-1){
        perror("Error in creating Shared Memory Id");
        exit(1);
    }
    
    shmPtr2 = shmat(shmId2,NULL,0);
    if(*shmPtr2 == -1){
        perror("Error in attaching to Shared Memory");
        exit(1);
    }
    
    //shmPtr2[0]= table status == 0 is table occupied and 1 is table empty;
    //shmPtr2[1]= table bill
    
    shmId3 = shmget(key3+waiterId,size,IPC_CREAT|0666);
    if(shmId3==-1){
        perror("Error in creating Shared Memory Id");
        exit(1);
    }
    
    shmPtr3 = shmat(shmId3,NULL,0);
    if(*shmPtr3 == -1){
        perror("Error in attaching to Shared Memory");
        exit(1);
    }
    
    //shmPtr3[0] = flag 0 = waiting for table , 1 = waiting for waiter;
    //shmPtr3[1] = orders;
    //shmPtr3[2] = table status == 0 is table occupied and 1 is table empty; 
    //shmPtr3[3] = total bill
    //shmPtr3[4] = order status 0=bad , 1 = good;
    //shmPtr3[5] = admin instructions 0=open , 1 =close;
    //shmPtr3[6] = order count;
    shmPtr3[0]=0;
    *shmflagPtr=0;
    shmflagPtr[4]=0;
    while(1){
        bill=0;
        shmPtr3[5]=shmflagPtr[3];
    while(1){
        if(shmPtr3[0]==1){
            break;
        }
        else{
            sleep(1);
        }
    }
    Menu(costs);
    bill=0;
    for(int i=7;i<shmPtr3[6]+7;i++){
        if(costs[shmPtr3[i]]==0){
            shmPtr3[1]=0;
            bill=0;
            break;
        }
        else{
            shmPtr3[1]=1;
            bill+=costs[shmPtr3[i]];


        }

    }
    shmPtr3[5]=shmflagPtr[3];
    


    if(shmPtr3[1]==1){
        printf("Valid Order\n");
        shmPtr3[4]=1;
        shmPtr3[5]=shmflagPtr[3];
        
        shmPtr3[3] = bill;
        shmflagPtr[4]=0;
        shmPtr2[1] = bill;
        printf("Bill Amount for table %d: %d\n",waiterId,shmPtr3[3]);
        shmPtr3[0]=0;
        while(1){
            if(shmPtr3[0]==1){
                bill=0;
                break;
            }
            else{
                bill=0;
                sleep(1);
            }
        }
        
        if(shmPtr3[2]==1){
            
            shmPtr2[0]=shmPtr3[2];
            *shmflagPtr=1;
            bill=0;
            break;
        }
        else{
            bill=0;
        *shmflagPtr=1;
        shmPtr2[1]=0;
        
        }
        

        
    }
    else{
        printf("Invalid Order\n");
        bill=0;
        shmPtr3[5]=shmflagPtr[3];
        
        shmPtr3[0]=0;
        shmPtr3[4]=0;
        continue;

    }
    }
    bill=0;
    shmflagPtr[4]=1;
    
    



    
    if(shmdt(shmPtr2)==-1){
        perror("Error in Detaching from Shared Memory\n");
        exit(1);
    }
    if(shmdt(shmflagPtr)==-1){
        perror("Error in Detaching from Shared Memory\n");
        exit(1);
    }
    return 0;

}