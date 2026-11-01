#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>


int main() {
    int p1, p2, p3;
    

  
    p1 = fork();
    if (p1 == 0) { 
        
        execlp("gcc", "gcc", "-c", "prog1.c", "-o", "prog1.o", NULL);
        printf("Error in process 1\n");
        exit(1);
    }
    
    wait(NULL); 
    printf("Process 1 finished file has been compiled\n");

  
    p2 = fork();
    if (p2 == 0) { 
        execlp("gcc", "gcc", "prog1.o", "-o", "prog1", NULL);
        printf("Error in process 2\n");
        exit(1);
    }
    
    wait(NULL); 
    printf("Process 2 finished link editing completed\n");

    p3 = fork();
    if (p3 == 0) { 
        execlp("./prog1", "./prog1", NULL);
        printf("Error in process 3\n");
        exit(1);

    }

    int status;
    wait(&status);

    if (WIFEXITED(status)) {
    int code = WEXITSTATUS(status);
    if (code == 0) {
        printf("Normal termination without error\n");
    } else {
        printf("Normal termination with error (code: %d)\n", code);
    }
} else if (WIFSIGNALED(status)) {
    printf("Abnormal termination by signal/exception (signal: %d)\n", WTERMSIG(status));
}
}
