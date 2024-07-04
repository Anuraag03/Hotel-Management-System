#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

//hotelmanager.c

int main(){
    int totalTables;
    int size = 300;
    int shmId;
    int *shmPtr;
    int shmId2;
    int *shmPtr2;
    int shmflagId;
    int *shmflagPtr;
    int totalEarnings = 0;
    float waiterWages = 0;
    float percentage = 0.4;
    float totalProfit =0;
    key_t key = ftok("hotelmanager.c",1);
    key_t key2 = ftok("waiter.c",2);
    
    while(1){
        printf("Enter the Total Number of Tables at the Hotel: ");
        scanf("%d",&totalTables);
        if(totalTables<=10 && totalTables>=1){
            break;
        }
        else{
            printf("Invalid Input : Enter a number between 1-10 \n");
        }

    }
    
    shmId = shmget(key,size,IPC_CREAT|0666);
    if(shmId==-1){
        perror("Error in creating Shared Memory Id");
        exit(1);
    }
    
    shmPtr = shmat(shmId,NULL,0);
    if(*shmPtr == -1){
        perror("Error in attaching to Shared Memory");
        exit(1);
    }
    
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
    // shmflagPtr[4] = table status;
    *shmflagPtr =0;
    shmflagPtr[2]=totalTables;
    printf("Total Tables : %d\n",shmflagPtr[2]);
    int value;
    int sts;
    
    shmflagPtr[3]=*shmPtr; //admin instructions change to = *shmPtr if needed
    

    

    while(1){
        
    
        if(*shmPtr==1){


            break;
        }
        while(1){
            
            if(*shmPtr==1){

                shmflagPtr[3]=*shmPtr;
                break;
            }
            if(shmflagPtr[0]==1){
                break;

            }
            else{
                sleep(1);
            }
        }
        if(*shmPtr==1){
            shmflagPtr[3]=*shmPtr;
            break;
        }
        value = shmflagPtr[1];
        
        shmId2 = shmget(key2+value,size,IPC_CREAT|0666);
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
        

        
        if(*shmPtr==1){
            shmflagPtr[3]=*shmPtr;
            break;
        }
        

    // Write earnings to the file for each table
        if(shmPtr2[1]!=0){
            FILE *fp = fopen("earnings.txt", "a");
        if (fp == NULL) {
            printf("Error opening file.\n");
            return 1;
        }
        fprintf(fp, "Earning from Table %d: %d INR\n",value, shmPtr2[1]);
        printf("Earning from table %d: %d INR\n",value,shmPtr2[1]);
        fclose(fp);
        }

//close the file
        
        totalEarnings+=shmPtr2[1];
        *shmflagPtr=0;
        if(*shmPtr==1){
            shmflagPtr[3]=*shmPtr;
            break;
        }





        




    }
    
    shmflagPtr[3]=1; //admin instructions change to = *shmPtr if needed;
    
    fflush(stdout);

    while(1){
        while(1){
            if(shmflagPtr[4]==1){
                break;

            }
            
            if(shmflagPtr[0]==1){
                break;

            }
            else{
                sleep(1);
            }
        }
        
        value = shmflagPtr[1];
        
        shmId2 = shmget(key2+value,size,IPC_CREAT|0666);
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
        //shmPtr2[3] = admin 

        if(shmPtr2[1]!=0){
            FILE *fp = fopen("earnings.txt", "a");
        if (fp == NULL) {
            printf("Error opening file.\n");
            return 1;
        }
        fprintf(fp, "Earning from Table %d: %d INR\n",value, shmPtr2[1]);
        printf("Earning from table %d: %d INR\n",value,shmPtr2[1]);
        fclose(fp);
        }

//close the file
        
        totalEarnings+=shmPtr2[1];
        
        if(shmPtr2[0]==1){
            break;
        }
        
    }
    if(shmdt(shmPtr)==-1){
        perror("Error in Detaching from Shared Memory\n");
        exit(1);
    }
    if(shmdt(shmflagPtr)==-1){
        perror("Error in Detaching from Shared Memory\n");
        exit(1);
    }
    FILE *fp = fopen("earnings.txt", "a");
        if (fp == NULL) {
            printf("Error opening file.\n");
            return 1;
        }

   


    printf("Total Earnings of Hotel: %d INR\n",totalEarnings);
    fprintf(fp,"Total Earnings of Hotel: %d INR\n",totalEarnings);
    waiterWages = (float)totalEarnings * percentage;

    printf("Total Wages of Waiters: %.2f INR\n",waiterWages);
    fprintf(fp,"Total Wages of Waiters: %.2f INR\n",waiterWages);
    totalProfit = (float)totalEarnings - waiterWages;
    printf("Total Profit : %.2f INR\n",totalProfit);
    fprintf(fp,"Total Profit : %.2f INR\n",totalProfit);
    totalEarnings=0;
    totalProfit=0;
    waiterWages=0;

    

    fprintf(fp,"Thank you for visiting the Hotel!\n");
    printf("Thank you for visiting the Hotel!\n");
    fclose(fp);




    return 0;
}