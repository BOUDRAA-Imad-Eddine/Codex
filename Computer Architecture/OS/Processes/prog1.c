#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    
   /* printf("prog1: Running successfully.\n"); //normal termination(no error)
    return 0; // Success (Exit code 0)
     */
    
    printf("prog1: normal termination with error.\n"); // normal termination(with error)
    return 1; // Error (Exit code 1)
    
    
    /* printf("prog1: Abnormal termination\n"); // abnormal termination (crash)
    int *ptr = NULL;
    *ptr = 7; // Forces the OS to kill the process with a signal (SIGSEGV)
    return 0;*/
    

    
}
