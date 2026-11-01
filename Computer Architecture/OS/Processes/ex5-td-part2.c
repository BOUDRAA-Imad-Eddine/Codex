#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>

int main(){
    p2 = fork();
    if (p==0){
        p3 = fork();
        if (p == 0){
            p4 = fork();
            if (p==0){
                printf("it's p4",getpid(),getppid());

 
            }
            int status;
            waitpid(p4,&status,0);
            if (WIFEXITED(status) && WEXITSTATUS(status)==0) {
                printf("p4 terminated with sucess");
            }
            else{
                exit(0)
                printf("p4 error");
            }
            
            printf("it's p3",getpid(),getppid());
            return 0;
        }
        int status;
        waitpid(p3,&status,0);
        if (WIFEXITED(status) && WEXITSTATUS(status)==0) {
            printf("p3 terminated with sucess");
        }
        else{
            printf("p3 error");
        }

        printf("it's p2",getpid(),getppid());
        return 0;
    }
    p5 = fork()
    if (p==0){
        execlp("./prog2","./prog2", NULL)
    }
    
    while(wait(null)!=-1);
    printf(getpid(),getppid());
    return 0;
}