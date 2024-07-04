#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#define MAX_ITEMS 100
#define MAX_CUSTOMERS 5
#define MAX_ORDERS 50
void Menu(){

    FILE *file = fopen("menu.txt", "r");
    
    
    if (file == NULL) {
        printf("Error opening file!\n");
        
    }
    
    
    char items[MAX_ITEMS][100]; 
    int costs[MAX_ITEMS];
    
    
    printf("Menu:\n");
    int item_count = 1; 
    while (fscanf(file, "%*d. %99[^0-9] %d INR\n", items[item_count], &costs[item_count]) == 2) {
        printf("%d. %s %d INR\n", item_count, items[item_count], costs[item_count]);
        item_count++;
    }
    
   
    fclose(file);
}

int main(){
    int tableId;
    int shmId3;
    int *shmPtr3;
    int reorder;
    int numCustomers=0;
    key_t key3 = ftok("table.c",3);
    int size = 300;
    printf("Enter Table Id : ");
    scanf("%d",&tableId);
    
    shmId3 = shmget(key3+tableId,size,IPC_CREAT|0666);
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
    //shmPtr3[6] = order count
    while(1){
    int tableNum, i, status, pipefd[2];
    int orders[MAX_ORDERS] = {0};
    int orderCount = 0;

    // Get number of customers
    if(numCustomers==0){
    printf("Enter Number of Customers at Table (maximum no. of customers can be 5): ");
    scanf("%d", &numCustomers);
    }

    
    Menu();

    
    for (i = 0; i < numCustomers; i++) {
        pipe(pipefd); 

        if (fork() == 0) { 
            close(pipefd[0]); 

            
            printf("Customer %d, enter the serial number(s) of the item(s) to order from the menu. Enter -1 when done:\n", i + 1);
            int item;
            while (scanf("%d", &item) == 1 && item != -1) {
                write(pipefd[1], &item, sizeof(item));
            }

            close(pipefd[1]);
            exit(0);
        }

       
        close(pipefd[1]); 

        
        int order;
        while (read(pipefd[0], &order, sizeof(order)) > 0) {
            orders[orderCount++] = order;
        }

        close(pipefd[0]);
        wait(&status); 
    }

    shmPtr3[6]=orderCount;
    for (i = 0; i < orderCount; i++) {
        shmPtr3[i+7]=orders[i];
    }
    

    
    shmPtr3[0]=1;
    
    while(1){
        if(shmPtr3[0]==0){break;}
        else{
            sleep(1);
        }
    }
    if(shmPtr3[4]==1){
        printf("Valid Order\n");
        printf("The total bill amount is %d INR.\n",shmPtr3[3]);
        if(shmPtr3[5]==0){

        printf("Enter Number of Customers at Table (maximum no. of customers can be 5): ");
        scanf("%d",&numCustomers);
        if(numCustomers>0){
           
            shmPtr3[2]=0;
            shmPtr3[0]=1;
            continue;
        }
        else{
            
            
            break;
        }
        }
        else{

            
            break;
        }
        
    }
    else{
        
        printf("Invalid order please order again.\n ");
        numCustomers=0;
        shmPtr3[2]=0;
        continue;
    }
    }
    
    shmPtr3[2]=1;
    
    shmPtr3[0]=1;
    /*
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }
*/


    

    return 0;
}