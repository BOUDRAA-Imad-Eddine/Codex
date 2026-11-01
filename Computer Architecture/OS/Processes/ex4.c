#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>




int main( )
{
/*int err;
err=system("cc -o prog prog.c");
*/

p1 = fork();
if (p1 == 0) {
    execlp("gcc","gcc","-o","prog","prog.c",NULL);
    print("\n Object module not created");
    exit(1);
}
wait(null);
print("\n Object module created");

/*if(err!=0){ printf("\n Object module not created : err : %d", err>>8);
exit(1);
}*/

p2= fork();
if (p2 == 0) {
    execlp("./prog", "./prog", NULL);
    print("\n Execution error");
    exit(2);
}


/* err=system("./prog");
if(err!=0){ printf("\n Execution error : %d", err>>8);
exit(2);
} */

printf("\n *** process = %d; *** Father process %d \n", getpid(), getppid());
while(wait(NULL):-1); /* Wait for children to terminate */
return 0;
}