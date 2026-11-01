#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <stdlib.h>

int main() {
    int p;
    p = fork();                        //P1 created
    if (p == 0){
        p= fork();                    //P1.1 created
        if (p == 0){                  // it's P1.1
            printf(getpid(),getppid());
            return 0;
        }
        else { // it's P1
            p = fork(); //P1.2 created
            while(wait(null)!=-1);
            printf(getpid(),getppid());
            return 0;
            
        
        }
    }
    // it's P0
    p = fork(); //P2 created
    if (p == 0){ // it's P2
            printf(getpid(),getppid());
            return 0;
        }
    p= fork(); //P3 created
    if (p == 0){ //it's P3
            p = fork(); //P3.1 created
            if (p == 0){ // it's P3.1
                printf(getpid(),getppid());
                return 0;
            }
            // it's P3
            p = fork(); //P3.2 created
            if (p == 0){// it's P3.2
                p = fork(); //P3.2.1 created
                if (p == 0){ // it's P3.2.1
                    printf(getpid(),getppid());
                    return 0;
                }
                printf(getpid(),getppid());
                return 0;
                }
                printf(getpid(),getppid());
                return 0;
                
            
    }
            while(wait(null)!=-1);
            printf(getpid(),getppid());
            return 0;

    

}